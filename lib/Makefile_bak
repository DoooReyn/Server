CC		=	g++
FLAGS	= 	-Wall -Werror -g -std=c++11
SRCFILE	=	$(wildcard *.cpp)
OBJS	=	$(patsubst %.cpp, %.o, $(SRCFILE))
PROGRAM =	libLBFing.a
LIBS 	=  -lpthread  -lrt
INC 	=

LIBS_MYSQL= ${shell mysql_config --libs_r}
CPPFLAGS_MYSQL=${shell mysql_config --cflags}

LIBS_REDIS= ${shell pkg-config --libs hiredis}
CPPFLAGS_REDIS=${shell pkg-config --cflags hiredis}

#mysql库
LIBS += $(LIBS_MYSQL)
INC += $(CPPFLAGS_MYSQL)

#redis库
LIBS += $(LIBS_REDIS)
INC += $(CPPFLAGS_REDIS)

defalut:	$(OBJS) $(PROGRAM)

%.o:%.cpp
	$(CC) $(FLAGS) $(INC) -c $< -o $@

#$(PROGRAM):$(OBJS)
#	$(CC) $(FLAGS) -o $@ $^ $(LIBS)

$(PROGRAM):$(OBJS)
	ar rcs $@ $^
	ranlib $(PROGRAM)

.PHONY:clean
clean:
	$(RM) $(OBJS) $(PROGRAM)
	$(RM) *.o
