#ifndef __LOGIN_SERVER_H__
#define __LOGIN_SERVER_H__
#include "type_define.h"
#include "singleton.h"
#include "xml_parse.h"
#include "tcp_server.h"
#include "event_loop.h"
#include "message_queue.h"
#include "mutex.h"
#include "tcp_client.h"


class GameServer : public SingletonBase<GameServer>
{
public:
	GameServer();
	~GameServer();

	//设置服务器ID
	void SetServerID(uint32 serverid);

	//获取服务器ID
	uint32 GetServerID()
	{
		return m_ServerId;
	};

	//Game服初始化
	bool Init(XMLParse& xmlparse);

	//获取服务器名字
	string GetName() const
	{
		return m_ServerName;
	}

	//消息循环
	void Loop();

	void onUpdate2();
	void onUpdate();
	//服务器退出
	void OnExit(int32 signum);


	//连接确认消息
	void ParseConnectVerifyReturn(MessagePack* pPack);

	//通知登录服地址
	void NotifyLoginGateUrl();
private:

	//链接网关
	void connectGate();

	//链接登陆
	void connectLogin();

	//消息注册
	void registerMessageHandle();

	//初始化Game服配置
	bool initGameServer(XMLParse& xmlparse);

	std::vector<std::shared_ptr<TCPClient> > m_vecTCPGate;  //网关链接
	std::shared_ptr<TCPClient> m_pLogin;					//登录服链接

	std::string m_ServerName;		//服务器名字
	uint32 m_ServerId;				//服务器ID
	EventLoop m_Loop;				//循环事件

	bool m_bSendUrl = false;	    //通知地址是否完成
};



#endif

