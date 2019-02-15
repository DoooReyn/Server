cmake_minimum_required(VERSION 2.8)
project(gateserver)

#set(CMAKE_BUILD_TYPE Debug CACHE STRING "set build type to debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -Wall -Werror -g -D _DEBUG -std=c++11")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -Wall -Werror -std=c++11")



set(EXECUTABLE_OUTPUT_PATH ../bin)

include_directories(
	"../common"
	"../lib"
	"../proto/gen_proto"
	"/usr/include/mysql"
	"/usr/local/include/hiredis"
	)

link_directories(
	${PROJECT_BINARY_DIR}/bin/
	"/usr/lib/x86_64-linux-gnu"
	"/usr/local/lib"
	)

file(GLOB_RECURSE HEADER *.h)
source_group("Header Files" FILES ${HEADER})

aux_source_directory(. SRC)
aux_source_directory(../proto/gen_proto PROTO)

add_executable(gateserver ${SRC} ${PROTO} ${HEADER})

target_link_libraries(gateserver framework mysqlclient  pthread z m rt dl protobuf)

#find_library(MYLIBRARY
#	NAMES LBFing
#	PATHS ../build/bin)
#if(MYLIBRARY)
#	target_link_libraries(gateserver ${MYLIBRARY})
#else()
#	message(FATAL_ERROR "LBFing find error")
#endif(MYLIBRARY)

