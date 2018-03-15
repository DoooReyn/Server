#ifndef __GAME_GATE_CLIENT_H__
#define __GAME_GATE_CLIENT_H__

#include "type_define.h"
#include "singleton.h"
#include "xml_parse.h"
#include "callback.h"
#include "message_queue.h"

//网关信息
struct GateInfo
{

	GateInfo(string& ipzone, string& ipuser, uint32 port)
	{
		m_port = port;
		strncpy(m_ipzone, ipzone.c_str(), sizeof(m_ipzone));
		strncpy(m_ipuser, ipuser.c_str(), sizeof(m_ipuser));
	}

	GateInfo& operator=(const GateInfo& info)
	{
		m_port = info.m_port;
		strncpy(m_ipzone, info.m_ipzone, sizeof(m_ipzone));
		strncpy(m_ipuser, info.m_ipuser, sizeof(m_ipuser));
		return *this;
	}

	uint32 m_port = 0;				//网关端口号
	char m_ipzone[128] = {0};		//网关内网IP
	char m_ipuser[128] = {0};		//网关外网IP
};
typedef std::map<uint32, GateInfo> MapGateinfo;

class GameGateClient : public SingletonBase <GameGateClient>
{
public:
	GameGateClient();
	~GameGateClient();

	//初始化
	bool Init(XMLParse& xmlparse);



	//Gate服链接成功回调
	void onConnection(const TcpConnectionPtr& conn);

	//Gate服链接端口回调
	void onDisconnect(const TcpConnectionPtr& conn);

	//发送消息到网关
	void SendPBToGate(int64 cid, uint32 messageid, ::google::protobuf::Message& data);

	//消息注册
	void registerMessageHandle();

	//链接是否完成
	bool IsConnected();

	MapGateinfo& GetGateInfoMap()
	{
		return  m_mapGateInfo;
	}

	MapConnPtr& GetGateConnectMap()
	{
		return  m_mapGateConnPtr;
	}


	//获取网关的外卖IP端口
	string GetIPPort(uint32 id);
	//通过ipport获取gateid
	uint32 GetGateId(string ipport);



	//测试消息
	void TestMessage1(MessagePack* pPack);
	void TestMessage2(MessagePack* pPack);


private:
	MapGateinfo m_mapGateInfo;		//网关信息
	MapConnPtr m_mapGateConnPtr;	//链接Map

};

#endif
