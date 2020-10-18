#pragma once
#include <GL/glew.h>

class VertexArray {
public:
  VertexArray() {
    glGenVertexArrays(1, &m_vao);
  }

  void bind() const {
    glBindVertexArray(m_vao);
  }

  ~VertexArray() {
    glDeleteBuffers(1, &m_vao);
  }

  static void unbind() {
    glBindVertexArray(0);
  }

private:
  unsigned int m_vao{0};
};
