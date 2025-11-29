#include "channel.h"
#include "epoll.h"
#include "event_loop.h"

namespace WS
{

void Channel::enableReading()
{
    _op = EPOLLIN | EPOLLET;
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