#include "event_loop.h"
#include "channel.h"
#include "epoller.h"
#include "sys/eventfd.h"
#include <functional>
#include <memory>
#include <sys/eventfd.h>
#include <thread>
#include <unistd.h>
#include <vector>

namespace WS
{
EventLoop::EventLoop() : _calling_func(false)
{
    _ep = std::make_unique<Epoller>();
    _wake_fd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    _wake_ch = std::make_unique<Channel>(_wake_fd, this);

    _wake_ch->setReadCallback(std::bind(&EventLoop::handleRead, this));
    _wake_ch->enableRead(false);
}

EventLoop::~EventLoop()
{
    deleteChannel(_wake_ch.get());
    if (_wake_fd != -1)
    {
        erro(::close(_wake_fd) == -1, "close failed");
        _wake_fd = -1;
    }
}

void EventLoop::loop()
{
    _tid = std::this_thread::get_id();
    while (true)
    {
        std::vector<Channel *> chs = _ep->poll();
        for (Channel *&ch : chs)
        {
            ch->handleEvent();
        }
        doTodoList();
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