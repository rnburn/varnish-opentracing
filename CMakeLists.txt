cmake_minimum_required(VERSION 3.0)

project(varnish-opentracing)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS "${CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS} -undefined dynamic_lookup")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS "${CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS} --unresolved-symbols=ignore-in-object-files")
endif()
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})

find_package(VarnishAPI REQUIRED)

find_path(LIGHTSTEP_INCLUDE_DIR NAMES lightstep/tracer.h)
find_library(LIGHTSTEP_CORE_CXX11_LIB NAMES lightstep_core_cxx11)
find_library(LIGHTSTEP_GRPC_CXX11_LIB NAMES lightstep_grpc_cxx11)
find_library(GRPCPP_LIB NAMES grpc++)
find_library(GRPC_LIB NAMES grpc)
find_library(PROTOBUF_LIB NAMES protobuf)
set(LIGHTSTEP_LIBRARIES ${LIGHTSTEP_CORE_CXX11_LIB}
                        ${LIGHTSTEP_GRPC_CXX11_LIB}
                        ${GRPCPP_LIB}
                        ${GRPC_LIB}
                        ${PROTOBUF_LIB})

include_directories(SYSTEM ${LIGHTSTEP_INCLUDE_DIR})

add_subdirectory(opentracing)
add_subdirectory(lightstep)
