#include "time_stamp.h"
#include <cstdint>
#include <string>

namespace WS
{
TimeStamp::TimeStamp() : _micro_seconds(0) {}

TimeStamp::TimeStamp(int64_t micro_seconds) : _micro_seconds(micro_seconds) {}

bool TimeStamp::operator<(const TimeStamp& rhs) const
{
    return _micro_seconds < rhs._micro_seconds;
}

bool TimeStamp::operator==(const TimeStamp& rhs) const
{
    return _micro_seconds == rhs._micro_seconds;
}

std::string TimeStamp::toFormattedString() const
{
    return std::to_string(_micro_seconds);
}

int64_t TimeStamp::getMicroSeconds() const { return _micro_seconds; }

} // namespace WS