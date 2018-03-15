#ifndef __CALLBACK_H__
#define __CALLBACK_H__

#include "timer.h"

class BufferEx;
class TcpConnection;
struct PackHead;

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef std::function<void (const TcpConnectionPtr&)> ConnectionCallback;
typedef std::function<void (const TcpConnectionPtr&)> DisConnectCallback;
typedef std::function<void (const TcpConnectionPtr&)> CloseCallback;
typedef std::function<void (const TcpConnectionPtr&)> WriteCompleteCallback;
typedef std::function<void (const TcpConnectionPtr&, size_t)> HighWaterMarkCallback;
typedef std::function<void(const TcpConnectionPtr&, int32, int32)> VerifyCallback;
typedef std::function<bool (const TcpConnectionPtr&, PackHead*, const char*)> TransmitCallback;

// the data has been read to (buf, len)
typedef std::function<void (const TcpConnectionPtr&, BufferEx*, Timestamp&)> MessageCallback;
void DefaultConnectionCallback(const TcpConnectionPtr& conn);
void DefaultDisconnectionCallback(const TcpConnectionPtr& conn);
void DefaultMessageCallback(const TcpConnectionPtr& conn, BufferEx* buffer, Timestamp& receiveTime);

typedef std::map<int64, const TcpConnectionPtr> MapConnPtr;
typedef MapConnPtr::iterator itConnPtr;


template<typename CLASS>
class WeakCallback
{
public:
	WeakCallback(const std::weak_ptr<CLASS>& obj, const std::function<void(CLASS*)>& func)
		: m_object(obj)
		, m_func(func)
	{
	}

	void operator()()const
	{
		std::shared_ptr<CLASS> ptr(m_object.lock());
		if (ptr)
		{
			m_func(ptr.get());
		}
	}

private:
	std::weak_ptr<CLASS> m_object;
	std::function<void (CLASS*)> m_func;
};

template<typename CLASS>
WeakCallback<CLASS> makeWeakCallback(const std::shared_ptr<CLASS>& object,
                                     void (CLASS::*function)())
{
	return WeakCallback<CLASS>(object, function);
}

template<typename CLASS>
WeakCallback<CLASS> makeWeakCallback(const std::shared_ptr<CLASS>& object,
                                     void (CLASS::*function)() const)
{
	return WeakCallback<CLASS>(object, function);
}

#endif
