#pragma once

// clang-format off

#define DEFINE_ENUM_FLAGS(ETYPE, INTTYPE) \
inline ETYPE  operator| (ETYPE a,  ETYPE b) { return      static_cast<ETYPE >(     static_cast<INTTYPE >(a) |  static_cast<INTTYPE>(b)); } \
inline ETYPE& operator|=(ETYPE& a, ETYPE b) { return reinterpret_cast<ETYPE&>(reinterpret_cast<INTTYPE&>(a) |= static_cast<INTTYPE>(b)); } \
inline ETYPE  operator& (ETYPE a,  ETYPE b) { return      static_cast<ETYPE >(     static_cast<INTTYPE >(a) &  static_cast<INTTYPE>(b)); } \
inline ETYPE& operator&=(ETYPE& a, ETYPE b) { return reinterpret_cast<ETYPE&>(reinterpret_cast<INTTYPE&>(a) &= static_cast<INTTYPE>(b)); } \
inline ETYPE  operator^ (ETYPE a,  ETYPE b) { return      static_cast<ETYPE >(     static_cast<INTTYPE >(a) ^  static_cast<INTTYPE>(b)); } \
inline ETYPE& operator^=(ETYPE& a, ETYPE b) { return reinterpret_cast<ETYPE&>(reinterpret_cast<INTTYPE&>(a) ^= static_cast<INTTYPE>(b)); } \
inline ETYPE  operator~(ETYPE a)            { return      static_cast<ETYPE>(~static_cast<INTTYPE>(a)); } \
inline bool EnumFlagSet(ETYPE a, ETYPE b)   { return (a & b) == b; }

// clang-format on
