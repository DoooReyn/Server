CC      =   g++
FLAGS   =   -Wall -Werror -g -std=c++11
SRCFILE =   $(wildcard *.cpp)
OBJS    =   $(patsubst %.cpp, %.o, $(SRCFILE))
LIBS    =   -L../../build/bin -lframework -lpthread  -lrt -L/usr/local/lib -lprotobuf
INC     =   -I../

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

PROGRAM = 	buffer_ex_test \
		  	singleton_test \
		  	entry_manager_test \
		  	buffer_test \
		  	timer_test \
		  	xml_test \
		  	json_test \
		  	regex_test \
		  	mysql_test \
		  	var_type_test \
		  	redis_test \
		  	blocking_queue_test \
		  	logger_test \
			threadpool_test \
			timer_queue_test \
			list_test \
			csv_test \
			memory_pool_test



defalut:	$(PROGRAM)

%.o:%.cpp
	$(CC) $(FLAGS) $(INC) -c $< -o $@

buffer_ex_test:buffer_ex_test.o
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)

singleton_test:singleton_test.o
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)

entry_manager_test:entry_manager_test.o
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)

buffer_test:buffer_test.o
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)

timer_test:timer_test.o
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)

xml_test:xml_test.o
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)

json_test:json_test.o
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)

regex_test:regex_test.o
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)

list_test:list_test.o
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)

message_queueu_test:message_queueu_test.o
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)

mysql_test:mysql_test.o
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)

var_type_test:var_type_test.o
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)

signal_test:signal_test.o
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)

mem_shared_test:mem_shared_test.o
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)

redis_test:redis_test.o
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)

blocking_queue_test:blocking_queue_test.o
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)

logger_test:logger_test.o
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)

thread_test:thread_test.o
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)

threadpool_test:threadpool_test.o
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)

timer_queue_test:timer_queue_test.o
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)

event_loop_test:event_loop_test.o
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)

event_loop_thread_test:event_loop_thread_test.o
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)

event_loop_thread_pool_test:event_loop_thread_pool_test.o
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)

inet_addr_test:inet_addr_test.o
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)

echo_server_test:echo_server_test.o
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)

echo_client_test:echo_client_test.o
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)

tcpclient_reg1_test:tcpclient_reg1_test.o
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)

tcpclient_reg2_test:tcpclient_reg2_test.o
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)

tcpclient_reg3_test:tcpclient_reg3_test.o
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)

csv_test:csv_test.o
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)

memory_pool_test:memory_pool_test.o
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)

.PHONY:clean
clean:
	$(RM) $(OBJS) $(PROGRAM)
	$(RM) *.o
