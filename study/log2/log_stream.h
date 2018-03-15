//日志流文件
#ifndef __LOG_STREAM_H__
#define __LOG_STREAM_H__

#include "type_define.h"
#include "nocopyable.h"
#include "buffer_ex.h"

const int32 kSmallBuffer = 4000;
const int32 KLargeBuffer = 4000 * 1000;

typedef void (*FunCookie)();

template <int32 SIZE>
class FixedBuffer : public Nocopyable
{
public:
	//设置开始回调
	FixedBuffer(): m_cur(m_data)
	{
		SetCookie(cookieStart);
	}

	//设置结束回调
	~FixedBuffer()
	{
		SetCookie(cookieEnd);
	}

	//添加字符空间
	void append(const char* buf, size_t len)
	{
		uint32 avali_able = Avail();
		if(avali_able > len)
		{
			memcpy(m_cur, buf, len);
			m_cur += len;
		}
	}

	//计算buff剩余空间
	int32 Avail() const { return static_cast<int32>(end() - Current()); }

	//设置工作函数
	void SetCookie(FunCookie cookie)
	{
		m_fun_cookie = cookie;
	}

	//返回数据地址
	const char* Data() const { return m_data; }

	//返回当前已写的长度
	int32 Length() const { return static_cast<int32> (m_cur - m_data); }

	//返回当前的写的地址
	char* Current() const { return m_cur; };

	//已写的空间便宜
	void Add(size_t len) { m_cur += len; };

	//缓存区重置
	void Reset() { m_cur = m_data; };

	//缓冲区数据清0
	void BZero() { ::bzero(m_data, sizeof(m_data)); }

	//调试使用
	const char* DebugString();

	//账号当前已写数据为string
	string ToString() const { return string(m_data, Length()); }


private:

	const char* end() const {return m_data + sizeof(m_data);}
	static void cookieStart();
	static void cookieEnd();

	FunCookie m_fun_cookie;
	char m_data[SIZE];
	char* m_cur;
};


class LogStream : public Nocopyable
{
	typedef LogStream self;
public:
	typedef FixedBuffer<kSmallBuffer> Buffer;
	self& operator<<(bool v)
	{
		m_buffer.append( v ? "1" : "0", 1);
		return *this;
	}
	self& operator<<(short);
	self& operator<<(unsigned short);
	self& operator<<(int);
	self& operator<<(unsigned int);
	self& operator<<(long);
	self& operator<<(unsigned long);
	self& operator<<(long long);
	self& operator<<(unsigned long long);
	self& operator<<(double);
	self& operator<<(const void*);
	self& operator<<(float v)
	{
		*this << static_cast<double>(v);
		return *this;
	}

	self& operator<<(char v)
	{
		m_buffer.append(&v, 1);
		return *this;
	}

	self& operator<<(const char* v)
	{
		if(v)
		{
			m_buffer.append(v, strlen(v));
		}
		else
		{
			m_buffer.append("(null)", 6);
		}
		return *this;
	}

	self& operator<<(const unsigned char* v)
	{
		return operator<<(reinterpret_cast<const char*>(v));
	}

	self& operator<<(const string& v)
	{
		m_buffer.append(v.c_str(), v.size());
		return *this;
	}

	self& operator<<(const Buffer& v)
	{
		*this << v.ToString();
		return *this;
	}

	void append(const char* data, int32 len)
	{
		m_buffer.append(data, len);
	}

	const Buffer& GetBuffer()const {return m_buffer;}

	void ResetBuffer() {m_buffer.Reset();}

private:
	void staticCheck();
	template <typename T>
	void formatInteger(T);

	Buffer m_buffer;
	static const int32 kMaxNumericSize = 32;
};

class Fmt
{
public:
	template <typename T>
	Fmt(const char* fmt, T value);
	const char* Data() const {return m_buf;}

	int Length() const {return m_length;}
private:
	char m_buf[32];
	int32 m_length;
};

inline LogStream& operator<<(LogStream& s, const Fmt& fmt)
{
	s.append(fmt.Data(), fmt.Length());
	return s;
}


#endif
