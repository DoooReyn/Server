#ifndef __POLL_POLLER_H__
#define __POLL_POLLER_H__

#include "poller.h"

class PollPoller : public Poller
{
public:
	explicit PollPoller(EventLoop* loop);
	virtual ~PollPoller();

	virtual Timestamp Poll(int32 timeOuts, ChannelVec* activeChannels);
	virtual void UpdateChannel(Channel* channel);
	virtual void RemoveChannel(Channel* channel);

private:
	void fillActiveChannel(int32 numEvents, ChannelVec* activeChannels) const;

	typedef std::vector<struct pollfd> vecPollFd;
	vecPollFd m_vecPollFd;
};



#endif