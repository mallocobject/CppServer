#include "event_loop_thread.h"
#include "event_loop.h"
#include <functional>
#include <mutex>
#include <thread>

namespace WS
{

void EventLoopThread::doThreadFunc()
{
    EventLoop loop;
    {
        std::lock_guard<std::mutex> lock(_mtx);
        _loop = &loop;
        _cv.notify_one();
    }
    _loop->loop();
    {
        std::lock_guard<std::mutex> lock(_mtx);
        _loop = nullptr;
    }
}

EventLoop *EventLoopThread::startLoop()
{
    _th = std::thread(std::bind(&EventLoopThread::doThreadFunc, this));
    EventLoop *loop = nullptr;
    {
        std::unique_lock<std::mutex> lock(_mtx);
        _cv.wait(lock, [this]() { return _loop != nullptr; });
        loop = _loop;
    }
    return loop;
}

} // namespace WS