#pragma once

#include "GraphicsState.hpp"
#include "SubPath.hpp"
#include "math/Triangle.hpp"
#include "math/Vector.hpp"
#include <vector>

enum class PathMode
{
  Fill,
  Stroke,
};

class Path
{
  std::vector<SubPath> m_subPaths;
  PathMode m_pathMode;

public:
  Path();

  void SetPathMode(PathMode pathMode);
  void AddNewSubPath();
  void CloseSubPath();
  void AddPoint(const Vector2& point);
  void AddBezierCurve(const Vector2& p1, const Vector2& p2, const Vector2& p3);
  void GetTriangles(const GraphicsState& graphicsState, std::vector<Triangle>& trianglesOut) const;
};
