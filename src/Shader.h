#pragma once
#include <cassert>
#include <map>
#include <vector>
#include <GL/glew.h>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Debug.h"
#include "Texture.h"

class Shader {
public:
  enum Type {
    Vertex,
    Fragment,
  };

  Shader(const char *vertexShader, const char *fragmentShader) {
    if (vertexShader == nullptr && fragmentShader == nullptr) {
      return;
    }

    m_program = compile(vertexShader, fragmentShader);
  }

  ~Shader() {
    if (m_program != 0) {
      GL_CHECK(glDeleteProgram(m_program));
    }
  }

  [[nodiscard]] unsigned int getHandle() const {
    return m_program;
  }

  void setUniform(std::string_view name, int value) const {
    auto loc = getUniformLocation(name);
    GL_CHECK(glUniform1i(loc, value));
  }

  void setAttribute(std::string_view name, const glm::vec2 &value) const {
    auto loc = getAttributeLocation(name);
    GL_CHECK(glVertexAttrib2f(loc, value.x, value.y));
  }

  void setUniform(std::string_view name, const glm::mat4 &value) const {
    auto loc = getUniformLocation(name);
    GL_CHECK(glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value)));
  }

  void setUniform(std::string_view name, const Texture& tex) {
    int loc = getUniformLocation(name);
    if (loc == -1)
      return;
    m_textures[loc] = &tex;
  }

  static void bind(const Shader *shader) {
    if (shader != nullptr && shader->m_program != 0) {
      GL_CHECK(glUseProgram(static_cast<GLuint>(shader->m_program)));

      // bind textures
      GLint index = 0;
      for (auto &item : shader->m_textures) {
        GL_CHECK(glActiveTexture(GL_TEXTURE0 + index));
        GL_CHECK(glUniform1i(item.first, index));
        item.second->bind();
        index++;
      }

    } else {
      GL_CHECK(glUseProgram(0));
    }
  }

private:
  [[nodiscard]] int getUniformLocation(std::string_view name) const {
    GLint loc;
    GL_CHECK(loc = glGetUniformLocation(static_cast<GLuint>(m_program), name.data()));
    return loc;
  }

  [[nodiscard]] int getAttributeLocation(std::string_view name) const {
    GLint loc;
    GL_CHECK(loc = glGetAttribLocation(static_cast<GLuint>(m_program), name.data()));
    return loc;
  }

  static GLuint compileShader(Type type, const char *source) {
    auto id = glCreateShader(type == Type::Vertex ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);

    // compile
    glShaderSource(id, 1, &source, nullptr);
    glCompileShader(id);
    GLint compileStatus = GL_FALSE;
    GL_CHECK(glGetShaderiv(id, GL_COMPILE_STATUS, &compileStatus));

    // if compilation failed
    if (compileStatus == GL_FALSE) {
      GLint infoLogLength = 0;
      GL_CHECK(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLogLength));

      assert(infoLogLength > 0);
      std::vector<char> infoLog(infoLogLength);
      GL_CHECK(glGetShaderInfoLog(id, infoLogLength, nullptr, infoLog.data()));

      const auto typeText = type == Type::Vertex ? "Vertex" : "Fragment";
      std::cerr << "Error while compiling " << typeText << "shader:\n" << infoLog.data() << "\n";
    }
    return id;
  }

  static GLuint compile(const char *vertexShaderCode, const char *fragmentShaderCode) {
    assert(vertexShaderCode != nullptr || fragmentShaderCode != nullptr);

    GLuint program;
    GL_CHECK(program = glCreateProgram());

    if (vertexShaderCode != nullptr) {
      auto id = compileShader(Type::Vertex, vertexShaderCode);
      GL_CHECK(glAttachShader(program, id));
      GL_CHECK(glDeleteShader(id)); // the shader is still here because it is attached to the program
    }

    if (fragmentShaderCode != nullptr) {
      auto id = compileShader(Type::Fragment, fragmentShaderCode);
      GL_CHECK(glAttachShader(program, id));
      GL_CHECK(glDeleteShader(id)); // the shader is still here because it is attached to the program
    }

    GL_CHECK(glLinkProgram(program));

    GLint linkStatus = GL_FALSE;
    GL_CHECK(glGetProgramiv(program, GL_LINK_STATUS, &linkStatus));

    if (linkStatus == GL_FALSE) {
      GLint infoLogLength;
      GL_CHECK(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength));

      assert(infoLogLength > 0);
      std::vector<char> infoLog(infoLogLength);
      GL_CHECK(glGetProgramInfoLog(program, infoLogLength, nullptr, infoLog.data()));

      std::cerr << "Error while linking program:\n" << infoLog.data() << "\n";
      throw std::runtime_error("Error while linking program");
    }

    return program;
  }

private:
  unsigned m_program{0};
  std::map<int, const Texture *> m_textures;
};