#include "account_server.h"
#include "mysql_pool.h"
#include "logger.h"
#include "string_tool.h"
#include "inet_address.h"
#include "platcmd.pb.h"
#include "command.h"
#include "network_manager.h"
#include "signal_catch.h"

void onServerQuit(int32 signum)
{
	AccountServer::getInstance().OnExit(signum);
}

AccountServer::AccountServer()
	: m_ServerName(AS)
	, m_nMaxThread(0)
{
	MysqlPool::newInstance();
	NetworkManager::newInstance();
	registerMessageHandle();
}

AccountServer::~AccountServer()
{
	MysqlPool::delInstance();
	NetworkManager::delInstance();
}

void AccountServer::registerMessageHandle()
{
	NetworkManager::getInstance().RegisterMessageHandle(LS_ACCOUT_VERIFY_REQUEST, &AccountServer::ParseAccoutVerify, this);
	NetworkManager::getInstance().RegisterMessageHandle(LS_ACCOUT_REG_REQUEST, &AccountServer::ParseAccoutRegister, this);
	NetworkManager::getInstance().RegisterMessageHandle(LS_ROLE_LIST_REQUEST, &AccountServer::ParseRoleList, this);
	NetworkManager::getInstance().RegisterMessageHandle(LS_AS_ROLE_CREATE_REQUEST, &AccountServer::ParseRoleCreate, this);
}

void AccountServer::onConnection(const TcpConnectionPtr& conn)
{
	DEBUG("onConnection %s -> %s", conn->GetPeerAddr().ToIPPort().c_str(), conn->GetLocalAddr().ToIPPort().c_str());
}

void AccountServer::onVerify(const TcpConnectionPtr& conn, int32 id, int32 conntype)
{
	conn->SetThisid(id, conntype);
	m_connPtr = conn;
	INFO("onVerify %s -> %s", conn->GetPeerAddr().ToIPPort().c_str(), conn->GetLocalAddr().ToIPPort().c_str());
}



void AccountServer::onDisconnect(const TcpConnectionPtr& conn)
{
	if (m_connPtr->GetThisid() == conn->GetThisid())
	{
		m_connPtr.reset();
	}
}


bool AccountServer::Init(XMLParse& xmlparse)
{
	string strMyql = xmlparse.GetNode(AS, "MySQL");
	bool bRet = MysqlPool::getInstance().PutUrl(strMyql.c_str());
	if (bRet == false)
	{
		ERROR("Mysql Init Failed :%s", strMyql.c_str());
		return bRet;
	}
	//INFO("Mysql Connect Success");

	m_nPort = stoi(xmlparse.GetNode(AS, "Port"));


	CatchCallback cb = onServerQuit;
	SetSignedCatched(cb);


	return bRet;
}

void AccountServer::Loop()
{
	// listen所有地址  不支持ipv6
	InetAddress listenAddr(m_nPort, false, false);
	TcpServer server(&m_Loop, listenAddr, m_ServerName);
	if (server.IsBindSuccess() == false)
	{
		ERROR("TcpServer Bind Failed IP:%s", listenAddr.ToIPPort().c_str());
		exit(0);
	}

	server.SetConnectionCallback(std::bind(&AccountServer::onConnection, this, _1));
	server.SetDisconnectCallback(std::bind(&AccountServer::onDisconnect, this, _1));

	server.SetMessageCallback(std::bind(&NetworkManager::OnMessage, NetworkManager::getInstancePtr(), _1, _2, _3));
	NetworkManager::getInstance().SetVerifyCallback(std::bind(&AccountServer::onVerify, this, _1, _2, _3));

	server.Start();

	m_Loop.RunEvery(UPDATETIME, std::bind(&AccountServer::OnUpdate, this));

	m_Loop.Loop();
}

void AccountServer::OnUpdate()
{
	NetworkManager::getInstance().ParseMsg();

}

void AccountServer::OnExit(int32 signum)
{
	m_Loop.Quit();
}

void AccountServer::SendPBToLS(const uint32 messageid, ::google::protobuf::Message& data, uint32 accid /* = 0*/)
{
	if (m_connPtr.get())
	{
		m_connPtr->SendProtoBuf(messageid, data, accid);
	}
}

void AccountServer::ParseAccoutVerify(MessagePack* pPack)
{
	Cmd::AccoutVerifyRequest recvPack;
	CHECKERR_AND_RETURN(recvPack.ParseFromArray(pPack->data, pPack->size));

	Cmd::AccoutVerifyReturn sendPack;
	sendPack.set_result(0);
	sendPack.set_user(recvPack.user());
	sendPack.set_cid(recvPack.cid());
	sendPack.set_accid(0);
	string sql = "select * from accounts where user = '" + recvPack.user() + "';";

	AutoHandle handle(MysqlPool::getInstancePtr());
	DataSet data;
	bool bret = handle()->Select(sql, data);
	if (bret == false)
	{
		ERROR(" ExecSelect Error sql:%s", sql.c_str());
	}
	do
	{
		if (data.Size() == 0)
		{
			//DEBUG("no this user: %s", recvPack.user().c_str());
			sendPack.set_result(1);
			break;
		}
		string passwd = data.GetValue(0, "passwd");
		uint32 accid = data.GetValue(0, "accid");
		sendPack.set_accid(accid);

		if (passwd != recvPack.passwd())
		{
			WARN("db passwd:%s", passwd.c_str());
			WARN("message passwd:%s", recvPack.passwd().c_str());
			sendPack.set_result(2);
			break;
		}
	}
	while (false);

	SendPBToLS(LS_ACCOUT_VERIFY_RETURN, sendPack);
}


