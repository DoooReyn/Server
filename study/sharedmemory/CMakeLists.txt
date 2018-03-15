PROJECT (test)

SET(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -Wall -Werror -g -D _DEBUG -std=c++11")

SET(SRC_LIST1 share_memory.cpp write.cpp)
SET(SRC_LIST2 share_memory.cpp read.cpp)

ADD_EXECUTABLE(write ${SRC_LIST1})
ADD_EXECUTABLE(read ${SRC_LIST2})

target_link_libraries(write rt)
target_link_libraries(read rt)