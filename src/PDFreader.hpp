#pragma once

#include <string>
#include <vector>

class PDFreader
{
public:
  bool ReadFile(std::string_view path);
  bool ReadDocument(std::istream& in);

private:
  std::vector<char> readbuffer;
  std::string_view ReadLine(std::istream& in);
  void SkipCommentLines(std::istream& in);
};
