#include "DoomFireApplication.h"
#include "Util.h"
#include <GL/glew.h>
#include <SDL.h>
#include <cstring>
#include <imgui.h>
#include <iostream>

const char *vertexShaderSource = "#version 330 core\n"
                                 "uniform mat4 xform;\n"
                                 "layout (location = 0) in vec4 attr_vertex;\n"
                                 "in vec2 uvscale;\n"
                                 "out vec2 uv;\n"
                                 "void main()\n"
                                 "{\n"
                                 "   gl_Position = xform * attr_vertex;\n"
                                 "   uv = (attr_vertex.xy * vec2(0.5, -0.5) + 0.5) * uvscale;\n"
                                 "}\0";
const char *fragmentShaderSource = "#version 330 core\n"
                                   "out vec4 FragColor;\n"
                                   "in vec2 uv;\n"
                                   "uniform sampler2D img_tex;\n"
                                   "uniform sampler1D pal_tex;\n"
                                   "void main()\n"
                                   "{\n"
                                   "  float cidx = texture(img_tex, uv).x;\n"
                                   "  vec3 color = texture(pal_tex, cidx).xyz;\n"
                                   "  FragColor.xyz = color;\n"
                                   "  FragColor.a = 1.0;\n"
                                   "}\n\0";

constexpr float vertices[] = {
    1.0f, 1.0f, 0.0f, 0.0f,  // top right
    1.0f, -1.0f, 0.0f, 0.0f, // bottom right
    -1.0f, -1.0f, 0.0f, 0.0f,// bottom left
    -1.0f, 1.0f, 0.0f, 0.0f  // top left
};

constexpr unsigned int indices[] = {
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

  m_vbo = std::make_unique<VertexBuffer>();
  m_ebo = std::make_unique<VertexBuffer>();

  reset();

  int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
  glCompileShader(vertexShader);
  // check for shader compile errors
  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
              << infoLog << std::endl;
  }
  // fragment shader
  int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
  glCompileShader(fragmentShader);
  // check for shader compile errors
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
              << infoLog << std::endl;
  }
  // link shaders
  m_shaderProgram = glCreateProgram();
  glAttachShader(m_shaderProgram, vertexShader);
  glAttachShader(m_shaderProgram, fragmentShader);
  glLinkProgram(m_shaderProgram);
  // check for linking errors
  glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(m_shaderProgram, 512, nullptr, infoLog);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
              << infoLog << std::endl;
  }
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  m_vao = std::make_unique<VertexArray>();

  // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
  m_vao->bind();
  m_vbo->buffer(VertexBuffer::Type::Array, sizeof(vertices), vertices);
  m_ebo->buffer(VertexBuffer::Type::Element, sizeof(indices), indices);

  // position attribute
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
  glEnableVertexAttribArray(0);

  auto tex_xsz = Util::nextPow2(FIRE_WIDTH);
  auto tex_ysz = Util::nextPow2(FIRE_HEIGHT);

  glGenTextures(1, &m_img_tex);
  glBindTexture(GL_TEXTURE_2D, m_img_tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, tex_xsz, tex_ysz, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);

  glGenTextures(1, &m_pal_tex);
  glBindTexture(GL_TEXTURE_1D, m_pal_tex);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, palette);

  glUseProgram(m_shaderProgram);
  glUniform1i(glGetUniformLocation(m_shaderProgram, "img_tex"), 0);
  glUniform1i(glGetUniformLocation(m_shaderProgram, "pal_tex"), 1);
  glVertexAttrib2f(glGetAttribLocation(m_shaderProgram, "uvscale"), (float) FIRE_WIDTH / (float) tex_xsz,
                   (float) FIRE_HEIGHT / (float) tex_ysz);
}

int width = 1280;
int height = 720;
float amountX = 0.f;
float amountY = 0.f;

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

  // bind textures on corresponding texture units
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_img_tex);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_1D, m_pal_tex);

  // draw our first triangle
  glUseProgram(m_shaderProgram);
  m_vao->bind();
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

  Application::onRender();
}

void DoomFireApplication::onUpdate(const TimeSpan &elapsed) {
  if (amountX != 0) {
    width += amountX;
    if (width < 10)
      width = 10;
    if (width > 1280 * 2)
      width = 1280 * 2;
    reshape(width, height);
  }

  if (amountY != 0) {
    height += amountY;
    if (height < 10)
      height = 10;
    if (height > 720 * 2)
      height = 720 * 2;
    reshape(width, height);
  }

  // Update palette buffer
  doFire();

  glBindTexture(GL_TEXTURE_2D, m_img_tex);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, FIRE_WIDTH, FIRE_HEIGHT, GL_RED, GL_UNSIGNED_BYTE, m_image.data());
}

void DoomFireApplication::reshape(int x, int y) const {
  int loc;
  float aspect = (float) x / (float) y;
  float fbaspect = (float) FIRE_WIDTH / (float) FIRE_HEIGHT;
  float xform[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};

  glViewport(0, 0, x, y);

  if (aspect > fbaspect) {
    xform[0] = fbaspect / aspect;
  } else if (fbaspect > aspect) {
    xform[5] = aspect / fbaspect;
  }

  glUseProgram(m_shaderProgram);
  if ((loc = glGetUniformLocation(m_shaderProgram, "xform")) >= 0) {
    glUniformMatrix4fv(loc, 1, GL_FALSE, xform);
  }

  auto err = glGetError();
  assert(err == GL_NO_ERROR);
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
