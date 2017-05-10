cmake_minimum_required(VERSION 3.0)

project(varnish-opentracing)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})

find_package(VarnishAPI REQUIRED)

add_subdirectory(opentracing)
add_subdirectory(lightstep)
