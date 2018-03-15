#include "gate_server.h"
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
		printf("Usage ./gateserver serverid\n");
		return 0;
	}

	string strServerid = argv[1];
	uint32 server_id = StringTool::StoI(strServerid);

	string GWNode;
	StringTool::Format(GWNode, "%s_%d", GW, server_id);
	InitLogger(xmlparse.GetNode(GWNode, "Log"), xmlparse.GetNode(GWNode, "LogLevel").c_str(), GWNode);

	//主线程ID
	INFO("[pid = %d tid =%d]", getpid(), CurrentThread::Tid());

	//GateServer 初始化
	GateServer::newInstance();
	GateServer::getInstance().SetServerID(server_id);
	bool bRet = GateServer::getInstance().Init(xmlparse);
	if (bRet == false)
	{
		return -1;
	}

	GateServer::getInstance().Loop();
	return 0;
}
