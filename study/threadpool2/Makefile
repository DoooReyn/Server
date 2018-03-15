all:
	g++ -g CondVar.cpp -lpthread -c -o CondVar.o
	g++ -g Mutex.cpp -lpthread -c -o Mutex.o
	g++ -g Task.cpp -lpthread -c -o Task.o
	g++ -g ThreadPool.cpp -lpthread -c -o ThreadPool.o
	g++ -g CondVar.o Mutex.o Task.o ThreadPool.o threadpool_test.cpp -lpthread -o threadpool_test

#all:
#	g++ threadpool.cpp -lpthread -fpic -c -o bin/obj/threadpool.o
#	g++ -L./bin bin/obj/threadpool.o -lpthread threadpool_test.cpp -o bin/example/threadpool_test

#threadpool:
#	g++ threadpool.cpp -lpthread -fpic -c -o bin/obj/threadpool.o
#	g++ -shared -fPIC bin/obj/threadpool.o -o bin/lib/libthreadpool.so
#example:
#	g++ -L./bin/lib -lthreadpool threadpool_test.cpp -o threadpool_test
#
clean:
	rm *.o threadpool_test -f
