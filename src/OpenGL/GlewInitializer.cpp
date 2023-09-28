#include "GlewInitializer.hpp"
#include <GL/glew.h>
#include <iostream>

namespace gl
{
GlewInitializer::GlewInitializer()
{
  if (glewInit() != GLEW_OK)
  {
    std::cerr << "Failed to initialize GLEW\n";
  }
}
}
