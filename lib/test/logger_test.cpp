
#include "logger.h"

void TestLogger()
{
	InitLogger("../log/test.log", "debug");
	DEBUG("++++++++++++++++++++++");
	WARN("++++++++++++++++++++++");
	INFO("++++++++++++++++++++++");
	ERROR("++++++++++++++++++++++");
}

int main(int argc, char const* argv[])
{
	TestLogger();
	return 0;
}
