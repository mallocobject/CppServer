#ifndef __CONNECTION_H__
#define __CONNECTION_H__

#include <functional>
namespace WS
{
class EventLoop;
class Channel;
class Socket;
class Buffer;
class Connection
{
  protected:
    EventLoop *_loop;
    Channel *_ch;
    Socket *_sock;
    std::function<void(Socket *)> _del_cb;
    // std::function<void(Connection *)> _on_connect_cb;
    Buffer *_buf;

  public:
    Connection() = delete;
    Connection(EventLoop *loop, Socket *sock);
    ~Connection();

    void echo();

    void setDelConnCallback(std::function<void(Socket *)> del_cb);
    void setOnConnCallback(std::function<void(Connection *)> on_connect_cb);
};
} // namespace WS

#endif