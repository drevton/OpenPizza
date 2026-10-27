// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
// Runtime state.

// runtime state

#include "runtime_state.h"
#include "calendar.h"
#include "frame.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

GuestFileNode *guest_free_file_nodes, *guest_active_file_nodes;
PTRuntimeState pt_runtime;
static PTPlatformClockView local_clock;
static PTPlatformClockView *platform_clock = &local_clock;
void pt_platform_clock_bind(PTPlatformClockView *clock) {
  platform_clock = clock ? clock : &local_clock;
}
PTPlatformClockView *pt_platform_clock(void) {
  return platform_clock;
}

static void finalize_files(void *context) {
  (void)context;
  GuestFinalizeFiles();
}
static void finalize_fpu(void *context) {
  (void)context;
  GuestFini387();
}
void pt_runtime_state_reset(void) {
  GuestFileNode *lists[] = {guest_free_file_nodes, guest_active_file_nodes};
  for (unsigned i = 0; i < sizeof(lists) / sizeof(*lists); ++i) {
    GuestFileNode *node = lists[i];
    while (node) {
      GuestFileNode *next = node->next;
      free(node);
      node = next;
    }
  }
  guest_free_file_nodes = guest_active_file_nodes = NULL;
  pt_runtime = (PTRuntimeState){.finalizers = {{.priority = 1, .callback = {finalize_fpu, NULL}},
                                  {.priority = 32, .callback = {finalize_files, NULL}}}};
}

void pt_runtime_invoke(PTRuntimeCallback callback) {
  if (callback.invoke)
    callback.invoke(callback.context);
}

// A dynamic FILE follows its node in one ordinary C allocation.

GuestFileNode *pt_file_node_create(GuestFileStream *stream) {
  typedef struct PTFileNodeAllocation {
    GuestFileNode node;
    GuestFileStream stream;
  } PTFileNodeAllocation;
  PTFileNodeAllocation *allocation = calloc(1, sizeof(*allocation));
  if (!allocation)
    return NULL;
  allocation->node.stream = stream ? stream : &allocation->stream;

  return &allocation->node;
}
// Rebuild the startup open-file list from the native standard streams.
// Save images and reference heap records do not own these nodes.

bool pt_runtime_initialize_file_nodes(void) {
  if (guest_free_file_nodes || guest_active_file_nodes)
    abort();
  for (unsigned i = 0; i < PT_FILE_STREAM_COUNT; ++i) {
    if (!(guest_file_streams[i].flags & 3u))
      continue;
    GuestFileNode *node = pt_file_node_create(&guest_file_streams[i]);
    if (!node)
      return false;
    node->next = guest_active_file_nodes;
    guest_active_file_nodes = node;
  }
  return true;
}
void pt_file_node_release(GuestFileNode *node) {
  free(node);
}

_Noreturn void pt_runtime_interrupt_unavailable(u32 interrupt) {

  if (pt_frame_worker())
    pt_frame_unsupported(PT_FRAME_INTERRUPT_UNAVAILABLE);
  fprintf(stderr, "Pizza Tycoon: unavailable interrupt %u\n", (unsigned)interrupt);
  abort();
}

_Noreturn void pt_runtime_finish_process(u8 status) {

  if (pt_frame_worker())
    pt_frame_process_exit(PT_FRAME_PROCESS_EXIT);
  exit(status);
}

// file stream state

#include "state_types.h"
#include "initial_data.h"

GuestFileStream guest_file_streams[PT_FILE_STREAM_COUNT];

GuestFileStream *pt_file_stream_reset(GuestFileStream *stream) {
  *stream = (GuestFileStream){0};

  return stream;
}

bool pt_file_stream_precedes(const GuestFileStream *stream, u32 first) {
  u32 limit = first < PT_FILE_STREAM_COUNT ? first : PT_FILE_STREAM_COUNT;
  for (u32 index = 0; index < limit; ++index)
    if (stream == &guest_file_streams[index])
      return true;
  return false;
}

