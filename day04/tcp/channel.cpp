#include "channel.h"
#include "epoll.h"
#include "event_loop.h"
#include <cstdint>
#include <functional>
#include <sys/epoll.h>

namespace WS
{

void Channel::handleEvent()
{
    if (_ready_events & (EPOLLIN | EPOLLPRI | EPOLLRDHUP))
    {
        if (_read_callback)
        {
            _read_callback();
        }
    }
    if (_ready_events & EPOLLOUT)
    {
        if (_write_callback)
        {
            _write_callback();
        }
    }
}

void Channel::enableRead(bool is_ET)
{

    _listen_events |= EPOLLIN | EPOLLPRI;
    if (is_ET)
    {
        _listen_events |= EPOLLET;
    }
    _loop->updateChannel(this);
}

void Channel::enableWrite()
{
    _listen_events |= EPOLLOUT;
    _loop->updateChannel(this);
}

void Channel::disableWrite()
{
    _listen_events &= ~EPOLLOUT;
    _loop->updateChannel(this);
}

void Channel::setReadyEvents(uint32_t ev)
{
    _ready_events = ev;
}

} // namespace WS