void AccountServer::ParseAccoutRegister(MessagePack* pPack)
{
	Cmd::AccoutVerifyRequest recvPack;
	CHECKERR_AND_RETURN(recvPack.ParseFromArray(pPack->data, pPack->size));

	Cmd::AccountRegisterReturn sendPack;
	sendPack.set_result(0);
	sendPack.set_user(recvPack.user());
	sendPack.set_cid(recvPack.cid());
	string user = recvPack.user();
	string passwd = recvPack.passwd();
	AutoHandle handle(MysqlPool::getInstancePtr());
	handle.EscapeString(user);
	handle.EscapeString(passwd);
	string sql = "select * from accounts where user = '" + user + "';";

	DataSet data;
	bool bret = handle()->Select(sql, data);
	do
	{
		if (bret == false)
		{
			sendPack.set_result(2);
			ERROR("select user:%s", recvPack.user().c_str());
			break;
		}
		if (data.Size() > 0)
		{
			//DEBUG("no this user: %s", recvPack.user().c_str());
			sendPack.set_result(1);
			break;
		}

		sql.clear();
		StringTool::Format(sql, "insert into accounts(user,passwd,time) values('%s','%s',now())", user.c_str(), passwd.c_str());

		uint32 ret = handle()->ExecSql(sql);
		if (ret > 0)
		{
			sendPack.set_result(3);
			break;
		}
	}
	while (false);

	SendPBToLS(LS_ACCOUT_REG_RETURN, sendPack);
}

void AccountServer::ParseRoleList(MessagePack* pPack)
{
	Cmd::RoleListRequest recvPack;
	CHECKERR_AND_RETURN(recvPack.ParseFromArray(pPack->data, pPack->size));

	Cmd::RoleListReturn sendPack;
	string sql = "select * from roles where zoneid = %d and accid = %d";
	StringTool::Format(sql, sql.c_str(), recvPack.zoneid(), recvPack.accid());
	//DEBUG("sql:%s", sql.c_str());
	AutoHandle handle(MysqlPool::getInstancePtr());
	DataSet data;
	bool bret = handle()->Select(sql, data);
	if (bret == false)
	{
		ERROR(" ExecSelect Error sql:%s", sql.c_str());
	}

	for (uint32 i = 0; i < data.Size(); i++)
	{
		Cmd::RoleInfo* roledata = sendPack.add_rolelist();
		CHECKERR_AND_CONTINUE(roledata != nullptr);
		roledata->set_accid(data.GetValue(i, "accid"));
		roledata->set_roleid(data.GetValue(i, "roleid"));
		string name = data.GetValue(i, "name");
		roledata->set_name(name);
	}
	SendPBToLS(LS_ROLE_LIST_RETURN, sendPack, recvPack.accid());
}

void AccountServer::ParseRoleCreate(MessagePack* pPack)
{
	Cmd::RoleCreateRequest recvPack;
	CHECKERR_AND_RETURN(recvPack.ParseFromArray(pPack->data, pPack->size));

	Cmd::RoleCreateReturn sendPack;

	sendPack.set_roleid(0);
	sendPack.set_name(recvPack.name());
	sendPack.set_result(0);
	sendPack.set_zoneid(recvPack.zoneid());
	sendPack.set_accid(recvPack.accid());
	string name = recvPack.name();
	AutoHandle handle(MysqlPool::getInstancePtr());
	handle.EscapeString(name);
	string sql = "select count(*) as num from roles where name= '" + name + "'";
	DataSet data;
	bool bret = handle()->Select(sql, data);
	if (bret == false)
	{
		ERROR(" ExecSelect Error sql:%s", sql.c_str());
		return;
	}
	int32 num = data.GetValue(0, "num");
	if (num > 0)
	{
		//名字重复
		sendPack.set_result(1);
		SendPBToLS(LS_AS_ROLE_CREATE_RETURN, sendPack, recvPack.accid());
		return;
	}

	sql = "insert into roles(zoneid,accid,name,time) values(%d,%d,'%s',now())";
	StringTool::Format(sql, sql.c_str(), recvPack.zoneid(), recvPack.accid(), recvPack.name().c_str());
	int32 nRet = handle()->ExecSql(sql);
	if (nRet != 0)
	{
		//插入失败
		sendPack.set_result(2);
	}
	else
	{
		int32 roleid = handle()->GetLastInsertId();
		sendPack.set_roleid(roleid);
	}
	SendPBToLS(LS_AS_ROLE_CREATE_RETURN, sendPack, recvPack.accid());
}





