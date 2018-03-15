#include "login_gate_parse.h"
#include "mysql_pool.h"
#include "logger.h"
#include "network_manager.h"
#include "login_server.h"
#include "command.h"
#include "platcmd.pb.h"

GateLoginParse::GateLoginParse()
{
	m_mapZoneList.clear();
	m_mapGateList.clear();

	registerMessageHandle();
}

GateLoginParse::~GateLoginParse()
{

}

void GateLoginParse::registerMessageHandle()
{
	NetworkManager::getInstance().RegisterMessageHandle(CMD_ZONE_STATE_CHANGE, &GateLoginParse::ParseZoneStateChange, this);
	NetworkManager::getInstance().RegisterMessageHandle(GS_GATE_URL_SENDTO_LOGIN, &GateLoginParse::ParseGateUrlNotify, this);
	NetworkManager::getInstance().RegisterMessageHandle(CLIENT_ZONE_LIST_REQUEST, &GateLoginParse::ParseZoneStateRequest, this);
}



bool GateLoginParse::InitZoneList()
{
	string sql = "select id,name,new_zone_id,state from zone_infos";
	AutoHandle handle(MysqlPool::getInstancePtr());

	DataSet data;
	bool bRet = handle()->ExecSelect(sql, data);
	if (bRet == false)
	{
		ERROR("initZoneList sql error");
		return false;
	}

	if (data.Size() == 0)
	{
		ERROR("mysql table zone_info is null");
		return false;
	}

	for (uint32 i = 0; i < data.Size(); i++)
	{
		ZoneData zone_list;
		zone_list.zoneid = data.GetValue(i, "id");
		zone_list.newzoneid = data.GetValue(i, "new_zone_id");
		zone_list.state = data.GetValue(i, "state");
		string zonename = data.GetValue(i, "name");
		strncpy(zone_list.zonename, zonename.c_str(), sizeof(zone_list.zonename));
		m_mapZoneList.insert(std::make_pair(zone_list.zoneid, zone_list));
		//INFO("Id:%d Name:%s NewId:%d State:%d", zone_list.zoneid, zone_list.zonename, zone_list.newzoneid, zone_list.state);
	}

	return true;
}

GateData* GateLoginParse::GetBestGate(uint32 zoneid)
{
	GateData* gw = NULL;
	MapGateIter it = m_mapGateList.begin();
	for (; it != m_mapGateList.end(); ++it)
	{
		if (it->second.zoneid != zoneid)
		{
			continue;
		}

		if (gw == NULL)
		{
			gw = &it->second;
		}
		else if (gw->onlinenum > it->second.onlinenum)
		{
			gw = &it->second;
		}
	}
	return gw;
}

void GateLoginParse::ParseZoneStateChange(MessagePack* pPack)
{
	Cmd::CmdZoneStateChange recvPack;
	CHECKERR_AND_RETURN(recvPack.ParseFromArray(pPack->data, pPack->size));
	MapZoneIter itFind = m_mapZoneList.find(recvPack.zoneid());
	if (itFind == m_mapZoneList.end())
	{
		ERROR("ParseZoneState Error zoneid:%d", recvPack.zoneid());
	}
	else
	{
		itFind->second.state = recvPack.state();
	}
	DEBUG("ParseZoneState Success id:%d state:%d", recvPack.zoneid(), recvPack.state());
}

void GateLoginParse::ParseGateUrlNotify(MessagePack* pPack)
{
	Cmd::GateUrlNotify recvPack;
	CHECKERR_AND_RETURN(recvPack.ParseFromArray(pPack->data, pPack->size));

	MapGateIter itFind = m_mapGateList.find(recvPack.gateid());
	if (itFind == m_mapGateList.end())
	{
		GateData gate_list;
		gate_list.zoneid = recvPack.zoneid();
		gate_list.gateid = recvPack.gateid();
		strncpy(gate_list.gateurl, recvPack.gateurl().c_str(), sizeof(gate_list.gateurl));
		m_mapGateList.insert(std::make_pair(gate_list.gateid, gate_list));
	}
	else
	{
		itFind->second.gateid = recvPack.gateid();
		itFind->second.zoneid = recvPack.zoneid();
		strncpy(itFind->second.gateurl, recvPack.gateurl().c_str(), sizeof(itFind->second.gateurl));
	}
}

void GateLoginParse::ParseZoneStateRequest(MessagePack* pPack)
{
	Cmd::ZoneListReturn sendPack;
	MapZoneIter itBegin = m_mapZoneList.begin();
	for (; itBegin != m_mapZoneList.end(); ++itBegin)
	{
		Cmd::ZoneList* pZoneList = sendPack.add_zonelist();
		CHECKERR_AND_CONTINUE(pZoneList != NULL);
		pZoneList->set_zoneid(itBegin->second.zoneid);
		pZoneList->set_state(itBegin->second.state);
		pZoneList->set_name(itBegin->second.zonename);
	}
	LoginServer::getInstance().SendPBToClient(CLIENT_ZONE_LIST_RETURN, pPack->connid, sendPack);
	//DEBUG("ParseZoneStateRequest");
}
