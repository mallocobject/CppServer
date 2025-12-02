#include "event_loop_thread_pool.h"
#include "event_loop_thread.h"
#include <memory>

namespace WS
{

EventLoopThreadPool::EventLoopThreadPool(EventLoop *main_loop, size_t num_ths)
    : _main_loop(main_loop), _num_ths(num_ths), _next_loop(0)
{
}

EventLoopThreadPool::~EventLoopThreadPool()
{
}

void EventLoopThreadPool::start()
{
    for (int i = 0; i < _num_ths; i++)
    {
        _vec_loop_th.push_back(std::make_unique<EventLoopThread>());
        _vec_loop.push_back(_vec_loop_th.back()->startLoop());
    }
}
} // namespace WS