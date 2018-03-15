#include "Thread.h"
#include <iostream>
using namespace std;

Thread::Thread()
{
	cout << " Thread ..." << endl;
}

Thread::~Thread()
{

	cout << " ~Thread ..." << endl;
}

void Thread::Start()
{
	pthread_create(&m_threadId, NULL, ThreadRoutine, this);
}

void Thread::Join()
{
	pthread_join(m_threadId, NULL);
}

void* Thread::ThreadRoutine(void* arg)
{
	Thread* pThis = static_cast<Thread*>(arg);
	pThis->Run();
	return NULL;
}
