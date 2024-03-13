#pragma once

#include "PDFObject.hpp"
#include <filesystem>
#include <unordered_map>

class PDFDocument
{
  std::unordered_map<PDFObject::ID, PDFObject> m_objects;

public:
  bool Load(const std::filesystem::path& path);
  bool Load(std::ifstream& stream);

  const std::unordered_map<PDFObject::ID, PDFObject>& GetObjects() const;
};
