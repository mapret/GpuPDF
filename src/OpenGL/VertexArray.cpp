#include "VertexArray.hpp"
#include "Error.hpp"
#include <GL/glew.h>

namespace gl
{
VertexArray::VertexArray()
{
  glGenVertexArrays(1, &m_name);

  CheckError();
}

VertexArray::~VertexArray()
{
  glDeleteVertexArrays(1, &m_name);

  CheckError();
}

void VertexArray::Bind() const
{
  glBindVertexArray(m_name);
}

void VertexArray::Unbind() const
{
  glBindVertexArray(0);
}
}
