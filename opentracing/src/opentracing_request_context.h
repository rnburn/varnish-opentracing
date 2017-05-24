#pragma once

#include <lightstep/tracer.h>

extern "C" {
#include <varnish/vrt.h>
}

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
