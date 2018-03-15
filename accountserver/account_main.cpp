#include "account_server.h"
#include "xml_parse.h"
#include "logger.h"
#include "thread.h"
#include "common.h"


int main()
{

	//解析配置文件
	XMLParse xmlparse("config/server.xml");

	//设置日志记录
	InitLogger(xmlparse.GetNode(AS, "Log"), xmlparse.GetNode(AS, "LogLevel").c_str(), AS);

	//主线程ID
	INFO("[pid = %d tid =%d]", getpid(), CurrentThread::Tid());

	//AccountServer 初始化
	AccountServer::newInstance();
	bool bRet = AccountServer::getInstance().Init(xmlparse);
	if (bRet == false)
	{
		return -1;
	}


	AccountServer::getInstance().Loop();
	return 0;
}
