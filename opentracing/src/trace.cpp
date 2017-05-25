#include "opentracing_request_context.h"
#include "varnish.h"
#include <cassert>
#include <cinttypes>
#include <iostream>
#include <lightstep/tracer.h>

lightstep::SpanContext extract_span_context(lightstep::Tracer &tracer,
                                            const vrt_ctx *ctx,
                                            gethdr_e where);

void inject_span_context(lightstep::Tracer &tracer, const vrt_ctx *ctx,
                         gethdr_e where,
                         const lightstep::SpanContext &span_context);

//------------------------------------------------------------------------------
// vmod_trace_request
//------------------------------------------------------------------------------
extern "C" VCL_VOID vmod_trace_request(VRT_CTX, struct vmod_priv *request_priv,
                                       struct vmod_priv *top_request_priv) {
  // If the request is restarted, a span will already have been created, so
  // do nothing.
  if (request_priv->priv)
    return;

  std::cout << "tracing request ...\n";
  auto tracer = lightstep::Tracer::Global();
  auto tracing_context = new OpenTracingRequestContext{};
  tracing_context->req = ctx->req;
  std::string operation_name{"request"};
  // Check to see if this is a request generated from an ESI. If so, specify the
  // top-level request's span as the parent if present.
  assert(!request_priv->priv && !request_priv->free);
  if (ctx->http_req != ctx->http_req_top && top_request_priv->priv) {
    auto parent_span_context =
        static_cast<OpenTracingRequestContext *>(top_request_priv->priv)
            ->span.context();
    tracing_context->span = tracer.StartSpan(
        operation_name,
        {lightstep::SpanReference{lightstep::ChildOfRef, parent_span_context}});
  } else {
    auto parent_span_context = extract_span_context(tracer, ctx, HDR_REQ);
    if (parent_span_context.valid())
      tracing_context->span =
          tracer.StartSpan(operation_name,
                           {lightstep::SpanReference{lightstep::ChildOfRef,
                                                     parent_span_context}});
    else
      tracing_context->span = tracer.StartSpan(operation_name);
  }

  tracing_context->span.SetTag("component", "varnish");
  request_priv->priv = tracing_context;
  request_priv->free = finalize_opentracing_request_context;
  if (ctx->http_req == ctx->http_req_top) {
    assert(!top_request_priv->priv && !top_request_priv->free);
    top_request_priv->priv = tracing_context;
  } else {
    tracing_context->span.SetTag("http.esi", true);
  }
  inject_span_context(
      tracer, ctx, HDR_REQ, tracing_context->span.context());
}

//------------------------------------------------------------------------------
// vmod_trace_backend_request
//------------------------------------------------------------------------------
extern "C" VCL_VOID vmod_trace_backend_request(VRT_CTX,
                                               struct vmod_priv *request_priv) {
  std::cout << "tracing backend request ...\n";
  std::cout << "req = " << ctx->req << "\n";
  auto tracer = lightstep::Tracer::Global();
  auto parent_span_context =
    extract_span_context(tracer, ctx, HDR_BEREQ);

  // Don't start a trace for the backend request if the client-side request
  // isn't traced.
  if (!parent_span_context.valid())
    return;

  auto tracing_context = new OpenTracingRequestContext{};

  const char *operation_name = "backend_request";
  tracing_context->span = tracer.StartSpan(
      operation_name,
      {lightstep::SpanReference{lightstep::ChildOfRef, parent_span_context}});

  tracing_context->span.SetTag("component", "varnish");
  request_priv->priv = tracing_context;
  request_priv->free = finalize_opentracing_request_context;
  inject_span_context(tracer, ctx, HDR_BEREQ, tracing_context->span.context());
}

//------------------------------------------------------------------------------
// vmod_operation_name
//------------------------------------------------------------------------------
extern "C" VCL_VOID vmod_operation_name(VRT_CTX, struct vmod_priv *request_priv,
                                        const char *operation_name_first, ...) {
  if (!request_priv->priv) {
    std::cerr << "Cannot call `operation_name` before the span is started\n";
    return;
  }
  std::string operation_name{operation_name_first};
  va_list args;
  va_start(args, operation_name_first);
  const char *operation_name_part = va_arg(args, const char *);
  while (operation_name_part != vrt_magic_string_end) {
    operation_name.append(operation_name_part);
    operation_name_part = va_arg(args, const char *);
  }
  va_end(args);
  auto tracing_context =
      static_cast<OpenTracingRequestContext *>(request_priv->priv);
  tracing_context->span.SetOperationName(operation_name);
}

//------------------------------------------------------------------------------
// vmod_tag
//------------------------------------------------------------------------------
extern "C" VCL_VOID vmod_tag(VRT_CTX, struct vmod_priv *request_priv,
                             VCL_STRING key, const char *value_first, ...) {
  if (!request_priv->priv) {
    return;
  }
  std::string value{value_first};
  va_list args;
  va_start(args, value_first);
  const char *value_part = va_arg(args, const char *);
  while (value_part != vrt_magic_string_end) {
    value.append(value_part);
    value_part = va_arg(args, const char *);
  }
  va_end(args);
  auto tracing_context =
      static_cast<OpenTracingRequestContext *>(request_priv->priv);
  tracing_context->span.SetTag(key, value);
}

//------------------------------------------------------------------------------
// vmod_status_code
//------------------------------------------------------------------------------
extern "C" VCL_VOID vmod_status_code(VRT_CTX, struct vmod_priv *request_priv,
                                     VCL_INT status_code) {
  if (!request_priv->priv) {
    return;
  }
  auto tracing_context =
      static_cast<OpenTracingRequestContext *>(request_priv->priv);
  tracing_context->span.SetTag("http.status_code",
                               static_cast<uint64_t>(status_code));
  if (status_code >= 500)
    tracing_context->span.SetTag("error", true);
  else
    tracing_context->span.SetTag("error", false);
}
