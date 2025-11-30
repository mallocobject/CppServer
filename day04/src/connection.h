#ifndef __CONNECTION_H__
#define __CONNECTION_H__

#include <algorithm>
#include <functional>
namespace WS
{
class EventLoop;
class Channel;
class Socket;
class Buffer;
class Connection
{
  public:
    enum State
    {
        Invalid = 1,
        Handshaking,
        Connected,
        Closed,
        Failed,
    };

  protected:
    EventLoop *_loop;
    Channel *_ch;
    Socket *_sock;
    std::function<void(Socket *)> _del_cb;
    // std::function<void(Connection *)> _on_connect_cb;
    Buffer *_buf_recv;
    Buffer *_buf_send;
    State _state;
    bool _is_non_blocking;

  public:
    Connection() = delete;
    Connection(EventLoop *loop, Socket *sock, bool is_non_blocking = true);
    ~Connection();

    // void echo();

    void setDelConnCallback(std::function<void(Socket *)> del_cb)
    {
        _del_cb = std::move(del_cb);
    }

    void setOnConnCallback(std::function<void(Connection *)> on_connect_cb);

    void read();
    void write();
    void close()
    {
        _del_cb(_sock);
    }

    State getState() const
    {
        return _state;
    }

    void typeLineSendBuffer();
    Socket *getSocket() const
    {
        return _sock;
    }

    const char *getMsg() const;
    void setMsg(const char *msg);

  protected:
    void readBlocking();
    void readNonBlocking();

    void writeBlocking();
    void writeNonBlocking();
};
} // namespace WS

#endif