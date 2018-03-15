#include "tcp_connection.h"
#include "logger.h"
#include "channel.h"
#include "event_loop.h"
#include "socket_ops.h"
#include "common.h"

std::map<int32, int32> g_map_parma;

void DefaultConnectionCallback(const TcpConnectionPtr& conn)
{
	//DEBUG("%s -> %s is Connect :%d", conn->GetLocalAddr().ToIPPort().c_str(), conn->GetPeerAddr().ToIPPort().c_str(), conn->Connected());
}

void DefaultDisconnectionCallback(const TcpConnectionPtr& conn)
{
	//DEBUG("%s -> %s is Connect :%d", conn->GetLocalAddr().ToIPPort().c_str(), conn->GetPeerAddr().ToIPPort().c_str(), conn->Connected());
}


void DefaultMessageCallback(const TcpConnectionPtr&, BufferEx* buf, Timestamp&)
{
	buf->RetrieveAll();
}

inline
void ByteSizeConsistencyError(int32 byte_size_before_serialization, int32 byte_size_after_serialization, int32 bytes_produced_by_serialization)
{
	GOOGLE_CHECK_EQ(byte_size_before_serialization, byte_size_after_serialization)
	        << "Protocol message was modified concurrently during serialization.";
	GOOGLE_CHECK_EQ(bytes_produced_by_serialization, byte_size_before_serialization)
	        << "Byte size calculation and serialization were inconsistent.  This "
	        "may indicate a bug in protocol buffers or it may be caused by "
	        "concurrent modification of the message.";
	GOOGLE_LOG(FATAL) << "This shouldn't be called if all the sizes are equal.";
}

AtomicInt32 TcpConnection::m_numCreated;

TcpConnection::TcpConnection(EventLoop* loop, const string& name, int32 sockfd, const InetAddress& local_addr, const InetAddress& peer_addr)
	: m_pLoop(loop)
	, m_strName(name)
	, m_state(kConnecting)
	, m_reading(true)
	, m_socket(new Socket(sockfd))
	, m_channel(new Channel(loop, sockfd))
	, m_localAddr(local_addr)
	, m_peerAddr(peer_addr)
	, m_thisid(0)
	, m_heart_beat(0)
{
	m_channel->SetReadCallBack(std::bind(&TcpConnection::handleRead, this, _1));
	m_channel->SetWriteCallBack(std::bind(&TcpConnection::handleWrite, this));
	m_channel->SetCloseCallBack(std::bind(&TcpConnection::handleClose, this));
	m_channel->SetErrorCallBack(std::bind(&TcpConnection::handleError, this));
	SetTcpNoDelay(true);
	m_socket->SetKeepAlive(true);
	m_numCreated.Increment();
	//DEBUG("TcpConnection::ctor[%s] at %p fd=%d", m_strName.c_str(), this, sockfd);
}

TcpConnection::~TcpConnection()
{
	//DEBUG("TcpConnection::dctor[%s] at %p fd=%d state=%s", m_strName.c_str(), this, m_channel->GetFd(), stateToString());
	assert(m_state == kDisconnected);
}

bool TcpConnection::GetTcpInfo(struct tcp_info* tcpinfo) const
{
	return m_socket->GetTcpInfo(tcpinfo);
}

std::string TcpConnection::GetTcpInfoString() const
{
	char buf[1024] = { 0 };
	m_socket->GetTcpInfoString(buf, sizeof(buf));
	return buf;
}

void TcpConnection::Send(const uint32 messageid, const void* message, int32 len, uint32 uid /*= 0*/)
{
	if (m_state == kConnected)
	{
		BufferEx buf;
		struct PackHead pack_head;
		pack_head.messageid = messageid;
		pack_head.size = len;
		pack_head.uid = uid;
		buf.EnsureWriteableBytes(pack_head.size + PACK_HEAD_LEN);
		buf.append((const char*)(&pack_head), PACK_HEAD_LEN);
		buf.append(message, len);
		Send(messageid, &buf);
	}
}

void TcpConnection::Send(const PackHead* pack_head, const void* message)
{
	if (m_state == kConnected)
	{
		BufferEx buf;
		buf.EnsureWriteableBytes(pack_head->size + PACK_HEAD_LEN);
		buf.append((const char*)(pack_head), PACK_HEAD_LEN);
		buf.append(message, pack_head->size);
		Send(pack_head->messageid, &buf);
	}
}


void TcpConnection::Send(const uint32 messageid, uint32 uid /*= 0*/)
{
	uint32 data = 0;
	Send(messageid, (const char*)&data, sizeof(data), uid);
}

