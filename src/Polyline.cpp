#include "Polyline.hpp"

namespace
{
std::pair<Triangle, Triangle> GetLineTriangles(const Vector2& a, const Vector2& b)
{
  float thickness = 5;

  Vector2 direction{ (b - a).Normalized() };
  Vector2 left{ -direction.y, direction.x };
  left *= thickness;

  Triangle t1{ a + left, a - left, b - left };
  Triangle t2{ b - left, b + left, a + left };

  return { t1, t2 };
}
}

void Polyline::AddPoint(const Vector2& point)
{
  m_points.push_back(point);
}

void Polyline::GetTriangles(std::vector<Triangle>& trianglesOut) const
{
  for (int i{ 0 }, count{ static_cast<int>(m_points.size()) - 1 }; i < count; i++)
  {
    auto [t1, t2]{ GetLineTriangles(m_points[i], m_points[i + 1]) };
    trianglesOut.push_back(t1);
    trianglesOut.push_back(t2);
  }
}
