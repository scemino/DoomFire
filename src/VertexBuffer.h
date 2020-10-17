#pragma once
#include <GL/glew.h>

class VertexBuffer {
public:
  enum class Type {
    Array,
    Element
  };

  VertexBuffer() {
    glGenBuffers(1, &m_vbo);
  }

  /// Create a new data store for a buffer object.
  /// \param type: Specifies the target to which the buffer object is bound.
  /// \param size: Specifies the size in bytes of the buffer object's new data store.
  /// \param data: Specifies a pointer to data that will be copied into the data store for initialization, or nullptr if no data is to be copied.
  void buffer(Type type, size_t size, const void *data) const {
    auto target = type == Type::Array ? GL_ARRAY_BUFFER : GL_ELEMENT_ARRAY_BUFFER;
    glBindBuffer(target, m_vbo);
    glBufferData(target, size, data, GL_STATIC_DRAW);
  }

  ~VertexBuffer() {
    glDeleteBuffers(1, &m_vbo);
  }

private:
  unsigned int m_vbo{0};
};