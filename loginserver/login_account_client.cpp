#include "login_account_client.h"
#include "common.h"
#include "command.h"
#include "logger.h"
#include "tcp_connection.h"
#include "network_manager.h"
#include "platcmd.pb.h"
#include "login_server.h"

LoginAccountClient::LoginAccountClient()
{
	registerMessageHandle();
}

LoginAccountClient::~LoginAccountClient()
{

}


void LoginAccountClient::registerMessageHandle()
{
	NetworkManager::getInstance().RegisterMessageHandle(CLIENT_ACCOUT_VERIFY_REQUEST, &LoginAccountClient::ParseAccoutVerify, this);
	NetworkManager::getInstance().RegisterMessageHandle(LS_ACCOUT_VERIFY_RETURN, &LoginAccountClient::ParseAccoutVerifyReturn, this);

	NetworkManager::getInstance().RegisterMessageHandle(CLIENT_ACCOUT_REG_REQUEST, &LoginAccountClient::ParseAccoutRegister, this);
	NetworkManager::getInstance().RegisterMessageHandle(LS_ACCOUT_REG_RETURN, &LoginAccountClient::ParseAccoutRegisterReturn, this);

	NetworkManager::getInstance().RegisterMessageHandle(CLIENT_ROLE_LIST_REQUEST, &LoginAccountClient::ParseRoleList, this);
	NetworkManager::getInstance().RegisterMessageHandle(LS_ROLE_LIST_RETURN, &LoginAccountClient::ParseRoleListReturn, this);

	NetworkManager::getInstance().RegisterMessageHandle(CLIENT_ROLE_CREATE_REQUEST, &LoginAccountClient::ParseRoleCreate, this);
	NetworkManager::getInstance().RegisterMessageHandle(LS_AS_ROLE_CREATE_RETURN, &LoginAccountClient::ParseRoleCreateReturn, this);

}

bool LoginAccountClient::Init(XMLParse& xmlparse)
{
	m_strIP = xmlparse.GetNode(AS, "IP");
	m_nPort = stoi(xmlparse.GetNode(AS, "Port"));
	//INFO("AccountServer IP:%s Port:%d", m_strIP.c_str(), m_nPort);
	return true;
}


void LoginAccountClient::onConnection(const TcpConnectionPtr& conn)
{
	DEBUG("tid:%d onConnection %s -> %s", CurrentThread::Tid(), conn->GetLocalAddr().ToIPPort().c_str(), conn->GetPeerAddr().ToIPPort().c_str());
	if (conn->Connected())
	{
		m_connPtr = conn;
		CmdConnectVerifyRequest sendPack;
		sendPack.id = 0;
		sendPack.conntype = CONNECT_LOGIN;
		m_connPtr->Send(CMD_CONNECT_VERIFY_REQUEST, &sendPack, sizeof(sendPack));
		m_connPtr->SetThisid(0, CONNECT_LOGIN);
	}
}

void LoginAccountClient::onDisconnect(const TcpConnectionPtr& conn)
{
	DEBUG("onDisconnect %s -> %s", conn->GetLocalAddr().ToIPPort().c_str(), conn->GetPeerAddr().ToIPPort().c_str());
	m_connPtr.reset();
}

void LoginAccountClient::SendProtoBuf(const uint32 messageid, ::google::protobuf::Message& data)
{
	if (m_connPtr.get())
	{
		m_connPtr->SendProtoBuf(messageid, data);
	}
}

void LoginAccountClient::ParseAccoutVerify(MessagePack* pPack)
{
	Cmd::AccoutVerifyRequest recvPack;
	CHECKERR_AND_RETURN(recvPack.ParseFromArray(pPack->data, pPack->size));
	recvPack.set_cid(pPack->connid);
	SendProtoBuf(LS_ACCOUT_VERIFY_REQUEST, recvPack);
}

void LoginAccountClient::ParseAccoutVerifyReturn(MessagePack* pPack)
{
	Cmd::AccoutVerifyReturn recvPack;
	CHECKERR_AND_RETURN(recvPack.ParseFromArray(pPack->data, pPack->size));
	int64 cid = recvPack.cid();
	uint32 accid = recvPack.accid();
	LoginServer::getInstance().SendPBToClient(CLIENT_ACCOUT_VERIFY_RETURN, cid, recvPack);
	if (recvPack.result() == 0) //账号验证成功
	{
		LoginServer::getInstance().ModifyConnThisid(cid, accid);
	}
}


void LoginAccountClient::ParseAccoutRegister(MessagePack* pPack)
{
	Cmd::AccountRegisterRequest recvPack;
	CHECKERR_AND_RETURN(recvPack.ParseFromArray(pPack->data, pPack->size));
	recvPack.set_cid(pPack->connid);
	SendProtoBuf(LS_ACCOUT_REG_REQUEST, recvPack);
}

void LoginAccountClient::ParseAccoutRegisterReturn(MessagePack* pPack)
{
	Cmd::AccountRegisterReturn recvPack;
	CHECKERR_AND_RETURN(recvPack.ParseFromArray(pPack->data, pPack->size));
	int64 cid = recvPack.cid();
	LoginServer::getInstance().SendPBToClient(CLIENT_ACCOUT_REG_RETURN, cid, recvPack);
}

void LoginAccountClient::ParseRoleList(MessagePack* pPack)
{
	Cmd::RoleListRequest recvPack;
	CHECKERR_AND_RETURN(recvPack.ParseFromArray(pPack->data, pPack->size));
	SendProtoBuf(LS_ROLE_LIST_REQUEST, recvPack);
}

void LoginAccountClient::ParseRoleListReturn(MessagePack* pPack)
{
	Cmd::RoleListReturn recvPack;
	CHECKERR_AND_RETURN(recvPack.ParseFromArray(pPack->data, pPack->size));
	LoginServer::getInstance().SendPBToClient(CLIENT_ROLE_LIST_RETURN, pPack->accid, recvPack);
}

void LoginAccountClient::ParseRoleCreate(MessagePack* pPack)
{
	Cmd::RoleCreateRequest recvPack;
	CHECKERR_AND_RETURN(recvPack.ParseFromArray(pPack->data, pPack->size));
	SendProtoBuf(LS_AS_ROLE_CREATE_REQUEST, recvPack);
}

void LoginAccountClient::ParseRoleCreateReturn(MessagePack* pPack)
{
	Cmd::RoleCreateReturn recvPack;
	CHECKERR_AND_RETURN(recvPack.ParseFromArray(pPack->data, pPack->size));
	LoginServer::getInstance().SendPBToGame(LS_GS_ROLE_CREATE_RETURN, pPack->accid, recvPack, pPack->accid);
}
