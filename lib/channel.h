#ifndef __CHANNEL_H__
#define __CHANNEL_H__

#include "type_define.h"
#include "nocopyable.h"
#include "timer.h"

class EventLoop;

class Channel : private Nocopyable
{
public:
	typedef std::function<void()> EventCallBack;
	typedef std::function<void(Timestamp&)> ReadEventCallback;

	Channel(EventLoop* loop, int32 fd);
	~Channel();

	void HandleEvent(Timestamp& receiveTime);
	void SetReadCallBack(const ReadEventCallback& cb)
	{
		m_readCallBack = cb;
	}

	void SetWriteCallBack(const EventCallBack& cb)
	{
		m_writeCallBack = cb;
	}

	void SetCloseCallBack(const EventCallBack& cb)
	{
		m_closeCallBack = cb;
	}

	void SetErrorCallBack(const EventCallBack& cb)
	{
		m_errorCallBack = cb;
	}

	void Tie(const  std::shared_ptr<void>&);

	int32 GetFd()const
	{
		return m_fd;
	}

	int32 GetEvents() const
	{
		return m_events;
	}

	void SetRevents(int32 revents)
	{
		m_revents = revents;
	}

	bool IsNoneEvent() const
	{
		return m_events == kNoneEvent;
	}

	void EnableReading()
	{
		m_events |= kReadEvent;
		update();
	}

	void DisableReading()
	{
		m_events &= ~kReadEvent;
		update();
	}

	void EnableWriting()
	{
		m_events |= kWriteEvent;
		update();
	}

	void DisableWriting()
	{
		m_events &= ~kWriteEvent;
		update();
	}

	void DisableAll()
	{
		m_events = kNoneEvent;
		update();
	}

	bool IsWriting() const
	{
		return m_events & kWriteEvent;
	}

	bool IsReading() const
	{
		return m_events & kReadEvent;
	}

	int32 Index()
	{
		return m_index;
	}

	void SetIndex(int32 index)
	{
		m_index = index;
	}

	string RevertsToString() const;
	string EventsToString() const;

	void doNotLogHup()
	{
		m_logHup = false;
	}

	EventLoop* OwnerLoop()
	{
		return m_pLoop;
	}

	void Remove();
private:
	static string eventsToString(int32 fd, int32 ev);

	void update();
	void handleEventWithGuard(Timestamp& receiveTime);

	static const int32 kNoneEvent;
	static const int32 kReadEvent;
	static const int32 kWriteEvent;

	EventLoop* m_pLoop;
	const int32 m_fd;
	int32 m_events;
	int32 m_revents;
	int32 m_index;
	bool m_logHup;

	std::weak_ptr<void> m_tie;
	bool m_tied;
	bool m_eventHandling;
	bool m_addedToLoop;
	ReadEventCallback m_readCallBack;
	EventCallBack m_writeCallBack;
	EventCallBack m_closeCallBack;
	EventCallBack m_errorCallBack;
};


#endif
