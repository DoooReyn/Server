#include "login_server.h"
#include "mysql_pool.h"
#include "logger.h"
#include "string_tool.h"
#include "inet_address.h"
#include "platcmd.pb.h"
#include "common.h"
#include "command.h"
#include "network_manager.h"
#include "login_account_client.h"
#include "tcp_client.h"
#include "signal_catch.h"
#include "mysql_pool.h"
#include "login_gate_parse.h"

void onServerQuit(int32 signum)
{
	LoginServer::getInstance().OnExit(signum);
}


LoginServer::LoginServer()
	: m_ServerName(LS)
	, m_nMaxThread(10)
{

	NetworkManager::newInstance();
	MysqlPool::newInstance();
	LoginAccountClient::newInstance();
	GateLoginParse::newInstance();



	registerMessageHandle();


}

LoginServer::~LoginServer()
{
	m_mapClientConnPtr.clear();
	m_mapGateConnPtr.clear();

	MysqlPool::delInstance();
	NetworkManager::delInstance();
	LoginAccountClient::delInstance();
	GateLoginParse::delInstance();
}


void LoginServer::registerMessageHandle()
{
	NetworkManager::getInstance().RegisterMessageHandle(CLIENT_ZONE_INFO_REQUEST, &LoginServer::ParseZoneInfoRequest, this);
	NetworkManager::getInstance().RegisterMessageHandle(CMD_CONNECT_VERIFY_RETURN, &LoginServer::ParseConnectVerifyReturn, this);
	NetworkManager::getInstance().RegisterMessageHandle(LS_GS_ROLE_CREATE_REQUEST, &LoginServer::ParseRoleCreateReturn, this);
}


bool LoginServer::Init(XMLParse& xmlparse)
{
	bool bRet = initLoginServer(xmlparse);
	if (bRet == false)
	{
		return false;
	}

	bRet = initAccountServer(xmlparse);
	if (bRet == false)
	{
		return false;
	}

	bRet = GateLoginParse::getInstance().InitZoneList();
	if (bRet == false)
	{
		return false;
	}

	CatchCallback cb = onServerQuit;
	SetSignedCatched(cb);

	return bRet;
}


bool LoginServer::initLoginServer(XMLParse& xmlparse)
{
	string strMyql = xmlparse.GetNode(LS, "MySQL");
	bool bRet = MysqlPool::getInstance().PutUrl(strMyql.c_str());
	if (bRet == false)
	{
		ERROR("Mysql Init Failed :%s", strMyql.c_str());
		return bRet;
	}
	//INFO("Mysql Connect Success");
	m_nPort = StringTool::StoI(xmlparse.GetNode(LS, "Port"));
	return bRet;
}


bool LoginServer::initAccountServer(XMLParse& xmlparse)
{
	bool bRet = LoginAccountClient::getInstance().Init(xmlparse);
	if (bRet == false)
	{
		ERROR("initLoginServer Failed");
		return false;
	}
	return bRet;
}


void LoginServer::Loop()
{
	InetAddress listenAddr(m_nPort, false, false);
	TcpServer server(&m_Loop, listenAddr, m_ServerName);
	if (server.IsBindSuccess() == false)
	{
		ERROR("TcpServer Bind Failed IP:%s", listenAddr.ToIPPort().c_str());
		exit(0);
	}

	server.SetConnectionCallback(std::bind(&LoginServer::onConnection, this, _1));
	server.SetDisconnectCallback(std::bind(&LoginServer::onDisconnect, this, _1));

	NetworkManager::getInstance().SetVerifyCallback(std::bind(&LoginServer::onVerify, this, _1, _2, _3));


	server.SetMessageCallback(std::bind(&NetworkManager::OnMessage, NetworkManager::getInstancePtr(), _1, _2, _3));
	m_Loop.RunEvery(UPDATETIME, std::bind(&LoginServer::onUpdate, this));


	string accountIP = LoginAccountClient::getInstance().GetIP();
	uint32 accountPort = LoginAccountClient::getInstance().GetPort();
	InetAddress accountAddr(accountIP, accountPort, false);
	TCPClient LoginAccountClient(&m_Loop, accountAddr, m_ServerName);

	LoginAccountClient.SetConnectionCallback(std::bind(&LoginAccountClient::onConnection, LoginAccountClient::getInstancePtr(), _1));
	LoginAccountClient.SetDisconnectCallback(std::bind(&LoginAccountClient::onDisconnect, LoginAccountClient::getInstancePtr(), _1));
	LoginAccountClient.SetMessageCallback(std::bind(&NetworkManager::OnMessage, NetworkManager::getInstancePtr(), _1, _2, _3));
	LoginAccountClient.EnableRetry();
	LoginAccountClient.Connect();


	server.SetThreadNum(m_nMaxThread);
	server.Start();
	m_Loop.Loop();
}

void LoginServer::onUpdate()
{
	NetworkManager::getInstance().ParseMsg();
}

void LoginServer::OnExit(int32 signum)
{
	m_Loop.Quit();
}

void LoginServer::onConnection(const TcpConnectionPtr& conn)
{
	//DEBUG("onConnection %s -> %s", conn->GetPeerAddr().ToIPPort().c_str(), conn->GetLocalAddr().ToIPPort().c_str());
}