void TcpConnection::SendProtoBuf(const uint32 messageid, ::google::protobuf::Message& message, uint32 uid /*= 0*/)
{
	if (m_state == kConnected)
	{
		BufferEx buf;
		int32 len = message.ByteSize();
		struct PackHead pack_head;
		pack_head.messageid = messageid;
		pack_head.size = len;
		pack_head.uid = uid;
		buf.EnsureWriteableBytes(pack_head.size + PACK_HEAD_LEN);
		buf.append((const char*)(&pack_head), PACK_HEAD_LEN);

		uint8* start = reinterpret_cast<uint8*>(buf.BeginWrite());
		uint8* end = message.SerializeWithCachedSizesToArray(start);
		if (end - start != len)
		{
			ByteSizeConsistencyError(len, message.ByteSize(), static_cast<int>(end - start));
		}
		buf.HasWriten(len);
		Send(messageid, &buf);
	}
}


void TcpConnection::Send(const uint32 messageid, BufferEx* message)
{
	if (m_pLoop->IsInLoopThread())
	{
		sendInLoop(messageid, message->Peek(), message->ReadableBytes());
		message->RetrieveAll();
	}
	else
	{
		m_pLoop->RunInLoop(std::bind(&TcpConnection::sendInLoop2, this, messageid, message->RetrieveAllAsString()));
	}

}


void TcpConnection::sendInLoop2(const uint32 messageid, const string& message)
{
	//DEBUG("send in loop 2 messageid:%d,size:%ld", messageid, message.length());
	sendInLoop(messageid, message.c_str(), message.size());
}

void TcpConnection::sendInLoop(const uint32 messageid, const void* message, size_t len)
{
	m_pLoop->AssertInLoopThread();
	ssize_t nwrote = 0;
	size_t remaining = len;
	bool faultError = false;

	if (m_state == kDisconnected)
	{
		WARN("disconnected give up writing");
		return;
	}

	///调试信息
	if (messageid == CMD_CONNECT_TEST1 || messageid == CMD_CONNECT_TEST2)
	{
		g_map_parma[0] += len;
	}

	if (!m_channel->IsWriting() && m_OutPutBuff.ReadableBytes() == 0)
	{
		nwrote = Write(m_channel->GetFd(), message, len);
		if (nwrote >= 0 )
		{
			remaining = len - nwrote;
			if (remaining == 0 && m_writeCompleteCallback)
			{
				m_pLoop->QueueInLoop(std::bind(m_writeCompleteCallback, shared_from_this()));
			}
		}
		else
		{
			nwrote = 0;
			if (errno != EWOULDBLOCK)
			{
				//ERROR("TcpConnection sendInLoop");
				if (errno == EPIPE || errno == ECONNRESET)
				{
					faultError = true;
				}
			}
		}
	}
	assert(remaining <= len);
	if (!faultError && remaining > 0)
	{
		size_t oldlen = m_OutPutBuff.ReadableBytes();
		if (oldlen + remaining >= m_highWaterMark &&
		        oldlen < m_highWaterMark &&
		        m_highWaterMarkCallback)
		{
			m_pLoop->QueueInLoop(std::bind(m_highWaterMarkCallback, shared_from_this(), oldlen + remaining));
		}
		m_OutPutBuff.append(static_cast<const char*>(message) + nwrote, remaining);

		if (!m_channel->IsWriting())
		{
			m_channel->EnableWriting();
			//g_map_parma[5] ++;
		}
	}
}


void TcpConnection::Shutdown()
{
	if (m_state == kConnected)
	{
		setState(kDisconnecting);
		m_pLoop->RunInLoop(std::bind(&TcpConnection::shutdownInLoop, this));
	}
}


void TcpConnection::shutdownInLoop()
{
	m_pLoop->AssertInLoopThread();
	if (!m_channel->IsWriting())
	{
		m_socket->ShutdownWrite();
	}
}

void TcpConnection::ForceClose()
{
	//DEBUG("ForceClose %d", m_state);
	if (m_state == kConnected || m_state == kDisconnecting)
	{
		setState(kDisconnecting);
		m_pLoop->QueueInLoop(std::bind(&TcpConnection::forceCloseInLoop, shared_from_this()));
	}
}

void TcpConnection::ForceCloseWithDelay(double seconds)
{
	if (m_state == kConnected || m_state == kDisconnecting)
	{
		setState(kDisconnecting);
		m_pLoop->RunAfter(seconds, makeWeakCallback(shared_from_this(), &TcpConnection::ForceClose));
	}
}

void TcpConnection::forceCloseInLoop()
{
	m_pLoop->AssertInLoopThread();
	if (m_state == kConnected || m_state == kDisconnecting)
	{
		handleClose();
	}
}

