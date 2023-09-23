#pragma once

#include "PDFStreamFinder.hpp"
#include "Polyline.hpp"
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

  std::vector<std::pair<Polyline, GraphicsState>> m_polylines;
  std::stack<GraphicsState> m_graphicStates;

public:
  PDFStreamReader(const PDFStreamFinder::GraphicsStream& data);
  void Read();

  std::vector<Triangle> CollectTriangles() const;
  const Rectangle& GetDrawArea() const;
};
