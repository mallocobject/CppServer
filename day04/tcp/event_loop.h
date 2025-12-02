#ifndef __EVENT_LOOP_H__
#define __EVENT_LOOP_H__

#include "../base/common.h"
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <memory>
#include <mutex>
#include <sched.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>
#include <utility>
#include <vector>
namespace WS
{

class Epoller;
class Channel;
class EventLoop
{
  protected:
    std::unique_ptr<Epoller> _ep;
    std::vector<std::function<void()>> _todo_list;
    std::mutex _mtx;
    int _wake_fd;
    std::unique_ptr<Channel> _wake_ch;
    bool _calling_func;
    std::thread::id _tid;

  public:
    DISALLOW_COPY_AND_MOVE(EventLoop)

    EventLoop();
    ~EventLoop();
    void loop();
    void updateChannel(Channel *ch);
    void deleteChannel(Channel *ch);

    void doTodoList()
    {
        _calling_func = true;
        std::vector<std::function<void()>> funcs;
        {
            std::lock_guard<std::mutex> lock(_mtx);
            funcs.swap(_todo_list);
        }
        for (auto &func : funcs)
        {
            func();
        }
        _calling_func = false;
    }

    void queueOneFunc(const std::function<void()> &fn)
    {
        {
            std::lock_guard<std::mutex> lock(_mtx);
            _todo_list.emplace_back(std::move(fn));
        }
        if (!isInLoopThread() || _calling_func)
        {
            uint64_t write_one_byte = 1;
            ssize_t bytes_write = ::write(_wake_fd, &write_one_byte, sizeof(write_one_byte));
            assert(bytes_write == sizeof(write_one_byte));
        }
    }

    void runOneFunc(const std::function<void()> &fn)
    {
        if (isInLoopThread())
        {
            fn();
        }
        else
        {
            queueOneFunc(std::move(fn));
        }
    }

    bool isInLoopThread() const
    {
        return std::this_thread::get_id() == _tid;
    }

    void handleRead()
    {
        uint64_t read_one_byte = 1;
        ssize_t bytes_read = ::write(_wake_fd, &read_one_byte, sizeof(read_one_byte));
        assert(bytes_read == sizeof(read_one_byte));
    }
};
} // namespace WS

#endif