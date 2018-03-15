
#include "logger.h"
#include "string_tool.h"
#include "xml_parse.h"
#include "platcmd.pb.h"
#include "common.h"
#include "network_manager.h"
#include "command.h"
#include "platcmd.pb.h"
#include "client_login.h"
#include "client_player.h"



ClientLoginClient::ClientLoginClient(EventLoop* pLoop, ClientPlayer* client_player)
	: m_client(NULL)
	, m_loop(pLoop)
	, m_client_player(client_player)
	, m_thr(std::bind(&ClientLoginClient::threadFunc, this))
{
	m_conn_type = 0;
	m_quit = false;
	m_zoneid = 0;
	m_accid = 0;
	m_gatePort = 0;


	registerMessageHandle();
}

ClientLoginClient::~ClientLoginClient()
{
	DELETE(m_client);
	unregisterMessageHandle();
}

void ClientLoginClient::registerMessageHandle()
{
	NetworkManager::getInstance().RegisterMessageHandle(CMD_CONNECT_VERIFY_RETURN, &ClientLoginClient::ParseConnectVerifyReturn, this);
	NetworkManager::getInstance().RegisterMessageHandle(CLIENT_ACCOUT_VERIFY_RETURN, &ClientLoginClient::ParseAccoutVerifyReturn, this);
	NetworkManager::getInstance().RegisterMessageHandle(CLIENT_ACCOUT_REG_RETURN, &ClientLoginClient::ParseAccoutRegReturn, this);
	NetworkManager::getInstance().RegisterMessageHandle(CLIENT_ZONE_LIST_RETURN, &ClientLoginClient::ParseZoneListReturn, this);
	NetworkManager::getInstance().RegisterMessageHandle(CLIENT_ZONE_INFO_RETURN, &ClientLoginClient::ParseZoneInfoReturn, this);
	NetworkManager::getInstance().RegisterMessageHandle(CLIENT_ROLE_LIST_RETURN, &ClientLoginClient::ParseRoleListReturn, this);
	NetworkManager::getInstance().RegisterMessageHandle(CLIENT_ROLE_CREATE_RETURN, &ClientLoginClient::ParseRoleCreateReturn, this);

}

void ClientLoginClient::unregisterMessageHandle()
{
	NetworkManager::getInstance().UnregisterMessageHandle(CMD_CONNECT_VERIFY_RETURN);
	NetworkManager::getInstance().UnregisterMessageHandle(CLIENT_ACCOUT_VERIFY_RETURN);
	NetworkManager::getInstance().UnregisterMessageHandle(CLIENT_ACCOUT_REG_RETURN);
	NetworkManager::getInstance().UnregisterMessageHandle(CLIENT_ZONE_LIST_RETURN);
	NetworkManager::getInstance().UnregisterMessageHandle(CLIENT_ZONE_INFO_RETURN);
	NetworkManager::getInstance().UnregisterMessageHandle(CLIENT_ROLE_LIST_RETURN);
	NetworkManager::getInstance().UnregisterMessageHandle(CLIENT_ROLE_CREATE_RETURN);

}
void ClientLoginClient::Init(const string& strLSIP, const uint16& nPort)
{
	InetAddress serverAddr(strLSIP, nPort);
	m_client = new TCPClient(m_loop, serverAddr, "Client");
	m_Timerid = m_loop->RunEvery(UPDATETIME, std::bind(&NetworkManager::ParseMsg, NetworkManager::getInstancePtr()));
	m_client->SetConnectionCallback(std::bind(&ClientLoginClient::onConnection, this, _1));
	m_client->SetDisconnectCallback(std::bind(&ClientLoginClient::onDisconntion, this, _1));
	m_client->SetMessageCallback(std::bind(&NetworkManager::OnMessage, NetworkManager::getInstancePtr(), _1, _2, _3));
}



void ClientLoginClient::threadFunc()
{
	while (!m_quit)
	{
		//cout << "指令输入>:";
		int32 op = 0;
		cin >> op;
		doMessage(op);
		cin.clear();
		cin.ignore();
		setbuf(stdin, NULL);
	}
}

void ClientLoginClient::onConnection(const TcpConnectionPtr& conn)
{
	DEBUG("onConnection %s -> %s is %d", conn->GetLocalAddr().ToIPPort().c_str(), conn->GetPeerAddr().ToIPPort().c_str(), conn->Connected());
	CmdConnectVerifyRequest sendPack;
	sendPack.id = m_accid;
	sendPack.conntype =  CONNECT_CLIENT;
	conn->Send(CMD_CONNECT_VERIFY_REQUEST, &sendPack, sizeof(sendPack));
}


