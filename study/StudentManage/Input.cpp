#include<stdio.h>
#include"Input.h"


void InputStr(char* pstr)
{
	if(NULL != pstr)
	{
		char ch = '\0';
		int i = 0;
		for(; (ch = getchar()) != '\n'; i++)
		{
			pstr[i] = ch;
		}
		pstr[i] = '\0';
	}
}

int InputInt(int* pNum)
{
	if(NULL != pNum)
	{
		char szBuf[10] = "";
		InputStr(szBuf);

		return AtoI(szBuf, pNum);
	}
	return false;
}

bool AtoI(const char* pstr, int* pNum)
{
	if(NULL != pstr && NULL != pNum)
	{
		int nSign = 1;
		int num = 0;
		while(*pstr == ' ' || *pstr == '\t')
		{
			pstr++;
		}
		if(*pstr == '-')
		{
			nSign = -1;
			pstr++;
		}

		while(*pstr)
		{
			if (*pstr >= '0' && *pstr <= '9')
			{
				num = 10 * num + *pstr - '0';
			}
			else
			{
				return false;
			}
			pstr++;

		}
		*pNum = num * nSign;

		return true;
	}
	return false;

}
