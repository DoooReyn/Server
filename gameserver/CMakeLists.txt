cmake_minimum_required(VERSION 2.8)
project(gameserver)

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
	"/root/oschina/Server/build/bin"
	"/usr/lib/x86_64-linux-gnu"
	"/usr/local/lib"
	)

file(GLOB_RECURSE HEADER *.h)
source_group("Header Files" FILES ${HEADER})

aux_source_directory(. SRC)
aux_source_directory(../proto/gen_proto PROTO)

add_executable(gameserver ${SRC} ${PROTO} ${HEADER})

target_link_libraries(gameserver framework mysqlclient  pthread z m rt dl protobuf hiredis)

#find_library(MYLIBRARY
#	NAMES LBFing
#	PATHS ../build/bin)
#if(MYLIBRARY)
#	target_link_libraries(gameserver ${MYLIBRARY})
#else()
#	message(FATAL_ERROR "LBFing find error")
#endif(MYLIBRARY)

