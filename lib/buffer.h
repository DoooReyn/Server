#ifndef __BUFFER_H__
#define __BUFFER_H__
#include "type_define.h"


const unsigned int trunkSize = 64 * 1024;
#define unzip_size(zip_size) ((zip_size) * 120 / 100 + 12)
#define trunkCount(size) (((size) + trunkSize - 1) / trunkSize)

//缓冲类，每次空间不够是扩大trunkSize大小
//缓冲区的空间，可以重复利用
template <typename T>
class Buffer
{
public:
	Buffer();
	inline void put(const unsigned char* buf, const unsigned int size);
	inline unsigned char* wr_buf();
	inline unsigned char* rd_buf();
	inline bool rd_ready() const;
	inline unsigned int rd_size() const;
	inline void rd_flip(unsigned int size);
	inline unsigned int wr_size() const;
	inline void wr_flip(const unsigned int size);
	inline void reset();
	inline unsigned int maxSize() const;
	inline void wr_reserve(const unsigned int size);

private:
	unsigned int m_maxSize;
	unsigned int m_offPtr;
	unsigned int m_currPtr;
	T m_buffer;

};

template <typename T>
void Buffer<T>::put(const unsigned char* buf, const unsigned int size)
{
	wr_reserve(size);
	//bcopy(buf, &m_buffer[m_currPtr], size);
	memcpy(&m_buffer[m_currPtr], buf, size);
	m_currPtr += size;
}

template <typename T>
unsigned char* Buffer<T>::wr_buf()
{
	return &m_buffer[m_currPtr];
}

template <typename T>
unsigned char* Buffer<T>::rd_buf()
{
	return &m_buffer[m_offPtr];
}

template <typename T>
bool Buffer<T>::rd_ready() const
{
	return m_currPtr > m_offPtr;
}

template <typename T>
unsigned int Buffer<T>::rd_size() const
{
	return m_currPtr - m_offPtr;
}

template <typename T>
void Buffer<T>::rd_flip(unsigned int size)
{
	m_offPtr += size;
	if(m_currPtr > m_offPtr)
	{
		unsigned int tmp = m_currPtr - m_offPtr;
		if(m_offPtr >= tmp)
		{
			memmove(&m_buffer[0], &m_buffer[m_offPtr], tmp);
			m_offPtr = 0;
			m_currPtr = tmp;
		}
	}
	else
	{
		m_offPtr = 0;
		m_currPtr = 0;
	}
}

template <typename T>
unsigned int Buffer<T>::wr_size() const
{
	return m_maxSize - m_currPtr;
}

template <typename T>
void Buffer<T>::wr_flip(const unsigned int size)
{
	m_currPtr += size;
}

template <typename T>
void Buffer<T>::reset()
{
	m_offPtr = 0;
	m_currPtr = 0;
}

template <typename T>
unsigned int Buffer<T>::maxSize() const
{
	return m_maxSize;
}

typedef Buffer<std::vector<unsigned char > > BufferCmdQueue;

template <>
inline Buffer< std::vector<unsigned char > >::Buffer()
	: m_maxSize(trunkSize)
	, m_offPtr(0)
	, m_currPtr(0)
	, m_buffer(m_maxSize)
{
}

template <>
inline void BufferCmdQueue::wr_reserve(const unsigned int size)
{
	if(wr_size() < size + 8)
	{
		m_maxSize += (trunkSize * trunkCount(size + 8));
		m_buffer.resize(m_maxSize);
	}
}
#endif
