#include "thread.h"
#include "mutex.h"
#include "timer.h"
#include <sys/types.h>
#include <sys/wait.h>

Mutex g_mutex;
std::map<int, int> g_delays;

void mysleep(int seconds)
{
	timespec t = { seconds, 0 };
	nanosleep(&t, NULL);
}

void threadFunc()
{
	printf("tid=%d\n", CurrentThread::Tid());
}

void threadFunc2(int x)
{
	printf("tid=%d, x=%d\n", CurrentThread::Tid(), x);
}

void threadFunc3()
{
	printf("tid=%d\n", CurrentThread::Tid());
	mysleep(1);
}
class Foo
{
public:
	explicit Foo(double x)
		: x_(x)
	{
	}

	void memberFunc()
	{
		printf("tid=%d, Foo::x_=%f\n", CurrentThread::Tid(), x_);
	}

	void memberFunc2(const std::string& text)
	{
		printf("tid=%d, Foo::x_=%f, text=%s\n", CurrentThread::Tid(), x_, text.c_str());
	}

private:
	double x_;
};

void threadFuncEx()
{
	//printf("tid=%d\n", muduo::CurrentThread::tid());
}

void threadFuncEx2(Time& start)
{
	Time end;
	int delay = static_cast<int32>(end.Usec() - start.Usec());
	MutexLockGuard lock(g_mutex);
	++g_delays[delay];
}

void forkBench()
{
	sleep(10);
	Time start;
	int kProcesses = 10 * 1000;

	for (int i = 0; i < kProcesses; ++i)
	{
		pid_t child = fork();
		if (child == 0)
		{
			exit(0);
		}
		else
		{
			waitpid(child, NULL, 0);
		}
	}

	Time end;
	printf("process creation time used %ld us\n", (end.Usec() - start.Usec()) / kProcesses);
	printf("number of created processes %d\n", kProcesses);
}

int main()
{
	printf("pid=%d, tid=%d\n", ::getpid(), CurrentThread::Tid());
	/*
	Thread t1(threadFunc);
	t1.Start();
	t1.Join();

	Thread t2(std::bind(threadFunc2, 42), "thread for free function with argument");
	t2.Start();
	t2.Join();

	Foo foo(87.53);
	Thread t3(std::bind(&Foo::memberFunc, &foo),"thread for member function without argument");
	t3.Start();
	t3.Join();

	Thread t4(std::bind(&Foo::memberFunc2, std::ref(foo), std::string("Shuo Chen")));
	t4.Start();
	t4.Join();

	{
		Thread t5(threadFunc3);
		t5.Start();
	// t5 may destruct eariler than thread creation.
	}
	mysleep(2);
	{
		Thread t6(threadFunc3);
		t6.Start();
		mysleep(2);
		// t6 destruct later than thread creation.
	}
	sleep(2);
	printf("number of created threads %d\n",Thread::NumCreated());
	*/
	printf("================================================\n");
	Time start;
	int kThreads = 100 * 1000;
	for (int i = 0; i < kThreads; ++i)
	{
		Thread t7(threadFuncEx);
		t7.Start();
		t7.Join();
	}
	Time end;
	double timeUsed = end.Usec() - start.Usec();
	printf("thread creation time %f us\n", timeUsed / kThreads);
	printf("number of created threads %d\n", Thread::NumCreated());

	for (int i = 0; i < kThreads; ++i)
	{
		Time time1;
		Thread t8(std::bind(threadFuncEx2, time1));
		t8.Start();
		t8.Join();
	}

	{
		MutexLockGuard lock(g_mutex);
		for (std::map<int, int>::iterator it = g_delays.begin();
		        it != g_delays.end(); ++it)
		{
			printf("delay = %d, count = %d\n",
			       it->first, it->second);
		}
	}

	forkBench();

	return 0;
}
