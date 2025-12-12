#ifndef __CHANNEL_H__
#define __CHANNEL_H__

#include "../base/common.h"
#include "../base/utils.h"
#include <cstdint>
#include <functional>
#include <memory>
#include <unistd.h>
namespace WS
{
class EventLoop;
class Channel
{
  protected:
    EventLoop* _loop;
    int _fd;
    uint32_t _listen_events;
    uint32_t _ready_events;
    bool _in_epoll;
    std::function<void()> _read_callback;
    std::function<void()> _write_callback;
    std::weak_ptr<void> _tie;
    bool _tied;

  public:
    DISALLOW_DEFAULT(Channel)
    DISALLOW_COPY_AND_MOVE(Channel)

    Channel(int fd, EventLoop* loop)
        : _fd(fd), _loop(loop), _listen_events(0), _ready_events(0),
          _in_epoll(false), _tied(false)
    {
    }

    ~Channel()
    {
        if (_fd != -1)
        {
            erro(::close(_fd) == -1, "close failed");
            _fd = -1;
        }
    }

    void tie(const std::shared_ptr<void>& ptr);

    void handleEvent();

    void enableRead(bool is_ET = true);
    void enableWrite();
    void disableWrite();

    int getFd() const { return _fd; }

    uint32_t getListenEvents() const { return _listen_events; }

    uint32_t getReadyEvents() const { return _ready_events; }

    bool inEpoller() const { return _in_epoll; }

    void setInEpoller(bool in_epoll) { _in_epoll = in_epoll; }

    void setReadyEvents(uint32_t ev);

    void setReadCallback(const std::function<void()>& cb)
    {
        _read_callback = std::move(cb);
    }
    void setWriteCallback(const std::function<void()>& cb)
    {
        _write_callback = std::move(cb);
    }

  protected:
    void handleEventWithGuard();
};
} // namespace WS

#endif