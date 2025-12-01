#include "acceptor.h"
#include "channel.h"
#include "event_loop.h"
#include <arpa/inet.h>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <memory>
#include <netinet/in.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>
#include <utility>

namespace WS
{

Acceptor::Acceptor(EventLoop *loop, const char *ip, uint16_t port) : _loop(loop), _fd(-1)
{
    create();
    bind(ip, port);
    listen();

    _ch = std::make_unique<Channel>(_fd, _loop);
    std::function<void()> cb = std::bind(&Acceptor::acceptNewConnection, this);
    _ch->setReadCallback(cb);
    _ch->enableRead(false);
}

Acceptor::~Acceptor()
{
    _loop->deleteChannel(_ch.get());
    if (_fd != -1)
    {
        erro(::close(_fd) == -1, "close failed");
        _fd = -1;
    }
}

void Acceptor::setNewConnectionCallback(const std::function<void(int)> &cb)
{
    _new_connection_callback = std::move(cb);
}

void Acceptor::acceptNewConnection()
{
    sockaddr_in client_addr;
    socklen_t client_addr_length = sizeof(sockaddr_in);
    bzero(&client_addr, sizeof(sockaddr_in));

    int client_fd =
        ::accept4(_fd, (sockaddr *)&client_addr, &client_addr_length, SOCK_NONBLOCK | SOCK_CLOEXEC);
    printf("new client(%d) IP: %s Port: %d\n", client_fd, inet_ntoa(client_addr.sin_addr),
           ntohs(client_addr.sin_port));
    if (_new_connection_callback)
    {
        _new_connection_callback(client_fd);
    }
}

void Acceptor::create()
{
    assert(_fd == -1);
    _fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    erro(_fd == -1, "create sockfd failed");
}

void Acceptor::bind(const char *ip, uint16_t port)
{
    sockaddr_in addr;
    bzero(&addr, sizeof(sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);
    erro(::bind(_fd, (sockaddr *)&addr, sizeof(sockaddr)) == -1, "bind addr failed");
}

void Acceptor::listen()
{
    erro(::listen(_fd, SOMAXCONN) == -1, "listen setting failed");
}

} // namespace WS