#include "regex_parse.h"

Regex::Regex()
{
	m_compiled = false;
	m_matched = false;
}

Regex::~Regex()
{
	if(m_compiled)
	{
		regfree(&m_rgx);
	}
}


bool Regex::Compile(const char* szRegex, int nFlags)
{
	if(m_compiled)
	{
		regfree(&m_rgx);
	}
	m_compiled = false;
	m_matched = false;
	int nRet = regcomp(&m_rgx, szRegex, nFlags);
	if(nRet == 0)
	{
		m_compiled = true;
	}
	return m_compiled;
}

bool Regex::Match(const char* szMatch)
{
	if(szMatch == NULL)
	{
		return false;
	}
	if(m_compiled == false)
	{
		return false;
	}

	m_smatch  = szMatch;
	int nRet = regexec(&m_rgx, szMatch, 32, m_rgm, 0);
	if(nRet == 0)
	{
		m_matched = true;
	}
	return m_matched;
}

bool Regex::Match_Mt(const char* szMatch)
{
	if(szMatch == NULL)
	{
		return false;
	}
	if(m_compiled == false)
	{
		return false;
	}

	return (0 == regexec(&m_rgx, szMatch, 32, m_rgm, 0));
}

string& Regex::GetSub(string& str, int nSub)
{
	if(m_matched)
	{
		if(nSub >= 32 || nSub < 0)
		{
			str = "";
		}
		else
		{
			str = string(m_smatch, m_rgm[nSub].rm_so, m_rgm[nSub].rm_eo - m_rgm[nSub].rm_so);
		}
	}
	return str;
}