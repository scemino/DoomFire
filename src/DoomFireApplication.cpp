#include "DoomFireApplication.h"
#include "Util.h"
#include <GL/glew.h>
#include <SDL.h>
#include <cstring>
#include <imgui.h>
#include <iostream>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

static const char *vertexShaderSource =
    R"(#version 330 core
uniform mat4 xform;
layout (location = 0) in vec4 attr_vertex;
out vec2 uv;
void main()
{
   gl_Position = xform * attr_vertex;
   uv = (attr_vertex.xy * vec2(0.5, -0.5) + 0.5);
})";

static const char *fragmentShaderSource =
    R"(#version 330 core
out vec4 FragColor;
in vec2 uv;
uniform sampler2D img_tex;
uniform sampler1D pal_tex;
void main()
{
  float cidx = texture(img_tex, uv).x;
  vec3 color = texture(pal_tex, cidx).xyz;
  FragColor.xyz = color;
  FragColor.a = 1.0;
})";

struct Vertex {
  glm::vec2 pos;
};

enum class VertexAttributeType {
  Byte = GL_BYTE,
  UnsignedByte = GL_UNSIGNED_BYTE,
  Short = GL_SHORT,
  UnsignedShort = GL_UNSIGNED_SHORT,
  Float = GL_FLOAT,
};

struct VertexAttribute {
  const char *name;
  int size;
  VertexAttributeType type;
  bool normalized;
  std::size_t offset;
};

static constexpr Vertex vertices[] = {
    {{1.0f, 1.0f}},   // top right
    {{1.0f, -1.0f}},  // bottom right
    {{-1.0f, -1.0f}}, // bottom left
    {{-1.0f, 1.0f}},   // top left
};

static constexpr unsigned int indices[] = {
    // note that we start from 0!
    0, 1, 3,// first triangle
    1, 2, 3 // second triangle
};

const std::uint8_t palette[] = {
    0x07, 0x07, 0x07,
    0x1F, 0x07, 0x07,
    0x2F, 0x0F, 0x07,
    0x47, 0x0F, 0x07,
    0x57, 0x17, 0x07,
    0x67, 0x1F, 0x07,
    0x77, 0x1F, 0x07,
    0x8F, 0x27, 0x07,
    0x9F, 0x2F, 0x07,
    0xAF, 0x3F, 0x07,
    0xBF, 0x47, 0x07,
    0xC7, 0x47, 0x07,
    0xDF, 0x4F, 0x07,
    0xDF, 0x57, 0x07,
    0xDF, 0x57, 0x07,
    0xD7, 0x5F, 0x07,
    0xD7, 0x5F, 0x07,
    0xD7, 0x67, 0x0F,
    0xCF, 0x6F, 0x0F,
    0xCF, 0x77, 0x0F,
    0xCF, 0x7F, 0x0F,
    0xCF, 0x87, 0x17,
    0xC7, 0x87, 0x17,
    0xC7, 0x8F, 0x17,
    0xC7, 0x97, 0x1F,
    0xBF, 0x9F, 0x1F,
    0xBF, 0x9F, 0x1F,
    0xBF, 0xA7, 0x27,
    0xBF, 0xA7, 0x27,
    0xBF, 0xAF, 0x2F,
    0xB7, 0xAF, 0x2F,
    0xB7, 0xB7, 0x2F,
    0xB7, 0xB7, 0x37,
    0xCF, 0xCF, 0x6F,
    0xDF, 0xDF, 0x9F,
    0xEF, 0xEF, 0xC7,
    0xFF, 0xFF, 0xFF};

static int
drawPalette(const std::uint8_t *pal, int numColors = 256, int numColorsByRow = 13, const ImVec2 &size = ImVec2(12, 12),
            const ImVec2 &spacing = ImVec2(2, 2)) {
  auto pos = ImGui::GetCursorScreenPos();
  const auto begPos = pos;
  auto drawList = ImGui::GetWindowDrawList();
  auto numRows = numColors / numColorsByRow;
  if (numColors % numColorsByRow != 0)
    numRows++;
  auto num = 0;
  int index = -1;
  for (auto j = 0; j < numRows; ++j) {
    for (auto i = 0; i < numColorsByRow; ++i) {
      if (num == numColors)
        break;
      num++;
      auto color = ImColor(static_cast<int>(pal[0]), static_cast<int>(pal[1]), static_cast<int>(pal[2]), 255);
      pal += 3;
      auto max = ImVec2(pos.x + size.x, pos.y + size.y);
      drawList->AddRectFilled(pos, max, color);
      drawList->AddRect(pos, max, ImColor(0.8f, 0.8f, 0.8f, 1.0f), 0.0f, ImDrawCornerFlags_All, 0.2f);
      if (ImGui::IsMouseHoveringRect(pos, max)) {
        index = num;
      }
      pos.x += (size.x + spacing.x);
    }
    pos.x = begPos.x;
    pos.y += (size.y + spacing.y);
  }
  ImGui::SetCursorScreenPos({begPos.x, pos.y});
  return index;
}

