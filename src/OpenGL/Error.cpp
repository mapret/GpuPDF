#include "Error.hpp"
#include <GL/glew.h>
#include <format>
#include <iostream>

namespace gl
{
void CheckError()
{
  if (auto error{ glGetError() }; error != GL_NO_ERROR)
  {
    std::cerr << "OpenGL Error: " << error << std::format(" (0x{:x})", error) << "\n";
  }
}
}
