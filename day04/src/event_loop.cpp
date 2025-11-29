#include "event_loop.h"
#include "channel.h"
#include "epoll.h"
#include <vector>

namespace WS
{
EventLoop::~EventLoop()
{
    if (_ep != nullptr)
    {
        delete _ep;
        _ep = nullptr;
    }
}
void EventLoop::loop()
{
    while (!_quit)
    {
        std::vector<Channel *> chs = _ep->wait();
        for (Channel *&ch : chs)
        {
            ch->handleEvent();
        }
    }
}
void EventLoop::updateChannel(Channel *ch)
{
    _ep->updateChannel(ch);
}
} // namespace WS