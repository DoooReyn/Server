#include "redis_command.h"

Redis::Redis()  : m_connect(nullptr), m_reply(nullptr)
{}

Redis::~Redis()
{
	if (m_connect)
	{
		redisFree(m_connect);
		m_connect = nullptr;
	}
	m_reply = nullptr;
}

bool Redis::connect(string host, int port)
{
	m_connect = redisConnect(host.c_str(), port);
	if(m_connect != nullptr && m_connect->err)
	{
		printf("connect error: %s\n", m_connect->errstr);
		return 0;
	}
	return 1;
}

string Redis::get(string key)
{
	m_reply = (redisReply*)redisCommand(m_connect, "GET %s", key.c_str());
	string str = m_reply->str;
	freeReplyObject(m_reply);
	return str;
}

void Redis::set(string key, string value)
{
	redisCommand(m_connect, "SET %s %s", key.c_str(), value.c_str());
}