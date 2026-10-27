// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
// Savegame format.

#include "savegame.h"
#include "menu.h"
#include "render_types.h"
#include "files.h"
#include "sound.h"
#include "calendar.h"
#include "input.h"
#include "frame.h"

#include <stdio.h>

static bool saved_object_readable(guest_ptr32 address, size_t size) {
  return pt_valid_range(address, size);
}

// Saved images are checked only within their declared record or field span.
// Keep the equality fast path for large buffers and report the first byte.

static u32 saved_bytes_difference(const void *current, const void *previous, u32 address, size_t size) {
  if (!size || !memcmp(current, previous, size))
    return 0;
  const u8 *bytes = current, *before = previous;
  for (size_t i = 0; i < size; ++i)
    if (bytes[i] != before[i])
      return address + (u32)i;
  return 0;
}

// Optional boundary observers receive real stores. Native relationships live
// directly in their owning structs and never need registration or alias hashes.

typedef void (*PTWriteHook)(const void *, size_t);
static PTWriteHook *write_observers;
static unsigned write_observer_capacity;
static unsigned write_observer_count;
void pt_world_objects_observe(void (*hook)(const void *, size_t)) {
  if (!hook)
    return;
  for (unsigned i = 0; i < write_observer_count; ++i)
    if (write_observers[i] == hook)
      return;
  if (write_observer_count == write_observer_capacity) {
    unsigned capacity = write_observer_capacity ? write_observer_capacity * 2 : 8;
    PTWriteHook *hooks = realloc(write_observers, capacity * sizeof(*hooks));
    if (!hooks)
      abort();
    write_observers = hooks;
    write_observer_capacity = capacity;
  }
  write_observers[write_observer_count++] = hook;
}
void pt_world_objects_written(const void *storage, size_t size) {
  if (size)
    for (unsigned i = 0; i < write_observer_count; ++i)
      write_observers[i](storage, size);
}

PTGuestMemory pt_memory;
typedef struct PTMemoryRange {
  const void *begin, *end;
} PTMemoryRange;
static PTMemoryRange encoded_input;
static bool encoded_input_active;

// Native scalar/table storage is independent of its original image offsets.
// History lives only at this save boundary, including partial-byte imports.

typedef struct PTSavedGlobalValue {
  const void *native;
  guest_ptr32 address;
  size_t size;
  bool read_only;
} PTSavedGlobalValue;
// One schema supplies both directions and their compile-time width checks.
// These are save-format identities; native tables retain their own storage.

#define SAVED_GLOBAL(member, read_only) {&member, PT_FIELD_##member, sizeof(member), read_only},
#define SAVED_INTERFACE(member, address, width) {&pt_interface_strings.member, address, width, true},
#define SAVED_VIEW(member, address, width, read_only) {&member, address, width, read_only},
static const PTSavedGlobalValue saved_global_values[] = {
#include "src/savegame_globals.def"
};
#undef SAVED_GLOBAL
#undef SAVED_INTERFACE
#undef SAVED_VIEW

