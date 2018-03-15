#ifndef __CLIENT_LOGIN_H__
#define __CLIENT_LOGIN_H__
#include "tcp_client.h"
#include "thread.h"
#include "inet_address.h"
#include "event_loop.h"
#include "message_queue.h"

#define CLIENT	"Client"


struct ZoneInfo
{
	uint32 zoneid = 0;
	uint32 state = 0;
	char name[128] = {0};
};

typedef std::map<uint32, ZoneInfo> MapZoneInfo;

class ClientPlayer;

class ClientLoginClient
{
public:
	ClientLoginClient(EventLoop* pLoop, ClientPlayer* client_player);
	~ClientLoginClient() ;
	void Init(const string& strLSIP, const uint16& nPort);

	void Connect();
	void ParseConnectVerifyReturn(MessagePack* pPack);

	void AccountVerify();
	void ParseAccoutVerifyReturn(MessagePack* pPack);

	void AccountReg();
	void ParseAccoutRegReturn(MessagePack* pPack);

	void ZoneRequest();
	void ParseZoneListReturn(MessagePack* pPack);

	void ZoneSelect();
	void ParseZoneInfoReturn(MessagePack* pPack);

	void RoleRequest();
	void ParseRoleListReturn(MessagePack* pPack);

	void RoleCreate();
	void ParseRoleCreateReturn(MessagePack* pPack);
private:


	void registerMessageHandle();
	void unregisterMessageHandle();

	void threadFunc();

	void onConnection(const TcpConnectionPtr& conn);
	void onDisconntion(const TcpConnectionPtr& conn);
	void doMessage(int32 op);

private:
	int32 m_conn_type;
	TCPClient* m_client;
	EventLoop* m_loop;
	ClientPlayer* m_client_player;
	TimerId m_Timerid;
	Thread m_thr;
	bool m_quit;
	MapZoneInfo m_mapZoneList;

	int32 m_zoneid;
	int32 m_accid;
	string m_gateurl;
	uint16 m_gatePort;
	const int32 m_mode = 0;
};

#endif