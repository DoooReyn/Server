#ifndef __FILE_UTIL_H__
#define __FILE_UTIL_H__
#include "type_define.h"
#include "nocopyable.h"

class ReadSmallFile: public Nocopyable
{
public:
	ReadSmallFile(string& filename);
	~ReadSmallFile();

	template<typename T>
	int32 ReadToString(int32 maxSize, T* content, int64* file_size, int64* modify_time, int64* create_time);
	int32 ReadToBuffer(int32* size);
	const char* Buffer() const {return m_buf;}

	static const int kBufferSize = 64 * 1024;
private:
	int32 m_fd;
	int32 m_err;
	char m_buf[kBufferSize];
};

class AppendFile : public Nocopyable
{
public:
	explicit AppendFile(string& filename);
	~AppendFile();

	void append(const char* log_line, const size_t len);

	void flush();

	size_t WrittenBytes() const {return m_writeBytes;};
private:
	size_t write(const char* log_line, size_t len);

	FILE* m_fp;
	char m_buffer[64 * 1024];
	size_t m_writeBytes;
};

template<typename String>
int32 ReadFile(string& filename,
               int32 maxSize,
               String* content,
               int64* fileSize = NULL,
               int64* modifyTime = NULL,
               int64* createTime = NULL)
{
	ReadSmallFile file(filename);
	return file.ReadToString(maxSize, content, fileSize, modifyTime, createTime);
}


#endif
