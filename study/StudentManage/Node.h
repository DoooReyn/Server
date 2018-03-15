#ifndef __NODE_H__
#define __NODE_H__

#include "Input.h"

typedef struct _STUDENT
{
	_STUDENT* pNext;
	char szNo[13];
	char szName[10];
	char szSex[2];
	int nAge;
	char szCourse[5][20];


} STUDENT, *PSTUDENT;


void ModifyNode(STUDENT* pNode);

void InputNode(STUDENT* pNode);

void ShowNode(STUDENT* pNode);

#endif
