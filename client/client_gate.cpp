#include "client_gate.h"
#include "network_manager.h"
#include "client_player.h"
#include "platcmd.pb.h"
#include "command.h"
#include "string_tool.h"
#include "r3c.h"

extern std::map<int32, int32> g_map_parma;

ClientGateClient::ClientGateClient(EventLoop* pLoop, ClientPlayer* client_player)
	: m_client(NULL)
	, m_loop(pLoop)
	, m_client_player(client_player)
	, m_thr(std::bind(&ClientGateClient::threadFunc, this))
{
	m_zoneid = 0;
	m_accid = 0;
	m_quit = false;
}

ClientGateClient::~ClientGateClient()
{
	DELETE(m_client);
}

void ClientGateClient::registerMessageHandle()
{
	NetworkManager::getInstance().RegisterMessageHandle(CMD_CONNECT_VERIFY_RETURN, &ClientGateClient::ParseConnectVerifyReturn, this);
	NetworkManager::getInstance().RegisterMessageHandle(CLIENT_ZONE_LOGIN_RETURN, &ClientGateClient::ParseZoneLoginReturn, this);

}


void ClientGateClient::threadFunc()
{
	while (!m_quit)
	{
		int32 op = 0;
		cin >> op;
		doMessage(op);
		cin.clear();
		cin.ignore();
		setbuf(stdin, NULL);
	}
}

void ClientGateClient::Connect()
{
	m_client->Connect();
}

void ClientGateClient::Init(const string& strLSIP, const uint16& nPort)
{
	InetAddress serverAddr(strLSIP, nPort);
	m_client = new TCPClient(m_loop, serverAddr, "Client");
	m_loop->RunEvery(UPDATETIME, std::bind(&NetworkManager::ParseMsg, NetworkManager::getInstancePtr()));
	m_client->SetConnectionCallback(std::bind(&ClientGateClient::onConnection, this, _1));
	m_client->SetDisconnectCallback(std::bind(&ClientGateClient::onDisconntion, this, _1));
	m_client->SetMessageCallback(std::bind(&NetworkManager::OnMessage, NetworkManager::getInstancePtr(), _1, _2, _3));
}

void ClientGateClient::SetUser(int32 accid, int32 zoneid)
{
	m_accid = accid;
	m_zoneid = zoneid;
}



void ClientGateClient::onConnection(const TcpConnectionPtr& conn)
{
	DEBUG("onConnection %s -> %s is %d", conn->GetLocalAddr().ToIPPort().c_str(), conn->GetPeerAddr().ToIPPort().c_str(), conn->Connected());
	CmdConnectVerifyRequest sendPack;
	sendPack.id = m_accid;
	sendPack.conntype = CONNECT_CLIENT;
	conn->Send(CMD_CONNECT_VERIFY_REQUEST, &sendPack, sizeof(sendPack));
}

void ClientGateClient::onDisconntion(const TcpConnectionPtr& conn)
{
	DEBUG("onDisconntion %s -> %s is %d", conn->GetLocalAddr().ToIPPort().c_str(), conn->GetPeerAddr().ToIPPort().c_str(), conn->Connected());
	m_quit = true;
	m_thr.Join();
}



void ClientGateClient::ParseConnectVerifyReturn(MessagePack* pPack)
{
	CHECKERR_AND_RETURN(sizeof(CmdConnectVerifyReturn) == pPack->size);
	m_thr.Start();
	CmdConnectVerifyReturn* recvPack = (CmdConnectVerifyReturn*)pPack->data;
	INFO("Verify Return Src IP:%s", recvPack->src);
}


void ClientGateClient::doMessage(int32 op)
{
	if (op == 1)
	{
		return TestCmd();
	}
	if (op == 2)
	{
		return TestCmd2();
	}
	if (op == 3)
	{
		WARN("totol send:%d", g_map_parma[0]);
	}
	if (op == 4)
	{
		return TestRedis();
	}
	if (op == 8)
	{
		return ZoneLogin();
	}
}


void ClientGateClient::TestCmd()
{
	TcpConnectionPtr conn = m_client->GetConnection();
	for (int32 i = 0; i < 100000; i++)
	{
		Cmd::CmdTest sendPack;
		sendPack.set_str(StringTool::Format("我们 Test:%d", i));
		conn->SendProtoBuf(CMD_CONNECT_TEST2, sendPack);

		string sendStr = StringTool::Format("WL Test:%d", i);
		conn->Send(CMD_CONNECT_TEST1, sendStr.c_str(), sendStr.length());
	}
}

void ClientGateClient::TestCmd2()
{
	TcpConnectionPtr conn = m_client->GetConnection();
	string sendStr = "WL Test:99999";
	conn->Send(CMD_CONNECT_TEST1, sendStr.c_str(), sendStr.length());
}

void ClientGateClient::TestRedis()
{
	r3c::CRedisClient redis("127.0.0.1:6379");
	Cmd::CmdRedisTest cmdTest;
	for (int32 i = 0 ; i < 52; i++)
	{
		cmdTest.set_count(i);
		Cmd::CmdZoneStateChange*  testobj = cmdTest.add_list();
		testobj->set_zoneid(i);
		testobj->set_state(i);
	}
	string value;
	cmdTest.SerializeToString(&value);
	redis.hset("pbtest", "1", value);

	value.clear();
	redis.hget("pbtest", "1", &value);

	Cmd::CmdRedisTest cmdTestNew;
	cmdTestNew.ParseFromString(value);
	DEBUG("cmdTestNew count:%d", cmdTestNew.count());
	for (int32 i = 0; i < cmdTestNew.list_size(); i++)
	{
		DEBUG("id:%d status:%d", cmdTestNew.list(i).zoneid(), cmdTestNew.list(i).state());
	}

}

void ClientGateClient::ZoneLogin()
{
	TcpConnectionPtr conn = m_client->GetConnection();
	cout << "===区服登陆===" << endl;
	Cmd::ZoneLoginRequest sendPack;
	sendPack.set_zoneid(m_zoneid);
	sendPack.set_accid(m_accid);
	conn->SendProtoBuf(CLIENT_ZONE_LOGIN_REQUEST, sendPack);
}

void ClientGateClient::ParseZoneLoginReturn(MessagePack* pPack)
{
	Cmd::ZoneLoginReturn recvPack;
	CHECKERR_AND_RETURN(recvPack.ParseFromArray(pPack->data, pPack->size));
	bool result = recvPack.result();
	if (result == false)
	{
		ERROR("ZoneLoginReturn Failed zoneid:%d accid:%d", recvPack.zoneid(), recvPack.accid());
		return;
	}

	INFO("ZoneLoginReturn Success zoneid:%d accid:%d", recvPack.zoneid(), recvPack.accid());
}

