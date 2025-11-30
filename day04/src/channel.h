#ifndef __CHANNEL_H__
#define __CHANNEL_H__

#include <cstdint>
#include <functional>
namespace WS
{
class EventLoop;
class Channel
{
  protected:
    EventLoop *_loop;
    int _fd;
    uint32_t _op;
    uint32_t _rop;
    bool _in_epoll;
    std::function<void()> _cb;

  public:
    Channel() = delete;
    Channel(EventLoop *loop, int fd)
        : _loop(loop), _fd(fd), _op(0), _rop(0), _in_epoll(false), _cb(nullptr)
    {
    }

    void handleEvent();
    void setCallback(std::function<void()> cb);

    void enableRead(bool is_ET = true);
    int getFd() const
    {
        return _fd;
    }

    uint32_t getOp() const
    {
        return _op;
    }

    uint32_t getRop() const
    {
        return _rop;
    }

    bool getInEpoll() const
    {
        return _in_epoll;
    }

    void setInEpoll()
    {
        _in_epoll = true;
    }

    void setRop(uint32_t rop);
};
} // namespace WS

#endif