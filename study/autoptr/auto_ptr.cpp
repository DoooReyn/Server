#include <iostream>
#include <memory>
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

void TestAutoPtr()
{
	std::auto_ptr<Simple> my_memory(new Simple(1));   // 创建对象，输出：Simple：1
	if (my_memory.get())                              // 判断智能指针是否为空
	{
		my_memory->PrintSomething();                    // 使用 operator-> 调用智能指针对象中的函数
		my_memory.get()->info_extend = "Addition";      // 使用 get() 返回裸指针，然后给内部对象赋值
		my_memory->PrintSomething();                    // 再次打印，表明上述赋值成功
		(*my_memory).info_extend += " other";           // 使用 operator* 返回智能指针内部对象，然后用“.”调用智能指针对象中的函数
		my_memory->PrintSomething();                    // 再次打印，表明上述赋值成功
	}
}

void TestAutoPtr2()
{
	std::auto_ptr<Simple> my_memory(new Simple(2));
	if (my_memory.get())
	{
		std::auto_ptr<Simple> my_memory2;   // 创建一个新的 my_memory2 对象
		my_memory2 = my_memory;             // 复制旧的 my_memory 给 my_memory2
		my_memory2->PrintSomething();       // 输出信息，复制成功
		my_memory->PrintSomething();        // 崩溃  复制后控制权消失了
	}
}

void TestAutoPtr3()
{
	std::auto_ptr<Simple> my_memory(new Simple(3));

	if (my_memory.get())
	{
		//my_memory.release();                //release 交出控制权 不会释放空间
		//Simple* temp_memory = my_memory.release();
		//delete temp_memory;
		//
		my_memory.reset();                    //释放空间
	}
}

int main()
{
	TestAutoPtr();
	//TestAutoPtr2();
	TestAutoPtr3();
	return 0;
}
