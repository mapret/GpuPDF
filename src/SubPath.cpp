#include "SubPath.hpp"
#include "math/Numbers.hpp"
#include <CDT.h>

void SubPath::AddPoint(const Vector2& point)
{
  m_points.push_back(point);
}

void SubPath::DrawPie(const Vector2& center,
                      float radius,
                      float beginAngle,
                      float angleSize,
                      const Vector3& color,
                      std::vector<Triangle>& out) const
{
  // TODO: Make the number of steps dependent on angleSize
  int numSteps{ 10 };
  float stepsize{ angleSize / numSteps };
  for (int i{ 0 }; i < numSteps; i++)
  {
    float sliceAngleBegin{ beginAngle + stepsize * i };
    float sliceAngleEnd{ beginAngle + stepsize * (i + 1) };
    out.push_back(Triangle{ center,
                            center + radius * Vector2{ std::cos(sliceAngleBegin), std::sin(sliceAngleBegin) },
                            center + radius * Vector2{ std::cos(sliceAngleEnd), std::sin(sliceAngleEnd) },
                            color });
  }
}

void SubPath::Stroke(const GraphicsState& graphicsState, std::vector<Triangle>& trianglesOut) const
{
  //     miterLength = 1 / sin(phi / 2)
  // <=>         phi = asin(1 / miterLength) * 2
  constexpr float miterLimit{ 10 };
  const float minMiterAngle{ std::asin(1.f / miterLimit) * 2.f };
  const float cosMinMiterAngle{ std::cos(minMiterAngle) };

  Vector2 previousDirection;
  Vector2 previousLeft;
  float halfThickness{ graphicsState.GetLineWidth() / 2.f };

  for (int i{ 0 }, count{ static_cast<int>(m_points.size()) - 1 }; i < count; i++)
  {
    const Vector2& p0{ m_points[i] };
    const Vector2& p1{ m_points[i + 1] };

    Vector2 direction{ (p1 - p0).Normalized() };
    Vector2 left{ -direction.y, direction.x };

    Vector2 a{ p0 + left * halfThickness };
    Vector2 b{ p0 - left * halfThickness };
    Vector2 c{ p1 - left * halfThickness };
    Vector2 d{ p1 + left * halfThickness };

    // This cannot be used in the first iteration!
    bool isLeftTurn{ previousDirection.Dot(Vector2{ -direction.y, direction.x }) < 0 };

    if (i == 0)
    {
      if (graphicsState.GetLineCapStyle() == LineCapStyle::Square)
      {
        a -= direction * halfThickness;
        b -= direction * halfThickness;
      }
      else if (graphicsState.GetLineCapStyle() == LineCapStyle::Round)
      {
        DrawPie(
          p0, halfThickness, std::atan2(left.y, left.x), numbers::PI, graphicsState.GetStrokeColor(), trianglesOut);
      }
    }
    if (i == count - 1)
    {
      if (graphicsState.GetLineCapStyle() == LineCapStyle::Square)
      {
        c += direction * halfThickness;
        d += direction * halfThickness;
      }
    }

    //    p1
    // d______c
    //  |    /|
    //  |   / |
    //  |  /  |
    //  | /   |
    // a|/____|b
    //    p0
    trianglesOut.push_back(Triangle{ a, b, c, graphicsState.GetStrokeColor() });
    trianglesOut.push_back(Triangle{ a, c, d, graphicsState.GetStrokeColor() });

    if (i > 0)
    {
      if (graphicsState.GetLineJoinStyle() == LineJoinStyle::Round)
      {
        float startAngleAdd{ isLeftTurn ? numbers::PI : 0.f };
        float angleSizeMul{ isLeftTurn ? -1.f : 1.f };
        DrawPie(p0,
                halfThickness,
                std::atan2(left.y, left.x) + startAngleAdd,
                std::acos(previousDirection.Dot(direction)) * angleSizeMul,
                graphicsState.GetStrokeColor(),
                trianglesOut);
      }
      else if (graphicsState.GetLineJoinStyle() == LineJoinStyle::Bevel ||
               (graphicsState.GetLineJoinStyle() == LineJoinStyle::Miter &&
                previousDirection.Dot(direction) + 1 < cosMinMiterAngle))
      {
        float leftMul{ isLeftTurn ? -1.f : 1.f };
        // TODO: Dont insert in middle
        trianglesOut.insert(trianglesOut.end() - 2,
                            Triangle{ p0 + previousLeft * halfThickness * leftMul,
                                      p0,
                                      p0 + left * halfThickness * leftMul,
                                      graphicsState.GetStrokeColor() });
      }
      else if (graphicsState.GetLineJoinStyle() == LineJoinStyle::Miter)
      {
        float tangent{ 1.f / std::tan(std::acos(previousDirection.Dot(-direction)) / 2.f) };
        Vector2 t1{ tangent * halfThickness * direction };
        Vector2 t2{ tangent * halfThickness * previousDirection };

        // There are t least 4 triangles in trianglesOut the first time this code is reached
        if (isLeftTurn)
        {
          trianglesOut[trianglesOut.size() - 2].b.position -= t1;
          trianglesOut[trianglesOut.size() - 3].b.position += t2;
          trianglesOut[trianglesOut.size() - 4].c.position += t2;
        }
        else
        {
          trianglesOut[trianglesOut.size() - 1].a.position -= t1;
          trianglesOut[trianglesOut.size() - 2].a.position -= t1;
          trianglesOut[trianglesOut.size() - 3].c.position += t2;
        }
      }
    }

    if (i == count - 1)
    {
      if (graphicsState.GetLineCapStyle() == LineCapStyle::Round)
      {
        DrawPie(
          p1, halfThickness, std::atan2(left.y, left.x), -numbers::PI, graphicsState.GetStrokeColor(), trianglesOut);
      }
    }

    previousDirection = direction;
    previousLeft = left;
  }
}

void SubPath::Fill(const GraphicsState& graphicsState, std::vector<Triangle>& trianglesOut) const
{
  if (m_points.size() < 3)
    return;

  using Triangulator = CDT::Triangulation<float>;
  Triangulator::V2dVec tVertices;
  for (const Vector2& v : m_points)
    tVertices.push_back(CDT::V2d<float>::make(v.x, v.y));

  std::vector<CDT::Edge> tEdges;
  for (unsigned i{ 0 }, count{ static_cast<unsigned>(m_points.size()) }; i < count; i++)
    tEdges.push_back(CDT::Edge{ i, (i + 1) % count });

  CDT::RemoveDuplicatesAndRemapEdges(tVertices, tEdges);

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
