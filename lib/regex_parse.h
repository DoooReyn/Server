#ifndef __REGEX_PARSE_H__
#define __REGEX_PARSE_H__
#include <regex.h>
#include "type_define.h"

class Regex
{
public:
	Regex();
	~Regex();

	bool Compile(const char* szRegex, int nFlags = REG_EXTENDED | REG_NEWLINE);
	bool Match(const char* szMatch);
	bool Match_Mt(const char* szMatch);
	string& GetSub(string& str, int nSub = 0);
private:
	regex_t m_rgx;//regex句柄
	regmatch_t m_rgm[32]; //最多匹配32个
	string m_smatch; //匹配字符串
	bool m_compiled;
	bool m_matched;
};

#endif
