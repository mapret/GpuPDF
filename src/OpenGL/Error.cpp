#include "Error.hpp"
#include <GL/glew.h>
#include <format>
#include <iostream>

namespace gl
{
const char* GetErrorString(GLenum error)
{
  // clang-format off
  switch (error)
  {
    case GL_INVALID_ENUM:                  return "GL_INVALID_ENUM";
    case GL_INVALID_VALUE:                 return "GL_INVALID_VALUE";
    case GL_INVALID_OPERATION:             return "GL_INVALID_OPERATION";
    case GL_STACK_OVERFLOW:                return "GL_STACK_OVERFLOW";
    case GL_STACK_UNDERFLOW:               return "GL_STACK_UNDERFLOW";
    case GL_OUT_OF_MEMORY:                 return "GL_OUT_OF_MEMORY";
    case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
    case GL_CONTEXT_LOST:                  return "GL_CONTEXT_LOST";
    default:                               return "UNKNOWN_ERROR";
  }
  // clang-format on
}

void CheckError()
{
  if (auto error{ glGetError() }; error != GL_NO_ERROR)
  {
    std::cerr << "OpenGL error: " << std::format("{} (0x{:04x})", GetErrorString(error), error) << "\n";
  }
}
} // namespace gl
