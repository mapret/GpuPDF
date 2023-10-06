#pragma once

#include "math/Vector.hpp"

namespace gl
{
class Program
{
  unsigned m_name;

public:
  Program(const char* vertexShader, const char* fragmentShader);
  ~Program();
  Program(const Program&) = delete;
  Program& operator=(const Program&) = delete;

  void Use() const;
  int GetUniformLocation(const char* name) const;
  void SetUniformValue(int location, const Vector4& vector);
  void SetUniformValue(int location, const Matrix3& matrix);
};
}
