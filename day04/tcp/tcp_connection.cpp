#include "tcp_connection.h"
#include "buffer.h"
#include "channel.h"
// #include "utils.h"
#include "event_loop.h"
#include <arpa/inet.h>
#include <cassert>
#include <cerrno>
#include <cstdio>
#include <functional>
#include <iterator>
#include <memory>
#include <netinet/in.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFF_SIZE 1024

namespace WS
{

TcpConnection::TcpConnection(EventLoop *loop, int conn_fd, int conn_id, bool is_non_blocking)
    : _loop(loop), _conn_fd(conn_fd), _conn_id(conn_id), _state(State::DisConnected),
      _is_non_blocking(is_non_blocking)
{
    if (loop != nullptr)
    {
        _ch = std::make_unique<Channel>(conn_fd, _loop);
        _ch->setReadCallback(std::bind(&TcpConnection::handleMessage, this));
    }
    _buf_recv = std::make_unique<Buffer>();
    _buf_send = std::make_unique<Buffer>();
}

TcpConnection::~TcpConnection()
{
    if (_conn_fd != -1)
    {
        erro(::close(_conn_fd) == -1, "close failed");
        _conn_fd = -1;
    }
}

void TcpConnection::connecntionEstablished()
{
    _state = TcpConnection::State::Connected;
    _ch->tie(shared_from_this());
    _ch->enableRead();
    if (_on_connect)
    {
        _on_connect(shared_from_this());
    }
}

void TcpConnection::connectionDestructor()
{
    _loop->deleteChannel(_ch.get());
}

void TcpConnection::send(const char *msg)
{
    setMsg(msg);
    write();
}

void TcpConnection::read()
{
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

void TcpConnection::write()
{
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

void TcpConnection::typeLineSendBuffer()
{
    _buf_send->getLine();
}

const char *TcpConnection::getMsg() const
{
    return _buf_recv->c_str();
}

void TcpConnection::setMsg(const char *msg)
{
    _buf_send->setBuf(msg);
}

void TcpConnection::readBlocking()
{
    char buf[BUFF_SIZE];

    ssize_t bytes_read = ::read(_conn_fd, buf, std::size(buf));
    if (bytes_read > 0)
    {
        _buf_recv->append(buf, bytes_read);
    }
    else if (bytes_read == 0)
    {
        printf("read: EOF, server disconnected\n");
        handleClose();
    }
    else if (errno == EINTR)
    {
        printf("continue reading");
    }
    else
    {
        printf("Other error on server\n");
        handleClose();
    }
}
void TcpConnection::readNonBlocking()
{
    char buf[BUFF_SIZE];
    while (true)
    {
        ssize_t bytes_read = ::read(_conn_fd, buf, std::size(buf));
        if (bytes_read > 0)
        {
            _buf_recv->append(buf, bytes_read);
            continue;
        }
        else if (bytes_read == 0)
        {
            printf("EOF, client(%d) disconnected\n", _conn_fd);
            handleClose();
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
            printf("Other error on client(%d)\n", _conn_fd);
            handleClose();
            break;
        }
    }
}

void TcpConnection::writeBlocking()
{
    int data_left = _buf_send->size();
    int data_size = _buf_send->size();
    const char *buf = _buf_send->c_str();
    while (data_left > 0)
    {
        ssize_t bytes_write = ::write(_conn_fd, buf + data_size - data_left, data_left);
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
            handleClose();
            break;
        }
    }
}

void TcpConnection::writeNonBlocking()
{
    int data_left = _buf_send->size();
    int data_size = _buf_send->size();
    const char *buf = _buf_send->c_str();
    while (data_left > 0)
    {
        ssize_t bytes_write = ::write(_conn_fd, buf + data_size - data_left, data_left);
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
            printf("Other error on client(%d)\n", _conn_fd);
            handleClose();
            break;
        }
    }
}

void TcpConnection::connect(const char *ip, uint16_t port)
{
    sockaddr_in addr;
    bzero(&addr, sizeof(sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);
    erro(::connect(_conn_fd, (sockaddr *)&addr, sizeof(sockaddr)) == -1, "connect failed");
}

} // namespace WS