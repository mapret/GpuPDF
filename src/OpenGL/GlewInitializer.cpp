#include "GlewInitializer.hpp"
#include <gl/glew.h>
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
