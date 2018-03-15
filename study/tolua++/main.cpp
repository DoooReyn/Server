#include <stdio.h>
#include "lua_student.h"

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "luaconf.h"
}
#include "tolua++.h"
#include"student.h"


int main(int argc, char* argv[])
{
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);

	tolua_student_open(L);

	luaL_dofile(L, "./test.lua");
	lua_close(L);

	return 0;
}
