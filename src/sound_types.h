// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
#ifndef PT_SOUND_TYPES_H
#define PT_SOUND_TYPES_H
#include "types.h"

// Sound objects.

#include <stddef.h>
#include <stdbool.h>
// Audio resources own ordinary C objects and sample buffers.

typedef enum PTSoundEndpointKind {
  PT_SOUND_ENDPOINT_NONE,
  PT_SOUND_ENDPOINT_OTHER,
  PT_SOUND_ENDPOINT_PCM,
  PT_SOUND_ENDPOINT_MIDI
} PTSoundEndpointKind;
typedef struct PTSoundVector PTSoundVector;
typedef struct PTSoundDMATransfer PTSoundDMATransfer;
typedef struct PTPCMResident PTPCMResident;
typedef struct PTSoundResources {
  u8 *bank;
  u8 *fm_status;
  u8 *music;
  u8 *cursor;
  u8 *driver;
  PTPCMResident *resident;
  u8 *vector;
  PTSoundVector *bridge;
  u8 *repeat_start;
  u8 *output;
  u8 *repeat_end;
  PTSoundDMATransfer *dma_record;
  u8 *samples;
  u8 *dos_blocks[256];
} PTSoundResources;
extern PTSoundResources pt_sound_resources;
typedef enum PTSoundBufferKind {
  PT_SOUND_BUFFER_STATUS,
  PT_SOUND_BUFFER_MUSIC,
  PT_SOUND_BUFFER_BANK,
  PT_SOUND_BUFFER_INTRO,
  PT_SOUND_BUFFER_COUNT
} PTSoundBufferKind;
// Keep the resident driver's complete byte windows, including unused tails.

u32 pt_sound_buffer_size(PTSoundBufferKind);
u8 *pt_sound_buffer_allocate(PTSoundBufferKind);
void pt_sound_buffer_release(u8 *);
void pt_sound_buffer_storage_reset(void);
void pt_native_sound_storage_reset(void);
void pt_sound_pcm_driver_image_set(const u8 *);
void pt_sound_vector_image_set(const u8 *);
void pt_sound_vector_storage_reset(void);

// Dos memory.

#include <stdbool.h>
#include <stddef.h>
enum { PT_DOS_BLOCKS = 256, PT_DOS_CONVENTIONAL_CAPACITY = 56 * 1024, PT_DOS_UPPER_CAPACITY = 256 * 1024 };
typedef enum PTDOSPool { PT_DOS_CONVENTIONAL, PT_DOS_UPPER, PT_DOS_POOL_COUNT } PTDOSPool;
u8 *pt_dos_memory_storage(PTDOSPool pool);
typedef struct PTPlatformAllocation {
  u8 *buffer;
  u32 size, handle;
} PTPlatformAllocation;
typedef struct PTDOSAllocator {
  u8 *begin, *end;
  u32 next_selector, count;
  PTPlatformAllocation allocations[PT_DOS_BLOCKS];
} PTDOSAllocator;
void pt_dos_memory_reset(void);
void pt_dos_memory_observer(void (*released)(const void *, size_t));
const PTDOSAllocator *pt_dos_memory_state(void);
void pt_dos_memory_restore(const PTDOSAllocator *);
u8 *pt_dos_memory_allocate_in(PTDOSAllocator *, u16, u16 *);
u32 pt_dos_memory_free_in(PTDOSAllocator *, u16);
u8 *pt_dos_memory_allocate(u16, u16 *);
u32 pt_dos_memory_free(u16);
bool pt_dos_memory_contains(const void *, u32);

// Sound tables.

extern const u8 pt_fm_operator_map[9];
extern const u8 pt_fm_default_instrument[11];
extern const u8 pt_fm_percussion_operators[16];
extern const u8 pt_fm_percussion_bits[11];
extern const u8 pt_fm_percussion_channels[11];
extern const u8 pt_fm_note_table[128];
extern const u16 pt_fm_frequency_table[768];
extern const u16 pt_fm_velocity_table[33];
extern const u8 pt_fm_default_instruments[256];
extern const struct PTPCMResident pt_pcm_initial_state;

#endif
