#pragma once

#include "varnish.h"
#include <lightstep/tracer.h>

//------------------------------------------------------------------------------
// OpenTracingRequestContext
//------------------------------------------------------------------------------
struct OpenTracingRequestContext {
  lightstep::Span span;
};

//------------------------------------------------------------------------------
// finalize_opentracing_request_context
//------------------------------------------------------------------------------
void finalize_opentracing_request_context(void* context);
