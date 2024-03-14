#pragma once

#include <string>
#include <unordered_map>
#include <vector>

class PDFObject
{
  enum class Type
  {
    Null,
    Boolean,
    Integer,
    Decimal,
    Name,
    Array,
    Dictionary,
    Reference
  };

public:
  using ID = int;
  using Boolean = bool;
  using Integer = int64_t;
  using Decimal = float;
  using Name = std::string;
  using Array = std::vector<PDFObject>;
  using Dictionary = std::unordered_map<Name, PDFObject>;
  using Reference = ID;
  using Stream = std::vector<std::byte>;

private:
  Type m_type{ Type::Null };
  // TODO: Change to union?
  Boolean m_boolean;
  Integer m_integer;
  Decimal m_decimal;
  Name m_name;
  Array m_array;
  std::unordered_map<Name, PDFObject> m_dictionary;
  Reference m_reference;

  bool m_hasStream{ false };
  Stream m_stream;

public:
  bool IsNull() const { return m_type == Type::Null; }
  bool IsBoolean() const { return m_type == Type::Boolean; }
  bool IsInteger() const { return m_type == Type::Integer; }
  bool IsDecimal() const { return m_type == Type::Decimal; }
  bool IsName() const { return m_type == Type::Name; }
  bool IsArray() const { return m_type == Type::Array; }
  bool IsDictionary() const { return m_type == Type::Dictionary; }
  bool IsReference() const { return m_type == Type::Reference; }
  bool HasStream() const { return m_hasStream; }

  Boolean GetBoolean() const { return m_boolean; }
  Integer GetInteger() const { return m_integer; }
  Decimal GetDecimal() const { return m_decimal; }
  Decimal GetDecimalOrInt() const { return m_type == Type::Integer ? static_cast<Decimal>(m_integer) : m_decimal; }
  const Name& GetName() const { return m_name; }
  const Array& GetArray() const { return m_array; }
  const Dictionary& GetDictionary() const { return m_dictionary; }
  const Reference& GetReference() const { return m_reference; }
  std::string GetStream() const;

  void SetNull();
  void SetBoolean(Boolean boolean);
  void SetInteger(Integer integer);
  void SetDecimal(Decimal decimal);
  void SetName(const Name& name);
  void AddArrayEntry(const PDFObject& pdfObject);
  void AddDictionaryEntry(const Name& name, const PDFObject& pdfObject);
  void SetReference(const Reference& reference);
  void SetStream(const Stream& stream);
};
