#include <iostream>

extern "C" {
#include <varnish/vdef.h>
#include <varnish/vcl.h>
#include <varnish/vrt.h>
#include <varnish/vrt_obj.h>


VCL_VOID vmod_trace(VRT_CTX, struct vmod_priv* trace_context);
}

static void finish_trace(void* ctx_) {
  auto ctx = static_cast<vrt_ctx*>(ctx_);
  std::cout << "finishing request...\n";
}

VCL_VOID vmod_trace(VRT_CTX, struct vmod_priv* trace_context) {
  std::cout << "url = " << VRT_r_req_url(ctx) << "\n";
  std::cout << "tracing...\n";
  trace_context->priv = const_cast<void*>(static_cast<const void*>(ctx));
  trace_context->free = finish_trace;
}
