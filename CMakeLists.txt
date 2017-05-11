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

add_subdirectory(opentracing)
add_subdirectory(lightstep)
