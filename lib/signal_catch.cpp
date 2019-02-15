#include "signal_catch.h"

//设置要捕获的信号
void SetSignedCatched(CatchCallback& cb)
{
	//设置要捕获的信号 调用ctrlHandler
	struct sigaction sig;
	sigemptyset(&sig.sa_mask);
	sig.sa_flags = 0;
	sig.sa_handler = cb;
	sigaction(SIGINT, &sig, nullptr);
	sigaction(SIGQUIT, &sig, nullptr);
	sigaction(SIGABRT, &sig, nullptr);
	sigaction(SIGTERM, &sig, nullptr);
	sigaction(SIGTERM, &sig, nullptr);
	sigaction(SIGUSR1, &sig, nullptr);
	sigaction(SIGUSR2, &sig, nullptr);

	// 忽略PIPE信号
	sig.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &sig, nullptr);
	sigaction(SIGHUP, &sig, nullptr);
}