const char* TcpConnection::stateToString() const
{
	switch (m_state)
	{
		case kDisconnected:
			return "kDisconnected";
		case kConnecting:
			return "kConnecting";
		case kConnected:
			return "kConnected";
		case kDisconnecting:
			return "kDisconnecting";
		default:
			return "unknown state";
	}
}

void TcpConnection::SetTcpNoDelay(bool on)
{
	m_socket->SetTcpNoDelay(on);
}

void TcpConnection::StartRead()
{
	m_pLoop->RunInLoop(std::bind(&TcpConnection::startReadInLoop, this));
}

void TcpConnection::startReadInLoop()
{
	m_pLoop->AssertInLoopThread();
	if (!m_reading || !m_channel->IsReading())
	{
		m_channel->EnableReading();
		m_reading = true;
	}
}

void TcpConnection::StopRead()
{
	m_pLoop->RunInLoop(std::bind(&TcpConnection::stopReadInLoop, this));
}

void TcpConnection::stopReadInLoop()
{
	m_pLoop->AssertInLoopThread();
	if (m_reading || m_channel->IsReading())
	{
		m_channel->DisableReading();
		m_reading = false;
	}
}

void TcpConnection::ConnectEstablished()
{
	m_pLoop->AssertInLoopThread();
	assert(m_state == kConnecting);
	setState(kConnected);
	m_channel->Tie(shared_from_this());
	m_channel->EnableReading();
	m_connectionCallback(shared_from_this());
	m_timer_id = m_pLoop->RunEvery(HEART_BEAT_TIME, std::bind(&TcpConnection::CheckHeartBeat, this));
}

void TcpConnection::ConnectDestroyed()
{
	m_pLoop->AssertInLoopThread();
	if (m_state == kConnected)
	{
		setState(kDisconnected);
		m_channel->DisableAll();
		m_disconnectCallback(shared_from_this());
	}
	m_channel->Remove();
	m_pLoop->Cancel(m_timer_id);

}

void TcpConnection::handleRead(Timestamp& receiveTime)
{
	m_pLoop->AssertInLoopThread();
	int32 savedError = 0;
	ssize_t n = m_InPutBuff.ReadFd(m_channel->GetFd(), &savedError);
	if (n > 0)
	{
		m_messageCallback(shared_from_this(), &m_InPutBuff, receiveTime);
	}
	else if (n == 0)
	{
		handleClose();
	}
	else
	{
		errno = savedError;
		ERROR("TcpConnect::handleRead");
		handleError();
	}
}

void TcpConnection::handleWrite()
{
	m_pLoop->AssertInLoopThread();
	if (m_channel->IsWriting())
	{
		ssize_t n = ::Write(m_channel->GetFd(), m_OutPutBuff.Peek(), m_OutPutBuff.ReadableBytes());
		if (n > 0)
		{
			m_OutPutBuff.Retrieve(n);
			if (m_OutPutBuff.ReadableBytes() == 0)
			{
				m_channel->DisableWriting();
				if (m_writeCompleteCallback)
				{
					m_pLoop->QueueInLoop(std::bind(m_writeCompleteCallback, shared_from_this()));
				}
				if (m_state == kDisconnecting)
				{
					shutdownInLoop();
				}
			}
			else
			{
				m_channel->EnableWriting();
			}
		}
		else
		{
			ERROR("TcpConnection::handleWrite");
		}
	}
	else
	{
		DEBUG("Connection fd = %d is down, no more writing", m_channel->GetFd());
	}
}

void TcpConnection::handleClose()
{
	m_pLoop->AssertInLoopThread();
	assert(m_state == kConnected || m_state == kDisconnecting);
	setState(kDisconnected);
	m_channel->DisableAll();
	TcpConnectionPtr guardThis(shared_from_this());
	m_disconnectCallback(guardThis);
	m_closeCallback(guardThis);
}

void TcpConnection::handleError()
{
	int32 err = GetSocketError(m_channel->GetFd());
	ERROR("TcpConnection::handleError [ %s ] - SO_ERROR = %d -- %s", m_strName.c_str(), err, strerror_tl(err));
}

void TcpConnection::SetThisid(int32 id, int32 typeconn)
{
	if (id == 0)
	{
		m_thisid = GetKeyInt64(typeconn, m_numCreated.Get());
	}
	else
	{
		m_thisid = GetKeyInt64(typeconn, id);
	}
}

void TcpConnection::CheckHeartBeat()
{
	int32 requesttime = Timestamp::Now().GetSeconds();
	//INFO("CheckHeartBeat second:%d - %d ", requesttime, m_heart_beat);
	if (m_heart_beat == 0)
	{
		m_heart_beat = requesttime;
		return;
	}

	if (requesttime - m_heart_beat  >= 2 * HEART_BEAT_TIME)
	{
		handleClose();
	}
}
