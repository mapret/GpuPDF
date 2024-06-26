#pragma once

#include "PDFStreamFinder.hpp"
#include "Path.hpp"
#include "math/Vector.hpp"
#include <stack>
#include <string>

class PDFStreamReader
{
  std::string m_data;
  Rectangle m_drawArea;
  size_t m_readPosition{ 0 };
  std::stack<float> m_stack;

  std::string_view NextToken();
  GraphicsState& GetGraphicsState();
  float PopFloat();
  int PopInt();
  Vector2 PopVector2();
  Vector3 PopVector3();
  Vector4 PopVector4();
  CTM PopCTM();
  static Vector3 CMYKtoRGB(const Vector4& cmyk);

  Path m_currentPath;
  std::vector<std::pair<Path, GraphicsState>> m_paths;
  std::stack<GraphicsState> m_graphicStates;

public:
  PDFStreamReader();
  void Read(const PDFStreamFinder::GraphicsStream& data);

  std::vector<Triangle> CollectTriangles() const;
  const Rectangle& GetDrawArea() const;
};
