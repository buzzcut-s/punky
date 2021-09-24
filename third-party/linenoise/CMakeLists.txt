add_library(linenoise INTERFACE)

target_sources(
  linenoise
  INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/linenoise.hpp>)

target_include_directories(
  linenoise INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
