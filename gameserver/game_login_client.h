#ifndef __GAME_LOGIN_CLIENT_H__
#define __GAME_LOGIN_CLIENT_H__

#include "type_define.h"
#include "singleton.h"
#include "xml_parse.h"
#include "callback.h"
#include "mutex.h"
#include "message_queue.h"

class GameLoginClient : public SingletonBase <GameLoginClient>
{
public:
	GameLoginClient();
	~GameLoginClient();

	//初始化
	bool Init(XMLParse& xmlparse);

	//登陆服IP
	string GetIP()
	{
		return m_strIP;
	}

	//账号服端口
	uint32 GetPort()
	{
		return m_nPort;
	}

	bool IsConnected()
	{
		return m_connPtr.get() != nullptr;
	}


	//登陆服链接成功回调
	void onConnection(const TcpConnectionPtr& conn);


	//登陆服断开链接回调
	void onDisconnect(const TcpConnectionPtr& conn);

	//角色创建 LS 发过来的
	void ParseRoleCreateReturn(MessagePack* pPack);

	//发送消息到Login服
	void SendPBToLogin(const uint32 messageid, ::google::protobuf::Message& data, int32 uid = 0);
private:
	//消息注册
	void registerMessageHandle();

private:
	Mutex m_mutex;
	std::string m_strIP;		//登陆服IP
	uint32 m_nPort;				//登陆服端口
	TcpConnectionPtr m_connPtr;	//登陆服链接
};

#endif
