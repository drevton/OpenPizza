// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
#ifndef PT_MEMORY_H
#define PT_MEMORY_H
#include "globals.h"
#include "arithmetic.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

enum {
  PT_AI_RESTAURANT_SIZE = offsetof(Rest, some_furniture_counter) + 1,
  PT_HUMAN_RESTAURANT_ALLOCATION_SIZE = sizeof(Rest) + 1
};

// Buffers are native allocations. Optional save storage binds their identities
// without replacing the allocation returned to the caller.

typedef struct PTBufferStorage {
  bool (*bind)(void *, u32, bool);
  void (*release)(void *);
} PTBufferStorage;
void pt_buffer_storage(const PTBufferStorage *);
u8 *pt_buffer_allocate(u32 size, bool clear);
void pt_buffer_release(void *);

// Month rollover copies all six receipt DWORDs, including reserved slots.

_Static_assert(sizeof(((Rest *)0)->curr_month_receipts) == 24, "six monthly receipt slots");

// Depot POIs overlay eight weapon quantities on the customer-count tail.
// Slots 6/7 share the ordinary POI total-customer DWORD. Use guest offsets
// rather than indexing the six-element customer array in native C.

_Static_assert(offsetof(Poi, customer_count_in_area) + 8u * 2u ==
        offsetof(Poi, total_customer_count) + sizeof(((Poi *)0)->total_customer_count),
    "depot POI weapon overlay");
_Static_assert(offsetof(RamTown, economy_history) + 255u < sizeof(RamTown), "economy history guest extent");
static inline Player *pt_player(u8 index) {
  if (index >= 8)
    abort();
  return &players[index];
}
static inline Character *pt_character(u8 index) {
  if (index >= 8)
    abort();
  return &characters[index];
}
#endif
