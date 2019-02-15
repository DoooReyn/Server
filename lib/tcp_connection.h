#ifndef __TCP_CONNECTION_H__
#define __TCP_CONNECTION_H__

#include "type_define.h"
#include "callback.h"
#include "buffer_ex.h"
#include "inet_address.h"
#include "nocopyable.h"
#include "message_queue.h"
#include "helper.h"
#include "string_piece.h"

class Channel;
class EventLoop;
class Socket;


class TcpConnection : private Nocopyable, public std::enable_shared_from_this < TcpConnection >
{
public:

	TcpConnection(EventLoop* loop, const string& name, int32 sockfd, const InetAddress& local_addr, const InetAddress& peer_addr);
	~TcpConnection();

	EventLoop* GetLoop() const
	{
		return m_pLoop;
	}
	const string& GetName() const
	{
		return m_strName;
	}

	const InetAddress& GetLocalAddr() const
	{
		return m_localAddr;
	}
	const InetAddress& GetPeerAddr() const
	{
		return m_peerAddr;
	}

	bool Connected() const
	{
		return m_state == kConnected;
	}
	bool Disconnected() const
	{
		return m_state == kDisconnected;
	}

	bool GetTcpInfo(struct tcp_info* tcpinfo) const;
	string GetTcpInfoString() const;
	void Send(const PackHead* pack_head, const void* message);
	void Send(const uint32 messageid, const void* message, int32 len, uint32 uid = 0);
	void Send(const uint32 messageid, uint32 uid = 0);
	void SendProtoBuf(const uint32 messageid, ::google::protobuf::Message& message, uint32 uid = 0);


	void Shutdown();

	void ForceClose();
	void ForceCloseWithDelay(double seconds);

	void SetTcpNoDelay(bool on);

	void StartRead();
	void StopRead();

	bool IsReading()const
	{
		return m_reading;
	}

	void SetConnectionCallback(const ConnectionCallback& cb)
	{
		m_connectionCallback = cb;
	}

	void SetDisconnectCallback(const DisConnectCallback& cb)
	{
		m_disconnectCallback = cb;
	}

	void SetMessageCallback(const MessageCallback& cb)
	{
		m_messageCallback = cb;
	}

	void SetWriteCompleteCallback(const WriteCompleteCallback& cb)
	{
		m_writeCompleteCallback = cb;
	}

	void SetHighWaterMarkCallback(const HighWaterMarkCallback& cb, size_t highWaterMark)
	{
		m_highWaterMarkCallback = cb;
		m_highWaterMark = highWaterMark;
	}

	BufferEx* GetInputBuffer()
	{
		return &m_InPutBuff;
	}

	BufferEx* GetOutputBuffer()
	{
		return &m_OutPutBuff;
	}

	void SetCloseCallback(const CloseCallback& cb)
	{
		m_closeCallback = cb;
	}

	void ConnectEstablished();

	void ConnectDestroyed();

	void SetThisid(int32 id, int32 typeconn);
	int64 GetThisid()
	{
		return m_thisid;
	};
	int32 GetId()
	{
		return GetKeyLowerInt64(m_thisid);
	};
	int32 GetType()
	{
		return GetKeyHigherInt64(m_thisid);
	};

	void SetHeartBeat()
	{
		m_heart_beat = Timestamp::Now().GetSeconds();
		m_timeouts = 0;
	}
	void CheckHeartBeat();
private:

	enum STATEE { kDisconnected, kConnecting, kConnected, kDisconnecting };


	void handleRead(Timestamp& receiveTime);
	void handleWrite();
	void handleClose();
	void handleError();


	void Send(const uint32 messageid, BufferEx* message);


	void sendInLoop2(const uint32 messageid, const string& message);
	void sendInLoop(const uint32 messageid, const void* message, size_t len);

	void shutdownInLoop();

	void forceCloseInLoop();
	void setState(STATEE s)
	{
		m_state = s;
	}

	const char* stateToString() const;

	void startReadInLoop();
	void stopReadInLoop();

	EventLoop* m_pLoop;
	const string m_strName;

	STATEE m_state;
	bool m_reading;

	std::unique_ptr<Socket> m_socket;
	std::unique_ptr<Channel> m_channel;

	const InetAddress m_localAddr;
	const InetAddress m_peerAddr;

	ConnectionCallback m_connectionCallback;
	DisConnectCallback m_disconnectCallback;

	MessageCallback m_messageCallback;
	WriteCompleteCallback m_writeCompleteCallback;
	HighWaterMarkCallback m_highWaterMarkCallback;
	CloseCallback m_closeCallback;

	static AtomicInt32 m_numCreated;

	int64 m_thisid;
	int32 m_heart_beat;
	TimerId m_timer_id;

	size_t m_highWaterMark;
	BufferEx m_InPutBuff;
	BufferEx m_OutPutBuff;

	MsgQueue m_msg_queue;

	int32 m_timeouts;
};


#endif
