#ifndef __TIMER_H__
#define __TIMER_H__

#include "../base/common.h"
#include "time_stamp.h"
#include <functional>

namespace WS
{
class Timer
{
  protected:
    TimeStamp _expiration;
    std::function<void()> _callback;
    double _interval;
    bool _repeat;

  public:
    DISALLOW_DEFAULT(Timer)
    DISALLOW_COPY_AND_MOVE(Timer)

    Timer(TimeStamp time_stamp, const std::function<void()>& cb,
	  double interval);
    TimeStamp getExpiration() const;
    void run() const;

    void restart(TimeStamp now);

    bool isRepeat() const;
};
} // namespace WS

#endif