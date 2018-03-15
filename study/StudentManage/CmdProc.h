#ifndef __CMDPROC_H__
#define __CMDPROC_H__
#include "Input.h"
#include "List.h"

void CmdProc();

void Create();
void Add();
void Delete();
void Modify();
void Search();
void Insert();
void Display();
void Clear();
void Help();
void Exit();
void Save();
void Load();

typedef void (*PFUN)();

typedef struct Command
{
	int nID;
	const char* pCmdName;
	const char* pCmdInfo;
	PFUN pFun;
} COMMAND;

#endif