void LoginServer::onDisconnect(const TcpConnectionPtr& conn)
{
	//DEBUG("onDisconnect %s -> %s", conn->GetPeerAddr().ToIPPort().c_str(), conn->GetLocalAddr().ToIPPort().c_str());
	m_Loop.RunInLoop(std::bind(&LoginServer::ConnectChange, this, conn->GetType(), conn, false));
}

void LoginServer::ConnectChange(int32 conntype, const TcpConnectionPtr& conn, bool status)
{
	INFO("current pid:%d status:%s conntype:%d", CurrentThread::Tid(), status ? "connect" : "disconnect", conntype);
	if (status)
	{
		if (conntype == CONNECT_CLIENT)  //客户端发过来的链接
		{
			m_mapClientConnPtr.insert(std::make_pair(conn->GetThisid(), conn));
		}
		if (conntype == CONNECT_GATE)
		{
			m_mapGateConnPtr.insert(std::make_pair(conn->GetThisid(), conn));
		}
		if (conntype == CONNECT_GAME)
		{
			m_GameConnPtr = conn;
		}
	}
	else
	{
		if (conntype == CONNECT_CLIENT || conntype == 0)  //客户端发过来的链接
		{
			m_mapClientConnPtr.erase(conn->GetThisid());
		}
		if (conntype == CONNECT_GATE)
		{
			m_mapGateConnPtr.erase(conn->GetThisid());
		}
		if (conntype == CONNECT_GAME)
		{
			m_GameConnPtr.reset();
		}
	}
}

void LoginServer::onVerify(const TcpConnectionPtr& conn, int32 id, int32 conntype)
{
	conn->SetThisid(id, conntype);
	INFO("onVerify %s -> %s", conn->GetPeerAddr().ToIPPort().c_str(), conn->GetLocalAddr().ToIPPort().c_str());
	m_Loop.RunInLoop(std::bind(&LoginServer::ConnectChange, this, conntype, conn, true));
}

void LoginServer::SendPBToClient(const uint32 messageid, int64 cid, ::google::protobuf::Message& data)
{
	itConnPtr itFind = m_mapClientConnPtr.find(cid);
	if (itFind != m_mapClientConnPtr.end() && itFind->second.get())
	{
		itFind->second->SendProtoBuf(messageid, data);
	}
	else
	{
		ERROR("SendPBToClient cid:%ld", cid);
	}
}

void LoginServer::SendPBToGame(const uint32 messageid, int64 cid, ::google::protobuf::Message& data, int32 uid /*= 0*/)
{
	if (m_GameConnPtr.get())
	{
		m_GameConnPtr->SendProtoBuf(messageid, data, uid);
	}
}

void LoginServer::ModifyConnThisid(int64 cid, uint32 accid)
{
	//DEBUG("ModifyConnThisid current id:%d", CurrentThread::Tid());
	itConnPtr itFind = m_mapClientConnPtr.find(cid);
	if (itFind != m_mapClientConnPtr.end())
	{
		const TcpConnectionPtr conn = itFind->second;
		conn->SetThisid(accid, 0);
		m_mapClientConnPtr.erase(itFind);
		m_mapClientConnPtr.insert(std::make_pair(conn->GetThisid(), conn));
	}
}


void LoginServer::ParseConnectVerifyReturn(MessagePack* pPack)
{
	CHECKERR_AND_RETURN(sizeof(CmdConnectVerifyReturn) == pPack->size);
	CmdConnectVerifyReturn* recvPack = (CmdConnectVerifyReturn*)pPack->data;
	INFO("Verify Return Src IP:%s", recvPack->src);
}


void LoginServer::ParseRoleCreateReturn(MessagePack* pPack)
{
	Cmd::RoleCreateReturn recvPack;
	CHECKERR_AND_RETURN(recvPack.ParseFromArray(pPack->data, pPack->size));
	SendPBToClient(CLIENT_ROLE_CREATE_RETURN, recvPack.accid(), recvPack);
}

void LoginServer::ParseZoneInfoRequest(MessagePack* pPack)
{
	Cmd::ZoneInfoRequest recvPack;
	CHECKERR_AND_RETURN(recvPack.ParseFromArray(pPack->data, pPack->size));
	int32 accid = recvPack.accid();
	int32 zoneid = recvPack.zoneid();

	int32 cid = pPack->connid;
	if (accid != cid)
	{
		ERROR(" ParseZoneInfoRequest  err accid:%d zoneid:%d", accid, zoneid);
		return;
	}

	GateData* gw = GateLoginParse::getInstance().GetBestGate(zoneid);
	if (gw == NULL)
	{
		ERROR("ParseZoneInfoRequest  gw is null");
		return;
	}
	DEBUG("ParseZoneInfoRequest accid:%d best gateid:%d", accid, gw->gateid);

	Cmd::ZoneInfoReturn sendPack;
	sendPack.set_zoneid(zoneid);
	sendPack.set_gateid(gw->gateid);
	sendPack.set_gateurl(gw->gateurl);
	SendPBToClient(CLIENT_ZONE_INFO_RETURN, pPack->connid, sendPack);
}




