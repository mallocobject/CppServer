#ifndef __TcpConnection_H__
#define __TcpConnection_H__

#include "../base/common.h"
#include <functional>
#include <memory>
namespace WS
{
class EventLoop;
class Channel;
class Buffer;
class TcpConnection : public std::enable_shared_from_this<TcpConnection>
{
  public:
    enum State
    {
        Invalid = 1,
        Connected,
        DisConnected,
    };

  protected:
    EventLoop *_loop;
    int _conn_fd;
    int _conn_id;
    std::function<void(const std::shared_ptr<TcpConnection> &)> _on_close;
    std::function<void(const std::shared_ptr<TcpConnection> &)> _on_message;
    std::function<void(const std::shared_ptr<TcpConnection> &)> _on_connect;

    std::unique_ptr<Channel> _ch;
    std::unique_ptr<Buffer> _buf_recv;
    std::unique_ptr<Buffer> _buf_send;
    State _state;
    bool _is_non_blocking;

  public:
    DISALLOW_DEFAULT(TcpConnection)
    DISALLOW_COPY_AND_MOVE(TcpConnection)

    TcpConnection(EventLoop *loop, int conn_fd, int conn_id, bool is_non_blocking = true);
    ~TcpConnection();

    void connecntionEstablished();

    void connectionDestructor();

    void setCloseCallback(const std::function<void(const std::shared_ptr<TcpConnection> &)> &cb)
    {
        _on_close = std::move(cb);
    }

    void setMessageCallback(const std::function<void(const std::shared_ptr<TcpConnection> &)> &cb)
    {
        _on_message = std::move(cb);
    }

    void setConnectionCallback(
        const std::function<void(const std::shared_ptr<TcpConnection> &)> &cb)
    {
        _on_connect = std::move(cb);
    }

    void handleMessage()
    {

        read();
        if (_on_message)
        {
            _on_message(shared_from_this());
        }
    }

    void handleClose()
    {
        if (_state != TcpConnection::State::DisConnected)
        {
            _state = TcpConnection::State::DisConnected;
            if (_on_close)
            {
                _on_close(shared_from_this());
            }
        }
    }

    void send(const char *msg);
    void read();
    void write();

    EventLoop *getEventLoop() const
    {
        return _loop;
    }

    int getId() const
    {
        return _conn_id;
    }

    int getFd() const
    {
        return _conn_fd;
    }

    State State() const
    {
        return _state;
    }

    void typeLineSendBuffer();

    const char *getMsg() const;
    void setMsg(const char *msg);

  protected:
    void readBlocking();
    void readNonBlocking();

    void writeBlocking();
    void writeNonBlocking();

    void connect(const char *ip, uint16_t port);
};
} // namespace WS

#endif