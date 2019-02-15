#ifndef __LOGIN_SERVER_H__
#define __LOGIN_SERVER_H__
#include "type_define.h"
#include "singleton.h"
#include "xml_parse.h"
#include "tcp_server.h"
#include "event_loop.h"
#include "message_queue.h"
#include "mutex.h"



class GateServer : public SingletonBase<GateServer>
{
public:
	GateServer();
	~GateServer();

	//设置网关ID
	void SetServerID(uint32 serverid);

	void GetConnectZoneId(vector<int32>& vecZone);

	//获取网关ID
	uint32 GetServerID()
	{
		return m_ServerId;
	};

	//网关服务器初始化
	bool Init(XMLParse& xmlparse);

	//获取网关服务器名字
	string GetName() const
	{
		return m_ServerName;
	}

	//消息循环
	void Loop();

	//消息解析入口
	void OnUpdate();
	void OnUpdate2();
	//消息转发入口
	bool OnTransmit(const TcpConnectionPtr& conn, PackHead* pHead, const char* data);

	//服务器退出
	void OnExit(int32 signum);

	//发送数据
	void SendProtoBuf(const uint32 messageid, int64 cid, ::google::protobuf::Message& data, uint32 messtype);
	void Send(const TcpConnectionPtr& conn, PackHead* pHead, const char* data);

	void ParseZoneLoginReturn(MessagePack* pPack);

private:

	//消息注册
	void registerMessageHandle();

	//网关配置
	bool initGateServer(XMLParse& xmlparse);

	//登陆服配置
	bool initLoginServer(XMLParse& xmlparse);

	//链接成功回调
	void onConnection(const TcpConnectionPtr& conn);
	//链接状态添加还是删除
	void ConnectChange(int32 conntype, const TcpConnectionPtr& conn, bool status);
	//链接端口回调
	void onDisconnect(const TcpConnectionPtr& conn);

	//client链接确认回调
	void onVerify(const TcpConnectionPtr& conn, int32 id, int32 conntype);

	//通过消息ID 判断消息是发往哪个服务器
	uint32 GetMessageType(uint32 messageid);



	std::string		m_ServerName;	//服务器名称
	uint32			m_nMaxThread;	//最大线程数
	uint32			m_ServerId;		//服务器ID
	uint32			m_nPort;		//端口号
	EventLoop		m_Loop;			//主循环事件

	//Mutex m_mutexConn;					//Client 连接锁
	MapConnPtr m_mapGameConnPtr;		//Game 和网关连接
	MapConnPtr m_mapClientConnPtr;		//Client 和网关连接
	std::map<int32, int32> m_mapAccidZone; //登陆区服关联
};



#endif

