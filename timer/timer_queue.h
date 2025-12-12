#ifndef __TIMER_QUEUE_H__
#define __TIMER_QUEUE_H__

#include "common.h"
#include "time_stamp.h"
#include <functional>
#include <memory>
#include <set>
#include <utility>
#include <vector>
namespace WS
{
class Timer;
class EventLoop;
class Channel;
class TimerQueue
{
  public:
    using entry = std::pair<TimeStamp, Timer*>;

  protected:
    EventLoop* _loop;
    int _timer_fd;
    std::unique_ptr<Channel> _ch;
    std::set<entry> _timers;
    std::vector<entry> _active_timers;

  public:
    DISALLOW_DEFAULT(TimerQueue)
    DISALLOW_COPY_AND_MOVE(TimerQueue)

    TimerQueue(EventLoop* loop);
    ~TimerQueue();

    void createTimerFd();
    void readTimerFd();
    void handleRead();
    void resetTimerFd(Timer* timer);
    void resetTimers();
    bool insert(Timer* timer);
    void addTimer(TimeStamp time_stamp, const std::function<void()>& cb,
		  double interval);
};
} // namespace WS

#endif