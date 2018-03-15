#include "inet_address.h"
#include "logger.h"

#define BOOST_CHECK_EQUAL(a,b) \
	if(a != b) \
	{\
		std::cout << "Test Failed Line:" <<__LINE__ << " "<< a << " "<<b << endl; \
		exit(0); \
	}


void Test1()
{
	DEBUG("Test1 Start");
	InetAddress addr0(1234);
	BOOST_CHECK_EQUAL(addr0.ToIP(), string("0.0.0.0"));
	BOOST_CHECK_EQUAL(addr0.ToIPPort(), string("0.0.0.0:1234"));
	BOOST_CHECK_EQUAL(addr0.ToPort(), 1234);

	InetAddress addr1(4321, true);
	BOOST_CHECK_EQUAL(addr1.ToIP(), string("127.0.0.1"));
	BOOST_CHECK_EQUAL(addr1.ToIPPort(), string("127.0.0.1:4321"));
	BOOST_CHECK_EQUAL(addr1.ToPort(), 4321);

	InetAddress addr2("1.2.3.4", 8888);
	BOOST_CHECK_EQUAL(addr2.ToIP(), string("1.2.3.4"));
	BOOST_CHECK_EQUAL(addr2.ToIPPort(), string("1.2.3.4:8888"));
	BOOST_CHECK_EQUAL(addr2.ToPort(), 8888);

	InetAddress addr3("255.254.253.252", 65535);
	BOOST_CHECK_EQUAL(addr3.ToIP(), string("255.254.253.252"));
	BOOST_CHECK_EQUAL(addr3.ToIPPort(), string("255.254.253.252:65535"));
	BOOST_CHECK_EQUAL(addr3.ToPort(), 65535);
	DEBUG("Test1 Success");
}

int main()
{
	InitLogger("../log/test.log", "debug");
	Test1();
	return 0;
}