u8 *pt_file_buffer_data(const GuestFileStream *stream) {
  if (stream->cursor.data != stream->buffer || stream->cursor.position > stream->buffer_size)
    abort();
  if (!stream->cursor.position)
    return stream->cursor.data;
  if (!stream->cursor.data)
    abort();
  return stream->cursor.data + stream->cursor.position;
}
u32 pt_file_buffer_distance(const GuestFileStream *stream) {
  if (stream->cursor.data != stream->buffer)
    abort();
  return 0u - stream->cursor.position;
}
u8 pt_file_buffer_take(GuestFileStream *stream) {
  if (stream->cursor.position >= stream->buffer_size)
    abort();
  u8 *cursor = pt_file_buffer_data(stream);
  stream->cursor.position += 1;
  return *cursor;
}
void pt_file_buffer_append(GuestFileStream *stream, u8 value) {
  if (stream->cursor.position >= stream->buffer_size)
    abort();
  u8 *cursor = pt_file_buffer_data(stream);
  stream->cursor.position += 1;
  *cursor = value;
}
void pt_game_read_set_cursor(Game *owner, u8 *data, u32 capacity) {
  if (!data && capacity)
    abort();
  owner->read_buffer.cursor = (PTBufferCursor){data, 0};
  owner->read_buffer.capacity = capacity;
}
bool pt_game_read_available(const Game *owner, u32 size) {
  const PTGameReadBuffer *buffer = &owner->read_buffer;
  return buffer->cursor.position <= buffer->capacity && size <= buffer->capacity - buffer->cursor.position &&
      (!(size || buffer->cursor.position) || buffer->cursor.data);
}
const u8 *pt_game_read_data(const Game *owner, u32 size) {
  if (!pt_game_read_available(owner, size))
    abort();
  const PTBufferCursor *cursor = &owner->read_buffer.cursor;
  if (!cursor->position)
    return cursor->data;
  if (!cursor->data)
    abort();
  return cursor->data + cursor->position;
}

static void retire_file_cursor(GuestFileStream *stream, const void *buffer, size_t size) {
  uintptr_t cursor = (uintptr_t)stream->cursor.data;
  uintptr_t begin = (uintptr_t)buffer;
  if (!stream->cursor.data || cursor < begin)
    return;
  uintptr_t offset = cursor - begin;
  if (offset < size || (offset == size && stream->buffer == buffer))
    stream->cursor = (PTBufferCursor){0};
}
void pt_file_stream_buffer_retired(const void *buffer, size_t size) {
  for (unsigned i = 0; i < PT_FILE_STREAM_COUNT; ++i)
    retire_file_cursor(&guest_file_streams[i], buffer, size);
  GuestFileNode *lists[] = {guest_free_file_nodes, guest_active_file_nodes};
  for (unsigned i = 0; i < sizeof(lists) / sizeof(*lists); ++i)
    for (GuestFileNode *node = lists[i]; node; node = node->next)
      retire_file_cursor(node->stream, buffer, size);
}

static const GuestFileStream initial_file_streams[PT_FILE_STREAM_COUNT] = {[0] = {.flags = 1},
  [1] = {.flags = 2, .handle = 1},
  [2] = {.flags = 2, .buffer_flags = 4, .handle = 2},
  [3] = {.flags = 3, .handle = 3},
  [4] = {.flags = 2, .handle = 4}};
void pt_native_file_defaults_apply(void) {
  memcpy(guest_file_streams, initial_file_streams, sizeof(guest_file_streams));
}

// process tables

#include "memory.h"
PTFileModeSlot *guest_io_mode_table;
static size_t io_mode_count;
static const u8 **environment;
static size_t environment_count;
static bool environment_present;

void pt_process_tables_reset(void) {
  guest_io_mode_table = NULL;
  io_mode_count = 0;
  pt_process_environment_import(NULL, 0);
}

