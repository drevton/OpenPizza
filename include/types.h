// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
#ifndef PT_TYPES_H
#define PT_TYPES_H

#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

// The supported native compilers narrow signed integers with two's complement semantics.
_Static_assert((i8)255u == -1 && (i16)65535u == -1, "native signed narrowing");

#endif
