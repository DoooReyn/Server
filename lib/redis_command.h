#ifndef __REDIS__COMMAND_H
#define __REDIS__COMMAND_H

#include "type_define.h"
#include <hiredis/hiredis.h>

class Redis
{
public:
	Redis();
	~Redis();
	bool connect(string host, int port);
	string get(string key);
	void set(string key, string value);
private:
	redisContext* m_connect;
	redisReply* m_reply;
};

#endif
