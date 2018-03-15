#ifndef __POLLER_H__
#define __POLLER_H__

#include "timer.h"
#include "event_loop.h"
#include "nocopyable.h"

class Channel;
class EventLoop;

class Poller : private Nocopyable
{
public:
	typedef std::vector<Channel*> ChannelVec;
	explicit Poller(EventLoop* loop);
	virtual ~Poller();

	// Polls the I/O events.
	// Must be called in the loop thread.
	virtual Timestamp Poll(int32 timeoutMs, ChannelVec* activeChannels) = 0;

	// Changes the interested I/O events.
	// Must be called in the loop thread.
	virtual void UpdateChannel(Channel* channel) = 0;

	// Remove the channel, when it destructs.
	// Must be called in the loop thread.
	virtual void RemoveChannel(Channel* channel) = 0;

	virtual bool HasChannel(Channel* channel) const;

	static Poller* NewDefaultPoller(EventLoop* loop); //待实现

	void AssertInLoopThread() const
	{
		m_ownerLoop->AssertInLoopThread();
	}
protected:
	typedef std::map<int32, Channel*> ChannelMap;
	ChannelMap m_mapChannel;
private:
	EventLoop* m_ownerLoop;

};

#endif

