#ifndef __STRING_PIECE_H__
#define __STRING_PIECE_H__
#include "type_define.h"

class StringPiece
{
private:
	const char* m_pStr;
	int32 m_length;
public:
	StringPiece() : m_pStr(NULL), m_length(0) {}
	explicit StringPiece(const char* str) : m_pStr(str), m_length(static_cast<int32>(strlen(str))) {}
	explicit StringPiece(const unsigned char* str): m_pStr(reinterpret_cast<const char*>(str)), m_length(static_cast<int32>(strlen(m_pStr))) { }
	explicit StringPiece(const string& str) : m_pStr(str.data()), m_length(static_cast<int32>(str.size())) { }
	StringPiece(const char* offset, int32 len): m_pStr(offset), m_length(len) { }

	const char* data() const
	{
		return m_pStr;
	}
	int32 size() const
	{
		return m_length;
	}
	bool empty() const
	{
		return m_length == 0;
	}
	const char* begin() const
	{
		return m_pStr;
	}
	const char* end() const
	{
		return m_pStr + m_length;
	}
	void clear()
	{
		m_pStr = NULL;
		m_length = 0;
	}
	void set(const char* buffer, int32 len)
	{
		m_pStr = buffer;
		m_length = len;
	}
	void set(const char* str)
	{
		m_pStr = str;
		m_length = static_cast<int32>(strlen(str));
	}
	void set(const void* buffer, int32 len)
	{
		m_pStr = reinterpret_cast<const char*>(buffer);
		m_length = len;
	}


	void remove_prefix(int n)
	{
		m_pStr += n;
		m_length -= n;
	}

	void remove_suffix(int n)
	{
		m_length -= n;
	}

	char operator[](int i) const
	{
		return m_pStr[i];
	}

	bool operator==(const StringPiece& x) const
	{
		return ((m_length == x.m_length) &&
		        (memcmp(m_pStr, x.m_pStr, m_length) == 0));
	}
	bool operator!=(const StringPiece& x) const
	{
		return !(*this == x);
	}

#define STRINGPIECE_BINARY_PREDICATE(cmp,auxcmp)											\
	bool operator cmp (const StringPiece& x) const											\
	{																						\
		int r = memcmp(m_pStr, x.m_pStr, m_length < x.m_length ? m_length : x.m_length);	\
		return ((r auxcmp 0) || ((r == 0) && (m_length cmp x.m_length)));					\
	}

	STRINGPIECE_BINARY_PREDICATE( <, < );
	STRINGPIECE_BINARY_PREDICATE( <=, < );
	STRINGPIECE_BINARY_PREDICATE( >=, > );
	STRINGPIECE_BINARY_PREDICATE( >, > );
#undef STRINGPIECE_BINARY_PREDICATE
	int compare(const StringPiece& x) const
	{
		int r = memcmp(m_pStr, x.m_pStr, m_length < x.m_length ? m_length : x.m_length);
		if (r == 0)
		{
			if (m_length < x.m_length)
			{
				r = -1;
			}
			else if (m_length > x.m_length)
			{
				r = +1;
			}
		}
		return r;
	}

	string as_string() const
	{
		return string(data(), size());
	}

	void CopyToString(string* target) const
	{
		target->assign(m_pStr, m_length);
	}
	bool starts_with(const StringPiece& x) const
	{
		return ((m_length >= x.m_length) && (memcmp(m_pStr, x.m_pStr, x.m_length) == 0));
	}
};


#endif
