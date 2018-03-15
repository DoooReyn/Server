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
{
	NetworkManager::newInstance();
	GameGateClient::newInstance();
	GameLoginClient::newInstance();

	registerMessageHandle();
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


void GameServer::connectGate()
{
	MapGateinfo mapGateInfo = GameGateClient::getInstance().GetGateInfoMap();
	for (std::map<uint32, GateInfo>::iterator it = mapGateInfo.begin(); it != mapGateInfo.end(); ++it)
	{
		InetAddress GateAddr(it->second.m_ipzone, it->second.m_port, false);
		std::shared_ptr<TCPClient> pGate(new TCPClient(&m_Loop, GateAddr, m_ServerName));
		pGate->SetConnectionCallback(std::bind(&GameGateClient::onConnection, GameGateClient::getInstancePtr(), _1));
		pGate->SetDisconnectCallback(std::bind(&GameGateClient::onDisconnect, GameGateClient::getInstancePtr(), _1));
		pGate->SetMessageCallback(std::bind(&NetworkManager::OnMessage, NetworkManager::getInstancePtr(), _1, _2, _3));
		pGate->EnableRetry();
		pGate->Connect();
		m_vecTCPGate.push_back(pGate);
	}
}

void GameServer::connectLogin()
{
	string strIp = GameLoginClient::getInstance().GetIP();
	uint32 nPort = GameLoginClient::getInstance().GetPort();
	InetAddress LoginAddr(strIp, nPort, false);
	std::shared_ptr<TCPClient> pLogin(new TCPClient(&m_Loop, LoginAddr, m_ServerName));
	m_pLogin = pLogin;
	m_pLogin->SetConnectionCallback(std::bind(&GameLoginClient::onConnection, GameLoginClient::getInstancePtr(), _1));
	m_pLogin->SetDisconnectCallback(std::bind(&GameLoginClient::onDisconnect, GameLoginClient::getInstancePtr(), _1));
	m_pLogin->SetMessageCallback(std::bind(&NetworkManager::OnMessage, NetworkManager::getInstancePtr(), _1, _2, _3));
	m_pLogin->EnableRetry();
	m_pLogin->Connect();
}


void GameServer::Loop()
{
	//链接网关服务器
	connectGate();

	//链接登陆服务器
	connectLogin();

	m_Loop.RunEvery(UPDATETIME, std::bind(&GameServer::onUpdate, this));
	//m_Loop.RunEvery(5, std::bind(&GameServer::onUpdate2, this));
	m_Loop.Loop();

}

void GameServer::onUpdate()
{
	NetworkManager::getInstance().ParseMsg();
}


void GameServer::onUpdate2()
{
	INFO("recv num:%d", g_map_parma[1]);
}


void GameServer::OnExit(int32 signum)
{
	m_Loop.Quit();
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
	//WARN("NotifyLoginGateUrl");

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
