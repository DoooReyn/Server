#include "client_player.h"
#include "network_manager.h"
#include "string_tool.h"


ClientPlayer::ClientPlayer()
	: m_login_client(&m_loop, this)
	, m_Gate_client(&m_loop, this)
{
}

ClientPlayer::~ClientPlayer()
{

}

void ClientPlayer::StartLogin()
{
	m_login_client.Connect();
}

void ClientPlayer::StartGate()
{
	m_Gate_client.Connect();
}

void ClientPlayer::InitLogin(XMLParse& xml)
{
	string strLSIP = xml.GetNode("LS", "IP");
	uint16 nPort = StringTool::StoI(xml.GetNode("LS", "Port"));
	m_login_client.Init(strLSIP, nPort);
}

void ClientPlayer::InitGate(string ip, uint16 nPort)
{
	m_Gate_client.Init(ip, nPort);
}

void ClientPlayer::Loop()
{
	m_loop.Loop();
}