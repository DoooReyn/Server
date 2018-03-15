#include "file_util.h"

AppendFile::AppendFile(string& filename)
{
	m_writeBytes = 0;
	m_fp = ::fopen(filename.c_str(), "ae");
	assert(m_fp != NULL);
	::setbuffer(m_fp, m_buffer, sizeof(m_buffer));
}

AppendFile::~AppendFile()
{
	::fclose(m_fp);
}

void AppendFile::append(const char* log_line, const size_t len)
{
	size_t n = write(log_line, len);
	size_t remain = len - n;
	while(remain > 0)
	{
		size_t x = write(log_line + n, remain);
		if (x == 0)
		{
			int32 err = ::ferror(m_fp);
			if (err)
			{
				::fprintf(stderr, "AppendFile::append() failed:%d\n", err);
			}
			break;
		}
		else
		{
			n += x;
			remain = len - n;
		}
	}
	m_writeBytes += len;
}

void AppendFile::flush()
{
	::fflush(m_fp);
}

size_t AppendFile::write(const char* log_line, size_t len)
{
	return ::fwrite_unlocked(log_line, 1, len, m_fp);
}

ReadSmallFile::ReadSmallFile(string& filename)
{
	m_fd = ::open(filename.c_str(), O_RDONLY | O_CLOEXEC);
	m_err = 0;
	m_buf[0] = '\0';
	if (m_fd < 0)
	{
		m_err = errno;
	}
}

ReadSmallFile::~ReadSmallFile()
{
	if(m_fd >= 0)
	{
		::close(m_fd);
	}
}

template <typename T>
int32 ReadSmallFile::ReadToString(int32 maxSize, T* content, int64* file_size, int64* modify_time, int64* create_time)
{
	STATIC_ASSERT_CHECK(sizeof(off_t) == 8);
	assert(content != NULL);
	int32 err = m_err;
	if (m_fd < 0)
	{
		return err;
	}

	content->clear();

	if(file_size)
	{
		struct stat statbuf;
		if(::fstat(m_fd, &statbuf) == 0)
		{
			if(S_ISREG(statbuf.st_mode))
			{
				*file_size = statbuf.st_size;
				content->reserve(static_cast<int>(std::min(static_cast<int64>(maxSize), *file_size)));
			}
			else if(S_ISDIR(statbuf.st_mode))
			{
				err = EISDIR;
			}
			if(modify_time)
			{
				*modify_time = statbuf.st_mtime;
			}
			if(create_time)
			{
				*create_time = statbuf.st_ctime;
			}
		}
		else
		{
			err = errno;
		}
	}
	while(content->size() < static_cast<size_t> (maxSize))
	{
		size_t toRead = std::min(static_cast<size_t>(maxSize) - content->size(), sizeof(m_buf));
		ssize_t n = ::read(m_fd, m_buf, toRead);
		if(n > 0)
		{
			content->append(m_buf, n);
		}
		else
		{
			if(n < 0)
			{
				err = errno;
			}
			break;
		}
	}
	return  err;
}


int32 ReadSmallFile::ReadToBuffer(int32* size)
{
	int32 err = m_err;
	if (m_fd < 0)
	{
		return err;
	}
	ssize_t n = ::pread(m_fd, m_buf, sizeof(m_buf) - 1, 0);
	if (n > 0)
	{
		if(size)
		{
			*size = static_cast<int32>(n);
		}
		m_buf[n] = '\0';
	}
	else
	{
		err = errno;
	}

	return err;
}

template int32 ReadFile(string& filename,
                        int32 maxSize,
                        string* content,
                        int64*, int64*, int64*);

template int32 ReadSmallFile::ReadToString(
    int32 maxSize,
    string* content,
    int64*, int64*, int64*);
