#include "channel.h"
#include "logger.h"
#include "event_loop.h"

const int32 Channel::kNoneEvent = 0;
const int32 Channel::kReadEvent = POLLIN | POLLPRI;
const int32 Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop* loop, int32 fd)
	: m_pLoop(loop)
	, m_fd(fd)
	, m_events(0)
	, m_revents(0)
	, m_index(-1)
	, m_logHup(true)
	, m_tied(false)
	, m_eventHandling(false)
	, m_addedToLoop(false)
{

}
Channel::~Channel()
{
	assert(!m_eventHandling);
	assert(!m_addedToLoop);
	if(m_pLoop->IsInLoopThread())
	{
		bool haved = m_pLoop->HasChannel(this);
		assert(haved == false);
	}
}

void Channel::Tie(const std::shared_ptr<void>& obj)
{
	m_tie = obj;
	m_tied = true;
}

void Channel::update()
{
	m_addedToLoop = true;
	m_pLoop->UpdateChannel(this);
}

void Channel::Remove()
{
	assert(IsNoneEvent());
	m_addedToLoop = false;
	m_pLoop->RemoveChannel(this);
}


string Channel::RevertsToString() const
{
	return eventsToString(m_fd, m_revents);
}
string Channel::EventsToString() const
{
	return eventsToString(m_fd, m_events);
}

string Channel::eventsToString(int32 fd, int32 ev)
{
	std::ostringstream oss;
	oss << fd << ": ";
	if (ev & POLLIN)
	{
		oss << "IN ";
	}
	if (ev & POLLPRI)
	{
		oss << "PRI ";
	}
	if (ev & POLLOUT)
	{
		oss << "OUT ";
	}
	if (ev & POLLHUP)
	{
		oss << "HUP ";
	}
	if (ev & POLLRDHUP)
	{
		oss << "RDHUP ";
	}
	if (ev & POLLERR)
	{
		oss << "ERR ";
	}
	if (ev & POLLNVAL)
	{
		oss << "NVAL ";
	}

	return oss.str();
}

void Channel::HandleEvent(Timestamp& receiveTime)
{
	std::shared_ptr<void> guard;
	if(m_tied)
	{
		guard = m_tie.lock();
		if(guard)
		{
			handleEventWithGuard(receiveTime);
		}
	}
	else
	{
		handleEventWithGuard(receiveTime);
	}
}

//POLLIN 有数据可读
//POLLPRI 有紧迫数据可读
//POLLOUT 写数据不会导致阻塞
//POLLHUP 指定的文件描述符挂起事件
//POLLNVAL 指定的文件描述符非法
void Channel::handleEventWithGuard(Timestamp& receiveTime)
{
	m_eventHandling = true;
	//DEBUG("%s", RevertsToString().c_str());
	if ((m_revents & POLLHUP) && !(m_revents & POLLIN))
	{
		if(m_logHup)
		{
			WARN("fd=%d Channel::HandleEvent() POLLHUP m_revents:%d", m_fd, m_revents);
		}
		if(m_closeCallBack)
		{
			m_closeCallBack();
		}
	}

	if(m_revents & POLLNVAL)
	{
		WARN("fd=%d Channel::HandleEvent() POLLNVAL", m_fd);
	}

	if(m_revents & (POLLERR | POLLNVAL))
	{
		if(m_errorCallBack)
		{
			m_errorCallBack();
		}
	}

	if (m_revents & (POLLIN | POLLPRI | POLLRDHUP))
	{
		if (m_readCallBack)
		{
			m_readCallBack(receiveTime);
		}
	}
	if (m_revents & POLLOUT)
	{
		if (m_writeCallBack)
		{
			m_writeCallBack();
		}
	}

	m_eventHandling = false;
}






