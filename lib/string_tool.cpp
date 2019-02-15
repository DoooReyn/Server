#include "string_tool.h"

string& StringTool::Format(string& resultOut, const char* fmt, ...)
{
	va_list vl;
	va_start(vl, fmt);
	VFormat(resultOut, fmt, vl);
	va_end(vl);
	return resultOut;
}
string  StringTool::Format(const char* fmt, ...)
{

	va_list vl;
	va_start(vl, fmt);
	string resultOut;
	VFormat(resultOut, fmt, vl);
	va_end(vl);
	return resultOut;
}


string  StringTool::VFormat(string& resultOut, const char* fmt, va_list argptr, int32 default_size /*= DEFAULT_BUFFER_SIZE*/)
{
	vector<char> buffer(default_size, 0);
	while(true)
	{
		int32 nLen = vsnprintf(&(buffer[0]), buffer.size(), fmt, argptr);
		if(nLen == -1)
		{
			buffer.resize(buffer.size() * 2);
			continue;
		}
		if(nLen == static_cast<int32>(buffer.size()))
		{
			buffer.push_back(0);
		}
		break;
	}
	resultOut.assign(&(buffer[0]));
	return resultOut;
}

void StringTool::Split(const string& src, const string& delim, std::vector<string >& vecStr)
{
	size_t last = 0;
	size_t index = src.find_first_of(delim, last);
	while (index != std::string::npos)
	{
		vecStr.push_back(src.substr(last, index - last));
		last = index + 1;
		index = src.find_first_of(delim, last);
	}
	if (index - last > 0)
	{
		vecStr.push_back(src.substr(last, index - last));
	}
}
