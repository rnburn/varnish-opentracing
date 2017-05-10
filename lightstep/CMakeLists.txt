configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)
declare_vmod(
  INSTALL
  NAME lightstep
  VCC ${CMAKE_CURRENT_SOURCE_DIR}/lightstep.vcc
  SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/lightstep.cpp
)
