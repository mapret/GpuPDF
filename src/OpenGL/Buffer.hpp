#pragma once

#include <cstddef>

namespace gl
{
class Buffer
{
  unsigned m_name;

public:
  Buffer();
  ~Buffer();
  Buffer(const Buffer&) = delete;
  Buffer& operator=(const Buffer&) = delete;

  void Bind() const;
  void Unbind() const;
  void SetData(std::ptrdiff_t dataLength, const void* data);
};
} // namespace gl
