#ifndef PT_OPL_ADAPTER_H
#define PT_OPL_ADAPTER_H
#include "nuked-opl3/opl3.h"
#include <stdbool.h>
#include <stddef.h>
#include "../src/types.h"
#include <string.h>
// Shared hardware model only. Native game logic and original x86 keep
// independent instances; snapshots replace host pointers with chip offsets.

typedef struct PTOplState {
  opl3_chip chip;
  u64 samples, output_hash;
} PTOplState;
enum { PT_OPL_RATE = 44100, PT_OPL_STATE_SIZE = 16 + sizeof(PTOplState) };
_Static_assert(sizeof(void *) == sizeof(uintptr_t), "OPL pointer snapshot representation");
static inline void pt_opl_reset(PTOplState *s) {
  memset(s, 0, sizeof(*s));
  OPL3_Reset(&s->chip, PT_OPL_RATE);
}
// Keep successive hardware writes apart. An immediate key-off/key-on pair
// otherwise disappears before the envelope runs and the next note cannot
// restart its attack. The chip's queue retains these writes in snapshots.

static inline void pt_opl_write(PTOplState *s, u8 reg, u8 value) {
  OPL3_WriteRegBuffered(&s->chip, reg, value);
}
static inline i16 pt_opl_sample(PTOplState *s) {
  i16 stereo[2];
  OPL3_GenerateResampled(&s->chip, stereo);
  i16 sample = (i16)(((i32)stereo[0] + stereo[1]) / 2);
  u16 bits = (u16)sample;
  s->output_hash = (s->output_hash ^ (u8)bits) * UINT64_C(1099511628211);
  s->output_hash = (s->output_hash ^ (u8)(bits >> 8)) * UINT64_C(1099511628211);
  ++s->samples;
  return sample;
}
static inline bool pt_opl_pointer(void *field, uintptr_t base, bool loading) {
  uintptr_t value;
  memcpy(&value, field, sizeof(value));
  if (value) {
    if (loading) {
      if (value - 1u >= sizeof(opl3_chip))
        return false;
      value = base + value - 1u;
    } else {
      if (value < base || value - base >= sizeof(opl3_chip))
        return false;
      value = value - base + 1u;
    }
  }
  memcpy(field, &value, sizeof(value));
  return true;
}
static inline bool pt_opl_relocate(PTOplState *s, uintptr_t base, bool loading) {
#define PTR(field)                                                                                                     \
  do {                                                                                                                 \
    if (!pt_opl_pointer(&(field), base, loading))                                                                      \
      return false;                                                                                                    \
  } while (0)
  for (unsigned i = 0; i < 36; ++i) {
    PTR(s->chip.slot[i].channel);
    PTR(s->chip.slot[i].chip);
    PTR(s->chip.slot[i].mod);
    PTR(s->chip.slot[i].trem);
  }
  for (unsigned i = 0; i < 18; ++i) {
    PTR(s->chip.channel[i].slotz[0]);
    PTR(s->chip.channel[i].slotz[1]);
    PTR(s->chip.channel[i].pair);
    PTR(s->chip.channel[i].chip);
    for (unsigned j = 0; j < 4; ++j)
      PTR(s->chip.channel[i].out[j]);
  }
#undef PTR
  return true;
}
static inline bool pt_opl_save(const PTOplState *s, void *data, size_t size) {
  if (!data || size != PT_OPL_STATE_SIZE)
    return false;
  PTOplState copy = *s;
  if (!pt_opl_relocate(&copy, (uintptr_t)&s->chip, false))
    return false;
  u32 version = 1, length = sizeof(copy);
  u8 *p = data;
  memcpy(p, "PTOPL001", 8);
  memcpy(p + 8, &version, 4);
  memcpy(p + 12, &length, 4);
  memcpy(p + 16, &copy, sizeof(copy));
  return true;
}
static inline bool pt_opl_load(PTOplState *s, const void *data, size_t size) {
  if (!data || size != PT_OPL_STATE_SIZE)
    return false;
  const u8 *p = data;
  u32 version, length;
  memcpy(&version, p + 8, 4);
  memcpy(&length, p + 12, 4);
  if (memcmp(p, "PTOPL001", 8) || version != 1 || length != sizeof(*s))
    return false;
  PTOplState copy;
  memcpy(&copy, p + 16, sizeof(copy));
  if (copy.chip.rateratio != ((PT_OPL_RATE << 10) / 49716) || !pt_opl_relocate(&copy, (uintptr_t)&s->chip, true))
    return false;
  *s = copy;
  return true;
}
#endif
