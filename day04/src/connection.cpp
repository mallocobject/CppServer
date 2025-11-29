#include "connection.h"
#include "buffer.h"
#include "channel.h"
#include "socket.h"
#include "utils.h"
#include <cstdio>
#include <functional>
#include <strings.h>
#include <utility>

#define BUFF_SIZE 1024

namespace WS
{

Connection::Connection(EventLoop *loop, Socket *sock)
    : _loop(loop), _sock(sock), _ch(nullptr), _buf(nullptr), _del_cb(nullptr)
{
    _ch = new Channel(loop, sock->getFd());
    // std::function<void()> cb = std::bind(&Connection::echo, this);
    // _ch->setCallback(cb);
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

void Connection::echo()
{
    char buf[BUFF_SIZE];
    while (true)
    {
        ssize_t bytes_read = _sock->read(buf, sizeof(buf));
        if (bytes_read > 0)
        {
            _buf->append(buf, bytes_read);
        }
        else if (bytes_read == 0)
        {
            printf("EOF, client(%d) disconnected\n", _sock->getFd());
            _del_cb(_sock);
            break;
        }
        else if (errno == EINTR)
        {
            printf("continue reading");
            continue;
        }
        else if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            printf("client(%d): %s\n", _sock->getFd(), _buf->c_str());
            _sock->write(_buf->c_str(), _buf->size());
            _buf->clear();
            break;
        }
    }
}

void Connection::setDelConnCallback(std::function<void(Socket *)> del_cb)
{
    _del_cb = std::move(del_cb);
}

void Connection::setOnConnCallback(std::function<void(Connection *)> on_connect_cb)
{
    // _on_connect_cb = std::move(on_connect_cb);

    _ch->setCallback([this, on_connect_cb]() { on_connect_cb(this); });
}
} // namespace WS