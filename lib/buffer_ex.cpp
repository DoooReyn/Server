#include "buffer_ex.h"
#include "logger.h"

const char BufferEx::kCRLF[] = "\r\n";

ssize_t BufferEx::ReadFd(int fd, int* saveErrno)
{
	char extrabuf[65536];
	struct iovec vec[2];

	const size_t writeable = WriteableBytes();
	vec[0].iov_base = begin() + m_write_index;
	vec[0].iov_len = writeable;
	vec[1].iov_base = extrabuf;
	vec[1].iov_len = sizeof(extrabuf);

	const int iovcnt = (writeable < sizeof(extrabuf) ? 2 : 1);
	const ssize_t n = ::readv(fd, vec, iovcnt);
	if(n < 0)
	{
		*saveErrno = errno;
	}
	else if( static_cast<size_t>(n) <= writeable)
	{
		m_write_index += n;
	}
	else
	{
		m_write_index = m_buffer.size();
		append(extrabuf, n - writeable);
	}

	//DEBUG("ReadFd:%ld", n);

	return n;
}
