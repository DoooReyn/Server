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

void TestScopedArray()
{
	boost::scoped_array<Simple> my_memory(new Simple[2]); // 使用内存数组来初始化
	if (my_memory.get())
	{
		my_memory[0].PrintSomething();
		my_memory.get()[0].info_extend = "Addition";
		my_memory[0].PrintSomething();
		//(*my_memory)[0].info_extend += " other";            // 编译 error，scoped_ptr 没有重载 operator*
		//my_memory[0].release();                             // 同上，没有 release 函数
		//boost::scoped_array<Simple> my_memory2;
		//my_memory2 = my_memory;                             // 编译 error，同上，没有重载 operator=
	}
}


int main()
{
	TestScopedArray();
	return 0;
}
