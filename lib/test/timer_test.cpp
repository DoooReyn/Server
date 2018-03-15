#include "timer.h"
#include "helper.h"


class A
{
public:
	virtual void Print()
	{
		//cout << " A::Print" << endl;
		a++;
	}

	virtual void Print2()
	{
		//cout << " A::Print" << endl;
	}


	virtual void Print3()
	{
		//cout << " A::Print" << endl;
	}
	int  a;
};

class B : public A
{
public:
	void Print()
	{
		//cout << " B::Print" << endl;
		a++;
	}
};

typedef std::function<void()> TestFunc;
void TestVirtual()
{
	A* obj = new A;
	A* obj2 = new B;
	TestFunc testfun2 = std::bind(&A::Print, obj);
	TestFunc testfun = std::bind(&A::Print, obj2);


	int64 time1 = Timestamp::Now().GetMicroSeconds();
	for (int32 i = 0 ; i < 10000000; i++)
	{
		obj->Print();
		obj2->Print();
		//testfun2();
		//testfun();
	}
	int64 time2 = Timestamp::Now().GetMicroSeconds();

	cout << time2 - time1 << endl;
}


void TestTimer()
{
	Time t2;
	Time t1;
	char Buffer[20] = {};
	t1.Format(Buffer, 20);
	cout << "Time:" << Buffer << endl;

	t1.AddDelay(1000L * 10);
	t1.Format(Buffer, 20);
	cout << "Time:" << Buffer << endl;

	cout << t2.Elapse(t1) << endl;;

	Timer t3(1000L);
	uint64 count = 0;
	while (1)
	{
		Time t4;
		if (t3(t4))
		{
			t4.Format(Buffer, 20);
			cout << "Time:" << Buffer << endl;
			cout << "count:" << count << endl;
			count++;
		}

		if (count == 3)
		{
			break;
		}
	}
	Clocker clocker(16 * 3600 + 19 * 60, 24 * 3600);
	uint64 escape = clocker.LeftSec(Time());
	cout << "escape:" << escape / 3600 << ":" << (escape % 3600) / 60 << endl;
}

int main(int argc, char const* argv[])
{

	TestVirtual();
	return 0;
	//TestTimer();
	time_t Now = time(NULL);
	time_t Now1 = Now - 12 * 3600;
	time_t Now2 = Now - 8 * 3600;
	time_t Now3 = Now + 10 * 3600;
	time_t Now4 = Now + 12 * 3600;

	cout << Now << endl;
	cout << Now1 << endl;
	cout << Now2 << endl;
	cout << Now3 << endl;
	cout << Now4 << endl;

	cout << IsSameDay(Now, Now1) << " " << Diffday(Now, Now1) << " " << Diffday(Now, Now1, false) << endl;
	cout << IsSameDay(Now, Now2) << " " << Diffday(Now, Now2) << " " << Diffday(Now, Now2, false) << endl;
	cout << IsSameDay(Now, Now3) << " " << Diffday(Now, Now3) << " " << Diffday(Now, Now3, false) << endl;
	cout << IsSameDay(Now, Now4) << " " << Diffday(Now, Now4) << " " << Diffday(Now, Now4, false) << endl;
	cout << "timezone :" << timezone << endl;

	cout << "======================" << endl;

	cout << IsSameDay(Now1, Now2) << " " << Diffday(Now1, Now2) << " " << Diffday(Now1, Now2, false) << endl;
	cout << IsSameDay(Now1, Now3) << " " << Diffday(Now1, Now3) << " " << Diffday(Now1, Now3, false) << endl;
	cout << IsSameDay(Now1, Now4) << " " << Diffday(Now1, Now4) << " " << Diffday(Now1, Now4, false) << endl;
	cout << "======================" << endl;

	cout << IsSameDay(Now2, Now3) << " " << Diffday(Now2, Now3) << " " << Diffday(Now2, Now3, false) << endl;
	cout << IsSameDay(Now2, Now4) << " " << Diffday(Now2, Now4) << " " << Diffday(Now2, Now4, false) << endl;
	cout << "======================" << endl;
	cout << IsSameDay(Now3, Now4) << " " << Diffday(Now3, Now4) << " " << Diffday(Now3, Now4, false) << endl;
	cout << "======================" << endl;


	/*
	for(int32 i = 0; i < 10; i++)
	{
		cout << "rand :" << RandBetween(1, 2) << endl;
	}


	for(int32 i = 0; i < 10; i++)
	{
		std::vector<int32> nums;
		RandNumInRegion(10, 4, nums);

		//sort(nums.begin(),nums.end(),less<int32>());
		for(uint32 j = 0; j < nums.size(); j++)
		{
			cout << nums[j] << " ";
		}
		cout << endl;
	}
	*/
	time_t nowTime = time(NULL);
	cout << nowTime << endl;
	string strTime;
	UTCToStr(nowTime, strTime);
	cout << strTime << endl;
	time_t nowTime2 = StrTimeToUTC(strTime);
	cout << nowTime2 << endl;

	vector<string> vecFile;
	ReadDir("..", vecFile);
	for (uint32 i = 0; i < vecFile.size(); i++)
	{
		cout << vecFile[i] << endl;
	}

	return 0;
}
