#include "poll_poller.h"
#include "logger.h"
#include "channel.h"

PollPoller::PollPoller(EventLoop* loop) : Poller(loop)
{
}

PollPoller::~PollPoller()
{
}

Timestamp PollPoller::Poll(int32 timeOuts, ChannelVec* activeChannels)
{
	int32 numEvevnts = ::poll(&*m_vecPollFd.begin(), m_vecPollFd.size(), timeOuts);
	int32 saveErrno = errno;
	Timestamp now(Timestamp::Now());
	if (numEvevnts > 0)
	{
		//DEBUG("Poll %d events happend", numEvevnts);
		fillActiveChannel(numEvevnts, activeChannels);
	}
	else if (numEvevnts == 0)
	{
		DEBUG("Nothing happend");
	}
	else
	{
		if (saveErrno != EINTR)
		{
			errno = saveErrno;
			ERROR("PollPoller::Poll()");
		}
	}
	return now;
}

void PollPoller::UpdateChannel(Channel* channel)
{
	Poller::AssertInLoopThread();
	//DEBUG("fd = %d events= %d", channel->GetFd(), channel->GetEvents());
	if (channel->Index() < 0)
	{
		assert(m_mapChannel.find(channel->GetFd()) == m_mapChannel.end());
		struct pollfd pfd;
		pfd.fd = channel->GetFd();
		pfd.events = static_cast<short>(channel->GetEvents());
		pfd.revents = 0;

		m_vecPollFd.push_back(pfd);
		int32 idx = static_cast<int32>(m_vecPollFd.size() - 1);
		channel->SetIndex(idx);
		m_mapChannel[pfd.fd] = channel;
		assert(pfd.fd == channel->GetFd() || pfd.fd == -channel->GetFd() - 1);
	}
	else
	{
		assert(m_mapChannel.find(channel->GetFd()) == m_mapChannel.end());
		assert(m_mapChannel[channel->GetFd()] == channel);
		int32 idx = channel->Index();
		assert(0 <= idx && idx < static_cast<int32>(m_vecPollFd.size()));
		struct pollfd& pfd = m_vecPollFd[idx];
		pfd.events = static_cast<short>(channel->GetEvents());
		pfd.revents = 0;
		if (channel->IsNoneEvent())
		{
			// ignore this pollfd
			pfd.fd = -channel->GetFd() - 1;
		}

	}
}

void PollPoller::RemoveChannel(Channel* channel)
{
	Poller::AssertInLoopThread();
	//DEBUG("fd = %d", channel->GetFd());
	assert(m_mapChannel.find(channel->GetFd()) != m_mapChannel.end());
	assert(m_mapChannel[channel->GetFd()] == channel);
	assert(channel->IsNoneEvent());
	int32 idx = channel->Index();
	assert(0 <= idx && idx < static_cast<int32>(m_vecPollFd.size()));
	const struct pollfd& pfd = m_vecPollFd[idx];
	assert(pfd.fd == -channel->GetFd() - 1 && pfd.events == channel->GetEvents());
	size_t n = m_mapChannel.erase(channel->GetFd());
	assert(n == 1);
	if (static_cast<size_t>(idx) == m_vecPollFd.size() - 1)
	{
		m_vecPollFd.pop_back();
	}
	else
	{
		int32 channelAtEnd = m_vecPollFd.back().fd;
		iter_swap(m_vecPollFd.begin() + idx, m_vecPollFd.end() - 1);
		if (channelAtEnd < 0)
		{
			channelAtEnd = -channelAtEnd - 1;
		}
		m_mapChannel[channelAtEnd]->SetIndex(idx);
		m_vecPollFd.pop_back();
	}

}

void PollPoller::fillActiveChannel(int32 numEvents, ChannelVec* activeChannels) const
{
	for (vecPollFd::const_iterator it = m_vecPollFd.begin();
	        it != m_vecPollFd.end() && numEvents > 0; ++it)
	{
		if (it->revents > 0)
		{
			--numEvents;
			ChannelMap::const_iterator itFind = m_mapChannel.find(it->fd);
			assert(itFind != m_mapChannel.end());

			Channel* channel = itFind->second;
			assert(channel->GetFd() == it->fd);
			channel->SetRevents(it->revents);
			activeChannels->push_back(channel);
		}
	}
}




