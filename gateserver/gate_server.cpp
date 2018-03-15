
#include "logger.h"
#include "string_tool.h"
#include "inet_address.h"
#include "platcmd.pb.h"
#include "common.h"
#include "command.h"
#include "network_manager.h"
#include "tcp_client.h"
#include "signal_catch.h"
#include "helper.h"
#include "gate_login_client.h"
#include "gate_server.h"

extern std::map<int32, int32> g_map_parma;

void onServerQuit(int32 signum)
{
	GateServer::getInstance().OnExit(signum);
}

GateServer::GateServer()
	: m_ServerName(GW)
	, m_nMaxThread(10)
	, m_ServerId(0)
{
	NetworkManager::newInstance();
	GateLoginClient::newInstance();


	registerMessageHandle();
}

GateServer::~GateServer()
{
	NetworkManager::delInstance();
	GateLoginClient::delInstance();
}


void GateServer::SetServerID(uint32 serverid)
{
	m_ServerId = serverid;
	StringTool::Format(m_ServerName, "%s_%d", GW, m_ServerId);
}

void GateServer::registerMessageHandle()
{
	//这里注册的消息要写到 OnTransmit 判断类型中，不需要转发
	NetworkManager::getInstance().RegisterMessageHandle(CLIENT_ZONE_LOGIN_REQUEST, &GateServer::ParseZoneLoginReturn, this);
}

uint32 GateServer::GetMessageType(uint32 messageid)
{
	switch (messageid)
	{
		case CLIENT_ZONE_LOGIN_REQUEST:
		case CMD_CONNECT_VERIFY_RETURN:
			return CONNECT_GATE;
		default:						//默认发送到客户端 或者来源为区服
			return CONNECT_CLIENT;
	}
}


bool GateServer::Init(XMLParse& xmlparse)
{
	bool bRet = initGateServer(xmlparse);
	if (bRet == false)
	{
		return false;
	}

	bRet = initLoginServer(xmlparse);
	if (bRet == false)
	{
		return false;
	}

	CatchCallback cb = onServerQuit;
	SetSignedCatched(cb);

	return bRet;
}


bool GateServer::initGateServer(XMLParse& xmlparse)
{
	m_nPort = StringTool::StoI(xmlparse.GetNode(m_ServerName, "Port"));
	m_nMaxThread = StringTool::StoI(xmlparse.GetNode(m_ServerName, "MaxThreadNum"));
	return true;
}


bool GateServer::initLoginServer(XMLParse& xmlparse)
{
	bool bRet = GateLoginClient::getInstance().Init(xmlparse);
	if (bRet == false)
	{
		ERROR("initGateServer Failed");
		return false;
	}
	return bRet;
}


void GateServer::Loop()
{
	InetAddress listenAddr(m_nPort, false, false);
	TcpServer server(&m_Loop, listenAddr, m_ServerName);
	if (server.IsBindSuccess() == false)
	{
		ERROR("TcpServer Bind Failed IP:%s", listenAddr.ToIPPort().c_str());
		exit(0);
	}

	server.SetConnectionCallback(std::bind(&GateServer::onConnection, this, _1));
	server.SetDisconnectCallback(std::bind(&GateServer::onDisconnect, this, _1));

	NetworkManager::getInstance().SetVerifyCallback(std::bind(&GateServer::onVerify, this, _1, _2, _3));
	NetworkManager::getInstance().SetTransmitCallBack(std::bind(&GateServer::OnTransmit, this, _1, _2, _3));

	server.SetMessageCallback(std::bind(&NetworkManager::OnMessage, NetworkManager::getInstancePtr(), _1, _2, _3));
	m_Loop.RunEvery(UPDATETIME, std::bind(&GateServer::OnUpdate, this));
	//m_Loop.RunEvery(5, std::bind(&GateServer::OnUpdate2, this));


	string LoginIP = GateLoginClient::getInstance().GetIP();
	uint32 LoginPort = GateLoginClient::getInstance().GetPort();
	InetAddress LoginAddr(LoginIP, LoginPort, false);
	TCPClient GateLoginClient(&m_Loop, LoginAddr, m_ServerName);

	GateLoginClient.SetConnectionCallback(std::bind(&GateLoginClient::onConnection, GateLoginClient::getInstancePtr(), _1));
	GateLoginClient.SetDisconnectCallback(std::bind(&GateLoginClient::onDisconnect, GateLoginClient::getInstancePtr(), _1));
	GateLoginClient.SetMessageCallback(std::bind(&NetworkManager::OnMessage, NetworkManager::getInstancePtr(), _1, _2, _3));
	GateLoginClient.EnableRetry();
	GateLoginClient.Connect();
	server.SetThreadNum(m_nMaxThread);
	server.Start();
	m_Loop.Loop();
	m_mapGameConnPtr.clear();

}

void GateServer::OnUpdate()
{
	NetworkManager::getInstance().ParseMsg();
}

void GateServer::OnUpdate2()
{
	WARN("totol send:%d num:%d", g_map_parma[0], g_map_parma[1]);
}

void GateServer::OnExit(int32 signum)
{
	m_Loop.Quit();
}

void GateServer::onConnection(const TcpConnectionPtr& conn)
{
	//DEBUG("onConnection %s -> %s is %d", conn->GetPeerAddr().ToIPPort().c_str(), conn->GetLocalAddr().ToIPPort().c_str(), conn->Connected());
}

