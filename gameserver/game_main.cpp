#include "game_server.h"
#include "xml_parse.h"
#include "logger.h"
#include "common.h"
#include "string_tool.h"


int main(int argc, char* argv[])
{
	//解析配置文件
	XMLParse xmlparse("config/server.xml");
	//设置日志记录
	if (argc < 2)
	{
		printf("Usage ./gameserver serverid\n");
		return 0;
	}

	string strServerid = argv[1];
	uint32 server_id = stoi(strServerid);

	string GSNode;
	StringTool::Format(GSNode, "%s_%d", GS, server_id);
	InitLogger(xmlparse.GetNode(GSNode, "Log"), xmlparse.GetNode(GSNode, "LogLevel").c_str(), GSNode);


	//主线程ID
	INFO("[pid = %d tid =%d]", getpid(), CurrentThread::Tid());

	//GameServer 初始化
	GameServer::newInstance();
	GameServer::getInstance().SetServerID(server_id);
	bool bRet = GameServer::getInstance().Init(xmlparse);

	if (bRet ==  false)
	{
		return -1;
	}

	GameServer::getInstance().Loop();
	return 0;
}
