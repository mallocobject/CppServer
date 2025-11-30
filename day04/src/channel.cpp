#include "channel.h"
#include "epoll.h"
#include "event_loop.h"
#include <sys/epoll.h>

namespace WS
{

void Channel::enableRead(bool is_ET)
{

    if (is_ET)
    {
        _op = EPOLLIN | EPOLLET;
    }
    else
    {
        _op = EPOLLIN | EPOLLPRI;
    }
    _loop->updateChannel(this);
}

void Channel::setRop(uint32_t rop)
{
    _rop = rop;
}

void Channel::handleEvent()
{
    _cb();
}
void Channel::setCallback(std::function<void()> cb)
{
    _cb = cb;
}

} // namespace WS