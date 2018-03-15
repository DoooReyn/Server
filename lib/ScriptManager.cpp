#include "ScriptManager.h"
#include "logger.h"
#include "string_tool.h"



//---------------------------------------------------------------------------------------
ScriptManager::ScriptManager(ToLuaInitGame pfun)
{
	m_currentid = CurrentThread::Tid();
	DEBUG("init ScriptManager");
	m_pLua = NULL;
	assert(pfun);

#ifdef LUA_VERSION_NUM /* lua 5.1 */
	m_pLua = luaL_newstate();
	luaL_openlibs(m_pLua);

#else
	m_pLua = lua_open();
	luaopen_base(m_pLua);
	luaopen_io(m_pLua);
	luaopen_string(m_pLua);
	luaopen_table(m_pLua);
	luaopen_math(m_pLua);
	luaopen_debug(m_pLua);
#endif

	lua_pushstring(m_pLua, TOLUA_VERSION);
	lua_setglobal(m_pLua, "TOLUA_VERSION");

	lua_pushstring(m_pLua, LUA_VERSION);
	lua_setglobal(m_pLua, "TOLUA_LUA_VERSION");


	(pfun)(m_pLua);
	//tolua_testScript_open(m_pLua);

}



//---------------------------------------------------------------------------------------
ScriptManager::~ScriptManager()
{

	lua_close(m_pLua);

}

//---------------------------------------------------------------------------------------
bool ScriptManager::initLuaPath(const string& path)
{
	if (path.empty())
	{
		return false;
	}

	std::vector<string> files;
	//Helper::getFiles(path.c_str(), "*.lua", files);


	for (uint32 index = 0; index != files.size(); ++index)
	{
		string fileName = files[index];
		bool b = LoadLuaFile(fileName.c_str());
		if (b == false)
		{
			return false;
		}
	}


	return true;
}

//---------------------------------------------------------------------------------------
void ScriptManager::addPath(const std::string&  path)
{
	VecPathList::const_iterator it = m_VecPathList.begin();
	VecPathList::const_iterator itend = m_VecPathList.end();

	for (; it != itend; ++it)
	{
		if ((*it) == path)
		{
			return;
		}
	}

	m_VecPathList.push_back(path);
	return;

}


//---------------------------------------------------------------------------------------
bool ScriptManager::getFileFullPath(const std::string& fileName, std::string& fullPath)
{
	VecPathList::const_iterator it = m_VecPathList.begin();
	VecPathList::const_iterator itend = m_VecPathList.end();

	for (; it != itend; ++it)
	{
		std::string temPath = (*it) + "/" + fileName;
		if (access(temPath.c_str(), 00) != -1)
		{
			fullPath = temPath;
			return true;

		}

	}
	return false;

}

//------------------------------------------------------------------------------------
bool ScriptManager::LoadLuaFile(const char* fileName)
{
	if (fileName == NULL)
	{
		return false;
	}
	uint32 top = lua_gettop(m_pLua);

	if (luaL_loadfile(m_pLua, fileName) != 0 || lua_pcall(m_pLua, 0, 0, 0) != 0)
	{
		std::string errMsg = lua_tostring(m_pLua, -1);
		lua_settop(m_pLua, top);

		//char err[1024]={0};
		//snprintf(err,1024,"loadLuaFile error,file is %s,%s",fileName,errMsg.c_str());
		ERROR("%s", errMsg.c_str());
		return false;
	}

	uint32 temtop = lua_gettop(m_pLua);
	if (temtop != top)
	{
		lua_settop(m_pLua, top);
		ERROR("loadLuaFile Change lua top:%s", fileName);
	}


	return true;
}



//---------------------------------------------------------------------------------------
bool  ScriptManager::hasLuaFunction(const char* fun)
{
	if (fun == NULL)
	{
		return false;
	}

	uint32 top = lua_gettop(m_pLua);

	lua_getglobal(m_pLua, fun);
	bool isfun = lua_isfunction(m_pLua, -1);

	lua_settop(m_pLua, top);
	return isfun;

}

