#include "timer.h"
#include "time_stamp.h"
#include <utility>

namespace WS
{
Timer::Timer(TimeStamp time_stamp, const std::function<void()>& cb,
	     double interval)
    : _expiration(time_stamp), _callback(std::move(cb)), _interval(interval)
{
}

TimeStamp Timer::getExpiration() const { return _expiration; }

void Timer::run() const { _callback(); }

void Timer::restart(TimeStamp now)
{
    _expiration = TimeStamp::addTime(now, _interval);
}

bool Timer::isRepeat() const { return _repeat; }
} // namespace WS