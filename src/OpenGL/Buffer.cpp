module;

#include <GL/glew.h>
#include <cstddef>

module gpupdf.renderer.opengl:Buffer;

import :Error;

namespace gl
{
class Buffer
{
  unsigned m_name;

public:
  Buffer()
  {
    glGenBuffers(1, &m_name);

    CheckError();
  }

  ~Buffer()
  {
    glDeleteBuffers(1, &m_name);

    CheckError();
  }

  void Bind() const
  {
    glBindBuffer(GL_ARRAY_BUFFER, m_name);
  }

  void Unbind() const
  {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }

  void SetData(std::ptrdiff_t dataLength, const void* data)
  {
    glBufferData(GL_ARRAY_BUFFER, dataLength, data, GL_STATIC_DRAW);
  }
};
} // namespace gl
