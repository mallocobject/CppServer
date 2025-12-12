#ifndef __TIME_STAMP_H__
#define __TIME_STAMP_H__

#include <bits/types/struct_timeval.h>
#include <cstdint>
#include <string>
#include <sys/time.h>
namespace WS
{

class TimeStamp
{
  protected:
    int64_t _micro_seconds;

  public:
    TimeStamp();
    explicit TimeStamp(int64_t micro_seconds);
    bool operator<(const TimeStamp& rhs) const;
    bool operator==(const TimeStamp& rhs) const;

    std::string toFormattedString() const;
    int64_t getMicroSeconds() const;

    static TimeStamp Now();

    static TimeStamp addTime(TimeStamp timestamp, double add_seconds);
};
inline TimeStamp TimeStamp::Now()
{
    timeval time;
    ::gettimeofday(&time, nullptr);
    int64_t micros = static_cast<int64_t>(time.tv_sec) * 1000000 +
		     static_cast<int64_t>(time.tv_usec);
    return TimeStamp(micros);
}

inline TimeStamp TimeStamp::addTime(TimeStamp timestamp, double add_seconds)
{
    int64_t add_micros = static_cast<int64_t>(add_seconds) * 1000000;
    return TimeStamp(timestamp._micro_seconds + add_micros);
}

} // namespace WS

#endif