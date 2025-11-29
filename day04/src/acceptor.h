#ifndef __ACCEPTOR_H__
#define __ACCEPTOR_H__

#include <functional>
namespace WS
{
class EventLoop;
class Channel;
class Socket;
class Acceptor
{
  protected:
    EventLoop *_loop;
    Channel *_ch;
    Socket *_sock;
    std::function<void(Socket *)> _cb;

  public:
    Acceptor() = delete;
    Acceptor(EventLoop *loop);
    ~Acceptor();

    void setNewConnCallback(std::function<void(Socket *)> cb);
    void acceptConn();

    EventLoop *getLoop() const
    {
        return _loop;
    }
};
} // namespace WS

#endif