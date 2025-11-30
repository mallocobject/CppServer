#include "event_loop.h"
#include "channel.h"
#include "epoll.h"
#include <memory>
#include <vector>

namespace WS
{
EventLoop::EventLoop()
{
    _ep = std::make_unique<Epoll>();
}

EventLoop::~EventLoop()
{
}

void EventLoop::loop()
{
    while (true)
    {
        std::vector<Channel *> chs = _ep->poll();
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

void EventLoop::deleteChannel(Channel *ch)
{
    _ep->deleteChannel(ch);
}
} // namespace WS