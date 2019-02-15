#ifndef __EVENT_MANAGER_H__
#define __EVENT_MANAGER_H__

#include "type_define.h"
#include "singleton.h"
#include "tcp_connection.h"
#include "message_queue.h"
#include "common.h"
#include "logger.h"
#include "memory_pool.h"


class CFunctionSlotBase
{
public:
	virtual ~CFunctionSlotBase() {}
	virtual bool operator()(void* pdata)
	{
		return true;
	}
	virtual long GetThisAddr()
	{
		return 0;
	}
	virtual long GetFunAddr()
	{
		return 0;
	}
	virtual void EmptyThisAddr() {}
};



template<typename T, typename T2>
class CFunctionSlot : public CFunctionSlotBase
{
	typedef void (T::*f)(T2*);
public:
	CFunctionSlot(void (T::*f)(T2*), T* obj)
		: m_funPtr(f), m_this(obj)
	{

	}
	virtual ~CFunctionSlot() {}

	virtual bool operator() (void* pdata)
	{
		if (m_this != nullptr && m_funPtr != nullptr)
		{
			(m_this->*m_funPtr)(reinterpret_cast<T2*>(pdata));
			return true;
		}
		else
		{
			return false;
		}
	}

	virtual long GetThisAddr()
	{
		return (*reinterpret_cast<long*>(&m_this));
	}
	virtual void EmptyThisAddr()
	{
		m_this = nullptr;
	}

	virtual long GetFunAddr()
	{
		//提取类成员函数地址，&m_funPtr 取出成员函数指针地址，将其转为指向long型的指针，再用*取出地址值
		return (*reinterpret_cast<long*>(&m_funPtr));
	}

private:
	f m_funPtr;
	T* m_this;
};


class NetworkManager : public SingletonBase<NetworkManager>
{
public:
	typedef std::map<uint32, CFunctionSlotBase*> FuncMap;

	NetworkManager()
	{
		m_transmit_cb = nullptr;
		m_mapFunc.clear();
		CMemPool::newInstance();
	}

	virtual ~NetworkManager()
	{
		for (FuncMap::iterator it = m_mapFunc.begin(); it != m_mapFunc.end(); it++)
		{
			DELETE(it->second);
		}
		m_mapFunc.clear();
	}


	void SetVerifyCallback(const VerifyCallback& cb)
	{
		m_verifyCallback = cb;
	}

	template<typename T, typename T2>
	bool RegisterMessageHandle(uint32 messageId, void (T::*f)(T2*), T* obj)
	{
		FuncMap::iterator itFind = m_mapFunc.find(messageId);
		if(itFind != m_mapFunc.end())
		{
			ERROR("messageId:%d Have been userd", messageId);
			return false;
		}
		else
		{
			CFunctionSlotBase* pslot = new CFunctionSlot<T, T2>(f, obj);
			m_mapFunc.insert(make_pair(messageId, pslot));
			return true;
		}
	}

	void UnregisterMessageHandle(uint32 messageId)
	{
		assert(m_mapFunc.find(messageId) != m_mapFunc.end());
		FuncMap::iterator itFind = m_mapFunc.find(messageId);
		DELETE(itFind->second);
		m_mapFunc.erase(messageId);
	}

	template<typename T>
	bool FireMessage(uint32 messageId, T* pdata)
	{
		FuncMap::iterator itFind = m_mapFunc.find(messageId);
		if (itFind == m_mapFunc.end())
		{
			ERROR("Messageid :%d Not Deal", messageId);
			return true;
		}

		if (itFind->second->GetThisAddr() != 0)
		{
			(*(itFind->second))(pdata);
		}
		return true;
	}

