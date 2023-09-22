#pragma once

#include "math/Triangle.hpp"
#include "math/Vector.hpp"
#include <vector>

enum class LineCapStyle
{
  Butt = 0,
  Round,
  Square,
};

enum class LineJoinStyle
{
  Miter = 0,
  Round,
  Bevel,
};

class Polyline
{
  std::vector<Vector2> m_points;
  LineJoinStyle m_joinStyle;
  LineCapStyle m_capStyle;
  Vector3 m_color;

  void DrawPie(const Vector2& center,
               float radius,
               float beginAngle,
               float angleSize,
               std::vector<Triangle>& out) const;

public:
  void AddPoint(const Vector2& point);
  void SetJoinStyle(LineJoinStyle joinStyle);
  void SetCapStyle(LineCapStyle capStyle);
  void SetColor(const Vector3& color);
  void GetTriangles(std::vector<Triangle>& trianglesOut) const;
};
