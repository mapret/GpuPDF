#pragma once

#include "math/Rectangle.hpp"
#include "math/Triangle.hpp"
#include <vector>

class GLRenderer
{
  bool m_ready{ false };
  bool m_initialDraw{ true };

  Vector2i m_windowSize{ 1, 1 };
  bool m_windowSizeChanged{ true };

  Rectangle m_drawArea{ { 0.f, 0.f }, { 100.f, 100.f } };
  bool m_drawAreaChanged{ true };

  std::vector<Triangle> m_triangles;

  unsigned m_vao;
  unsigned m_program;

public:
  GLRenderer();
  ~GLRenderer();
  void SetTriangleBuffer(std::vector<Triangle>&& triangles);
  void SetWindowSize(const Vector2i& windowSize);
  void SetDrawArea(const Rectangle& drawArea);
  void Draw();
};
