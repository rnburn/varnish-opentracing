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
  std::string operation_name;
};

//------------------------------------------------------------------------------
// get_opentracing_request_context
//------------------------------------------------------------------------------
OpenTracingRequestContext& get_opentracing_request_context(vmod_priv* priv);

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
