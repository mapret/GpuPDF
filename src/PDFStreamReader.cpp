#include "PDFStreamReader.hpp"
#include "Polyline.hpp"
#include <iostream>

namespace
{
bool IsNumber(std::string_view token)
{
  return token.find_first_not_of("0123456789.") == std::string::npos;
}
}

PDFStreamReader::PDFStreamReader(const PDFStreamFinder::GraphicsStream& data)
  : m_data(data.m_data)
  , m_drawArea(data.m_drawArea)
{
}

void PDFStreamReader::Read()
{
  while (true)
  {
    std::string_view token = NextToken();
    if (token.empty())
      break;
    if (IsNumber(token))
    {
      std::string copy{ token };
      m_stack.push(std::stof(copy));
    }
    if (token == "m")
    {
      m_polylines.emplace_back();
      m_polylines.back().AddPoint(PopVector2());
    }
    else if (token == "l")
    {
      m_polylines.back().AddPoint(PopVector2());
    }
    else if (token == "S")
    {
    }
    // std::cout << "token: " << token << ", " << "\n";
  }
}

std::vector<Triangle> PDFStreamReader::CollectTriangles() const
{
  std::vector<Triangle> triangles;
  for (const Polyline& polyline : m_polylines)
    polyline.GetTriangles(triangles);
  // triangles.emplace_back(Triangle{ Vector2{ 0.f, 0.f }, Vector2{ 1.f, 0.f }, Vector2{ 0.f, 1.f } });
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

float PDFStreamReader::PopFloat()
{
  float ret{ m_stack.top() };
  m_stack.pop();
  return ret;
}

Vector2 PDFStreamReader::PopVector2()
{
  float x{ PopFloat() };
  float y{ PopFloat() };
  return Vector2{ y, x };
}
