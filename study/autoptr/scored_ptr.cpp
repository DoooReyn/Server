#include <iostream>
#include <boost/smart_ptr.hpp>

using namespace std;

class Simple
{
public:
	Simple(int param = 0)
	{
		number = param;
		std::cout << "Simple: " << number << std::endl;
	}

	~Simple()
	{
		std::cout << "~Simple: " << number << std::endl;
	}

	void PrintSomething()
	{
		std::cout << "PrintSomething: " << info_extend.c_str() << std::endl;
	}

	std::string info_extend;
	int number;
};

void TestScopedPtr()
{
	boost::scoped_ptr<Simple> my_memory(new Simple(1));
	if (my_memory.get())
	{
		my_memory->PrintSomething();
		my_memory.get()->info_extend = "Addition";
		my_memory->PrintSomething();
		(*my_memory).info_extend += " other";
		my_memory->PrintSomething();

		//my_memory.release();           // 编译 error: scoped_ptr 没有 release 函数
		//std::auto_ptr<Simple> my_memory2;
		//my_memory2 = my_memory;        // 编译 error: scoped_ptr 没有重载 operator=，不会导致所有权转移
	}
}

int main()
{
	TestScopedPtr();
	return 0;
}
