cmake_minimum_required(VERSION 3.0)

project(varnish-opentracing)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})

find_package(VarnishAPI REQUIRED)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)
declare_vmod(
  INSTAL
  NAME opentracing
  VCC ${PROJECT_SOURCE_DIR}/opentracing.vcc
  SOURCES ${PROJECT_SOURCE_DIR}/module/src/init_tracer.cpp
)
