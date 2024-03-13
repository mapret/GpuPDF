#include "PDFDocument.hpp"
#include <fstream>
#include <iostream>

namespace
{
void NotImplemented(const std::string& function)
{
  std::cerr << function << " not implemented\n";
}

bool IsWhitespace(char c)
{
  return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

bool IsDigit(char c)
{
  return c >= '0' && c <= '9';
}

bool IsSpecialChar(char c)
{
  return c == '/' || c == '<' || c == '>' || c == '[' || c == ']';
}

void SkipWhitespace(std::istream& in)
{
  while (true)
  {
    if (IsWhitespace(in.peek()))
      in.get();
    else
      break;
  }
}

PDFObject::Name ReadName(std::istream& in)
{
  PDFObject::Name name;
  while (!IsWhitespace(in.peek()) && !IsSpecialChar(in.peek()))
    name += static_cast<char>(in.get());
  return name;
}

PDFObject ReadObject(std::istream& in)
{
  SkipWhitespace(in);

  char c{ static_cast<char>(in.peek()) };
  if ((c >= '0' && c <= '9') || c == '.' || c == '-')
  {
    std::string s;
    while (!IsWhitespace(in.peek()) && !IsSpecialChar(in.peek()))
      s += static_cast<char>(in.get());
    if (s.find_first_of('.') != std::string::npos)
    {
      PDFObject pdfObject;
      pdfObject.SetDecimal(std::stof(s));
      return pdfObject;
    }

    PDFObject::Integer number{ std::stoll(s) };

    int64_t positionBeforePeek{ in.tellg() };

    if (IsWhitespace(in.peek()))
    {
      SkipWhitespace(in);
      if (IsDigit(in.peek()))
      {
        int potentialGenerationNumber;
        in >> potentialGenerationNumber;
        if (IsWhitespace(in.peek()))
        {
          char potentialReferenceSpecifier;
          in >> potentialReferenceSpecifier;
          if (potentialReferenceSpecifier == 'R')
          {
            PDFObject pdfObject;
            pdfObject.SetReference(static_cast<PDFObject::Reference>(number));
            return pdfObject;
          }
        }
      }
    }

    // TODO: Seek only if tried to read reference and failed
    in.seekg(positionBeforePeek, std::ios::beg);

    PDFObject pdfObject;
    pdfObject.SetInteger(number);
    return pdfObject;
  }
  else if (c == 't')
  {
    for (int i{ 0 }; i < 4; i++)
      in.get();
    PDFObject pdfObject;
    pdfObject.SetBoolean(true);
    return pdfObject;
  }
  else if (c == 'f')
  {
    for (int i{ 0 }; i < 5; i++)
      in.get();
    PDFObject pdfObject;
    pdfObject.SetBoolean(false);
    return pdfObject;
  }
  else if (c == '/')
  {
    in.get();
    PDFObject pdfObject;
    pdfObject.SetName(ReadName(in));
    return pdfObject;
  }
  else if (c == '<')
  {
    in.get();
    if (in.peek() == '<')
    {
      in.get();
      PDFObject pdfObject;
      SkipWhitespace(in);
      while (in.peek() != '>')
      {
        in.get();
        PDFObject::Name dictionaryKey{ ReadName(in) };
        PDFObject dictionaryValue{ ReadObject(in) };
        pdfObject.AddDictionaryEntry(dictionaryKey, dictionaryValue);
      }
      SkipWhitespace(in);
      in.get();
      in.get();
      return pdfObject;
    }
    else
    {
      NotImplemented("Hex-string");
    }
  }
  else if (c == '[')
  {
    in.get();
    PDFObject pdfObject;
    SkipWhitespace(in);
    while (in.peek() != ']')
    {
      pdfObject.AddArrayEntry(ReadObject(in));
    }
    in.get();
    return pdfObject;
  }
  NotImplemented("\"" + std::string(1, c) + "\"");
  return PDFObject{};
}
} // namespace

bool PDFDocument::Load(const std::filesystem::path& path)
{
  std::ifstream in{ path, std::ios::binary };
  return Load(in);
}

bool PDFDocument::Load(std::ifstream& in)
{
  in.seekg(0, std::ios::end);
  int64_t length{ in.tellg() };
  int64_t readLength{ std::min(length, 30ll) };
  in.seekg(length - readLength, std::ios::beg);

  std::string trailer(readLength, ' ');
  in.read(trailer.data(), 30);
  size_t startxrefOffset{ trailer.find("startxref") };
  if (startxrefOffset == std::string::npos)
    return false;

  while (trailer[startxrefOffset] < '0' || trailer[startxrefOffset] > '9')
    startxrefOffset++;
  size_t endxrefOffset{ trailer.find_first_not_of("0123456789", startxrefOffset) };
  int64_t xrefOffset{ std::stoll(trailer.substr(startxrefOffset, endxrefOffset - startxrefOffset)) };

  in.seekg(xrefOffset + 4, std::ios::beg);
  int firstObject, objectCount;
  in >> firstObject >> objectCount;

  std::unordered_map<int, int64_t> objectOffsets;

  for (int i{ 0 }; i < objectCount; i++)
  {
    int64_t byteOffset;
    int generationNumber;
    char usage;

    in >> byteOffset >> generationNumber >> usage;
    if (usage == 'f')
      continue;

    objectOffsets.emplace(firstObject + i, byteOffset);
  }

  std::unordered_map<int, int64_t> pdfStreams;

  for (const auto& [objectId, byteOffset] : objectOffsets)
  {
    in.seekg(byteOffset, std::ios::beg);

    int _objectId, _generationNumber;
    std::string _obj;
    in >> _objectId >> _generationNumber >> _obj;

    PDFObject pdfObject{ ReadObject(in) };
    std::string endobjOrStream;
    in >> endobjOrStream;
    if (endobjOrStream == "stream")
    {
      while (in.peek() == '\r' || in.peek() == '\n')
        in.get();
      pdfStreams[objectId] = in.tellg();
    }

    m_objects[objectId] = pdfObject;
  }

  for (const auto& [objectId, streamOffset] : pdfStreams)
  {
    PDFObject& pdfObject{ m_objects[objectId] };

    PDFObject streamLengthObject{ pdfObject.GetDictionary().at("Length") };
    if (streamLengthObject.IsReference())
      streamLengthObject = m_objects[streamLengthObject.GetReference()];
    PDFObject::Integer streamLength{ streamLengthObject.GetInteger() };

    in.seekg(streamOffset, std::ios::beg);
    PDFObject::Stream objectStream(streamLength);
    in.read(reinterpret_cast<char*>(objectStream.data()), streamLength);
    pdfObject.SetStream(objectStream);
  }

  return true;
}

const std::unordered_map<PDFObject::ID, PDFObject>& PDFDocument::GetObjects() const
{
  return m_objects;
}
