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
  void AddPoint(const Vector2& point);
  void AddBezierCurve(const Vector2& p1, const Vector2& p2, const Vector2& p3);
  void ClosePath();

  bool IsEmpty() const;
  const std::vector<Vector2>& GetPoints() const;
};
