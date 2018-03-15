CC	=	g++
FLAGS	= 	-Wall
SRCFILE	=	$(wildcard *.cpp)
OBJS	=	$(patsubst %.cpp, %.o, $(SRCFILE))
PROGRAM =	Main
LIBS	=   -lpthread

defalut:	$(OBJS) $(PROGRAM)

%.o : %.cpp
	$(CC) $(FLAGS) -c $< -o $@

$(PROGRAM) : $(OBJS)
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)
clean:
	$(RM) $(OBJS) $(PROGRAM)