void ClientLoginClient::onDisconntion(const TcpConnectionPtr& conn)
{
	DEBUG("onDisconntion %s -> %s is %d", conn->GetLocalAddr().ToIPPort().c_str(), conn->GetPeerAddr().ToIPPort().c_str(), conn->Connected());
	m_quit = true;
	//------------ 自动登陆
	m_thr.Join();
	//------------
}

void ClientLoginClient::Connect()
{
	m_client->Connect();
	m_conn_type = 1;
}

void ClientLoginClient::ParseConnectVerifyReturn(MessagePack* pPack)
{
	CHECKERR_AND_RETURN(sizeof(CmdConnectVerifyReturn) == pPack->size);
	CmdConnectVerifyReturn* recvPack = (CmdConnectVerifyReturn*)pPack->data;
	INFO("Verify Return Src IP:%s", recvPack->src);
	//------------ 自动登陆
	m_thr.Start();
	//------------
	//doMessage(1);
}

void ClientLoginClient::doMessage(int32 op)
{

	if (op == 1)
	{
		return AccountVerify();
	}

	if (op == 2)
	{
		return AccountReg();
	}

	if (op == 3)
	{
		return ZoneRequest();
	}

	if (op == 4)
	{
		return ZoneSelect();
	}

	if (op == 5)
	{
		return RoleRequest();
	}

	if (op == 6)
	{
		return RoleCreate();
	}
	if (op == 7)
	{
		cout << "7:===连接区服===" << endl;
		m_quit = true;
		m_loop->Cancel(m_Timerid);
		m_client->Stop();
		m_client->Disconnect();
		//DELETE(m_client);

		unregisterMessageHandle();
		if (!m_gateurl.empty())
		{
			m_client_player->InitGate(m_gateurl, m_gatePort);
			m_client_player->StartGate();
			m_client_player->m_Gate_client.SetUser(m_accid, m_zoneid);
			m_client_player->m_Gate_client.registerMessageHandle();
		}
	}
}


void ClientLoginClient::AccountVerify()
{
	TcpConnectionPtr conn = m_client->GetConnection();
	cout << "1:===账号验证===" << endl;
	//------------ 自动登陆
	cout << "user:";
	string user;
	cin >> user;
	//std::getline(std::cin, user);
	//------------
	//string user = g_accname;
	cout << user << endl;
	Cmd::AccoutVerifyRequest sendPack;
	sendPack.set_user(user);
	sendPack.set_passwd("passwd_" + user);
	conn->SendProtoBuf(CLIENT_ACCOUT_VERIFY_REQUEST, sendPack);
}

void ClientLoginClient::ParseAccoutVerifyReturn(MessagePack* pPack)
{
	Cmd::AccoutVerifyReturn recvPack;
	CHECKERR_AND_RETURN(recvPack.ParseFromArray(pPack->data, pPack->size));
	uint32 result = recvPack.result();
	if (result == 1)
	{
		ERROR("账号:%s--不存在", recvPack.user().c_str());
	}
	else if (result == 2)
	{
		ERROR("账号:%s--密码错误", recvPack.user().c_str());
	}
	else
	{
		INFO("账号:%s--验证成功", recvPack.user().c_str());
		m_accid = recvPack.accid();
		//------------ 自动登陆
		//doMessage(3);
	}
}

void ClientLoginClient::AccountReg()
{
	TcpConnectionPtr conn = m_client->GetConnection();
	cout << "2:===账号注册===" << endl;
	string user;
	cout << "user:";
	cin >> user;
	Cmd::AccountRegisterRequest sendPack;
	sendPack.set_user(user);
	sendPack.set_passwd("passwd_" + user);
	conn->SendProtoBuf(CLIENT_ACCOUT_REG_REQUEST, sendPack);
}

void ClientLoginClient::ParseAccoutRegReturn(MessagePack* pPack)
{
	Cmd::AccountRegisterReturn recvPack;
	CHECKERR_AND_RETURN(recvPack.ParseFromArray(pPack->data, pPack->size));
	uint32 result = recvPack.result();
	if (result == 1)
	{
		ERROR("账号:%s--已被使用", recvPack.user().c_str());
	}
	else
	{
		INFO("账号:%s--注册成功", recvPack.user().c_str());
	}
}


void ClientLoginClient::ZoneRequest()
{
	TcpConnectionPtr conn = m_client->GetConnection();
	cout << "3:===区分列表===" << endl;
	conn->Send(CLIENT_ZONE_LIST_REQUEST);

}

