CC	=	g++
FLAGS	= 	-Wall
SRCFILE	=	$(wildcard *.cpp)
OBJS	=	$(patsubst %.cpp, %.o, $(SRCFILE))
LIB		=   -llua -ltolua++ -ldl
PROGRAM =	Main

defalut:	$(OBJS) $(PROGRAM)

lua:
	tolua++ -o lua_student.h student.pkg

%.o : %.cpp
	$(CC) $(FLAGS) -c $< -o $@

$(PROGRAM) : $(OBJS)
	$(CC) $(FLAGS) -o $@ $^ $(LIB)

.PHONY : clean
clean:
	$(RM) $(OBJS) $(PROGRAM) lua_student.h lua_student.o
