#ifndef __MESSAGE_QUEUE_H__
#define __MESSAGE_QUEUE_H__

#include "type_define.h"

#define MAX_QUEUE_SIZE 4096

struct MessagePack
{
public:
	MessagePack()
		: messageid(0)
		, size(0)
		, connid(0)
		, accid(0)
		, data(nullptr)
	{
	}
	MessagePack(uint32 mid, uint32 nLen, const int64 cid, uint32 account, char* cmd)
		: messageid(mid)
		, size(nLen)
		, connid(cid)
		, accid(account)
		, data(cmd)
	{
	}

	uint32 messageid;		//消息id
	uint32 size;			//数据长度
	int64 connid;			//链接标示
	uint32 accid;			//账号id
	char* data;				//数据指针
};

typedef pair<volatile bool, MessagePack> CmdQueue;

class MsgQueue
{
public:
	MsgQueue();
	~MsgQueue();

	bool Put(uint32 mid, uint32 nLen, int64 cid, uint32 accid, const char* cmd);
	MessagePack* Get();
	void Erase();
	void GetAndSwapTo(MsgQueue& msg_queue);

	void Print()
	{
		cout << "read:" << m_cmd_read << " write:" << m_cmd_write << " size:" << m_queue.size() << endl;
	}
	void Clear();
private:
	bool PutQueueToArray();

	CmdQueue m_cmd[MAX_QUEUE_SIZE];
	std::queue<MessagePack> m_queue;

	uint32 m_cmd_read;
	uint32 m_cmd_write;
};
#endif
