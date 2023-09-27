#include "PDFStreamReader.hpp"
#include <iostream>

namespace
{
bool IsNumber(std::string_view token)
{
  return token.find_first_not_of("0123456789.-") == std::string::npos;
}
}

PDFStreamReader::PDFStreamReader()
{
  m_graphicStates.emplace(); // Need to start with one graphics state on the stack
}

void PDFStreamReader::Read(const PDFStreamFinder::GraphicsStream& data)
{
  m_readPosition = 0;
  m_drawArea = data.m_drawArea; // TODO: Draw area should not be per stream if there are multiple streams
  m_data = data.m_data;

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
      m_currentPath.AddNewSubPath();
      m_currentPath.AddPoint(PopVector2());
    }
    else if (token == "l")
    {
      m_currentPath.AddPoint(PopVector2());
    }
    else if (token == "c")
    {
      Vector2 xy3{ PopVector2() };
      Vector2 xy2{ PopVector2() };
      Vector2 xy1{ PopVector2() };
      m_currentPath.AddBezierCurve(xy1, xy2, xy3);
    }
    else if (token == "h")
    {
      m_currentPath.CloseSubPath();
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
      m_currentPath.SetPathMode(PathMode::Stroke);
      m_paths.emplace_back(std::move(m_currentPath), GetGraphicsState());
      m_currentPath = Path{};
    }
    else if (token == "f")
    {
      m_currentPath.SetPathMode(PathMode::Fill);
      m_paths.emplace_back(std::move(m_currentPath), GetGraphicsState());
      m_currentPath = Path{};
    }
    else if (token == "RG")
    {
      GetGraphicsState().SetStrokeColor(PopVector3());
    }
    else if (token == "rg")
    {
      GetGraphicsState().SetFillColor(PopVector3());
    }
    else if (token == "K")
    {
      GetGraphicsState().SetStrokeColor(CMYKtoRGB(PopVector4()));
    }
    else if (token == "k")
    {
      GetGraphicsState().SetFillColor(CMYKtoRGB(PopVector4()));
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
  for (const auto& [polyline, graphicsState] : m_paths)
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
  while (m_readPosition < m_data.size() && m_data[m_readPosition] != ' ' && m_data[m_readPosition] != '\n' &&
         m_data[m_readPosition] != '\r')
    m_readPosition++;
  if (m_readPosition == m_data.size())
    return {};
  size_t endRead{ m_readPosition };
  while (m_readPosition < m_data.size() &&
         (m_data[m_readPosition] == ' ' || m_data[m_readPosition] == '\n' || m_data[m_readPosition] == '\r'))
    m_readPosition++;
  return std::string_view(m_data.data() + startRead, endRead - startRead);
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
