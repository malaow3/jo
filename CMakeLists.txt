cmake_minimum_required(VERSION 3.20)

project(jo)

# Use c++23
set(CMAKE_CXX_STANDARD 23)

include(ProcessorCount)
ProcessorCount(N)
if(NOT N EQUAL 0)
  set(CMAKE_BUILD_PARALLEL_LEVEL ${N})
endif()

set(CMAKE_UNITY_BUILD ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Export compile commands for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(fmt CONFIG REQUIRED)
find_package(CLI11 CONFIG REQUIRED)

add_executable(jo src/main.cc)

target_link_libraries(jo PRIVATE fmt::fmt)
target_link_libraries(jo PRIVATE CLI11::CLI11)

# Add address sanitizer to the executable
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_link_options(jo PRIVATE -fsanitize=address)
endif()

target_precompile_headers(jo PRIVATE <vector> <string> <fmt/core.h>)
