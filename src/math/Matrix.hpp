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
protected:
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
protected:
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
protected:
  Matrix4Base() = default;
  Matrix4Base(T _x, T _y, T _z, T _w) : x(_x), y(_y), z(_z), w(_w) {}
  T& at2(int i)       { switch(i) { case 0: return x; case 1: return y; case 2: return z; default: return w; } }
  T  at2(int i) const { switch(i) { case 0: return x; case 1: return y; case 2: return z; default: return w; } }
};
// clang-format on

template<typename T, int SIZE>
class MatrixElseBase
{
protected:
  std::array<T, SIZE> m_data;

  MatrixElseBase() = default;

  template<typename... Ts, typename = std::enable_if_t<sizeof...(Ts) == SIZE>>
  MatrixElseBase(Ts... values)
    : m_data{ values... }
  {
  }

  T& at2(int i) { return m_data[i]; }
  T at2(int i) const { return m_data[i]; }
};
} // namespace detail

template<typename T, int ROWS, int COLS>
class Matrix final
  : public std::conditional_t<
      4 < ROWS || 1 < COLS,
      detail::MatrixElseBase<T, ROWS * COLS>,
      std::conditional_t<ROWS == 2,
                         detail::Matrix2Base<T>,
                         std::conditional_t<ROWS == 3, detail::Matrix3Base<T>, detail::Matrix4Base<T>>>>
{
  constexpr static int SIZE{ ROWS * COLS };
  constexpr static bool HAS_XYZW{ ROWS <= 4 && COLS == 1 };
  constexpr static bool IS_SQUARE{ ROWS == COLS };
  //  using Self = Matrix<T, ROWS, COLS>;
  using Base = std::conditional_t<
    4 < ROWS || 1 < COLS,
    detail::MatrixElseBase<T, ROWS * COLS>,
    std::conditional_t<ROWS == 2,
                       detail::Matrix2Base<T>,
                       std::conditional_t<ROWS == 3, detail::Matrix3Base<T>, detail::Matrix4Base<T>>>>;

  // clang-format off
  T& at(int y, int x)       { return this->at2(y * COLS + x); }
  T  at(int y, int x) const { return this->at2(y * COLS + x); }
  T& at(int i)              { return this->at2(i); }
  T  at(int i) const        { return this->at2(i); }
  // clang-format on

  template<int N,
           typename U,
           typename... Us,
           typename = std::enable_if_t<std::is_integral_v<U> || std::is_same_v<U, T>>>
  void FillConstruct(U value, Us... rest)
  {
    at(N) = static_cast<T>(value);
    if constexpr (N + 1 < SIZE)
      FillConstruct<N + 1>(rest...);
  }

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
  // clang-format on

  template<typename... Ts,
           typename = std::enable_if_t<sizeof...(Ts) == SIZE && std::conjunction_v<std::is_same<T, Ts>...>>>
  Matrix(Ts... values)
    : Base{ values... }
  {
  }

  // TODO: Why is "typename U = T" not possible for both constructors?
  template<typename U = T,
           typename... Ts,
           typename = std::enable_if_t<(sizeof...(Ts) == SIZE) && !(std::conjunction_v<std::is_same<U, Ts>...>)>>
  Matrix(Ts... values)
  {
    FillConstruct<0>(values...);
  }

  template<typename U>
  explicit Matrix(const Matrix<U, ROWS, COLS>& source)
  {
    for (int y{ 0 }; y < ROWS; y++)
      for (int x{ 0 }; x < COLS; x++)
        at(y, x) = static_cast<T>(source(y, x));
  }

  template<bool _unused = true, typename = std::enable_if_t<IS_SQUARE && _unused>>
  static Matrix Identity()
  {
    Matrix m{ T{ 0 } };
    for (int i{ 0 }; i < ROWS; i++)
      m.at(i, i) = T{ 1 };
    return m;
  }

  // clang-format off
  T& operator()(int y, int x)       { return at(y, x); }
  T  operator()(int y, int x) const { return at(y, x); }
  T* Data()             { if constexpr (HAS_XYZW) return &this->x; else return this->m_data.data(); }
  const T* Data() const { if constexpr (HAS_XYZW) return &this->x; else return this->m_data.data(); }
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
  Matrix operator-() const { Matrix copy{ *this }; for (int i{ 0 }; i < SIZE; i++) copy.at(i) = -copy.at(i); return copy; }

  Matrix& operator+=(T scalar) { for (int i{ 0 }; i < SIZE; i++) at(i) += scalar; return *this; }
  Matrix& operator-=(T scalar) { for (int i{ 0 }; i < SIZE; i++) at(i) -= scalar; return *this; }
  Matrix& operator*=(T scalar) { for (int i{ 0 }; i < SIZE; i++) at(i) *= scalar; return *this; }
  Matrix& operator/=(T scalar) { for (int i{ 0 }; i < SIZE; i++) at(i) /= scalar; return *this; }
  Matrix operator+(T scalar) const { Matrix copy{ *this }; copy += scalar; return copy; }
  Matrix operator-(T scalar) const { Matrix copy{ *this }; copy -= scalar; return copy; }
  Matrix operator*(T scalar) const { Matrix copy{ *this }; copy *= scalar; return copy; }
  Matrix operator/(T scalar) const { Matrix copy{ *this }; copy /= scalar; return copy; }
  friend Matrix operator+(T scalar, const Matrix& rhs) { return rhs + scalar; }
  friend Matrix operator-(T scalar, const Matrix& rhs) { return -rhs + scalar; }
  friend Matrix operator*(T scalar, const Matrix& rhs) { return rhs * scalar; }
  friend Matrix operator/(T scalar, const Matrix& rhs) { Matrix copy; for (int i{ 0 }; i < SIZE; i++) copy.at(i) = scalar / rhs.at(i); return copy; }

  // TODO: cwiseMultiply and cwiseProduct (also divide) are very easy to confuse
  Matrix& cwiseMultiply(const Matrix& rhs) { for (int i{ 0 }; i < SIZE; i++) at(i) *= rhs.at(i); return *this; }
  Matrix& cwiseDivide  (const Matrix& rhs) { for (int i{ 0 }; i < SIZE; i++) at(i) /= rhs.at(i); return *this; }
  Matrix cwiseProduct (const Matrix& rhs) { Matrix copy{ *this }; copy.cwiseMultiply(rhs); return copy; }
  Matrix cwiseQuotient(const Matrix& rhs) { Matrix copy{ *this }; copy.cwiseDivide(rhs);   return copy; }
  // clang-format on

  template<bool _unused = true, typename = std::enable_if_t<COLS == 1 && _unused>>
  T Dot(const Matrix& rhs) const
  {
    T dotProduct{ 0 };
    for (int i{ 0 }; i < SIZE; i++)
      dotProduct += at(i) * rhs.at(i);
    return dotProduct;
  }

  template<int ROWS_RHS, int COLS_RHS, typename = std::enable_if_t<COLS == ROWS_RHS>>
  Matrix<T, ROWS, COLS_RHS> operator*(const Matrix<T, ROWS_RHS, COLS_RHS>& rhs) const
  {
    Matrix<T, ROWS, COLS_RHS> product{ T{ 0 } };
    for (int y{ 0 }; y < ROWS; y++)
      for (int x{ 0 }; x < COLS_RHS; x++)
        for (int i{ 0 }; i < COLS; i++)
          product(y, x) += at(y, i) * rhs(i, x);
    return product;
  }

  template<bool _unused = true, typename = std::enable_if_t<IS_SQUARE && _unused>>
  Matrix& operator*=(const Matrix& rhs)
  {
    // TODO: Can this be done in-place?
    Matrix result{ *this * rhs };
    for (int i{ 0 }; i < SIZE; i++)
      at(i) = result.at(i);
    return *this;
  }

  T LengthSquared() const { return Dot(*this); }
  T Length() const { return std::sqrt(LengthSquared()); }
  Matrix Normalized() const { return *this / Length(); }
  void Normalize() { *this /= Length(); }

  // clang-format off
  template<bool _unused = true, typename = std::enable_if_t<ROWS == 3 && COLS == 3 && _unused>>
  static Matrix Translate(const Matrix<T, 2, 1>& t)
  {
    return Matrix{  1,   0,  0,
                    0,   1,  0,
                   t.x, t.y, 1 };
  }
  template<bool _unused = true, typename = std::enable_if_t<ROWS == 4 && COLS == 4 && _unused>>
  static Matrix Translate(const Matrix<T, 3, 1>& t)
  {
    return Matrix{  1,   0,   0,  0,
                    0,   1,   0,  0,
                    0,   0,   1,  0,
                   t.x, t.y, t.z, 1 };
  }

  template<bool _unused = true, typename = std::enable_if_t<ROWS == 3 && COLS == 3 && _unused>>
  static Matrix Rotate(T angle)
  {
    const T cos{ std::cos(angle) };
    const T sin{ std::sin(angle) };
    return Matrix{ cos, -sin, 0,
                   sin,  cos, 0,
                    0,    0,  1 };
  }

  template<bool _unused = true, typename = std::enable_if_t<ROWS == 3 && COLS == 3 && _unused>>
  static Matrix Scale(const Matrix<T, 2, 1>& s)
  {
    return Matrix{ s.x, 0,  0,
                    0, s.y, 0,
                    0,  0,  1 };
  }
  template<bool _unused = true, typename = std::enable_if_t<ROWS == 4 && COLS == 4 && _unused>>
  static Matrix Scale(const Matrix<T, 3, 1>& s)
  {
    return Matrix{ s.x, 0,  0,  0,
                   0,  s.y, 0,  0,
                   0,   0, s.z, 0,
                   0,   0,  0,  1 };
  }
  // clang-format on
};

using Matrix3 = Matrix<float, 3, 3>;
using Matrix4 = Matrix<float, 4, 4>;
