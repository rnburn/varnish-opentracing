#include <lightstep/tracer.h>
#include <iostream>
#include <cinttypes>
#include <cassert>
#include "opentracing_request_context.h"

extern "C" {
#include <varnish/vrt_obj.h>
}

void inject_span_context(lightstep::Tracer& tracer, const vrt_ctx* ctx,
                         gethdr_e where,
                         const lightstep::SpanContext& span_context);

//------------------------------------------------------------------------------
// vmod_trace_request
//------------------------------------------------------------------------------
extern "C" VCL_VOID vmod_trace_request(VRT_CTX,
                                       struct vmod_priv* request_priv,
                                       struct vmod_priv* top_request_priv) {
  std::cout << "tracing request ...\n";
  auto tracer = lightstep::Tracer::Global();
  auto tracing_context = new OpenTracingRequestContext{};

  tracing_context->operation_name = VRT_r_req_url(ctx);
  // Check to see if this is a request generated from an ESI. If so, specify the
  // top-level request's span as the parent if present.
  assert(!request_priv->priv && !request_priv->free);
  if (ctx->http_req != ctx->http_req_top && top_request_priv->priv) {
    auto parent_span_context =
        reinterpret_cast<OpenTracingRequestContext*>(top_request_priv->priv)
            ->span.context();
    tracing_context->span = tracer.StartSpan(
        tracing_context->operation_name,
        {lightstep::SpanReference{lightstep::ChildOfRef, parent_span_context}});
  } else {
    tracing_context->span = tracer.StartSpan(tracing_context->operation_name);
  }

  tracing_context->span.SetTag("component", "varnish");
  request_priv->priv = tracing_context;
  request_priv->free = finalize_opentracing_request_context;
  if (ctx->http_req == ctx->http_req_top) {
    assert(!top_request_priv->priv && !top_request_priv->free);
    top_request_priv->priv = tracing_context;
  }
  pass_opentracing_request_context_through_header(ctx, HDR_REQ, tracing_context);
}

//------------------------------------------------------------------------------
// vmod_trace_backend_request
//------------------------------------------------------------------------------
extern "C" VCL_VOID vmod_trace_backend_request(VRT_CTX,
                                               struct vmod_priv* request_priv) {
  std::cout << "tracing backend request ...\n";
  auto tracer = lightstep::Tracer::Global();
  auto parent_tracing_context =
    receive_opentracing_request_context_from_header(ctx, HDR_BEREQ);

  auto tracing_context = new OpenTracingRequestContext{};

  tracing_context->operation_name =
      "backend:" + parent_tracing_context->operation_name;
  if (parent_tracing_context) {
    auto parent_span_context = parent_tracing_context->span.context();
    tracing_context->span = tracer.StartSpan(
        tracing_context->operation_name,
        {lightstep::SpanReference{lightstep::ChildOfRef, parent_span_context}});
  } else {
    tracing_context->span = tracer.StartSpan(tracing_context->operation_name);
  }

  tracing_context->span.SetTag("component", "varnish");
  request_priv->priv = tracing_context;
  request_priv->free = finalize_opentracing_request_context;
  inject_span_context(tracer, ctx, HDR_BEREQ, tracing_context->span.context());
}
