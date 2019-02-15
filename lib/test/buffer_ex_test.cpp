#include "buffer_ex.h"


#define BOOST_CHECK_EQUAL(a,b) \
	if(a != b) \
	{\
		std::cout << "Test Failed Line:" <<__LINE__ <<endl; \
		exit(0); \
	}

void TestBuff1()
{
	BufferEx buf;
	BOOST_CHECK_EQUAL(buf.ReadableBytes(), 0);
	BOOST_CHECK_EQUAL(buf.WriteableBytes(), BufferEx::nInitialSize);
	BOOST_CHECK_EQUAL(buf.PrependableBytes(), BufferEx::nCheapPrepend);


	const string str(200, 'x');
	buf.append(str);
	BOOST_CHECK_EQUAL(buf.ReadableBytes(), str.size());
	BOOST_CHECK_EQUAL(buf.WriteableBytes(), BufferEx::nInitialSize - str.size());
	BOOST_CHECK_EQUAL(buf.PrependableBytes(), BufferEx::nCheapPrepend);

	const string str2 =  buf.RetrieveAsString(50);
	BOOST_CHECK_EQUAL(str2.size(), 50);
	BOOST_CHECK_EQUAL(buf.ReadableBytes(), str.size() - str2.size());
	BOOST_CHECK_EQUAL(buf.WriteableBytes(), BufferEx::nInitialSize - str.size());
	BOOST_CHECK_EQUAL(buf.PrependableBytes(), BufferEx::nCheapPrepend + str2.size());
	BOOST_CHECK_EQUAL(str2, string(50, 'x'));

	buf.append(str);
	BOOST_CHECK_EQUAL(buf.ReadableBytes(), 2 * str.size() - str2.size());
	BOOST_CHECK_EQUAL(buf.WriteableBytes(), BufferEx::nInitialSize - 2 * str.size());
	BOOST_CHECK_EQUAL(buf.PrependableBytes(), BufferEx::nCheapPrepend + str2.size());

	const string str3 =  buf.RetrieveAllAsString();
	BOOST_CHECK_EQUAL(str3.size(), 350);
	BOOST_CHECK_EQUAL(buf.ReadableBytes(), 0);
	BOOST_CHECK_EQUAL(buf.WriteableBytes(), BufferEx::nInitialSize);
	BOOST_CHECK_EQUAL(buf.PrependableBytes(), BufferEx::nCheapPrepend);
	BOOST_CHECK_EQUAL(str3, string(350, 'x'));

	cout << __FUNCTION__ << " success!!!" << endl;
}

void TestBuff2()
{
	BufferEx buf;
	buf.append(string(400, 'y'));
	BOOST_CHECK_EQUAL(buf.ReadableBytes(), 400);
	BOOST_CHECK_EQUAL(buf.WriteableBytes(), BufferEx::nInitialSize - 400);

	buf.Retrieve(50);
	BOOST_CHECK_EQUAL(buf.ReadableBytes(), 350);
	BOOST_CHECK_EQUAL(buf.WriteableBytes(), BufferEx::nInitialSize - 400);
	BOOST_CHECK_EQUAL(buf.PrependableBytes(), BufferEx::nCheapPrepend + 50);

	buf.append(string(1000, 'z'));
	BOOST_CHECK_EQUAL(buf.ReadableBytes(), 1350);
	BOOST_CHECK_EQUAL(buf.WriteableBytes(), 0);
	BOOST_CHECK_EQUAL(buf.PrependableBytes(), BufferEx::nCheapPrepend + 50); // FIXME

	buf.RetrieveAll();
	BOOST_CHECK_EQUAL(buf.ReadableBytes(), 0);
	BOOST_CHECK_EQUAL(buf.WriteableBytes(), 1400); // FIXME
	BOOST_CHECK_EQUAL(buf.PrependableBytes(), BufferEx::nCheapPrepend);

	cout << __FUNCTION__ << " success!!!" << endl;
}

void TestBuff3()
{
	BufferEx buf;
	buf.append(string(800, 'y'));
	BOOST_CHECK_EQUAL(buf.ReadableBytes(), 800);
	BOOST_CHECK_EQUAL(buf.WriteableBytes(), BufferEx::nInitialSize - 800);

	buf.Retrieve(500);
	BOOST_CHECK_EQUAL(buf.ReadableBytes(), 300);
	BOOST_CHECK_EQUAL(buf.WriteableBytes(), BufferEx::nInitialSize - 800);
	BOOST_CHECK_EQUAL(buf.PrependableBytes(), BufferEx::nCheapPrepend + 500);

	buf.append(string(300, 'z'));
	BOOST_CHECK_EQUAL(buf.ReadableBytes(), 600);
	BOOST_CHECK_EQUAL(buf.WriteableBytes(), BufferEx::nInitialSize - 600);
	BOOST_CHECK_EQUAL(buf.PrependableBytes(), BufferEx::nCheapPrepend);

	cout << __FUNCTION__ << " success!!!" << endl;
}

void TestBuff4()
{
	BufferEx buf;
	buf.append(string(2000, 'y'));
	BOOST_CHECK_EQUAL(buf.ReadableBytes(), 2000);
	BOOST_CHECK_EQUAL(buf.WriteableBytes(), 0);
	BOOST_CHECK_EQUAL(buf.PrependableBytes(), BufferEx::nCheapPrepend);

	buf.Retrieve(1500);
	BOOST_CHECK_EQUAL(buf.ReadableBytes(), 500);
	BOOST_CHECK_EQUAL(buf.WriteableBytes(), 0);
	BOOST_CHECK_EQUAL(buf.PrependableBytes(), BufferEx::nCheapPrepend + 1500);

	buf.Shrink(0);
	BOOST_CHECK_EQUAL(buf.ReadableBytes(), 500);
	BOOST_CHECK_EQUAL(buf.WriteableBytes(), BufferEx::nInitialSize - 500);
	BOOST_CHECK_EQUAL(buf.RetrieveAllAsString(), string(500, 'y'));
	BOOST_CHECK_EQUAL(buf.PrependableBytes(), BufferEx::nCheapPrepend);
	cout << __FUNCTION__ << " success!!!" << endl;
}

void TestBuff5()
{
	BufferEx buf;
	buf.append(string(100000, 'x'));
	const char* null = nullptr;
	BOOST_CHECK_EQUAL(buf.FindEOL(), null);
	BOOST_CHECK_EQUAL(buf.FindEOL(buf.Peek() + 90000), null);
	cout << __FUNCTION__ << " success!!!" << endl;
}

int main()
{
	TestBuff1();
	TestBuff2();
	TestBuff3();
	TestBuff4();
	TestBuff5();
	return 0;
}
