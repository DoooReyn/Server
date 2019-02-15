#ifndef __GATE_LOGIN_CLIENT_H__
#define __GATE_LOGIN_CLIENT_H__

#include "type_define.h"
#include "singleton.h"
#include "xml_parse.h"
#include "callback.h"
#include "message_queue.h"

//Gate服链接 Login服 封装类
class GateLoginClient : public SingletonBase<GateLoginClient>
{
public:
	GateLoginClient();
	~GateLoginClient();

	//初始化
	bool Init(XMLParse& xmlparse);

	//消息注册函数
	void registerMessageHandle();

	//获取登录服IP
	string GetIP()
	{
		return m_strIP;
	}

	//获取登录服端口号
	uint32 GetPort()
	{
		return m_nPort;
	}

	void NotifyLoginGateStatus();

	//Login服链接成功回调
	void onConnection(const TcpConnectionPtr& conn);

	//Login服断开链接回调
	void onDisconnect(const TcpConnectionPtr& conn);

	//发送消息
	void SendProtoBuf(const uint32 messageid, ::google::protobuf::Message& data);

	void Send(uint32 messageid, uint32 nLen, const char* data);

	//===================================================
	//Login服 回复链接确认消息
	void ParseConnectVerifyReturn(MessagePack* pPack);

	//设置Game服的状态
	void SetZoneState(uint32 zoneid, uint32 state);

private:
	std::string m_strIP;
	uint32 m_nPort;
	TcpConnectionPtr m_connPtr;
};

#endif
