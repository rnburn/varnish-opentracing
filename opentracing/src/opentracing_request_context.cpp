#include "opentracing_request_context.h"
#include <cinttypes>
#include <cassert>

//------------------------------------------------------------------------------
// finalize_opentracing_request_context
//------------------------------------------------------------------------------
void finalize_opentracing_request_context(void* context) {
  auto tracing_context = static_cast<OpenTracingRequestContext*>(context);
  tracing_context->span.Finish();
  delete tracing_context;
}
