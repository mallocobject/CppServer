#include "connection.h"
#include "buffer.h"
#include "channel.h"
#include "socket.h"
#include "utils.h"
#include <cstdio>
#include <functional>
#include <strings.h>

#define BUFF_SIZE 1024

namespace WS
{

Connection::Connection(EventLoop *loop, Socket *sock)
    : _loop(loop), _sock(sock), _ch(nullptr), _buf(nullptr)
{
    _ch = new Channel(loop, sock->getFd());
    std::function<void()> cb = std::bind(&Connection::echo, this, sock);
    _ch->setCallback(cb);
    _ch->enableRead();
    _buf = new Buffer;
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
    char buf[BUFF_SIZE];
    while (true)
    {
        ssize_t bytes_read = sock->read(buf, sizeof(buf));
        if (bytes_read > 0)
        {
            _buf->append(buf, bytes_read);
        }
        else if (bytes_read == 0)
        {
            printf("EOF, client(%d) disconnected\n", sock->getFd());
            _del_cb(sock);
            break;
        }
        else if (errno == EINTR)
        {
            printf("continue reading");
            continue;
        }
        else if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            printf("client(%d): %s\n", sock->getFd(), _buf->c_str());
            sock->write(_buf->c_str(), _buf->size());
            _buf->clear();
            break;
        }
    }
}

void Connection::setDelConnCallback(std::function<void(Socket *)> del_cb)
{
    _del_cb = del_cb;
}
} // namespace WS