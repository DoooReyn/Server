CC	=	g++
FLAGS	= 	-Wall -g
SRCFILE	=	$(wildcard *.cpp)
OBJS	=	$(patsubst %.cpp, %.o, $(SRCFILE))
LIB		=
PROGRAM =	Main

defalut:	$(OBJS) $(PROGRAM)

%.o : %.cpp
	$(CC) $(FLAGS) -c $< -o $@

$(PROGRAM) : $(OBJS)
	$(CC) $(FLAGS) -o $@ $^ $(LIB)

.PHONY : clean
clean:
	$(RM) $(OBJS) $(PROGRAM)
