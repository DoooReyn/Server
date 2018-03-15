#ifndef __LOGIN_SERVER_H__
#define __LOGIN_SERVER_H__
#include "type_define.h"
#include "singleton.h"
#include "xml_parse.h"
#include "tcp_server.h"
#include "event_loop.h"
#include "message_queue.h"
#include "mutex.h"
#include "login_zone_info.h"


class LoginServer : public SingletonBase<LoginServer>
{
public:
	LoginServer();
	~LoginServer();

	//Login 初始化
	bool Init(XMLParse& xmlparse);

	//获取Login服名称
	string GetName() const
	{
		return m_ServerName;
	}

	//主循环
	void Loop();
	//消息解析
	void onUpdate();
	//服务器结束
	void OnExit(int32 signum);

	//发送PB结构到GameServer
	void SendPBToGame(const uint32 messageid, int64 cid, ::google::protobuf::Message& data, int32 uid = 0);

	//发送PB结构到客户端
	void SendPBToClient(const uint32 messageid, int64 cid, ::google::protobuf::Message& data);

	//修改链接的标示ID
	void ModifyConnThisid(int64 cid, uint32 accid);



	//区服登录请求
	void ParseZoneInfoRequest(MessagePack* pPack);

	//account 登陆返回
	void ParseConnectVerifyReturn(MessagePack* pPack);

	//角色创建返回
	void ParseRoleCreateReturn(MessagePack* pPack);

private:
	//消息注册函数
	void registerMessageHandle();

	//初始化Login服相关的配置文件
	bool initLoginServer(XMLParse& xmlparse);

	//初始化Account服相关的配置文件
	bool initAccountServer(XMLParse& xmlparse);

	//TCP链接成功回调
	void onConnection(const TcpConnectionPtr& conn);
	//链接状态添加or删除
	void ConnectChange(int32 conntype, const TcpConnectionPtr& conn, bool status);
	//TCP链接断开回调
	void onDisconnect(const TcpConnectionPtr& conn);

	//链接端链接确认
	void onVerify(const TcpConnectionPtr& conn, int32 id, int32 conntype);




	std::string m_ServerName;			//区服名称
	uint32 m_nMaxThread;				//线程池最大线程数
	uint32 m_nPort;						//LoginServer 监控端口号
	EventLoop m_Loop;					//主循环
	//Mutex m_mutexConn;				//网络连接锁
	MapConnPtr m_mapClientConnPtr;		//Client网络连接Map
	MapConnPtr m_mapGateConnPtr;		//gate网络连接Map
	TcpConnectionPtr m_GameConnPtr;		//game网络链接
};



#endif

