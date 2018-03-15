cmake_minimum_required(VERSION 2.6)

project(POLL_TMP)

set(CXX_FLAGS -g -Wall -Werror)
set(CMAKE_CXX_COMPILER "g++")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

add_executable(thread_test Thread.cpp Thread_test.cpp)
target_link_libraries(thread_test pthread)
add_executable(bf_test bf_test.cpp)
