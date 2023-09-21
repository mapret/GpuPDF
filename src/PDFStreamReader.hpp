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
  Vector2 PopVector2();

  std::vector<Polyline> m_polylines;

public:
  PDFStreamReader(const PDFStreamFinder::GraphicsStream& data);
  void Read();

  std::vector<Triangle> CollectTriangles() const;
  const Rectangle& GetDrawArea() const;
};