PTFileModeSlot *pt_file_mode(u32 handle) {
  if (!guest_io_mode_table || handle >= guest_io_mode_capacity || handle >= io_mode_count)
    abort();
  return &guest_io_mode_table[handle];
}
void pt_file_mode_set(u32 handle, u32 mode) {
  PTFileModeSlot *entry = pt_file_mode(handle);
  entry->mode = mode;
}
void pt_file_mode_or(u32 handle, u32 bits) {
  pt_file_mode_set(handle, pt_file_mode(handle)->mode | bits);
}

void pt_process_environment_import(const u8 *const *entries, size_t count) {
  size_t used = 0;
  if (entries)
    while (used < count) {
      if (!entries[used++])
        break;
    }
  if (used > SIZE_MAX / sizeof(*environment))
    abort();
  const u8 **copy = used ? malloc(used * sizeof(*copy)) : NULL;
  if (used && !copy)
    abort();
  if (used)
    memcpy(copy, entries, used * sizeof(*copy));
  free(environment);
  environment = copy;
  environment_count = used;
  environment_present = entries != NULL;
}
bool pt_process_environment_present(void) {
  return environment_present;
}
const u8 *pt_environment_string(u32 slot) {
  return (size_t)slot < environment_count ? environment[slot] : NULL;
}

static const PTFileModeSlot initial_io_modes[20] = {
  [0] = {.mode = 1}, [1] = {.mode = 2}, [2] = {.mode = 2}, [3] = {.mode = 3}, [4] = {.mode = 2}};
void pt_native_process_defaults_apply(void) {
  pt_process_tables_reset();
  _Static_assert(sizeof(guest_io_modes) == sizeof(initial_io_modes), "native mode table extent");
  memcpy(guest_io_modes, initial_io_modes, sizeof(guest_io_modes));
  guest_io_mode_table = guest_io_modes;
  io_mode_count = sizeof(guest_io_modes) / sizeof(*guest_io_modes);
}

// file label

#include "files.h"

static u8 *current_filename;

void pt_file_set_current_filename(const u8 *filename) {
  size_t length = strlen((const char *)filename);
  u8 *copy = malloc(length + 1);
  if (!copy)
    abort();
  memcpy(copy, filename, length + 1);
  free(current_filename);
  current_filename = copy;
}

const u8 *pt_file_current_filename(void) {
  return current_filename;
}

void pt_file_label_import(const u8 *filename, size_t length) {
  if (length == SIZE_MAX || (!filename && length))
    abort();
  u8 *copy = filename ? malloc(length + 1) : NULL;
  if (filename) {
    if (!copy)
      abort();
    memcpy(copy, filename, length);
    copy[length] = 0;
  }
  // Copy first: the source may be the currently owned spelling.

  pt_file_label_reset();
  current_filename = copy;
}

void pt_file_label_reset(void) {
  free(current_filename);
  current_filename = NULL;
}

// dos memory

#include "sound_types.h"
static PTDOSAllocator dos_memory;
static u8 *dos_storage[PT_DOS_POOL_COUNT];
u8 *pt_dos_memory_storage(PTDOSPool pool) {
  static const size_t capacities[] = {PT_DOS_CONVENTIONAL_CAPACITY, PT_DOS_UPPER_CAPACITY};
  if ((unsigned)pool >= PT_DOS_POOL_COUNT)
    abort();
  // Keep each one-past payload pointer distinct from other arena starts.

  if (!dos_storage[pool])
    dos_storage[pool] = calloc(capacities[pool] + 16u, 1);
  if (!dos_storage[pool])
    abort();
  return dos_storage[pool];
}

