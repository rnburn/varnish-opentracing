#include <lightstep/tracer.h>
#include <iostream>

extern "C" {
#include <varnish/vcl.h>
#include <varnish/vdef.h>
#include <varnish/vrt.h>
#include <varnish/vrt_obj.h>
}

struct TopRequestTracingContext {
  const vrt_ctx* ctx;
  lightstep::Span span;
};

static void finish_top_request(void* tracing_context_) {
  std::cout << "finishing request span...\n";
  auto tracing_context =
      static_cast<TopRequestTracingContext*>(tracing_context_);
  // How do I figure out if the request resulted in an error or not? Running 
  // something like
  //      VRT_r_obj_status(tracing_context.ctx)
  // won't work because many of the varnish request's resources have already
  // been cleaned up.
  tracing_context->span.Finish();
  delete tracing_context;
}

extern "C" VCL_VOID vmod_trace_request(VRT_CTX,
                                       struct vmod_priv* top_request_context) {
  std::cout << "tracing request ...\n";
  auto tracer = lightstep::Tracer::Global();
  auto tracing_context = new TopRequestTracingContext{};
  tracing_context->span = tracer.StartSpan(VRT_r_req_url(ctx));
  tracing_context->span.SetTag("component", "varnish");
  tracing_context->ctx = ctx;
  top_request_context->priv = tracing_context;
  top_request_context->free = finish_top_request;
}

extern "C" VCL_VOID vmod_trace_backend_request(VRT_CTX) {
  std::cout << "tracing backend request ...\n";
  // TODO: Inject the span's context into the backend request headers so that
  // spans created from the backend can reference it.
  //    tracer.Inject(tracing_context->span.context(),
  //                  lightstep::CarrierFormat::HTTPHeaders,
  //                  <backend_request_writer>);
}
