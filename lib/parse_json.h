#ifndef __PARSE_JSON_H__
#define __PARSE_JSON_H__
#include "type_define.h"
#include "cjson.h"

class JSonParse
{
public:
	JSonParse();
	~JSonParse();
	bool LoadFile(const char* szFileName);
	bool Parse();
	cJSON* GetObjectItem(const char* szkey);
	cJSON* GetObjectItemEx(cJSON* pJson, const char* szkey);
private:
	char* m_filebuf;
	cJSON* m_pJson;
};

#endif
/*
char *makeJson()
{
	cJSON *pJsonRoot = nullptr;

	pJsonRoot = cJSON_CreateObject();
	if(nullptr == pJsonRoot)
	{
		//error happend here
		fputs("cJSON_CreateObject1", stderr);
		return nullptr;
	}
	cJSON_AddStringToObject(pJsonRoot, "hello", "hello world");
	cJSON_AddNumberToObject(pJsonRoot, "number", 10010);
	cJSON_AddBoolToObject(pJsonRoot, "bool", 1);
	cJSON *pSubJson = nullptr;
	pSubJson = cJSON_CreateObject();
	if(nullptr == pSubJson)
	{
		fputs("cJSON_CreateObject2", stderr);
		cJSON_Delete(pJsonRoot);
		return nullptr;
	}
	cJSON_AddStringToObject(pSubJson, "subjsonobj", "a sub json string");
	cJSON_AddItemToObject(pJsonRoot, "subobj", pSubJson);

	char *p = cJSON_Print(pJsonRoot);
	// else use :
	// char * p = cJSON_PrintUnformatted(pJsonRoot);
	if(nullptr == p)
	{
		//convert json list to string faild, exit
		//because sub json pSubJson han been add to pJsonRoot, so just delete pJsonRoot, if you also delete pSubJson, it will coredump, and error is : double free
		cJSON_Delete(pJsonRoot);
		return nullptr;
	}
	//free(p);

	cJSON_Delete(pJsonRoot);
	return p;
}

void parseJson(char *pMsg)
{
	if(nullptr == pMsg)
	{
		return;
	}
	cJSON *pJson = cJSON_Parse(pMsg);
	if(nullptr == pJson)
	{
		// parse faild, return
		return ;
	}

	// get string from json
	cJSON *pSub = cJSON_GetObjectItem(pJson, "hello");
	if(nullptr == pSub)
	{
		//get object named "hello" faild
	}
	printf("obj_1 : %s\n", pSub->valuestring);

	// get number from json
	pSub = cJSON_GetObjectItem(pJson, "number");
	if(nullptr == pSub)
	{
		//get number from json faild
	}
	printf("obj_2 : %d\n", pSub->valueint);

	// get bool from json
	pSub = cJSON_GetObjectItem(pJson, "bool");
	if(nullptr == pSub)
	{
		// get bool from json faild
	}
	printf("obj_3 : %d\n", pSub->valueint);

	// get sub object
	pSub = cJSON_GetObjectItem(pJson, "subobj");
	if(nullptr == pSub)
	{
		// get sub object faild
	}
	cJSON *pSubSub = cJSON_GetObjectItem(pSub, "subjsonobj");
	if(nullptr == pSubSub)
	{
		// get object from subject object faild
	}
	printf("sub_obj_1 : %s\n", pSubSub->valuestring);
	cJSON_Delete(pJson);
}
*/