#include<iostream>
#include <iomanip>
using namespace std;
#include <stdio.h>
#include"List.h"

STUDENT* g_pHead = NULL;

bool CreateNode()
{
	if(g_pHead == NULL)
	{
		PSTUDENT pTemp = new STUDENT;
		if(NULL != pTemp)
		{
			InputNode(pTemp);
			g_pHead = pTemp;
			return true;
		}
	}
	return false;

}
bool AddNode()
{
	if(NULL != g_pHead)
	{
		PSTUDENT pTemp = g_pHead;
		PSTUDENT pNewNode = new STUDENT;
		if(NULL != pNewNode)
		{
			while(NULL != pTemp->pNext)
			{
				pTemp = pTemp->pNext;
			}
			InputNode(pNewNode);
			pTemp->pNext = pNewNode;
		}
		return true;
	}
	else
	{
		return false;
	}

}

bool DeleteNode()
{
	if(NULL != g_pHead)
	{
		PSTUDENT pLast = g_pHead;
		PSTUDENT pCurrent = g_pHead;
		cout << "Input the delete SNO:";

		char szNo[13] = "";
		InputStr(szNo);
		while(NULL != pCurrent)
		{
			if(StrComp(pCurrent->szNo, szNo))
			{
				if(pCurrent == g_pHead)
				{
					g_pHead = pCurrent->pNext;
				}
				else
				{
					pLast->pNext = pCurrent->pNext;
				}
				delete pCurrent;
				pCurrent = NULL;
				return true;
			}
			else
			{
				pLast = pCurrent;
				pCurrent = pCurrent->pNext;
			}
		}
		return false;
	}
	else
	{
		return false;
	}
}
bool FindNode()
{
	if(NULL != g_pHead)
	{
		PSTUDENT pTemp = g_pHead;
		cout << "Input the delete SNO:";

		char szNo[13] = "";
		InputStr(szNo);
		while(NULL != pTemp)
		{
			if(StrComp(pTemp->szNo, szNo))
			{
				ShowNode(pTemp);
				return true;
			}
			pTemp = pTemp->pNext;
		}
		return false;

	}
	else
	{
		return false;
	}

}
bool ModifyList()
{
	if(NULL != g_pHead)
	{
		PSTUDENT pCurrent = g_pHead;
		cout << "Input the Modify SNO:";

		char szNo[13] = "";
		InputStr(szNo);
		while(NULL != pCurrent)
		{
			if(StrComp(pCurrent->szNo, szNo))
			{
				ShowNode(pCurrent);
				ModifyNode(pCurrent);
				return true;
			}
			pCurrent = pCurrent->pNext;
		}


	}

	return false;

}
bool InsertNode()
{
	if(NULL != g_pHead)
	{
		cout << "Input The Insert Postion SNO:";

		char szNo[13] = "";
		InputStr(szNo);

		PSTUDENT pTemp = g_pHead;
		PSTUDENT pLast = g_pHead;

		while(NULL != pTemp)
		{
			if(StrComp(pTemp->szNo, szNo))
			{
				PSTUDENT pNewNode = new STUDENT;
				InputNode(pNewNode);
				if(NULL == pTemp->pNext)
				{
					pTemp->pNext = pNewNode;
				}
				else
				{
					pNewNode->pNext = pTemp->pNext;
					pLast->pNext = pNewNode;
				}
				return true;
			}
			pLast = pTemp;
			pTemp = pTemp->pNext;
		}

	}
	return false;
}
bool ClearList()
{
	if(NULL != g_pHead)
	{
		STUDENT* pDel = NULL;
		while(NULL != g_pHead)
		{
			pDel = g_pHead;
			g_pHead = g_pHead->pNext;

			delete pDel;
			pDel = NULL;
		}
		return true;

	}
	return false;
}
bool DisplayList()
{
	if(NULL != g_pHead)
	{
		STUDENT* pTemp = g_pHead;

		cout << "+-------------+----------+------+----+-------------------+" << endl;
		cout << setw(14) << "|SNO" << setw(11) << "|Name" << setw(7) << "|Sex" << setw(5) << "|Age" << setw(20) << "|SubJects" << "|" << endl;
		while(pTemp != NULL)
		{
			ShowNode(pTemp);
			pTemp = pTemp->pNext;
		}
		cout << "+-------------+----------+------+----+-------------------+" << endl;
		return true;
	}

	return false;
}
bool LoadFile()
{
	FILE* fp = NULL;
	if ((fp = fopen("Student.txt", "rb")) == NULL)
	{
		printf("can not open file\n");
		return false;
	}
	STUDENT* pTemp, *pOld;
	int nCount = 0;
	cout << endl << "Load from file..." << endl;
	pTemp = new STUDENT;
	g_pHead = pTemp;
	pOld = g_pHead;
	while(!feof(fp))
	{
		if(1 != fread(pTemp, sizeof(STUDENT), 1, fp))
		{
			break;
		}
		nCount++;
		pTemp->pNext = new STUDENT;
		pOld = pTemp->pNext;
		pOld = pTemp;
		pTemp = pTemp->pNext;
	}
	pOld->pNext = NULL;
	delete pTemp;
	pTemp = NULL;
	fclose(fp);
	cout << "read " << nCount << "records" << endl;
	return true;
//	PSTUDENT pOne = NULL;
//	PSTUDENT pTwo = NULL;
//
//	pOne = new STUDENT;
//	pTwo = pOne;
// 	g_pHead = pOne;
// 	g_pHead->pNext = NULL;
//	while (!feof(fp))
// 	{
// 		if (!fscanf(fp, FORMAT, pOne->szNo, pOne->szName, pOne->szSex,&pOne->nAge,pOne->szCourse[0],pOne->szCourse[1],pOne->szCourse[2]))
// 		{
// 			printf("/n读入文件出错，可能文件为空，或文件不存在!/n");
//  		}
//  		pTwo = pOne;
// 		pOne = new STUDENT;
//  		pTwo->pNext = pOne;
//
//	}
//	pTwo->pNext = NULL;
//	free(pOne); /*释放多余的创建*/
//	pOne = NULL;
//	PSTUDENT pLast = NULL;
//	PSTUDENT pTemp = new STUDENT;
//	g_pHead = pTemp;
//	while(!feof(fp))
//	{
// 		int ret = fread(pTemp, sizeof(STUDENT), 1, fp);
//		if (ret < 1) break;
// 		pLast = pTemp;
// 		pTemp = new STUDENT;
//
//	}
//	pLast->pNext = NULL;
//	//delete pTemp;
//	//pTemp = NULL;
// 	fclose(fp);


	return true;
}

bool SaveFile()
{
	if(NULL != g_pHead)
	{
		FILE* fp = NULL;
		STUDENT* pTemp = g_pHead;
		if ((fp = fopen("Student.txt", "wb")) == NULL)
		{
			printf("can not open file\n");
			return false;
		}
		while (pTemp)
		{
			//if (fprintf(fp,FORMAT,pTemp->szNo,pTemp->szName,pTemp->szSex,pTemp->nAge,pTemp->szCourse[0],pTemp->szCourse[1],pTemp->szCourse[2])<0)
			//if(fwrite(pTemp,sizeof(STUDENT),1,fp)!=1)
			//{
			//	printf("file write error\n");
			//}
			fwrite(pTemp, sizeof(STUDENT), 1, fp);
			pTemp = pTemp->pNext;
		}

		fclose(fp);
		return true;
	}
	return false;

}
