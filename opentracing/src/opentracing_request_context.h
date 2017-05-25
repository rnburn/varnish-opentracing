#pragma once

#include "varnish.h"
#include <lightstep/tracer.h>

//------------------------------------------------------------------------------
// OpenTracingRequestContext
//------------------------------------------------------------------------------
struct OpenTracingRequestContext {
  lightstep::Span span;
  const struct req* req = nullptr;
};

//------------------------------------------------------------------------------
// finalize_opentracing_request_context
//------------------------------------------------------------------------------
void finalize_opentracing_request_context(void* context);
