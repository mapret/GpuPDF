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

public:
  void AddPoint(const Vector2& point);
  void SetJoinStyle(LineJoinStyle joinStyle);
  void SetCapStyle(LineCapStyle capStyle);
  void GetTriangles(std::vector<Triangle>& trianglesOut) const;
};
