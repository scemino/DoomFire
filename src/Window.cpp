#include "Window.h"
#include <GL/glew.h>
#include <SDL.h>
#include <imgui.h>
#include <imgui/examples/imgui_impl_opengl3.h>
#include <imgui/examples/imgui_impl_sdl.h>
#include <sstream>

Window::Window() = default;

void Window::init() {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
    std::ostringstream ss;
    ss << "Error when initializing SDL (error=" << SDL_GetError() << ")";
    throw std::runtime_error(ss.str());
  }

  // Decide GL+GLSL versions
#if __APPLE__
  // GL 3.2 Core + GLSL 150
  const char *glsl_version = "#version 150";
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS,
                      SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);// Always required on Mac
  SDL_GL_SetAttribute(
      SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
  // GL 3.0 + GLSL 130
  const char *glsl_version = "#version 130";
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
  SDL_GL_SetAttribute(
      SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

  // Create window with graphics context
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  auto window_flags = (SDL_WindowFlags)(
      SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
  m_window = SDL_CreateWindow("SDL/OpenGL Doom Fire", SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);

  // setup OpenGL
  m_glContext = SDL_GL_CreateContext(m_window);
  if (!m_glContext) {
    std::ostringstream ss;
    ss << "Error when creating GL context (error=" << SDL_GetError() << ")";
    throw std::runtime_error(ss.str());
  }

  SDL_GL_MakeCurrent(m_window, m_glContext);
  SDL_GL_SetSwapInterval(1);

  auto err = glGetError();
  if (err != GL_NO_ERROR) {
    std::ostringstream ss;
    ss << "Error when initializing OpenGL " << SDL_GetError();
    throw std::runtime_error(ss.str());
  };

  err = glewInit();
  if (err != GLEW_OK) {
    std::ostringstream ss;
    ss << "Error when initializing glew " << glewGetErrorString(err);
    throw std::runtime_error(ss.str());
  }

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer bindings
  // window is the SDL_Window*
  // contex is the SDL_GLContext
  ImGui_ImplSDL2_InitForOpenGL(m_window, m_glContext);
  ImGui_ImplOpenGL3_Init(glsl_version);
}

void Window::display() {
  SDL_GL_SwapWindow(m_window);
}

bool Window::pollEvent(SDL_Event &event) {
  return SDL_PollEvent(&event);
}

Window::~Window() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  SDL_GL_DeleteContext(m_glContext);
  SDL_DestroyWindow(m_window);
  SDL_Quit();
}
