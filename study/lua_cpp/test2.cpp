#include <iostream>
#include <string>
using namespace std;

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

int main()
{
	lua_State* L = luaL_newstate();
	if (L == NULL)
	{
		return 0;
	}

	int bRet = luaL_loadfile(L, "hello.lua");
	if (bRet)
	{
		cout << "load file error" <<  endl;
		return 0;
	}

	bRet = lua_pcall(L, 0, 0, 0);
	if(bRet)
	{
		cout << "pcall error" <<  endl;
		return 0;
	}
	lua_getglobal(L, "str");
	string str = lua_tostring(L, -1);
	cout << "str = " << str.c_str() << endl;

	lua_getglobal(L, "tbl");
	lua_getfield(L, -1, "id");
	lua_getfield(L, -2, "name");
	str = lua_tostring(L, -1);
	int id  = lua_tonumber(L, -2);
	cout << "tbl:name = " << str.c_str() << " id = " << id << endl;
	lua_pushstring(L, "asdfasdfasdfasdfasdf");
	lua_setfield(L, 2, "name");

	lua_getfield(L, 2, "name");
	str = lua_tostring(L, -1);
	cout << "tbl:name = " << str.c_str() << endl;

	lua_getglobal(L, "add");        // 获取函数，压入栈中
	lua_pushnumber(L, 10);          // 压入第一个参数
	lua_pushnumber(L, 20);          // 压入第二个参数
	int iRet = lua_pcall(L, 2, 1, 0);// 调用函数，调用完成以后，会将返回值压入栈中，2表示参数个数，1表示返回结果个数。
	if (iRet)                       // 调用出错
	{
		const char* pErrorMsg = lua_tostring(L, -1);
		cout << pErrorMsg << endl;
		lua_close(L);
		return 0;
	}
	if (lua_isnumber(L, -1))        //取值输出
	{
		double fValue = lua_tonumber(L, -1);
		cout << "Result is " << fValue << endl;
	}

	//至此，栈中的情况是：
	//=================== 栈顶 ===================
	//  索引  类型      值
	//   5   int：      30
	//   4   int：		20114442
	//   3   string：   shun
	//   2   table:     tbl
	//   1   string:    I am so cool~
	//=================== 栈底 ===================

	//7.关闭state
	lua_close(L);
	return 0;

}
