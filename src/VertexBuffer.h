#pragma once
#include <GL/glew.h>

class VertexBuffer {
public:
  enum class Type {
    Array,
    Element
  };

  /// Creates a new data store for a buffer object.
  /// \param type: Specifies the target to which the buffer object is bound.
  explicit VertexBuffer(Type type) : m_type(type) {
    glGenBuffers(1, &m_vbo);
  }

  ~VertexBuffer() {
    glDeleteBuffers(1, &m_vbo);
  }

  /// Sets new data to a buffer object.
  /// \param size: Specifies the size in bytes of the buffer object's new data store.
  /// \param data: Specifies a pointer to data that will be copied into the data store for initialization, or nullptr if no data is to be copied.
  void buffer(size_t size, const void *data) const {
    auto target = getTarget(m_type);
    glBindBuffer(target, m_vbo);
    glBufferData(target, size, data, GL_STATIC_DRAW);
  }

  void bind() const {
    auto target = getTarget(m_type);
    glBindBuffer(target, m_vbo);
  }

  static void unbind(Type type) {
    auto target = getTarget(type);
    glBindBuffer(target, 0);
  }

private:
  static GLenum getTarget(Type type) {
    return type == Type::Array ? GL_ARRAY_BUFFER : GL_ELEMENT_ARRAY_BUFFER;
  }

private:
  Type m_type;
  unsigned int m_vbo{0};
};