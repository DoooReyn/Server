#include "log_stream.h"

template class FixedBuffer < kSmallBuffer > ;
template class FixedBuffer < KLargeBuffer >;

const char digits[] = "9876543210123456789";
const char* zero = digits + 9;
STATIC_ASSERT_CHECK(sizeof(digits) == 20);

const char digitsHex[] = "0123456789ABCDEF";
STATIC_ASSERT_CHECK(sizeof(digitsHex) == 17);


//转换数字为字符串
template <typename T>
size_t convent(char buff[], T value)
{
	T i = value;
	char* p = buff;
	do
	{
		int32 lsd = static_cast<int32>(i % 10);
		i /= 10;
		*p++ = zero[lsd];
	}
	while (i != 0);
	if(value < 0)
	{
		*p++ = '-';
	}
	*p = '\0';
	std::reverse(buff, p); //反转p的顺序
	return p - buff;
}
size_t convertHex(char buf[], uintptr_t value)
{
	uintptr_t i = value;
	char* p = buf;

	do
	{
		int lsd = static_cast<int>(i % 16);
		i /= 16;
		*p++ = digitsHex[lsd];
	}
	while (i != 0);

	*p = '\0';
	std::reverse(buf, p);

	return p - buf;
}



template<int SIZE>
const char* FixedBuffer<SIZE>::DebugString()
{
	*m_cur = '\0';
	return m_cur;
}

template<int SIZE>
void FixedBuffer<SIZE>::cookieStart()
{
}

template<int SIZE>
void FixedBuffer<SIZE>::cookieEnd()
{
}

void LogStream::staticCheck()
{
	STATIC_ASSERT_CHECK(kMaxNumericSize - 10 > std::numeric_limits<double>::digits10);
	STATIC_ASSERT_CHECK(kMaxNumericSize - 10 > std::numeric_limits<long double>::digits10);
	STATIC_ASSERT_CHECK(kMaxNumericSize - 10 > std::numeric_limits<long>::digits10);
	STATIC_ASSERT_CHECK(kMaxNumericSize - 10 > std::numeric_limits<long long>::digits10);
}

template <typename T>
void LogStream::formatInteger(T v)
{
	if (m_buffer.Avail() >= kMaxNumericSize)
	{
		size_t len = convent(m_buffer.Current(), v);
		m_buffer.Add(len);
	}
}

LogStream& LogStream::operator<<(short v)
{
	*this << static_cast<int>(v);
	return *this;
}

LogStream& LogStream::operator<<(unsigned short v)
{
	*this << static_cast<unsigned int>(v);
	return *this;
}

LogStream& LogStream::operator<<(int v)
{
	formatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(unsigned int v)
{
	formatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(long v)
{
	formatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(unsigned long v)
{
	formatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(long long v)
{
	formatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(unsigned long long v)
{
	formatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(const void* p)
{
	uintptr_t v = reinterpret_cast<uintptr_t>(p);
	if (m_buffer.Avail() >= kMaxNumericSize)
	{
		char* buf = m_buffer.Current();
		buf[0] = '0';
		buf[1] = 'x';
		size_t len = convertHex(buf + 2, v);
		m_buffer.Add(len + 2);
	}
	return *this;
}


LogStream& LogStream::operator<<(double v)
{
	if (m_buffer.Avail() >= kMaxNumericSize)
	{
		int len = snprintf(m_buffer.Current(), kMaxNumericSize, "%.12g", v);
		m_buffer.Add(len);
	}
	return *this;
}

template<typename T>
Fmt::Fmt(const char* fmt, T val)
{
	m_length = snprintf(m_buf, sizeof(m_buf), fmt, val);
	assert(static_cast<size_t>(m_length) < sizeof(m_buf));
}

// Explicit instantiations

template Fmt::Fmt(const char* fmt, char);
template Fmt::Fmt(const char* fmt, short);
template Fmt::Fmt(const char* fmt, unsigned short);
template Fmt::Fmt(const char* fmt, int);
template Fmt::Fmt(const char* fmt, unsigned int);
template Fmt::Fmt(const char* fmt, long);
template Fmt::Fmt(const char* fmt, unsigned long);
template Fmt::Fmt(const char* fmt, long long);
template Fmt::Fmt(const char* fmt, unsigned long long);
template Fmt::Fmt(const char* fmt, float);
template Fmt::Fmt(const char* fmt, double);


