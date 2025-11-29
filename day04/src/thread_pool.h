#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

#include <condition_variable>
#include <cstddef>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
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
    bool _stop;

  public:
    ThreadPool(size_t num_ths = std::thread::hardware_concurrency());
    ~ThreadPool();
    void addTask(std::function<void()> task);
};
} // namespace WS

#endif