#include "socket.h"
#include "inet_addr.h"
#include "utils.h"
#include <cstddef>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

namespace WS
{
Socket::Socket()
{

    _fd = socket(AF_INET, SOCK_STREAM, 0);
    erro(_fd == -1, "create socket failed");
}

Socket::~Socket()
{
    if (_fd != -1)
    {
        close();
        _fd = -1;
    }
}

void Socket::bind(InetAddr *inet_addr)
{
    erro(::bind(_fd, inet_addr->getSockaddr(), inet_addr->getSocklen()), "bind failed");
}

void Socket::listen()
{
    erro(::listen(_fd, SOMAXCONN), "listen failed");
}
int Socket::accept(InetAddr *inet_addr)
{
    int clnt_sockfd = ::accept(_fd, inet_addr->getSockaddr(), inet_addr->getSocklenPtr());
    erro(clnt_sockfd == -1, "accept failed");
    return clnt_sockfd;
}
void Socket::connect(InetAddr *inet_addr)
{
    erro(::connect(_fd, inet_addr->getSockaddr(), inet_addr->getSocklen()) == -1, "connect failed");
}

void Socket::close()
{
    erro(::close(_fd) == -1, "close failed");
}

ssize_t Socket::write(const char *buf, size_t nbytes)
{
    return ::write(_fd, buf, nbytes);
}

ssize_t Socket::read(char *buf, size_t nbytes)
{
    return ::read(_fd, buf, nbytes);
}

void Socket::set_non_blocking()
{
    int flags = fcntl(_fd, F_GETFL, 0);
    erro(flags == -1, "fail to get flags");
    flags |= O_NONBLOCK;
    erro(fcntl(_fd, F_SETFL, flags) == -1, "fail to set flags");
}

} // namespace WS