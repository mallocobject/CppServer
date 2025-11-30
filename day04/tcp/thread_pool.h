#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

#include "common.h"
#include "utils.h"
#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>
namespace WS
{

class ThreadPool
{
  protected:
    std::vector<std::thread> _vec_ths;
    std::queue<std::function<void()>> _que_tasks;
    std::mutex _mtx;
    std::condition_variable _cv;
    std::atomic<bool> _stop;

  public:
    DISALLOW_DEFAULT(ThreadPool)
    DISALLOW_COPY_AND_MOVE(ThreadPool)

    ThreadPool(size_t num_ths = std::thread::hardware_concurrency());
    ~ThreadPool();

    template <typename F, typename... Args>
    auto addTask(F &&f, Args &&...args) -> std::future<typename std::result_of<F(Args...)>::type>
    {
        using result_type = typename std::result_of<F(Args...)>::type;
        auto task = std::make_shared<std::packaged_task<result_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        std::future<result_type> result = task->get_future();
        {
            std::lock_guard<std::mutex> lock(_mtx);
            erro(_stop, "ThreadPoll already stop, cannot add task any more");
            _que_tasks.emplace([task]() { (*task)(); });
        }
        _cv.notify_one();
        return result;
    }
};
} // namespace WS

#endif