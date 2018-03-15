#ifndef __LOGIN_ZONE_INFO_H__
#define __LOGIN_ZONE_INFO_H__

#include "type_define.h"

struct ZoneData
{
	uint32 zoneid = 0;		//区服ID
	uint32 newzoneid = 0;	//区服ID2
	uint32 state = 0;		//区别状态
	char zonename[128] = {0}; //区服名称
};

typedef std::map<uint32, ZoneData> MapZoneList;
typedef MapZoneList::iterator  MapZoneIter;



struct GateData
{
	uint32 zoneid = 0;		//区服id
	uint32 gateid = 0;		//网关id
	char gateurl[128] = {0};	//网关地址
	uint32 onlinenum = 0;	//网关上的人数
};

typedef std::map<uint32, GateData> MapGateList;
typedef MapGateList::iterator  MapGateIter;

#endif