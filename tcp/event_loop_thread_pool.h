#ifndef __EVENT_LOOP_THREAD_POOL_H__
#define __EVENT_LOOP_THREAD_POOL_H__

#include "common.h"
#include <cstddef>
#include <memory>
#include <thread>
#include <vector>
namespace WS
{
class EventLoop;
class EventLoopThread;
class EventLoopThreadPool
{

  protected:
    size_t _num_ths;
    EventLoop *_main_loop;
    std::vector<EventLoop *> _vec_loop;
    std::vector<std::unique_ptr<EventLoopThread>> _vec_loop_th;
    size_t _next_loop;

  public:
    DISALLOW_DEFAULT(EventLoopThreadPool)
    DISALLOW_COPY_AND_MOVE(EventLoopThreadPool)

    EventLoopThreadPool(EventLoop *main_loop, size_t num_ths = std::thread::hardware_concurrency());

    ~EventLoopThreadPool();

    void startup();
    EventLoop *getNextLoop()
    {
        EventLoop *_loop = _main_loop;
        if (!_vec_loop.empty())
        {
            _loop = _vec_loop[_next_loop++];
            if (_next_loop >= _vec_loop.size())
            {
                _next_loop = 0;
            }
        }
        return _loop;
    }
};
} // namespace WS

#endif