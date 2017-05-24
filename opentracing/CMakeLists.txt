find_library(VARNISHAPI_LIBRARY varnishapi)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)
declare_vmod(
  INSTALL
  NAME opentracing
  VCC ${CMAKE_CURRENT_SOURCE_DIR}/opentracing.vcc
  SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/trace.cpp
          ${CMAKE_CURRENT_SOURCE_DIR}/src/extract_span_context.cpp
          ${CMAKE_CURRENT_SOURCE_DIR}/src/inject_span_context.cpp
          ${CMAKE_CURRENT_SOURCE_DIR}/src/opentracing_request_context.cpp
  ADDITIONNAL_LIBRARIES ${VARNISHAPI_LIBRARY}
    ${LIGHTSTEP_CORE_CXX11_LIB}
)
