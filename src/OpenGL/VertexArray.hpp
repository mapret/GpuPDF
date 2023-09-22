#pragma once

namespace gl
{
class VertexArray
{
  unsigned m_name;

public:
  VertexArray();
  ~VertexArray();
  VertexArray(const VertexArray&) = delete;
  VertexArray& operator=(const VertexArray&) = delete;

  void Bind() const;
  void Unbind() const;
};
}
