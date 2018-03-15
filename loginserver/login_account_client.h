#ifndef __LOGIN_ACCOUNT_H__
#define __LOGIN_ACCOUNT_H__

#include "type_define.h"
#include "singleton.h"
#include "xml_parse.h"
#include "callback.h"
#include "message_queue.h"


class LoginAccountClient : public SingletonBase<LoginAccountClient>
{
public:
	LoginAccountClient();
	~LoginAccountClient();

	//初始化
	bool Init(XMLParse& xmlparse);

	//消息注册
	void registerMessageHandle();

	//账号服IP
	string GetIP()
	{
		return m_strIP;
	}

	//账号服端口
	uint32 GetPort()
	{
		return m_nPort;
	}

	//账号服链接成功回调
	void onConnection(const TcpConnectionPtr& conn);

	//账号服断开链接回调
	void onDisconnect(const TcpConnectionPtr& conn);

	//PB发送
	void SendProtoBuf(const uint32 messageid, ::google::protobuf::Message& data);

	//===================================================
	//client 请求账号验证 发送到AS
	void ParseAccoutVerify(MessagePack* pPack);

	//账号验证结果 AS发送过来
	void ParseAccoutVerifyReturn(MessagePack* pPack);

	//client 请求账号注册 发送到AS
	void ParseAccoutRegister(MessagePack* pPack);

	//账号注册结果 AS发送过来
	void ParseAccoutRegisterReturn(MessagePack* pPack);

	//client 请求角色列表 发送到AS
	void ParseRoleList(MessagePack* pPack);

	//角色列表结果 AS发送过来
	void ParseRoleListReturn(MessagePack* pPack);

	//client 请求角色创建 发送到AS
	void ParseRoleCreate(MessagePack* pPack);

	//角色创建结果 AS发送过来
	void ParseRoleCreateReturn(MessagePack* pPack);

private:
	std::string m_strIP;		//账号服IP
	uint32 m_nPort;				//账号服端口
	TcpConnectionPtr m_connPtr;	//账号服链接
};

#endif
