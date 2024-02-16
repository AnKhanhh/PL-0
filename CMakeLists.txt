cmake_minimum_required(VERSION 3.23)
project(compilers C)

set(CMAKE_C_STANDARD 11)
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)

add_executable(compilers ${SOURCE_DIR}/main.c)
target_compile_options(compilers PRIVATE -Wall -Wextra -pedantic)

# disable assert for release
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(compilers PRIVATE NDEBUG)
    target_compile_options(compilers PRIVATE -O3)
endif()

# not published to git
add_executable(test test.c)
