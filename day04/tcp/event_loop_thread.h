#ifndef __EVENT_LOOP_THREAD_H__
#define __EVENT_LOOP_THREAD_H__

#include "../base/common.h"
#include <condition_variable>
#include <mutex>
#include <thread>
namespace WS
{
class EventLoop;
class EventLoopThread
{
  protected:
    std::mutex _mtx;
    std::condition_variable _cv;
    EventLoop *_loop;
    std::thread _th;

  public:
    DISALLOW_COPY_AND_MOVE(EventLoopThread);

    EventLoopThread() : _loop(nullptr)
    {
    }

    ~EventLoopThread()
    {
        if (_th.joinable())
        {
            _th.join();
        }
    }

    EventLoop *startLoop();

  protected:
    void doThreadFunc();
};
} // namespace WS

#endif