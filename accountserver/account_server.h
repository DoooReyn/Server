#ifndef __LOGIN_SERVER_H__
#define __LOGIN_SERVER_H__
#include "type_define.h"
#include "singleton.h"
#include "xml_parse.h"
#include "tcp_server.h"
#include "event_loop.h"
#include "message_queue.h"


class AccountServer : public SingletonBase<AccountServer>
{
public:
	AccountServer();
	~AccountServer();

	bool Init(XMLParse& xmlparse);
	string GetName() const { return m_ServerName; }
	void Loop();
	void OnUpdate();
	void OnExit(int32 signum);

	//链接端链接确认
	void onVerify(const TcpConnectionPtr& conn, int32 id, int32 conntype);

	void ParseAccoutVerify(MessagePack* pPack);
	void ParseAccoutRegister(MessagePack* pPack);
	void ParseRoleList(MessagePack* pPack);
	void ParseRoleCreate(MessagePack* pPack);

private:

	void registerMessageHandle();
	void onConnection(const TcpConnectionPtr& conn);
	void onDisconnect(const TcpConnectionPtr& conn);
	void SendPBToLS(const uint32 messageid, ::google::protobuf::Message& data, uint32 accid = 0);


private:
	std::string m_ServerName;
	uint32 m_nMaxThread;
	uint32 m_nPort;
	TcpConnectionPtr m_connPtr;
	EventLoop m_Loop;
};



#endif

