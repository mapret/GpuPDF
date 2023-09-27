#include "GraphicsState.hpp"

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
