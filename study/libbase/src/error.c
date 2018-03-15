#include "base.h"

int daemon_proc; /*set nonzero by daemon_init*/

static void err_doit(int, int, const char*, va_list);

//输出错误信息
void err_ret(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	err_doit(1, LOG_INFO, fmt, ap);
	va_end(ap);
}

//输出错误信息，然后退出
void err_sys(const char* fmt, ...)
{
	va_list	ap;
	va_start(ap, fmt);
	err_doit(1, LOG_ERR, fmt, ap);
	va_end(ap);
	exit(1);
}

void err_dump(const char* fmt, ...)
{
	va_list		ap;
	va_start(ap, fmt);
	err_doit(1, LOG_ERR, fmt, ap);
	va_end(ap);
	abort();		/* dump core and terminate */
	exit(1);		/* shouldn't get here */
}

void
err_msg(const char* fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	err_doit(0, LOG_INFO, fmt, ap);
	va_end(ap);
	return;
}

void
err_quit(const char* fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	err_doit(0, LOG_ERR, fmt, ap);
	va_end(ap);
	exit(1);
}

static void
err_doit(int errnoflag, int level, const char* fmt, va_list ap)
{
	int		errno_save, n;
	char	buf[MAXLINE];

	errno_save = errno;		/* value caller might want printed */
#ifdef	HAVE_VSNPRINTF
	vsnprintf(buf, sizeof(buf), fmt, ap);	/* this is safe */
#else
	vsprintf(buf, fmt, ap);					/* this is not safe */
#endif
	n = strlen(buf);
	if (errnoflag)
	{
		snprintf(buf + n, sizeof(buf) - n, ": %s", strerror(errno_save));
	}
	strcat(buf, "\n");

	if (daemon_proc)
	{
		syslog(level, "%s", buf);
	}
	else
	{
		//清除读写缓冲区，需要立即把输出缓冲区的数据进行物理写入时
		fflush(stdout);		/* in case stdout and stderr are the same */
		//向指定的文件写入一个字符串 不自动写入字符串结束标记符'\0'
		fputs(buf, stderr);
		fflush(stderr);
	}
	return;
}
