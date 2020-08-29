#ifndef COLORCYCLING__TIMESPAN_H
#define COLORCYCLING__TIMESPAN_H

#include <cmath>
#include <limits>
#include <stdexcept>

struct TimeSpan
{
private:
  static const long TicksPerMillisecond = 10000;
  static constexpr float MillisecondsPerTick = 1.0 / TicksPerMillisecond;

  static const long TicksPerSecond = TicksPerMillisecond * 1000;
  static constexpr float SecondsPerTick = 1.0 / TicksPerSecond;

  static const long TicksPerMinute = TicksPerSecond * 60;
  static constexpr float MinutesPerTick = 1.0 / TicksPerMinute;

  static const long MaxSeconds =
      std::numeric_limits<long>::max() / TicksPerSecond;
  static const long MinSeconds =
      std::numeric_limits<long>::min() / TicksPerSecond;

  static const long MaxMilliSeconds =
      std::numeric_limits<long>::max() / TicksPerMillisecond;
  static const long MinMilliSeconds =
      std::numeric_limits<long>::min() / TicksPerMillisecond;

  static const int MillisPerSecond = 1000;
  static const int MillisPerMinute = MillisPerSecond * 60;

public:
  TimeSpan() noexcept = default;
  explicit TimeSpan(long ticks) noexcept : m_ticks(ticks)
  {
  }
  TimeSpan(int hour, int minute, int second)
      : m_ticks(toTicks(hour, minute, second))
  {
  }

  static TimeSpan seconds(float value)
  {
    return interval(value, MillisPerSecond);
  }

  static TimeSpan milliseconds(float value)
  {
    return interval(value, 1);
  }

  static TimeSpan minutes(float value)
  {
    return interval(value, MillisPerMinute);
  }

  [[nodiscard]] long getTicks() const
  {
    return m_ticks;
  }
  [[nodiscard]] int getMilliseconds() const
  {
    return (int)((m_ticks / TicksPerMillisecond) % 1000);
  }
  [[nodiscard]] int getMinutes() const
  {
    return (int)((m_ticks / TicksPerMinute) % 60);
  }
  [[nodiscard]] int getSeconds() const
  {
    return (int)((m_ticks / TicksPerSecond) % 60);
  }
  [[nodiscard]] float getTotalMilliseconds() const
  {
    float temp = (float)m_ticks * MillisecondsPerTick;
    if(temp > MaxMilliSeconds) return (float)MaxMilliSeconds;

    if(temp < MinMilliSeconds) return (float)MinMilliSeconds;

    return temp;
  }

  [[nodiscard]] float getTotalMinutes() const
  {
    return (float)m_ticks * MinutesPerTick;
  }

  [[nodiscard]] float getTotalSeconds() const
  {
    return (float)m_ticks * SecondsPerTick;
  }

private:
  static long toTicks(int hour, int minute, int second)
  {
    long totalSeconds =
        (long)hour * 3600 + (long)minute * 60 + (long)second;
    if(totalSeconds > MaxSeconds || totalSeconds < MinSeconds)
      throw std::invalid_argument("totalSeconds is out of the bounds");
    return totalSeconds * TicksPerSecond;
  }

private:
  static TimeSpan interval(float value, int scale)
  {
    if(std::isnan(value))
      throw std::invalid_argument("value cannot be nan (not a number)");
    float tmp = value * scale;
    float millis = tmp + (value >= 0 ? 0.5 : -0.5);
    if((millis > std::numeric_limits<long>::max() / TicksPerMillisecond) ||
        (millis < std::numeric_limits<long>::min() / TicksPerMillisecond))
      throw std::overflow_error("Specifed time is tool long");
    return TimeSpan{(long)millis * TicksPerMillisecond};
  }

public:
  static const TimeSpan Zero;

private:
  long m_ticks{0};
};

bool operator==(const TimeSpan& left, const TimeSpan& right);
bool operator!=(const TimeSpan& left, const TimeSpan& right);
bool operator<(const TimeSpan& left, const TimeSpan& right);
bool operator>(const TimeSpan& left, const TimeSpan& right);
bool operator<=(const TimeSpan& left, const TimeSpan& right);
bool operator>=(const TimeSpan& left, const TimeSpan& right);
TimeSpan operator+(const TimeSpan& left, const TimeSpan& right);
TimeSpan& operator+=(TimeSpan& left, const TimeSpan& right);
TimeSpan operator-(const TimeSpan& left, const TimeSpan& right);
TimeSpan& operator-=(TimeSpan& left, const TimeSpan& right);

#endif//COLORCYCLING__TIMESPAN_H
