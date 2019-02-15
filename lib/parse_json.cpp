#include "parse_json.h"
#include "logger.h"

JSonParse::JSonParse() : m_filebuf(nullptr), m_pJson(nullptr)
{

}
JSonParse::~JSonParse()
{
	if(m_filebuf)
	{
		delete m_filebuf;
		m_filebuf = nullptr;
	}
	if(m_pJson)
	{
		cJSON_Delete(m_pJson);
	}
}

bool JSonParse::LoadFile(const char* szFileName)
{
	/* 若要一个byte不漏地读入整个文件，只能采用二进制方式打开 */
	FILE* pFile = fopen(szFileName, "rb");
	if(pFile == nullptr)
	{
		ERROR("File error");
		return false;
	}

	/* 获取文件大小 */
	fseek(pFile, 0, SEEK_END);
	size_t lSize = ftell(pFile);
	rewind(pFile);

	/* 分配内存存储整个文件 */
	m_filebuf = new char [lSize + 1];
	if(m_filebuf == nullptr)
	{
		ERROR("Memory error");
		fclose(pFile);
		return false;
	}

	/* 将文件拷贝到buffer中 */
	size_t result = fread(m_filebuf, lSize, 1, pFile);
	if(result == 0)
	{
		printf("result :%zu size:%zu", result, lSize);
		fputs("Reading error", stderr);
		fclose(pFile);
		return false;
	}
	/* 现在整个文件已经在buffer中，可由标准输出打印内容 */
	//printf("%s", m_filebuf);
	/* 结束演示，关闭文件并释放内存 */
	fclose(pFile);
	return true;
}

bool JSonParse::Parse()
{
	if(nullptr == m_filebuf)
	{
		printf("m_filebuf is null\n");
		return false;
	}
	m_pJson = cJSON_Parse(m_filebuf);
	if(nullptr == m_pJson)
	{
		printf("cJSON_Parse is failed\n");
		return false;
	}
	return true;
}

cJSON* JSonParse::GetObjectItem(const char* szkey)
{
	return cJSON_GetObjectItem(m_pJson, szkey);
}

cJSON* JSonParse::GetObjectItemEx(cJSON* pJson, const char* szkey)
{
	return cJSON_GetObjectItem(pJson, szkey);
}
