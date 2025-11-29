#include "thread_pool.h"
#include <functional>
#include <mutex>
#include <thread>

namespace WS
{
ThreadPool::ThreadPool(size_t num_ths) : _stop(false)
{
    for (int i = 0; i < num_ths; i++)
    {
        _vec_ths.push_back(std::thread([this]() {
            while (true)
            {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(_mtx);
                    _cv.wait(lock, [this]() { return _stop || !_que_tasks.empty(); });
                    if (_stop && _que_tasks.empty())
                    {
                        return;
                    }
                    task = _que_tasks.front();
                    _que_tasks.pop();
                }
                task();
            }
        }));
    }
}

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(_mtx);
        _stop = true;
    }
    _cv.notify_all();
    for (std::thread &th : _vec_ths)
    {
        if (th.joinable())
        {
            th.join();
        }
    }
}

} // namespace WS