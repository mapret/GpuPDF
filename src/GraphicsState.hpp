#pragma once

#include "math/Matrix.hpp"
#include "math/Vector.hpp"

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

using CTM = Matrix<float, 3, 3>;

class GraphicsState
{
  LineCapStyle m_lineCapStyle;
  LineJoinStyle m_lineJoinStyle;
  Vector3 m_strokeColor;
  Vector3 m_fillColor;
  float m_lineWidth;
  CTM m_transform{ CTM::Identity() };

public:
  void SetLineCapStyle(LineCapStyle lineCapStyle);
  void SetLineJoinStyle(LineJoinStyle lineJoinStyle);
  void SetStrokeColor(const Vector3& strokeColor);
  void SetFillColor(const Vector3& fillColor);
  void SetLineWidth(float lineWidth);
  void SetTransform(const CTM& transform);

  LineCapStyle GetLineCapStyle() const;
  LineJoinStyle GetLineJoinStyle() const;
  const Vector3& GetStrokeColor() const;
  const Vector3& GetFillColor() const;
  float GetLineWidth() const;
  const CTM& GetTransform() const;

  Vector2 Transform(const Vector2& point) const;
};
