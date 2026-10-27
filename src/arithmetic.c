// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
// Arithmetic.

#include "arithmetic.h"
#include <stdlib.h>

// A one-unit tolerance applies only to the third component.

u8 CompareDateTime(u8 a, u8 b, u8 c, u8 d, u8 e, u8 f) {
  u8 result;
  if (d < a)
    result = 1;
  else if (d > a)
    result = 2;
  else if (e < b)
    result = 1;
  else if (e > b)
    result = 2;
  else if ((int)f < (int)c - 1)
    result = 1;
  else if ((int)f > (int)c + 1)
    result = 2;
  else
    result = 0;

  return result;
}

// Returns base for exponent zero as well as one.

u32 Pow(u32 base, u32 exponent) {
  u32 result = base;
  for (u32 i = 1; i < exponent; ++i)
    result *= base;
  return result;
}

// Rounds exact multiples upward too, and uses unsigned DIV.

u32 RoundValueToMultipleOf(u32 value, u32 multiple) {
  if (multiple == 0)
    abort();
  return multiple * ((multiple + value) / multiple);
}

i32 Clamp(i32 value, i32 min, i32 max) {
  if (value < min)
    return min;
  if (value > max)
    return max;
  return value;
}

// Multiplies the low dwords before signed IDIV by 100.

i32 MulDiv100(i32 value, i32 percent) {
  i32 result = pt_signed_dword((u32)value * (u32)percent) / 100;

  return result;
}

// Low-dword product, signed division, zero divisor => 100.

i32 Mul100Div(i32 value, i32 divisor) {
  if (!divisor)
    return 100;
  i32 product = pt_signed_dword((u32)value * 100u);
  if (product == INT32_MIN && divisor == -1)
    abort();
  return product / divisor;
}

// Pow(10,0) is 10, and positive digits add a full multiple even when the input is already
// divisible. Zero factors trap.

u32 RoundNumberToZeros(u32 value, i8 digits) {
  u32 magnitude = digits < 0 ? (u32)(-(int)digits) : (u32)digits;

  u32 factor = Pow(10, magnitude);

  if (!factor)
    abort();
  u32 result = factor * (value / factor);

  return digits > 0 ? result + factor : result;
}

u32 GetSumOfBytes(const void *data, u32 count) {
  const unsigned char *bytes = data;
  u32 sum = 0;
  for (u32 i = 0; i < count; ++i)
    sum += bytes[i];
  return sum;
}

u32 SumListOfWords(const i16 *values, u32 count) {
  u32 sum = 0;
  for (u32 i = 0; i < count; ++i)
    sum += (u16)values[i];
  return sum;
}

u32 SumArrayOfDword(const i32 *values, u32 count) {
  u32 sum = 0;
  for (u32 i = 0; i < count; ++i)
    sum += (u32)values[i];
  return sum;
}

u32 SumListOfUnsignedWords(const u16 *values, u32 count) {
  u32 sum = 0;
  for (u32 i = 0; i < count; ++i)
    sum += values[i];
  return sum;
}
