CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(libframework)

#SET(CMAKE_BUILD_TYPE Debug CACHE STRING "set build type to debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -Wall -Werror -g -D _DEBUG -std=c++11")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -Wall -Werror -std=c++11")



SET(LIBRARY_OUTPUT_PATH ../bin)

INCLUDE_DIRECTORIES(
	"../common"
	"../proto/gen_proto"
	"/usr/include/mysql"
	"/usr/local/include/hiredis"
	)

file(GLOB_RECURSE HEADER *.h)
source_group("Header Files" FILES ${HEADER})


AUX_SOURCE_DIRECTORY(. LIBSRC)
ADD_LIBRARY(framework ${LIBSRC} ${HEADER})

