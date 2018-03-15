#include "login_server.h"
#include "xml_parse.h"
#include "logger.h"
#include "thread.h"
#include "common.h"


int main()
{
	//解析配置文件
	XMLParse xmlparse("config/server.xml");

	//设置日志记录
	InitLogger(xmlparse.GetNode(LS, "Log"), xmlparse.GetNode(LS, "LogLevel").c_str(), LS);

	//主线程ID
	INFO("[pid = %d tid =%d]", getpid(), CurrentThread::Tid());

	///AccountServer 初始化
	LoginServer::newInstance();
	bool bRet = LoginServer::getInstance().Init(xmlparse);
	if (bRet == false)
	{
		return -1;
	}

	LoginServer::getInstance().Loop();
	return 0;
}
