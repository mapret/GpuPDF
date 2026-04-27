export module gpupdf.pdf:GraphicsState;

import gpupdf.math;

export enum class LineCapStyle
{
  Butt = 0,
  Round,
  Square,
};

export enum class LineJoinStyle
{
  Miter = 0,
  Round,
  Bevel,
};

export using CTM = Matrix<float, 3, 3>;

export class GraphicsState
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

void GraphicsState::SetLineCapStyle(LineCapStyle lineCapStyle)
{
  m_lineCapStyle = lineCapStyle;
}

void GraphicsState::SetLineJoinStyle(LineJoinStyle lineJoinStyle)
{
  m_lineJoinStyle = lineJoinStyle;
}

void GraphicsState::SetStrokeColor(const Vector3& strokeColor)
{
  m_strokeColor = strokeColor;
}

void GraphicsState::SetFillColor(const Vector3& fillColor)
{
  m_fillColor = fillColor;
}

void GraphicsState::SetLineWidth(float lineWidth)
{
  m_lineWidth = lineWidth;
}

void GraphicsState::SetTransform(const CTM& transform)
{
  m_transform = m_transform * transform;
}

LineCapStyle GraphicsState::GetLineCapStyle() const
{
  return m_lineCapStyle;
}

LineJoinStyle GraphicsState::GetLineJoinStyle() const
{
  return m_lineJoinStyle;
}

const Vector3& GraphicsState::GetStrokeColor() const
{
  return m_strokeColor;
}

const Vector3& GraphicsState::GetFillColor() const
{
  return m_fillColor;
}

float GraphicsState::GetLineWidth() const
{
  return m_lineWidth;
}

const CTM& GraphicsState::GetTransform() const
{
  return m_transform;
}

Vector2 GraphicsState::Transform(const Vector2& point) const
{
  Vector3 result{ m_transform * Vector3{ point.x, point.y, 1.f } };
  return { result.x / result.z, result.y / result.z };
}
