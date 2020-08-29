#ifndef COLORCYCLING__WINDOW_H
#define COLORCYCLING__WINDOW_H

#include <SDL.h>

class Window {
public:
  Window();
  ~Window();

  void init();
  void display();
  bool pollEvent(SDL_Event& event);

  SDL_Window* getNativeHandle()
  {
    return m_window;
  }

private:
  SDL_Window *m_window{nullptr};
  SDL_GLContext m_glContext{nullptr};
};

#endif//COLORCYCLING__WINDOW_H
