#include "message_queue.h"

MsgQueue::MsgQueue()
{
	m_cmd_write = 0;
	m_cmd_read = 0;
}
MsgQueue::~MsgQueue()
{
	Clear();
}

bool MsgQueue::Put(uint32 mid, uint32 nLen, const int64 cid, uint32 accid, const char* cmd)
{
	char* cmd_buf  = new char[nLen];
	if (cmd_buf == NULL)
	{
		return false;
	}
	memcpy(cmd_buf, cmd, nLen);

	if(PutQueueToArray() && m_cmd[m_cmd_write].first == false)
	{
		m_cmd[m_cmd_write].first = true;
		m_cmd[m_cmd_write].second.messageid = mid;
		m_cmd[m_cmd_write].second.size = nLen;
		m_cmd[m_cmd_write].second.connid = cid;
		m_cmd[m_cmd_write].second.accid = accid;
		m_cmd[m_cmd_write].second.data = cmd_buf;
		m_cmd_write = (m_cmd_write + 1) % MAX_QUEUE_SIZE;
	}
	else
	{
		m_queue.push(MessagePack(mid, nLen, cid, accid, cmd_buf));
	}
	return true;
}
MessagePack* MsgQueue::Get()
{
	MessagePack* cmdPack = NULL;
	if(m_cmd[m_cmd_read].first)
	{
		cmdPack = &m_cmd[m_cmd_read].second;
	}

	if (cmdPack == NULL && m_queue.size() > 0)
	{
		PutQueueToArray();
		if (m_cmd[m_cmd_read].first)
		{
			cmdPack = &m_cmd[m_cmd_read].second;
		}
	}
	return cmdPack;
}
void MsgQueue::Erase()
{
	DELETE(m_cmd[m_cmd_read].second.data);
	m_cmd[m_cmd_read].first = false;
	m_cmd_read = (m_cmd_read + 1) % MAX_QUEUE_SIZE;
}
void MsgQueue::Clear()
{
	while(m_cmd[m_cmd_read].first)
	{
		Erase();
	}

	while(!m_queue.empty())
	{
		DELETE(m_queue.front().data);
		m_queue.pop();
	}
	m_cmd_write = 0;
	m_cmd_read = 0;
}

bool MsgQueue::PutQueueToArray()
{
	bool result = true;
	while(!m_queue.empty())
	{
		if(m_cmd[m_cmd_write].first == false)
		{
			m_cmd[m_cmd_write].first = true;
			m_cmd[m_cmd_write].second = m_queue.front();
			m_cmd_write = (m_cmd_write + 1) % MAX_QUEUE_SIZE;
			m_queue.pop();
		}
		else
		{
			result = false;
			break;
		}
	}
	return result;
}


void MsgQueue::GetAndSwapTo(MsgQueue& msg_queue)
{
	assert(msg_queue.m_cmd_read == 0);
	assert(msg_queue.m_cmd_write == 0);
	assert(msg_queue.m_queue.size() == 0);

	if (m_queue.size() > 0)
	{
		msg_queue.m_queue.swap(m_queue);  //msg_queue 数据交换
	}
	msg_queue.m_cmd_read = m_cmd_read;
	msg_queue.m_cmd_write = m_cmd_write;

	if (m_cmd_write > m_cmd_read)
	{
		for (uint32 i = m_cmd_read; i < m_cmd_write; i++)
		{
			msg_queue.m_cmd[i].swap(m_cmd[i]);
		}
	}
	else if (m_cmd_write == m_cmd_read)
	{
		if (m_cmd[m_cmd_write].first == false) //准备写入的下一个位置可用 说明队列是空的，直接返回
		{
			return;
		}
		else //队列是满的，需要全部复制
		{
			for (uint32 i = 0; i < MAX_QUEUE_SIZE; i++)
			{
				msg_queue.m_cmd[i].swap(m_cmd[i]);
			}
		}
	}
	else
	{
		for (uint32 i = m_cmd_read; i < MAX_QUEUE_SIZE; i++)
		{
			msg_queue.m_cmd[i].swap(m_cmd[i]);
		}
		for (uint32 i = 0; i < m_cmd_write; i++)
		{
			msg_queue.m_cmd[i].swap(m_cmd[i]);
		}
	}

	/*
	bool bCopy = false;
	for (int32 i = 0; i < MAX_QUEUE_SIZE; i++ )
	{
		if (m_cmd[i].first == true)
		{
			msg_queue.m_cmd[i].swap(m_cmd[i]);
			bCopy = true;
		}
		else if (bCopy)
		{
			break;
		}
	}
	*/


	m_cmd_read = 0;
	m_cmd_write = 0;
}
