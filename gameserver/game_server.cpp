#include "game_server.h"
#include "mysql_pool.h"
#include "logger.h"
#include "string_tool.h"
#include "inet_address.h"
#include "platcmd.pb.h"
#include "common.h"
#include "command.h"
#include "network_manager.h"
#include "tcp_client.h"
#include "signal_catch.h"
#include "platcmd.pb.h"
#include "game_gate_client.h"
#include "game_login_client.h"


extern std::map<int32, int32> g_map_parma;


void onServerQuit(int32 signum)
{
	GameServer::getInstance().OnExit(signum);
}

GameServer::GameServer()
	: m_ServerName(GS)
	, m_ServerId(0)
	, m_thr(std::bind(&GameServer::ThreadFunc, this))
	, m_pLoop(nullptr)
	, m_quit(false)
{
	NetworkManager::newInstance();
	GameGateClient::newInstance();
	GameLoginClient::newInstance();

	registerMessageHandle();
	registerTimerHandle();
}

GameServer::~GameServer()
{
	m_vecTCPGate.clear();
	m_pLogin.reset();
	GameLoginClient::delInstance();
	GameGateClient::delInstance();
	NetworkManager::delInstance();
}


void GameServer::SetServerID(uint32 serverid)
{
	m_ServerId = serverid;
	StringTool::Format(m_ServerName, "%s_%d", GS, m_ServerId);
}

void GameServer::registerMessageHandle()
{
	NetworkManager::getInstance().RegisterMessageHandle(CMD_CONNECT_VERIFY_RETURN, &GameServer::ParseConnectVerifyReturn, this);
}

void GameServer::registerTimerHandle()
{
	//m_Loop.RunEvery(5, std::bind(&GameServer::FiveTimer, this));
	//m_pLoop.RunAtDay(12, 11, 0, std::bind(&GameServer::FiveTimer, this));
}

void GameServer::FiveTimer()
{
	DEBUG("FiveTimer");
}

bool GameServer::Init(XMLParse& xmlparse)
{
	bool bRet = GameGateClient::getInstance().Init(xmlparse);
	if (bRet == false)
	{
		return bRet;
	}

	bRet = GameLoginClient::getInstance().Init(xmlparse);

	CatchCallback cb = onServerQuit;
	SetSignedCatched(cb);

	return bRet;
}


bool GameServer::initGameServer(XMLParse& xmlparse)
{
	return true;
}


void GameServer::connectGate(EventLoop& Loop)
{
	MapGateinfo mapGateInfo = GameGateClient::getInstance().GetGateInfoMap();
	for (std::map<uint32, GateInfo>::iterator it = mapGateInfo.begin(); it != mapGateInfo.end(); ++it)
	{
		InetAddress GateAddr(it->second.m_ipzone, it->second.m_port, false);
		std::shared_ptr<TCPClient> pGate(new TCPClient(&Loop, GateAddr, m_ServerName));
		pGate->SetConnectionCallback(std::bind(&GameGateClient::onConnection, GameGateClient::getInstancePtr(), _1));
		pGate->SetDisconnectCallback(std::bind(&GameGateClient::onDisconnect, GameGateClient::getInstancePtr(), _1));
		pGate->SetMessageCallback(std::bind(&NetworkManager::OnMessage, NetworkManager::getInstancePtr(), _1, _2, _3));
		pGate->EnableRetry();
		pGate->Connect();
		m_vecTCPGate.push_back(pGate);
	}
}

void GameServer::connectLogin(EventLoop& Loop)
{
	string strIp = GameLoginClient::getInstance().GetIP();
	uint32 nPort = GameLoginClient::getInstance().GetPort();
	InetAddress LoginAddr(strIp, nPort, false);
	m_pLogin = std::shared_ptr<TCPClient>(new TCPClient(&Loop, LoginAddr, m_ServerName));
	m_pLogin->SetConnectionCallback(std::bind(&GameLoginClient::onConnection, GameLoginClient::getInstancePtr(), _1));
	m_pLogin->SetDisconnectCallback(std::bind(&GameLoginClient::onDisconnect, GameLoginClient::getInstancePtr(), _1));
	m_pLogin->SetMessageCallback(std::bind(&NetworkManager::OnMessage, NetworkManager::getInstancePtr(), _1, _2, _3));
	m_pLogin->EnableRetry();
	m_pLogin->Connect();
}


void GameServer::Loop()
{
	m_thr.Start();
	while (m_quit == false)
	{
		NetworkManager::getInstance().ParseMsg();
		::usleep(UPDATETIME * 1000);
	}
	m_thr.Join();
}


void GameServer::ThreadFunc()
{
	EventLoop Loop;
	m_pLoop = &Loop;

	//链接网关服务器
	connectGate(Loop);

	//链接登陆服务器
	connectLogin(Loop);

	Loop.Loop();
}



void GameServer::onUpdate2()
{
	INFO("recv num:%d", g_map_parma[1]);
}


void GameServer::OnExit(int32 signum)
{
	m_pLoop->Quit();
	m_quit = true;
}



void GameServer::ParseConnectVerifyReturn(MessagePack* pPack)
{
	CHECKERR_AND_RETURN(sizeof(CmdConnectVerifyReturn) == pPack->size);
	CmdConnectVerifyReturn* recvPack = (CmdConnectVerifyReturn*)pPack->data;
	INFO("Verify Return Src IP:%s", recvPack->src);

	if (GameLoginClient::getInstance().IsConnected() && GameGateClient::getInstance().IsConnected() && !m_bSendUrl)
	{
		NotifyLoginGateUrl();
		m_bSendUrl = true;
	}
}


void GameServer::NotifyLoginGateUrl()
{
	MapConnPtr& mapGate = GameGateClient::getInstance().GetGateConnectMap();
	itConnPtr it = mapGate.begin();
	int32 serverid = GameServer::getInstance().GetServerID();
	for (; it != mapGate.end(); ++it)
	{
		uint32 gateid = it->second->GetId();
		string ipport = GameGateClient::getInstance().GetIPPort(gateid);
		Cmd::GateUrlNotify sendPack;
		sendPack.set_gateid(gateid);
		sendPack.set_zoneid(serverid);
		sendPack.set_gateurl(ipport);
		GameLoginClient::getInstance().SendPBToLogin(GS_GATE_URL_SENDTO_LOGIN, sendPack);
	}
}