void ClientLoginClient::ParseZoneListReturn(MessagePack* pPack)
{
	m_mapZoneList.clear();
	Cmd::ZoneListReturn recvPack;
	CHECKERR_AND_RETURN(recvPack.ParseFromArray(pPack->data, pPack->size));
	DEBUG("ParseZoneListReturn");
	for (int32 i = 0; i < recvPack.zonelist_size(); i++)
	{
		const Cmd::ZoneList& zonelist = recvPack.zonelist(i);
		if (zonelist.state() == ZONE_SHUTDOWN)
		{
			WARN("id:%d Name:%s", zonelist.zoneid(), zonelist.name().c_str());
		}
		else
		{
			INFO("id:%d Name:%s", zonelist.zoneid(), zonelist.name().c_str());
		}

		ZoneInfo zonetemp;
		zonetemp.zoneid = zonelist.zoneid();
		zonetemp.state = zonelist.state();
		strncpy(zonetemp.name, zonelist.name().c_str(), 128);
		m_mapZoneList.insert(std::make_pair(zonetemp.zoneid, zonetemp));
	}
	//------------ 自动登陆
	//doMessage(4);
}

void ClientLoginClient::ZoneSelect()
{
	TcpConnectionPtr conn = m_client->GetConnection();
	cout << "4:===区服选择===" << endl;
	//------------ 自动登陆
	cout << "zoneid:";
	int32 zoneid;
	cin >> zoneid;
	//-----------------
	//uint32 zoneid = g_zoneid;
	ZoneInfo zone_info = m_mapZoneList[zoneid];
	if (zone_info.state == ZONE_SHUTDOWN)
	{
		cout << "zone not open" << endl;
		return;
	}
	cout << "Login Server:" << zone_info.name << endl;

	Cmd::ZoneInfoRequest sendPack;
	sendPack.set_zoneid(zoneid);
	sendPack.set_accid(m_accid);
	conn->SendProtoBuf(CLIENT_ZONE_INFO_REQUEST, sendPack);
	m_zoneid = zoneid;
}

void ClientLoginClient::ParseZoneInfoReturn(MessagePack* pPack)
{
	Cmd::ZoneInfoReturn recvPack;
	CHECKERR_AND_RETURN(recvPack.ParseFromArray(pPack->data, pPack->size));
	DEBUG("ParseZoneLoginReturn zoneid:%d gateid:%d url:%s", recvPack.zoneid(), recvPack.gateid(), recvPack.gateurl().c_str());

	if (recvPack.gateurl().empty())
	{
		ERROR("can't get gate address");
		return;
	}
	string gwUrl = recvPack.gateurl();
	std::vector<string> vecStr;
	StringTool::Split(gwUrl, ":", vecStr);
	assert(vecStr.size() == 2);
	m_gateurl = vecStr[0];
	m_gatePort = StringTool::StoI(vecStr[1]);
	//------------ 自动登陆
	//doMessage(5);
}


void ClientLoginClient::RoleRequest()
{
	cout << "5:===角色列表请求===" << endl;
	TcpConnectionPtr conn = m_client->GetConnection();
	Cmd::RoleListRequest sendPack;
	sendPack.set_accid(m_accid);
	sendPack.set_zoneid(m_zoneid);
	conn->SendProtoBuf(CLIENT_ROLE_LIST_REQUEST, sendPack);
}

void ClientLoginClient::ParseRoleListReturn(MessagePack* pPack)
{
	Cmd::RoleListReturn recvPack;
	CHECKERR_AND_RETURN(recvPack.ParseFromArray(pPack->data, pPack->size));
	if (recvPack.rolelist_size() == 0)
	{
		INFO("RoleList Is NULL");
		return;
	}

	for (int32 i = 0; i < recvPack.rolelist_size(); i++)
	{
		Cmd::RoleInfo roleinfo = recvPack.rolelist(i);
		DEBUG("roleid:%d accid:%d name:%s", roleinfo.roleid(), roleinfo.accid(), roleinfo.name().c_str());
	}
	//------------ 自动登陆
	//doMessage(7);
}

void ClientLoginClient::RoleCreate()
{
	cout << "6:===创建角色===" << endl;
	cout << "name:";
	string name;
	cin >> name;

	TcpConnectionPtr conn = m_client->GetConnection();
	Cmd::RoleCreateRequest sendPack;
	sendPack.set_accid(m_accid);
	sendPack.set_zoneid(m_zoneid);
	sendPack.set_name(name);
	conn->SendProtoBuf(CLIENT_ROLE_CREATE_REQUEST, sendPack);
}

void ClientLoginClient::ParseRoleCreateReturn(MessagePack* pPack)
{
	Cmd::RoleCreateReturn recvPack;
	CHECKERR_AND_RETURN(recvPack.ParseFromArray(pPack->data, pPack->size));
	if (recvPack.result() == 1)
	{
		ERROR("role name haved used");
		return;
	}
	else if (recvPack.result() == 2)
	{
		ERROR("insert error");
		return;
	}
	else
	{
		INFO("Create Success roleid:%d name:%s", recvPack.roleid(), recvPack.name().c_str());
	}
}