void DoomFireApplication::reset() {
  // Set whole screen to 0 (color: 0x07,0x07,0x07)
  memset(m_image.data(), 0, m_image.size());

  // Set bottom line to 37 (color white: 0xFF,0xFF,0xFF)
  memset(m_image.data() + (FIRE_HEIGHT - 1) * FIRE_WIDTH, 36, FIRE_WIDTH);
}

void DoomFireApplication::onInit() {
  Application::onInit();

  reset();

  m_vbo = std::make_unique<VertexBuffer>(VertexBuffer::Type::Array);
  m_ebo = std::make_unique<VertexBuffer>(VertexBuffer::Type::Element);
  m_shader = std::make_unique<Shader>(vertexShaderSource, fragmentShaderSource);
  m_vao = std::make_unique<VertexArray>();

  // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
  m_vao->bind();
  m_vbo->buffer(sizeof(vertices), vertices);
  m_ebo->buffer(sizeof(indices), indices);

  VertexAttribute attributes[]{
      "attr_vertex", 2, VertexAttributeType::Float, false, sizeof(Vertex)
  };

  for (auto info : attributes) {
    auto loc = m_shader->getAttributeLocation(info.name);
    if (loc == -1)
      continue;

    GL_CHECK(glEnableVertexAttribArray(loc));
    GL_CHECK(glVertexAttribPointer(loc,
                                   info.size,
                                   static_cast<GLenum>(info.type),
                                   info.normalized ? GL_TRUE : GL_FALSE,
                                   info.offset,
                                   nullptr));
  }

  VertexArray::unbind();
  VertexBuffer::unbind(VertexBuffer::Type::Array);
  VertexBuffer::unbind(VertexBuffer::Type::Element);

  m_img_tex = std::make_unique<Texture>(Texture::Format::Alpha, FIRE_WIDTH, FIRE_HEIGHT, nullptr);
  m_pal_tex = std::make_unique<Texture>(Texture::Format::Rgb, 256, palette);

  m_shader->setUniform("img_tex", *m_img_tex);
  m_shader->setUniform("pal_tex", *m_pal_tex);
}

int width = 1280;
int height = 720;
int amountX = 0;
int amountY = 0;

void DoomFireApplication::onEvent(SDL_Event &event) {
  switch (event.type)
  case SDL_WINDOWEVENT: {
    int w, h;
    SDL_GL_GetDrawableSize(m_window.getNativeHandle(), &w, &h);
    reshape(w, h);
    break;
  case SDL_KEYDOWN:
    if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
      m_done = true;
    }
    break;
  case SDL_CONTROLLERAXISMOTION:
    if (event.jaxis.axis == 0) {
      amountX = static_cast<int>((10.f * event.jaxis.value) / std::numeric_limits<std::int16_t>::max());
    } else if (event.jaxis.axis == 1) {
      amountY = static_cast<int>((10.f * event.jaxis.value) / std::numeric_limits<std::int16_t>::max());
    }
    break;
  case SDL_JOYBUTTONDOWN: {
    auto button = event.jbutton.button;
    if (button == 0) {
      reset();
    }
  }
    break;
  }
}

void DoomFireApplication::onRender() {
  glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  m_vao->bind();
  m_target.draw(PrimitiveType::Triangles, ElementType::UnsignedInt, 6, m_shader.get());
  m_vao->unbind();

  Application::onRender();
}

void DoomFireApplication::onUpdate(const TimeSpan &elapsed) {
  if (amountX != 0) {
    width += amountX;
    std::clamp(width, 10,  1280 * 2);
    reshape(width, height);
  }

  if (amountY != 0) {
    height += amountY;
    std::clamp(height, 10,  720 * 2);
    reshape(width, height);
  }

  // Update palette buffer
  doFire();

  m_img_tex->setData(FIRE_WIDTH, FIRE_HEIGHT, m_image.data());
}

void DoomFireApplication::reshape(int x, int y) const {
  auto aspect = (float) x / (float) y;
  auto fbaspect = (float) FIRE_WIDTH / (float) FIRE_HEIGHT;

  glViewport(0, 0, x, y);

  glm::vec3 vaspect(1, 1, 1);
  if (aspect > fbaspect) {
    vaspect[0] = fbaspect / aspect;
  } else if (fbaspect > aspect) {
    vaspect[1] = aspect / fbaspect;
  }

  auto xform = glm::scale(glm::mat4(1), vaspect);
  m_shader->setUniform("xform", xform);
}

void DoomFireApplication::onImGuiRender() {
  ImGui::Begin("Info");
  ImGui::Text("%.2f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
  if (ImGui::Button("Reset")) {
    reset();
  }
  drawPalette(palette, 37);
  ImGui::End();
}

void DoomFireApplication::spreadFire(int src) {
  auto pixel = m_image[src];
  if (pixel == 0) {
    m_image[src - FIRE_WIDTH] = 0;
  } else {
    auto randIdx = static_cast<int>(3.0f * static_cast<float>(rand()) / RAND_MAX);
    auto dst = src - randIdx + 1;
    m_image[dst - FIRE_WIDTH] = pixel - (randIdx & 1);
  }
}

void DoomFireApplication::doFire() {
  for (auto x = 0; x < FIRE_WIDTH; x++) {
    for (auto y = 1; y < FIRE_HEIGHT; y++) {
      spreadFire(y * FIRE_WIDTH + x);
    }
  }
}
