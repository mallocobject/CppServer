#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <sys/socket.h>
#include <sys/types.h>
namespace WS
{

class InetAddr;

class Socket
{
  protected:
    int _fd;

  public:
    Socket();

    Socket(int fd) : _fd(fd)
    {
    }

    ~Socket();

    void bind(InetAddr *inet_addr);
    void listen();
    int accept(InetAddr *inet_addr);
    void connect(InetAddr *inet_addr);
    void close();

    ssize_t write(const char *buf, size_t nbytes);
    ssize_t read(char *buf, size_t nbytes);

    int getFd() const
    {
        return _fd;
    }

    void set_non_blocking();
};
} // namespace WS

#endif