
#include "sh/net/Buffer.h"

#include "sh/net/SocketOps.h"

#include <errno.h>
#include <sys/uio.h>

using namespace sh;
using namespace net;

const char Buffer::kCRLF[] = "\r\n";
// const size_t Buffer::kCheapPrepend = 8;
// const size_t Buffer::kInitialSize = 1024;


ssize_t Buffer::readFd(int fd, int *savedErrno)
{
    char extrabuf[65536];
    struct iovec vec[2];
    const size_t writable = writableBytes();
    vec[0].iov_base = begin() + writerInedx_;
    vec[0].iov_len = writable;
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof extrabuf;
    // when there is enough space in this buffer, don't read into extrabuf.
    // when extrabuf is used, we read 128k-1 bytes at most. 每次
    const int iovcnt = (writable < sizeof extrabuf) ? 2 : 1;
    const ssize_t n = sockets::readv(fd, vec, iovcnt);
    if (n < 0)
    {
        *savedErrno = errno;
    }
    else if (implicit_cast<size_t>(n) <= writable)
    {
        writerInedx_ += n;
    }
    else
    {
        writerInedx_ = buffer_.size();
        append(extrabuf, n - writable);
    }
    return n;
}
