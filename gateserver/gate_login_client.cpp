#include "gate_login_client.h"
#include "string_tool.h"
#include "common.h"
#include "command.h"
#include "logger.h"
#include "tcp_connection.h"
#include "network_manager.h"
#include "platcmd.pb.h"
#include "gate_server.h"

GateLoginClient::GateLoginClient()
{
	registerMessageHandle();
}

GateLoginClient::~GateLoginClient()
{

}


void GateLoginClient::registerMessageHandle()
{
	//这里注册的消息要写到 OnTransmit 判断类型中，不需要转发
	NetworkManager::getInstance().RegisterMessageHandle(CMD_CONNECT_VERIFY_RETURN, &GateLoginClient::ParseConnectVerifyReturn, this);
}

bool GateLoginClient::Init(XMLParse& xmlparse)
{
	m_strIP = xmlparse.GetNode(LS, "IP");
	m_nPort = StringTool::StoI(xmlparse.GetNode(LS, "Port"));

	//INFO("LoginServer IP:%s Port:%d", m_strIP.c_str(), m_nPort);
	return true;
}


void GateLoginClient::onConnection(const TcpConnectionPtr& conn)
{
	DEBUG("onConnection %s -> %s", conn->GetLocalAddr().ToIPPort().c_str(), conn->GetPeerAddr().ToIPPort().c_str());
	if (conn->Connected())
	{
		m_connPtr = conn;
		CmdConnectVerifyRequest sendPack;
		sendPack.id =  GateServer::getInstance().GetServerID();
		sendPack.conntype = CONNECT_GATE;
		conn->Send(CMD_CONNECT_VERIFY_REQUEST, &sendPack, sizeof(sendPack));
		m_connPtr->SetThisid(GateServer::getInstance().GetServerID(), CONNECT_GATE);
	}
}

void GateLoginClient::onDisconnect(const TcpConnectionPtr& conn)
{
	INFO("onDisconnect %s -> %s", conn->GetLocalAddr().ToIPPort().c_str(), conn->GetPeerAddr().ToIPPort().c_str());
	m_connPtr.reset();
}

void GateLoginClient::SendProtoBuf(const uint32 messageid, ::google::protobuf::Message& data)
{
	if (m_connPtr.get())
	{
		m_connPtr->SendProtoBuf(messageid, data);
	}
}

void GateLoginClient::Send(uint32 messageid, uint32 nLen, const char* data)
{
	if (m_connPtr.get())
	{
		m_connPtr->Send(messageid, data, nLen);
	}
}


void GateLoginClient::ParseConnectVerifyReturn(MessagePack* pPack)
{
	CHECKERR_AND_RETURN(sizeof(CmdConnectVerifyReturn) == pPack->size);
	CmdConnectVerifyReturn* recvPack = (CmdConnectVerifyReturn*)pPack->data;
	INFO("Verify Return Src IP:%s", recvPack->src);
}

void GateLoginClient::SetZoneState(uint32 zoneid, uint32 state)
{
	Cmd::CmdZoneStateChange sendPack;
	sendPack.set_zoneid(zoneid);
	sendPack.set_state(state);
	SendProtoBuf(CMD_ZONE_STATE_CHANGE, sendPack);
}

