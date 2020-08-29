#ifndef COLORCYCLING__COLORCYCLINGAPPLICATION_H
#define COLORCYCLING__COLORCYCLINGAPPLICATION_H

#include <array>
#include "Application.h"

class DoomFireApplication final : public Application {
public:
  explicit DoomFireApplication();
  ~DoomFireApplication() override;

protected:
  void onInit() override;
  void onImGuiRender() override;
  void onEvent(SDL_Event& event) override;
  void onRender() override;
  void onUpdate(const TimeSpan& elapsed) override;
  void reset();

private:
  void reshape(int x, int y) const;
  void spreadFire(int src);
  void doFire();

private:
  static const int FIRE_WIDTH =  640;
  static const int FIRE_HEIGHT = 480;
  std::array<std::uint8_t, FIRE_WIDTH*FIRE_HEIGHT> m_image{};
  int m_shaderProgram{0};
  unsigned int m_vao{0};
  unsigned int m_vbo{0}, m_ebo{0};
  unsigned int m_img_tex{0}, m_pal_tex{0};
};

#endif//COLORCYCLING__COLORCYCLINGAPPLICATION_H