	void OnMessage(const TcpConnectionPtr& conn, BufferEx* buf, Timestamp& time)
	{
		//DEBUG("%s -> %s : %ld", conn->GetPeerAddr().ToIPPort().c_str(), conn->GetLocalAddr().ToIPPort().c_str(), buf->ReadableBytes());
		//int32 nByte = 0;
		//INFO("Recv Bytes %ld", buf->ReadableBytes());
		if (buf->ReadableBytes() < PACK_HEAD_LEN)
		{
			return;
		}

		MutexLockGuard lock(m_mutex);
		while (buf->ReadableBytes() >= PACK_HEAD_LEN)
		{
			struct PackHead* pack_head = (PackHead*)(buf->Peek());
			if (pack_head->head != PACK_HEAD || pack_head->tail != PACK_TAIL)
			{
				ERROR("Pack Head Parse Failed");
				conn->Shutdown();
				break;
			}

			if (pack_head->size + PACK_HEAD_LEN <= buf->ReadableBytes())
			{
				buf->Retrieve(PACK_HEAD_LEN); //消息包解析开始
				//nByte += PACK_HEAD_LEN;
				//判断是否直接处理
				bool bProcess = PreProcessMessage(conn, pack_head->messageid, pack_head->size, buf->Peek());
				if (!bProcess && m_transmit_cb)
				{
					bProcess = m_transmit_cb(conn, pack_head, buf->Peek());  //判断是否转发处理
				}

				if (bProcess == false) //进入消息队列
				{

					bool bRet = m_MsgQueue.Put(pack_head->messageid, pack_head->size, conn->GetThisid(), pack_head->uid, buf->Peek());
					if (bRet == false)
					{
						ERROR("m_MsgQueue Put Error");
						m_MsgQueue.Print();
					}
				}
				buf->Retrieve(pack_head->size);  //消息包解析结束
				//nByte += pack_head->size;
			}
			else
			{
				//INFO("pack not full: %x %x %d %d", pack_head->head, pack_head->tail, pack_head->messageid, pack_head->size);
				break;
			}
		}
		//INFO("Send Over  remaind: %ld Read:%d", buf->ReadableBytes(), nByte);
	}

	bool PreProcessMessage(const TcpConnectionPtr& conn, uint32 messageid, uint32 nLen, const char* cmd)
	{
		if (messageid == CMD_CONNECT_HEART_BEAT)  //心跳包 用于检测链接是否正常
		{
			CHECKERR_AND_RETURN_VALUE(nLen == sizeof(CmdConnectHeartBeat), true);
			conn->SetHeartBeat();
			return true;
		}
		else if (messageid == CMD_CONNECT_VERIFY_REQUEST) //链接确认包，通知链接的类型和id
		{
			CmdConnectVerifyRequest* recvPack = (CmdConnectVerifyRequest*)cmd;
			CHECKERR_AND_RETURN_VALUE(nLen == sizeof(CmdConnectVerifyRequest), true);

			conn->SetThisid(recvPack->id, recvPack->conntype);
			m_verifyCallback(conn, recvPack->id, recvPack->conntype);

			CmdConnectVerifyReturn sendPack;
			sendPack.result = 0;
			strncpy(sendPack.src, conn->GetLocalAddr().ToIPPort().c_str(), sizeof(sendPack.src));
			conn->Send(CMD_CONNECT_VERIFY_RETURN, &sendPack, sizeof(CmdConnectVerifyReturn));
			return true;
		}
		return false;
	}

	void ParseMsg()
	{
		m_mutex.Lock();
		static MsgQueue m_MsgQueue2;
		m_MsgQueue2.Clear();
		m_MsgQueue.GetAndSwapTo(m_MsgQueue2);
		m_mutex.UnLock();

		MessagePack* pPack = m_MsgQueue2.Get();
		while (pPack != nullptr)
		{

			NetworkManager::getInstance().FireMessage(pPack->messageid, pPack);
			m_MsgQueue2.Erase();
			pPack = m_MsgQueue2.Get();
		}
		//m_MsgQueue.Print();
	}

	void SetTransmitCallBack(const TransmitCallback& cb)
	{
		m_transmit_cb = cb;
	}


private:
	FuncMap m_mapFunc;
	MsgQueue m_MsgQueue;
	Mutex m_mutex;
	TransmitCallback  m_transmit_cb;
	VerifyCallback m_verifyCallback;
};




#endif
