#pragma once

#include "GraphicsState.hpp"
#include "math/Triangle.hpp"
#include "math/Vector.hpp"
#include <vector>

class SubPath
{
  std::vector<Vector2> m_points;

  void DrawPie(const Vector2& center,
               float radius,
               float beginAngle,
               float angleSize,
               const Vector3& color,
               std::vector<Triangle>& out) const;

public:
  void Stroke(const GraphicsState& graphicsState, std::vector<Triangle>& trianglesOut) const;
  void Fill(const GraphicsState& graphicsState, std::vector<Triangle>& trianglesOut) const;
  void AddPoint(const Vector2& point);
};
