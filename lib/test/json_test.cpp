#include "parse_json.h"


void TestJson()
{
	JSonParse dc;
	char szFileName[] = "../config/test.json";
	dc.LoadFile(szFileName);
	dc.Parse();
	cJSON* pSub = dc.GetObjectItem("hello");
	printf("obj_1 : %s\n", pSub->valuestring);
}

int main(int argc, char const* argv[])
{
	TestJson();
	return 0;
}