static void (*allocation_retired)(const void *, size_t);
void pt_dos_memory_observer(void (*released)(const void *, size_t)) {
  allocation_retired = released;
}
void pt_dos_memory_reset(void) {
  u8 *storage = pt_dos_memory_storage(PT_DOS_CONVENTIONAL);
  dos_memory =
      (PTDOSAllocator){.begin = storage, .end = storage + PT_DOS_CONVENTIONAL_CAPACITY, .next_selector = 0x100};
}
void pt_dos_memory_restore(const PTDOSAllocator *state) {
  dos_memory = *state;
}

u8 *pt_dos_memory_allocate_in(PTDOSAllocator *state, u16 paragraphs, u16 *selector) {
  u32 size = (u32)paragraphs * 16u;
  u8 *buffer = state->begin;
  if (!size || state->count == PT_DOS_BLOCKS || state->next_selector > 0xfff8)
    return NULL;
  for (;;) {
    bool occupied = false;
    if ((uintptr_t)buffer > (uintptr_t)state->end || size > (uintptr_t)state->end - (uintptr_t)buffer)
      return NULL;
    for (unsigned i = 0; i < state->count; ++i) {
      const PTPlatformAllocation *a = &state->allocations[i];
      if ((uintptr_t)buffer < (uintptr_t)a->buffer + a->size && (uintptr_t)a->buffer < (uintptr_t)buffer + size) {
        buffer = a->buffer + a->size;
        occupied = true;
        break;
      }
    }
    if (!occupied)
      break;
  }
  *selector = (u16)state->next_selector;
  state->next_selector += 8;
  state->allocations[state->count++] = (PTPlatformAllocation){buffer, size, *selector};
  return buffer;
}

u32 pt_dos_memory_free_in(PTDOSAllocator *state, u16 selector) {
  for (unsigned i = 0; i < state->count; ++i)
    if (state->allocations[i].handle == selector) {
      memmove(
          &state->allocations[i], &state->allocations[i + 1], (state->count - i - 1u) * sizeof(PTPlatformAllocation));
      memset(&state->allocations[--state->count], 0, sizeof(PTPlatformAllocation));
      return 0x101;
    }
  // DPMI invalid selector; ownership remains unchanged.
  return 0x8022;
}

u8 *pt_dos_memory_allocate(u16 paragraphs, u16 *selector) {
  u8 *storage = pt_dos_memory_allocate_in(&dos_memory, paragraphs, selector);
  return storage;
}

u32 pt_dos_memory_free(u16 selector) {
  for (unsigned i = 0; i < dos_memory.count; ++i) {
    PTPlatformAllocation allocation = dos_memory.allocations[i];
    if (allocation.handle == selector) {
      u32 result = pt_dos_memory_free_in(&dos_memory, selector);
      if (result == 0x101 && allocation_retired && allocation.size)
        allocation_retired(allocation.buffer, allocation.size);
      return result;
    }
  }
  return pt_dos_memory_free_in(&dos_memory, selector);
}

bool pt_dos_memory_contains(const void *buffer, u32 size) {
  uintptr_t address = (uintptr_t)buffer;
  for (unsigned i = 0; i < dos_memory.count; ++i) {
    const PTPlatformAllocation *a = &dos_memory.allocations[i];
    if (address >= (uintptr_t)a->buffer && size <= a->size && address - (uintptr_t)a->buffer <= a->size - size)
      return true;
  }
  return false;
}

const PTDOSAllocator *pt_dos_memory_state(void) {
  return &dos_memory;
}

// Process lifetime and finalization.

#include "sound.h"
static PTProcessState process_state;
void pt_runtime_reset_platform(void) {
  process_state = (PTProcessState){0};
  pt_runtime_state_reset();
}
bool pt_process_load_platform(const PTProcessState *state) {
  if (!state || state->terminated > 1 || state->return_code > 255 || (!state->terminated && state->return_code))
    return false;
  process_state = *state;
  return true;
}
bool pt_process_export_platform(PTProcessState *state) {
  if (!state)
    return false;
  *state = process_state;
  return true;
}

