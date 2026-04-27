module;

#include <GL/glew.h>

export module gpupdf.renderer.opengl:VertexArray;

import :Error;

namespace gl
{
class VertexArray
{
  unsigned m_name;

public:
  VertexArray()
  {
    glGenVertexArrays(1, &m_name);

    CheckError();
  }

  ~VertexArray()
  {
    glDeleteVertexArrays(1, &m_name);

    CheckError();
  }

  void Bind() const
  {
    glBindVertexArray(m_name);
  }

  void Unbind() const
  {
    glBindVertexArray(0);
  }
};
} // namespace gl
