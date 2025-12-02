#ifndef __ACCEPTOR_H__
#define __ACCEPTOR_H__

#include "../base/common.h"
#include <cstdint>
#include <functional>
#include <memory>
namespace WS
{
class EventLoop;
class Channel;
class Acceptor
{
  protected:
    EventLoop *_loop;
    int _fd;
    std::unique_ptr<Channel> _ch;
    std::function<void(int)> _new_connection_callback;

  public:
    DISALLOW_COPY_AND_MOVE(Acceptor)
    DISALLOW_DEFAULT(Acceptor)

    Acceptor(EventLoop *loop, const char *ip, uint16_t port);
    ~Acceptor();

    void setNewConnectionCallback(const std::function<void(int)> &cb);
    void acceptNewConnection();

  protected:
    void create();
    void bind(const char *ip, uint16_t port);
    void listen();
};
} // namespace WS

#endif