void GateServer::onDisconnect(const TcpConnectionPtr& conn)
{
	//DEBUG("onDisconnect %s -> %s is %d", conn->GetPeerAddr().ToIPPort().c_str(), conn->GetLocalAddr().ToIPPort().c_str(), conn->Connected());
	int32 conntype = conn->GetType();
	m_Loop.RunInLoop(std::bind(&GateServer::ConnectChange, this, conntype, conn, false));
}


void GateServer::ConnectChange(int32 conntype, const TcpConnectionPtr& conn, bool status)
{
	INFO("current pid:%d status:%s", CurrentThread::Tid(), status ? "connect" : "disconnect");
	if (status)
	{
		if (conntype == CONNECT_GAME)
		{
			m_mapGameConnPtr.insert(std::make_pair(conn->GetId(), conn));
			GateLoginClient::getInstance().SetZoneState(conn->GetId(), ZONE_NORMAL);
		}
		if (conntype == CONNECT_CLIENT)
		{
			m_mapClientConnPtr.insert(std::make_pair(conn->GetId(), conn));
		}
	}
	else
	{
		if (conntype == CONNECT_GAME)
		{
			m_mapGameConnPtr.erase(conn->GetId());
			GateLoginClient::getInstance().SetZoneState(conn->GetId(), ZONE_SHUTDOWN);
		}
		if (conntype == CONNECT_CLIENT)
		{
			m_mapClientConnPtr.erase(conn->GetId());
			m_mapAccidZone.erase(conn->GetId());
		}
	}
}

void GateServer::onVerify(const TcpConnectionPtr& conn, int32 id, int32 conntype)
{
	conn->SetThisid(id, conntype);
	INFO("onVerify %s -> %s", conn->GetPeerAddr().ToIPPort().c_str(), conn->GetLocalAddr().ToIPPort().c_str());
	m_Loop.RunInLoop(std::bind(&GateServer::ConnectChange, this, conntype, conn, true));
}

void GateServer::SendProtoBuf(const uint32 messageid, int64 cid, ::google::protobuf::Message& data, uint32 messtype)
{

}

void GateServer::Send(const TcpConnectionPtr& conn, PackHead* pHead, const char* data)
{
	if (conn->GetType() == CONNECT_GAME) //game 发来的信息发送到 client
	{
		int32 accid = pHead->uid;
		itConnPtr itFind = m_mapClientConnPtr.find(accid);
		if (itFind != m_mapClientConnPtr.end())
		{
			itFind->second->Send(pHead, data);
		}
		else
		{
			ERROR("Send To Client  accid:%d", accid);
		}
		return;
	}

	if (conn->GetType() == CONNECT_CLIENT) //client 发来的信息转发到game
	{
		int32 accid = conn->GetId();
		int32 zonid = m_mapAccidZone[accid];

		itConnPtr itFind = m_mapGameConnPtr.find(zonid);
		if (itFind != m_mapGameConnPtr.end())
		{
			itFind->second->Send(pHead, data);
			//g_map_parma[2] += PACK_HEAD_LEN + pHead->size;
			//
			//if (pHead->messageid == CMD_CONNECT_TEST1 || pHead->messageid == CMD_CONNECT_TEST2)
			//{
			//	g_map_parma[1]++;
			//}
		}
		else
		{
			ERROR("Send To Game accid:%d zoneid:%d", accid, zonid);
		}
		return;
	}

	ERROR("Send error messtype:%d messageid:%d", conn->GetType(), pHead->messageid);
}


bool GateServer::OnTransmit(const TcpConnectionPtr& conn, PackHead* pHead, const char* data)
{
	uint32 message_type = GetMessageType(pHead->messageid);
	if (message_type == CONNECT_LOGIN)
	{
		GateLoginClient::getInstance().Send(pHead->messageid, pHead->size, data);
		return true;
	}

	else if (message_type == CONNECT_CLIENT)
	{
		Send(conn, pHead, data);
		return true;
	}

	return false;
}


void GateServer::ParseZoneLoginReturn(MessagePack* pPack)
{
	Cmd::ZoneLoginRequest recvPack;
	CHECKERR_AND_RETURN(recvPack.ParseFromArray(pPack->data, pPack->size));
	int32 zoneid = recvPack.zoneid();
	int32 accid = recvPack.accid();

	itConnPtr itFind = m_mapClientConnPtr.find(accid);
	if (itFind == m_mapClientConnPtr.end())
	{
		ERROR("ParseZoneLoginReturn  accid not find :%d", accid);
		return;
	}

	bool bFind = true;
	itConnPtr itFindZone = m_mapGameConnPtr.find(zoneid);
	if (itFindZone == m_mapGameConnPtr.end())
	{
		ERROR("ParseZoneLoginReturn  zoneid not find :%d", zoneid);
		bFind = false;
	}

	m_mapAccidZone[accid] = zoneid;
	Cmd::ZoneLoginReturn sendPack;
	sendPack.set_result(bFind);
	sendPack.set_accid(accid);
	sendPack.set_zoneid(zoneid);
	itFind->second->SendProtoBuf(CLIENT_ZONE_LOGIN_RETURN, sendPack);
}