#ifndef __CLIENT_PLAYER_H__
#define __CLIENT_PLAYER_H__

#include "event_loop.h"
#include "xml_parse.h"
#include "client_login.h"
#include "client_gate.h"

class ClientPlayer
{
public:

	ClientPlayer();
	~ClientPlayer();


	void InitLogin(XMLParse& xml);
	void InitGate(string ip, uint16 nPort);
	void StartLogin();
	void StartGate();
	void Loop();

	EventLoop m_loop;
	ClientLoginClient m_login_client;
	ClientGateClient m_Gate_client;
};

extern int32 g_zoneid;
extern string g_accname;

#endif
