#include "PDFStreamReader.hpp"
#include "Polyline.hpp"
#include <iostream>

namespace
{
bool IsNumber(std::string_view token)
{
  return token.find_first_not_of("0123456789.-") == std::string::npos;
}
}

PDFStreamReader::PDFStreamReader(const PDFStreamFinder::GraphicsStream& data)
  : m_data(data.m_data)
  , m_drawArea(data.m_drawArea)
{
  m_graphicStates.emplace(); // Need to start with one graphics state on the stack
}

void PDFStreamReader::Read()
{
  while (true)
  {
    std::string_view token = NextToken();
    if (token.empty())
      break;
    // std::cout << "token: " << token << "\n";
    if (IsNumber(token))
    {
      std::string copy{ token };
      m_stack.push(std::stof(copy));
    }
    if (token == "q")
    {
      if (m_graphicStates.empty())
      {
        m_graphicStates.emplace();
      }
      else
      {
        GraphicsState copy{ m_graphicStates.top() };
        m_graphicStates.push(copy);
      }
    }
    else if (token == "Q")
    {
      m_graphicStates.pop();
      if (m_graphicStates.empty()) // TODO: Not sure why this happens
        m_graphicStates.emplace();
    }
    else if (token == "m")
    {
      m_polylines.emplace_back();
      m_polylines.back().second = GetGraphicsState();
      m_polylines.back().first.AddPoint(PopVector2());
    }
    else if (token == "l")
    {
      if (!m_polylines.empty()) // TODO: Should the last point from the previous stream be used?
        m_polylines.back().first.AddPoint(PopVector2());
    }
    else if (token == "j")
    {
      GetGraphicsState().SetLineJoinStyle(static_cast<LineJoinStyle>(PopInt()));
    }
    else if (token == "J")
    {
      GetGraphicsState().SetLineCapStyle(static_cast<LineCapStyle>(PopInt()));
    }
    else if (token == "S")
    {
    }
    else if (token == "RG")
    {
      GetGraphicsState().SetStrokeColor(PopVector3());
    }
    else if (token == "K")
    {
      GetGraphicsState().SetStrokeColor(CMYKtoRGB(PopVector4()));
    }
    else if (token == "w")
    {
      GetGraphicsState().SetLineWidth(PopFloat());
    }
    else if (token == "cm")
    {
      // TODO: Append to existing matrix?
      GetGraphicsState().SetTransform(PopCTM());
    }
  }
}

std::vector<Triangle> PDFStreamReader::CollectTriangles() const
{
  std::vector<Triangle> triangles;
  for (const auto& [polyline, graphicsState] : m_polylines)
    polyline.GetTriangles(graphicsState, triangles);
  return triangles;
}

const Rectangle& PDFStreamReader::GetDrawArea() const
{
  return m_drawArea;
}

std::string_view PDFStreamReader::NextToken()
{
  size_t startRead{ m_readPosition };
  while (m_readPosition < m_data.size() && m_data[m_readPosition] != ' ' && m_data[m_readPosition] != '\n')
    m_readPosition++;
  if (m_readPosition == m_data.size())
    return {};
  m_readPosition++;
  return std::string_view(m_data.data() + startRead, m_readPosition - startRead - 1);
}

GraphicsState& PDFStreamReader::GetGraphicsState()
{
  return m_graphicStates.top();
}

float PDFStreamReader::PopFloat()
{
  float ret{ m_stack.top() };
  m_stack.pop();
  return ret;
}

int PDFStreamReader::PopInt()
{
  return static_cast<int>(PopFloat());
}

Vector2 PDFStreamReader::PopVector2()
{
  float x{ PopFloat() };
  float y{ PopFloat() };
  return Vector2{ y, x };
}

Vector3 PDFStreamReader::PopVector3()
{
  float x{ PopFloat() };
  float y{ PopFloat() };
  float z{ PopFloat() };
  return Vector3{ z, y, x };
}

Vector4 PDFStreamReader::PopVector4()
{
  float x{ PopFloat() };
  float y{ PopFloat() };
  float z{ PopFloat() };
  float w{ PopFloat() };
  return Vector4{ w, z, y, x };
}

CTM PDFStreamReader::PopCTM()
{
  float f{ PopFloat() };
  float e{ PopFloat() };
  float d{ PopFloat() };
  float c{ PopFloat() };
  float b{ PopFloat() };
  float a{ PopFloat() };
  return CTM{ a, c, e, b, d, f, 0.f, 0.f, 1.f };
}

Vector3 PDFStreamReader::CMYKtoRGB(const Vector4& cmyk)
{
  return { (1.f - cmyk.x) * (1.f - cmyk.w), (1.f - cmyk.y) * (1.f - cmyk.w), (1.f - cmyk.z) * (1.f - cmyk.w) };
}
