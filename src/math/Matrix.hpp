#pragma once

#include <array>
#include <cmath>

namespace detail
{
// clang-format off
template<typename T>
class Matrix2Base
{
public:
  T x, y;
  Matrix2Base() = default;
  Matrix2Base(T _x, T _y) : x(_x), y(_y) {}
  T& at2(int i)       { switch(i) { case 0: return x; default: return y; } }
  T  at2(int i) const { switch(i) { case 0: return x; default: return y; } }
};

template<typename T>
class Matrix3Base
{
public:
  T x, y, z;
  Matrix3Base() = default;
  Matrix3Base(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}
  T& at2(int i)       { switch(i) { case 0: return x; case 1: return y; default: return z; } }
  T  at2(int i) const { switch(i) { case 0: return x; case 1: return y; default: return z; } }
};

template<typename T>
class Matrix4Base
{
public:
  T x, y, z, w;
  Matrix4Base() = default;
  Matrix4Base(T _x, T _y, T _z, T _w) : x(_x), y(_y), z(_z), w(_w) {}
  T& at2(int i)       { switch(i) { case 0: return x; case 1: return y; case 2: return z; default: return w; } }
  T  at2(int i) const { switch(i) { case 0: return x; case 1: return y; case 2: return z; default: return w; } }
};
// clang-format on

template<typename T, int SIZE>
class MatrixElseBase
{
public:
  std::array<T, SIZE> m_data;

  MatrixElseBase() = default;

  template<typename... Ts, typename = std::enable_if_t<sizeof...(Ts) == SIZE>>
  MatrixElseBase(Ts... values)
    : m_data{ values... }
  {
  }
};
}

template<typename T, int ROWS, int COLS>
class Matrix
  : public std::conditional_t<
      4 < ROWS || 1 < COLS,
      detail::MatrixElseBase<T, ROWS * COLS>,
      std::conditional_t<ROWS == 2,
                         detail::Matrix2Base<T>,
                         std::conditional_t<ROWS == 3, detail::Matrix3Base<T>, detail::Matrix4Base<T>>>>
{
  constexpr static int SIZE{ ROWS * COLS };
  constexpr static bool HAS_XYZW{ ROWS <= 4 && COLS == 1 };
  //  using Self = Matrix<T, ROWS, COLS>;
  using Base = std::conditional_t<
    4 < ROWS || 1 < COLS,
    detail::MatrixElseBase<T, ROWS * COLS>,
    std::conditional_t<ROWS == 2,
                       detail::Matrix2Base<T>,
                       std::conditional_t<ROWS == 3, detail::Matrix3Base<T>, detail::Matrix4Base<T>>>>;

  // clang-format off
  T& at(int y, int x)       { return at(y * COLS + x); }
  T  at(int y, int x) const { return at(y * COLS + x); }
  T& at(int i)       { if constexpr (HAS_XYZW) return this->at2(i); else return this->m_data[i]; }
  T  at(int i) const { if constexpr (HAS_XYZW) return this->at2(i); else return this->m_data[i]; }
  // clang-format on

public:
  Matrix() = default;

  // clang-format off
  explicit Matrix(T fillValue)
  {
    if constexpr (HAS_XYZW && ROWS >= 1) this->x = fillValue;
    if constexpr (HAS_XYZW && ROWS >= 2) this->y = fillValue;
    if constexpr (HAS_XYZW && ROWS >= 3) this->z = fillValue;
    if constexpr (HAS_XYZW && ROWS >= 4) this->w = fillValue;
    if constexpr (!HAS_XYZW) this->m_data.fill(fillValue);
  }
  // clang-format off

  template<typename... Ts, typename = std::enable_if_t<sizeof...(Ts) == SIZE>>
  Matrix(Ts... values)
    //: m_data{ values... } {  }
    : Base{ values... }
  {
  }

  // clang-format off
  T& operator()(int y, int x)       { return at(y, x); }
  T  operator()(int y, int x) const { return at(y, x); }
  T* Data()             { if constexpr (HAS_XYZW) return &this->x; else return this->m_data.data(); }
  const T* Data() const { if constexpr (HAS_XYZW) return &this->x; else return this->m_data.data(); }
  // clang-format off

  // clang-format off
  /*
  template<typename = std::enable_if_t<ROWS >= 1 && COLS == 1>> T& x()       { return operator()(0, 0); }
  template<typename = std::enable_if_t<ROWS >= 1 && COLS == 1>> T  x() const { return operator()(0, 0); }
  template<typename = std::enable_if_t<ROWS >= 2 && COLS == 1>> T& y()       { return operator()(1, 0); }
  template<typename = std::enable_if_t<ROWS >= 2 && COLS == 1>> T  y() const { return operator()(1, 0); }
  template<typename = std::enable_if_t<ROWS >= 3 && COLS == 1>> T& z()       { return operator()(2, 0); }
  template<typename = std::enable_if_t<ROWS >= 3 && COLS == 1>> T  z() const { return operator()(2, 0); }
  template<typename = std::enable_if_t<ROWS >= 4 && COLS == 1>> T& w()       { return operator()(3, 0); }
  template<typename = std::enable_if_t<ROWS >= 4 && COLS == 1>> T  w() const { return operator()(3, 0); }
  */
  // clang-format on

  bool operator==(const Matrix& rhs) const
  {
    for (int i{ 0 }; i < SIZE; i++)
      if (at(i) != rhs.at(i))
        return false;
    return true;
  }

  // clang-format off
  Matrix& operator+=(const Matrix& rhs) { for (int i{ 0 }; i < SIZE; i++) at(i) += rhs.at(i); return *this; }
  Matrix& operator-=(const Matrix& rhs) { for (int i{ 0 }; i < SIZE; i++) at(i) -= rhs.at(i); return *this; }
  Matrix operator+(const Matrix& rhs) const { Matrix copy{ *this }; copy += rhs; return copy; }
  Matrix operator-(const Matrix& rhs) const { Matrix copy{ *this }; copy -= rhs; return copy; }

  Matrix& operator+=(T scalar) { for (int i{ 0 }; i < SIZE; i++) at(i) += scalar; return *this; }
  Matrix& operator-=(T scalar) { for (int i{ 0 }; i < SIZE; i++) at(i) -= scalar; return *this; }
  Matrix& operator*=(T scalar) { for (int i{ 0 }; i < SIZE; i++) at(i) *= scalar; return *this; }
  Matrix& operator/=(T scalar) { for (int i{ 0 }; i < SIZE; i++) at(i) /= scalar; return *this; }
  Matrix operator+(T scalar) const { Matrix copy{ *this }; copy += scalar; return copy; }
  Matrix operator-(T scalar) const { Matrix copy{ *this }; copy -= scalar; return copy; }
  Matrix operator*(T scalar) const { Matrix copy{ *this }; copy *= scalar; return copy; }
  Matrix operator/(T scalar) const { Matrix copy{ *this }; copy /= scalar; return copy; }
  // clang-format on

  template<typename = std::enable_if_t<COLS == 1>>
  T Dot(const Matrix& rhs) const
  {
    T dotProduct{ 0 };
    for (int i{ 0 }; i < SIZE; i++)
      dotProduct += at(i) * rhs.at(i);
    return dotProduct;
  }

  T LengthSquared() const { return Dot(*this); }
  T Length() const { return std::sqrt(LengthSquared()); }
  Matrix Normalized() const { return *this / Length(); };
  void Normalize() { *this /= Length(); };
};
