#include "poller.h"
#include "poll_poller.h"
#include "poll_epoller.h"

Poller* Poller::NewDefaultPoller(EventLoop* loop)
{
	if (::getenv("LIB_USE_POLL"))
	{
		return new PollPoller(loop);
	}
	else
	{
		return new EPollPoller(loop);
	}
}