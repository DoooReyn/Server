#ifndef __BASE_H__
#define __BASE_H__

#include <stdio.h>      // 系统标准输入输出
#include <stdarg.h>		/* ANSI C header file */
#include <syslog.h>		// 系统日志相关
#include <stdlib.h>     // 标准库相关
#include <errno.h>      // 定义系统错误相关
#include <string.h>     // 字符和字符串定义
#include <unistd.h>     // unix 读写相关
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>   //文件状态
#include <fcntl.h>      //文件控制

#define	MAXLINE		4096	/* max text line length */
#define	MAX_PATH	1024
#define	MAXLINE		4096	/* max text line length */
#define	BUFFSIZE	8192	/* buffer size for reads and writes */

int	Open(const char*, int, ...);
char* Fgets(char*, int, FILE*);
FILE* Fopen(const char*, const char*);
void Fclose(FILE*);
void  Fputs(const char*, FILE*);
FILE* Popen(const char*, const char*);
int	Pclose(FILE*);


void Write(int, void*, size_t);
ssize_t Read(int, void*, size_t);
void Close(int);

void Pipe(int*);
pid_t Waitpid(pid_t, int*, int);
pid_t Fork(void);

void Unlink(const char*);


void err_dump(const char*, ...);
void err_msg(const char*, ...);
void err_quit(const char*, ...);
void err_ret(const char*, ...);
void err_sys(const char*, ...);

#define	FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)	//定义文件模式
#define	va_mode_t	mode_t
#endif
