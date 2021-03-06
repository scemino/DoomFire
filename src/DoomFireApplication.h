#pragma once
#include <array>
#include "Application.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "Texture.h"
#include "Shader.h"
#include "RenderTarget.h"

class DoomFireApplication final : public Application {
protected:
  void onInit() override;
  void onImGuiRender() override;
  void onEvent(SDL_Event &event) override;
  void onRender() override;
  void onUpdate(const TimeSpan &elapsed) override;
  void reset();

private:
  void reshape(int x, int y) const;
  void spreadFire(int src);
  void doFire();

private:
  static constexpr int FIRE_WIDTH = 640;
  static constexpr int FIRE_HEIGHT = 480;
  RenderTarget m_target{};
  std::array<std::uint8_t, FIRE_WIDTH * FIRE_HEIGHT> m_image{};
  std::unique_ptr<Shader> m_shader{};
  std::unique_ptr<VertexArray> m_vao{};
  std::unique_ptr<VertexBuffer> m_vbo{};
  std::unique_ptr<VertexBuffer> m_ebo{};
  std::unique_ptr<Texture> m_img_tex{};
  std::unique_ptr<Texture> m_pal_tex{};
};
