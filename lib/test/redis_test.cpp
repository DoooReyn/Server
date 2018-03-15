#include "redis_command.h"
#include "logger.h"
#include "r3c.h"

void TestRedisCommand()
{
	Redis* r = new Redis();
	if(!r->connect("127.0.0.1", 6379))
	{
		ERROR("redis connect error!");
		return;
	}
	INFO("redis connect success!");
	r->set("name", "Mayuyu");
	DEBUG("Redis name:%s", r->get("name").c_str());
	delete r;
}

void TestRedisCommand2()
{
	r3c::CRedisClient rc("127.0.0.1:6379");
	std::string value;
	rc.get("name", &value);
	DEBUG("CRedisClient name:%s", value.c_str());

}


int main(int argc, char const* argv[])
{
	InitLogger("/root/oschina/Server/lib/test/log/test.log", "debug");

	TestRedisCommand();
	TestRedisCommand2();
	return 0;
}