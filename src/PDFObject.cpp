#include "PDFObject.hpp"
#include <array>
#include <zlib.h>

void PDFObject::SetNull()
{
  m_type = Type::Null;
}

void PDFObject::SetBoolean(PDFObject::Boolean boolean)
{
  m_type = Type::Boolean;
  m_boolean = boolean;
}

void PDFObject::SetInteger(PDFObject::Integer integer)
{
  m_type = Type::Integer;
  m_integer = integer;
}

void PDFObject::SetDecimal(PDFObject::Decimal decimal)
{
  m_type = Type::Decimal;
  m_decimal = decimal;
}

void PDFObject::SetName(const PDFObject::Name& name)
{
  m_type = Type::Name;
  m_name = name;
}

void PDFObject::SetString(const PDFObject::String& string)
{
  m_type = Type::String;
  m_string = string;
}

void PDFObject::AddArrayEntry(const PDFObject& pdfObject)
{
  m_type = Type::Array;
  m_array.push_back(pdfObject);
}

void PDFObject::AddDictionaryEntry(const PDFObject::Name& name, const PDFObject& pdfObject)
{
  m_type = Type::Dictionary;
  m_dictionary[name] = pdfObject;
}

void PDFObject::SetReference(const PDFObject::Reference& reference)
{
  m_type = Type::Reference;
  m_reference = reference;
}

void PDFObject::SetStream(const PDFObject::Stream& stream)
{
  m_hasStream = true;
  m_stream = stream;
}

std::string PDFObject::GetStream() const
{
  // TODO: What about other filters?

  z_stream zs;
  std::memset(&zs, 0, sizeof(zs));
  inflateInit(&zs);
  zs.next_in = reinterpret_cast<Bytef*>(const_cast<std::byte*>(m_stream.data()));
  zs.avail_in = static_cast<uInt>(m_stream.size());

  std::array<std::byte, 1024> tempBuffer;
  PDFObject::Stream streambuffer;

  int ret;
  do
  {
    zs.next_out = reinterpret_cast<Bytef*>(tempBuffer.data());
    zs.avail_out = static_cast<uInt>(tempBuffer.size());
    ret = inflate(&zs, 0);
    if (zs.total_out > streambuffer.size())
      streambuffer.insert(streambuffer.end(),
                          tempBuffer.begin(),
                          tempBuffer.begin() + (zs.total_out - static_cast<uLong>(streambuffer.size())));
  } while (ret == Z_OK);
  inflateEnd(&zs);

  std::string stream(streambuffer.size(), ' ');
  std::memcpy(stream.data(), streambuffer.data(), streambuffer.size());
  return stream;
}
