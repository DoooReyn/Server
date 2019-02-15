#include "mysql_pool.h"
#include "logger.h"

void TestMysqlPool()
{
	MysqlPool obj(111);
	string str = "mysql://root:123456@127.0.0.1:3306/LBF";
	bool bRet = obj.PutUrl(str.c_str(), 1);
	if (bRet)
	{
		cout << "mysql init success" << endl;
	}

	AutoHandle handle(&obj, 1);
	if (!handle())
	{
		printf("不能从数据库连接池获取连接句柄\n");
		return ;
	}

	string strSql = "select * from SERVERLIST";
	DataSet ret_set;
	handle()->Select(strSql.c_str(), strSql.length(), ret_set);


	for (uint32 i = 0 ; i < ret_set.Size() ; i++)
	{
		int a1 = ret_set.GetValue(i, "ID");
		int a2 = ret_set.GetValue(i, "TYPE");
		int a3 = ret_set.GetValue(i, "DYNAMIC");
		const char* b1 = ret_set.GetValue(i, "NAME");
		const char* b2 = ret_set.GetValue(i, "IP");
		int a4 = ret_set.GetValue(i, "PORT");
		const char* b3 = ret_set.GetValue(i, "EXTIP");
		int a5 = ret_set.GetValue(i, "EXTPORT");
		int a6 = ret_set.GetValue(i, "NETTYPE");
		printf("%d\t%d\t%d\t%s\t%s\t%d\t%s\t%d\t%d\n", a1, a2, a3, b1, b2, a4, b3, a5, a6);
	}

	string strSql2 = "select FunctionTest(7,8) as result";
	DataSet ret_set2;
	handle()->Select(strSql2.c_str(), strSql2.length(), ret_set2);
	for (uint32 i = 0 ; i < ret_set2.Size() ; i++)
	{
		int result = ret_set2.GetValue(i, "result");
		printf("test Function :%d\n", result);
	}
	printf("====================\n");

	string strSql3 = "call ProductTest(7,8,@add,@sub)";
	string resultSql3 = "select @add,@sub";
	DataSet ret_set3;
	handle()->ExecProduct(strSql3,resultSql3,ret_set3);
	for (uint32 i = 0 ; i < ret_set3.Size() ; i++)
	{
		int result = ret_set3.GetValue(i, "@add");
		int result2 = ret_set3.GetValue(i, "@sub");
		printf("test Product :%d %d\n", result,result2);
	}
	printf("====================\n");

}

int main(int argc, char const* argv[])
{
	InitLogger("/root/oschina/Server/lib/test/log/test.log", "debug");
	TestMysqlPool();
	return 0;
}
