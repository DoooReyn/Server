#ifndef __CLIENT_GATE_H__
#define __CLIENT_GATE_H__
#include "tcp_client.h"
#include "thread.h"
#include "inet_address.h"
#include "event_loop.h"
#include "message_queue.h"

class ClientPlayer;

class ClientGateClient
{
public:

	ClientGateClient(EventLoop* pLoop, ClientPlayer* client_player);
	~ClientGateClient();


	void Connect();
	void ParseConnectVerifyReturn(MessagePack* pPack);

	void Init(const string& strLSIP, const uint16& nPort);

	void SetUser(int32 accid, int32 zoneid, int32 roleid);
	void registerMessageHandle();


	void ZoneLogin();
	void ParseZoneLoginReturn(MessagePack* pPack);


private:


	void threadFunc();

	void onConnection(const TcpConnectionPtr& conn);
	void onDisconntion(const TcpConnectionPtr& conn);
	void doMessage(int32 op);

	void TestCmd();
	void TestCmd2();
	void TestRedis();
private:
	TCPClient* m_client;
	EventLoop* m_loop;
	ClientPlayer* m_client_player;
	Thread m_thr;
	bool m_quit;

	int32 m_roleid;
	int32 m_zoneid;
	int32 m_accid;
};

#endif

