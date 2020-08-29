#ifndef COLORCYCLING__STOPWATCH_H
#define COLORCYCLING__STOPWATCH_H

class StopWatch
{
public:
  StopWatch() : m_startTime(TimeSpan::milliseconds(SDL_GetTicks()))
  {
  }

  [[nodiscard]] TimeSpan getElapsedTime() const
  {
    return TimeSpan::milliseconds(SDL_GetTicks()) - m_startTime;
  }

  TimeSpan restart()
  {
    TimeSpan now = TimeSpan::milliseconds(SDL_GetTicks());
    TimeSpan elapsed = now - m_startTime;
    m_startTime = now;
    return elapsed;
  }

private:
  TimeSpan m_startTime;
};

#endif//COLORCYCLING__STOPWATCH_H
