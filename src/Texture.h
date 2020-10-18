#pragma once
#include <cassert>
#include <GL/glew.h>
#include "Debug.h"

class Texture {
public:
  enum class Format {
    Rgba,
    Rgb,
    Alpha,
  };

  enum class Type {
    Texture1D,
    Texture2D,
  };

  explicit Texture(Type type = Type::Texture2D, Format format = Format::Rgba)
      : m_type(type), m_format{format} {
    GL_CHECK(glGenTextures(1, &m_img_tex));
  }

  Texture(Format format, const int width, const int height, const void *data)
      : m_type(Type::Texture2D), m_format{format} {
    GL_CHECK(glGenTextures(1, &m_img_tex));
    bind();
    auto glFormat = getGlFormat(format);
    glTexImage2D(GL_TEXTURE_2D, 0, glFormat, width, height, 0, glFormat, GL_UNSIGNED_BYTE, data);
    updateFilters();
  }

  Texture(Format format, const int width, const void *data)
      : m_type(Type::Texture1D), m_format{format} {
    GL_CHECK(glGenTextures(1, &m_img_tex));
    bind();
    auto glFormat = getGlFormat(format);
    glTexImage1D(GL_TEXTURE_1D, 0, glFormat, width, 0, glFormat, GL_UNSIGNED_BYTE, data);
    updateFilters();
  }

  ~Texture() {
    GL_CHECK(glDeleteTextures(1, &m_img_tex));
  }

  void setSmooth(bool smooth = true) {
    if (m_smooth == smooth) {
      return;
    }

    m_smooth = smooth;

    if (!m_img_tex)
      return;

    GL_CHECK(glBindTexture(getGlType(m_type), m_img_tex));
    updateFilters();
  }

  [[nodiscard]] bool isSmooth() const noexcept {
    return m_smooth;
  }

  void setData(const int width, const int height, const void *data) const {
    auto type = getGlType(m_type);
    GL_CHECK(glBindTexture(type, m_img_tex));
    if(m_type == Type::Texture2D) {
      GL_CHECK(glTexSubImage2D(type, 0, 0, 0, width, height, getGlFormat(m_format), GL_UNSIGNED_BYTE, data));
    }else {
      GL_CHECK(glTexSubImage1D(type, 0, 0, width, getGlFormat(m_format), GL_UNSIGNED_BYTE, data));
    }
    GL_CHECK(glTexParameteri(type, GL_TEXTURE_MAG_FILTER, m_smooth ? GL_LINEAR : GL_NEAREST));
    GL_CHECK(glTexParameteri(type, GL_TEXTURE_MIN_FILTER, getGlFilter(m_smooth)));
  }

  void bind() const {
    auto type = getGlType(m_type);
    GL_CHECK(glBindTexture(type, m_img_tex));
  }

private:
  void updateFilters(){
    auto type = getGlType(m_type);
    GL_CHECK(glTexParameteri(type, GL_TEXTURE_MAG_FILTER, getGlFilter(m_smooth)));
    GL_CHECK(glTexParameteri(type, GL_TEXTURE_MIN_FILTER, getGlFilter(m_smooth)));
  }

  static GLenum getGlType(Type type) {
    return type == Type::Texture2D ? GL_TEXTURE_2D : GL_TEXTURE_1D;
  }

  static GLenum getGlFilter(bool smooth) {
    return smooth ? GL_LINEAR : GL_NEAREST;
  }

  static GLenum getGlFormat(Format format) {
    switch(format){
    case Format::Alpha: return GL_RED;
    case Format::Rgba: return GL_RGBA;
    case Format::Rgb: return GL_RGB;
    }
    assert(false);
  }

private:
  Type m_type;
  Format m_format;
  bool m_smooth{false};
  unsigned int m_img_tex{0};
};