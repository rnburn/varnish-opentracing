#include <lightstep/tracer.h>
#include <iostream>
#include "span_map.h"

extern "C" {
#include <varnish/vcl.h>
#include <varnish/vdef.h>
#include <varnish/vrt.h>
#include <varnish/vrt_obj.h>
}

void inject_span_context(lightstep::Tracer& tracer, const vrt_ctx* ctx,
                         gethdr_e where,
                         const lightstep::SpanContext& span_context);

static SpanMap& get_span_map() {
  static SpanMap span_map;
  return span_map;
}

struct TopRequestTracingContext {
  const vrt_ctx* ctx;
  struct sess* sp;
  lightstep::Span span;
};

static void finish_top_request(void* tracing_context_) {
  std::cout << "finishing request span...\n";
  auto tracing_context =
      static_cast<TopRequestTracingContext*>(tracing_context_);
  std::cout << "finish: sp = " << tracing_context->ctx->sp << "\n";
  get_span_map().erase(tracing_context->sp);
  // How do I figure out if the request resulted in an error or not? Running 
  // something like
  //      VRT_r_obj_status(tracing_context->ctx)
  // won't work because many of the varnish request's resources have already
  // been cleaned up.
  tracing_context->span.Finish();
  delete tracing_context;
}

extern "C" VCL_VOID vmod_trace_request(VRT_CTX,
                                       struct vmod_priv* top_request_context) {
  if (ctx->http_req != ctx->http_req_top) {
    std::cout << "tracing ESI requests isn't supported\n";
    return;
  }
  std::cout << "tracing request ...\n";
  std::cout << "req: sp = " << ctx->sp << "\n";
  auto tracer = lightstep::Tracer::Global();
  auto tracing_context = new TopRequestTracingContext{};
  tracing_context->span = tracer.StartSpan(VRT_r_req_url(ctx));
  tracing_context->span.SetTag("component", "varnish");
  tracing_context->ctx = ctx;
  tracing_context->sp = ctx->sp;
  top_request_context->priv = tracing_context;
  top_request_context->free = finish_top_request;
  get_span_map().insert(ctx->sp, &tracing_context->span);
}

extern "C" VCL_VOID vmod_trace_backend_request(VRT_CTX) {
  std::cout << "tracing backend request ...\n";
  std::cout << "fetch: ctx->sp = " << ctx->sp << "\n";
  auto parent_span = get_span_map().lookup_span(ctx->sp);
  if (!parent_span)
    return;
  auto tracer = lightstep::Tracer::Global();
  inject_span_context(tracer, ctx, HDR_BEREQ, parent_span->context());
}
