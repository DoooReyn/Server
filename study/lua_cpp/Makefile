CC	=	g++
FLAGS	= 	-Wall
SRCFILE	=	$(wildcard *.cpp)
OBJS	=	$(patsubst %.cpp, %.o, $(SRCFILE))
LIB		=   -llua -ltolua++ -ldl
PROGRAM =	test1 test2

defalut:	$(OBJS) $(PROGRAM)

#lua:
#	tolua++ -o lua_student.h student.pkg

%.o : %.cpp
	$(CC) $(FLAGS) -c $< -o $@

test1:test1.o
	$(CC) $(FLAGS) -o $@ $^ $(LIB)
test2:test2.o
	$(CC) $(FLAGS) -o $@ $^ $(LIB)

.PHONY : clean
clean:
	$(RM) $(OBJS) $(PROGRAM)