//---------------------------------------------------------------------------------------
bool  ScriptManager::CallLuaFuction(const char* func, const char* sig, ...)
{
	int32 dwThreadId = CurrentThread::Tid();
	if (dwThreadId != m_currentid)
	{
		ERROR("%s: multi thread call", func);
		return false;
	}

	va_list vl;

	va_start(vl, sig);
	uint32 callnumber = 0;
	if (m_pLua == NULL)
	{
		va_end(vl);
		return false;
	}
	callnumber = lua_gettop(m_pLua);
	uint32 top = callnumber;
	try
	{

		uint32 narg, nres;
		if (sig == NULL)
		{
			va_end(vl);
			return false;
		}

		std::vector<std::string> temParament;
		StringTool::Split(sig, "@", temParament);
		uint32 parasize = temParament.size();

		lua_getglobal(m_pLua, func);

		if (!lua_isfunction(m_pLua, -1))
		{
			lua_settop(m_pLua, top);
			ERROR("ScriptManager::CallLuaFuction failed can't find function : %s", func);
			va_end(vl);
			return false;
		}

		narg = 0;
		uint32 paraindex = 0;
		bool hasreturn = false;///是否有返回值
		for (paraindex = 0; paraindex < parasize; ++paraindex)
		{
			if (temParament[paraindex] == "d")
			{
				lua_pushnumber(m_pLua, va_arg(vl, double));
			}
			else if (temParament[paraindex] == "l")
			{
				lua_pushnumber(m_pLua, va_arg(vl, long));
			}
			else if (temParament[paraindex] == "i")
			{
				lua_pushnumber(m_pLua, va_arg(vl, int));
			}
			else if (temParament[paraindex] == "s")
			{
				lua_pushstring(m_pLua, va_arg(vl, char*));
			}
			else if (temParament[paraindex] == ":")
			{
				hasreturn = true;
				break;
			}
			else
			{
				tolua_pushusertype(m_pLua, va_arg(vl, void*), temParament[paraindex].c_str());
			}
			++narg;
			lua_checkstack(m_pLua, 1);
		}


		///计算返回值的多少
		nres = parasize - narg;
		if (hasreturn)
		{
			--nres;
			++paraindex;
		}

		if (lua_pcall(m_pLua, narg, nres, 0) != 0)
		{
			string errMsg = lua_tostring(m_pLua, -1);
			string message = " ScriptManager::CallLuaFuction failed can't find function : ";
			message += func;
			message += " error:";
			message += errMsg;
			ERROR("%s", message.c_str());
			lua_settop(m_pLua, top);
			va_end(vl);
			return false;
		}
		nres = -nres;

		for (; paraindex < parasize; ++paraindex)
		{
			if (temParament[paraindex] == "d")
			{
				if (!lua_isnumber(m_pLua, nres))
				{
					lua_settop(m_pLua, top);
					va_end(vl);

					ERROR(" ScriptManager::CallLuaFuction failed can't find param d: %s", func);

					return false;
				}

				*va_arg(vl, double*) = lua_tonumber(m_pLua, nres);

			}
			else if (temParament[paraindex] == "f")
			{
				if (!lua_isnumber(m_pLua, nres))
				{
					lua_settop(m_pLua, top);
					va_end(vl);
					ERROR(" ScriptManager::CallLuaFuction failed can't find param  f: %s", func);
					return false;
				}

				*va_arg(vl, float*) = (float)lua_tonumber(m_pLua, nres);

			}
			else if (temParament[paraindex] == "l")
			{
				if (!lua_isnumber(m_pLua, nres))
				{
					lua_settop(m_pLua, top);
					va_end(vl);
					ERROR(" ScriptManager::CallLuaFuction failed can't find param : %s l", func);
					return false;
				}

				*va_arg(vl, long*) = (long)lua_tonumber(m_pLua, nres);
			}
			else if (temParament[paraindex] == "i")
			{

				if (!lua_isnumber(m_pLua, nres))
				{
					lua_settop(m_pLua, top);
					va_end(vl);
					ERROR(" ScriptManager::CallLuaFuction failed can't find param  i: %s", func);
					return false;
				}

				*va_arg(vl, int*) = (int)lua_tonumber(m_pLua, nres);

			}
			else if (temParament[paraindex] == "s")
			{

				if (!lua_isstring(m_pLua, nres))
				{
					lua_settop(m_pLua, top);
					va_end(vl);
					ERROR(" ScriptManager::CallLuaFuction failed can't find param s: %s", func);
					return false;
				}
				*va_arg(vl, const char**) = lua_tostring(m_pLua, nres);

			}
			else
			{
				tolua_Error tolua_err;
				if (!tolua_isusertype(m_pLua, nres, temParament[paraindex].c_str(), 0, &tolua_err))
				{
					lua_settop(m_pLua, top);
					va_end(vl);

					ERROR("ScriptManager::CallLuaFuction failed can't find param t : %s", func);
					return false;
				}
				*va_arg(vl, const void**) = tolua_tousertype(m_pLua, nres, 0);
			}
			++nres;
		}

		va_end(vl);
		lua_settop(m_pLua, top);
		return true;
	}
	catch (...)
	{

		uint32 affertCallnumber = lua_gettop(m_pLua);
		if (affertCallnumber > callnumber)
		{
			ERROR(" ScriptManager::CallLuaFuction Callnumber increase :   function:%s callnumber: %d", func, affertCallnumber);
		}

		lua_settop(m_pLua, top);
		va_end(vl);
		return false;
	}
	uint32 affertCallnumber = lua_gettop(m_pLua);
	if (affertCallnumber > callnumber)
	{
		ERROR("ScriptManager::CallLuaFuction Callnumber increase :   function:%s callnumber: %d", func, affertCallnumber);
	}
	return true;
}


//---------------------------------------------------------------------------------------
bool ScriptManager::GetInt(const char* field, int* ret)
{
	uint32 top = lua_gettop(m_pLua);
	lua_getglobal(m_pLua, field);
	if (lua_isnumber(m_pLua, -1))
	{
		*ret = (int)lua_tonumber(m_pLua, -1);
		lua_settop(m_pLua, top);

		return true;
	}
	else
	{
		lua_settop(m_pLua, top);

		return false;
	}
}


//---------------------------------------------------------------------------------------
bool ScriptManager::GetFloat(const char* field, float* ret)
{
	uint32 top = lua_gettop(m_pLua);
	lua_getglobal(m_pLua, field);
	if (lua_isnumber(m_pLua, -1))
	{
		*ret = (float)lua_tonumber(m_pLua, -1);
		lua_settop(m_pLua, top);

		return true;
	}
	else
	{
		lua_settop(m_pLua, top);

		return false;
	}
}


//---------------------------------------------------------------------------------------
bool ScriptManager::GetString(const char* field, const char** ret)
{
	uint32 top = lua_gettop(m_pLua);
	lua_getglobal(m_pLua, field);
	if (lua_isstring(m_pLua, -1))
	{
		*ret = lua_tostring(m_pLua, -1);
		lua_settop(m_pLua, top);

		return true;
	}
	else
	{
		lua_settop(m_pLua, top);

		return false;
	}
}
