cmake_minimum_required(VERSION 2.6)

project(POLL_TMP)

set(CXX_FLAGS -g -Wall -Werror)
set(CMAKE_CXX_COMPILER "g++")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

add_executable(poll_svr poll_server.cpp)
add_executable(client client.cpp)
