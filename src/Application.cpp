#include "Application.h"
#include "StopWatch.h"
#include <imgui.h>
#include <imgui/examples/imgui_impl_opengl3.h>
#include <imgui/examples/imgui_impl_sdl.h>
#include <utility>

namespace {
const TimeSpan TimePerFrame = TimeSpan::seconds(1.f / 60.f);
}

Application::Application() = default;

Application::~Application() = default;

void Application::run() {
  onInit();
  processEvents();

  int frames = 0;
  StopWatch fpsStopWatch;
  StopWatch stopWatch;
  auto timeSinceLastUpdate = TimeSpan::Zero;
  // Main loop
  while (!m_done) {
    auto elapsed = stopWatch.restart();
    timeSinceLastUpdate += elapsed;
    while (timeSinceLastUpdate > TimePerFrame) {
      timeSinceLastUpdate -= TimePerFrame;
      processEvents();
      onUpdate(TimePerFrame);
    }

    if (fpsStopWatch.getElapsedTime() >= TimeSpan::seconds(1)) {
      m_fps = static_cast<float>(frames) / fpsStopWatch.getElapsedTime().getTotalSeconds();
      fpsStopWatch.restart();
      frames = 0;
    }

    onRender();
    frames++;
    m_window.display();
  }

  onExit();
}

void Application::processEvents() {
  SDL_Event event;
  while (m_window.pollEvent(event)) {
    ImGui_ImplSDL2_ProcessEvent(&event);
    if (event.type == SDL_QUIT) {
      m_done = true;
    } else {
      if (!ImGui::GetIO().WantTextInput && !ImGui::GetIO().WantCaptureMouse) {
        onEvent(event);
      }
    }
  }
}

void Application::onInit() {
  m_window.init();
}

void Application::onExit() {
}

void Application::onUpdate(const TimeSpan &) {
}

void Application::onRender() {
  // Render dear imgui
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame(m_window.getNativeHandle());
  ImGui::NewFrame();

  onImGuiRender();

  // imgui render
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  m_frames++;
}

void Application::onImGuiRender() {
}

void Application::onEvent(SDL_Event &) {
}
