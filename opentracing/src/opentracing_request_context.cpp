#include "opentracing_request_context.h"
#include <cinttypes>
#include <cassert>

static const char* const kOpenTracingContextHeaderKey =
    "\023ot-varnish-context:";

//------------------------------------------------------------------------------
// finalize_opentracing_request_context
//------------------------------------------------------------------------------
void finalize_opentracing_request_context(void* context) {
  auto tracing_context = static_cast<OpenTracingRequestContext*>(context);
  tracing_context->span.Finish();
  delete tracing_context;
}

//------------------------------------------------------------------------------
// pass_opentracing_request_context_through_header
//------------------------------------------------------------------------------
void pass_opentracing_request_context_through_header(
    const vrt_ctx* ctx, gethdr_e where,
    OpenTracingRequestContext* tracing_context) {
  const size_t max_chars = sizeof(uintptr_t) * 2 + 1;
  char s[max_chars];
  snprintf(s, max_chars, "%" PRIxPTR,
           reinterpret_cast<uintptr_t>(tracing_context));
  gethdr_s gethdr = {where, kOpenTracingContextHeaderKey};
  VRT_SetHdr(ctx, &gethdr, s, vrt_magic_string_end);
}

//------------------------------------------------------------------------------
// receive_opentracing_request_context_from_header
//------------------------------------------------------------------------------
OpenTracingRequestContext* receive_opentracing_request_context_from_header(
    const vrt_ctx* ctx, gethdr_e where) {
  gethdr_s gethdr = {where, kOpenTracingContextHeaderKey};
  auto value = VRT_GetHdr(ctx, &gethdr);
  if (!value) return nullptr;
  uintptr_t ptr;
  if (sscanf(value, "%" SCNxPTR, &ptr))
    return reinterpret_cast<OpenTracingRequestContext*>(ptr);
  return nullptr;
}
