#include "connection.h"
#include "channel.h"
#include "socket.h"
#include "utils.h"
#include <cstdio>
#include <functional>
#include <strings.h>

#define BUFF_SIZE 1024

namespace WS
{

Connection::Connection(EventLoop *loop, Socket *sock) : _loop(loop), _sock(sock), _ch(nullptr)
{
    _ch = new Channel(loop, sock->getFd());
    std::function<void()> cb = std::bind(&Connection::echo, this, sock);
    _ch->setCallback(cb);
    _ch->enableReading();
}

Connection::~Connection()
{
    if (_sock != nullptr)
    {
        delete _sock;
        _sock = nullptr;
    }
    if (_ch != nullptr)
    {
        delete _ch;
        _ch = nullptr;
    }
}

void Connection::echo(Socket *sock)
{
    char buf[BUFF_SIZE + 1];
    size_t nbytes = BUFF_SIZE;
    while (true)
    {
        bzero(buf, sizeof(buf));
        ssize_t bytes_read = sock->read(buf, nbytes);
        if (bytes_read > 0)
        {
            buf[bytes_read] = 0;
            printf("messgae from client fd: %d: %s\n", sock->getFd(), buf);
            sock->write(buf, bytes_read);
        }
        else if (bytes_read == -1 && errno == EINTR)
        {
            printf("continue reading");
            continue;
        }
        else if (bytes_read == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
        {
            printf("finish reading once\n");
            break;
        }
        else if (bytes_read == 0)
        {
            printf("EOF, client fd: %d disconnected\n", sock->getFd());
            _del_cb(sock);
            break;
        }
    }
}

void Connection::setDelConnCallback(std::function<void(Socket *)> del_cb)
{
    _del_cb = del_cb;
}
} // namespace WS