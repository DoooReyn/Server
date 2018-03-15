#include<iostream>

#include <boost/function.hpp>
#include <boost/bind.hpp>

using namespace std;

class Foo
{
public:
	void memerFunc(double d, int i, int j)
	{
		cout << d << endl;
		cout << i << endl;
		cout << j << endl;

	}
};

int main()
{
	Foo foo;
	boost::function<void (int, int)> fp = boost::bind(&Foo::memerFunc, &foo, 0.5, _1, _2);
	fp(100, 200);
	boost::function<void (int, int)> fp2 = boost::bind(&Foo::memerFunc, boost::ref(foo), 0.5, _1, _2);
	fp2(100, 200);
	return 0;
}
