#pragma once

#include "GraphicsState.hpp"
#include "SubPath.hpp"
#include "math/EnumFlagOperators.hpp"
#include "math/Triangle.hpp"
#include "math/Vector.hpp"
#include <vector>

enum class PathMode : unsigned
{
  None = 0,
  Fill = (1 << 0),
  Stroke = (1 << 1),
};
DEFINE_ENUM_FLAGS(PathMode, unsigned)

class Path
{
  std::vector<SubPath> m_subPaths;
  PathMode m_pathMode{ PathMode::None };

public:
  Path();

  void AddPathMode(PathMode pathMode);
  void AddNewSubPath();
  void CloseSubPath();
  void AddPoint(const Vector2& point);
  void AddBezierCurve(const Vector2& p1, const Vector2& p2, const Vector2& p3);
  void AddBezierCurveDuplicateStartPoint(const Vector2& p2, const Vector2& p3);
  int GetApproximateTriangleCount() const;
  void GetTriangles(const GraphicsState& graphicsState, std::vector<Triangle>& trianglesOut) const;
};
