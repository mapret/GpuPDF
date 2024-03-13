#include "Path.hpp"
#include <CDT.h>

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

int Path::GetApproximateTriangleCount() const
{
  int approximateTriangleCount{ 0 };
  // This approximation should be good enough for now
  for (const auto& subPath : m_subPaths)
    approximateTriangleCount += static_cast<int>(subPath.GetPoints().size());
  return approximateTriangleCount;
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
    using Triangulator = CDT::Triangulation<float>;
    Triangulator::V2dVec tVertices;
    std::vector<CDT::Edge> tEdges;

    for (const SubPath& subPath : m_subPaths)
    {
      if (subPath.IsEmpty())
        continue;

      unsigned vertexOffset{ static_cast<unsigned>(tVertices.size()) };
      for (const Vector2& v : subPath.GetPoints())
        tVertices.push_back(CDT::V2d<float>{ v.x, v.y });
      for (unsigned i{ 0 }, count{ static_cast<unsigned>(subPath.GetPoints().size()) - 1 }; i < count; i++)
        tEdges.emplace_back(i + vertexOffset, i + 1 + vertexOffset);
    }

    CDT::RemoveDuplicatesAndRemapEdges(tVertices, tEdges);

    if (tEdges.size() < 3)
      return;

    // TODO: Investigate asserts "vv[0] == iVedge2 ||..." from debug build
    Triangulator triangulator;
    triangulator.insertVertices(tVertices);
    triangulator.insertEdges(tEdges);
    triangulator.eraseOuterTrianglesAndHoles();

    auto convert{ [&](unsigned index) { return Vector2{ tVertices[index].x, tVertices[index].y }; } };
    for (const auto& tTriangle : triangulator.triangles)
    {
      const Vector2& p0{ convert(tTriangle.vertices[0]) };
      const Vector2& p1{ convert(tTriangle.vertices[1]) };
      const Vector2& p2{ convert(tTriangle.vertices[2]) };
      trianglesOut.push_back(Triangle{ p0, p1, p2, graphicsState.GetFillColor() });
    }
  }

  for (size_t i{ startOffset }; i < trianglesOut.size(); i++)
  {
    auto& triangle{ trianglesOut[i] };
    triangle.a.position = graphicsState.Transform(triangle.a.position);
    triangle.b.position = graphicsState.Transform(triangle.b.position);
    triangle.c.position = graphicsState.Transform(triangle.c.position);
  }
}
