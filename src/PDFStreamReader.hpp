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
  float PopFloat();
  int PopInt();
  Vector2 PopVector2();
  Vector3 PopVector3();
  Vector4 PopVector4();
  CTM PopCTM();
  static Vector3 CMYKtoRGB(const Vector4& cmyk);

  std::vector<Polyline> m_polylines;
  LineCapStyle m_currentLineCapStyle;
  LineJoinStyle m_currentLineJoinStyle;
  Vector3 m_currentColor;
  float m_currentLineWidth;
  CTM m_currentCTM{ CTM::Identity() };

public:
  PDFStreamReader(const PDFStreamFinder::GraphicsStream& data);
  void Read();

  std::vector<Triangle> CollectTriangles() const;
  const Rectangle& GetDrawArea() const;
};
