#include<iostream>
#include <iomanip>
using namespace std;

#include"Node.h"

void ModifyNode(STUDENT* pNode)
{
	cout << "SNO:";
	InputStr(pNode->szNo);

	cout << "NAME:";
	InputStr(pNode->szName);

	cout << "SEX(M:man/W:woman):";
	InputStr(pNode->szSex);

	while(1)
	{
		cout << "AGE:";
		if(InputInt(&(pNode->nAge)))
		{
			break;
		}
		cout << "input error!" << endl;
	}
	for(int i = 0; i < 1; i++)
	{
		cout << "SUBJECTS:";
		InputStr(pNode->szCourse[i]);
	}
}

void InputNode(STUDENT* pNode)
{
	ModifyNode(pNode);
	pNode->pNext = NULL;
}

void ShowNode(STUDENT* pNode)
{
	cout << "+-------------+----------+------+----+-------------------+" << endl;
	cout << "|" << setw(13) << pNode->szNo << "|" << setw(10) << pNode->szName;
	if(StrComp(pNode->szSex, "M"))
	{
		cout << setw(7) << "|man";
	}
	else
	{
		cout << setw(7) << "|woman";
	}
	cout << "|" << setw(4) << pNode->nAge;
	for(int i = 0; i < 1; i++)
	{
		cout << "|" << setw(19) << pNode->szCourse[i];
	}
	cout << "|" << endl;
}
