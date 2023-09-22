#pragma once

#include "OpenGL/GlewInitializer.hpp"
#include "OpenGL/Program.hpp"
#include "OpenGL/VertexArray.hpp"
#include "math/Rectangle.hpp"
#include "math/Triangle.hpp"
#include <vector>

namespace gl
{
class Renderer
{
  bool m_ready{ false };
  bool m_initialDraw{ true };

  Vector2i m_windowSize{ 1, 1 };
  bool m_windowSizeChanged{ true };

  Rectangle m_drawArea{ { 0.f, 0.f }, { 100.f, 100.f } };
  bool m_drawAreaChanged{ true };

  std::vector<Triangle> m_triangles;

  unsigned m_fbo{ 0 };
  GlewInitializer m_glewInitializer;
  VertexArray m_vao;
  Program m_program;

  void RecreateFramebuffer();

public:
  Renderer();
  ~Renderer();
  void SetTriangleBuffer(std::vector<Triangle>&& triangles);
  void AddTriangles(const std::vector<Triangle>& triangles);
  void Finish();
  void SetWindowSize(const Vector2i& windowSize);
  void SetDrawArea(const Rectangle& drawArea);
  void Draw();
};
}
