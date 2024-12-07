cmake_minimum_required(VERSION 3.30)
project(AoC)

set(CMAKE_CXX_STANDARD 20)

# Include "src" and "lib"
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/lib)

find_package(fmt CONFIG REQUIRED)
# Specify language as CXX to avoid error (header only library)
add_library(color_tools STATIC lib/color_tools.h lib/color_tools.cpp)
add_library(array_tools STATIC lib/arr_util.cpp lib/arr_util.dec.h lib/arr_util.h)
target_link_libraries(color_tools PRIVATE fmt::fmt)
target_link_libraries(array_tools PRIVATE fmt::fmt)

add_executable(AoC1 src/1/main.cpp src/1/main.h)
target_link_libraries(AoC1 PRIVATE fmt::fmt color_tools array_tools)