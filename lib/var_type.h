#ifndef __VAR_TYPE_H__
#define __VAR_TYPE_H__

#include "type_define.h"

class VarType
{
public:
	VarType();
	VarType(const VarType& vt);
	template<typename T>
	explicit VarType(const T& value);

	template <typename T>
	VarType& operator= (const T& value);

	VarType& operator= (const VarType& vt);

	template <typename T>
	void put(const T& value);

	void put(const void* data, const unsigned int len);
	void put(const uint8& value);

	operator unsigned char() const;
	operator char() const;

	operator unsigned short() const;
	operator short() const;

	operator unsigned int() const;
	operator int() const;

	operator unsigned long() const;
	operator long() const;

	operator unsigned long long() const;
	operator long long() const;

	operator float() const;
	operator double() const;

	operator const char* () const;
	operator const void* () const;
	operator string() const;

	unsigned int size() const;
	bool empty() const;
	bool valid() const;

private:
	vector<uint8> m_data;
};

inline VarType::VarType() : m_data(1, '\0')
{
}

inline VarType::VarType(const VarType& vt) : m_data(vt.m_data)
{
}

template <typename T>
inline VarType::VarType(const T& value)
{
	put(value);
}

inline VarType& VarType::operator= (const VarType& vt)
{
	m_data = vt.m_data;
	return *this;
}

template <typename T>
inline VarType& VarType::operator= (const T& value)
{
	put(value);
	return *this;
}

inline void VarType::put(const void* data, const unsigned int len)
{
	m_data.resize(len + 1);
	if(len)
	{
		bcopy(data, &m_data[0], len);
	}
	m_data[len] = '\0';
}

template <typename T>
inline void VarType::put(const T& value)
{
	std::ostringstream oss;
	oss << value;
	m_data.resize(oss.str().length() + 1);
	strcpy((char*)(&m_data[0]), oss.str().c_str());
}

inline void VarType::put(const unsigned char& value)
{
	put(static_cast< unsigned short >(value));
}

inline VarType::operator unsigned char() const
{
	return atoi((const char*)(*this));;
}

inline VarType::operator char() const
{
	return atoi((const char*)(*this));;
}

inline VarType::operator unsigned short() const
{
	return atoi((const char*)(*this));;
}

inline VarType::operator short() const
{
	return atoi((const char*)(*this));;
}

inline VarType::operator unsigned int() const
{
	return atoi((const char*)(*this));;
}

inline VarType::operator int() const
{
	return atoi((const char*)(*this));;
}

inline VarType::operator unsigned long() const
{
	return strtoul((const char*)(*this), NULL, 10);
}

inline VarType::operator long() const
{
	return strtol((const char*)(*this), NULL, 10);
}

inline VarType::operator unsigned long long() const
{
	return strtoull((const char*)(*this), NULL, 10);
}

inline VarType::operator long long() const
{
	return strtoll((const char*)(*this), NULL, 10);
}

inline VarType::operator float() const
{
	return atof((const char*)(*this));
}

inline VarType::operator double() const
{
	return atof((const char*)(*this));
}

inline VarType::operator const char* () const
{
	return (const char*)(&m_data[0]);
}

inline VarType::operator string() const
{
	return string((const char*)(&m_data[0]), m_data.size() - 1);
}



inline VarType::operator const void* () const
{
	return (const void*)(&m_data[0]);
}

inline unsigned int VarType::size() const
{
	if(m_data.empty())
	{
		return 0;
	}
	else
	{
		return m_data.size() - 1;
	}
}

inline bool VarType::empty() const
{
	return m_data.empty() || (1 == m_data.size());
}

inline bool VarType::valid() const
{
	return !empty();
}
#endif
