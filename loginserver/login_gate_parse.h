#ifndef __LOGIN_GATE_PARSE_H__
#define __LOGIN_GATE_PARSE_H__

#include "singleton.h"
#include "login_zone_info.h"
#include "mutex.h"
#include "message_queue.h"

class GateLoginParse : public SingletonBase < GateLoginParse >
{
public:
	GateLoginParse();
	~GateLoginParse();
	//消息注册
	void registerMessageHandle();
	//初始化区服列表
	bool InitZoneList();

	//获取网关区服人数最少的一个
	GateData* GetBestGate(uint32 zoneid);

	//设置区服状态
	void ParseZoneStateChange(MessagePack* pPack);

	//区服列表通知
	void ParseZoneStateRequest(MessagePack* pPack);

	//网关地址设置
	void ParseGateUrlNotify(MessagePack* pPack);

private:
	MapZoneList m_mapZoneList;			//区服列表
	MapGateList m_mapGateList;			//网关列表
};


#endif