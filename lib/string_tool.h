#ifndef ___STRING_TOOL_H__
#define ___STRING_TOOL_H__
#include "type_define.h"

class StringTool
{
public:
	//static const string WHITE_SPACE = " \t\r\n\f\v";
	static const int DEFAULT_BUFFER_SIZE = 1024;
public:
	static string& Format(string& resultOut, const char* fmt, ...);
	static string  Format(const char* fmt, ...);
	static string  VFormat(string& resultOut, const char* fmt, va_list argptr, int32 default_size = DEFAULT_BUFFER_SIZE);

	static uint32  StoI(const string& str);
	static void Split(const string& src, const string& delim, std::vector<string >& vecStr);

};

#endif
