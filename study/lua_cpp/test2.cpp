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

	lua_getglobal(L, "add");        // ��ȡ������ѹ��ջ��
	lua_pushnumber(L, 10);          // ѹ���һ������
	lua_pushnumber(L, 20);          // ѹ��ڶ�������
	int iRet = lua_pcall(L, 2, 1, 0);// ���ú�������������Ժ󣬻Ὣ����ֵѹ��ջ�У�2��ʾ����������1��ʾ���ؽ��������
	if (iRet)                       // ���ó���
	{
		const char* pErrorMsg = lua_tostring(L, -1);
		cout << pErrorMsg << endl;
		lua_close(L);
		return 0;
	}
	if (lua_isnumber(L, -1))        //ȡֵ���
	{
		double fValue = lua_tonumber(L, -1);
		cout << "Result is " << fValue << endl;
	}

	//���ˣ�ջ�е�����ǣ�
	//=================== ջ�� ===================
	//  ����  ����      ֵ
	//   5   int��      30
	//   4   int��		20114442
	//   3   string��   shun
	//   2   table:     tbl
	//   1   string:    I am so cool~
	//=================== ջ�� ===================

	//7.�ر�state
	lua_close(L);
	return 0;

}
