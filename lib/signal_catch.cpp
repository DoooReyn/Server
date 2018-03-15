#include "signal_catch.h"

//设置要捕获的信号
void SetSignedCatched(CatchCallback& cb)
{
	//设置要捕获的信号 调用ctrlHandler
	struct sigaction sig;
	sigemptyset(&sig.sa_mask);
	sig.sa_flags = 0;
	sig.sa_handler = cb;
	sigaction(SIGINT, &sig, NULL);
	sigaction(SIGQUIT, &sig, NULL);
	sigaction(SIGABRT, &sig, NULL);
	sigaction(SIGTERM, &sig, NULL);
	sigaction(SIGTERM, &sig, NULL);
	sigaction(SIGUSR1, &sig, NULL);
	sigaction(SIGUSR2, &sig, NULL);

	// 忽略PIPE信号
	sig.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &sig, NULL);
	sigaction(SIGHUP, &sig, NULL);
}

