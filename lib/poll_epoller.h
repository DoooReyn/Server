#ifndef __POLL_EPOLLER_H__
#define  __POLL_EPOLLER_H__
#include "poller.h"


class EPollPoller : public Poller
{
public:
	explicit EPollPoller(EventLoop* loop);
	virtual ~EPollPoller();

	virtual Timestamp Poll(int32 timeOuts, ChannelVec* activeChannels);
	virtual void UpdateChannel(Channel* channel);
	virtual void RemoveChannel(Channel* channel);

private:
	static const int32 kInitEventListSize = 16;

	static const char* operationToString(int32 op);

	void fillActiveChannels(int32 numEvents, ChannelVec* activeChannels) const;
	void update(int32 operation, Channel* channel);

	typedef std::vector<struct epoll_event> EventVec;

	int32 m_epollfd;
	EventVec m_vecEvent;

};


#endif
