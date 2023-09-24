#pragma once

#include "GraphicsState.hpp"
#include "math/Triangle.hpp"
#include "math/Vector.hpp"
#include <vector>

enum class PathMode
{
  Fill,
  Stroke,
};

class Polyline
{
  std::vector<Vector2> m_points;
  PathMode m_pathMode;

  void DrawPie(const Vector2& center,
               float radius,
               float beginAngle,
               float angleSize,
               const Vector3& color,
               std::vector<Triangle>& out) const;
  void StrokePolyline(const GraphicsState& graphicsState, std::vector<Triangle>& trianglesOut) const;
  void FillPolyline(const GraphicsState& graphicsState, std::vector<Triangle>& trianglesOut) const;

public:
  void AddPoint(const Vector2& point);
  void SetPathMode(PathMode pathMode);
  void GetTriangles(const GraphicsState& graphicsState, std::vector<Triangle>& trianglesOut) const;
};
