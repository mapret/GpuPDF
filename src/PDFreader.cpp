#include "PDFreader.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

bool PDFreader::ReadFile(std::string_view path)
{
  std::ifstream in{ path.data(), std::ios::binary };

  return ReadDocument(in);

  while (in)
  {
    auto line = ReadLine(in);
    if (line[0] == '%')
      continue;
    std::cout << line << "\n";
  }
  return true;
}

bool PDFreader::ReadDocument(std::istream& in)
{
  SkipCommentLines(in);

  while (in)
  {
    // auto line{ReadLine(in)};
    //  std::cout << line << "\n";

    int objectId, asdf;
    std::string objstring;
    in >> objectId >> asdf >> objstring;
    if (objstring == "obj")
    {
      std::cout << "obj " << objectId << "\n";
      std::string_view l;
      do
      {
        l = ReadLine(in);
        // std::cout << l << "\n";
      } while (in && l != "endobj");
    }
  }
  return true;
}

std::string_view PDFreader::ReadLine(std::istream& in)
{

  readbuffer.clear();
  do
  {
    char next = in.get();
    if (next == '\r')
      continue;
    if (next == '\n' || !in)
    {
      if (!readbuffer.empty() && readbuffer[0] == '%')
      {
        readbuffer.clear();
        continue;
      }
      else
      {
        return std::string_view{ readbuffer.data(), readbuffer.size() };
      }
    }
    readbuffer.push_back(next);
  } while (true);
}

void PDFreader::SkipCommentLines(std::istream& in)
{
  while (true)
  {
    if (in.peek() == '%')
    {
      while (true)
      {
        char next = in.get();
        if (next == '\n')
          break;
      }
    }
    else
    {
      break;
    }
  }
}