i32 GuestStrncmp(const u8 *first, const u8 *second, u32 size) {
  i32 result = 0;
  while (size--) {
    u8 a = *first;
    u8 b = *second;
    ++first;
    ++second;
    if (a != b) {
      result = (i32)a - b;
      break;
    }
    if (!a)
      break;
  }

  return result;
}

// Retain the next node before shutdown moves the current one to the free list.

u32 GuestCloseAllStreams(u32 first) {
  u32 count = 0;
  for (GuestFileNode *node = guest_active_file_nodes; node;) {
    GuestFileNode *next = node->next;
    GuestFileStream *stream = node->stream;
    bool close_handle = true;
    if (!(stream->buffer_flags & 0x40u)) {
      if (pt_file_stream_precedes(stream, first)) {
        node = next;
        continue;
      }
      close_handle = !pt_file_stream_precedes(stream, 5);
    }
    ++count;
    GuestShutdownStream(stream, close_handle);
    node = next;
  }
  return count;
}
void GuestPurgeFileNodes(void) {
  while (guest_free_file_nodes) {
    GuestFileNode *node = guest_free_file_nodes;
    GuestFileNode *next = node->next;
    pt_file_node_release(node);
    guest_free_file_nodes = next;
  }
}
void GuestFinalizeFiles(void) {
  GuestCloseAllStreams(0);
  GuestPurgeFileNodes();
}
void GuestFini387(void) {
  pt_runtime_invoke(pt_runtime.fpu_finalizer);
}
void GuestFiniRtns(u8 first, u8 last) {
  PTRuntimeFinalizer *finalizers = pt_runtime.finalizers;
  for (;;) {
    PTRuntimeFinalizer *selected = NULL;
    u8 priority = first;
    for (unsigned i = 0; i < PT_RUNTIME_FINALIZER_COUNT; ++i) {
      PTRuntimeFinalizer *entry = &finalizers[i];
      if (entry->state != 2 && priority <= entry->priority) {
        selected = entry;
        priority = entry->priority;
      }
    }
    if (!selected)
      break;
    if (priority <= last && selected->callback.invoke) {
      PTRuntimeCallback callback = selected->callback;
      pt_runtime_invoke(callback);
    }
    selected->state = 2;
  }
}
static void exit_callback(unsigned index) {
  if (index >= PT_RUNTIME_EXIT_HOOK_COUNT)
    abort();
  PTRuntimeCallback callback = pt_runtime.exit_hooks[index];
  pt_runtime_invoke(callback);
}
static _Noreturn void exit_tail(u32 status) {
  GuestFiniRtns(0, 255);
  // Publish the final state before host resources can be reclaimed.

  process_state = (PTProcessState){1, (u8)status};
  pt_runtime_finish_process((u8)status);
}
_Noreturn void GuestImmediateExit(u32 status) {
  exit_callback(1);
  exit_callback(2);
  exit_tail(status);
}
_Noreturn void GuestExit(u32 status) {
  exit_callback(0);
  exit_callback(1);
  GuestImmediateExit(status);
}

// memory

static PTBufferStorage buffer_storage;
void pt_buffer_storage(const PTBufferStorage *storage) {
  if (storage && (!!storage->bind != !!storage->release))
    abort();
  buffer_storage = storage ? *storage : (PTBufferStorage){0};
}
u8 *pt_buffer_allocate(u32 size, bool clear) {
  if (!size)
    return NULL;
  // Keep native bytes defined independently of the serialized allocator.
  // The codec retains original bytes the caller has not written yet.

  u8 *buffer = calloc(1, size);
  if (buffer && buffer_storage.bind && !buffer_storage.bind(buffer, size, clear)) {
    free(buffer);
    return NULL;
  }
  return buffer;
}
void pt_buffer_release(void *buffer) {
  if (!buffer)
    return;
  if (buffer_storage.release)
    buffer_storage.release(buffer);
  else
    free(buffer);
}

PTNativeGlobals pt_native_globals;
