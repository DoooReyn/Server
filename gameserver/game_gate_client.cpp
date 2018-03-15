#include "game_gate_client.h"
#include "string_tool.h"
#include "command.h"
#include "logger.h"
#include "tcp_connection.h"
#include "network_manager.h"
#include "platcmd.pb.h"
#include "game_server.h"
#include "mysql_pool.h"


extern std::map<int32, int32> g_map_parma;


GameGateClient::GameGateClient()
{
	registerMessageHandle();
}

GameGateClient::~GameGateClient()
{

}


void GameGateClient::registerMessageHandle()
{
	NetworkManager::getInstance().RegisterMessageHandle(CMD_CONNECT_TEST1, &GameGateClient::TestMessage1, this);
	NetworkManager::getInstance().RegisterMessageHandle(CMD_CONNECT_TEST2, &GameGateClient::TestMessage2, this);
}


void GameGateClient::onConnection(const TcpConnectionPtr& conn)
{
	DEBUG("onConnection %s -> %s", conn->GetLocalAddr().ToIPPort().c_str(), conn->GetPeerAddr().ToIPPort().c_str());

	CmdConnectVerifyRequest sendPack;
	sendPack.id = GameServer::getInstance().GetServerID();
	sendPack.conntype = CONNECT_GAME;
	conn->Send(CMD_CONNECT_VERIFY_REQUEST, &sendPack, sizeof(sendPack));


	uint32 gateid = GameGateClient::getInstance().GetGateId(conn->GetPeerAddr().ToIPPort());
	conn->SetThisid(gateid, CONNECT_GAME);

	m_mapGateConnPtr.insert(std::make_pair(conn->GetThisid(), conn));
}

void GameGateClient::onDisconnect(const TcpConnectionPtr& conn)
{
	DEBUG("onDisconnect %s -> %s is %d Pid:%d", conn->GetPeerAddr().ToIPPort().c_str(), conn->GetLocalAddr().ToIPPort().c_str(), conn->Connected(), CurrentThread::Tid());
	m_mapGateConnPtr.erase(conn->GetThisid());
}





void GameGateClient::TestMessage1(MessagePack* pPack)
{
	string str(pPack->data, pPack->size);
	if (str == "WL Test:0" || str == "WL Test:99999")
	{
		DEBUG("1234 message:%s", str.c_str());
	}
	static int64 start = 0;
	static int32 count = 0;
	static int32 timecost = 0;
	if (str == "WL Test:0")
	{
		start = Timestamp::Now().GetMicroSeconds();
	}
	if (str == "WL Test:99999")
	{
		int64 time2 = Timestamp::Now().GetMicroSeconds();
		count++;
		timecost += time2 - start;
		WARN("TestMessage count:%d average:%f", count, double(timecost) / count);
	}
	//g_map_parma[1]++;
}

void GameGateClient::TestMessage2(MessagePack* pPack)
{
	Cmd::CmdTest recvPack;
	CHECKERR_AND_RETURN(recvPack.ParseFromArray(pPack->data, pPack->size));
	if (recvPack.str() == "我们 Test:0" || recvPack.str() == "我们 Test:99999")
	{
		DEBUG("5678 message:%s", recvPack.str().c_str());
	}
	g_map_parma[1]++;
}



bool GameGateClient::Init(XMLParse& xmlparse)
{
	MysqlPool::newInstance();
	string strMyql = xmlparse.GetNode(GameServer::getInstance().GetName(), "MySQL");
	bool bRet = MysqlPool::getInstance().PutUrl(strMyql.c_str());
	if (bRet == false)
	{
		ERROR("Mysql Init Failed strMyql:%s ServerId:%d", strMyql.c_str(), GameServer::getInstance().GetServerID());
		return false;
	}

	m_mapGateInfo.clear();
	uint32 serverid = GameServer::getInstance().GetServerID();
	string sql = "SELECT \
		c.id, \
		c.gateway_id, \
		d.ip_zone, \
		d.ip_user, \
		d.port \
		FROM \
		( \
			SELECT \
			a.id, \
			a.`name`, \
			b.gateway_id \
			FROM \
			zone_infos a \
			LEFT JOIN zone_infos_gateways b ON a.id = b.zone_info_id \
			WHERE \
			a.id = %d \
			OR a.new_zone_id = %d \
		) c\
		LEFT JOIN gateways d ON c.gateway_id = d.id";

	StringTool::Format(sql, sql.c_str(), serverid, serverid);

	AutoHandle handle(MysqlPool::getInstancePtr());
	DataSet data;
	handle()->ExecSelect(sql, data);
	do
	{
		if (data.Size() == 0)
		{
			bRet = false;
			ERROR("Gate List is null");
			break;
		}
		for (uint32 i = 0; i < data.Size(); i++)
		{
			uint32 gateid = data.GetValue(i, "gateway_id");
			string ip_zone = data.GetValue(i, "ip_zone");
			string ip_user = data.GetValue(i, "ip_user");
			uint32 port = data.GetValue(i, "port");

			GateInfo gate_info(ip_zone, ip_user, port);
			m_mapGateInfo.insert(std::make_pair(gateid, gate_info));
		}
	}
	while (false);
	MysqlPool::delInstance();
	return bRet;
}


void GameGateClient::SendPBToGate(int64 cid, uint32 messageid, ::google::protobuf::Message& data)
{
	itConnPtr itFind = m_mapGateConnPtr.find(cid);
	if (itFind == m_mapGateConnPtr.end())
	{
		ERROR("SendPBToGW Cid:%ld Not Find", cid);
	}
	else
	{
		itFind->second->SendProtoBuf(messageid, data);
	}
}


bool GameGateClient::IsConnected()
{
	return m_mapGateInfo.size() == m_mapGateConnPtr.size();
}

std::string GameGateClient::GetIPPort(uint32 id)
{
	MapGateinfo::iterator itFind = m_mapGateInfo.find(id);
	if (itFind == m_mapGateInfo.end())
	{
		ERROR("GetIPPort Error gateid:%d", id);
		return "";
	}
	return StringTool::Format("%s:%d", itFind->second.m_ipuser, itFind->second.m_port);
}


uint32 GameGateClient::GetGateId(string ipport)
{
	uint32 gateid = 0;
	MapGateinfo::iterator it = m_mapGateInfo.begin();
	for (; it != m_mapGateInfo.end(); ++it)
	{
		string ip = StringTool::Format("%s:%d", it->second.m_ipzone, it->second.m_port);
		if (ipport == ip)
		{
			gateid = it->first;
		}
	}
	if (gateid == 0)
	{
		ERROR("GetGateId Error ipport:%s", ipport.c_str());
	}

	return gateid;
}





