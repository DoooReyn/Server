#include "poller.h"
#include "channel.h"

Poller::Poller(EventLoop* loop)
	: m_ownerLoop(loop)
{
}

Poller::~Poller()
{
}

bool Poller::HasChannel(Channel* channel) const
{
	AssertInLoopThread();
	ChannelMap::const_iterator it = m_mapChannel.find(channel->GetFd());
	return it != m_mapChannel.end() && it->second == channel;
}
