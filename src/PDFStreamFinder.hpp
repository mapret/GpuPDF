#pragma once

#include "math/Rectangle.hpp"
#include <filesystem>
#include <string>
#include <vector>

class PDFStreamFinder
{
public:
  struct GraphicsStream
  {
    std::string m_data;
    Rectangle m_drawArea;
  };

  std::vector<GraphicsStream> GetGraphicsStreams(const std::filesystem::path& sourceFile) const;
};
