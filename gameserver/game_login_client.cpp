
#include "game_login_client.h"
#include "logger.h"
#include "network_manager.h"
#include "string_tool.h"
#include "command.h"
#include "platcmd.pb.h"


GameLoginClient::GameLoginClient()
{
	registerMessageHandle();
}

GameLoginClient::~GameLoginClient()
{

}


void GameLoginClient::registerMessageHandle()
{
	NetworkManager::getInstance().RegisterMessageHandle(LS_GS_ROLE_CREATE_RETURN, &GameLoginClient::ParseRoleCreateReturn, this);
}

bool GameLoginClient::Init(XMLParse& xmlparse)
{
	m_strIP = xmlparse.GetNode(LS, "IP");
	m_nPort = StringTool::StoI(xmlparse.GetNode(LS, "Port"));
	return true;
}


void GameLoginClient::onConnection(const TcpConnectionPtr& conn)
{
	DEBUG("tid:%d onConnection %s -> %s", CurrentThread::Tid(), conn->GetLocalAddr().ToIPPort().c_str(), conn->GetPeerAddr().ToIPPort().c_str());
	if (conn->Connected())
	{
		m_connPtr = conn;
		CmdConnectVerifyRequest sendPack;
		sendPack.id = 0;
		sendPack.conntype = CONNECT_GAME;
		m_connPtr->Send(CMD_CONNECT_VERIFY_REQUEST, &sendPack, sizeof(sendPack));
		m_connPtr->SetThisid(0, CONNECT_GAME);
	}
}

void GameLoginClient::onDisconnect(const TcpConnectionPtr& conn)
{
	DEBUG("onDisconnect %s -> %s", conn->GetLocalAddr().ToIPPort().c_str(), conn->GetPeerAddr().ToIPPort().c_str());
	m_connPtr.reset();
}

void GameLoginClient::ParseRoleCreateReturn(MessagePack* pPack)
{
	Cmd::RoleCreateReturn recvPack;
	CHECKERR_AND_RETURN(recvPack.ParseFromArray(pPack->data, pPack->size));
	DEBUG("ParseRoleCreateReturn zoneid:%d accid:%d name:%s roleid:%d", recvPack.zoneid(), recvPack.accid(), recvPack.name().c_str(), recvPack.roleid());
	SendPBToLogin(LS_GS_ROLE_CREATE_REQUEST, recvPack);
}


void GameLoginClient::SendPBToLogin(const uint32 messageid, ::google::protobuf::Message& data, int32 uid /*= 0*/)
{
	if (m_connPtr.get())
	{
		m_connPtr->SendProtoBuf(messageid, data, uid);
	}
}

