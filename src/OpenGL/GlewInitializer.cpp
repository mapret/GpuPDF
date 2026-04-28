module;

#include <GL/glew.h>
#include <iostream>

module gpupdf.renderer.opengl:GlewInitializer;

namespace gl
{
class GlewInitializer
{
public:
  GlewInitializer()
  {
    if (glewInit() != GLEW_OK)
    {
      std::cerr << "Failed to initialize GLEW\n";
    }
  }
};
} // namespace gl
