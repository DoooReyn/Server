#include "poll_epoller.h"
#include "logger.h"
#include "channel.h"


STATIC_ASSERT_CHECK(EPOLLIN == POLLIN);
STATIC_ASSERT_CHECK(EPOLLPRI == POLLPRI);
STATIC_ASSERT_CHECK(EPOLLOUT == POLLOUT);
STATIC_ASSERT_CHECK(EPOLLRDHUP == POLLRDHUP);
STATIC_ASSERT_CHECK(EPOLLERR == POLLERR);
STATIC_ASSERT_CHECK(EPOLLHUP == POLLHUP);

const int32 kNew = -1;
const int32 kAdded = 1;
const int32 kDeleted = 2;


EPollPoller::EPollPoller(EventLoop* loop) : Poller(loop), m_epollfd(::epoll_create1(EPOLL_CLOEXEC)), m_vecEvent(kInitEventListSize)
{
	//INFO("EPollPoller Fd:%d",m_epollfd);
}

EPollPoller::~EPollPoller()
{
	::close(m_epollfd);
}

Timestamp EPollPoller::Poll(int32 timeOuts, ChannelVec* activeChannels)
{
	//DEBUG("fd total count :%ld", m_mapChannel.size());
	int32 numEvents = ::epoll_wait(m_epollfd, &*m_vecEvent.begin(), static_cast<int32>(m_vecEvent.size()), timeOuts);
	int32 saveErrno = errno;
	Timestamp now(Timestamp::Now());
	if (numEvents > 0)
	{
		//DEBUG("%d events happended", numEvents);
		fillActiveChannels(numEvents, activeChannels);
		if (static_cast<size_t>(numEvents) == m_vecEvent.size())
		{
			m_vecEvent.resize(m_vecEvent.size() * 2);
		}
	}
	else if (numEvents == 0)
	{
		//DEBUG("Nothing happended");
		return now;
	}
	else
	{
		if (saveErrno != EINTR)
		{
			errno = saveErrno;
			ERROR("EPollPoller::Poll()");
		}
	}
	return now;
}

void EPollPoller::UpdateChannel(Channel* channel)
{
	Poller::AssertInLoopThread();
	const int32 index = channel->Index();
	//DEBUG("fd = %d events = %d index = %d", channel->GetFd(), channel->GetEvents(), index);
	if (index == kNew || index == kDeleted)
	{
		int32 fd = channel->GetFd();
		if (index == kNew)
		{
			assert(m_mapChannel.find(fd) == m_mapChannel.end());
			m_mapChannel[fd] = channel;
		}
		else
		{
			assert(m_mapChannel.find(fd) != m_mapChannel.end());
			assert(m_mapChannel[fd] == channel);
		}
		channel->SetIndex(kAdded);
		update(EPOLL_CTL_ADD, channel);
	}
	else
	{
		int32 fd = channel->GetFd();
		assert(m_mapChannel.find(fd) != m_mapChannel.end());
		assert(m_mapChannel[fd] == channel);
		assert(index == kAdded);
		if (channel->IsNoneEvent())
		{
			update(EPOLL_CTL_DEL, channel);
			channel->SetIndex(kDeleted);
		}
		else
		{
			update(EPOLL_CTL_MOD, channel);
		}
	}
}

void EPollPoller::RemoveChannel(Channel* channel)
{
	Poller::AssertInLoopThread();
	int32 fd = channel->GetFd();
	//DEBUG("fd = %d", fd);
	assert(m_mapChannel.find(fd) != m_mapChannel.end());
	assert(m_mapChannel[fd] == channel);
	assert(channel->IsNoneEvent());
	int32 index = channel->Index();
	assert(index == kAdded || index == kDeleted);
	size_t n = m_mapChannel.erase(fd);
	assert(n == 1);

	if (index == kAdded)
	{
		update(EPOLL_CTL_DEL, channel);
	}
	channel->SetIndex(kNew);

}

const char* EPollPoller::operationToString(int32 op)
{
	switch (op)
	{
		case EPOLL_CTL_ADD:
			return "ADD";
		case EPOLL_CTL_DEL:
			return "DEL";
		case EPOLL_CTL_MOD:
			return "MOD";
		default:
			assert(false && "ERROR op");
			return "Unknown Operation";
	}
}

void EPollPoller::fillActiveChannels(int32 numEvents, ChannelVec* activeChannels) const
{
	assert(static_cast<size_t>(numEvents) <= m_vecEvent.size());
	for (int32 i = 0; i < numEvents; ++i)
	{
		Channel* channel = static_cast<Channel*>(m_vecEvent[i].data.ptr);

		int32 fd = channel->GetFd();
		ChannelMap::const_iterator itFind = m_mapChannel.find(fd);
		assert(itFind != m_mapChannel.end());
		assert(itFind->second == channel);

		channel->SetRevents(m_vecEvent[i].events);
		activeChannels->push_back(channel);
	}
}

void EPollPoller::update(int32 operation, Channel* channel)
{
	struct epoll_event event;
	bzero(&event, sizeof(event));
	event.events = channel->GetEvents();
	event.data.ptr = channel;
	int fd = channel->GetFd();
	//DEBUG("epoll_ctl op = %s fd = %d event = { %s }", operationToString(operation), fd, channel->EventsToString().c_str());
	if (::epoll_ctl(m_epollfd, operation, fd, &event) < 0)
	{
		ERROR("epoll_ctl op = %s fd = %d", operationToString(operation), fd);
	}
}
