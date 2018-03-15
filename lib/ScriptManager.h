#ifndef __SCRIPT_MANAGER_H__
#define __SCRIPT_MANAGER_H__

#include "type_define.h"
#include "singleton.h"

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "luaconf.h"
}
#include "tolua++.h"

typedef int32(*ToLuaInitGame)(lua_State*);

class ScriptManager : public SingletonBase<ScriptManager>
{

public:
	explicit ScriptManager(ToLuaInitGame pfun);



	~ScriptManager();


	/**加入一个lua文件搜所路径
	@warn path 路径后面不需要
	*/
	void addPath(const std::string&  path);


	/**获取一个文件的全路径，如果没找到返回false*/
	bool getFileFullPath(const std::string& fileName, std::string& fullPath);



	///执行脚本文件
	bool  LoadLuaFile(const char* fileName);


	/**
	*@param func lua函数名
	*@ sig 输入输出数据标准 i@f@class@:@d@l,，标式：左边为输入类型和数量。右边为返回类型和数量
	*用@分格开。如果是用户自定义类直接用类名。如果是标准类型用d,f,l,i,s,
	*/
	//输入参数直接用参数名，返回的值要要地址接收
	bool CallLuaFuction(const char* func, const char* sig, ...);


	/**判断是否有指定的lua函数名*/
	bool hasLuaFunction(const char* fun);


	/**获取lua里指定名字的int32类型
	*@param field lua变量名
	*@param ret 返回获取的值
	*@return 获取成功返回真。失败返回假
	*/
	bool GetInt(const char* field, int32* ret);


	/**@see getint32*/
	bool GetFloat(const char* field, float* ret);



	/**@see getint32*/
	bool GetString(const char* field, const  char** ret);



	/**获取luastate;
	*/
	lua_State* getLuaState()
	{
		return m_pLua;
	}


	/**初始化指定文件夹下的所有lua函数
	*/
	bool initLuaPath(const string& path);
protected:

	typedef std::vector<std::string> VecPathList;
	lua_State* m_pLua;				//lua系统接口对象
	VecPathList   m_VecPathList;
	int32 m_currentid;
};

#endif