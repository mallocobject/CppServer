#include "acceptor.h"
#include "channel.h"
#include "inet_addr.h"
#include "server.h"
#include "socket.h"
#include <arpa/inet.h>
#include <cstdio>
#include <functional>
#include <netinet/in.h>

namespace WS
{
Acceptor::Acceptor(EventLoop *loop) : _loop(loop)
{
    _sock = new Socket;
    InetAddr addr("127.0.0.1", 8888);
    _sock->bind(&addr);
    _sock->listen();
    _sock->set_non_blocking();

    _ch = new Channel(loop, _sock->getFd());
    std::function<void()> cb = std::bind(&Acceptor::acceptConn, this);
    _ch->setCallback(cb);
    _ch->enableReading();
}

Acceptor::~Acceptor()
{
    if (_ch != nullptr)
    {
        delete _ch;
        _ch = nullptr;
    }
    if (_sock != nullptr)
    {
        _sock->close();
        delete _sock;
        _sock = nullptr;
    }
}

void Acceptor::setNewConnCallback(std::function<void(Socket *)> cb)
{
    _cb = cb;
}

void Acceptor::acceptConn()
{
    InetAddr clnt_addr;
    Socket *clnt_sock = new Socket(_sock->accept(&clnt_addr));
    printf("new client fd %d! IP: %s Port: %d\n", clnt_sock->getFd(), clnt_addr.getIP(),
           clnt_addr.getPort());
    clnt_sock->set_non_blocking();
    _cb(clnt_sock);
}
} // namespace WS