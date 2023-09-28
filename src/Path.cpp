#include "Path.hpp"

Path::Path()
{
  m_subPaths.emplace_back();
}

void Path::AddPathMode(PathMode pathMode)
{
  m_pathMode |= pathMode;
}

void Path::AddNewSubPath()
{
  m_subPaths.emplace_back();
}

void Path::CloseSubPath()
{
  m_subPaths.back().ClosePath();
}

void Path::AddPoint(const Vector2& point)
{
  m_subPaths.back().AddPoint(point);
}

void Path::AddBezierCurve(const Vector2& p1, const Vector2& p2, const Vector2& p3)
{
  m_subPaths.back().AddBezierCurve(p1, p2, p3);
}

void Path::GetTriangles(const GraphicsState& graphicsState, std::vector<Triangle>& trianglesOut) const
{
  size_t startOffset{ trianglesOut.size() };

  if (EnumFlagSet(m_pathMode, PathMode::Stroke))
  {
    for (const SubPath& subPath : m_subPaths)
      subPath.Stroke(graphicsState, trianglesOut);
  }
  if (EnumFlagSet(m_pathMode, PathMode::Fill))
  {
    for (const SubPath& subPath : m_subPaths)
      subPath.Fill(graphicsState, trianglesOut);
  }

  for (size_t i{ startOffset }; i < trianglesOut.size(); i++)
  {
    auto& triangle{ trianglesOut[i] };
    triangle.a.position = graphicsState.Transform(triangle.a.position);
    triangle.b.position = graphicsState.Transform(triangle.b.position);
    triangle.c.position = graphicsState.Transform(triangle.c.position);
  }
}