#define SAVED_GLOBAL(member, read_only)                                                                                \
  _Static_assert(sizeof(member) == sizeof(((PTNamedMemory *)0)->saved_##member), "native " #member " extent");
#define SAVED_INTERFACE(member, address, width)                                                                        \
  _Static_assert(sizeof(pt_interface_strings.member) >= (width), "native interface " #member " capacity");
#define SAVED_VIEW(member, address, width, read_only)                                                                  \
  _Static_assert(sizeof(member) >= (width), "native " #member " capacity");
#include "src/savegame_globals.def"
#undef SAVED_GLOBAL
#undef SAVED_INTERFACE
#undef SAVED_VIEW
_Static_assert(sizeof(character_initial_labels) == 37, "native character_initial_labels extent");
_Static_assert(sizeof(character_value_labels) == 128, "native character_value_labels extent");
_Static_assert(sizeof(info_box_formats) == 272, "native info_box_formats extent");
_Static_assert(sizeof(balance_labels) == 118, "native balance_labels extent");
_Static_assert(sizeof(finance_stats_labels) == 110, "native finance_stats_labels extent");
_Static_assert(sizeof(sound_digital_configuration) == 2, "native sound_digital_configuration extent");
_Static_assert(sizeof(sound_config_fallback_word) == 2, "native sound_config_fallback_word extent");
_Static_assert(sizeof(age_taste_preferences) == sizeof(((PTNamedMemory *)0)->saved_serialized_age_taste_preferences),
    "native age_taste_preferences extent");
_Static_assert(sizeof(season_names) == sizeof(((PTNamedMemory *)0)->saved_season_names), "native season_names extent");
_Static_assert(sizeof(PTFurnitureInfoFormats) == 150, "furniture format table extent");
_Static_assert(offsetof(PTFurnitureInfoFormats, oven_capacity) == 0, "oven_capacity format offset");
_Static_assert(offsetof(PTFurnitureInfoFormats, oven_rate) == 20, "oven_rate format offset");
_Static_assert(offsetof(PTFurnitureInfoFormats, oven_minutes) == 39, "oven_minutes format offset");
_Static_assert(offsetof(PTFurnitureInfoFormats, oven_price) == 60, "oven_price format offset");
_Static_assert(offsetof(PTFurnitureInfoFormats, oven_link) == 69, "oven_link format offset");
_Static_assert(offsetof(PTFurnitureInfoFormats, item_number) == 84, "item_number format offset");
_Static_assert(offsetof(PTFurnitureInfoFormats, furniture_price) == 94, "furniture_price format offset");
_Static_assert(offsetof(PTFurnitureInfoFormats, table_capacity) == 104, "table_capacity format offset");
_Static_assert(offsetof(PTFurnitureInfoFormats, table_chairs) == 123, "table_chairs format offset");
_Static_assert(offsetof(PTFurnitureInfoFormats, chair_link) == 138, "chair_link format offset");
_Static_assert(sizeof(mission_receipts) == sizeof(((PTNamedMemory *)0)->saved_serialized_mission_receipts),
    "native mission_receipts extent");
_Static_assert(
    sizeof(previous_month_expenses) == sizeof(((PTNamedMemory *)0)->saved_serialized_previous_month_expenses),
    "native previous_month_expenses extent");
_Static_assert(
    sizeof(previous_month_receipts) == sizeof(((PTNamedMemory *)0)->saved_serialized_previous_month_receipts),
    "native previous_month_receipts extent");
_Static_assert(
    sizeof(player_loans) == sizeof(((PTNamedMemory *)0)->saved_serialized_player_loans), "native player_loans extent");
_Static_assert(sizeof(player_investments) == sizeof(((PTNamedMemory *)0)->saved_serialized_player_investments),
    "native player_investments extent");
_Static_assert(sizeof(mission_expenses) == sizeof(((PTNamedMemory *)0)->saved_serialized_mission_expenses),
    "native mission_expenses extent");
_Static_assert(sizeof(player_goals) == 8u * 2u * sizeof(PersonalMissionFile), "native player goal storage extent");
_Static_assert(
    sizeof(((PTMissionGoals *)0)->kind) == sizeof(((PersonalMissionFile *)0)->goal), "personal mission kind field");
_Static_assert(
    sizeof(((PTMissionGoals *)0)->day) == sizeof(((PersonalMissionFile *)0)->day), "personal mission day field");
_Static_assert(
    sizeof(((PTMissionGoals *)0)->month) == sizeof(((PersonalMissionFile *)0)->month), "personal mission month field");
_Static_assert(
    sizeof(((PTMissionGoals *)0)->year) == sizeof(((PersonalMissionFile *)0)->year), "personal mission year field");
_Static_assert(sizeof(map_animation_colors) ==
        sizeof(((PTNamedMemory *)0)->saved_map_animation_palette_colors) +
            sizeof(((PTNamedMemory *)0)->saved_map_animation_palette_repeat),
    "native map_animation_colors extent");
_Static_assert(
    sizeof(loaded_character) == sizeof(((PTNamedMemory *)0)->saved_loaded_character), "native loaded_character extent");
_Static_assert(
    sizeof(temp_character) == sizeof(((PTNamedMemory *)0)->saved_temp_character), "native temp_character extent");
static PTPreferencesFileImage preferences_file_image;
const PTPreferencesFileImage *pt_preferences_file_image(void) {
  preferences_file_image.preferences = prefs;
  memcpy(preferences_file_image.animation_coordinates,
      staff_ui_file_animation,
      sizeof(preferences_file_image.animation_coordinates));
  return &preferences_file_image;
}
static PTNativeGlobals previous_native_globals, previous_saved_globals;
static bool global_values_active, publishing_global_values;
static u8 *global_history(PTNativeGlobals *history, const PTSavedGlobalValue *field) {
  size_t offset = (size_t)((uintptr_t)field->native - (uintptr_t)&pt_native_globals);
  return (u8 *)history + offset;
}
static bool saved_global_span(const void *pointer, size_t size, guest_ptr32 *address) {
  uintptr_t at = (uintptr_t)pointer, base = (uintptr_t)&pt_native_globals;
  uintptr_t file = (uintptr_t)&preferences_file_image;
  if (at >= file && at - file < sizeof(preferences_file_image) &&
      size <= sizeof(preferences_file_image) - (at - file)) {
    *address = PT_FIELD_prefs + (guest_ptr32)(at - file);
    return true;
  }
  uintptr_t tables = (uintptr_t)&pt_read_only_tables, strings = (uintptr_t)&pt_interface_strings;
  if ((at < base || at - base > sizeof(pt_native_globals)) &&
      (at < tables || at - tables > sizeof(pt_read_only_tables)) &&
      (at < strings || at - strings > sizeof(pt_interface_strings)))
    return false;
  // Prefer an actual field over an adjacent field's one-past pointer.

  for (unsigned pass = 0; pass < 2; ++pass)
    for (size_t i = 0; i < sizeof(saved_global_values) / sizeof(*saved_global_values); ++i) {
      const PTSavedGlobalValue *field = &saved_global_values[i];
      uintptr_t first = (uintptr_t)field->native;
      if (at < first || at - first > field->size || (!pass && at - first == field->size))
        continue;
      size_t offset = (size_t)(at - first);
      if (size > field->size - offset)
        continue;
      *address = field->address + (guest_ptr32)offset;
      return true;
    }
  return false;
}
static void *saved_global_decode(guest_ptr32 address, size_t size) {
  for (unsigned pass = 0; pass < 2; ++pass)
    for (size_t i = 0; i < sizeof(saved_global_values) / sizeof(*saved_global_values); ++i) {
      const PTSavedGlobalValue *field = &saved_global_values[i];
      if (address < field->address)
        continue;
      size_t offset = (size_t)(address - field->address);
      if (offset > field->size || (!pass && offset == field->size) || size > field->size - offset)
        continue;
      return (u8 *)(uintptr_t)field->native + offset;
    }
  return NULL;
}
static void commit_saved_dos_store(const void *storage, size_t size);
static u32 serialize_saved_globals(u32 address, size_t size) {
  if (!global_values_active || publishing_global_values)
    return 0;
  // Validate the requested fields before writing any pending value.

  for (size_t i = 0; i < sizeof(saved_global_values) / sizeof(*saved_global_values); ++i) {
    const PTSavedGlobalValue *field = &saved_global_values[i];
    if ((u64)field->address + field->size <= address || (u64)address + size <= field->address)
      continue;
    const u8 *saved = pt_buffer_from_guest(field->address);
    const u8 *previous = field->read_only ? field->native : global_history(&previous_saved_globals, field);
    u32 fault = saved_bytes_difference(saved, previous, field->address, field->size);
    if (fault)
      return fault;
  }
  publishing_global_values = true;
  for (size_t i = 0; i < sizeof(saved_global_values) / sizeof(*saved_global_values); ++i) {
    const PTSavedGlobalValue *field = &saved_global_values[i];
    if (field->read_only || (u64)field->address + field->size <= address || (u64)address + size <= field->address)
      continue;
    u8 *before = global_history(&previous_native_globals, field);
    if (!memcmp(field->native, before, field->size))
      continue;
    u8 *saved = pt_buffer_from_guest(field->address);
    memcpy(saved, field->native, field->size);
    memcpy(before, field->native, field->size);
    memcpy(global_history(&previous_saved_globals, field), saved, field->size);
    // This typed write also advances an overlapping DOS buffer view.

    commit_saved_dos_store(saved, field->size);
    pt_world_objects_written(saved, field->size);
  }
  publishing_global_values = false;
  return 0;
}
static void saved_global_values_written(const void *storage, size_t size) {
  if (!global_values_active || publishing_global_values || !size)
    return;
  uintptr_t at = (uintptr_t)storage;
  for (size_t i = 0; i < sizeof(saved_global_values) / sizeof(*saved_global_values); ++i) {
    const PTSavedGlobalValue *field = &saved_global_values[i];
    const u8 *saved = pt_buffer_from_guest(field->address);
    uintptr_t first = (uintptr_t)saved;
    size_t offset = at > first ? (size_t)(at - first) : 0;
    if (offset >= field->size || (first > at && first - at >= size))
      continue;
    size_t available = first > at ? size - (size_t)(first - at) : size;
    size_t count = field->size - offset < available ? field->size - offset : available;
    if (field->read_only)
      continue;
    memcpy((u8 *)(uintptr_t)field->native + offset, saved + offset, count);
    memcpy(global_history(&previous_native_globals, field) + offset, saved + offset, count);
    memcpy(global_history(&previous_saved_globals, field) + offset, saved + offset, count);
    commit_saved_dos_store(saved + offset, count);
  }
}
void pt_saved_global_values_restore(void) {
  global_values_active = false;
  publishing_global_values = false;
  for (size_t i = 0; i < sizeof(saved_global_values) / sizeof(*saved_global_values); ++i) {
    const PTSavedGlobalValue *field = &saved_global_values[i];
    const u8 *saved = pt_buffer_from_guest(field->address);
    if (field->read_only)
      continue;
    memcpy((void *)(uintptr_t)field->native, saved, field->size);
    memcpy(global_history(&previous_native_globals, field), saved, field->size);
    memcpy(global_history(&previous_saved_globals, field), saved, field->size);
  }
  pt_world_objects_observe(saved_global_values_written);
  global_values_active = true;
}

// Native owners report releases without knowing any allocator layout.

static void (*allocation_retired)(const void *, size_t);
void pt_heap_allocation_observer(void (*retired)(const void *, size_t)) {
  allocation_retired = retired;
  pt_dos_memory_observer(retired);
}
void pt_heap_allocation_retired(const void *storage, size_t size) {
  if (allocation_retired && size)
    allocation_retired(storage, size);
}

// Serialized identities reserve file-image bytes independently of native C allocations.

struct PTSavedResourceOwner;
enum {
  PT_ROOT_BUFFER = -1,
  PT_ROOT_FONT = -2,
  PT_ROOT_BITMAP = -3,
  PT_ROOT_CANVAS = -4,
  PT_ROOT_JATOBA = -5,
  PT_ROOT_RECIPE_SCRATCH = -7,
  PT_ROOT_COLOR = -9,
  PT_ROOT_PALETTE_POINTER = -10,
  PT_ROOT_COORDINATE = -11,
  PT_ROOT_JUICE = -14,
  PT_ROOT_MOUSE_IDENTITY = -17,
  PT_ROOT_CANONICAL_BUFFER = -18,
  PT_ROOT_PALETTE_SLOT = -19
};

const u8 *pt_saved_data_buffer(const void *pointer, size_t size) {
  (void)size;
  return pointer;
}

typedef struct PTSavedIdentitySpan {
  u32 offset, size;
  struct PTSavedIdentitySpan *next;
} PTSavedIdentitySpan;
static PTSavedIdentitySpan *saved_identity_spans;
static void saved_heap_bytes_written(const void *, size_t);
static void reset_saved_identities(void) {
  while (saved_identity_spans) {
    PTSavedIdentitySpan *next = saved_identity_spans->next;
    free(saved_identity_spans);
    saved_identity_spans = next;
  }
}
static void *reserve_saved_identity(u32 size) {
  if (!size || size > PT_HEAP_PLATFORM_CAPACITY)
    return NULL;
  // Keep a one-past identity distinct from the next object's start.

  u32 extent = (size + 16u) & ~UINT32_C(15);
  u32 offset = 0;
  PTSavedIdentitySpan **link = &saved_identity_spans;
  while (*link) {
    if (offset <= (*link)->offset && extent <= (*link)->offset - offset)
      break;
    offset = (*link)->offset + (*link)->size;
    link = &(*link)->next;
  }
  if (offset > PT_HEAP_PLATFORM_CAPACITY || extent > PT_HEAP_PLATFORM_CAPACITY - offset)
    return NULL;
  PTSavedIdentitySpan *span = malloc(sizeof(*span));
  if (!span)
    return NULL;
  *span = (PTSavedIdentitySpan){offset, extent, *link};
  *link = span;
  u8 *storage = pt_saved_heap_storage(PT_HEAP_ARENA_BASE + offset);
  // Fresh image bytes represent the native zero-initialized object, rather
  // than metadata left by a different allocation at this saved identity.

  memset(storage, 0, extent);
  saved_heap_bytes_written(storage, extent);
  return storage;
}
static void *allocate_saved_storage(u32 size, u32 flags) {
  if (!(flags & 2u))
    return reserve_saved_identity(size);
  u16 selector;
  void *storage = pt_dos_memory_allocate((u16)size, &selector);
  if (storage && (flags & 1u)) {
    memset(storage, 0, size);
    saved_heap_bytes_written(storage, size);
  }
  return storage;
}

static void release_saved_storage(void *storage) {
  if (!storage)
    return;
  u32 address = pt_buffer_to_guest(storage);
  PTSavedIdentitySpan **link = &saved_identity_spans;
  while (*link && PT_HEAP_ARENA_BASE + (*link)->offset != address)
    link = &(*link)->next;
  if (!*link) {
    fputs("Pizza Tycoon: releasing an unknown saved identity\n", stderr);
    abort();
  }
  PTSavedIdentitySpan *span = *link;
  pt_heap_allocation_retired(storage, span->size);
  *link = span->next;
  free(span);
}

void *mallocSafe(u32 size) {
  void *result = allocate_saved_storage(size, 1);
  if (!result && temp_canvas) {
    u8 yes = ShowMessage2(0, no_memory_continue_prompt);
    if (!yes)
      ExitProcess(0);
  }
  return result;
}

void *MemAllocOrMsg_(u32 size) {
  void *result = mallocSafe(size);

  return result;
}

// Temporary image byte buffers have native ownership. Serialized heap bytes
// remain an independent projection, including bytes left behind after free.

typedef struct PTSavedBufferOwner {
  u8 *native, *previous_native, *previous_saved;
  guest_ptr32 address;
  u8 *wire;
  u32 size;
  struct PTSavedBufferOwner *next;
} PTSavedBufferOwner;
static PTSavedBufferOwner *saved_buffers;
// A concrete live allocation wins over its neighbour's one-past address.
// Host allocators may place payloads adjacently even when saved IDs differ.

static bool saved_buffer_borrows(const PTSavedBufferOwner *buffer, const void *pointer) {
  uintptr_t offset = (uintptr_t)pointer - (uintptr_t)buffer->native;
  if (offset > buffer->size)
    return false;
  if (offset == buffer->size)
    for (const PTSavedBufferOwner *other = saved_buffers; other; other = other->next)
      if (other != buffer && other->native == pointer)
        return false;
  return true;
}

// Platform arenas own native bytes independently of their serialized image.
// They are separate from malloc-backed game buffers and allocator metadata.

static PTSavedBufferOwner saved_dos_buffers[PT_DOS_POOL_COUNT];
static bool saved_dos_buffers_active;
// Paragraphs outside the configured arena/resident allocations can belong to

// owns or imports them as native payload.

static u8 saved_dos_owned[PT_DOS_POOL_COUNT][PT_DOS_UPPER_CAPACITY / 16];
static bool saved_dos_native_span(const void *pointer, size_t size, u32 *address, bool live) {
  if (!saved_dos_buffers_active)
    return false;
  uintptr_t at = (uintptr_t)pointer;
  for (unsigned i = 0; i < PT_DOS_POOL_COUNT; ++i) {
    const PTSavedBufferOwner *owner = &saved_dos_buffers[i];
    uintptr_t base = (uintptr_t)owner->native;
    if (at < base || at - base > owner->size || size > owner->size - (at - base))
      continue;
    if (live && !pt_dos_memory_contains(pointer, (u32)size))
      return false;
    u32 word = owner->address + (u32)(at - base);
    if (live)
      for (PTSavedBufferOwner *primary = saved_buffers; primary; primary = primary->next)
        if ((u64)word < (u64)primary->address + primary->size && (u64)primary->address < (u64)word + size)
          return false;
    *address = word;
    return true;
  }
  return false;
}
static u8 *saved_dos_storage(u32 address) {
  if (!saved_dos_buffers_active)
    abort();
  for (unsigned i = 0; i < PT_DOS_POOL_COUNT; ++i) {
    const PTSavedBufferOwner *owner = &saved_dos_buffers[i];
    if (address >= owner->address && address - owner->address <= owner->size)
      return owner->native + (address - owner->address);
  }
  abort();
}
static bool saved_dos_word_contains(u32 address, u32 size) {
  const PTDOSAllocator *state = pt_dos_memory_state();
  for (unsigned i = 0; i < state->count; ++i) {
    const PTPlatformAllocation *allocation = &state->allocations[i];
    u32 base = pt_buffer_to_guest(allocation->buffer);
    if (address >= base && size <= allocation->size && address - base <= allocation->size - size)
      return true;
  }
  return false;
}
static void reset_saved_dos_buffers(void) {
  static const struct {
    u32 address, size;
  } ranges[] = {{PT_DOS_ARENA_BASE, PT_DOS_CONVENTIONAL_CAPACITY}, {PT_DOS_UPPER_BASE, PT_DOS_UPPER_CAPACITY}};
  for (unsigned i = 0; i < PT_DOS_POOL_COUNT; ++i) {
    PTSavedBufferOwner *owner = &saved_dos_buffers[i];
    owner->native = pt_dos_memory_storage((PTDOSPool)i);
    owner->address = ranges[i].address;
    owner->wire = pt_buffer_from_guest(owner->address);
    owner->size = ranges[i].size;
    if (!owner->previous_native)
      owner->previous_native = malloc(owner->size);
    if (!owner->previous_saved)
      owner->previous_saved = malloc(owner->size);
    if (!owner->previous_native || !owner->previous_saved)
      abort();
    const u8 *saved = owner->wire;
    memcpy(owner->native, saved, owner->size);
    memcpy(owner->previous_native, saved, owner->size);
    memcpy(owner->previous_saved, saved, owner->size);
  }
  memset(saved_dos_owned, 0, sizeof(saved_dos_owned));
  memset(saved_dos_owned[PT_DOS_CONVENTIONAL], 1, PT_DOS_CONVENTIONAL_CAPACITY / 16);
  saved_dos_buffers_active = true;
}
static void mark_saved_dos_range(u8 owned[PT_DOS_POOL_COUNT][PT_DOS_UPPER_CAPACITY / 16], u32 address, u32 size) {
  for (unsigned i = 0; i < PT_DOS_POOL_COUNT; ++i) {
    const PTSavedBufferOwner *owner = &saved_dos_buffers[i];
    if (address < owner->address || address - owner->address > owner->size ||
        size > owner->size - (address - owner->address))
      continue;
    memset(owned[i] + (address - owner->address) / 16, 1, size / 16);
    return;
  }
  abort();
}
static void bind_saved_dos_ownership(const PTDOSMemoryPlatform *state) {
  u8 owned[PT_DOS_POOL_COUNT][PT_DOS_UPPER_CAPACITY / 16] = {{0}};
  mark_saved_dos_range(owned, state->arena_base, state->arena_end - state->arena_base);
  for (unsigned i = 0; i < state->count; ++i)
    mark_saved_dos_range(owned, state->allocations[i].address, state->allocations[i].size);
  for (unsigned i = 0; i < PT_DOS_POOL_COUNT; ++i) {
    PTSavedBufferOwner *owner = &saved_dos_buffers[i];
    const u8 *saved = owner->wire;
    for (u32 paragraph = 0; paragraph < owner->size / 16; ++paragraph) {
      if (!owned[i][paragraph] || saved_dos_owned[i][paragraph])
        continue;
      u32 byte = paragraph * 16;
      memcpy(owner->native + byte, saved + byte, 16);
      memcpy(owner->previous_native + byte, saved + byte, 16);
      memcpy(owner->previous_saved + byte, saved + byte, 16);
    }
  }
  memcpy(saved_dos_owned, owned, sizeof(saved_dos_owned));
}
// The allocator's byte arena is native. Only this boundary maps its stable
// saved identities; gameplay sees native buffers and pool-relative positions.

static PTSavedBufferOwner saved_heap_buffer;
static bool saved_heap_buffer_active;
_Static_assert(PT_HEAP_PLATFORM_CAPACITY == PT_STACK_BASE - PT_HEAP_ARENA_BASE, "heap platform capacity");
static void reset_saved_heap_buffer(void) {
  PTSavedBufferOwner *owner = &saved_heap_buffer;
  if (!owner->native)
    owner->native = calloc(PT_HEAP_PLATFORM_CAPACITY + 16u, 1);
  if (!owner->native)
    abort();
  owner->address = PT_HEAP_ARENA_BASE;
  owner->wire = pt_buffer_from_guest(owner->address);
  owner->size = PT_HEAP_PLATFORM_CAPACITY;
  if (!owner->previous_native)
    owner->previous_native = malloc(owner->size);
  if (!owner->previous_saved)
    owner->previous_saved = malloc(owner->size);
  if (!owner->previous_native || !owner->previous_saved)
    abort();
  const u8 *wire = owner->wire;
  memcpy(owner->native, wire, owner->size);
  memcpy(owner->previous_native, wire, owner->size);
  memcpy(owner->previous_saved, wire, owner->size);
  saved_heap_buffer_active = true;
}
static bool saved_heap_native_span(const void *pointer, size_t size, u32 *address) {
  if (!saved_heap_buffer_active)
    return false;
  uintptr_t offset = (uintptr_t)pointer - (uintptr_t)saved_heap_buffer.native;
  if (offset > saved_heap_buffer.size || size > saved_heap_buffer.size - offset)
    return false;
  *address = saved_heap_buffer.address + (u32)offset;
  return true;
}
u8 *pt_saved_heap_storage(u32 address) {
  if (!address)
    return NULL;
  if (!saved_heap_buffer_active)
    abort();
  if (address >= saved_heap_buffer.address && address - saved_heap_buffer.address <= saved_heap_buffer.size)
    return saved_heap_buffer.native + (address - saved_heap_buffer.address);
  return saved_dos_storage(address);
}

static void commit_saved_heap_store(const void *storage, size_t size) {
  if (!saved_heap_buffer_active)
    return;
  PTSavedBufferOwner *owner = &saved_heap_buffer;
  uintptr_t offset = (uintptr_t)storage - (uintptr_t)owner->wire;
  if (offset >= owner->size)
    return;
  if (size > owner->size - offset)
    abort();
  memcpy(owner->native + offset, storage, size);
  memcpy(owner->previous_native + offset, storage, size);
  memcpy(owner->previous_saved + offset, storage, size);
}

static void require_saved_buffer_range(const PTSavedBufferOwner *owner, size_t offset, size_t size) {
  if (offset > owner->size || size > owner->size - offset)
    abort();
  u32 fault =
      saved_bytes_difference(owner->wire + offset, owner->previous_saved + offset, owner->address + (u32)offset, size);
  if (fault) {
    fprintf(stderr, "Pizza Tycoon: selected buffer image changed independently at %08x\n", fault);
    abort();
  }
}
static void commit_saved_dos_store(const void *storage, size_t size) {
  if (!saved_dos_buffers_active)
    return;
  uintptr_t first = (uintptr_t)storage;
  for (unsigned i = 0; i < PT_DOS_POOL_COUNT; ++i) {
    PTSavedBufferOwner *owner = &saved_dos_buffers[i];
    uintptr_t base = (uintptr_t)owner->wire;
    if (first < base || first - base >= owner->size)
      continue;
    size_t byte = first - base;
    if (size > owner->size - byte)
      abort();
    for (size_t offset = 0; offset < size;) {
      size_t count = 16 - ((byte + offset) & 15u);
      if (count > size - offset)
        count = size - offset;
      if (saved_dos_owned[i][(byte + offset) / 16]) {
        const u8 *source = (const u8 *)storage + offset;
        memcpy(owner->native + byte + offset, source, count);
        memcpy(owner->previous_native + byte + offset, source, count);
        memcpy(owner->previous_saved + byte + offset, source, count);
      }
      offset += count;
    }
    return;
  }
}
static void publish_saved_buffer_span(PTSavedBufferOwner *owner, size_t offset, size_t size) {
  u8 *saved = owner->wire + offset;
  memcpy(saved, owner->native + offset, size);
  memcpy(owner->previous_native + offset, owner->native + offset, size);
  memcpy(owner->previous_saved + offset, saved, size);
  commit_saved_heap_store(saved, size);
  commit_saved_dos_store(saved, size);
  pt_world_objects_written(saved, size);
}
// An explicit write also owns bytes whose value equals the native seed.

static bool saved_buffer_native_store(const void *storage, size_t size) {
  u32 heap_address;
  if (saved_heap_native_span(storage, size, &heap_address)) {
    saved_heap_bytes_written(storage, size);
    return true;
  }
  if (saved_dos_buffers_active)
    for (unsigned i = 0; i < PT_DOS_POOL_COUNT; ++i) {
      PTSavedBufferOwner *owner = &saved_dos_buffers[i];
      uintptr_t byte = (uintptr_t)storage - (uintptr_t)owner->native;
      if (byte >= owner->size)
        continue;
      if (size > owner->size - byte || !pt_dos_memory_contains(storage, (u32)size))
        abort();
      require_saved_buffer_range(owner, byte, size);
      publish_saved_buffer_span(owner, byte, size);
      return true;
    }
  uintptr_t begin = (uintptr_t)storage;
  for (PTSavedBufferOwner *owner = saved_buffers; owner; owner = owner->next) {
    uintptr_t offset = begin - (uintptr_t)owner->native;
    if (offset >= owner->size)
      continue;
    if (size > owner->size - offset) {
      fputs("Pizza Tycoon: native buffer write exceeds its allocation\n", stderr);
      abort();
    }
    require_saved_buffer_range(owner, 0, owner->size);
    publish_saved_buffer_span(owner, offset, size);
    return true;
  }
  return false;
}
static void saved_buffers_written(const void *storage, size_t size) {
  // A generic notification is not permission to refresh native bytes from
  // the saved image. Typed codec writes use commit_saved_buffer_store.

  if (size)
    (void)saved_buffer_native_store(storage, size);
}

bool pt_saved_buffer_span(const void *pointer, size_t size, guest_ptr32 *address) {
  if (saved_global_span(pointer, size, address))
    return true;
  uintptr_t at = (uintptr_t)pointer;
  // Prefer a concrete allocation over another buffer's one-past identity.

  for (unsigned interior = 0; interior < 2; ++interior)
    for (PTSavedBufferOwner *owner = saved_buffers; owner; owner = owner->next) {
      uintptr_t base = (uintptr_t)owner->native;
      if ((interior == 0 && at != base) || at < base || at - base > owner->size)
        continue;
      size_t offset = at - base;
      if (size > owner->size - offset)
        continue;
      *address = owner->address + (guest_ptr32)offset;
      return true;
    }
  if (!size && saved_heap_native_span(pointer, 0, address))
    return true;
  return saved_dos_native_span(pointer, size, address, size != 0);
}
static u32 saved_buffer_capacity(const u8 *pointer) {
  if (!pointer)
    return 0;
  uintptr_t at = (uintptr_t)pointer;
  for (unsigned pass = 0; pass < 2; ++pass)
    for (size_t i = 0; i < sizeof(saved_global_values) / sizeof(*saved_global_values); ++i) {
      const PTSavedGlobalValue *field = &saved_global_values[i];
      uintptr_t base = (uintptr_t)field->native;
      if (at < base || at - base > field->size || (!pass && at - base == field->size))
        continue;
      return (u32)(field->size - (at - base));
    }
  for (unsigned pass = 0; pass < 2; ++pass)
    for (PTSavedBufferOwner *buffer = saved_buffers; buffer; buffer = buffer->next) {
      uintptr_t base = (uintptr_t)buffer->native;
      if (at < base || at - base > buffer->size || (!pass && at - base == buffer->size))
        continue;
      return buffer->size - (u32)(at - base);
    }
  u32 dos_address;
  if (saved_dos_native_span(pointer, 0, &dos_address, false)) {
    for (PTSavedBufferOwner *primary = saved_buffers; primary; primary = primary->next)
      if (dos_address >= primary->address && dos_address - primary->address < primary->size)
        return 0;
  }
  const PTDOSAllocator *dos = pt_dos_memory_state();
  for (unsigned pass = 0; pass < 2; ++pass)
    for (unsigned i = 0; i < dos->count; ++i) {
      const PTPlatformAllocation *allocation = &dos->allocations[i];
      uintptr_t base = (uintptr_t)allocation->buffer;
      if (at < base || at - base > allocation->size || (!pass && at - base == allocation->size))
        continue;
      return allocation->size - (u32)(at - base);
    }
  return 0;
}

// Resolve owned storage without constructing a pointer for unknown words.
// Asset headers may retain obsolete pointer bytes until native relocation.

static inline bool saved_buffer_decode_owned(guest_ptr32 address, void **pointer) {
  if (!address) {
    *pointer = NULL;
    return true;
  }
  void *global = saved_global_decode(address, 0);
  if (global) {
    *pointer = global;
    return true;
  }
  for (unsigned interior = 0; interior < 2; ++interior)
    for (PTSavedBufferOwner *owner = saved_buffers; owner; owner = owner->next) {
      if (interior == 0 && address != owner->address)
        continue;
      u64 offset = (u64)address - owner->address;
      if (offset <= owner->size) {
        *pointer = owner->native + offset;
        return true;
      }
    }
  // Platform DMA/file allocations own only their live payload extent.
  // The rest of the serialized arena remains an unknown buffer identity.

  const PTDOSAllocator *dos = pt_dos_memory_state();
  for (unsigned i = 0; i < dos->count; ++i) {
    const PTPlatformAllocation *allocation = &dos->allocations[i];
    u32 native_address;
    if (!saved_dos_native_span(allocation->buffer, 0, &native_address, false))
      continue;
    if (native_address >= PT_MEMORY_SIZE || address < native_address)
      continue;
    size_t offset = address - native_address;
    if (offset <= allocation->size) {
      *pointer = allocation->buffer + offset;
      return true;
    }
  }
  return false;
}

// must match their opaque identity; a live owner always takes precedence.

static bool saved_buffer_identity_matches(guest_ptr32 address, const void *pointer) {
  void *owned;
  if (saved_buffer_decode_owned(address, &owned))
    return pointer == owned;
  return false;
}
u8 *pt_saved_buffer_decode(guest_ptr32 address) {
  void *pointer;
  if (saved_buffer_decode_owned(address, &pointer))
    return pointer;
  fprintf(stderr, "Pizza Tycoon: saved buffer has no native owner at %08x\n", address);
  abort();
}
bool pt_saved_buffer_identity(const void *pointer, guest_ptr32 *address) {
  return pt_saved_buffer_span(pointer, 1, address);
}
bool pt_saved_pointer_identity(const void *pointer, guest_ptr32 *address) {
  return pt_saved_buffer_identity(pointer, address) || pt_saved_resource_object_pointer(pointer, address) ||
      pt_saved_resource_pointer(pointer, address) || pt_saved_buffer_pointer(pointer, address);
}

bool pt_saved_buffer_pointer(const void *pointer, guest_ptr32 *address) {
  return pt_saved_buffer_span(pointer, 0, address);
}
// A known typed codec store may overlay opaque file bytes (BOB headers).
// Validate mirrors before the store, then commit only its exact written span
// to both representations. Arbitrary saved-image edits are never imported.

static void commit_saved_buffer_store(const void *storage, size_t size) {
  uintptr_t first = (uintptr_t)storage;
  for (PTSavedBufferOwner *owner = saved_buffers; owner; owner = owner->next) {
    uintptr_t base = (uintptr_t)owner->wire;
    if (first < base || first - base >= owner->size)
      continue;
    size_t offset = first - base;
    if (size > owner->size - offset)
      abort();
    memcpy(owner->native + offset, storage, size);
    memcpy(owner->previous_native + offset, storage, size);
    memcpy(owner->previous_saved + offset, storage, size);
  }
  commit_saved_heap_store(storage, size);
  commit_saved_dos_store(storage, size);
}
// Pointer words can arrive a byte at a time. Commit only the announced
// intersection, including while their native pointer is still unresolved.

static void retire_native_buffer_aliases(const PTSavedBufferOwner *);
static void retire_saved_resource_buffer(const PTSavedBufferOwner *);

static bool bind_saved_buffer_as(void *native, u32 size, u32 reserved, u32 flags) {
  PTSavedBufferOwner *owner = calloc(1, sizeof(*owner));
  if (!owner)
    abort();
  owner->previous_native = malloc(size);
  owner->previous_saved = malloc(size);
  if (!owner->previous_native || !owner->previous_saved)
    abort();
  u8 *saved = allocate_saved_storage(reserved, flags);
  if (!saved) {
    free(owner->previous_native);
    free(owner->previous_saved);
    free(owner);
    return false;
  }
  owner->address = pt_buffer_to_guest(saved);
  owner->wire = pt_buffer_from_guest(owner->address);
  owner->native = native;
  owner->size = size;
  memcpy(owner->previous_native, native, size);
  memcpy(owner->previous_saved, saved, size);
  owner->next = saved_buffers;
  saved_buffers = owner;
  return true;
}
static bool bind_saved_buffer(void *native, u32 size, bool clear) {
  return bind_saved_buffer_as(native, size, size, clear ? 1u : 0u);
}

static void retire_saved_search_buffer(const void *storage, size_t size) {
  PTDosSearchState state;
  if (!pt_dos_search_export_state(&state))
    abort();
  bool changed = false;
  if (state.dta && (uintptr_t)state.dta - (uintptr_t)storage < size) {
    state.dta = NULL;
    changed = true;
  }
  if (state.fixed_dta && (uintptr_t)state.fixed_dta - (uintptr_t)storage < size) {
    state.fixed_dta = NULL;
    changed = true;
  }
  if (changed && !pt_dos_search_load_state(&state))
    abort();
}
static void release_saved_buffer(void *native) {
  PTSavedBufferOwner **link = &saved_buffers;
  while (*link && (*link)->native != native)
    link = &(*link)->next;
  if (!*link) {
    fputs("Release of an unknown native byte buffer\n", stderr);
    abort();
  }
  PTSavedBufferOwner *owner = *link;
  retire_saved_search_buffer(owner->native, owner->size);
  pt_file_stream_buffer_retired(owner->native, owner->size);
  retire_saved_resource_buffer(owner);
  pt_graphics_retire_buffer(owner->native, owner->size);
  retire_native_buffer_aliases(owner);
  *link = owner->next;
  release_saved_storage(pt_saved_heap_storage(owner->address));
  free(owner->native);
  free(owner->previous_native);
  free(owner->previous_saved);
  free(owner);
}
static void reset_saved_buffers(void) {
  while (saved_buffers) {
    PTSavedBufferOwner *owner = saved_buffers;
    saved_buffers = owner->next;
    free(owner->native);
    free(owner->previous_native);
    free(owner->previous_saved);
    free(owner);
  }
}

static void saved_audio_released(const void *, size_t);
static void serialize_io_range(u32, size_t);
static void serialize_resource_range(u32, size_t);

bool pt_fm_hardware_load_platform(const PTFMHardwareState *state) {
  if (!state || state->index > UINT8_MAX ||
      (state->address_port && state->address_port != 0x220 && state->address_port != 0x228 &&
          state->address_port != 0x388))
    return false;
  PTFMOutputState output = {.enabled = state->address_port != 0};
  memcpy(output.registers, state->registers, sizeof(output.registers));
  bool loaded = pt_fm_output_restore(&output);
  return loaded;
}

bool pt_sound_timer_load_platform(const PTSoundTimerState *state) {
  if (!state || state->vector_selector != 8 || state->latched > UINT16_MAX || state->read_phase > 1 ||
      state->write_phase > 2 || state->write_low > UINT8_MAX ||
      (state->vector_offset != 0 && state->vector_offset != 1))
    return false;
  PTSoundTimer native = {.reload = state->reload,
    .counter = state->counter,
    .handler = state->vector_offset == 0 ? pt_sound_timer_bios_handler() : pt_sound_timer_callback_handler(),
    .previous_handler = pt_sound_timer_state()->previous_handler};
  bool loaded = pt_sound_timer_restore(&native);
  return loaded;
}

bool pt_directory_load_platform(const PTDirectoryPlatform *image) {
  if (!image)
    return false;
  PTDirectoryState state = {.getcwd_error = image->getcwd_error};
  memcpy(state.cwd, image->cwd, sizeof(state.cwd));
  return pt_directory_restore(&state);
}

bool pt_mouse_load_platform(const PTMousePlatform *state) {
  if (!state || (state->callback && state->callback != PT_MOUSE_EVENT_CALLBACK))
    return false;
  PTMouseState native = {.x = state->x,
    .y = state->y,
    .buttons = state->buttons,
    .callback = state->callback ? MouseInterrupt : NULL,
    .ready = true};
  return pt_mouse_restore(&native);
}
typedef struct PTFMImportField {
  size_t native, saved, size;
  int buffer;
} PTFMImportField;
#define FM_IMPORT_VALUE(member)                                                                                        \
  {offsetof(PTFMResident, member), offsetof(PTSavedFMResident, member), sizeof(((PTSavedFMResident *)0)->member), -1}
static const PTFMImportField fm_import_fields[] = {
  FM_IMPORT_VALUE(version),
  FM_IMPORT_VALUE(opl_port),
  FM_IMPORT_VALUE(flags),
  FM_IMPORT_VALUE(address_delay),
  FM_IMPORT_VALUE(data_delay),
  FM_IMPORT_VALUE(restore_period),
  FM_IMPORT_VALUE(period),
  FM_IMPORT_VALUE(current_period),
  FM_IMPORT_VALUE(bios_remainder),
  FM_IMPORT_VALUE(counter),
  FM_IMPORT_VALUE(event_count),
  FM_IMPORT_VALUE(control),
  FM_IMPORT_VALUE(instrument_count),
  FM_IMPORT_VALUE(playing),
  FM_IMPORT_VALUE(event_note),
  FM_IMPORT_VALUE(event_velocity),
  FM_IMPORT_VALUE(busy),
  FM_IMPORT_VALUE(programs),
  FM_IMPORT_VALUE(pitch_offsets),
  FM_IMPORT_VALUE(enabled_channels),
  FM_IMPORT_VALUE(channel_pairs),
  FM_IMPORT_VALUE(volume_pair),
  FM_IMPORT_VALUE(voice_count.word),
  FM_IMPORT_VALUE(voice_count.low),
  FM_IMPORT_VALUE(voice_count.high),
  FM_IMPORT_VALUE(channel.word),
  FM_IMPORT_VALUE(channel.low),
  FM_IMPORT_VALUE(channel.high),
  FM_IMPORT_VALUE(event_kind.word),
  FM_IMPORT_VALUE(event_kind.low),
  FM_IMPORT_VALUE(event_kind.high),
  FM_IMPORT_VALUE(rhythm.word),
  FM_IMPORT_VALUE(rhythm.low),
  FM_IMPORT_VALUE(rhythm.high),
  FM_IMPORT_VALUE(delta.value),
  FM_IMPORT_VALUE(delta.low),
  FM_IMPORT_VALUE(delta.high),
  FM_IMPORT_VALUE(fade.current),
  FM_IMPORT_VALUE(fade.target),
  FM_IMPORT_VALUE(fade.counter),
  FM_IMPORT_VALUE(fade.period),
  FM_IMPORT_VALUE(fade.step),
  FM_IMPORT_VALUE(voices.channels),
  FM_IMPORT_VALUE(voices.notes),
  FM_IMPORT_VALUE(voices.keys),
  FM_IMPORT_VALUE(voices.levels),
  FM_IMPORT_VALUE(voices.volumes),
  FM_IMPORT_VALUE(voices.original_volumes),
  FM_IMPORT_VALUE(voices.frequencies),
  FM_IMPORT_VALUE(voices.ages),
  {offsetof(PTFMResident, instruments),
    offsetof(PTSavedFMResident, serialized_instruments),
    sizeof(((PTSavedFMResident *)0)->serialized_instruments),
    PT_FM_BUFFER_INSTRUMENTS},
  {offsetof(PTFMResident, stream),
    offsetof(PTSavedFMResident, serialized_stream),
    sizeof(((PTSavedFMResident *)0)->serialized_stream),
    PT_FM_BUFFER_STREAM},
  {offsetof(PTFMResident, position),
    offsetof(PTSavedFMResident, serialized_position),
    sizeof(((PTSavedFMResident *)0)->serialized_position),
    PT_FM_BUFFER_POSITION},
  {offsetof(PTFMResident, stream_start),
    offsetof(PTSavedFMResident, serialized_stream_start),
    sizeof(((PTSavedFMResident *)0)->serialized_stream_start),
    PT_FM_BUFFER_START},
  {offsetof(PTFMResident, status),
    offsetof(PTSavedFMResident, serialized_status),
    sizeof(((PTSavedFMResident *)0)->serialized_status),
    PT_FM_BUFFER_STATUS},
};
#undef FM_IMPORT_VALUE

enum { PT_FM_STARTUP_DEVICES = 65 };
static struct {
  const u8 *images[PT_FM_STARTUP_DEVICES];
  PTFMResident devices[PT_FM_STARTUP_DEVICES];
  unsigned count;
} fm_startup;

static PTFMBuffer import_fm_startup_buffer(PTFMFarPointer words, const u8 *image, const PTFMResident *driver) {
  u32 base = (u32)words.segment * 16u;
  u32 defaults = PT_FM_version + offsetof(PTSavedFMResident, original_instruments);
  if (base == pt_buffer_to_guest(image) && words.offset == defaults)
    return driver->default_instruments;
  const u8 *bytes = words.segment ? pt_saved_buffer_decode(base) : pt_buffer_from_guest(0);
  return (PTFMBuffer){bytes, words.offset, saved_buffer_capacity(bytes)};
}
static PTFMResident *import_fm_startup_device(u8 *image) {
  for (unsigned i = 0; i < fm_startup.count; ++i)
    if (fm_startup.images[i] == image)
      return &fm_startup.devices[i];
  if (fm_startup.count == PT_FM_STARTUP_DEVICES ||
      !pt_saved_buffer_is_mapped(image, PT_FM_version + sizeof(PTSavedFMResident)))
    return NULL;
  PTFMResident *driver = &fm_startup.devices[fm_startup.count];
  *driver = (PTFMResident){.default_instruments = {pt_fm_default_instruments, 0, sizeof(pt_fm_default_instruments)}};
  const PTSavedFMResident *saved = (const PTSavedFMResident *)(const void *)(image + PT_FM_version);
  for (size_t i = 0; i < sizeof(fm_import_fields) / sizeof(*fm_import_fields); ++i) {
    const PTFMImportField *field = &fm_import_fields[i];
    void *target = (u8 *)driver + field->native;
    const void *source = (const u8 *)saved + field->saved;
    if (field->buffer < 0)
      memcpy(target, source, field->size);
    else {
      PTFMFarPointer words;
      memcpy(&words, source, sizeof(words));
      *(PTFMBuffer *)target = import_fm_startup_buffer(words, image, driver);
    }
  }
  fm_startup.images[fm_startup.count++] = image;
  return driver;
}

// Driver signature tags and real-mode words are decoded only on import or audit.

static bool saved_sound_endpoint(PTSoundFarPointer vector, PTSoundEndpointKind *kind) {
  *kind = PT_SOUND_ENDPOINT_NONE;
  if (!vector.encoded)
    return true;
  u32 address = (u32)vector.segment * 16u + vector.offset - 6u;
  if (address >= PT_HEAP_ARENA_BASE || address + 4u > PT_HEAP_ARENA_BASE)
    return false;
  const u8 *signature = pt_buffer_from_guest(address);
  *kind = !memcmp(signature, "KERN", 4) ? PT_SOUND_ENDPOINT_PCM
      : !memcmp(signature, "MIDI", 4)   ? PT_SOUND_ENDPOINT_MIDI
                                        : PT_SOUND_ENDPOINT_OTHER;
  return true;
}
bool pt_saved_sound_endpoint_kind(PTSoundEndpointKind *kind, PTSoundEndpointKind *previous) {
  *previous = PT_SOUND_ENDPOINT_NONE;
  if (!saved_sound_endpoint(pt_saved_real_vectors()->handlers[PT_SOUND_INTERRUPT], kind))
    return false;
  if (*kind == PT_SOUND_ENDPOINT_PCM && pt_sound_resources.driver) {
    guest_ptr32 image_address;
    if (!pt_saved_buffer_span(pt_sound_resources.driver, sizeof(PTSavedDigpakResident), &image_address) &&
        !pt_saved_buffer_is_mapped(pt_sound_resources.driver, sizeof(PTSavedDigpakResident)))
      return false;
    const PTSavedDigpakResident *image =
        (const PTSavedDigpakResident *)(const void *)pt_saved_data_buffer(pt_sound_resources.driver, sizeof(*image));
    PTSoundFarPointer prior = {.encoded = image->playback.previous_vector};
    if (!saved_sound_endpoint(prior, previous))
      return false;
  }
  return true;
}
bool pt_saved_native_sound_endpoint_restore(void) {
  PTSoundEndpointKind kind, previous;
  if (!pt_saved_sound_endpoint_kind(&kind, &previous))
    return false;
  pt_sound_endpoint_bind(kind, previous);
  return true;
}

bool pt_saved_native_fm_devices_restore(void) {
  PTFMResident *devices[64] = {0}, *timer = NULL;
  const PTSavedRealVectors *vectors = pt_saved_real_vectors();
  for (unsigned slot = 0; slot < 64; ++slot) {
    PTSoundFarPointer vector = vectors->handlers[slot + 128];
    u32 signature = (u32)((i32)(i16)vector.segment * 16) + 0x103u;
    if (!pt_valid_platform_range(signature, 5))
      return false;
    if (!memcmp(pt_buffer_from_guest(signature), "FMDRV", 5)) {
      u8 *base = pt_buffer_from_guest((u32)vector.segment * 16u);
      if (vector.offset != PT_FM_interrupt_entry ||
          !saved_dos_word_contains(pt_buffer_to_guest(base) + 0x100, 0x17ae - 0x100))
        return false;
      devices[slot] = import_fm_startup_device(base);
      if (!devices[slot])
        return false;
    }
  }
  PTSoundFarPointer timer_vector = vectors->handlers[8];
  if (timer_vector.offset == PT_FM_timer_entry) {
    u8 *base = pt_buffer_from_guest((u32)timer_vector.segment * 16u);
    if (!saved_dos_word_contains(pt_buffer_to_guest(base) + 0x100, 0x17ae - 0x100) ||
        memcmp(pt_fm_image_tag(base)->text, "FMDRV", 6))
      return false;
    timer = import_fm_startup_device(base);
    if (!timer)
      return false;
  }
  pt_fm_devices_reset();
  for (unsigned slot = 0; slot < 64; ++slot)
    pt_fm_device_bind((u8)(slot + 128), devices[slot]);
  pt_fm_timer_bind(timer);
  return true;
}
void pt_native_fm_storage_reset(void) {
  fm_startup.count = 0;
  memset(fm_startup.images, 0, sizeof(fm_startup.images));
  memset(fm_startup.devices, 0, sizeof(fm_startup.devices));
  pt_fm_devices_reset();
  pt_heap_allocation_observer(saved_audio_released);
}

guest_ptr32 pt_pcm_real_address(u32 packed) {
  return (packed >> 16) * 16u + (packed & 65535u);
}
// Allocation retirement ends packet ownership; the mapped bytes still

static void saved_audio_released(const void *storage, size_t size) {
  retire_saved_search_buffer(storage, size);
}

static PTPCMSampleBuffer saved_pcm_samples(u32 address) {
  return (PTPCMSampleBuffer){pt_saved_buffer_decode(address)};
}
static const u32 saved_pcm_operations[] = {
  0, 0x689, 0x68c, 0x6a0, 0x68a, 0x696, 0x69d, 0x69c, 0x698, 0x69e, 0x688, 0x695};
static bool saved_pcm_operation_valid(u32 word) {
  for (unsigned i = 0; i < sizeof(saved_pcm_operations) / sizeof(*saved_pcm_operations); ++i)
    if (saved_pcm_operations[i] == word)
      return true;
  return false;
}

bool pt_pcm_driver_load_platform(const PTPCMDriverState *state) {
  if (!state || pt_frame_active() || state->installed > 1 || state->active > 1 || state->queued > 1 ||
      state->reserved ||
      (state->transport_reserved && ((state->transport_reserved & 0xfeu) || !(state->transport_reserved & 1u))) ||
      state->current_position > state->current_length || state->block_position > state->block_size ||
      !saved_pcm_operation_valid(state->last_service))
    return false;
  PTPCMPlaybackState native = {.installed = state->installed,
    .active = state->active,
    .queued = state->queued,
    .current_length = state->current_length,
    .current_frequency = state->current_frequency,
    .current_position = state->current_position,
    .block_size = state->block_size,
    .block_position = state->block_position,
    .current_samples = saved_pcm_samples(state->current_buffer),
    .queued_samples = saved_pcm_samples(pt_pcm_real_address(state->queued_buffer)),
    .queued_length = pt_pcm_buffer_length(state->queued_length_fields),
    .queued_frequency = state->queued_frequency_fields >> 16};
  pt_pcm_driver_restore(&native);
  return true;
}

bool pt_named_global_overlaps(guest_ptr32 address, size_t size) {
  static const struct {
    guest_ptr32 address;
    size_t size;
  } ranges[] = {PT_NAMED_GLOBAL_RANGES};
  for (unsigned i = 0; i < sizeof(ranges) / sizeof(*ranges); ++i)
    if (size && (u64)address < (u64)ranges[i].address + ranges[i].size && (u64)ranges[i].address < (u64)address + size)
      return true;
  return false;
}

// Platform snapshots are wire DTOs; live allocation policy uses native buffers.

void pt_saved_heap_platform_reset(void) {
  reset_saved_identities();
  pt_dos_memory_reset();
  reset_saved_heap_buffer();
  reset_saved_dos_buffers();
  pt_dos_memory_reset();
}
bool pt_dos_memory_load_platform(const PTDOSMemoryPlatform *initial) {
  if (!initial)
    return false;
  bool lower = initial->arena_base >= PT_DOS_ARENA_BASE && initial->arena_end <= PT_DOS_ARENA_END;
  bool upper = initial->arena_base >= PT_DOS_UPPER_BASE && initial->arena_end <= PT_DOS_UPPER_END;
  if ((!lower && !upper) || pt_named_global_overlaps(initial->arena_base, initial->arena_end - initial->arena_base) ||
      initial->arena_base >= initial->arena_end || ((initial->arena_base | initial->arena_end) & 15u) ||
      initial->next_selector < 0x100 || initial->next_selector > 0x10000 || (initial->next_selector & 7u) ||
      initial->count > PT_DOS_BLOCKS)
    return false;
  for (unsigned i = 0; i < initial->count; ++i) {
    const PTHeapAllocation *a = &initial->allocations[i];
    bool in_arena = a->address >= initial->arena_base && (u64)a->address + a->size <= initial->arena_end;
    // A DOS UMB allocation policy can retain a driver already installed
    // in the conventional gap. These blocks still own their payloads.

    bool conventional_resident =
        upper && a->address >= PT_DOS_ARENA_BASE && (u64)a->address + a->size <= PT_DOS_ARENA_END;
    if ((!in_arena && !conventional_resident) || !a->size || ((a->address | a->size) & 15u) || a->handle < 0x100 ||
        a->handle >= initial->next_selector || (a->handle & 7u))
      return false;
    for (unsigned j = 0; j < i; ++j) {
      const PTHeapAllocation *b = &initial->allocations[j];
      if (a->handle == b->handle ||
          ((u64)a->address < (u64)b->address + b->size && (u64)b->address < (u64)a->address + a->size))
        return false;
    }
  }
  if (!saved_dos_buffers_active)
    reset_saved_dos_buffers();
  bind_saved_dos_ownership(initial);
  PTDOSAllocator state = {.begin = saved_dos_storage(initial->arena_base),
    .end = saved_dos_storage(initial->arena_end),
    .next_selector = initial->next_selector,
    .count = initial->count};
  for (unsigned i = 0; i < PT_DOS_BLOCKS; ++i) {
    const PTHeapAllocation *a = &initial->allocations[i];
    u8 *buffer = i < initial->count ? saved_dos_storage(a->address) : pt_optional_buffer_from_guest(a->address);
    state.allocations[i] = (PTPlatformAllocation){buffer, a->size, a->handle};
  }
  pt_dos_memory_restore(&state);
  // Cold imports precede the platform allocation list. Resolve their
  // borrowed byte aliases once the actual native payloads are available.

  return true;
}
bool pt_heap_load_platform(const PTHeapPlatform *initial) {
  if (!initial || initial->count > PT_HEAP_PLATFORM_ALLOCATIONS || !initial->next_handle ||
      initial->reuse_released > 1 || initial->free_count > PT_HEAP_PLATFORM_ALLOCATIONS + 1 ||
      (!initial->reuse_released && initial->free_count) || initial->next_address < PT_HEAP_ARENA_BASE ||
      initial->next_address >= PT_STACK_BASE || (initial->next_address & 4095u))
    return false;
  for (unsigned i = 0; i < initial->count; ++i) {
    const PTHeapAllocation *a = &initial->allocations[i];
    if (a->address < PT_HEAP_ARENA_BASE || (a->address & 4095u) || !a->size || (a->size & 4095u) ||
        (u64)a->address + a->size > initial->next_address || !a->handle || a->handle >= initial->next_handle)
      return false;
    for (unsigned j = 0; j < i; ++j) {
      const PTHeapAllocation *b = &initial->allocations[j];
      if (a->handle == b->handle ||
          ((u64)a->address < (u64)b->address + b->size && (u64)b->address < (u64)a->address + a->size))
        return false;
    }
  }
  for (unsigned i = 0; i < initial->free_count; ++i) {
    const PTHeapFreeBlock *a = &initial->free_blocks[i];
    if (a->address < PT_HEAP_ARENA_BASE || !a->size || ((a->address | a->size) & 4095u) ||
        (u64)a->address + a->size > initial->next_address ||
        (i && (u64)initial->free_blocks[i - 1].address + initial->free_blocks[i - 1].size >= a->address))
      return false;
    for (unsigned j = 0; j < initial->count; ++j) {
      const PTHeapAllocation *b = &initial->allocations[j];
      if ((u64)a->address < (u64)b->address + b->size && (u64)b->address < (u64)a->address + a->size)
        return false;
    }
  }
  if (!saved_heap_buffer_active)
    reset_saved_heap_buffer();
  return true;
}

// Saved profiles decode addresses once; live directory services retain pointers.

static bool saved_search_record(u32 address, GuestFindRecord **record) {
  if (!address) {
    *record = NULL;
    return true;
  }
  if (address == PT_SAVED_DEFAULT_FIND_RECORD) {
    *record = pt_default_find_record();
    return true;
  }
  for (PTSavedBufferOwner *owner = saved_buffers; owner; owner = owner->next) {
    if (address < owner->address)
      continue;
    u32 offset = address - owner->address;
    if (offset <= owner->size && sizeof(**record) <= owner->size - offset) {
      *record = (GuestFindRecord *)(void *)(owner->native + offset);
      return true;
    }
  }
  const PTDOSAllocator *dos = pt_dos_memory_state();
  for (unsigned i = 0; i < dos->count; ++i) {
    const PTPlatformAllocation *allocation = &dos->allocations[i];
    u32 base;
    if (!saved_dos_native_span(allocation->buffer, allocation->size, &base, true) || address < base)
      continue;
    u32 offset = address - base;
    if (offset <= allocation->size && sizeof(**record) <= allocation->size - offset) {
      *record = (GuestFindRecord *)(void *)(allocation->buffer + offset);
      return true;
    }
  }
  return false;
}
bool pt_dos_search_load_platform(const PTDosSearchPlatform *image) {
  if (!image)
    return false;
  PTDosSearchState state = {.first_error = image->first_error, .next_error = image->next_error};
  if (!saved_search_record(image->dta, &state.dta) || !saved_search_record(image->fixed_dta, &state.fixed_dta))
    return false;
  memcpy(state.network_reserved, image->network_reserved, sizeof(state.network_reserved));
  return pt_dos_search_load_state(&state);
}

// Actual pointer assignments notify this image codec immediately, preserving
// command-time alias observations without decoding ordinary runtime reads.

// Root identities are serialized only here; gameplay assigns native globals.

static void *resource_identity_only(PTResourceKind kind, guest_ptr32 address);

// Native imports resolve declared owners. Only an external image provider
// can construct an otherwise unowned record from a serialized identity.

typedef struct PTSavedNativeRoot {
  void *field;
  guest_ptr32 address;
  int kind;
  bool graphics;
} PTSavedNativeRoot;

// Canonical buffer roots must retain their native storage identity. Encoding
// and auditing this relation are passive: neither publishes pending bytes.

// Root and record pointer codecs share both directions. Keep null-word
// policy in the table: a zero file-record word can name an original record,
// while ordinary nullable roots must not ask their decoder to import it.

typedef struct PTSavedPointerCodec {
  bool (*encode)(const void *pointer, u32 *word);
  void *(*decode)(u32 word);
  bool decode_zero;
} PTSavedPointerCodec;
static bool encode_saved_buffer(const void *pointer, u32 *word) {
  if (pt_saved_pointer_identity(pointer, word) || pt_saved_buffer_span(pointer, 0, word))
    return true;
  // Spare native string capacity has no serialized identity.

  return false;
}

// These adapters convert typed return values, without casting function pointers.
// The paired codec list is the single source for both encode and decode dispatch.

#define SAVED_POINTER_CODECS(X) X(PT_ROOT_BUFFER, encode_saved_buffer, pt_saved_buffer_decode, false)

#define ROOT_DECODER(kind, encoder, decoder, decode_zero)                                                              \
  static void *decode_root_##kind(u32 word) {                                                                          \
    return decoder(word);                                                                                              \
  }
SAVED_POINTER_CODECS(ROOT_DECODER)
#undef ROOT_DECODER
static const PTSavedPointerCodec saved_root_codecs[] = {
#define ROOT_CODEC(kind, encoder, decoder, zero) [-kind] = {encoder, decode_root_##kind, zero},
  SAVED_POINTER_CODECS(ROOT_CODEC)
#undef ROOT_CODEC
};
#undef SAVED_POINTER_CODECS
static const PTSavedPointerCodec *saved_root_codec(int kind) {
  if (kind >= 0 || kind <= -(int)(sizeof(saved_root_codecs) / sizeof(*saved_root_codecs)))
    return NULL;
  const PTSavedPointerCodec *codec = &saved_root_codecs[-kind];
  return codec->decode ? codec : NULL;
}
static bool encode_native_root(const PTSavedNativeRoot *root, const void *pointer, u32 *word) {
  *word = 0;
  if (!pointer)
    return true;
  const PTSavedPointerCodec *codec = saved_root_codec(root->kind);
  if (codec)
    return codec->encode(pointer, word);
  if (root->kind < 0)
    return false;
  return pt_saved_resource_identity(pointer, (PTResourceKind)root->kind, word);
}

void pt_saved_global_roots_reset(void) {
  encoded_input_active = false;
  encoded_input = (PTMemoryRange){0};
}
static void require_saved_root(u32 fault) {
  if (fault) {
    fprintf(stderr, "Pizza Tycoon: invalid native root or unexpected saved root edit at %08x\n", fault);
    abort();
  }
}
void pt_saved_graphics_objects_bind_native_defaults(void) {
  pt_native_graphics_root_defaults_apply();
}

void pt_saved_game_objects_bind_native_defaults(void) {
  pt_native_game_root_defaults_apply();
}

void pt_saved_palette_state_restore(void) {
  pt_palette_state_reset();
}

void pt_saved_file_label_restore(void) {
  u32 filename = pt_memory.globals.saved_serialized_secondary_filename_word;
  if (!filename) {
    pt_file_label_import((const u8 *)"", 0);
    return;
  }
  const u8 *name = pt_saved_buffer_decode(filename);
  size_t capacity = saved_buffer_capacity(name);
  const u8 *end = capacity ? memchr(name, 0, capacity) : NULL;
  if (!end) {
    fputs("Pizza Tycoon: serialized filename has no bounded terminator\n", stderr);
    abort();
  }
  pt_file_label_import(name, (size_t)(end - name));
}

static void bind_native_resource_objects(void);
static void bind_native_graphics_storage(void) {
  pt_native_graphics_defaults_apply();
  bind_native_resource_objects();
  (void)pt_saved_resource_decode(PT_RESOURCE_KIND_MOUSEINITIALIZER, PT_FIELD_mouse_initializer);
  pt_graphics_pixels_import((PTGraphicsPixels){.background = {.row_stride = PT_SCREEN_WIDTH},
    .mask = {.surface = {.row_stride = PT_SCREEN_WIDTH / 4}, .repeat = 4}});
}

void pt_saved_graphics_bind_native_defaults(void) {
  bind_native_graphics_storage();
}

void pt_saved_runtime_state_restore(void) {
  pt_default_find_record_bind(NULL);
  pt_platform_clock_bind(NULL);
  PTSoundTimer timer = *pt_sound_timer_state();
  timer.previous_handler = pt_sound_timer_bios_handler();
  if (!pt_sound_timer_restore(&timer))
    abort();
  pt_runtime_state_reset();
  pt_runtime.sound_timer = pt_sound_pcm_timer_callback();
  if (!pt_runtime_initialize_file_nodes())
    abort();
}

static void bind_native_stream_state(void) {
  pt_native_file_defaults_apply();
}

void pt_saved_stream_bind_native_defaults(void) {
  bind_native_stream_state();
}

typedef struct PTSavedEnvironmentText {
  const u8 *text;
  size_t bytes;
} PTSavedEnvironmentText;
static u8 *native_environment_strings;
static _Noreturn void invalid_saved_environment(void) {
  fputs("Pizza Tycoon: invalid serialized environment table or string\n", stderr);
  abort();
}
static void import_native_environment(void) {
  u32 source = pt_saved_process_environment;
  if (!source) {
    pt_process_environment_import(NULL, 0);
    free(native_environment_strings);
    native_environment_strings = NULL;
    return;
  }
  PTSavedEnvironmentText *rows = NULL;
  size_t count = 0, capacity = 0, bytes = 0;
  for (;;) {
    u64 address = (u64)source + count * sizeof(u32);
    if (address > UINT32_MAX || !saved_object_readable((u32)address, sizeof(u32)))
      invalid_saved_environment();
    const PTSavedPointerWord *entry = (const PTSavedPointerWord *)(const void *)pt_buffer_from_guest((u32)address);
    u32 word = entry->value;
    if (!word)
      break;
    size_t length = 0;
    for (;;) {
      u64 at = (u64)word + length;
      if (at > UINT32_MAX || !saved_object_readable((u32)at, 1))
        invalid_saved_environment();
      ++length;
      if (!pt_memory.bytes[at])
        break;
    }
    if (bytes > SIZE_MAX - length)
      invalid_saved_environment();
    if (count == capacity) {
      size_t next = capacity ? capacity * 2 : 8;
      if (next < capacity || next > SIZE_MAX / sizeof(*rows))
        abort();
      PTSavedEnvironmentText *grown = realloc(rows, next * sizeof(*grown));
      if (!grown)
        abort();
      rows = grown;
      capacity = next;
    }
    rows[count++] = (PTSavedEnvironmentText){pt_buffer_from_guest(word), length};
    bytes += length;
  }
  u8 *strings = bytes ? malloc(bytes) : NULL;
  if (bytes && !strings)
    abort();
  if (count > SIZE_MAX / sizeof(const u8 *))
    abort();
  const u8 **entries = malloc((count ? count : 1) * sizeof(*entries));
  if (!entries)
    abort();
  size_t offset = 0;
  for (size_t i = 0; i < count; ++i) {
    memcpy(strings + offset, rows[i].text, rows[i].bytes);
    entries[i] = strings + offset;
    offset += rows[i].bytes;
  }
  pt_process_environment_import(entries, count);
  free(native_environment_strings);
  native_environment_strings = strings;
  free(entries);
  free(rows);
}

static void bind_native_process_tables(void) {
  pt_native_process_defaults_apply();
  free(native_environment_strings);
  native_environment_strings = NULL;
  // Seed mode histories from native defaults before enabling environment lookup.

  for (size_t i = 0; i < sizeof(saved_global_values) / sizeof(*saved_global_values); ++i) {
    const PTSavedGlobalValue *field = &saved_global_values[i];
    if (field->native != (const void *)&guest_io_modes)
      continue;
    memcpy(global_history(&previous_native_globals, field), guest_io_modes, field->size);
    memcpy(global_history(&previous_saved_globals, field), guest_io_modes, field->size);
  }
  import_native_environment();
}

void pt_saved_process_bind_native_defaults(void) {
  bind_native_process_tables();
}

// Import only bytes actually written through a saved alias.

static void import_saved_scalar(void *native, const void *saved, size_t width, const void *storage, size_t size) {
  uintptr_t first = (uintptr_t)saved, changed = (uintptr_t)storage;
  size_t offset = changed > first ? changed - first : 0;
  if (offset >= width || (first > changed && first - changed >= size))
    return;
  size_t available = first > changed ? size - (first - changed) : size;
  size_t count = width - offset < available ? width - offset : available;
  memcpy((u8 *)native + offset, (const u8 *)saved + offset, count);
}
// Native resource objects and original resource records have independent
// layouts. Field profiles describe only the saved/import boundary.

typedef struct PTSavedResourceField {
  PTResourceField field;
  size_t native_offset, saved_offset, width;
  // PTSavedFieldCodec or a nonnegative PTResourceKind.
  int target;
} PTSavedResourceField;
// Negative codec tags are format metadata; nonnegative tags name resources.
// Keep their values stable for the descriptor audit and diagnostic visitors.

typedef enum PTSavedFieldCodec {
  PT_SAVED_VALUE = -1,
  PT_SAVED_BUFFER = -2,
  PT_SAVED_FONT = -3,
  PT_SAVED_BITMAP = -4,
  PT_SAVED_PALETTE_SLOT = -5,
  PT_SAVED_JATOBA = -6,
  PT_SAVED_LINK = -7,
  PT_SAVED_CANVAS = -8,
  PT_SAVED_LINK_BASE = -100
} PTSavedFieldCodec;
#define SAVED_LINK_TO(kind) (PT_SAVED_LINK_BASE - (kind))
typedef struct PTSavedResourceSpec {
  size_t native_size, saved_size, count;
  const PTSavedResourceField *fields;
} PTSavedResourceSpec;
// One record schema supplies both directions, their registration and their
// layout checks. Native pointers are encoded only at this format boundary.

#define SAVED_RECORD(kind, Native, wire_size, fields) static const PTSavedResourceField fields[] = {
#define SAVED_VALUE(id, Record, member)                                                                                \
  {id,                                                                                                                 \
    offsetof(Record, member),                                                                                          \
    offsetof(PTSaved##Record, member),                                                                                 \
    sizeof(((PTSaved##Record *)0)->member),                                                                            \
    PT_SAVED_VALUE},
#define SAVED_REFERENCE(id, Record, member, wire_member, target)                                                       \
  {id,                                                                                                                 \
    offsetof(Record, member),                                                                                          \
    offsetof(PTSaved##Record, wire_member),                                                                            \
    sizeof(((PTSaved##Record *)0)->wire_member),                                                                       \
    target},
#define SAVED_SPAN(id, Record, native_offset, saved_offset, width)                                                     \
  {id, native_offset, saved_offset, width, PT_SAVED_VALUE},
#define SAVED_BYTES(id, Record) {id, 0, 0, sizeof(Record), PT_SAVED_VALUE},
#define SAVED_END_RECORD()                                                                                             \
  }                                                                                                                    \
  ;
#include "src/savegame_records.def"
#undef SAVED_END_RECORD
#undef SAVED_SPAN
#undef SAVED_BYTES
#undef SAVED_REFERENCE
#undef SAVED_VALUE
#undef SAVED_RECORD

// Recipe's saved extent ends at the flexible entries; their own record
// profile follows the header. Counts always come from the field tables.

#define SAVED_RECORD(kind, Native, wire_size, fields)                                                                  \
  [kind] = {sizeof(Native), wire_size, sizeof(fields) / sizeof(*(fields)), fields},
#define SAVED_VALUE(...)
#define SAVED_REFERENCE(...)
#define SAVED_SPAN(...)
#define SAVED_BYTES(...)
#define SAVED_END_RECORD()
static const PTSavedResourceSpec resource_specs[] = {
#include "src/savegame_records.def"
};
#undef SAVED_END_RECORD
#undef SAVED_SPAN
#undef SAVED_BYTES
#undef SAVED_REFERENCE
#undef SAVED_VALUE
#undef SAVED_RECORD

// Ordinary values have equal extents in both layouts. Pointer words and
// explicitly split legacy spans are checked independently of host layout.

#define SAVED_RECORD(kind, Native, wire_size, fields)
#define SAVED_VALUE(id, Record, member)                                                                                \
  _Static_assert(sizeof(((Record *)0)->member) == sizeof(((PTSaved##Record *)0)->member),                              \
      #Record "." #member " saved value extent");
#define SAVED_REFERENCE(id, Record, member, wire_member, target)                                                       \
  _Static_assert(sizeof(((Record *)0)->member) == sizeof(void *), #Record "." #member " native pointer extent");       \
  _Static_assert(                                                                                                      \
      sizeof(((PTSaved##Record *)0)->wire_member) == sizeof(u32), #Record "." #wire_member " saved pointer extent");
#define SAVED_SPAN(id, Record, native_offset, saved_offset, width)                                                     \
  _Static_assert((native_offset) + (width) <= sizeof(Record) && (saved_offset) + (width) <= sizeof(PTSaved##Record),   \
      #Record "." #id " saved span extent");
#define SAVED_BYTES(id, Record)
#define SAVED_END_RECORD()
#include "src/savegame_records.def"
#undef SAVED_END_RECORD
#undef SAVED_SPAN
#undef SAVED_BYTES
#undef SAVED_REFERENCE
#undef SAVED_VALUE
#undef SAVED_RECORD

typedef struct PTSavedResourceOwner {
  PTResourceKind kind;
  guest_ptr32 address;
  u8 *wire;
  u8 *native;
  size_t count, capacity;
  u8 *initialized, *owned, *pointer_overrides, *pending_pointers;
  u8 *published_native, *published_saved;
  size_t saved_extent, native_extent;
  bool extent_fixed;
  struct PTSavedResourceOwner *container;
  size_t *active_indices, active_count, active_capacity;
  bool allocated;
  struct PTSavedResourceOwner *next;
} PTSavedResourceOwner;
static PTSavedResourceOwner *saved_resources;
static const void *storing_resource;
static unsigned saved_reference_depth;
void pt_saved_reference_import_begin(void) {
  ++saved_reference_depth;
}
void pt_saved_reference_import_end(void) {
  if (!saved_reference_depth)
    abort();
  --saved_reference_depth;
}
void pt_saved_record_bytes_written(void *storage, size_t size) {
  pt_saved_reference_import_begin();
  pt_world_objects_written(storage, size);
  pt_saved_reference_import_end();
}
// File I/O owns these writes explicitly. Ordinary diagnostic notifications
// never authorize copying arbitrary image bytes into native byte buffers.

void pt_saved_io_bytes_written(void *storage, size_t size) {
  commit_saved_buffer_store(storage, size);
  pt_saved_record_bytes_written(storage, size);
}
void pt_saved_encoded_input_begin(const void *storage, size_t size) {
  if (encoded_input_active || (!storage && size))
    abort();
  encoded_input = (PTMemoryRange){storage, size ? (const u8 *)storage + size : storage};
  encoded_input_active = true;
}
void pt_saved_encoded_input_end(void) {
  if (!encoded_input_active)
    abort();
  encoded_input_active = false;
  encoded_input = (PTMemoryRange){0};
}
// Scalar gameplay assignments are ordinary C. The save boundary
// tracks the last imported/published bytes and projects changed native fields.
// Unexpected edits to its private saved mirror are reported, never repaired.

// Pointer histories contain encoded words only at this storage boundary.

static bool tracked_resource_pointer(PTResourceKind kind, const PTSavedResourceField *field) {
  if ((field->target == PT_SAVED_FONT && !saved_root_codec(PT_ROOT_FONT)) ||
      (field->target == PT_SAVED_BITMAP && !saved_root_codec(PT_ROOT_BITMAP)) ||
      (field->target == PT_SAVED_CANVAS && !saved_root_codec(PT_ROOT_CANVAS)) ||
      (field->target == PT_SAVED_JATOBA && !saved_root_codec(PT_ROOT_JATOBA)) ||
      (field->target == PT_SAVED_PALETTE_SLOT && !saved_root_codec(PT_ROOT_PALETTE_SLOT)))
    return false;
  static const struct {
    PTResourceKind kind;
    size_t offset;
  } fields[] = {
    {PT_RESOURCE_KIND_IGLOO_LINK, offsetof(IglooLink, first)},
    {PT_RESOURCE_KIND_IGLOO_LINK, offsetof(IglooLink, last)},
    {PT_RESOURCE_KIND_HAZELNUT, offsetof(Hazelnut, background)},
    {PT_RESOURCE_KIND_HAZELNUT, offsetof(Hazelnut, actor)},
    {PT_RESOURCE_KIND_BOB_HEADER, offsetof(BobHeader, pixels)},
    {PT_RESOURCE_KIND_LANG_FILE_DESC, offsetof(LangFileDesc, buffer)},
    {PT_RESOURCE_KIND_IGLOO, offsetof(Igloo, next)},
    {PT_RESOURCE_KIND_IGLOO, offsetof(Igloo, previous)},
    {PT_RESOURCE_KIND_IGLOO, offsetof(Igloo, pixels)},
    {PT_RESOURCE_KIND_IGLOO, offsetof(Igloo, sprite)},
    {PT_RESOURCE_KIND_BOB_HOST, offsetof(BobHost, data)},
    {PT_RESOURCE_KIND_BOB_HOST, offsetof(BobHost, next)},
    {PT_RESOURCE_KIND_BOB_HOST, offsetof(BobHost, payload)},
    {PT_RESOURCE_KIND_BOB_HOST, offsetof(BobHost, header_source)},
    {PT_RESOURCE_KIND_CONTRACT, offsetof(Contract, supplier)},
    {PT_RESOURCE_KIND_RESTORECIPE, offsetof(RestoRecipe, recipe)},
    {PT_RESOURCE_KIND_POI, offsetof(Poi, restaurant)},
    {PT_RESOURCE_KIND_PLAYER, offsetof(Player, selected_property)},
    {PT_RESOURCE_KIND_APPLE, offsetof(Apple, restaurant)},
    {PT_RESOURCE_KIND_REST, offsetof(Rest, property)},
    {PT_RESOURCE_KIND_GAME, offsetof(Game, player_table)},
    {PT_RESOURCE_KIND_GAME, offsetof(Game, character_table)},
    {PT_RESOURCE_KIND_GAME, offsetof(Game, background)},
    {PT_RESOURCE_KIND_GAME, offsetof(Game, language)},
    {PT_RESOURCE_KIND_GAME, offsetof(Game, language_storage)},
    {PT_RESOURCE_KIND_GAME, offsetof(Game, scratch_first)},
    {PT_RESOURCE_KIND_GAME, offsetof(Game, scratch_second)},
    {PT_RESOURCE_KIND_GAME, offsetof(Game, font_backup)},
    {PT_RESOURCE_KIND_GAME, offsetof(Game, palette_slots[0])},
    {PT_RESOURCE_KIND_GAME, offsetof(Game, palette_slots[1])},
    {PT_RESOURCE_KIND_GAME, offsetof(Game, read_buffer.allocation)},
    {PT_RESOURCE_KIND_GAME, offsetof(Game, read_buffer.cursor.data)},
    {PT_RESOURCE_KIND_PIG, offsetof(Pig, partner)},
    {PT_RESOURCE_KIND_PIG, offsetof(Pig, character)},
    {PT_RESOURCE_KIND_MOUSEINITIALIZER, offsetof(MouseInitializer, parent)},
    {PT_RESOURCE_KIND_MOUSE, offsetof(Mouse, parent)},
    {PT_RESOURCE_KIND_MOUSE, offsetof(Mouse, canvas)},
  };
  if (field->target <= PT_SAVED_LINK_BASE || field->target == PT_SAVED_LINK)
    return true;
  if (field->target == PT_SAVED_VALUE)
    return false;
  // All ten depot slots are byte-buffer references in the shared schema.

  if (kind == PT_RESOURCE_KIND_PLAYER && field->target == PT_SAVED_BUFFER)
    return true;
  for (size_t i = 0; i < sizeof(fields) / sizeof(*fields); ++i)
    if (fields[i].kind == kind && fields[i].offset == field->native_offset)
      return true;
  return false;
}
static bool encode_saved_link_pointer(const void *pointer, int family, u32 *word) {
  *word = 0;
  if (!pointer)
    return true;
  uintptr_t address = (uintptr_t)pointer;
  for (PTSavedResourceOwner *owner = saved_resources; owner; owner = owner->next) {
    if (owner->kind < PT_RESOURCE_KIND_CONTRACT)
      continue;
    const PTSavedResourceSpec *spec = &resource_specs[owner->kind];
    uintptr_t base = (uintptr_t)owner->native;
    if (address < base || address - base >= owner->count * spec->native_size)
      continue;
    size_t index = (address - base) / spec->native_size, local = (address - base) % spec->native_size;
    if (family >= 0 && local == 0 && (int)owner->kind == family) {
      *word = owner->address + (u32)(index * spec->saved_size);
      return true;
    }
    for (size_t i = 0; i < spec->count; ++i) {
      const PTSavedResourceField *field = &spec->fields[i];
      if (field->native_offset != local)
        continue;
      if (field->target <= PT_SAVED_LINK_BASE && (family < 0 || PT_SAVED_LINK_BASE - field->target == family)) {
        *word = owner->address + (u32)(index * spec->saved_size + field->saved_offset);
        return true;
      }
      if (field->target == PT_SAVED_LINK && family < 0) {
        *word = owner->address + (u32)(index * spec->saved_size + field->saved_offset);
        return true;
      }
    }
  }
  return false;
}
// Record fields select the same codecs as global pointer roots. The numeric
// field tags remain stable because existing files and schema audits use them.

static const int saved_pointer_root_kinds[] = {[-PT_SAVED_BUFFER] = PT_ROOT_BUFFER,
  [-PT_SAVED_FONT] = PT_ROOT_FONT,
  [-PT_SAVED_BITMAP] = PT_ROOT_BITMAP,
  [-PT_SAVED_JATOBA] = PT_ROOT_JATOBA,
  [-PT_SAVED_CANVAS] = PT_ROOT_CANVAS,
  [-PT_SAVED_PALETTE_SLOT] = PT_ROOT_PALETTE_SLOT};
static const PTSavedPointerCodec *saved_pointer_codec(int target) {
  if (target >= 0 || target <= -(int)(sizeof(saved_pointer_root_kinds) / sizeof(*saved_pointer_root_kinds)))
    return NULL;
  return saved_root_codec(saved_pointer_root_kinds[-target]);
}
static bool encode_saved_field_pointer(int target, const void *pointer, u32 *word) {
  if (target <= PT_SAVED_LINK_BASE || target == PT_SAVED_LINK)
    return encode_saved_link_pointer(pointer, target <= PT_SAVED_LINK_BASE ? PT_SAVED_LINK_BASE - target : -1, word);
  const PTSavedPointerCodec *codec = saved_pointer_codec(target);
  if (target < 0 && !codec) {
    *word = 0;
    return !pointer;
  }
  if (codec) {
    *word = 0;
    return !pointer || codec->encode(pointer, word);
  }
  const PTSavedNativeRoot root = {NULL, 0, target, false};
  return encode_native_root(&root, pointer, word);
}
static PTForwardLink *simulation_link_decode(guest_ptr32, int);
static PTForwardLink *simulation_link_identity(guest_ptr32);
static void *decode_saved_field_pointer(int target, u32 word, bool identity_only) {
  if (target <= PT_SAVED_LINK_BASE || target == PT_SAVED_LINK)
    return simulation_link_decode(word, target <= PT_SAVED_LINK_BASE ? PT_SAVED_LINK_BASE - target : -1);
  const PTSavedPointerCodec *codec = saved_pointer_codec(target);
  if (codec)
    return word || codec->decode_zero ? codec->decode(word) : NULL;
  if (target < 0) {
    if (!word)
      return NULL;
    fputs("Pizza Tycoon: invalid saved pointer codec\n", stderr);
    abort();
  }
  if (!word)
    return NULL;
  return identity_only ? resource_identity_only((PTResourceKind)target, word)
                       : pt_saved_resource_decode((PTResourceKind)target, word);
}
// Saved cursor values retain DWORD wrap without constructing an out-of-range
// host pointer. A matching numeric address cannot conceal a wrong native owner.

bool pt_saved_game_cursor_identity(const Game *owner, guest_ptr32 *word) {
  const PTBufferCursor *cursor = &owner->read_buffer.cursor;
  u32 anchor = 0;
  if (cursor->data && !pt_saved_buffer_span(cursor->data, 0, &anchor))
    return false;
  if (!saved_buffer_identity_matches(anchor, cursor->data)) {
    void *actual = NULL;
    (void)saved_buffer_decode_owned(anchor, &actual);
    fprintf(stderr,
        "Pizza Tycoon: read cursor identity %08x has owner %p, expected %p (position %u)\n",
        anchor,
        actual,
        (const void *)cursor->data,
        cursor->position);
    return false;
  }
  *word = anchor + cursor->position;
  return true;
}
guest_ptr32 pt_saved_game_cursor_word(const Game *owner) {
  guest_ptr32 word;
  if (!pt_saved_game_cursor_identity(owner, &word))
    abort();
  return word;
}
// Cursor capacity belongs to its native source view.

static void import_game_cursor(Game *owner, u8 *pointer) {
  PTGameReadBuffer *buffer = &owner->read_buffer;
  u32 capacity = saved_buffer_capacity(pointer);
  uintptr_t at = (uintptr_t)pointer, base = (uintptr_t)buffer->allocation;
  u32 allocation_capacity = saved_buffer_capacity(buffer->allocation);
  if (owner->loading_buffer_size && owner->loading_buffer_size < allocation_capacity)
    allocation_capacity = owner->loading_buffer_size;
  if (pointer && buffer->allocation && at >= base && at - base <= allocation_capacity) {
    buffer->cursor = (PTBufferCursor){buffer->allocation, (u32)(at - base)};
    buffer->capacity = allocation_capacity;
  } else {
    buffer->cursor = (PTBufferCursor){pointer, 0};
    buffer->capacity = capacity;
  }
}
static bool resource_pointer_word(
    const PTSavedResourceOwner *owner, size_t index, const PTSavedResourceField *field, u32 *word) {
  if (owner->kind == PT_RESOURCE_KIND_GAME && field->field == PT_RESOURCE_GAME_READ_BUFFER_CURSOR)
    return pt_saved_game_cursor_identity(
        (const Game *)(const void *)(owner->native + index * resource_specs[owner->kind].native_size), word);
  const void *pointer;
  memcpy(&pointer,
      owner->native + index * resource_specs[owner->kind].native_size + field->native_offset,
      sizeof(pointer));
  return encode_saved_field_pointer(field->target, pointer, word);
}
static void commit_resource_pointer_span(PTSavedResourceOwner *owner, size_t index, const PTSavedResourceField *field,
    u32 native, size_t byte, size_t size) {
  const PTSavedResourceSpec *spec = &resource_specs[owner->kind];
  memcpy(owner->published_native + index * spec->native_size + field->native_offset + byte,
      (const u8 *)&native + byte,
      size);
  memcpy(owner->published_saved + index * spec->saved_size + field->saved_offset + byte,
      (owner->wire + (u32)(index * spec->saved_size + field->saved_offset + byte)),
      size);
  memset(owner->pending_pointers + index * spec->saved_size + field->saved_offset + byte, 0, size);
  commit_saved_buffer_store((owner->wire + (u32)(index * spec->saved_size + field->saved_offset + byte)), size);
}
static u32 validate_resource_record(const PTSavedResourceOwner *owner, size_t index) {
  const PTSavedResourceSpec *spec = &resource_specs[owner->kind];
  u32 fault = 0;
  for (size_t i = 0; i < spec->count; ++i) {
    const PTSavedResourceField *field = &spec->fields[i];
    if (!owner->owned[index * spec->count + i] ||
        (field->target != PT_SAVED_VALUE && !tracked_resource_pointer(owner->kind, field)))
      continue;
    size_t offset = index * spec->saved_size + field->saved_offset;
    const u8 *saved = (owner->wire + (u32)offset);
    const u8 *previous = owner->published_saved + offset;
    u32 address = saved_bytes_difference(saved, previous, owner->address + (u32)offset, field->width);
    if (address && (!fault || address < fault))
      fault = address;
  }
  return fault;
}
static u32 serialize_resource_pointers(PTSavedResourceOwner *owner, size_t index, unsigned phase) {
  const PTSavedResourceSpec *spec = &resource_specs[owner->kind];
  if (!phase) {
    u32 fault = validate_resource_record(owner, index);
    if (fault)
      return fault;
  }
  for (size_t i = 0; i < spec->count; ++i) {
    const PTSavedResourceField *field = &spec->fields[i];
    if (!tracked_resource_pointer(owner->kind, field) || !owner->owned[index * spec->count + i])
      continue;
    u32 word, previous;
    guest_ptr32 address = owner->address + (u32)(index * spec->saved_size + field->saved_offset);
    if (!resource_pointer_word(owner, index, field, &word))
      return address;
    memcpy(&previous, owner->published_native + index * spec->native_size + field->native_offset, sizeof(previous));
    u8 *saved = pt_buffer_from_guest(address);
    const u8 *before = owner->published_saved + index * spec->saved_size + field->saved_offset;
    if (!phase) {
      if (owner->pointer_overrides[index * spec->count + i] && word != previous)
        return address;
      u32 fault = saved_bytes_difference(saved, before, address, sizeof(word));
      if (fault)
        return fault;
      const u8 *pending = owner->pending_pointers + index * spec->saved_size + field->saved_offset;
      u32 old_saved;
      memcpy(&old_saved, before, sizeof(u32));
      if (!word && !previous && old_saved && !(pending[0] || pending[1] || pending[2] || pending[3]))
        return address;
      continue;
    }
    if (word == previous)
      continue;
    memcpy(saved, &word, sizeof(word));
    commit_resource_pointer_span(owner, index, field, word, 0, sizeof(word));
    commit_saved_buffer_store(saved, sizeof(word));
    const void *old = storing_resource;
    storing_resource = owner->native + index * spec->native_size;
    pt_world_objects_written(saved, sizeof(word));
    storing_resource = old;
  }
  return 0;
}

static void commit_resource_scalar_span(
    PTSavedResourceOwner *owner, size_t index, const PTSavedResourceField *field, size_t byte, size_t size) {
  if (!owner->published_native || field->target != PT_SAVED_VALUE)
    return;
  const PTSavedResourceSpec *spec = &resource_specs[owner->kind];
  memcpy(owner->published_native + index * spec->native_size + field->native_offset + byte,
      owner->native + index * spec->native_size + field->native_offset + byte,
      size);
  memcpy(owner->published_saved + index * spec->saved_size + field->saved_offset + byte,
      (owner->wire + (guest_ptr32)(index * spec->saved_size + field->saved_offset + byte)),
      size);
  commit_saved_buffer_store((owner->wire + (u32)(index * spec->saved_size + field->saved_offset + byte)), size);
}
static void commit_resource_scalar(PTSavedResourceOwner *owner, size_t index, const PTSavedResourceField *field) {
  if (tracked_resource_pointer(owner->kind, field)) {
    u32 word;
    if (!resource_pointer_word(owner, index, field, &word))
      require_saved_root(owner->address);
    commit_resource_pointer_span(owner, index, field, word, 0, sizeof(word));
  } else
    commit_resource_scalar_span(owner, index, field, 0, field->width);
}
static void import_resource_record(PTSavedResourceOwner *, size_t, const void *, size_t);
static void initialize_resource_record(PTSavedResourceOwner *, size_t);
static void construct_resource_record(PTSavedResourceOwner *, size_t);
static void activate_resource_record(PTSavedResourceOwner *, size_t);
static void construct_simulation_children(PTSavedResourceOwner *, size_t);
static bool resource_field_readable(guest_ptr32 address, size_t size) {
  return pt_valid_range(address, size);
}
static PTSavedResourceOwner *resource_owner(const void *pointer, size_t *index) {
  for (PTSavedResourceOwner *owner = saved_resources; owner; owner = owner->next) {
    const PTSavedResourceSpec *spec = &resource_specs[owner->kind];
    uintptr_t offset = (uintptr_t)pointer - (uintptr_t)owner->native;
    if (offset < owner->count * spec->native_size && offset % spec->native_size == 0) {
      *index = offset / spec->native_size;
      return owner;
    }
  }
  return NULL;
}
static bool resource_native_address(
    const void *pointer, size_t size, guest_ptr32 *address, bool fields, bool endpoints) {
  for (PTSavedResourceOwner *owner = saved_resources; owner; owner = owner->next) {
    const PTSavedResourceSpec *spec = &resource_specs[owner->kind];
    uintptr_t offset = (uintptr_t)pointer - (uintptr_t)owner->native;
    if (offset >= owner->count * spec->native_size)
      continue;
    size_t index = offset / spec->native_size, local = offset % spec->native_size;
    if (!local && (!size || size <= spec->saved_size)) {
      *address = owner->address + (guest_ptr32)(index * spec->saved_size);
      return true;
    }
    if (!fields)
      continue;
    for (size_t i = 0; i < spec->count; ++i) {
      const PTSavedResourceField *field = &spec->fields[i];
      if (local < field->native_offset)
        continue;
      if (field->target != PT_SAVED_VALUE && (size || local != field->native_offset))
        continue;
      size_t byte = local - field->native_offset;
      if (byte <= field->width && size <= field->width - byte) {
        *address = owner->address + (guest_ptr32)(index * spec->saved_size + field->saved_offset + byte);
        return true;
      }
    }
  }
  // Adjacent host globals may share an object's start and another object's
  // one-past value. Actual owned storage always takes precedence.

  bool endpoint = false;
  guest_ptr32 encoded = 0;
  if (!size && endpoints)
    for (PTSavedResourceOwner *owner = saved_resources; owner; owner = owner->next) {
      const PTSavedResourceSpec *spec = &resource_specs[owner->kind];
      uintptr_t offset = (uintptr_t)pointer - (uintptr_t)owner->native;
      if (offset != owner->count * spec->native_size)
        continue;
      guest_ptr32 candidate = owner->address + (guest_ptr32)(owner->count * spec->saved_size);
      if (endpoint && candidate != encoded) {
        fputs("Pizza Tycoon: ambiguous saved one-past identity\n", stderr);
        abort();
      }
      endpoint = true;
      encoded = candidate;
    }
  if (endpoint) {
    *address = encoded;
    return true;
  }
  return false;
}
bool pt_saved_resource_identity(const void *pointer, PTResourceKind kind, guest_ptr32 *address) {
  size_t index;
  PTSavedResourceOwner *owner = resource_owner(pointer, &index);
  if (!owner || owner->kind != kind)
    return false;
  *address = owner->address + (guest_ptr32)(index * resource_specs[kind].saved_size);
  return true;
}
bool pt_saved_resource_object_pointer(const void *pointer, guest_ptr32 *address) {
  return resource_native_address(pointer, 0, address, false, false);
}
bool pt_saved_resource_pointer(const void *pointer, guest_ptr32 *address) {
  return resource_native_address(pointer, 0, address, true, true);
}

static const PTSavedResourceSpec *resource_spec_dimensions(PTResourceKind kind, size_t count, size_t capacity) {
  if ((unsigned)kind >= sizeof(resource_specs) / sizeof(*resource_specs))
    abort();
  const PTSavedResourceSpec *spec = &resource_specs[kind];
  if (!capacity || count > capacity || !spec->native_size || !spec->saved_size || !spec->count ||
      capacity > SIZE_MAX / spec->native_size || capacity > SIZE_MAX / spec->saved_size ||
      capacity > SIZE_MAX / spec->count) {
    fputs("Pizza Tycoon: invalid saved resource dimensions\n", stderr);
    abort();
  }
  return spec;
}
static PTSavedResourceOwner *new_resource_owner(
    PTResourceKind kind, guest_ptr32 address, void *native, size_t count, size_t capacity) {
  if (!native)
    abort();
  const PTSavedResourceSpec *spec = resource_spec_dimensions(kind, count, capacity);
  const size_t field_bytes = capacity * spec->count;
  const size_t tracked_capacity = kind == PT_RESOURCE_KIND_RECIPE_ENTRY && capacity < 256 ? 256 : capacity;
  if (tracked_capacity > SIZE_MAX / spec->native_size || tracked_capacity > SIZE_MAX / spec->saved_size)
    abort();
  const size_t native_history = tracked_capacity * spec->native_size;
  const size_t saved_history = tracked_capacity * spec->saved_size;
  PTSavedResourceOwner *owner = calloc(1, sizeof(*owner));
  if (!owner)
    abort();
  owner->kind = kind;
  owner->address = address;
  owner->wire = pt_buffer_from_guest(owner->address);
  owner->count = count;
  owner->capacity = capacity;
  owner->native = native;
  owner->native_extent = capacity * spec->native_size;
  owner->initialized = calloc(capacity, 1);
  owner->owned = calloc(1, field_bytes);
  owner->pointer_overrides = calloc(1, field_bytes);
  owner->saved_extent = SIZE_MAX;
  owner->published_native = calloc(1, native_history);
  owner->published_saved = calloc(1, saved_history);
  owner->pending_pointers = calloc(1, saved_history);
  if (!owner->published_native || !owner->published_saved || !owner->pending_pointers)
    abort();
  if (!owner->native || !owner->initialized || !owner->owned || !owner->pointer_overrides)
    abort();
  owner->next = saved_resources;
  saved_resources = owner;
  if (kind == PT_RESOURCE_KIND_REST) {
    Rest *restaurant = (Rest *)(void *)owner->native;
    PTSavedResourceOwner *child = new_resource_owner(
        PT_RESOURCE_KIND_RESTORECIPE, address + offsetof(PTSavedRest, recipes), restaurant->recipes, 0, 24);
    child->container = owner;
  }
  return owner;
}
static PTSavedResourceOwner *import_unowned_resource(
    PTResourceKind kind, guest_ptr32 address, size_t count, size_t capacity) {
  fprintf(
      stderr, "Pizza Tycoon: unowned native resource kind %u at %08x (rows %zu/%zu)\n", kind, address, count, capacity);
  abort();
}

static void saved_restaurant_extent(PTSavedResourceOwner *owner) {
  if (owner->kind != PT_RESOURCE_KIND_REST || !owner->allocated || owner->saved_extent != SIZE_MAX)
    return;
  fputs("Pizza Tycoon: restaurant has no native allocation extent\n", stderr);
  abort();
}
// Reserve identities before following parent pointers. A request for an embedded
// row must never allocate a second native object at that same saved location.

static PTSavedResourceOwner *saved_restaurant_menu(PTSavedResourceOwner *owner) {
  if (owner->kind != PT_RESOURCE_KIND_REST)
    return NULL;
  saved_restaurant_extent(owner);
  const size_t begin = offsetof(PTSavedRest, recipes);
  const size_t bytes = sizeof(((PTSavedRest *)0)->recipes);
  if (owner->saved_extent < begin + bytes)
    return NULL;
  PTSavedResourceOwner *menu = NULL;
  for (PTSavedResourceOwner *child = saved_resources; child; child = child->next)
    if (child->container == owner && child->kind == PT_RESOURCE_KIND_RESTORECIPE) {
      if (menu) {
        fputs("Pizza Tycoon: duplicate restaurant menu owners\n", stderr);
        abort();
      }
      menu = child;
    }
  if (!menu) {
    fputs("Pizza Tycoon: missing restaurant menu owner\n", stderr);
    abort();
  }
  const size_t count = sizeof(((Rest *)0)->recipes) / sizeof(((Rest *)0)->recipes[0]);
  if (menu->capacity != count || menu->native != (u8 *)((Rest *)(void *)owner->native)->recipes ||
      menu->address != owner->address + begin) {
    fputs("Pizza Tycoon: invalid embedded restaurant menu owner\n", stderr);
    abort();
  }
  u64 end = (u64)menu->address + bytes;
  for (PTSavedResourceOwner *row = saved_resources; row; row = row->next) {
    if (row == menu || row->kind != PT_RESOURCE_KIND_RESTORECIPE || !row->count)
      continue;
    u64 row_end = (u64)row->address + row->count * sizeof(PTSavedRestoRecipe);
    if (row->address < end && row_end > menu->address) {
      fprintf(stderr,
          "Pizza Tycoon: conflicting menu row %08x for restaurant %08x (row parent %08x)\n",
          row->address,
          owner->address,
          row->container ? row->container->address : 0);
      abort();
    }
  }
  menu->count = count;
  return menu;
}
static PTSavedResourceOwner *resource_address_owner(PTResourceKind kind, guest_ptr32 address, size_t *index) {
  for (PTSavedResourceOwner *owner = saved_resources; owner; owner = owner->next) {
    if (owner->kind != kind)
      continue;
    if (kind == PT_RESOURCE_KIND_RESTORECIPE && owner->container && !owner->count)
      (void)saved_restaurant_menu(owner->container);
    const PTSavedResourceSpec *spec = &resource_specs[kind];
    u32 offset = address - owner->address;
    if ((u64)offset < owner->count * spec->saved_size && offset % spec->saved_size == 0) {
      *index = offset / spec->saved_size;
      return owner;
    }
  }
  return NULL;
}
static void *resource_identity_only(PTResourceKind kind, guest_ptr32 address) {
  size_t index;
  PTSavedResourceOwner *owner = resource_address_owner(kind, address, &index);
  if (!owner) {
    owner = import_unowned_resource(kind, address, 1, 1);
    index = 0;
  }
  return owner->native + index * resource_specs[kind].native_size;
}
void *pt_saved_resource_decode(PTResourceKind kind, guest_ptr32 address) {
  if ((unsigned)kind >= sizeof(resource_specs) / sizeof(*resource_specs))
    abort();
  size_t index;
  PTSavedResourceOwner *owner = resource_address_owner(kind, address, &index);
  if (!owner) {
    owner = import_unowned_resource(kind, address, 1, 1);
    index = 0;
  }
  initialize_resource_record(owner, index);
  return owner->native + index * resource_specs[kind].native_size;
}
static void construct_simulation_children(PTSavedResourceOwner *owner, size_t index) {
  if (owner->kind == PT_RESOURCE_KIND_RECIPE) {
    Recipe *recipe = (Recipe *)(void *)(owner->native + index * resource_specs[owner->kind].native_size);
    if (!recipe->entry_capacity)
      recipe->entry_capacity = (size_t)recipe->num_entries + 1;
    if (recipe->entry_capacity > 256)
      abort();
    guest_ptr32 address = owner->address + offsetof(PTSavedRecipe, entries);
    size_t child_index;
    PTSavedResourceOwner *child = resource_address_owner(PT_RESOURCE_KIND_RECIPE_ENTRY, address, &child_index);
    if (!child)
      child = new_resource_owner(
          PT_RESOURCE_KIND_RECIPE_ENTRY, address, recipe->entries, recipe->entry_capacity, recipe->entry_capacity);
    if (child->capacity < recipe->entry_capacity) {
      size_t capacity = recipe->entry_capacity;
      u8 *initialized = calloc(capacity, 1), *owned = calloc(capacity, 1);
      if (!initialized || !owned)
        abort();
      free(child->initialized);
      free(child->owned);
      child->initialized = initialized;
      child->owned = owned;
      child->capacity = capacity;
    }
    child->count = recipe->entry_capacity;
    if (child->native != (u8 *)(void *)recipe->entries || child->count != recipe->entry_capacity) {
      fputs("Pizza Tycoon: overlapping native recipe-entry identities\n", stderr);
      abort();
    }
    child->container = owner;
    for (size_t i = 0; i < child->count; ++i) {
      construct_resource_record(child, i);
    }
  } else if (owner->kind == PT_RESOURCE_KIND_REST) {
    if (owner->saved_extent < offsetof(PTSavedRest, recipes) + sizeof(((PTSavedRest *)0)->recipes))
      return;
    if (!resource_field_readable(owner->address + offsetof(PTSavedRest, num_recipes), 1))
      return;
    PTSavedResourceOwner *child = saved_restaurant_menu(owner);
    if (child) {
      unsigned previous = saved_reference_depth;
      ++saved_reference_depth;
      for (size_t i = 0; i < child->count; ++i) {
        construct_resource_record(child, i);
      }
      saved_reference_depth = previous;
    }
  }
}
static const PTSavedResourceField *resource_field(PTResourceKind kind, PTResourceField id) {
  const PTSavedResourceSpec *spec = &resource_specs[kind];
  for (size_t i = 0; i < spec->count; ++i)
    if (spec->fields[i].field == id)
      return &spec->fields[i];
  return NULL;
}
guest_ptr32 pt_saved_resource_word(const void *native, PTResourceKind kind, PTResourceField id) {
  const PTSavedResourceField *field = resource_field(kind, id);
  if (!field || field->target == PT_SAVED_VALUE)
    abort();
  size_t index;
  PTSavedResourceOwner *owner = resource_owner(native, &index);
  if (owner) {
    if (owner->kind != kind)
      abort();
    guest_ptr32 address = owner->address + (guest_ptr32)(index * resource_specs[kind].saved_size + field->saved_offset);
    return ((const PTSavedPointerWord *)(const void *)pt_buffer_from_guest(address))->value;
  }
  if (kind == PT_RESOURCE_KIND_GAME && id == PT_RESOURCE_GAME_READ_BUFFER_CURSOR)
    return pt_saved_game_cursor_word(native);
  const void *pointer;
  memcpy(&pointer, (const u8 *)native + field->native_offset, sizeof(pointer));
  return pt_buffer_to_guest(pointer);
}
// Keep obsolete file pointer bytes as history without granting native access.
// Only the notified bytes are imported; unrelated dirty bytes remain visible
// to the saved-image validator. Native pointer history is explicitly null.

static void defer_resource_buffer_pointer(
    PTSavedResourceOwner *owner, size_t index, const PTSavedResourceField *field, size_t byte, size_t size) {
  const PTSavedResourceSpec *spec = &resource_specs[owner->kind];
  size_t native_offset = index * spec->native_size + field->native_offset;
  void *empty = NULL;
  memcpy(owner->native + native_offset, &empty, sizeof(empty));
  commit_resource_pointer_span(owner, index, field, 0, byte, size);
  memset(owner->published_native + native_offset, 0, sizeof(u32));
  memset(owner->pending_pointers + index * spec->saved_size + field->saved_offset, 1, sizeof(u32));
}
static void retire_saved_resource_buffer(const PTSavedBufferOwner *buffer) {
  for (PTSavedResourceOwner *owner = saved_resources; owner; owner = owner->next) {
    // Asset views and Game input buffers are transient. Simulation file
    // references keep their normal explicit assignment/fixup lifecycle.

    if (owner->kind > PT_RESOURCE_KIND_GAME)
      continue;
    const PTSavedResourceSpec *spec = &resource_specs[owner->kind];
    for (size_t active = 0; active < owner->active_count; ++active) {
      size_t index = owner->active_indices[active];
      for (size_t i = 0; i < spec->count; ++i) {
        const PTSavedResourceField *field = &spec->fields[i];
        if (field->target != PT_SAVED_BUFFER || !owner->owned[index * spec->count + i])
          continue;
        void *pointer;
        memcpy(&pointer, owner->native + index * spec->native_size + field->native_offset, sizeof(pointer));
        if (!saved_buffer_borrows(buffer, pointer))
          continue;
        void *empty = NULL;
        memcpy(owner->native + index * spec->native_size + field->native_offset, &empty, sizeof(empty));
        if (owner->kind == PT_RESOURCE_KIND_GAME &&
            (field->field == PT_RESOURCE_GAME_READ_BUFFER_ALLOCATION ||
                field->field == PT_RESOURCE_GAME_READ_BUFFER_CURSOR)) {
          Game *game = (Game *)(void *)(owner->native + index * spec->native_size);
          if (field->field == PT_RESOURCE_GAME_READ_BUFFER_CURSOR)
            game->read_buffer.cursor.position = 0;
          game->read_buffer.capacity = 0;
        }
        if (owner->kind == PT_RESOURCE_KIND_BOB_HOST && field->field == PT_RESOURCE_HEADER_SOURCE)
          ((BobHost *)(void *)(owner->native + index * spec->native_size))->header_capacity = 0;
      }
    }
  }
}

static void import_resource_record(PTSavedResourceOwner *owner, size_t index, const void *storage, size_t size) {
  const PTSavedResourceSpec *spec = &resource_specs[owner->kind];
  u8 *native = owner->native + index * spec->native_size;
  if (native == storing_resource)
    return;
  guest_ptr32 address = owner->address + (guest_ptr32)(index * spec->saved_size);
  uintptr_t begin = (uintptr_t)storage;
  // Scalar bytes precede pointer construction so arrays see the new count.

  for (unsigned phase = 0; phase < 2; ++phase)
    for (size_t i = 0; i < spec->count; ++i) {
      const PTSavedResourceField *field = &spec->fields[i];
      if ((field->target == PT_SAVED_VALUE) != (phase == 0))
        continue;
      const u8 *saved = pt_buffer_from_guest(address + (guest_ptr32)field->saved_offset);
      uintptr_t location = (uintptr_t)saved;
      if (!size || (location >= begin ? location - begin >= size : begin - location >= field->width))
        continue;
      if ((u64)index * spec->saved_size + field->saved_offset + field->width > owner->saved_extent)
        continue;
      if (!resource_field_readable(address + (guest_ptr32)field->saved_offset, field->width))
        continue;
      if (owner->pointer_overrides[index * spec->count + i])
        require_saved_root(address + (u32)field->saved_offset);
      bool was_owned = owner->owned[index * spec->count + i] != 0;
      owner->owned[index * spec->count + i] = 1;
      if (field->target == PT_SAVED_VALUE) {
        import_saved_scalar(native + field->native_offset, saved, field->width, storage, size);
        size_t byte = begin > location ? begin - location : 0;
        size_t overlap = size - (location + byte - begin);
        if (overlap > field->width - byte)
          overlap = field->width - byte;
        commit_resource_scalar_span(owner, index, field, byte, overlap);
        continue;
      }
      guest_ptr32 word = ((const PTSavedPointerWord *)(const void *)saved)->value;
      size_t byte = begin > location ? begin - location : 0;
      size_t overlap = size - (location + byte - begin);
      if (overlap > field->width - byte)
        overlap = field->width - byte;
      bool tracked = tracked_resource_pointer(owner->kind, field);
      u8 *pending = owner->pending_pointers + index * spec->saved_size + field->saved_offset;
      bool incomplete = tracked && (pending[0] || pending[1] || pending[2] || pending[3]);
      if (incomplete && was_owned) {
        u32 current;
        if (!resource_pointer_word(owner, index, field, &current))
          require_saved_root(address + (u32)field->saved_offset);
        if (!current) {
          memcpy(&word, owner->published_saved + index * spec->saved_size + field->saved_offset, sizeof(u32));
          memcpy((u8 *)&word + byte, saved + byte, overlap);
          memset(pending + byte, 0, overlap);
          if (pending[0] || pending[1] || pending[2] || pending[3]) {
            memcpy(
                owner->published_saved + index * spec->saved_size + field->saved_offset + byte, saved + byte, overlap);
            continue;
          }
        } else {
          word = current;
          memcpy((u8 *)&word + byte, saved + byte, overlap);
          memset(pending, 0, sizeof(u32));
        }
      } else if (tracked && was_owned) {
        if (!resource_pointer_word(owner, index, field, &word))
          require_saved_root(address + (u32)field->saved_offset);
        memcpy((u8 *)&word + byte, saved + byte, overlap);
      }
      if (saved_reference_depth && owner->kind == PT_RESOURCE_KIND_PLAYER && field->target == PT_SAVED_BUFFER) {
        // File depot words indicate presence; LoadSavegame allocates and
        // binds their new storage after reading the player section.

        defer_resource_buffer_pointer(owner, index, field, byte, overlap);
        continue;
      }
      if (saved_reference_depth && word) {
        bool unresolved = false;
        if (field->target >= 0) {
          size_t row;
          unresolved = !resource_address_owner((PTResourceKind)field->target, word, &row);
        } else if (field->target <= PT_SAVED_LINK_BASE || field->target == PT_SAVED_LINK) {
          unresolved = !simulation_link_identity(word);
        }
        if (unresolved) {
          // File references may precede their records or carry IDs used
          // by the loader's later fixups. Keep their exact wire bytes;
          // they do not justify constructing another simulation object.

          defer_resource_buffer_pointer(owner, index, field, byte, overlap);
          continue;
        }
      }
      void *pointer = NULL;
      pointer = decode_saved_field_pointer(field->target, word, saved_reference_depth != 0);
      if (owner->kind == PT_RESOURCE_KIND_GAME && field->field == PT_RESOURCE_GAME_READ_BUFFER_CURSOR)
        import_game_cursor((Game *)(void *)native, pointer);
      else
        memcpy(native + field->native_offset, &pointer, sizeof(pointer));
      if (tracked) {
        commit_resource_pointer_span(owner, index, field, word, byte, overlap);
        if (incomplete)
          memcpy(owner->published_native + index * spec->native_size + field->native_offset, &word, sizeof(u32));
      }
    }
}
static void activate_resource_record(PTSavedResourceOwner *owner, size_t index) {
  if (index >= owner->capacity)
    abort();
  if (owner->initialized[index])
    return;
  owner->initialized[index] = 1;
  if (owner->active_count == owner->active_capacity) {
    size_t capacity = owner->active_capacity ? owner->active_capacity * 2 : 16;
    size_t *indices = realloc(owner->active_indices, capacity * sizeof(*indices));
    if (!indices)
      abort();
    owner->active_indices = indices;
    owner->active_capacity = capacity;
  }
  owner->active_indices[owner->active_count++] = index;
}
static void initialize_resource_record(PTSavedResourceOwner *owner, size_t index) {
  if (owner->initialized[index])
    return;
  fputs("Pizza Tycoon: uninitialized native resource record\n", stderr);
  abort();
}
// Constructors establish each native record extent before pointer decoding.

static PTForwardLink *simulation_link_identity(guest_ptr32 address) {
  if (!address)
    return NULL;
  for (PTSavedResourceOwner *owner = saved_resources; owner; owner = owner->next) {
    if (owner->kind < PT_RESOURCE_KIND_CONTRACT)
      continue;
    const PTSavedResourceSpec *spec = &resource_specs[owner->kind];
    u32 offset = address - owner->address;
    size_t index = offset / spec->saved_size, local = offset % spec->saved_size;
    if (index >= owner->count)
      continue;
    for (size_t i = 0; i < spec->count; ++i) {
      const PTSavedResourceField *field = &spec->fields[i];
      if ((field->target <= PT_SAVED_LINK_BASE || field->target == PT_SAVED_LINK) && field->saved_offset == local) {
        if (!saved_reference_depth)
          initialize_resource_record(owner, index);
        return (PTForwardLink *)(void *)(owner->native + index * spec->native_size + field->native_offset);
      }
    }
  }
  return NULL;
}
static PTForwardLink *simulation_link_decode(guest_ptr32 address, int kind) {
  if (!address)
    return NULL;
  PTForwardLink *pointer = simulation_link_identity(address);
  if (pointer)
    return pointer;
  if (kind < 0) {
    fputs("Pizza Tycoon: unknown native list payload family\n", stderr);
    abort();
  }
  return saved_reference_depth ? resource_identity_only((PTResourceKind)kind, address)
                               : pt_saved_resource_decode((PTResourceKind)kind, address);
}
// Byte-buffer writes may alias an imported physical field. Dispatch from
// the mutation boundary, so audits never refresh cached records.

static void resources_written(const void *storage, size_t size) {
  for (PTSavedResourceOwner *owner = saved_resources; owner; owner = owner->next) {
    const PTSavedResourceSpec *spec = &resource_specs[owner->kind];
    uintptr_t first = (uintptr_t)owner->wire, begin = (uintptr_t)storage;
    size_t bytes = owner->count * spec->saved_size;
    if (bytes > owner->saved_extent)
      bytes = owner->saved_extent;
    if (!size || (first >= begin ? first - begin >= size : begin - first >= bytes))
      continue;
    size_t start = begin > first ? (begin - first) / spec->saved_size : 0;
    size_t stop = owner->count;
    if (begin + size > begin && begin + size < first + bytes)
      stop = (begin + size - first + spec->saved_size - 1) / spec->saved_size;
    for (size_t i = start; i < stop; ++i)
      if (owner->initialized[i]) {
        import_resource_record(owner, i, storage, size);
      }
  }
}

static u32 serialize_resource_values(PTSavedResourceOwner *owner, size_t index) {
  const PTSavedResourceSpec *spec = &resource_specs[owner->kind];
  for (size_t i = 0; i < spec->count; ++i) {
    const PTSavedResourceField *field = &spec->fields[i];
    if (field->target != PT_SAVED_VALUE || !owner->owned[index * spec->count + i])
      continue;
    const u8 *native = owner->native + index * spec->native_size + field->native_offset;
    const u8 *previous_native = owner->published_native + index * spec->native_size + field->native_offset;
    guest_ptr32 address = owner->address + (guest_ptr32)(index * spec->saved_size + field->saved_offset);
    u8 *saved = pt_buffer_from_guest(address);
    const u8 *previous_saved = owner->published_saved + index * spec->saved_size + field->saved_offset;
    u32 fault = saved_bytes_difference(saved, previous_saved, address, field->width);
    if (fault)
      return fault;
    if (!memcmp(native, previous_native, field->width))
      continue;
    memcpy(saved, native, field->width);
    commit_resource_scalar(owner, index, field);
    const void *previous = storing_resource;
    storing_resource = owner->native + index * spec->native_size;
    pt_world_objects_written(saved, field->width);
    storing_resource = previous;
  }
  return 0;
}
// A save request serializes only its intersecting records, including embedded
// menu rows and recipe entries. It never follows the entire object graph.

static void serialize_resource_range(u32 address, size_t size) {
  for (unsigned phase = 0; phase < 3; ++phase)
    for (PTSavedResourceOwner *owner = saved_resources; owner; owner = owner->next) {
      if (!owner->published_native)
        continue;
      const PTSavedResourceSpec *spec = &resource_specs[owner->kind];
      u64 end = (u64)owner->address + owner->count * spec->saved_size;
      if (end <= address || (u64)address + size <= owner->address)
        continue;
      for (size_t active = 0; active < owner->active_count; ++active) {
        size_t index = owner->active_indices[active];
        u64 first = (u64)owner->address + index * spec->saved_size;
        if (first + spec->saved_size <= address || (u64)address + size <= first)
          continue;
        u32 fault =
            phase < 2 ? serialize_resource_pointers(owner, index, phase) : serialize_resource_values(owner, index);
        require_saved_root(fault);
      }
    }
}

// Canonical identity queries never initialize or import a resource record.
// Raw image aliases never substitute for an owned native scalar field.

static const PTResourceKind simulation_kinds[] = {
  [PT_SIM_MOUSE] = PT_RESOURCE_KIND_MOUSE,
  [PT_SIM_CONTRACT] = PT_RESOURCE_KIND_CONTRACT,
  [PT_SIM_RESTORECIPE] = PT_RESOURCE_KIND_RESTORECIPE,
  [PT_SIM_TOWN] = PT_RESOURCE_KIND_TOWN,
  [PT_SIM_SUPPLIER] = PT_RESOURCE_KIND_SUPPLIER,
  [PT_SIM_ZONE] = PT_RESOURCE_KIND_ZONE,
  [PT_SIM_POI] = PT_RESOURCE_KIND_POI,
  [PT_SIM_RECIPE] = PT_RESOURCE_KIND_RECIPE,
  [PT_SIM_PLAYER] = PT_RESOURCE_KIND_PLAYER,
  [PT_SIM_APPLE] = PT_RESOURCE_KIND_APPLE,
  [PT_SIM_REST] = PT_RESOURCE_KIND_REST,
};
// A byte span may cover adjacent scalar fields only when both layouts have
// the same uninterrupted byte sequence. Pointer storage and padding stop it.

static bool serialized_scalar_span(const PTSavedResourceSpec *spec, size_t local, size_t size, size_t *saved_offset) {
  size_t consumed = 0, first = 0;
  do {
    const PTSavedResourceField *match = NULL;
    size_t byte = 0;
    for (size_t i = 0; i < spec->count; ++i) {
      const PTSavedResourceField *field = &spec->fields[i];
      if (field->target != PT_SAVED_VALUE || local + consumed < field->native_offset)
        continue;
      size_t at = local + consumed - field->native_offset;
      if (at >= field->width)
        continue;
      if (consumed && field->saved_offset + at != first + consumed)
        continue;
      match = field;
      byte = at;
      break;
    }
    if (!match)
      return false;
    if (!consumed)
      first = match->saved_offset + byte;
    size_t count = match->width - byte;
    if (count > size - consumed)
      count = size - consumed;
    consumed += count;
  } while (consumed < size);
  *saved_offset = first;
  return true;
}

// Save files replace selected reference words with coordinates or recipe IDs.
// This changes only the wire view. The native pointer and its history remain
// intact; every override is validated and explicitly restored by its caller.

void pt_saved_store_reference_word(void *native, PTResourceKind kind, PTResourceField id, u32 word) {
  size_t index;
  PTSavedResourceOwner *owner = resource_owner(native, &index);
  if (!owner || owner->kind != kind)
    abort();
  serialize_resource_range(
      owner->address + (u32)(index * resource_specs[kind].saved_size), resource_specs[kind].saved_size);
  const PTSavedResourceSpec *spec = &resource_specs[kind];
  const PTSavedResourceField *field = resource_field(kind, id);
  if (!field || !tracked_resource_pointer(kind, field))
    abort();
  size_t slot = index * spec->count + (size_t)(field - spec->fields);
  if (!owner->owned[slot])
    abort();
  u32 pointer;
  if (!resource_pointer_word(owner, index, field, &pointer))
    require_saved_root(owner->address);
  u8 *saved = (owner->wire + (u32)(index * spec->saved_size + field->saved_offset));
  memcpy(saved, &word, sizeof(word));
  memcpy(owner->published_saved + index * spec->saved_size + field->saved_offset, &word, sizeof(word));
  owner->pointer_overrides[slot] = word != pointer;
  commit_saved_buffer_store(saved, sizeof(word));
}

bool pt_saved_serialized_span(const void *pointer, size_t size, guest_ptr32 *address, u32 *wire_size) {
  if (size > UINT32_MAX)
    return false;
  if (pt_saved_buffer_span(pointer, size, address)) {
    *wire_size = (u32)size;
    return true;
  }
  if (pt_saved_buffer_is_mapped(pointer, size)) {
    *address = (guest_ptr32)((uintptr_t)pointer - (uintptr_t)pt_memory.bytes);
    *wire_size = (u32)size;
    return true;
  }
  for (PTSavedResourceOwner *owner = saved_resources; owner; owner = owner->next) {
    const PTSavedResourceSpec *spec = &resource_specs[owner->kind];
    uintptr_t offset = (uintptr_t)pointer - (uintptr_t)owner->native;
    if (offset >= owner->count * spec->native_size)
      continue;
    size_t index = offset / spec->native_size, local = offset % spec->native_size;
    *address = owner->address + (guest_ptr32)(index * spec->saved_size);
    if (!local) {
      if (owner->kind == PT_RESOURCE_KIND_RECIPE && size >= spec->native_size) {
        const Recipe *recipe = (const Recipe *)(const void *)(owner->native + index * spec->native_size);
        size_t entries = size - spec->native_size;
        if (entries <= recipe->entry_capacity * sizeof(RecipeEntry)) {
          *wire_size = (u32)(spec->saved_size + entries);
          return true;
        }
      }
      if (owner->kind == PT_RESOURCE_KIND_REST && size == spec->native_size + 1u) {
        *wire_size = (u32)spec->saved_size + 1u;
        return true;
      }
      if (size % spec->native_size == 0 && size / spec->native_size <= owner->count - index) {
        *wire_size = (u32)((size / spec->native_size) * spec->saved_size);
        return true;
      }
      for (size_t i = 0; i < spec->count; ++i) {
        const PTSavedResourceField *field = &spec->fields[i];
        size_t width = field->target == PT_SAVED_VALUE ? field->width : sizeof(void *);
        if (size == field->native_offset + width) {
          *wire_size = (u32)(field->saved_offset + field->width);
          return true;
        }
      }
    }
    size_t saved_offset;
    if (serialized_scalar_span(spec, local, size, &saved_offset)) {
      *address += (guest_ptr32)saved_offset;
      *wire_size = (u32)size;
      return true;
    }
  }
  return false;
}
static void serialize_io_bytes(PTSavedBufferOwner *owner, u32 address, size_t size) {
  u64 first = address > owner->address ? address : owner->address;
  u64 end = (u64)address + size, limit = (u64)owner->address + owner->size;
  if (end > limit)
    end = limit;
  if (end <= first)
    return;
  size_t offset = (size_t)(first - owner->address), count = (size_t)(end - first);
  require_saved_root(saved_bytes_difference(owner->wire + offset, owner->previous_saved + offset, (u32)first, count));
  if (memcmp(owner->native + offset, owner->previous_native + offset, count))
    publish_saved_buffer_span(owner, offset, count);
}
static void serialize_io_range(u32 address, size_t size) {
  require_saved_root(serialize_saved_globals(address, size));
  // Byte buffers precede typed overlays, as in the file decoder. A typed
  // record then projects its fields into the same saved byte allocation.

  for (PTSavedBufferOwner *owner = saved_buffers; owner; owner = owner->next)
    serialize_io_bytes(owner, address, size);
  serialize_resource_range(address, size);
}
void *pt_saved_io_buffer(const void *pointer, u32 size, u32 *wire_size) {
  guest_ptr32 address;
  if (pt_saved_serialized_span(pointer, size, &address, wire_size)) {
    serialize_io_range(address, *wire_size);
    return pt_buffer_from_guest(address);
  }
  // Stack scalars and temporary byte buffers have no persistent identity.

  for (PTSavedResourceOwner *owner = saved_resources; owner; owner = owner->next) {
    const PTSavedResourceSpec *spec = &resource_specs[owner->kind];
    uintptr_t offset = (uintptr_t)pointer - (uintptr_t)owner->native;
    if (offset < owner->count * spec->native_size) {
      fputs("Pizza Tycoon: unrepresentable native record span in legacy I/O\n", stderr);
      abort();
    }
  }
  *wire_size = size;
  return (void *)pointer;
}
Recipe *pt_saved_allocate_recipe(u32 wire_size) {
  if (wire_size < sizeof(PTSavedRecipe) || wire_size > offsetof(PTSavedRecipe, entries) + 256u * sizeof(RecipeEntry)) {
    fputs("Pizza Tycoon: invalid legacy recipe allocation size\n", stderr);
    abort();
  }
  return pt_simulation_allocate(PT_SIM_RECIPE, sizeof(Recipe) + wire_size - offsetof(PTSavedRecipe, entries));
}

static u32 simulation_saved_size(PTResourceKind kind, size_t native_size) {
  const PTSavedResourceSpec *spec = &resource_specs[kind];
  if (kind == PT_RESOURCE_KIND_REST && native_size == PT_AI_RESTAURANT_SIZE)
    return offsetof(PTSavedRest, some_furniture_counter) + 1;
  if (native_size < spec->native_size || native_size - spec->native_size > UINT32_MAX - spec->saved_size)
    abort();
  return (u32)(spec->saved_size + native_size - spec->native_size);
}
u32 pt_saved_simulation_byte_count(PTSimulationKind kind, size_t size) {
  if ((unsigned)kind >= PT_SIM_KIND_COUNT)
    abort();
  return simulation_saved_size(simulation_kinds[kind], size);
}

static void construct_resource_record(PTSavedResourceOwner *owner, size_t index) {
  const PTSavedResourceSpec *spec = &resource_specs[owner->kind];
  activate_resource_record(owner, index);
  for (size_t i = 0; i < spec->count; ++i) {
    const PTSavedResourceField *field = &spec->fields[i];
    size_t offset = index * spec->saved_size + field->saved_offset;
    if (offset + field->width > owner->saved_extent)
      continue;
    if (field->target != PT_SAVED_VALUE && !tracked_resource_pointer(owner->kind, field))
      continue;
    owner->owned[index * spec->count + i] = 1;
    commit_resource_scalar(owner, index, field);
    if (field->target != PT_SAVED_VALUE) {
      u32 word;
      memcpy(&word, owner->published_saved + offset, sizeof(u32));
      if (word)
        memset(owner->pending_pointers + offset, 1, sizeof(u32));
    }
  }
}
static bool bind_saved_simulation(PTSimulationKind kind, size_t size, void *object) {
  if ((unsigned)kind >= PT_SIM_KIND_COUNT)
    abort();
  PTResourceKind resource = simulation_kinds[kind];
  u32 saved_size = simulation_saved_size(resource, size);
  void *storage = kind == PT_SIM_MOUSE ? allocate_saved_storage(saved_size, 0) : MemAllocOrMsg_(saved_size);
  if (!storage)
    return false;
  guest_ptr32 address = pt_buffer_to_guest(storage);
  size_t index;
  PTSavedResourceOwner *owner = resource_address_owner(resource, address, &index);
  if (owner) {
    fputs("Pizza Tycoon: duplicate live simulation storage identity\n", stderr);
    abort();
  }
  owner = new_resource_owner(resource, address, object, 1, 1);
  index = 0;
  owner->allocated = true;
  owner->native_extent = resource == PT_RESOURCE_KIND_REST && size < sizeof(Rest) + 1u ? sizeof(Rest) + 1u : size;
  // Fresh pointer words are not objects until the original initialization
  // assigns them. Do not follow heap metadata left in the new payload.

  owner->saved_extent = saved_size;
  owner->extent_fixed = true;
  construct_resource_record(owner, index);
  u8 *native = owner->native;
  if (resource == PT_RESOURCE_KIND_RECIPE) {
    if (size < sizeof(Recipe) + sizeof(RecipeEntry) || (size - sizeof(Recipe)) % sizeof(RecipeEntry))
      abort();
    ((Recipe *)(void *)native)->entry_capacity = (size - sizeof(Recipe)) / sizeof(RecipeEntry);
  }
  construct_simulation_children(owner, index);
  return true;
}
static bool simulation_owner_contains(const PTSavedResourceOwner *owner, const void *pointer) {
  return pointer && (uintptr_t)pointer - (uintptr_t)owner->native < owner->native_extent;
}
static bool simulation_owner_belongs_to(const PTSavedResourceOwner *owner, const PTSavedResourceOwner *parent) {
  for (; owner; owner = owner->container)
    if (owner == parent)
      return true;
  return false;
}
static void clear_simulation_references(PTSavedResourceOwner *released) {
  // Inspect real pointer members only. No encoded pointer history participates
  // in the lifetime of a native object. Intrusive nodes must be unlinked by
  // their caller; silently repairing list links would lose list counts.

  for (PTSavedResourceOwner *owner = saved_resources; owner; owner = owner->next) {
    if (simulation_owner_belongs_to(owner, released))
      continue;
    const PTSavedResourceSpec *spec = &resource_specs[owner->kind];
    // Unused menu slots can still hold borrows after a count changes.

    for (size_t index = 0; index < owner->capacity; ++index) {
      for (size_t i = 0; i < spec->count; ++i) {
        const PTSavedResourceField *field = &spec->fields[i];
        if (field->target == PT_SAVED_VALUE)
          continue;
        u8 *member = owner->native + index * spec->native_size + field->native_offset;
        const void *pointer;
        memcpy(&pointer, member, sizeof(pointer));
        if (!simulation_owner_contains(released, pointer))
          continue;
        if (field->target == PT_SAVED_LINK || field->target <= PT_SAVED_LINK_BASE) {
          fputs("Pizza Tycoon: native simulation object released while still linked\n", stderr);
          abort();
        }
        pointer = NULL;
        memcpy(member, &pointer, sizeof(pointer));
      }
    }
  }
  if (simulation_owner_contains(released, current_town))
    current_town = NULL;
  if (simulation_owner_contains(released, mouse_over_zone))
    mouse_over_zone = NULL;
  if (simulation_owner_contains(released, last_mouse_over_district))
    last_mouse_over_district = NULL;
  if (simulation_owner_contains(released, player_ptr))
    player_ptr = NULL;
  if (simulation_owner_contains(released, last_customer_count_poi))
    last_customer_count_poi = NULL;
  if (simulation_owner_contains(released, last_recipe_ptr))
    last_recipe_ptr = NULL;
  if (simulation_owner_contains(released, restaurant_with_guests))
    restaurant_with_guests = NULL;
  if (simulation_owner_contains(released, restaurant_hit_test_owner))
    restaurant_hit_test_owner = NULL;
  if (simulation_owner_contains(released, recipe_lookup_fallback))
    recipe_lookup_fallback = NULL;
  if (simulation_owner_contains(released, ptr_apple))
    ptr_apple = NULL;
  if (simulation_owner_contains(released, curr_rest))
    curr_rest = NULL;
  if (simulation_owner_contains(released, mouse_result))
    mouse_result = NULL;
  if (jatoba_1 && simulation_owner_contains(released, jatoba_1->mouse))
    jatoba_1->mouse = NULL;
  pt_placement_contacts_retire(released->native, released->native_extent);
  const PTSavedBufferOwner bytes = {.native = released->native, .size = (u32)released->native_extent};
  retire_native_buffer_aliases(&bytes);
}
static void free_resource_owner_metadata(PTSavedResourceOwner *owner) {
  free(owner->published_native);
  free(owner->published_saved);
  free(owner->pending_pointers);
  free(owner->active_indices);
  free(owner->initialized);
  free(owner->owned);
  free(owner->pointer_overrides);
  free(owner);
}
static void remove_simulation_children(PTSavedResourceOwner *parent) {
  PTSavedResourceOwner **link = &saved_resources;
  while (*link) {
    PTSavedResourceOwner *child = *link;
    if (child->container != parent) {
      link = &child->next;
      continue;
    }
    remove_simulation_children(child);
    // A recursive removal can change the list head or its predecessor.

    link = &saved_resources;
    while (*link != child) {
      if (!*link)
        abort();
      link = &(*link)->next;
    }
    *link = child->next;
    free_resource_owner_metadata(child);
  }
}
static void release_saved_simulation(const void *native, size_t size) {
  if (!native)
    return;
  size_t index;
  PTSavedResourceOwner *owner = resource_owner(native, &index);
  if (!owner || index || !owner->allocated || owner->count != 1 || !size || size > owner->native_extent) {
    fputs("Pizza Tycoon: release requires one complete live simulation object\n", stderr);
    abort();
  }
  clear_simulation_references(owner);
  void *storage = owner->wire;
  void *object = owner->native;
  remove_simulation_children(owner);
  PTSavedResourceOwner **link = &saved_resources;
  while (*link != owner) {
    if (!*link)
      abort();
    link = &(*link)->next;
  }
  *link = owner->next;
  free_resource_owner_metadata(owner);
  // Releasing an identity never serializes a dead record or keeps its C
  // allocation alive. A later save observes only surviving native records.

  release_saved_storage(storage);
  free(object);
}
static void bind_native_resource_defaults(PTSavedResourceOwner *owner, size_t index) {
  const PTSavedResourceSpec *spec = &resource_specs[owner->kind];
  activate_resource_record(owner, index);
  const u8 *native = owner->native + index * spec->native_size;
  u8 *history = owner->published_native + index * spec->native_size;
  u8 *saved = owner->published_saved + index * spec->saved_size;
  memcpy(saved, (owner->wire + (u32)(index * spec->saved_size)), spec->saved_size);
  for (size_t i = 0; i < spec->count; ++i) {
    const PTSavedResourceField *field = &spec->fields[i];
    if ((field->target == PT_SAVED_FONT || field->target == PT_SAVED_BITMAP || field->target == PT_SAVED_CANVAS ||
            field->target == PT_SAVED_JATOBA || field->target == PT_SAVED_PALETTE_SLOT) &&
        !tracked_resource_pointer(owner->kind, field))
      continue;
    owner->owned[index * spec->count + i] = 1;
    if (field->target == PT_SAVED_VALUE) {
      memcpy(history + field->native_offset, native + field->native_offset, field->width);
      memcpy(saved + field->saved_offset, native + field->native_offset, field->width);
    } else {
      u32 word;
      if (!resource_pointer_word(owner, index, field, &word))
        abort();
      memcpy(history + field->native_offset, &word, sizeof(word));
      memcpy(saved + field->saved_offset, &word, sizeof(word));
    }
  }
}
// Each static object or array has one wire identity and record extent.
// The order also preserves registration of embedded restaurant rows.

typedef struct PTSavedResourceBinding {
  PTResourceKind kind;
  u32 address;
  void *native;
  size_t count, capacity;
} PTSavedResourceBinding;
static const PTSavedResourceBinding saved_resource_bindings[] = {
  {PT_RESOURCE_KIND_MOUSEINITIALIZER, PT_FIELD_mouse_initializer, &mouse_initializer, 1, 1},
  {PT_RESOURCE_KIND_PIG, PT_FIELD_walking_guest, &walking_guest, 1, 1},
  {PT_RESOURCE_KIND_PIG, PT_FIELD_g_pigs, g_pigs, 2, 2},
  {PT_RESOURCE_KIND_GAME, PT_FIELD_game, &game, 1, 1},
  {PT_RESOURCE_KIND_PLAYER, PT_FIELD_players, players, 8, 8},
  {PT_RESOURCE_KIND_TOWN, PT_FIELD_durian, &durian, 1, 1},
  {PT_RESOURCE_KIND_REST, PT_FIELD_temp_rest, &temp_rest, 1, 1},
  {PT_RESOURCE_KIND_APPLE, PT_FIELD_room, &room, 1, 1},
  {PT_RESOURCE_KIND_IGLOO_LINK, PT_FIELD_temp_igloo_link, &temp_igloo_link, 1, 1},
  {PT_RESOURCE_KIND_IGLOO_LINK, PT_FIELD_map_igloo_link, &map_igloo_link, 1, 1},
  {PT_RESOURCE_KIND_HAZELNUT, PT_FIELD_hazelnuts, hazelnuts, 150, 150},
  {PT_RESOURCE_KIND_HAZELNUT, PT_FIELD_temp_hazel, &temp_hazel, 1, 1},
  {PT_RESOURCE_KIND_LANG_FILE_DESC, PT_FIELD_langfile_artikel3, &langfile_artikel3, 1, 1},
  {PT_RESOURCE_KIND_LANG_FILE_DESC, PT_FIELD_langfile_credits, &langfile_credits, 1, 1},
  {PT_RESOURCE_KIND_LANG_FILE_DESC, PT_FIELD_langfile_ende, &langfile_ende, 1, 1},
  {PT_RESOURCE_KIND_LANG_FILE_DESC, PT_FIELD_langfile_gtalk0, &langfile_gtalk0, 1, 1},
  {PT_RESOURCE_KIND_LANG_FILE_DESC, PT_FIELD_langfile_gtalk1, &langfile_gtalk1, 1, 1},
  {PT_RESOURCE_KIND_LANG_FILE_DESC, PT_FIELD_langfile_markt, &langfile_markt, 1, 1},
  {PT_RESOURCE_KIND_LANG_FILE_DESC, PT_FIELD_langfile_action, &langfile_action, 1, 1},
  {PT_RESOURCE_KIND_LANG_FILE_DESC, PT_FIELD_langfile_werbung, &langfile_werbung, 1, 1},
  {PT_RESOURCE_KIND_LANG_FILE_DESC, PT_FIELD_langfile_levent, &langfile_levent, 1, 1},
  {PT_RESOURCE_KIND_LANG_FILE_DESC, PT_FIELD_langfile_mafia, &langfile_mafia, 1, 1},
  {PT_RESOURCE_KIND_LANG_FILE_DESC, PT_FIELD_langfile_waffen, &langfile_waffen, 1, 1},
  {PT_RESOURCE_KIND_LANG_FILE_DESC, PT_FIELD_langfile_realtors, &langfile_realtors, 1, 1},
  {PT_RESOURCE_KIND_LANG_FILE_DESC, PT_FIELD_langfile_bank, &langfile_bank, 1, 1},
  {PT_RESOURCE_KIND_LANG_FILE_DESC, PT_FIELD_langfile_insurance, &langfile_insurance, 1, 1},
  {PT_RESOURCE_KIND_LANG_FILE_DESC, PT_FIELD_langfile_townhall, &langfile_townhall, 1, 1},
  {PT_RESOURCE_KIND_LANG_FILE_DESC, PT_FIELD_langfile_police, &langfile_police, 1, 1},
  {PT_RESOURCE_KIND_LANG_FILE_DESC, PT_FIELD_langfile_lief, &langfile_lief, 1, 1},
  {PT_RESOURCE_KIND_LANG_FILE_DESC, PT_FIELD_langfile_leben, &langfile_leben, 1, 1},
  {PT_RESOURCE_KIND_LANG_FILE_DESC, PT_FIELD_langfile_pers, &langfile_pers, 1, 1},
  {PT_RESOURCE_KIND_LANG_FILE_DESC, PT_FIELD_langfile_persf, &langfile_persf, 1, 1},
  {PT_RESOURCE_KIND_LANG_FILE_DESC, PT_FIELD_langfile_staff_applications, langfile_staff_applications, 5, 5},
  {PT_RESOURCE_KIND_LANG_FILE_DESC, PT_FIELD_langfile_double, &langfile_double, 1, 1},
  {PT_RESOURCE_KIND_IGLOO, PT_FIELD_igloos, igloos, 150, 150},
  {PT_RESOURCE_KIND_IGLOO, PT_FIELD_map_igloo_tail, &map_igloo_tail, 1, 1},
  {PT_RESOURCE_KIND_IGLOO, PT_FIELD_map_igloo_head, &map_igloo_head, 1, 1},
  {PT_RESOURCE_KIND_IGLOO, PT_FIELD_temp_igloo_1, &temp_igloo_1, 1, 1},
  {PT_RESOURCE_KIND_IGLOO, PT_FIELD_temp_igloo_2, &temp_igloo_2, 1, 1},
  {PT_RESOURCE_KIND_IGLOO, PT_FIELD_temp_igloo, &temp_igloo, 1, 1},
  {PT_RESOURCE_KIND_BOB_HOST, PT_FIELD_bob_restaurant, &bob_restaurant, 1, 1},
  {PT_RESOURCE_KIND_BOB_HOST, PT_FIELD_bob_ingredients, &bob_ingredients, 1, 1},
  {PT_RESOURCE_KIND_BOB_HOST, PT_FIELD_bob_kitchen, &bob_kitchen, 1, 1},
  {PT_RESOURCE_KIND_BOB_HOST, PT_FIELD_bob_restaurant_open, &bob_restaurant_open, 1, 1},
  {PT_RESOURCE_KIND_BOB_HOST, PT_FIELD_bob_stanbobs, &bob_stanbobs, 1, 1},
  {PT_RESOURCE_KIND_BOB_HOST, PT_FIELD_bob_typ, &bob_typ, 1, 1},
  {PT_RESOURCE_KIND_BOB_HOST, PT_FIELD_bob_walk, &bob_walk, 1, 1},
  {PT_RESOURCE_KIND_BOB_HOST, PT_FIELD_bob7, &bob7, 1, 1},
  {PT_RESOURCE_KIND_BOB_HOST, PT_FIELD_bob11, &bob11, 1, 1},
  {PT_RESOURCE_KIND_BOB_HOST, PT_FIELD_menu_bob, &menu_bob, 1, 1},
  {PT_RESOURCE_KIND_BOB_HOST, PT_FIELD_bob_restaurant_stats, &bob_restaurant_stats, 1, 1},
  {PT_RESOURCE_KIND_BOB_HOST, PT_FIELD_bob9, &bob9, 1, 1},
};
static void bind_native_resource_objects(void) {
  pt_saved_global_roots_reset();
  pt_buffer_storage(NULL);
  reset_saved_buffers();
  while (saved_resources) {
    PTSavedResourceOwner *next = saved_resources->next;
    if (saved_resources->allocated)
      free(saved_resources->native);
    free_resource_owner_metadata(saved_resources);
    saved_resources = next;
  }
  pt_native_resource_defaults_apply();
  pt_native_world_defaults_apply();
  pt_native_game_defaults_apply();
  for (size_t i = 0; i < sizeof(saved_resource_bindings) / sizeof(*saved_resource_bindings); ++i) {
    const PTSavedResourceBinding *binding = &saved_resource_bindings[i];
    new_resource_owner(binding->kind, binding->address, binding->native, binding->count, binding->capacity);
  }
  for (PTSavedResourceOwner *owner = saved_resources; owner; owner = owner->next) {
    if (owner->container) {
      // The static temporary restaurant owns all 24 embedded rows,
      // including unused rows. No poisoned saved count selects them.

      if (owner->kind != PT_RESOURCE_KIND_RESTORECIPE || owner->container->native != (u8 *)&temp_rest)
        abort();
      owner->count = owner->capacity;
    }
    for (size_t i = 0; i < owner->count; ++i)
      bind_native_resource_defaults(owner, i);
  }
  const PTSimulationStorage simulation = {bind_saved_simulation, release_saved_simulation};
  pt_simulation_storage(&simulation);
  const PTBufferStorage buffers = {bind_saved_buffer, release_saved_buffer};
  pt_buffer_storage(&buffers);
  pt_world_objects_observe(saved_buffers_written);
  pt_world_objects_observe(resources_written);
}

// Saved list addresses are decoded only through established object ownership.
// Generic byte fixtures have no implied native pointer representation.

// A freshly allocated display supplies its embedded native children. Their
// original record offsets are serialization details, never C object offsets.

static void bind_native_contacts(void) {
  pt_placement_contacts_reset();
}

static void bind_native_mouse_bindings(void) {
  pt_mouse_bindings_reset();
}

static void saved_heap_bytes_written(const void *storage, size_t size) {
  if (!size)
    return;
  u32 address;
  if (saved_heap_native_span(storage, size, &address)) {
    require_saved_buffer_range(&saved_heap_buffer, address - saved_heap_buffer.address, size);
    publish_saved_buffer_span(&saved_heap_buffer, address - saved_heap_buffer.address, size);
    return;
  }
  if (saved_dos_native_span(storage, size, &address, false)) {
    for (unsigned i = 0; i < PT_DOS_POOL_COUNT; ++i) {
      PTSavedBufferOwner *owner = &saved_dos_buffers[i];
      if (address >= owner->address && address - owner->address < owner->size) {
        require_saved_buffer_range(owner, address - owner->address, size);
        publish_saved_buffer_span(owner, address - owner->address, size);
        return;
      }
    }
  }
  pt_world_objects_written(storage, size);
}

static void bind_native_sound_resources(void) {
  pt_native_sound_defaults_apply();
}

static void bind_native_world_objects(void) {
  bind_native_mouse_bindings();
  bind_native_sound_resources();
  bind_native_contacts();
}

void pt_saved_world_bind_native_defaults(void) {
  bind_native_world_objects();
}

static void retire_native_buffer_aliases(const PTSavedBufferOwner *buffer) {
  if (saved_buffer_borrows(buffer, rect_1))
    rect_1 = NULL;
  if (saved_buffer_borrows(buffer, unit_for_number_query))
    unit_for_number_query = NULL;
  if (saved_buffer_borrows(buffer, current_button_hint))
    current_button_hint = NULL;
  for (unsigned row = 0; row < sizeof(button_texts) / sizeof(*button_texts); ++row)
    for (unsigned column = 0; column < sizeof(button_texts[row]) / sizeof(*button_texts[row]); ++column)
      if (saved_buffer_borrows(buffer, button_texts[row][column]))
        button_texts[row][column] = NULL;
  for (unsigned i = 0; i < sizeof(popup_menu_items) / sizeof(*popup_menu_items); ++i)
    if (saved_buffer_borrows(buffer, popup_menu_items[i]))
      popup_menu_items[i] = NULL;
  if (saved_buffer_borrows(buffer, ingredient_background))
    ingredient_background = NULL;
  if (saved_buffer_borrows(buffer, buffer_backup))
    buffer_backup = NULL;
  if (saved_buffer_borrows(buffer, buffer_2))
    buffer_2 = NULL;
  if (saved_buffer_borrows(buffer, newspaper_article_buffer))
    newspaper_article_buffer = NULL;
  if (saved_buffer_borrows(buffer, pt_sound_resources.bank))
    pt_sound_resources.bank = NULL;
  if (saved_buffer_borrows(buffer, pt_sound_resources.fm_status))
    pt_sound_resources.fm_status = NULL;
  if (saved_buffer_borrows(buffer, pt_sound_resources.music))
    pt_sound_resources.music = NULL;
  if (saved_buffer_borrows(buffer, pt_sound_resources.cursor))
    pt_sound_resources.cursor = NULL;
  if (saved_buffer_borrows(buffer, pt_sound_resources.driver))
    pt_sound_resources.driver = NULL;
  if (saved_buffer_borrows(buffer, pt_sound_resources.vector))
    pt_sound_resources.vector = NULL;
  if (saved_buffer_borrows(buffer, pt_sound_resources.repeat_start))
    pt_sound_resources.repeat_start = NULL;
  if (saved_buffer_borrows(buffer, pt_sound_resources.output))
    pt_sound_resources.output = NULL;
  if (saved_buffer_borrows(buffer, pt_sound_resources.repeat_end))
    pt_sound_resources.repeat_end = NULL;
  if (saved_buffer_borrows(buffer, pt_sound_resources.samples))
    pt_sound_resources.samples = NULL;
  for (unsigned i = 0; i < sizeof(pt_sound_resources.dos_blocks) / sizeof(*pt_sound_resources.dos_blocks); ++i)
    if (saved_buffer_borrows(buffer, pt_sound_resources.dos_blocks[i]))
      pt_sound_resources.dos_blocks[i] = NULL;
  for (PTSavedResourceOwner *owner = saved_resources; owner; owner = owner->next) {
    if (owner->kind != PT_RESOURCE_KIND_PLAYER)
      continue;
    for (size_t i = 0; i < owner->count; ++i) {
      Player *player = (Player *)(void *)owner->native + i;
      for (unsigned town = 0; town < sizeof(player->depots) / sizeof(*player->depots); ++town)
        if (saved_buffer_borrows(buffer, player->depots[town]))
          player->depots[town] = NULL;
    }
  }
  if (saved_buffer_borrows(buffer, walking_guest.character))
    walking_guest.character = NULL;
  for (unsigned i = 0; i < sizeof(g_pigs) / sizeof(*g_pigs); ++i)
    if (saved_buffer_borrows(buffer, g_pigs[i].character))
      g_pigs[i].character = NULL;
}

// Typed field-store dispatch is private to save/compare adapters.

// Savegame io.

#include "graphics.h"
#include "initial_data.h"
#include "character.h"
#include "objects.h"
#include <string.h>
#include <assert.h>
#include "finance.h"
#include "text.h"
#include "game.h"
#include "world.h"
#include "recipe.h"
#include "player.h"
#include "messages.h"
#include "runtime_state.h"

// Town counts are loaded before any count-dependent byte extent is used.

static u32 town_staff_bytes(void) {
  if (ramtown.num_staff >= sizeof(g_staff) / sizeof(*g_staff))
    abort();
  return (u32)ramtown.num_staff * sizeof(Staff);
}
typedef enum PTTownReadStep { PT_TOWN_READ_NONE, PT_TOWN_READ_COUNTS, PT_TOWN_READ_STAFF } PTTownReadStep;
typedef enum PTTownStateBlock {
#define TOWN_STATE_BLOCK(id, data, size, dynamic_size, after_read) PT_TOWN_##id,
#include "src/savegame_blocks.def"
#undef TOWN_STATE_BLOCK
  PT_TOWN_STATE_BLOCKS
} PTTownStateBlock;
static const struct {
  PTSavegameDataBlock block;
  u32 (*dynamic_size)(void);
  PTTownReadStep after_read;
} town_state_sections[] = {
#define TOWN_STATE_BLOCK(id, data, size, dynamic_size, after_read)                                                     \
  [PT_TOWN_##id] = {{(u8 *)(data), size, false}, dynamic_size, after_read},
#include "src/savegame_blocks.def"
#undef TOWN_STATE_BLOCK
};
_Static_assert(
    sizeof(town_state_sections) / sizeof(*town_state_sections) == PT_TOWN_STATE_BLOCKS, "town state section count");
static PTSavegameDataBlock town_state_block(unsigned id) {
  PTSavegameDataBlock block = town_state_sections[id].block;
  if (town_state_sections[id].dynamic_size)
    block.size = town_state_sections[id].dynamic_size();
  return block;
}
typedef struct __attribute__((packed, may_alias)) PTTownSavedTrends {
  i8 current[96], future[96];
} PTTownSavedTrends;
_Static_assert(sizeof(PTTownSavedTrends) == 192, "saved town trend extent");
_Static_assert(offsetof(Town, stockarr90A) == offsetof(Town, ingredient_trend) + 96, "saved trend columns adjacent");
static inline PTTownSavedTrends *pt_town_load_trends(Town *town) {
  return (PTTownSavedTrends *)(void *)&town->ingredient_trend;
}

// On disk these three sections precede supplier records in both directions.

typedef enum PTTownHeaderBlock {
#define TOWN_HEADER_BLOCK(id, data, size) PT_TOWN_HEADER_##id,
#include "src/savegame_blocks.def"
#undef TOWN_HEADER_BLOCK
  PT_TOWN_HEADER_BLOCKS
} PTTownHeaderBlock;
static void town_header_blocks(Town *town, PTSavegameDataBlock output[PT_TOWN_HEADER_BLOCKS]) {
  const PTSavegameDataBlock blocks[] = {
#define TOWN_HEADER_BLOCK(id, data, size) [PT_TOWN_HEADER_##id] = {(u8 *)(data), size, false},
#include "src/savegame_blocks.def"
#undef TOWN_HEADER_BLOCK
  };
  _Static_assert(sizeof(blocks) / sizeof(*blocks) == PT_TOWN_HEADER_BLOCKS, "town header blocks");
  memcpy(output, blocks, sizeof(blocks));
}

typedef u32 (*PTSavedReadBytes)(void *, u32);
typedef u32 (*PTSavedWriteBytes)(const void *, u32);
static u32 read_saved_bytes(void *destination, u32 size, PTSavedReadBytes read_bytes) {
  u32 wire_size;
  void *wire = pt_saved_io_buffer(destination, size, &wire_size);
  pt_saved_reference_import_begin();
  u32 result = read_bytes(wire, wire_size);
  if (result > wire_size)
    abort();
  pt_saved_io_bytes_written(wire, result);
  pt_saved_reference_import_end();
  return result;
}
static u32 write_saved_bytes(const void *source, u32 size, PTSavedWriteBytes write_bytes) {
  u32 wire_size;
  const void *wire = pt_saved_io_buffer(source, size, &wire_size);
  return write_bytes(wire, wire_size);
}

Recipe *pt_recipe_allocate_file(u32 file_size) {
  return pt_saved_allocate_recipe(file_size);
}
u32 pt_recipe_read_file_bytes(Recipe *recipe, u32 file_size, PTSavedReadBytes read_bytes) {
  return read_saved_bytes(pt_saved_recipe_record(recipe), file_size, read_bytes);
}
u32 pt_recipe_file_checksum(const Recipe *recipe, u32 file_size, PTSavedWriteBytes checksum) {
  u32 wire_size;
  const void *wire = pt_saved_io_buffer(pt_saved_recipe_record(recipe), file_size, &wire_size);
  return checksum(wire, wire_size);
}
u32 pt_recipe_saved_checksum(const Recipe *recipe, u32 native_size, PTSavedWriteBytes checksum) {
  if (native_size < sizeof(*recipe))
    abort();
  size_t entries = native_size - sizeof(*recipe);
  if (entries % sizeof(RecipeEntry) || entries / sizeof(RecipeEntry) > recipe->entry_capacity)
    abort();
  u32 file_size = pt_saved_simulation_byte_count(PT_SIM_RECIPE, native_size);
  return pt_recipe_file_checksum(recipe, file_size, checksum);
}

// encoded file

// The selected repair initializes partial input headers deterministically.

// Fixed wire headers and encoded payloads need no second record conversion.

u32 WriteEncodedBytesToCurrentFile(const u8 *source, u32 size) {
  // The DOS wire format uses the serialized source identity as its key.

  source = pt_saved_io_buffer(source, size, &size);
  EncodedFileHeader header = {pt_buffer_to_guest(source)};
  u32 key = header.source_pointer >> 2;
  u8 *encoded = AllocateBufferOrMessage(size);
  WriteBytesToCurrentFile(&header, sizeof(header));
  const u8 *cursor = source;
  for (u32 i = 0; i < size; ++i) {
    encoded[i] = (u8)(*cursor + key + i);
    ++cursor;
  }
  WriteBytesToCurrentFile(encoded, size);
  pt_buffer_release(encoded);
  return size;
}
u32 ReadEncodedBytesFromCurrentFile(u8 *destination, u32 size) {
  destination = pt_saved_io_buffer(destination, size, &size);
  EncodedFileHeader header;
  header.source_pointer = 0;
  read_saved_bytes(&header, sizeof(header), ReadBytesFromCurrentFile);
  u32 key = header.source_pointer >> 2;
  pt_saved_encoded_input_begin(destination, size);
  ReadBytesFromCurrentFile(destination, size);
  for (u32 i = 0; i < size; ++i)
    destination[i] = (u8)(destination[i] - key - i);
  pt_saved_encoded_input_end();
  pt_saved_io_bytes_written(destination, size);
  return size;
}

// Save sections use the same transfer policy in both directions. The raw
// operations still translate native records through the wire codec.

static u32 read_plain_block(u8 *data, u32 size) {
  return read_saved_bytes(data, size, ReadBytesFromCurrentFile);
}
static u32 write_plain_block(const u8 *data, u32 size) {
  return write_saved_bytes(data, size, WriteBytesToCurrentFile);
}
static const struct {
  u32 (*read)(u8 *, u32);
  u32 (*write)(const u8 *, u32);
} block_io[] = {[false] = {read_plain_block, write_plain_block},
  [true] = {ReadEncodedBytesFromCurrentFile, WriteEncodedBytesToCurrentFile}};
static u32 write_block(const PTSavegameDataBlock *block) {
  return block_io[block->encoded].write(block->data, block->size);
}

// character file

u32 WriteCharacterRecord(u8 player) {
  Character *character = pt_character_record_slot(player);

  write_saved_bytes(&(*pt_character_record_index(player)), 1, WriteBytesToCurrentFile);

  u32 sum = ComputeCharacterChecksum(character);

  character->checksum = pt_signed_dword(sum);

  u32 result = WriteEncodedBytesToCurrentFile((const u8 *)character, sizeof(Character));

  return result;
}
u32 SaveCharacterFile(const u8 *filename, u8 player) {
  u8 text[256];
  size_t length = strlen((const char *)filename);
  assert(length + 5 <= sizeof(text));
  memcpy(text, filename, length);

  memcpy(strbuf, text, length);
  memcpy(strbuf + length, ".chr", 5);

  OpenFileForWritingAsCurrent(strbuf);

  WriteCharacterRecord(player);

  return CloseCurrentWritingFile();
}
PTCharacterRecordLoad LoadCharacterRecordFile(const u8 *filename, u8 player) {
  Character *character = pt_character_record_slot(player);
  PTCharacterRecordLoad result = {0};

  u32 opened = OpenFileAsCurrent(filename);

  if ((u16)opened) {
    read_saved_bytes(&result.index, 1, ReadBytesFromCurrentFile);

    ReadEncodedBytesFromCurrentFile((u8 *)character, sizeof(Character));

    CloseCurrentFile();

    u32 expected = (u32)character->checksum;

    result.checksum = ComputeCharacterChecksum(character);

    if (result.checksum != expected) {
      memset(character, 0, sizeof(Character));
      result.cleared_record = character;
    }
  }

  return result;
}

// Load a personal portrait and record, copy cash and restore the active player.

void LoadPersonalCharacterImage(const u8 *name, u8 player) {
  u8 previous_player;

  previous_player = current_player_index;

  SetCurrentPlayer(player);

  u8 base_name[256];
  size_t name_length = strlen((const char *)name);
  assert(name_length + 8 <= sizeof(strbuf));
  memcpy(base_name, name, name_length + 1);
  snprintf((char *)strbuf, sizeof(strbuf), "chr\\%s.pc", (const char *)base_name);

  OpenFileAsCurrent(strbuf);

  read_saved_bytes(&loaded_character, sizeof(CharacterFileImage) + sizeof(Character), ReadBytesFromCurrentFile);

  CloseCurrentFile();

  LoadBitmapFromCharacterFile();

  memmove(pt_character_record_slot(player), &temp_character, sizeof(Character));

  const Character *character = character_ptr;
  player_ptr->money = character->money;
  players[player].character_index = 255;
  players[player].name = 0;

  SetCurrentPlayer(previous_player);
}

// town file

// Original 48240 exports the property geometry, a contiguous 32-byte floor
// block, both counts, and the used furniture/oven records. It is separate
// from a full game save. The restaurant is not read if opening fails.

size_t pt_town_filename(u8 town, u8 text[5]) {
  size_t n = 0;
  text[n++] = 'c';
  if (town >= 100)
    text[n++] = (u8)('0' + town / 100);
  if (town >= 10)
    text[n++] = (u8)('0' + town / 10 % 10);
  text[n++] = (u8)('0' + town % 10);
  text[n] = 0;
  return n;
}

u32 PrintCityFilename(u8 town) {
  u8 text[5];
  size_t length = pt_town_filename(town, text);

  memcpy(strbuf, text, length + 1);

  return (u32)length;
}
Game *SaveTempTownFileToSavegame(u8 town) {
  DisableDiskReadErrors();

  PrintCityFilename(town);

  OpenFileAsCurrent(strbuf);

  num_read_bytes = 0;
  u32 requested = TOWN_COPY_CHUNK;
  while (requested) {
    u8 *buffer = pt_buffer_allocate(requested, false);

    if (!buffer) {
      break;
    }

    requested = read_saved_bytes(buffer, requested, ReadBytesFromCurrentFile);

    write_saved_bytes(buffer, requested, WriteBytesToCurrentFile);

    pt_buffer_release(buffer);
  }

  CloseCurrentFile();

  Game *result = EnableDiskReadErrors();

  return result;
}
u32 LoadTownFromSaveGame(u8 town) {
  u32 remaining = (u32)ptr_apple->town_sizes[town];

  PrintCityFilename(town);

  OpenFileForWritingAsCurrent(strbuf);

  while (remaining) {
    u32 requested = remaining < TOWN_COPY_CHUNK ? remaining : TOWN_COPY_CHUNK;

    u8 *buffer = pt_buffer_allocate(requested, false);

    if (!buffer) {
      break;
    }

    u32 received = read_saved_bytes(buffer, requested, ReadBytesFromCurrentFile);

    write_saved_bytes(buffer, received, WriteBytesToCurrentFile);

    pt_buffer_release(buffer);

    // Preserve the original request-based countdown.
    remaining -= requested;
  }

  u32 result = CloseCurrentWritingFile();

  return result;
}
u32 SaveRecipeRecord(const Recipe *recipe) {
  u32 size = sizeof(PTSavedRecipe) + sizeof(RecipeEntry) * recipe->num_entries;

  WriteBytesToCurrentFile(&size, sizeof(size));

  u32 result = write_saved_bytes((const u8 *)pt_saved_recipe_record(recipe), size, WriteBytesToCurrentFile);

  return result;
}

// town save

u32 ReadFromFileSkipLinkHeader(void *record, u32 size) {
  record = pt_saved_io_buffer(record, size, &size);
  u32 result;
  result = read_saved_bytes(
      pt_saved_link_record_body(record), size - (u32)sizeof(PTSavedLinkedElem), ReadBytesFromCurrentFile);
  return result;
}
// Serialize once, then write the resulting wire body directly.

u32 SaveStructWithoutLinkHeader(const void *record, u32 size) {
  record = pt_saved_io_buffer(record, size, &size);
  u32 result;
  result = WriteBytesToCurrentFile(pt_saved_link_record_body(record), size - (u32)sizeof(PTSavedLinkedElem));
  return result;
}
u32 WriteBezFile(void) {
  u32 result = 0;
  for (unsigned i = 0; i < 15; ++i) {
    result = write_saved_bytes(loaded_bez_file[i], sizeof(loaded_bez_file[i]), WriteBytesToCurrentFile);
  }
  return result;
}
u32 WriteHeaderAndSuppliers(void) {
  u8 suppliers = 0;
  Town *t = current_town;
  u32 count = 0;
  for (const PTForwardLink *node = t->poi.begin.next; node->next; node = node->next)
    ++count;
  t->num_poi = (i16)count;
  u32 result;
  PTSavegameDataBlock header[PT_TOWN_HEADER_BLOCKS];
  town_header_blocks(t, header);
  for (unsigned i = 0; i < PT_TOWN_HEADER_BLOCKS; ++i)
    result = write_block(&header[i]);
  for (const PTForwardLink *node = t->suppliers.begin.next; node->next; node = node->next) {
    ++suppliers;
    result = SaveStructWithoutLinkHeader(node, sizeof(Supplier));
    if (suppliers > 50)
      break;
  }
  return result;
}

u32 SavePlayerRestaurant(Rest *r) {
  const Recipe *saved[24];
  u8 count = 0;
  while (count < r->num_recipes) {
    unsigned i = count;
    saved[i] = r->recipes[i].recipe;
    pt_saved_store_reference_word(&r->recipes[i],
        PT_RESOURCE_KIND_RESTORECIPE,
        PT_RESOURCE_SIM_RESTORECIPE_RECIPE,
        (u32)GetRecipeId(player_ptr, saved[i]));
    ++count;
  }
  const Poi *property = r->property;
  pt_saved_store_reference_word(
      r, PT_RESOURCE_KIND_REST, PT_RESOURCE_SIM_REST_PROPERTY, ((u32)property->xcoord << 8) | property->ycoord);
  u32 result;
  result = SaveStructWithoutLinkHeader(r, PT_HUMAN_RESTAURANT_ALLOCATION_SIZE);
  pt_saved_store_reference_word(r, PT_RESOURCE_KIND_REST, PT_RESOURCE_SIM_REST_PROPERTY, pt_buffer_to_guest(property));
  // Preserve both restoration passes and the original incidental DWORD return.

  for (unsigned loop = 0; loop < 2; ++loop) {
    count = 0;
    result &= UINT32_C(0xffffff00);
    while (count < r->num_recipes) {
      unsigned i = count;
      u32 recipe = pt_buffer_to_guest(saved[i]);
      pt_saved_store_reference_word(
          &r->recipes[i], PT_RESOURCE_KIND_RESTORECIPE, PT_RESOURCE_SIM_RESTORECIPE_RECIPE, recipe);
      ++count;
      result = (loop ? recipe : 0u) & UINT32_C(0xffffff00);
      result |= count;
    }
  }
  return result;
}
u32 SaveAiRestaurant(Rest *r) {
  const Poi *property = r->property;
  pt_saved_store_reference_word(
      r, PT_RESOURCE_KIND_REST, PT_RESOURCE_SIM_REST_PROPERTY, ((u32)property->xcoord << 8) | property->ycoord);
  u32 result;
  result = SaveStructWithoutLinkHeader(r, PT_AI_RESTAURANT_SIZE);
  pt_saved_store_reference_word(r, PT_RESOURCE_KIND_REST, PT_RESOURCE_SIM_REST_PROPERTY, pt_buffer_to_guest(property));
  return result;
}
u32 SaveCurrTownToTempFileInner(void) {
  for (unsigned player = 0; player < 8; ++player)
    if (player < game_ptr->num_interactive_players) {
      WriteEncodedBytesToCurrentFile((const u8 *)&players[player].city_part, sizeof(PlayerDialledNumbers));
    }
  WriteBezFile();
  WriteHeaderAndSuppliers();
  for (unsigned player = 0; player < 8; ++player) {
    const Player *p = &players[player];
    ramtown.num_contracts_per_player[player] = p->num_contracts;
    ramtown.num_restaurant_per_player[player] = p->num_restaurants;
  }
  for (unsigned i = 0; i < PT_TOWN_STATE_BLOCKS; ++i) {
    PTSavegameDataBlock block = town_state_block(i);
    write_block(&block);
  }
  for (const PTForwardLink *node = current_town->poi.begin.next; node->next; node = node->next) {
    SaveStructWithoutLinkHeader(node, sizeof(Poi));
  }
  unsigned player = 0;
  u32 result = 0;
  while (player < 8) {
    const Player *p = &players[player];
    for (PTForwardLink *node = p->restaurants.begin.next; node->next; node = node->next) {
      if (player < game_ptr->num_interactive_players)
        SavePlayerRestaurant((Rest *)node);
      else
        SaveAiRestaurant((Rest *)node);
    }
    result = game_ptr->num_interactive_players;
    if (player < game_ptr->num_interactive_players)
      for (PTForwardLink *node = p->contracts.begin.next; node->next; node = node->next) {
        Contract *contract = (Contract *)node;
        const Supplier *supplier = contract->supplier;
        pt_saved_store_reference_word(
            contract, PT_RESOURCE_KIND_CONTRACT, PT_RESOURCE_SIM_CONTRACT_SUPPLIER, supplier->supplier_id);
        result = SaveStructWithoutLinkHeader(contract, sizeof(Contract));
        pt_saved_store_reference_word(
            contract, PT_RESOURCE_KIND_CONTRACT, PT_RESOURCE_SIM_CONTRACT_SUPPLIER, pt_buffer_to_guest(supplier));
      }
    ++player;
  }
  return result;
}
u32 SaveCurrTownToTempFile(void) {
  PrintCityFilename(ptr_apple->town_index);
  u32 result;
  result = OpenFileForWritingAsCurrent(strbuf);
  if (!(u16)result)
    return result & UINT32_C(0xffffff00);
  num_written_bytes = 0;
  SaveCurrTownToTempFileInner();
  ptr_apple->town_sizes[ptr_apple->town_index] = pt_signed_dword(num_written_bytes);
  result = CloseCurrentWritingFile();
  return (result & UINT32_C(0xffffff00)) | 1u;
}
u32 SaveGame(const u8 *filename) {
  u32 result;
  result = SaveCurrTownToTempFile();
  if (!(u8)result) {
    return result;
  }
  strcpy((char *)strbuf, (const char *)filename);
  strcat((char *)strbuf, ".gam");
  result = OpenFileForWritingAsCurrent(strbuf);
  if (!(u16)result) {
    return result;
  }
  num_written_bytes = 0;
  PTSavegameDataBlock blocks[PT_SAVEGAME_BLOCKS];
  unsigned n = pt_savegame_data_blocks(game_ptr, ptr_apple, blocks);
  for (unsigned i = 0; i < n; ++i) {
    write_block(&blocks[i]);
  }
  unsigned player = 0;
  player_ptr = &players[0];
  while (player < 8) {
    if (player < game_ptr->num_interactive_players) {
      const Player *p = &players[player];
      for (const PTForwardLink *node = p->recipes.begin.next; node->next; node = node->next) {
        SaveRecipeRecord((const Recipe *)node);
      }
      unsigned city = 0;
      while (city < 10) {
        const Depot *depot = p->depots[city];
        if (depot) {
          write_saved_bytes(depot, sizeof(Depot), WriteBytesToCurrentFile);
        }
        ++city;
      }
    }
    ++player;
    player_ptr = &players[player];
  }
  SetCurrentPlayer(current_player_index);
  unsigned town = 0;
  while (town < 10) {
    if (ptr_apple->is_town_inited[town]) {
      SaveTempTownFileToSavegame((u8)town);
    }
    ++town;
  }
  result = CloseCurrentWritingFile();
  return result;
}

// town load

Game *ReadMapFile(void) {
  DisableDiskReadErrors();
  for (unsigned row = 0; row < sizeof(loaded_map_file) / sizeof(loaded_map_file[0]); ++row) {
    read_saved_bytes(loaded_map_file[row], sizeof(loaded_map_file[row]), ReadBytesFromCurrentFile);
  }
  Game *result;
  result = EnableDiskReadErrors();
  return result;
}
u32 LoadMapAndBez(u8 town) {
  did_load_map_and_bez = 1;
  for (unsigned bez = 0; bez < 2; ++bez) {
    u8 text[16];
    size_t length = pt_town_map_asset_name(town, bez != 0, text);
    memcpy(strbuf, text, length + 1);
    OpenFileAsCurrent(strbuf);
    if (bez)
      LoadBezFile();
    else
      ReadMapFile();
    CloseCurrentFile();
  }
  return 1;
}
u32 LoadBaseTownFromFile(void) {
  Town *town = current_town;
  read_saved_bytes(town, sizeof(*town), ReadBytesFromCurrentFile);
  InitTown(town);
  for (unsigned i = 0; i < town->num_zones; ++i) {
    Zone *zone;
    zone = pt_simulation_allocate(PT_SIM_ZONE, sizeof(*zone));
    read_saved_bytes(zone, sizeof(*zone), ReadBytesFromCurrentFile);
    LinkList_InsertLast(&town->zones, &zone->link);
  }
  for (unsigned i = 0; i < town->num_supplier; ++i) {
    Supplier *supplier;
    supplier = pt_simulation_allocate(PT_SIM_SUPPLIER, sizeof(*supplier));
    read_saved_bytes(supplier, sizeof(*supplier), ReadBytesFromCurrentFile);
    LinkList_InsertSorted(&town->suppliers, &supplier->link, PT_LIST_SUPPLIER_QUALITY);
  }
  u32 result;
  result = read_saved_bytes(&extra_town_data, sizeof(extra_town_data), ReadBytesFromCurrentFile);
  return result;
}
Game *LoadBaseTownFile(u8 town) {
  LoadMapAndBez(town);
  u8 text[16];
  size_t length = pt_town_city_asset_name(town, text);
  memcpy(strbuf, text, length + 1);
  LoadFileIntoFileLoaderBuffer(strbuf, 20000);
  LoadBaseTownFromFile();
  CloseCurrentFile();
  return game_ptr;
}
u32 LoadTempTownFile(u8 town) {
  LoadBaseTownFile(town);
  u8 text[5];
  size_t length = pt_town_temp_asset_name(town, text);
  memcpy(strbuf, text, length + 1);
  OpenFileAsCurrent(strbuf);
  num_read_bytes = 0;
  LoadTownFromTempFile();
  CloseCurrentFile();
  return 0;
}
u32 LoadBezFile(void) {
  u32 result = 0;
  for (unsigned row = 0; row < sizeof(loaded_bez_file) / sizeof(loaded_bez_file[0]); ++row) {
    result = read_saved_bytes(loaded_bez_file[row], sizeof(loaded_bez_file[row]), ReadBytesFromCurrentFile);
  }
  return result;
}
static void read_town_bytes(void *destination, u32 size) {
  read_saved_bytes(destination, size, ReadBytesFromCurrentFile);
}
static void *load_town_record(PTSimulationKind kind, u32 size, LinkedHead *head, bool first) {
  void *record;
  record = pt_simulation_allocate(kind, size);
  ReadFromFileSkipLinkHeader(record, size);
  if (first)
    LinkList_InsertFirst(head, record);
  else
    LinkList_InsertLast(head, record);
  return record;
}
u32 LoadTownFromTempFile(void) {
  for (unsigned player = 0; player < 8; ++player)
    if (player < game_ptr->num_interactive_players) {
      ReadEncodedBytesFromCurrentFile((u8 *)&pt_player((u8)player)->city_part, sizeof(PlayerDialledNumbers));
    }
  LoadBezFile();
  Town *town = current_town;
  PTSavegameDataBlock header[PT_TOWN_HEADER_BLOCKS];
  town_header_blocks(town, header);
  for (unsigned i = 0; i < PT_TOWN_HEADER_BLOCKS; ++i)
    read_town_bytes(header[i].data, header[i].size);
  // The original counts suppliers once before reading them.

  for (PTForwardLink *node = town->suppliers.begin.next; node->next; node = node->next) {
  }
  for (PTForwardLink *node = town->suppliers.begin.next; node->next; node = node->next) {
    ReadFromFileSkipLinkHeader(node, sizeof(Supplier));
  }
  Player *p;
  for (unsigned section = 0; section < PT_TOWN_STATE_BLOCKS; ++section) {
    PTSavegameDataBlock block = town_state_block(section);
    read_town_bytes(block.data, block.size);
    switch (town_state_sections[section].after_read) {
    case PT_TOWN_READ_COUNTS:
      p = players;
      for (unsigned player = 0; player < 8; ++player) {
        p->num_restaurants_in_city[ptr_apple->town_index] = ramtown.num_restaurant_per_player[player];
        p->num_restaurants = ramtown.num_restaurant_per_player[player];
        p->num_contracts = ramtown.num_contracts_per_player[player];
        ++p;
      }
      break;
    case PT_TOWN_READ_STAFF:
      for (unsigned i = (unsigned)ramtown.num_staff + 1u; i < 200; ++i) {
        memset(pt_staff_record((u8)i), 0, sizeof(Staff));
      }
      break;
    case PT_TOWN_READ_NONE:
      break;
    }
  }
  for (unsigned i = 0; i < (u16)town->num_poi; ++i)
    load_town_record(PT_SIM_POI, sizeof(Poi), &town->poi, true);
  unsigned player = 0;
  p = players;
  while (player < 8) {
    bool active = player < game_ptr->num_interactive_players;
    for (unsigned i = 0; i < p->num_restaurants; ++i)
      load_town_record(
          PT_SIM_REST, active ? PT_HUMAN_RESTAURANT_ALLOCATION_SIZE : PT_AI_RESTAURANT_SIZE, &p->restaurants, !active);
    if (active)
      for (unsigned i = 0; i < p->num_contracts; ++i) {
        Contract *contract = load_town_record(PT_SIM_CONTRACT, sizeof(Contract), &p->contracts, false);
        contract->supplier = ResolveSupplier(
            (u8)pt_saved_resource_word(contract, PT_RESOURCE_KIND_CONTRACT, PT_RESOURCE_SIM_CONTRACT_SUPPLIER));
      }
    ++player;
    ++p;
  }
  player = 0;
  p = players;
  u32 result = 0;
  while (player < 8) {
    unsigned restaurant_index = 0;
    PTForwardLink *node = p->restaurants.begin.next;
    while (restaurant_index < p->num_restaurants) {
      bool active = player < game_ptr->num_interactive_players;
      Rest *restaurant = (Rest *)(void *)node;
      (void)active;
      u32 property_id = pt_saved_resource_word(restaurant, PT_RESOURCE_KIND_REST, PT_RESOURCE_SIM_REST_PROPERTY);
      unsigned x = (property_id >> 8) & 255u;
      unsigned y = property_id & 255u;
      for (PTForwardLink *property = town->poi.begin.next; property->next; property = property->next) {
        Poi *poi = (Poi *)property;
        if (poi->ycoord != y || poi->xcoord != x)
          continue;
        poi->restaurant = restaurant;
        restaurant->property = poi;
        restaurant->player_index = (u8)player;
        loaded_map_file[poi->ycoord][poi->xcoord] = (u8)((player << 4) | 10u);
        if (active)
          for (unsigned i = 0; i < restaurant->num_recipes; ++i) {
            restaurant->recipes[i].recipe = (Recipe *)ResolveRecipe(p,
                (u8)pt_saved_resource_word(
                    &restaurant->recipes[i], PT_RESOURCE_KIND_RESTORECIPE, PT_RESOURCE_SIM_RESTORECIPE_RECIPE));
          }
      }
      node = node->next;
      ++restaurant_index;
    }
    result = p->num_restaurants;
    ++player;
    ++p;
  }
  return result;
}

// recipe load

void InitializeQuickstartRecipes(u8 id) {
  Recipe *large = AllocateRecipe(200);
  LoadRecipeFromFile(id, large);
  const u8 *name = GetDefaultRecipeName(id);
  (void)pt_strcpy(large->name, name);
  u8 entries = large->num_entries;
  large->id = id;
  Recipe *small = AllocateRecipe(entries);
  (void)pt_recipe_copy(small, large, entries);
  pt_simulation_release(large, pt_recipe_allocation_bytes(200));
  (void)LinkList_InsertSorted(&player_ptr->recipes, &small->linked, PT_LIST_RECIPE_ID);
  (void)ComputeRecipeStatistics(small);
  RecipeComputeAgeRatings(small);
  small->imitation = 100;
  ++player_ptr->num_recipes;
  last_recipe_ptr = small;
  memset(small->rating, 0, sizeof(small->rating));
}
void LoadRecipeFromFile(u8 id, Recipe *recipe) {
  u32 size = 0;
  (void)snprintf((char *)strbuf, sizeof(strbuf), "r\\r%d.pc", id);
  OpenFileAsCurrent(strbuf);
  read_saved_bytes(&size, sizeof(size), ReadBytesFromCurrentFile);
  read_saved_bytes(pt_saved_recipe_record(recipe), size, ReadBytesFromCurrentFile);
  ComputeRecipeWeights(recipe);
  CloseCurrentFile();
}
Recipe *LoadRecipe(void) {
  u32 size = 0;
  read_saved_bytes(&size, sizeof(size), ReadBytesFromCurrentFile);
  Recipe *recipe = pt_saved_allocate_recipe(size);
  if (recipe) {
    read_saved_bytes(pt_saved_recipe_record(recipe), size, ReadBytesFromCurrentFile);
    LinkList_InsertSorted(&player_ptr->recipes, &recipe->linked, PT_LIST_RECIPE_ID);
    ++player_ptr->num_recipes;
    ComputeRecipeWeights(recipe);
  }
  return recipe;
}
Recipe *LoadRecipeFromCompetitionFile(u8 id) {
  u32 size = 0;
  (void)snprintf((char *)strbuf, sizeof(strbuf), "r\\r%d.pc", id);
  OpenFileAsCurrent(strbuf);
  read_saved_bytes(&size, sizeof(size), ReadBytesFromCurrentFile);
  Recipe *recipe = pt_saved_allocate_recipe(size);
  read_saved_bytes(pt_saved_recipe_record(recipe), size, ReadBytesFromCurrentFile);
  ComputeRecipeWeights(recipe);
  recipe->id = (u8)(id + 30u);
  CloseCurrentFile();
  return recipe;
}

// savegame load

static void read_block(const PTSavegameDataBlock *block) {
  block_io[block->encoded].read(block->data, block->size);
}
// Hooks run immediately after their serialized section, before the next read.

static void normalize_loaded_session(u8 recipe_counts[8]) {
  (void)recipe_counts;
  if (ptr_apple->difficulty_level < 1 || ptr_apple->difficulty_level > 5)
    ptr_apple->difficulty_level = 3;
}
static void restore_loaded_player_lists(u8 recipe_counts[8]) {
  for (unsigned player = 0; player < 8; ++player) {
    recipe_counts[player] = players[player].num_recipes;
    InitLinkedListsInPlayer((u8)player);
  }
}
static void (*const savegame_after_read[])(u8 recipe_counts[8]) = {
#define SAVE_BLOCK(id, data, size, encoded, after_read) [PT_SAVEGAME_BLOCK_##id] = after_read,
#include "src/savegame_blocks.def"
#undef SAVE_BLOCK
};
_Static_assert(
    sizeof(savegame_after_read) / sizeof(*savegame_after_read) == PT_SAVEGAME_BLOCKS, "savegame read step count");
Game *LoadSavegame(const u8 *filename) {
  strcpy((char *)strbuf, (const char *)filename);
  u32 result;
  result = OpenFileAsCurrent(strbuf);
  if (!(u16)result) {
    return NULL;
  }
  state_slot_009 = 1;
  FreeLotsOfStuff();
  num_read_bytes = 0;
  PTSavegameDataBlock blocks[PT_SAVEGAME_BLOCKS];
  pt_savegame_data_blocks(game_ptr, ptr_apple, blocks);
  u8 recipe_counts[8];
  for (unsigned i = 0; i < PT_SAVEGAME_BLOCKS; ++i) {
    read_block(&blocks[i]);
    if (savegame_after_read[i])
      savegame_after_read[i](recipe_counts);
  }
  player_ptr = &players[0];
  for (unsigned player = 0; player < 8; ++player) {
    Player *p = &players[player];
    if (player < game_ptr->num_interactive_players) {
      unsigned recipes = recipe_counts[player];
      for (unsigned i = 0; i < recipes; ++i)
        LoadRecipe();
      for (unsigned city = 0; city < 10; ++city)
        if (pt_saved_resource_word(
                p, PT_RESOURCE_KIND_PLAYER, (PTResourceField)(PT_RESOURCE_SIM_PLAYER_DEPOTS_0_ + city))) {
          Depot *depot = (Depot *)AllocateBufferOrMessage(sizeof(Depot));
          p->depots[city] = depot;
          read_saved_bytes(depot, sizeof(Depot), ReadBytesFromCurrentFile);
        }
    } else
      p->num_recipes = recipe_counts[player];
    player_ptr = &players[player + 1];
  }
  SetCurrentPlayer(ptr_apple->player_index);
  u8 saved_town = ptr_apple->town_index;
  for (unsigned city = 0; city < 10; ++city)
    if (ptr_apple->is_town_inited[city])
      LoadTownFromSaveGame((u8)city);
  CloseCurrentFile();
  ptr_apple->town_index = saved_town;
  LoadTempTownFile(saved_town);
  game_ptr->mask_of_players = 0;
  for (unsigned player = 0; player < game_ptr->num_interactive_players; ++player)
    if (!pt_player((u8)player)->days_away)
      game_ptr->mask_of_players = (u8)(game_ptr->mask_of_players | (u8)(UINT32_C(1) << (player & 31u)));
  return game_ptr;
}

// savegame state

// Import serialized storage and rebuild its C owners. CPU continuations,

bool pt_savegame_begin_state_image(const u8 *initial, size_t size) {
  if (!initial || size != sizeof(pt_memory.bytes))
    return false;
  pt_frame_reset();

  pt_native_graphics_release();
  pt_saved_global_roots_reset();
  memcpy(pt_memory.bytes, initial, size);
  pt_platform_clock_bind(NULL);
  memcpy(pt_platform_clock(), initial + PT_BIOS_CLOCK_ADDRESS, sizeof(*pt_platform_clock()));
  pt_saved_global_values_restore();
  pt_input_reset_platform();
  pt_delay_reset_platform();
  pt_pcm_driver_reset_platform();
  pt_native_sound_storage_reset();
  pt_native_fm_storage_reset();
  pt_runtime_reset_platform();
  pt_framebuffer_reset();
  pt_saved_heap_platform_reset();
  pt_file_read_reset();
  return true;
}
// Native startup images carry platform/media storage only. Original game
// addresses and the native C defaults are resolved at this save boundary.

u32 pt_public_heap_limit(void) {
  return PT_STACK_BASE;
}
void pt_saved_global_values_seed(void) {
  global_values_active = false;
  for (size_t i = 0; i < sizeof(saved_global_values) / sizeof(*saved_global_values); ++i) {
    const PTSavedGlobalValue *field = &saved_global_values[i];
    memcpy(pt_memory.bytes + field->address, field->native, field->size);
  }
}
