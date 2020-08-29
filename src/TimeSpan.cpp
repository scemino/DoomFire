#include "TimeSpan.h"

const TimeSpan TimeSpan::Zero;

bool operator==(const TimeSpan& left, const TimeSpan& right)
{
  return left.getTicks() == right.getTicks();
}

bool operator!=(const TimeSpan& left, const TimeSpan& right)
{
  return left.getTicks() != right.getTicks();
}

bool operator<(const TimeSpan& left, const TimeSpan& right)
{
  return left.getTicks() < right.getTicks();
}

bool operator>(const TimeSpan& left, const TimeSpan& right)
{
  return left.getTicks() > right.getTicks();
}

bool operator<=(const TimeSpan& left, const TimeSpan& right)
{
  return left.getTicks() <= right.getTicks();
}

bool operator>=(const TimeSpan& left, const TimeSpan& right)
{
  return left.getTicks() >= right.getTicks();
}

TimeSpan operator+(const TimeSpan& left, const TimeSpan& right)
{
  long result = left.getTicks() + right.getTicks();
  // Overflow if signs of operands was identical and result's
  // sign was opposite.
  // >> 63 gives the sign bit (either 64 1's or 64 0's).
  if((left.getTicks() >> 63 == right.getTicks() >> 63) &&
      (left.getTicks() >> 63 != result >> 63))
    throw std::overflow_error("TimeSpan overflow");
  return TimeSpan{result};
}

TimeSpan& operator+=(TimeSpan& left, const TimeSpan& right)
{
  return left = left + right;
}

TimeSpan operator-(const TimeSpan& left, const TimeSpan& right)
{
  long result = left.getTicks() - right.getTicks();
  // Overflow if signs of operands was different and result's
  // sign was opposite from the first argument's sign.
  // >> 63 gives the sign bit (either 64 1's or 64 0's).
  if((left.getTicks() >> 63 != right.getTicks() >> 63) &&
      (left.getTicks() >> 63 != result >> 63))
    throw std::overflow_error("TimeSpan overflow");
  return TimeSpan{result};
}

TimeSpan& operator-=(TimeSpan& left, const TimeSpan& right)
{
  return left = left - right;
}
