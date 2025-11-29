#include "event_loop.h"
#include "channel.h"
#include "epoll.h"
#include "thread_pool.h"
#include <functional>
#include <vector>

namespace WS
{
EventLoop::EventLoop() : _ep(new Epoll), _th_pool(new ThreadPool), _quit(false)
{
}

EventLoop::~EventLoop()
{
    if (_ep != nullptr)
    {
        delete _ep;
        _ep = nullptr;
    }
    if (_th_pool != nullptr)
    {
        delete _th_pool;
        _th_pool = nullptr;
    }
}
void EventLoop::loop()
{
    while (!_quit)
    {
        std::vector<Channel *> chs = _ep->wait();
        for (Channel *&ch : chs)
        {
            std::function<void()> task = std::bind(&Channel::handleEvent, ch);
            _th_pool->addTask(task);
        }
    }
}
void EventLoop::updateChannel(Channel *ch)
{
    _ep->updateChannel(ch);
}
} // namespace WS