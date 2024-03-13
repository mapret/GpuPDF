#include "Buffer.hpp"
#include "Error.hpp"
#include <GL/glew.h>

namespace gl
{
Buffer::Buffer()
{
  glGenBuffers(1, &m_name);

  CheckError();
}

Buffer::~Buffer()
{
  glDeleteBuffers(1, &m_name);

  CheckError();
}

void Buffer::Bind() const
{
  glBindBuffer(GL_ARRAY_BUFFER, m_name);
}

void Buffer::Unbind() const
{
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Buffer::SetData(std::ptrdiff_t dataLength, const void* data)
{
  glBufferData(GL_ARRAY_BUFFER, dataLength, data, GL_STATIC_DRAW);
}
} // namespace gl
