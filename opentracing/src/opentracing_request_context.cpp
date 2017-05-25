#include "opentracing_request_context.h"
#include "varnishext.h"
#include <cinttypes>
#include <cassert>
#include <iostream>

//------------------------------------------------------------------------------
// finalize_opentracing_request_context
//------------------------------------------------------------------------------
void finalize_opentracing_request_context(void* context) {
  auto tracing_context = static_cast<OpenTracingRequestContext*>(context);
  if (tracing_context->req) {
    // How do I pull information out of this object?
  }
  tracing_context->span.Finish();
  delete tracing_context;
}
