#pragma once

#include "math/Triangle.hpp"
#include "math/Vector.hpp"
#include <vector>

class Polyline
{
  std::vector<Vector2> m_points;

public:
  void AddPoint(const Vector2& point);
  void GetTriangles(std::vector<Triangle>& trianglesOut) const;
};
