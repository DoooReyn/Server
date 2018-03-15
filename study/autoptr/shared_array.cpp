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

void TestSharedArray(boost::shared_array<Simple> memory)    // 注意：无需使用 reference (或 const reference)
{
	std::cout << "TestSharedArray UseCount: " << memory.use_count() << std::endl;
}

void TestSharedArray2()
{
	boost::shared_array<Simple> my_memory(new Simple[2]);
	if (my_memory.get())
	{
		my_memory[0].PrintSomething();
		my_memory.get()[0].info_extend = "Addition 00";
		my_memory[0].PrintSomething();
		my_memory[1].PrintSomething();
		my_memory.get()[1].info_extend = "Addition 11";
		my_memory[1].PrintSomething();
		//(*my_memory)[0].info_extend += " other";  // 编译 error，scoped_ptr 没有重载 operator*
	}
	std::cout << "TestSharedArray2 UseCount: " << my_memory.use_count() << std::endl;
	TestSharedArray(my_memory);
	std::cout << "TestSharedArray2 UseCount: " << my_memory.use_count() << std::endl;
}

int main()
{
	TestSharedArray2();
	return 0;
}
