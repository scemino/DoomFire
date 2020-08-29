#ifndef COLORCYCLING__APPLICATION_H
#define COLORCYCLING__APPLICATION_H

#include "TimeSpan.h"
#include "Window.h"

class Application {
public:
  Application();
  virtual ~Application();

  void run();

protected:
  virtual void onInit();
  virtual void onExit();
  virtual void onUpdate(const TimeSpan& elapsed);
  virtual void onRender();
  virtual void onImGuiRender();
  virtual void onEvent(SDL_Event& event);

private:
  void processEvents();

protected:
  Window m_window;
  bool m_done{false};
  float m_fps{0};
  int m_frames{0};
};

#endif//COLORCYCLING__APPLICATION_H
