#ifndef __CONNECTION_H__
#define __CONNECTION_H__

#include <functional>
namespace WS
{
class EventLoop;
class Channel;
class Socket;
class Connection
{
  protected:
    EventLoop *_loop;
    Channel *_ch;
    Socket *_sock;
    std::function<void(Socket *)> _del_cb;

  public:
    Connection() = delete;
    Connection(EventLoop *loop, Socket *sock);
    ~Connection();

    void echo(Socket *sock);

    void setDelConnCallback(std::function<void(Socket *)> del_cb);
};
} // namespace WS

#endif