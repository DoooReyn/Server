#include<iostream>
using namespace std;
#include<stdlib.h>
#include <iomanip>
#include <stdio.h>


#include "CmdProc.h"

COMMAND g_Cmd[] =
{
	{0, "Help", "帮助", Help},
	{1, "Create", "创建", Create},
	{2, "Add", "添加", Add},
	{3, "Delete", "删除", Delete},
	{4, "Modify", "修改", Modify},
	{5, "Insert", "插入", Insert},
	{6, "Display", "显示", Display},
	{7, "Search", "查找", Search},
	{8, "Clear", "清除", Clear},
	{9, "Exit", "退出", Exit},
	{10, "Save", "保存", Save},
	{11, "Load", "导入", Load}
};

int g_CmdSize = sizeof(g_Cmd) / sizeof(COMMAND);

void CmdProc()
{
	Help();
	char szCmd[20] = "";
	int i = 0;
	while(1)
	{
		cout << ">";
		InputStr(szCmd);
		if(StrComp(szCmd, ""))
		{
			continue;
		}
		for(i = 0; i < g_CmdSize; i++)
		{
			if(StrComp(g_Cmd[i].pCmdName, szCmd))
			{
				g_Cmd[i].pFun();
				break;
			}
		}
		if(g_CmdSize == i)
		{
			cout << "Error Command" << endl;
		}
	}
}


bool CreateNode();
bool AddNode();
bool InsertNode();
bool DeleteNode();
bool ClearList();
bool FindNode();
bool DisplayList();
bool ModifyList();

void Create()
{
	if(CreateNode())
	{
		cout << "Create List Success" << endl;
	}
	else
	{
		cout << "Create List False, List Exist" << endl;
	}
}
void Add()
{
	if(AddNode())
	{
		cout << "Add Node Success" << endl;
	}
	else
	{
		cout << "Add Node False, List Not Exist" << endl;
	}
}
void Delete()
{
	if(DeleteNode())
	{
		cout << "Del Node Success" << endl;
	}
	else
	{
		cout << "Del Node False, List Not Exist" << endl;
	}
}
void Modify()
{
	if(ModifyList())
	{
		cout << "Modify Node Success" << endl;
	}
	else
	{
		cout << "Modify Node False, List Not Exist" << endl;
	}
}
void Search()
{
	if(FindNode())
	{
		cout << "Fond Node Success" << endl;
	}
	else
	{
		cout << "Fond Node False, List Not Exist" << endl;
	}
}
void Insert()
{
	if(InsertNode())
	{
		cout << "Insert Node Success" << endl;
	}
	else
	{
		cout << "Insert Node False, List Not Exist" << endl;
	}
}
void Display()
{
	if(DisplayList())
	{
		cout << "DisplayList List Success" << endl;
	}
	else
	{
		cout << "DisplayList List False, List Not Exist" << endl;
	}
}
void Clear()
{
	if(ClearList())
	{
		cout << "Clear List Success" << endl;
	}
	else
	{
		cout << "Clear List, List Not Exist" << endl;
	}
}
void Help()
{
	cout << setiosflags(ios::left) << setw(22) << "Command" << "Function" << endl;
	for(int i = 0; i < g_CmdSize; i++)
	{
		cout << setiosflags(ios::left) << setw(22) << g_Cmd[i].pCmdName << g_Cmd[i].pCmdInfo << endl;
	}

}
void Exit()
{

	cout << "Quit STMS(Y/N):";
	char ch = getchar();

	if(ch == 'Y' || ch == 'y')
	{
		exit(0);
	}
	else
	{
		getchar();
	}
}

void Save()
{

	if(SaveFile())
	{
		cout << "SaveFile  Success" << endl;
	}
	else
	{
		cout << "SaveFile Success, List Not Exist" << endl;
	}
}

void Load()
{
	LoadFile();
}
