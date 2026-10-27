// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
#ifndef PT_ARITHMETIC_H
#define PT_ARITHMETIC_H
#include "types.h"
#include <stdbool.h>
#include <string.h>

// Defined representation conversion for the original numeric algorithms.

static inline float pt_float_from_bits(u32 bits) {
  float value;
  memcpy(&value, &bits, sizeof(value));
  return value;
}
static inline u32 pt_float_bits(float value) {
  u32 bits;
  memcpy(&bits, &value, sizeof(bits));
  return bits;
}
static inline double pt_double_from_bits(u64 bits) {
  double value;
  memcpy(&value, &bits, sizeof(value));
  return value;
}
static inline u64 pt_double_bits(double value) {
  u64 bits;
  memcpy(&bits, &value, sizeof(bits));
  return bits;
}
// Preserve the original non-arithmetic test, including signaling NaNs.

static inline bool pt_float_is_zero(float value) {
  // An observable integer read prevents FCMP substitution on signaling NaNs.

  volatile u32 bits = pt_float_bits(value);
  return (bits & UINT32_C(0x7fffffff)) == 0;
}

// Reinterpret an x86 DWORD with defined C signed arithmetic.

static inline i32 pt_signed_dword(u32 value) {
  return value <= INT32_MAX ? (i32)value : -1 - (i32)(UINT32_MAX - value);
}

u8 CompareDateTime(u8 a, u8 b, u8 c, u8 d, u8 e, u8 f);
u32 RoundNumberToZeros(u32 value, i8 digits);
u32 Pow(u32 base, u32 exponent);
u32 RoundValueToMultipleOf(u32 value, u32 multiple);
i32 MulDiv100(i32 value, i32 percent);
i32 Mul100Div(i32 value, i32 divisor);
i32 Clamp(i32 value, i32 min, i32 max);

// Add the unsigned bit patterns modulo 2^32. A zero count permits NULL.
// Numeric arrays are native, aligned values; decoding file bytes belongs

u32 GetSumOfBytes(const void *data, u32 count);
u32 SumListOfWords(const i16 *values, u32 count);
u32 SumListOfUnsignedWords(const u16 *values, u32 count);
u32 SumArrayOfDword(const i32 *values, u32 count);
#endif
