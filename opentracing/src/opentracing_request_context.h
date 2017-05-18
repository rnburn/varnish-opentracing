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

//------------------------------------------------------------------------------
// pass_opentracing_request_context_through_header
//------------------------------------------------------------------------------
void pass_opentracing_request_context_through_header(
    const vrt_ctx* ctx, gethdr_e where,
    OpenTracingRequestContext* tracing_context);

//------------------------------------------------------------------------------
// receive_opentracing_request_context_from_header
//------------------------------------------------------------------------------
OpenTracingRequestContext* receive_opentracing_request_context_from_header(
    const vrt_ctx* ctx, gethdr_e where);
