module;

#include <GL/glew.h>
#include <iostream>

export module gpupdf.renderer.opengl:GlewInitializer;

namespace gl
{
export class GlewInitializer
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
