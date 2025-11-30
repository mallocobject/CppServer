#include "connection.h"
#include "buffer.h"
#include "channel.h"
#include "socket.h"
// #include "utils.h"
#include <cassert>
#include <cerrno>
#include <cstdio>
#include <functional>
#include <strings.h>
#include <unistd.h>

#define BUFF_SIZE 1024

namespace WS
{

Connection::Connection(EventLoop *loop, Socket *sock, bool is_non_blocking)
    : _loop(loop), _sock(sock), _ch(nullptr), _del_cb(nullptr), _buf_recv(new Buffer),
      _buf_send(new Buffer), _state(State::Connected), _is_non_blocking(is_non_blocking)
{
    if (loop != nullptr)
    {
        _ch = new Channel(loop, sock->getFd());
        _ch->enableRead();
    }
}

Connection::~Connection()
{
    if (_sock != nullptr)
    {
        delete _sock;
        _sock = nullptr;
    }
    if (_loop != nullptr && _ch != nullptr)
    {
        delete _ch;
        _ch = nullptr;
    }
    if (_buf_send != nullptr)
    {
        delete _buf_send;
        _buf_send = nullptr;
    }
    if (_buf_recv != nullptr)
    {
        delete _buf_recv;
        _buf_recv = nullptr;
    }
}

void Connection::setOnConnCallback(std::function<void(Connection *)> on_connect_cb)
{
    // _on_connect_cb = std::move(on_connect_cb);

    _ch->setCallback([this, on_connect_cb]() { on_connect_cb(this); });
}

void Connection::read()
{
    assert(_state == State::Connected);
    _buf_recv->clear();
    if (_is_non_blocking)
    {
        readNonBlocking();
    }
    else
    {
        readBlocking();
    }
}

void Connection::write()
{
    assert(_state == State::Connected);
    if (_is_non_blocking)
    {
        writeNonBlocking();
    }
    else
    {
        writeBlocking();
    }
    _buf_send->clear();
}

void Connection::typeLineSendBuffer()
{
    _buf_send->getLine();
}

const char *Connection::getMsg() const
{
    return _buf_recv->c_str();
}

void Connection::setMsg(const char *msg)
{
    _buf_send->setBuf(msg);
}

void Connection::readBlocking()
{
    int sockfd = _sock->getFd();
    char buf[BUFF_SIZE];

    ssize_t bytes_read = _sock->read(buf, sizeof(buf));
    if (bytes_read > 0)
    {
        _buf_recv->append(buf, bytes_read);
    }
    else if (bytes_read == 0)
    {
        printf("read: EOF, server disconnected\n");
        _state = State::Closed;
    }
    else if (errno == EINTR)
    {
        printf("continue reading");
    }
    else
    {
        printf("Other error on server\n");
        _state = State::Closed;
    }
}
void Connection::readNonBlocking()
{
    int sockfd = _sock->getFd();
    char buf[BUFF_SIZE];
    while (true)
    {
        ssize_t bytes_read = _sock->read(buf, sizeof(buf));
        if (bytes_read > 0)
        {
            _buf_recv->append(buf, bytes_read);
            continue;
        }
        else if (bytes_read == 0)
        {
            printf("EOF, client(%d) disconnected\n", sockfd);
            _state = State::Closed;
            break;
        }
        else if (errno == EINTR)
        {
            printf("continue reading");
            continue;
        }
        else if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            break;
        }
        else
        {
            printf("Other error on client(%d)\n", sockfd);
            _state = State::Closed;
            break;
        }
    }
}

void Connection::writeBlocking()
{
    int sockfd = _sock->getFd();
    int data_left = _buf_send->size();
    int data_size = _buf_send->size();
    const char *buf = _buf_send->c_str();
    while (data_left > 0)
    {
        ssize_t bytes_write = _sock->write(buf + data_size - data_left, data_left);
        if (bytes_write > 0)
        {
            data_left -= bytes_write;
            continue;
        }
        else if (bytes_write == -1 && errno == EINTR)
        {
            printf("continue writing");
            continue;
        }
        else
        {
            printf("Other error on server\n");
            _state = State::Closed;
            break;
        }
    }
}

void Connection::writeNonBlocking()
{
    int sockfd = _sock->getFd();
    int data_left = _buf_send->size();
    int data_size = _buf_send->size();
    const char *buf = _buf_send->c_str();
    while (data_left > 0)
    {
        ssize_t bytes_write = _sock->write(buf + data_size - data_left, data_left);
        if (bytes_write > 0)
        {
            data_left -= bytes_write;
            continue;
        }
        else if (bytes_write == -1 && errno == EINTR)
        {
            printf("continue writing");
            continue;
        }
        else if (bytes_write == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
        {
            break;
        }
        else
        {
            printf("Other error on client(%d)\n", sockfd);
            _state = State::Closed;
            break;
        }
    }
}

} // namespace WS