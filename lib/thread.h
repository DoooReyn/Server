#ifndef __THREAD_H__
#define __THREAD_H__
#include "type_define.h"
#include "atomic.h"
#include "nocopyable.h"


namespace CurrentThread
{
extern __thread int32 t_CachedTid;
extern __thread char t_TidString[32];
extern __thread int32 t_TidStringLength;
extern __thread const char* t_ThreadName;

void CacheTid();

inline int32 Tid()
{
	if(__builtin_expect(t_CachedTid == 0, 0))
	{
		CacheTid();
	}
	return t_CachedTid;
}

inline const char* TidString()
{
	return t_TidString;
}

inline int32 TidStringLength()
{
	return t_TidStringLength;
}

inline const char* Name()
{
	return t_ThreadName;
}

bool isMainThread();

void SleepUsec(int64 usec);
}



class Thread : private Nocopyable
{
public:
	typedef std::function<void ()> ThreadFunc;
	explicit Thread(const ThreadFunc& func, const string& name = string());

	~Thread();

	void Start();
	int32 Join();
	bool Started() const {return m_bStarted;}
	pid_t Tid() const {return *m_tid;}
	const string& Name() const {return m_strName;}
	static int32 NumCreated() {return m_numCreated.Get();}
private:
	void setDefaultName();
	bool m_bStarted;
	bool m_bJoined;
	pthread_t m_pthreadId;
	std::shared_ptr<pid_t> m_tid;
	ThreadFunc m_func;
	string m_strName;
	static AtomicInt32 m_numCreated;
};

#endif
