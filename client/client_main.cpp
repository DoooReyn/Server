#include "logger.h"
#include "string_tool.h"
#include "xml_parse.h"
#include "platcmd.pb.h"
#include "client_player.h"
#include "network_manager.h"


int32 g_zoneid;
string g_accname;

void Help()
{
	cout << " ---------------------------- " << endl;
	cout << "|        1 账号验证           |" << endl;
	cout << "|        2 账号注册           |" << endl;
	cout << "|        3 区服列表           |" << endl;
	cout << "|        4 区服选择           |" << endl;
	cout << "|        5 角色列表           |" << endl;
	cout << "|        6 角色创建           |" << endl;
	cout << "|        7 区服链接           |" << endl;
	cout << "|        8 区服登陆           |" << endl;
	cout << " ---------------------------- " << endl;
}

int main(int argc, char* argv[])
{
	//Help();
	//解析配置文件
	XMLParse xmlparse("config/server.xml");
	/* -------- 自动登陆
	if (argc == 3)
	{
		g_accname = argv[1];
		g_zoneid = stoi(argv[2]);
	}
	else
	{
		cout << "usage ./client accname zoneid" << endl;
		return 0;
	}
	*/

	//设置日志记录
	InitLogger(xmlparse.GetNode("CLIENT", "Log"), xmlparse.GetNode("CLIENT", "LogLevel").c_str(), "CLIENT");
	INFO("pid = %d ,tid = %d start", getpid(), CurrentThread::Tid());



	NetworkManager::newInstance();
	ClientPlayer::newInstance();
	ClientPlayer::getInstance().InitLogin(xmlparse);
	ClientPlayer::getInstance().StartLogin();
	ClientPlayer::getInstance().Loop();
	INFO("pid = %d ,tid = %d end", getpid(), CurrentThread::Tid());
	return 0;
}
