#include<stdio.h>
#include"String.h"

int StrLen(const char* pstr)
{
	int nSize = 0;
	while(*pstr++ != '\0')
	{
		nSize++;
	}
	return nSize;
}

char* StrCopy(char* pDes, const char* psrc)
{
	if(NULL != pDes)
	{
		if(NULL != psrc)
		{
			int nSize = StrLen(psrc) + 1;
			for(int i = 0; i < nSize; i++)
			{
				pDes[i] = psrc[i];
			}
		}
	}
	return pDes;
}

bool StrComp(const char* pStr1, const char* pStr2)
{
	if(NULL != pStr2 && NULL != pStr2)
	{
		int nSize = StrLen(pStr1);
		if(nSize != StrLen(pStr2))
		{
			return false;
		}
		for(int i = 0; i < nSize; i++)
		{
			if(pStr1[i] != pStr2[i])
			{
				return false;
			}
		}
		return true;
	}
	return false;
}
