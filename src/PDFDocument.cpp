#include "PDFDocument.hpp"
#include <fstream>
#include <iostream>

namespace
{
void NotImplemented(const std::string& function)
{
  std::cerr << function << " not implemented\n";
}

char PeekChar(std::istream& in)
{
  return static_cast<char>(in.peek());
}

char GetChar(std::istream& in)
{
  return static_cast<char>(in.get());
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
  return c == '/' || c == '<' || c == '>' || c == '[' || c == ']' || c == '(' || c == ')';
}

void SkipWhitespace(std::istream& in)
{
  while (true)
  {
    if (IsWhitespace(PeekChar(in)))
      GetChar(in);
    else
      break;
  }
}

PDFObject::Name ReadName(std::istream& in)
{
  PDFObject::Name name;
  while (!IsWhitespace(PeekChar(in)) && !IsSpecialChar(PeekChar(in)))
    name += static_cast<char>(GetChar(in));
  return name;
}

PDFObject ReadObject(std::istream& in)
{
  SkipWhitespace(in);

  char c{ static_cast<char>(PeekChar(in)) };
  if ((c >= '0' && c <= '9') || c == '.' || c == '-')
  {
    std::string s;
    while (!IsWhitespace(PeekChar(in)) && !IsSpecialChar(PeekChar(in)))
      s += static_cast<char>(GetChar(in));
    if (s.find_first_of('.') != std::string::npos)
    {
      PDFObject pdfObject;
      pdfObject.SetDecimal(std::stof(s));
      return pdfObject;
    }

    PDFObject::Integer number{ std::stoll(s) };

    int64_t positionBeforePeek{ in.tellg() };

    if (IsWhitespace(PeekChar(in)))
    {
      SkipWhitespace(in);
      if (IsDigit(PeekChar(in)))
      {
        int potentialGenerationNumber;
        in >> potentialGenerationNumber;
        if (IsWhitespace(PeekChar(in)))
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
  else if (c == 'n')
  {
    for (int i{ 0 }; i < 4; i++)
      GetChar(in);
    PDFObject pdfObject;
    pdfObject.SetNull();
    return pdfObject;
  }
  else if (c == 't')
  {
    for (int i{ 0 }; i < 4; i++)
      GetChar(in);
    PDFObject pdfObject;
    pdfObject.SetBoolean(true);
    return pdfObject;
  }
  else if (c == 'f')
  {
    for (int i{ 0 }; i < 5; i++)
      GetChar(in);
    PDFObject pdfObject;
    pdfObject.SetBoolean(false);
    return pdfObject;
  }
  else if (c == '/')
  {
    GetChar(in);
    PDFObject pdfObject;
    pdfObject.SetName(ReadName(in));
    return pdfObject;
  }
  else if (c == '(')
  {
    GetChar(in);
    int openParanthesisCount{ 1 };
    PDFObject::String string;
    while (openParanthesisCount > 0)
    {
      c = GetChar(in);
      if (c == '(')
        openParanthesisCount++;
      if (c == ')')
        openParanthesisCount--;
      if (openParanthesisCount > 0)
        string += c;
    }

    PDFObject pdfObject;
    pdfObject.SetString(string);
    return pdfObject;
  }
  else if (c == '<')
  {
    GetChar(in);
    if (PeekChar(in) == '<')
    {
      GetChar(in);
      PDFObject pdfObject;
      SkipWhitespace(in);
      while (PeekChar(in) != '>')
      {
        GetChar(in);
        PDFObject::Name dictionaryKey{ ReadName(in) };
        PDFObject dictionaryValue{ ReadObject(in) };
        pdfObject.AddDictionaryEntry(dictionaryKey, dictionaryValue);
        SkipWhitespace(in);
      }
      GetChar(in);
      GetChar(in);
      return pdfObject;
    }
    else
    {
      PDFObject::String string;
      bool running{ true };
      auto convertNibble{ [](char n) -> char
      {
        if (n >= 'a' && n <= 'f')
          return n - 'a' + 10;
        else if (n >= 'A' && n <= 'F')
          return n - 'A' + 10;
        return n - '0';
      } };
      while (running)
      {
        char upperNibble = GetChar(in);
        if (upperNibble == '>')
          break;
        char lowerNibble = GetChar(in);
        if (lowerNibble == '>')
        {
          lowerNibble = 0;
          running = false;
        }
        else
          lowerNibble = convertNibble(lowerNibble);
        upperNibble = convertNibble(upperNibble);

        c = static_cast<char>(upperNibble << 4) | lowerNibble;
        string += c;
      }
      PDFObject pdfObject;
      pdfObject.SetString(string);
      return pdfObject;
    }
  }
  else if (c == '[')
  {
    GetChar(in);
    PDFObject pdfObject;
    SkipWhitespace(in);
    while (PeekChar(in) != ']')
    {
      pdfObject.AddArrayEntry(ReadObject(in));
      SkipWhitespace(in);
    }
    GetChar(in);
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
  int64_t readLength{ std::min<int64_t>(length, 30) };
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
    // std::cout << "Object " << objectId << ": " << pdfObject << "\n";
    std::string endobjOrStream;
    in >> endobjOrStream;
    if (endobjOrStream == "stream")
    {
      while (PeekChar(in) == '\r' || PeekChar(in) == '\n')
        GetChar(in);
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
