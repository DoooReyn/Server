cmake_minimum_required(VERSION 2.8)
project(StudentManager)

set(CMAKE_BUILD_TYPE Debug CACHE STRING "set build type to debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -Wall -Werror -g -D _DEBUG -std=c++11")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -Wall -Werror -std=c++11")



set(EXECUTABLE_OUTPUT_PATH bin)

file(GLOB_RECURSE HEADER *.h)
source_group("Header Files" FILES ${HEADER})

aux_source_directory(. SRC)

add_executable(StudentManager ${SRC} ${HEADER})

