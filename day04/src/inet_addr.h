#ifndef __INET_ADDR_H__
#define __INET_ADDR_H__

#include <arpa/inet.h>
#include <cstdint>
#include <netinet/in.h>
#include <string>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>

namespace WS
{
class InetAddr
{
  protected:
    sockaddr_in _addr;
    socklen_t _addr_len;

  public:
    InetAddr() = default;
    InetAddr(const std::string &hostname, uint16_t port)
    {
        bzero(&_addr, sizeof(_addr));
        _addr.sin_family = AF_INET;
        _addr.sin_addr.s_addr = inet_addr(hostname.c_str());
        _addr.sin_port = htons(port);
        _addr_len = sizeof(sockaddr_in);
    }

    ~InetAddr()
    {
    }

    sockaddr *getSockaddr()
    {
        return reinterpret_cast<sockaddr *>(&_addr);
    }

    const char *getIP() const
    {
        return inet_ntoa(_addr.sin_addr);
    }

    const uint16_t getPort() const
    {
        return ntohs(_addr.sin_port);
    }

    socklen_t getSocklen() const
    {
        return sizeof(sockaddr_in);
    }

    socklen_t *getSocklenPtr()
    {
        return &_addr_len;
    }
};
} // namespace WS

#endif