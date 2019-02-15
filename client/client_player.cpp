#include "client_player.h"
#include "network_manager.h"
#include "signal_catch.h"

void onServerQuit(int signnum)
{
	ClientPlayer::getInstance().Quit(signnum);
}

ClientPlayer::ClientPlayer()
	: m_login_client(&m_loop, this)
	, m_Gate_client(&m_loop, this)
{
	CatchCallback cb = onServerQuit;
	SetSignedCatched(cb);
}

ClientPlayer::~ClientPlayer()
{

}

void ClientPlayer::Quit(int signnum)
{
	m_loop.Quit();
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
	string strLSIP = xml.GetNode("Client", "LOGGIN_IP");
	uint16 nPort = stoi(xml.GetNode("Client", "LOGGIN_PORT"));
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
