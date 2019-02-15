#include "thread.h"
#include "logger.h"

pid_t GetTid()
{
	return static_cast<pid_t>(::syscall(SYS_gettid));
}

//void AfterFork()
//{
//	CurrentThread::t_CachedTid = 0;
//	CurrentThread::t_ThreadName = "main";
//	CurrentThread::Tid();
//}

namespace CurrentThread
{
__thread int32_t t_CachedTid = 0;
__thread char t_TidString[32];
__thread int32 t_TidStringLength = 6;
__thread const char* t_ThreadName = "unknow";
}

void CurrentThread::CacheTid()
{
	if(t_CachedTid == 0)
	{
		t_CachedTid = GetTid();
		t_TidStringLength = snprintf(t_TidString, sizeof(t_TidString), "%5d", t_CachedTid);
	}
}

bool CurrentThread::isMainThread()
{
	return Tid() == ::getpid();
}

void CurrentThread::SleepUsec(int64 usec)
{
	struct timespec ts = {0, 0};
	ts.tv_sec = static_cast<time_t> (usec / 1000000);
	ts.tv_nsec = static_cast<long>(usec % 1000000 * 1000);
	::nanosleep(&ts, nullptr);
}

struct ThreadData
{
	typedef::Thread::ThreadFunc ThreadFunc;
	ThreadFunc m_func;
	string m_strName;
	std::weak_ptr<pid_t> m_wkTid;

	ThreadData(const ThreadFunc& func,
	           const string& name,
	           const std::shared_ptr<pid_t>& tid)
		: m_func(func)
		, m_strName(name)
		, m_wkTid(tid)
	{}

	void RunInThread()
	{
		pid_t tid = CurrentThread::Tid();
		std::shared_ptr<pid_t> ptid = m_wkTid.lock();
		if(ptid)
		{
			*ptid = tid;
			ptid.reset();
		}

		CurrentThread::t_ThreadName = m_strName.empty() ? "Thread" : m_strName.c_str();
		::prctl(PR_SET_NAME, CurrentThread::t_ThreadName);

		try
		{
			m_func();
			CurrentThread::t_ThreadName = "finished";
		}
		catch(const std::exception& ex)
		{
			CurrentThread::t_ThreadName = "crashed";
			ERROR("exception caught in Thread %s\n", m_strName.c_str());
			ERROR("reason: %s\n", ex.what());
			//abort();
			//throw;
		}
		catch (...)
		{
			CurrentThread::t_ThreadName = "crashed";
			ERROR("unknown exception caught in Thread %s\n", m_strName.c_str());
			throw; // rethrow
		}
	}
};

void* StartThread(void* obj)
{
	ThreadData* data = static_cast<ThreadData*>(obj);
	if (data)
	{
		data->RunInThread();
		delete data;
		data = nullptr;
	}
	return data;
}

AtomicInt32 Thread::m_numCreated;

Thread::Thread(const ThreadFunc& func, const string& name)
	: m_bStarted(false)
	, m_bJoined(false)
	, m_pthreadId(0)
	, m_tid(new pid_t(0))
	, m_func(func)
	, m_strName(name)
{
	setDefaultName();
}

Thread::~Thread()
{
	if(m_bStarted && !m_bJoined)
	{
		pthread_detach(m_pthreadId);
	}
}

void Thread::Start()
{
	assert(!m_bStarted);
	m_bStarted = true;
	ThreadData* data = new ThreadData(m_func, m_strName, m_tid);
	if(data == nullptr)
	{
		fprintf(stderr, "new thread data failed!!!");
		abort();
	}

	if(pthread_create(&m_pthreadId, nullptr, StartThread, data) != 0)
	{
		m_bStarted = false;
		fprintf(stderr, "create thread failed!!!");
		delete data;
		data = nullptr;
		abort();
	}
}
int32 Thread::Join()
{
	assert(m_bStarted);
	assert(!m_bJoined);
	m_bJoined = true;
	return pthread_join(m_pthreadId, nullptr);
}
void Thread::setDefaultName()
{
	int32 num = m_numCreated.IncrementAndGet();
	if(m_strName.empty())
	{
		char buf[32];
		snprintf(buf, sizeof(buf), "Thread_%d", num);
		m_strName = buf;
	}
}
