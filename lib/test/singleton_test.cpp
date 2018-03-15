#include "singleton.h"

class CTestSingle : public SingletonBase<CTestSingle>
{
public:
	friend class SingletonBase<CTestSingle>;
	void Print() {cout << "Print" << endl;}
	CTestSingle() {cout << "CTestSingle" << endl;}
	~CTestSingle() {cout << "~CTestSingle" << endl;}
};

void TestSingleTone()
{
	CTestSingle::newInstance();
	CTestSingle::getInstance().Print();
	CTestSingle::delInstance();
}

int main(int argc, char const* argv[])
{
	TestSingleTone();
	return 0;
}