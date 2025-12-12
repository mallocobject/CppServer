#include "timer_queue.h"
#include "../base/utils.h"
#include "channel.h"
#include "event_loop.h"
#include "time_stamp.h"
#include "timer.h"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <functional>
#include <memory>
#include <sys/time.h>
#include <sys/timerfd.h>
#include <sys/types.h>
#include <unistd.h>

namespace WS
{

TimerQueue::TimerQueue(EventLoop* loop) : _loop(loop)
{
    createTimerFd();
    _ch = std::make_unique<Channel>(_timer_fd, loop);
    _ch->setReadCallback(std::bind(&TimerQueue::handleRead, this));
    _ch->enableRead(false);
}

void TimerQueue::createTimerFd()
{
    _timer_fd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    erro(_timer_fd == -1, "create timerfd failed");
}

TimerQueue::~TimerQueue()
{
    _loop->deleteChannel(_ch.get());
    for (auto& timer : _timers)
    {
	::delete timer.second;
    }
}

void TimerQueue::readTimerFd()
{
    uint64_t read_byte;
    ssize_t read = ::read(_timer_fd, &read_byte, sizeof(read_byte));
    assert(read == sizeof(read_byte));
}

void TimerQueue::handleRead()
{
    readTimerFd();
    _active_timers.clear();
    auto end = _timers.lower_bound(
	entry(TimeStamp::Now(), reinterpret_cast<Timer*>(UINTPTR_MAX)));
    _active_timers.insert(_active_timers.end(), _timers.begin(), end);

    _timers.erase(_timers.begin(), end);

    for (const auto& timer : _active_timers)
    {
	timer.second->run();
    }
    resetTimers();
}

void TimerQueue::resetTimers()
{
    for (auto& timer : _active_timers)
    {
	if (timer.second->isRepeat())
	{
	    auto temp = timer.second;
	    temp->restart(TimeStamp::Now());
	    insert(temp);
	}
	else
	{
	    ::delete timer.second;
	}
    }

    if (!_timers.empty())
    {
	resetTimerFd(_timers.begin()->second);
    }
}

bool TimerQueue::insert(Timer* timer)
{
    bool reset_instantly = false;
    if (_timers.empty() || timer->getExpiration() < _timers.begin()->first)
    {
	reset_instantly = true;
    }
    _timers.emplace(timer->getExpiration(), timer);
    return reset_instantly;
}

void TimerQueue::addTimer(TimeStamp time_stamp, const std::function<void()>& cb,
			  double interval)
{
    Timer* timer = new Timer(time_stamp, cb, interval);
    if (insert(timer))
    {
	resetTimerFd(timer);
    }
}

void TimerQueue::resetTimerFd(Timer* timer)
{
    itimerspec _new, _old;
    memset(&_new, 0, sizeof(_new));
    memset(&_old, 0, sizeof(_old));

    int64_t micro_seconds_diff = timer->getExpiration().getMicroSeconds() -
				 TimeStamp::Now().getMicroSeconds();
    if (micro_seconds_diff < 100)
    {
	micro_seconds_diff = 100;
    }
    _new.it_value.tv_sec = static_cast<time_t>(micro_seconds_diff / 1000000);
    _new.it_value.tv_nsec =
	static_cast<long>((micro_seconds_diff % 1000000) * 1000);
    erro(::timerfd_settime(_timer_fd, 0, &_new, &_old) == -1,
	 "set timefd failed");
}

} // namespace WS