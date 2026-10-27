// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
// File lifecycle.

#include "files.h"
#include "memory.h"
#include "input.h"
#include "runtime_state.h"
#include "menu.h"
#include "objects.h"

// Stock Watcom FILE lifecycle. DOS resource ownership is implemented by the
// independent platform adapter; native buffers, flags and lists stay here.

u32 GuestToLower(u32 value) {
  return value >= 'A' && value <= 'Z' ? value + 32u : value;
}
u32 *GuestDOSErrnoAddress(void) {
  return &guest_dos_errno;
}
u32 *GuestSetDOSErrno(u32 value) {
  guest_dos_errno = value;
  return GuestDOSErrnoAddress();
}
u32 GuestErrnoFromDOS(u32 value) {
  u32 index = (u8)value;
  GuestSetDOSErrno(index);
  u32 error;
  if (value < 256u) {
    if (guest_dos_major >= 3) {
      if (index == 80)
        index = 14;
      else if (index >= 34)
        index = 19;
      else if (index >= 32)
        index = 5;
    }
    if (index > 19)
      index = 19;
    u8 byte = guest_dos_error_errno[index];
    error = byte <= INT8_MAX ? byte : UINT32_MAX - (UINT8_MAX - byte);
  } else
    error = (value >> 8) & 255u;
  GuestSetErrno(error);
  return UINT32_MAX;
}
u32 GuestIOMode(u32 handle) {
  u32 result = 0;
  if (handle >= guest_io_mode_capacity)
    goto done;
  PTFileModeSlot *entry = pt_file_mode(handle);
  u32 mode = entry->mode;
  if (handle <= 5 && !(mode & 0x4000u)) {
    pt_file_mode_set(handle, mode | 0x4000u);
    u32 tty;
    tty = GuestIsTTY(handle);
    if (tty)
      pt_file_mode_or(handle, 0x2000u);
  }
  result = pt_file_mode(handle)->mode;
done:
  return result;
}
PTFileModeSlot *GuestSetIOMode(u32 handle, u32 mode) {
  PTFileModeSlot *base = guest_io_mode_table;
  pt_file_mode_set(handle, mode | 0x4000u);
  return base;
}
GuestFileStream *GuestAllocFileStream(void) {
  GuestFileNode *node = guest_free_file_nodes;
  GuestFileStream *stream = NULL;
  u32 flags;
  if (node) {
    stream = node->stream;
    flags = (stream->status_word & 0x4003u) | 3u;
    guest_free_file_nodes = node->next;
  } else {
    for (unsigned i = 0; i < sizeof(guest_file_streams) / sizeof(guest_file_streams[0]); ++i)
      if (!(guest_file_streams[i].flags & 3u)) {
        stream = &guest_file_streams[i];
        break;
      }
    GuestFileNode *allocation = pt_file_node_create(stream);
    if (!allocation) {
      GuestSetErrno(5);
      return NULL;
    }
    node = allocation;
    flags = stream ? 3u : 0x4003u;
    if (!stream)
      stream = node->stream;
  }
  (void)pt_file_stream_reset(stream);
  stream->status_word = flags;
  node->stream = stream;

  node->next = guest_active_file_nodes;

  guest_active_file_nodes = node;
  return stream;
}
void GuestFreeFileStream(GuestFileStream *stream) {
  GuestFileNode *previous = NULL;
  for (GuestFileNode *node = guest_active_file_nodes; node;) {
    if (node->stream != stream) {
      previous = node;
      node = node->next;
      continue;
    }
    stream->flags |= 3u;
    if (previous) {
      previous->next = node->next;

    } else
      guest_active_file_nodes = node->next;
    node->next = guest_free_file_nodes;

    guest_free_file_nodes = node;
    return;
  }
}
u32 GuestOpenFlags(const u8 *mode) {
  const u8 *text = mode;
  u8 first = (u8)GuestToLower(text[0]);
  if (first != 'r' && first != 'w' && first != 'a') {
    GuestSetErrno(9);
    return 0;
  }
  u32 flags = 0;
  if (text[1] == '+') {
    flags = 3;
    if (text[2] == 'b' || (text[2] != 't' && guest_default_open_mode == 0x200u))
      flags |= 0x40u;
  } else if (text[1] == 'b') {
    flags = 0x40u;
    if (text[2] == '+')
      flags |= 3u;
  } else if (text[1] == 't') {
    if (text[2] == '+')
      flags = 3;
  } else if (guest_default_open_mode == 0x200u)
    flags = 0x40u;
  return flags | (first == 'w' ? 2u : first == 'a' ? 0x82u : 1u);
}
u32 GuestSOpen(const u8 *filename, u32 flags, u32 sharing, u32 permissions) {
  (void)sharing;
  u32 error, result;
  while (*filename == ' ')
    ++filename;
  u32 handle = pt_file_platform_open_mode(filename, flags & 3u, &error);
  u32 sopen_tty = 0;
  if ((flags & 3u) && handle != UINT32_MAX) {

    sopen_tty = GuestIsTTY(handle);
  }
  if ((flags & 3u) && handle != UINT32_MAX && !sopen_tty) {
    if (flags & 0x400u) {
      pt_file_platform_close(handle);
      GuestSetErrno(7);
      result = UINT32_MAX;
      goto done;
    }
    if ((flags & 0x40u) && pt_file_platform_write(handle, NULL, 0, &error) == UINT32_MAX) {
      pt_file_platform_close(handle);
      result = GuestErrnoFromDOS(error);
      goto done;
    }
  }
  if (handle == UINT32_MAX) {
    if (!(flags & 0x20u) || error != 2) {
      result = GuestErrnoFromDOS(error);
      goto done;
    }
    u32 rights = (permissions ? permissions : 0x180u) ^ guest_permission_mask;
    handle = pt_file_platform_create(filename, !(rights & 0x80u), &error);
    if (handle == UINT32_MAX) {
      result = GuestErrnoFromDOS(error);
      goto done;
    }
    pt_file_platform_close(handle);
    handle = pt_file_platform_open_mode(filename, flags & 3u, &error);
    if (handle == UINT32_MAX) {
      result = GuestErrnoFromDOS(error);
      goto done;
    }
  }
  u32 mode = GuestIOMode(handle);
  mode = (mode & UINT32_C(0xffffff00)) | (mode & 0x3cu);
  u32 tty = GuestIsTTY(handle);
  if (tty)
    mode |= 0x2000u;
  u32 access = flags & 3u;
  if (access == 2)
    mode |= 3u;
  else if (!access)
    mode |= 1u;
  else if (access == 1)
    mode |= 2u;
  if (flags & 0x10u)
    mode |= 0x80u;
  if ((flags & 0x300u) ? flags & 0x200u : guest_default_open_mode == 0x200u)
    mode |= 0x40u;
  GuestSetIOMode(handle, mode);
  result = handle;
done:
  return result;
}
GuestFileStream *GuestDoOpen(const u8 *filename, const u8 *mode, u32 sharing, GuestFileStream *stream) {
  GuestFileStream *f = stream;
  f->flags &= 0xfcu;
  u32 open_flags = GuestOpenFlags(mode);
  f->status_word |= open_flags;
  u32 raw = *mode, lower;
  lower = GuestToLower(raw);
  u8 first = (u8)lower;
  u32 flags = first == 'r' ? (f->flags & 2u ? 2u : 0u) : ((f->flags & 1u) != 0) + 0x21u;
  if (first != 'r')
    flags |= first == 'a' ? 0x10u : 0x40u;
  flags |= f->flags & 0x40u ? 0x200u : 0x100u;
  u32 permissions = first == 'r' ? 0 : 0x180u;
  f->handle = GuestSOpen(filename, flags, sharing, permissions);
  if (f->handle == UINT32_MAX) {
    GuestFreeFileStream(stream);
    return NULL;
  }
  f->buffered_bytes = 0;
  f->buffer = NULL;
  f->buffer_size = 0;
  if (first == 'a') {
    GuestFseek(stream, 0, 2);
  }
  GuestCheckTTY(stream);
  return stream;
}
GuestFileStream *GuestFSOpen(const u8 *filename, const u8 *mode, u32 sharing) {
  GuestFileStream *stream, *result = NULL;
  stream = GuestAllocFileStream();
  if (stream) {
    result = GuestDoOpen(filename, mode, sharing, stream);
  }
  return result;
}
GuestFileStream *GuestFopen(const u8 *filename, const u8 *mode) {
  return GuestFSOpen(filename, mode, 0);
}
u32 GuestLSeek(u32 handle, u32 offset, u32 whence) {
  u32 error, position = pt_file_platform_seek(handle, offset, whence, &error);
  if (error) {
    GuestErrnoFromDOS(error);
    position = UINT32_MAX;
  }
  return position;
}
u32 GuestTell(u32 handle) {
  return GuestLSeek(handle, 0, 1);
}
u32 GuestFtell(GuestFileStream *stream) {
  const GuestFileStream *f = stream;
  u32 position = GuestTell(f->handle);
  if (position != UINT32_MAX && f->buffered_bytes)
    position = f->buffer_flags & 0x10u ? position + f->buffered_bytes : position - f->buffered_bytes;
  return position;
}
u32 GuestQuickClose(u32 handle) {
  pt_file_platform_close(handle);
  GuestSetIOMode(handle, 0);
  return 0;
}
u32 GuestProcessID(void) {
  return guest_psp_selector;
}
u32 GuestHexDigit(u32 digit) {
  u32 value = digit + 48u;
  return pt_signed_dword(value) > 57 ? value + 39u : value;
}
u32 GuestMakeTempFilename(u8 *destination, u32 number) {
  u32 process = GuestProcessID();
  destination[0] = 't';
  for (unsigned i = 4; i; --i) {
    u32 digit = process & 15u, value;
    process >>= 4;
    value = GuestHexDigit(digit);
    destination[i] = (u8)value;
  }
  destination[5] = '_';
  u32 digit = number >> 4 & 15u, value;
  value = GuestHexDigit(digit);
  destination[6] = (u8)value;
  digit = number & 15u;
  value = GuestHexDigit(digit);
  memcpy(destination + 8, ".tmp", 5);
  destination[7] = (u8)value;
  return value;
}
u32 GuestDOSUnlink(const u8 *filename) {
  u32 error, result = pt_file_platform_unlink(filename, &error);
  return result == UINT32_MAX ? GuestErrnoFromDOS(error) : 0;
}
u32 GuestUnlink(const u8 *filename) {
  return GuestDOSUnlink(filename);
}
u32 GuestDoClose(GuestFileStream *stream, u32 close_handle) {
  GuestFileStream *f = stream;
  if (!f->status_word)
    return UINT32_MAX;
  u8 temporary_filename[28];
  u32 result = f->buffer_flags & 0x10u ? GuestFlush(stream) : 0;
  u32 position = GuestFtell(stream);
  if (position != UINT32_MAX)
    GuestLSeek(f->handle, position, 0);
  if (close_handle)
    result |= GuestQuickClose(f->handle);
  if (f->flags & 8u) {
    pt_buffer_release(f->buffer);
    f->buffer = NULL;
  }
  if (f->buffer_flags & 8u) {
    GuestMakeTempFilename(temporary_filename, f->temporary_index);
    GuestUnlink(temporary_filename);
  }
  return result;
}
u32 GuestShutdownStream(GuestFileStream *stream, u32 close_handle) {
  u32 result = GuestDoClose(stream, close_handle);
  GuestFreeFileStream(stream);
  return result;
}
u32 GuestFclose(GuestFileStream *stream) {
  for (GuestFileNode *node = guest_active_file_nodes; node; node = node->next)
    if (node->stream == stream)
      return GuestShutdownStream(stream, 1);
  return UINT32_MAX;
}
GuestFileStream *OpenFile(const u8 *filename, u32 flags) {
  GuestFileStream *stream;
  if (flags == 1) {
    stream = GuestFopen(filename, file_write_mode);
    if (!stream)
      return NULL;
    GuestFclose(stream);
  }
  stream = GuestFopen(filename, file_readwrite_mode);
  return stream;
}
u32 CloseFile(GuestFileStream *stream) {
  return GuestFclose(stream);
}

u32 OpenFileAsCurrent(const u8 *filename) {
  u8 error_message[200];
  pt_file_set_current_filename(filename);
  u32 result = 1;
  for (;;) {
    cur_loading_filehandle = OpenFile(filename, 0);
    if (cur_loading_filehandle) {
      result = 1;
      break;
    }
    result = 0;
    if (game_ptr->skip_missing_file_dialog)
      break;
    static const char format[] = "File '%s' not found! Try again?";
    snprintf((char *)error_message, sizeof(error_message), format, (const char *)filename);

    if (!temp_canvas)
      break;
    u8 retry = ShowMessage2(0, error_message);
    if (!retry)
      break;
  }
  num_read_bytes = 0;
  return result;
}

void CloseCurrentFile(void) {
  if (game_ptr->is_loading_from_buffer) {
    pt_buffer_release(game_ptr->read_buffer.allocation);
    game_ptr->is_loading_from_buffer = 0;
    return;
  }
  CloseFile(cur_loading_filehandle);
  cur_loading_filehandle = NULL;
  // The disk branch also clears pending input events; the memory-buffer branch returns directly.

  ClearInputEvents();
}

// File read.

#include "frame.h"
#include "graphics.h"
#include "render_types.h"

// Disk resources are separate from native CRT buffers and ownership.
// Shared file contents and independent handle positions are compared.

static PTFileResource resources[PT_FILE_READ_HANDLES];
static unsigned resource_count;
typedef struct AssetResource {
  char *name;
  u8 *bytes;
  u32 size, attributes;
  u16 write_time, write_date;
} AssetResource;
static AssetResource *assets;
static unsigned asset_count;
static u64 disk_revision;
static char **directories;
static unsigned directory_count;
static u32 file_limit, reserved_handles;
static bool file_profile_ready;
static PTDirectoryState directory_state;
static PTDosSearchState search_platform;
static GuestFindRecord local_find_record;
static GuestFindRecord *default_find_record = &local_find_record;
GuestFindRecord *pt_default_find_record(void) {
  return default_find_record;
}
void pt_default_find_record_bind(GuestFindRecord *record) {
  GuestFindRecord *previous = default_find_record;
  default_find_record = record ? record : &local_find_record;
  if (search_platform.dta == previous)
    search_platform.dta = default_find_record;
}

void pt_file_read_reset(void) {
  for (unsigned i = 0; i < resource_count; ++i) {
    free(resources[i].bytes);
    free(resources[i].name);
  }
  memset(resources, 0, sizeof(resources));
  resource_count = 0;
  for (unsigned i = 0; i < asset_count; ++i) {
    free(assets[i].name);
    free(assets[i].bytes);
  }
  free(assets);
  assets = NULL;
  asset_count = 0;
  for (unsigned i = 0; i < directory_count; ++i)
    free(directories[i]);
  free(directories);
  directories = NULL;
  directory_count = 0;
  file_limit = reserved_handles = 0;
  file_profile_ready = false;
  pt_file_label_reset();
  disk_revision = 0;
  memset(&directory_state, 0, sizeof(directory_state));
  local_find_record = (GuestFindRecord){0};
  default_find_record = &local_find_record;
  search_platform = (PTDosSearchState){.dta = pt_default_find_record()};
}
static PTFileResource *resource(u32 handle) {
  for (unsigned i = 0; i < resource_count; ++i)
    if (resources[i].state.handle == handle)
      return &resources[i];
  return NULL;
}

bool pt_file_read_load_handle(u32 handle, const u8 *data, u32 size, u32 position) {
  if (resource_count == PT_FILE_READ_HANDLES || resource(handle) || (size && !data))
    return false;
  u8 *copy = size ? malloc(size) : NULL;
  if (size && !copy)
    return false;
  if (size)
    memcpy(copy, data, size);
  resources[resource_count++] = (PTFileResource){{handle, size, position}, copy, NULL, 2};
  return true;
}

u32 *GuestErrnoAddress(void) {
  return &guest_errno;
}
u32 *GuestSetErrno(u32 value) {
  guest_errno = value;
  return GuestErrnoAddress();
}
u32 GuestIsTTY(u32 handle) {
  u32 result = handle < 3u;

  return result;
}
void GuestCheckTTY(GuestFileStream *stream) {
  if (!(stream->buffer_flags & 0x20u)) {
    u32 tty;
    tty = GuestIsTTY(stream->handle);
    if (tty) {
      stream->buffer_flags |= 0x20u;
      if (!(stream->buffer_flags & 7u))
        stream->buffer_flags |= 2u;
    }
  }
}
u8 *GuestIOAllocate(GuestFileStream *stream) {
  GuestCheckTTY(stream);
  if (!stream->buffer_size)
    stream->buffer_size = stream->buffer_flags & 2u ? 134u : stream->buffer_flags & 4u ? 1u : 4096u;
  stream->buffer = pt_buffer_allocate(stream->buffer_size, false);
  if (!stream->buffer) {
    stream->buffer_size = 1;
    stream->buffer = &stream->inline_buffer;
    stream->buffer_flags = (stream->buffer_flags & 0xf8u) | 4u;
  } else
    stream->flags |= 8u;
  stream->buffered_bytes = 0;
  stream->cursor = (PTBufferCursor){stream->buffer, 0};

  return stream->buffer;
}
u32 GuestQuickRead(u32 handle, void *destination, u32 size) {
  PTFileResource *r = resource(handle);
  if (!r)
    return GuestErrnoFromDOS(6);
  if (r->mode == 1)
    return GuestErrnoFromDOS(5);
  u32 available = r->state.position < r->state.size ? r->state.size - r->state.position : 0;
  u32 amount = size < available ? size : available;
  if (amount)
    memcpy(destination, r->bytes + r->state.position, amount);
  r->state.position += amount;
  return amount;
}
// Count every matching FILE, flushing only its pending-output bit. The original intentionally
// ignores individual flush errors and continues.

u32 GuestFlushMatchingStreams(u32 mask) {
  u32 count = 0;
  for (GuestFileNode *node = guest_active_file_nodes; node; node = node->next) {
    GuestFileStream *stream = node->stream;
    if (!(stream->status_word & mask))
      continue;
    ++count;
    if (!(stream->buffer_flags & 0x10u))
      continue;

    GuestFlush(stream);
  }

  return count;
}

// The supported AH=01 redirection profile has readable stdin and writable
// stdout files. Live devices, exhausted input and DOS control handling stop
// before consuming input. In particular, a literal ^Z is not file EOF here.

bool pt_file_redirected_character_available(void) {
  const PTFileResource *input = resource(0), *output = resource(1);
  if (!input || !output || input->mode == 1 || !output->mode || input->state.position >= input->state.size ||
      !pt_file_write_platform_ready(1, 1))
    return false;
  u8 value = input->bytes[input->state.position];
  return value >= 32 || value == 0 || value == 10 || value == 13 || value == 26;
}
// A nonzero cached DWORD is returned intact and consumed once.

u32 GuestGetche(void) {
  u32 value = guest_console_cached_character;
  guest_console_cached_character = 0;
  if (value)
    return value;
  if (!pt_file_redirected_character_available()) {
    abort();
  }
  PTFileResource *input = resource(0);
  u8 character = input->bytes[input->state.position];
  u32 error, amount = pt_file_platform_write(1, &character, 1, &error);
  if (error || amount != 1)
    abort();
  ++input->state.position;
  return character;
}
u32 GuestFillBuffer(GuestFileStream *stream) {
  if (!stream->buffer) {
    GuestIOAllocate(stream);
  }
  if ((stream->buffer_flags & 0x20u) && (stream->buffer_flags & 6u)) {
    GuestFlushMatchingStreams(0x2000);
  }
  stream->cursor = (PTBufferCursor){stream->buffer, 0};
  bool character = (stream->status_word & 0x2400u) == 0x2400u && !stream->handle;
  stream->flags &= 0xfbu;
  if (character) {
    stream->buffered_bytes = 0;
    u32 value;
    value = GuestGetche();
    if (value != UINT32_MAX) {
      *pt_file_buffer_data(stream) = (u8)value;
      stream->buffered_bytes = 1;
    }
  } else {
    u32 read_size = stream->buffer_flags & 4u ? 1u : stream->buffer_size;
    stream->buffered_bytes = GuestQuickRead(stream->handle, pt_file_buffer_data(stream), read_size);
  }
  if (pt_signed_dword(stream->buffered_bytes) < 0) {
    stream->buffered_bytes = 0;
    stream->flags |= 0x20u;
  } else if (!stream->buffered_bytes)
    stream->flags |= 0x10u;
  u32 result = stream->buffered_bytes;

  return result;
}
u32 GuestFread(void *buffer, u32 size, u32 count, GuestFileStream *stream) {
  u8 *destination = buffer;
  u32 result = 0;
  if (!(stream->flags & 1u)) {
    GuestSetErrno(4);
    stream->flags |= 0x20u;
    goto done;
  }
  u32 remaining = size * count;
  if (!remaining)
    goto done;
  if (!stream->buffer)
    GuestIOAllocate(stream);
  u32 read = 0;
  if (stream->flags & 0x40u) {
    for (;;) {
      if (stream->buffered_bytes) {
        u32 amount = stream->buffered_bytes < remaining ? stream->buffered_bytes : remaining;
        memmove(destination, pt_file_buffer_data(stream), amount);
        remaining -= amount;
        destination += amount;
        stream->cursor.position += amount;
        read += amount;
        stream->buffered_bytes -= amount;
      }
      if (!remaining)
        break;
      if (remaining >= stream->buffer_size || (stream->buffer_flags & 4u)) {
        u32 amount = remaining;
        if (!(stream->buffer_flags & 4u) && remaining > 512u)
          amount &= ~UINT32_C(511);
        amount = GuestQuickRead(stream->handle, destination, amount);
        if (amount == UINT32_MAX) {
          stream->flags |= 0x20u;
          break;
        }
        if (!amount) {
          stream->flags |= 0x10u;
          break;
        }
        destination += amount;
        remaining -= amount;
        read += amount;
      } else if (!GuestFillBuffer(stream))
        break;
    }
  } else {
    // Stock text input discards CR and takes the following byte even when
    // it is not LF. CTRL-Z ends this operation; later calls may continue.

    for (;;) {
      if (!stream->buffered_bytes && !GuestFillBuffer(stream))
        break;
      stream->buffered_bytes -= 1;
      u8 value = pt_file_buffer_take(stream);
      if (value == 13) {
        if (!stream->buffered_bytes && !GuestFillBuffer(stream))
          break;
        stream->buffered_bytes -= 1;
        value = pt_file_buffer_take(stream);
      }
      if (value == 26) {
        stream->flags |= 0x10u;
        break;
      }
      *destination++ = value;
      ++read;
      if (!--remaining)
        break;
    }
  }
  result = read / size;
done:
  return result;
}
u32 ReadFile(GuestFileStream *stream, void *destination, u32 size) {
  return GuestFread(destination, 1, size, stream);
}
u32 ReadBytesFromCurrentFile(void *destination, u32 size) {
  u8 error_message[100];
  u32 amount = 0;
  if (!destination) {
    return 0;
  }
  if (game_ptr->is_loading_from_buffer) {
    if (size)
      memmove(destination, pt_game_read_data(game_ptr, size), size);

    game_ptr->read_buffer.cursor.position += size;
    amount = size;
  } else if (cur_loading_filehandle) {
    amount = ReadFile(cur_loading_filehandle, destination, size);
    num_read_bytes += amount;
    if (game_ptr->show_disk_read_errors && amount != size) {
      snprintf((char *)error_message,
          sizeof(error_message),
          "Error reading file %s!",
          (const char *)pt_file_current_filename());
      DisplayErrorMessage(0, error_message);
    }
  }
  return amount;
}
Game *EnableDiskReadErrors(void) {
  game_ptr->show_disk_read_errors = 1;
  return game_ptr;
}
Game *DisableDiskReadErrors(void) {
  game_ptr->show_disk_read_errors = 0;
  return game_ptr;
}

// Read-only count projection for deciding whether an error dialog
// would execute. It does not allocate, advance a handle or modify RAM.

// Initial asset bytes belong to the platform, not the guest CRT. Opens have
// independent positions; writes publish contents to other handles of the same
// file, implemented separately by the original DOS adapter.

static bool canonical_name(const char *input, char output[1024]) {
  unsigned i = 0, length = 0;
  for (; i < 1023 && input[i]; ++i) {
    u8 value = (u8)input[i];
    if (value >= 128)
      return false;
    char converted = (char)(value == '\\' ? '/' : value >= 'A' && value <= 'Z' ? value + 32u : value);
    // The original ending uses gfx\\\\ende.vga. DOS accepts repeated
    // interior separators; UNC rejection still examines the raw path.

    if (converted == '/' && length && output[length - 1] == '/')
      continue;
    output[length++] = converted;
  }
  if (input[i])
    return false;
  output[length] = 0;
  if (length >= 3 && !memcmp(output, "c:/", 3))
    memmove(output, output + 3, length - 2u);
  return true;
}
static bool directory_profile_valid(const PTDirectoryState *initial) {
  if (!initial || !memchr(initial->cwd, 0, sizeof(initial->cwd)) || initial->getcwd_error > 255)
    return false;
  char normalized[1024];
  if (!canonical_name(initial->cwd, normalized) || strcmp(normalized, initial->cwd))
    return false;
  size_t n = strlen(normalized);
  if (n && (normalized[0] == '/' || normalized[n - 1] == '/'))
    return false;
  const char *part = normalized;
  while (*part) {
    const char *end = strchr(part, '/');
    size_t size = end ? (size_t)(end - part) : strlen(part);
    if (!size || memchr(part, ':', size) || (size == 1 && part[0] == '.') || (size == 2 && !memcmp(part, "..", 2)))
      return false;
    if (!end)
      break;
    part = end + 1;
  }
  return true;
}
bool pt_file_resolved_name_at(const PTDirectoryState *directory, const char *name, char result[1024]) {
  if (!directory_profile_valid(directory) || !name || !result)
    return false;
  char normalized[1024];
  if (!canonical_name(name, normalized))
    return false;
  if ((name[0] == '/' || name[0] == '\\') && (name[1] == '/' || name[1] == '\\'))
    return false;
  bool absolute = (name[0] == '/' || name[0] == '\\' ||
      ((name[0] == 'c' || name[0] == 'C') && name[1] == ':' && (name[2] == '/' || name[2] == '\\')));
  char *start = normalized;
  while (*start == '/')
    ++start;
  for (const char *part = start; *part;) {
    const char *end = strchr(part, '/');
    size_t n = end ? (size_t)(end - part) : strlen(part);
    if (!n || (n == 1 && part[0] == '.') || (n == 2 && !memcmp(part, "..", 2)) || memchr(part, ':', n))
      return false;
    if (!end)
      break;
    part = end + 1;
  }
  size_t prefix = absolute ? 0 : strlen(directory->cwd), length = strlen(start);
  if (prefix + (prefix && length ? 1u : 0u) + length >= 1024)
    return false;
  if (prefix) {
    memcpy(result, directory->cwd, prefix);
    if (length)
      result[prefix++] = '/';
  }
  memcpy(result + prefix, start, length + 1);
  return true;
}
bool pt_file_resolved_name(const char *name, char result[1024]) {
  return pt_file_resolved_name_at(&directory_state, name, result);
}

static bool directory_exists(const char *name) {
  if (!name[0])
    return true;
  for (unsigned i = 0; i < directory_count; ++i)
    if (!strcmp(name, directories[i]))
      return true;
  size_t n = strlen(name);
  for (unsigned i = 0; i < asset_count; ++i)
    if (!strncmp(name, assets[i].name, n) && assets[i].name[n] == '/')
      return true;
  return false;
}
bool pt_directory_restore(const PTDirectoryState *initial) {
  if (!directory_profile_valid(initial))
    return false;
  directory_state = *initial;
  return true;
}
const PTDirectoryState *pt_directory_state(void) {
  return &directory_state;
}
bool pt_directory_plan_at(const PTDirectoryState *directory, const char *name, char result[64], u32 *error) {
  char normalized[1024];
  if (!name || !result || !error || !pt_file_resolved_name_at(directory, name, normalized))
    return false;
  size_t n = strlen(normalized);
  while (n && normalized[n - 1] == '/')
    normalized[--n] = 0;
  if (n >= 64)
    return false;
  for (unsigned i = 0; i < n; ++i)
    if ((u8)normalized[i] < 32 || normalized[i] == '*' || normalized[i] == '?')
      return false;
  memcpy(result, normalized, n + 1);
  *error = (!name[0] || !directory_exists(normalized)) ? 3 : 0;
  return true;
}
bool pt_directory_plan(const char *name, char result[64], u32 *error) {
  return pt_directory_plan_at(&directory_state, name, result, error);
}
void pt_directory_commit(const char *name) {
  size_t n = strlen(name);
  if (n >= 64 || !directory_exists(name))
    abort();
  memset(directory_state.cwd, 0, sizeof(directory_state.cwd));
  memcpy(directory_state.cwd, name, n);
}
bool pt_dos_search_load_state(const PTDosSearchState *initial) {
  if (!initial || initial->first_error > 65535 || initial->next_error > 65535)
    return false;
  search_platform = *initial;
  return true;
}
bool pt_dos_search_export_state(PTDosSearchState *result) {
  if (!result)
    return false;
  *result = search_platform;
  return true;
}
void pt_dos_set_find_record(GuestFindRecord *record) {
  search_platform.dta = search_platform.fixed_dta ? search_platform.fixed_dta : record;
}
GuestFindRecord *pt_dos_find_record(void) {
  return search_platform.dta;
}
// The local DOS profile uses the sorted imported directory table as a stable
// virtual cluster map, and imported asset order as its directory slot order.
// The 21-byte continuation follows Microsoft's local DOS search format.

static bool dos_name(const char *name, u8 result[11], bool pattern) {
  const char *dot = strchr(name, '.');
  size_t base = dot ? (size_t)(dot - name) : strlen(name), ext = dot ? strlen(dot + 1) : 0;
  if (!base || base > 8 || ext > 3 || (dot && strchr(dot + 1, '.')))
    return false;
  if (pattern && !dot && memchr(name, '*', base))
    return false;
  memset(result, ' ', 11);
  for (unsigned side = 0; side < 2; ++side) {
    const char *text = side ? (dot ? dot + 1 : "") : name;
    size_t n = side ? ext : base;
    unsigned offset = side ? 8 : 0, width = side ? 3 : 8;
    for (unsigned i = 0; i < n; ++i) {
      u8 c = (u8)text[i];
      if (c >= 'a' && c <= 'z')
        c -= 32;
      if (pattern && c == '*') {
        memset(result + offset + i, '?', width - i);
        break;
      }
      bool literal = (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || strchr("!#$%&'()-@^_`{}~", c);
      if (!literal && !(pattern && c == '?'))
        return false;
      result[offset + i] = c;
    }
  }
  return true;
}
static bool dos_matches(const u8 pattern[11], const u8 name[11]) {
  for (unsigned index = 0; index < 11; ++index)
    if (pattern[index] != '?' && pattern[index] != name[index])
      return false;
  return true;
}

static void dos_pack_find_record(GuestFindRecord *record, const AssetResource *asset, const u8 dos_name_bytes[11]) {
  record->attributes = (u8)asset->attributes;
  record->write_time = asset->write_time;
  record->write_date = asset->write_date;
  record->size = asset->size;
  // DOS PackName writes the padded base first. Do not clear the unused
  // tail: bytes after the terminating NUL remain observable guest state.

  memcpy(record->name, dos_name_bytes, 8);
  unsigned name_length = 8;
  while (name_length && record->name[name_length - 1] == ' ')
    --name_length;
  if (memcmp(dos_name_bytes + 8, "   ", 3)) {
    record->name[name_length++] = '.';
    memcpy(record->name + name_length, dos_name_bytes + 8, 3);
    name_length += 3;
    while (record->name[name_length - 1] == ' ')
      --name_length;
  }
  record->name[name_length] = 0;
}
static bool dos_unrepresented_extension_disjoint(const char *name, const u8 pattern[11]) {
  for (unsigned index = 8; index < 11; ++index)
    if (pattern[index] == '?')
      return false;
  const char *extension = strrchr(name, '.');
  size_t extension_length = extension ? strlen(extension + 1) : 0;
  if (extension_length > 3)
    return false;
  u8 extension_bytes[3] = {' ', ' ', ' '};
  for (unsigned index = 0; index < extension_length; ++index) {
    u8 c = (u8)extension[1 + index];
    if (c >= 128)
      return false;
    extension_bytes[index] = c >= 'a' && c <= 'z' ? c - 32 : c;
  }
  return memcmp(pattern + 8, extension_bytes, 3) != 0;
}
bool pt_dos_find_plan_in_state(const PTDirectoryState *directory, const PTDosSearchState *search, bool first,
    const char *filename, u32 attributes, const GuestFindRecord *incoming, PTDosFindPlan *plan) {
  if (!directory_profile_valid(directory) || !search || search->first_error > 65535 || search->next_error > 65535 ||
      !incoming || !plan || directory_count > 65535 || asset_count > 32765)
    return false;
  memset(plan, 0, sizeof(*plan));
  plan->record = *incoming;
  memcpy(plan->network_reserved, search->network_reserved, sizeof(plan->network_reserved));
  u8 pattern[11], search_attributes;
  u16 directory_index = 0, last_entry = 65535;
  const char *parent = "";
  char path[1024];
  if (first) {
    if (!filename || !pt_file_resolved_name_at(directory, filename, path))
      return false;
    char *slash = strrchr(path, '/');
    const char *leaf = slash ? slash + 1 : path;
    if (!dos_name(leaf, pattern, true))
      return false;
    if (slash) {
      *slash = 0;
      parent = path;
    }
    if (strlen(parent) >= 64 || strpbrk(parent, "*?"))
      return false;
    bool found = !parent[0];
    for (unsigned i = 0; i < directory_count; ++i)
      if (!strcmp(parent, directories[i])) {
        directory_index = (u16)i;
        found = true;
        break;
      }
    search_attributes = (u8)attributes;
    if (search_attributes & ~0x27u)
      // Volume/directory/unknown branches need their own profile.
      return false;
    if (search->first_error) {
      plan->error = search->first_error;
      return true;
    }
    if (!found) {
      plan->error = 3;
      return true;
    }
  } else {
    memcpy(plan->network_reserved, incoming->continuation.network_reserved, sizeof(plan->network_reserved));
    if (search->next_error) {
      plan->error = search->next_error;
      return true;
    }
    if (incoming->continuation.drive != 3) {
      plan->error = 18;
      return true;
    }
    last_entry = incoming->continuation.last_entry;
    directory_index = incoming->continuation.directory_cluster;
    search_attributes = incoming->continuation.attributes;
    memcpy(pattern, incoming->continuation.pattern, sizeof(pattern));
    if (last_entry >= 32768 || directory_index >= directory_count) {
      plan->error = 18;
      return true;
    }
    if (search_attributes & ~0x27u)
      return false;
    parent = directories[directory_index];
  }
  size_t parent_length = strlen(parent);
  for (unsigned i = 0, directory_slot = parent[0] ? 2u : 0u; i < asset_count; ++i) {
    const char *name = assets[i].name, *slash = strrchr(name, '/');
    size_t asset_parent_length = slash ? (size_t)(slash - name) : 0;
    if (asset_parent_length != parent_length || (asset_parent_length && memcmp(name, parent, asset_parent_length)))
      continue;
    u16 entry = (u16)directory_slot++;
    if (!first && entry <= last_entry)
      continue;
    const char *leaf = slash ? slash + 1 : name;
    u8 dos_name_bytes[11];
    if (!dos_name(leaf, dos_name_bytes, false)) {
      if (dos_unrepresented_extension_disjoint(leaf, pattern))
        continue;
      // No fabricated 8.3 aliases for host long names.
      return false;
    }
    if (assets[i].attributes > 255 || (assets[i].attributes & ~0x27u))
      return false;
    if ((assets[i].attributes & 6u & ~search_attributes) || !dos_matches(pattern, dos_name_bytes))
      continue;
    GuestFindRecord *record = &plan->record;
    if (first) {
      record->continuation.drive = 3;
      memcpy(record->continuation.pattern, pattern, sizeof(record->continuation.pattern));
      record->continuation.attributes = search_attributes;
      record->continuation.directory_cluster = directory_index;
      memcpy(
          record->continuation.network_reserved, plan->network_reserved, sizeof(record->continuation.network_reserved));
    }
    record->continuation.last_entry = entry;
    dos_pack_find_record(record, &assets[i], dos_name_bytes);
    plan->write = true;
    return true;
  }
  plan->error = 18;
  return true;
}
bool pt_dos_find_plan(
    bool first, const char *filename, u32 attributes, const GuestFindRecord *incoming, PTDosFindPlan *plan) {
  return pt_dos_find_plan_in_state(&directory_state, &search_platform, first, filename, attributes, incoming, plan);
}
u32 pt_dos_find_execute(bool first, const char *filename, u32 attributes) {

  PTDosFindPlan plan;
  if (!pt_dos_find_plan(first, filename, attributes, pt_dos_find_record(), &plan))
    abort();
  memcpy(search_platform.network_reserved, plan.network_reserved, 4);
  if (plan.write)
    *pt_dos_find_record() = plan.record;

  return plan.error;
}
bool pt_file_asset_load(const char *name, const u8 *data, u32 size) {
  if (!name || (size && !data) || asset_count == UINT32_MAX)
    return false;
  char normalized[1024];
  if (!canonical_name(name, normalized))
    return false;
  for (unsigned i = 0; i < asset_count; ++i)
    if (!strcmp(assets[i].name, normalized))
      return false;
  size_t length = strlen(normalized) + 1;
  char *copy_name = malloc(length);
  u8 *copy_bytes = size ? malloc(size) : NULL;
  if (!copy_name || (size && !copy_bytes)) {
    free(copy_name);
    free(copy_bytes);
    return false;
  }
  AssetResource *grown = realloc(assets, ((size_t)asset_count + 1u) * sizeof(*assets));
  if (!grown) {
    free(copy_name);
    free(copy_bytes);
    return false;
  }
  assets = grown;
  memcpy(copy_name, normalized, length);
  if (size)
    memcpy(copy_bytes, data, size);
  assets[asset_count++] = (AssetResource){copy_name, copy_bytes, size, 0, 0, 0};
  ++disk_revision;
  return true;
}

bool pt_file_load_profile(u32 limit, u32 reserved) {
  if (!limit || limit > 32 || (limit < 32 && reserved >> limit))
    return false;
  for (unsigned i = 0; i < resource_count; ++i)
    if (resources[i].state.handle >= limit || (reserved & (UINT32_C(1) << resources[i].state.handle)))
      return false;
  file_limit = limit;
  reserved_handles = reserved;
  file_profile_ready = true;
  return true;
}

static u32 seek_position(const PTFileResource *r, u32 offset, u32 whence) {
  if ((u8)whence == 0)
    return offset;
  if ((u8)whence == 1)
    return r->state.position + offset;
  return r->state.size + offset;
}
bool pt_file_seek_ready(u32 handle, u32 offset, u32 whence) {
  (void)handle;
  (void)offset;
  (void)whence;
  // DOS ADD/ADC wraps positions to 32 bits. Invalid handles/origins are
  // supported error results; seek itself never extends file contents.

  return file_profile_ready;
}
u32 pt_file_platform_seek(u32 handle, u32 offset, u32 whence, u32 *dos_error) {
  *dos_error = 0;
  if ((u8)whence > 2) {
    *dos_error = 1;
    return 0;
  }
  if (!resource(handle) && !pt_file_reserved_character(handle)) {
    *dos_error = 6;
    return 0;
  }
  if (pt_file_reserved_character(handle))
    return 0;
  PTFileResource *r = resource(handle);
  if (!r || !pt_file_seek_ready(handle, offset, whence))
    abort();
  r->state.position = seek_position(r, offset, whence);
  return r->state.position;
}
bool pt_file_reserved_character(u32 handle) {
  return file_profile_ready && handle < 5 && (reserved_handles & (UINT32_C(1) << handle));
}
void pt_file_platform_close(u32 handle) {
  for (unsigned i = 0; i < resource_count; ++i)
    if (resources[i].state.handle == handle) {
      free(resources[i].bytes);
      free(resources[i].name);
      memmove(&resources[i], &resources[i + 1], (resource_count - i - 1u) * sizeof(*resources));
      memset(&resources[--resource_count], 0, sizeof(*resources));
      return;
    }
  abort();
}
bool pt_file_platform_open_ready(void) {
  return file_profile_ready && resource_count < PT_FILE_READ_HANDLES;
}
static void native_asset_name(const u8 *filename, char normalized[1024]) {
  char input[1024];
  unsigned i = 0;
  while (*filename == ' ')
    ++filename;
  for (; i < 1023; ++i) {
    input[i] = (char)*(filename + i);
    if (!input[i])
      break;
  }
  if (i == 1023 || !pt_file_resolved_name(input, normalized))
    abort();
}

u32 pt_file_platform_open_mode(const u8 *filename, u32 access, u32 *error) {
  if (!pt_file_platform_open_ready() || !error)
    abort();
  char normalized[1024];
  native_asset_name(filename, normalized);
  const AssetResource *a = NULL;
  for (unsigned i = 0; i < asset_count; ++i)
    if (!strcmp(assets[i].name, normalized)) {
      a = &assets[i];
      break;
    }
  if (!a) {
    *error = 2;
    return UINT32_MAX;
  }
  if ((access & 3u) == 3u) {
    *error = 12;
    return UINT32_MAX;
  }
  if ((access & 3u) && (a->attributes & 1u)) {
    *error = 5;
    return UINT32_MAX;
  }
  u32 handle = 0;
  for (; handle < file_limit; ++handle)
    if (!(reserved_handles & (UINT32_C(1) << handle)) && !resource(handle))
      break;
  if (handle == file_limit) {
    *error = 4;
    return UINT32_MAX;
  }
  if (!pt_file_read_load_handle(handle, a->bytes, a->size, 0) || !pt_file_bind_handle(handle, a->name, access & 3u))
    abort();
  *error = 0;
  return handle;
}

static AssetResource *named_asset(const char *name) {
  char normalized[1024];
  if (!canonical_name(name, normalized))
    return NULL;
  for (unsigned i = 0; i < asset_count; ++i)
    if (!strcmp(assets[i].name, normalized))
      return &assets[i];
  return NULL;
}
bool pt_file_asset_contents(const char *name, const u8 **data, size_t *size) {
  if (!name || !data || !size)
    return false;
  const AssetResource *asset = named_asset(name);
  if (!asset)
    return false;
  *data = asset->bytes;
  *size = asset->size;
  return true;
}
u32 pt_file_platform_unlink(const u8 *filename, u32 *error) {
  char name[1024];
  native_asset_name(filename, name);
  if (!error)
    abort();
  AssetResource *a = named_asset(name);
  if (!a) {
    *error = 2;
    return UINT32_MAX;
  }
  if (a->attributes & 1u) {
    *error = 5;
    return UINT32_MAX;
  }
  for (unsigned i = 0; i < resource_count; ++i)
    if (resources[i].name && !strcmp(resources[i].name, name)) {
      *error = 5;
      return UINT32_MAX;
    }
  unsigned index = (unsigned)(a - assets);
  free(a->name);
  free(a->bytes);
  memmove(a, a + 1, (asset_count - index - 1u) * sizeof(*assets));
  memset(&assets[--asset_count], 0, sizeof(*assets));
  ++disk_revision;
  *error = 0;
  return 0;
}
bool pt_file_bind_handle(u32 handle, const char *name, u32 mode) {
  PTFileResource *r = resource(handle);
  if (!r || !name || mode > 2)
    return false;
  char normalized[1024];
  if (!canonical_name(name, normalized))
    return false;
  if (!normalized[0]) {
    free(r->name);
    r->name = NULL;
    r->mode = mode;
    return true;
  }
  if (!named_asset(normalized))
    return false;
  char *copy = malloc(strlen(normalized) + 1);
  if (!copy)
    return false;
  strcpy(copy, normalized);
  free(r->name);
  r->name = copy;
  r->mode = mode;
  return true;
}

bool pt_file_asset_load_times(const char *name, u16 time, u16 date) {
  AssetResource *a = named_asset(name);
  if (!a)
    return false;
  if (a->write_time != time || a->write_date != date)
    ++disk_revision;
  a->write_time = time;
  a->write_date = date;
  return true;
}

bool pt_file_set_asset_attributes(const char *name, u32 attributes) {
  AssetResource *a = named_asset(name);
  if (!a || attributes > 65535)
    return false;
  if (a->attributes != attributes)
    ++disk_revision;
  a->attributes = attributes;
  return true;
}

bool pt_file_directory_load(const char *name) {
  char normalized[1024];
  if (!name || !canonical_name(name, normalized))
    return false;
  for (unsigned i = 0; i < directory_count; ++i)
    if (!strcmp(directories[i], normalized))
      return true;
  char *copy = malloc(strlen(normalized) + 1);
  if (!copy)
    return false;
  strcpy(copy, normalized);
  char **grown = realloc(directories, (directory_count + 1u) * sizeof(*directories));
  if (!grown) {
    free(copy);
    return false;
  }
  directories = grown;
  directories[directory_count++] = copy;
  ++disk_revision;
  return true;
}
static void replace_bytes(u8 **target, u32 old_size, const u8 *data, u32 size) {
  (void)old_size;
  u8 *copy = size ? malloc(size) : NULL;
  if (size && !copy)
    abort();
  if (size)
    memcpy(copy, data, size);
  free(*target);
  *target = copy;
}
static void publish_file(const char *name, const u8 *data, u32 size) {
  AssetResource *a = named_asset(name);
  if (!a)
    abort();
  replace_bytes(&a->bytes, a->size, data, size);
  a->size = size;
  ++disk_revision;
  for (unsigned i = 0; i < resource_count; ++i)
    if (resources[i].name && !strcmp(resources[i].name, name)) {
      replace_bytes(&resources[i].bytes, resources[i].state.size, a->bytes, size);
      resources[i].state.size = size;
    }
}

u32 pt_file_platform_create(const u8 *filename, u32 attributes, u32 *error) {
  char name[1024];
  unsigned n = 0;
  while (*filename == ' ')
    ++filename;
  for (; n < 1023; ++n) {
    name[n] = (char)*(filename + n);
    if (!name[n])
      break;
  }
  char normalized[1024];
  if (n == 1023 || !pt_file_resolved_name(name, normalized) || !pt_file_platform_open_ready() || !error)
    abort();
  u32 handle = 0;
  for (; handle < file_limit; ++handle)
    if (!(reserved_handles & (UINT32_C(1) << handle)) && !resource(handle))
      break;
  if (handle == file_limit) {
    *error = 4;
    return UINT32_MAX;
  }
  char parent[1024];
  strcpy(parent, normalized);
  char *slash = strrchr(parent, '/');
  if (slash)
    *slash = 0;
  else
    parent[0] = 0;
  bool exists = !parent[0];
  for (unsigned i = 0; i < directory_count; ++i)
    if (!strcmp(parent, directories[i]))
      exists = true;
  size_t length = strlen(parent);
  for (unsigned i = 0; i < asset_count; ++i)
    if (!strncmp(parent, assets[i].name, length) && assets[i].name[length] == '/')
      exists = true;
  if (!exists) {
    *error = 3;
    return UINT32_MAX;
  }
  AssetResource *a = named_asset(normalized);
  if (a && (a->attributes & 1u)) {
    *error = 5;
    return UINT32_MAX;
  }
  if (!a && !pt_file_asset_load(normalized, NULL, 0))
    abort();
  publish_file(normalized, NULL, 0);
  a = named_asset(normalized);
  a->attributes = attributes & 65535u;
  if (!pt_file_read_load_handle(handle, NULL, 0, 0) || !pt_file_bind_handle(handle, normalized, 2))
    abort();
  *error = 0;
  return handle;
}
bool pt_file_write_platform_ready(u32 handle, u32 size) {
  const PTFileResource *r = resource(handle);
  return !r || (u64)r->state.position + size <= UINT32_MAX;
}
u32 pt_file_platform_write(u32 handle, const u8 *data, u32 size, u32 *error) {
  PTFileResource *r = resource(handle);
  if (!error)
    abort();
  if (!r) {
    *error = 6;
    return UINT32_MAX;
  }
  if (!r->mode) {
    *error = 5;
    return UINT32_MAX;
  }
  if (!pt_file_write_platform_ready(handle, size) || (size && !data))
    abort();
  u32 end = r->state.position + size, new_size = size ? (end > r->state.size ? end : r->state.size) : r->state.position;
  u8 *copy = new_size ? calloc(new_size, 1) : NULL;
  if (new_size && !copy)
    abort();
  u32 retained = r->state.size < new_size ? r->state.size : new_size;
  if (retained)
    memcpy(copy, r->bytes, retained);
  if (size)
    memcpy(copy + r->state.position, data, size);
  if (r->name)
    publish_file(r->name, copy, new_size);
  else {
    replace_bytes(&r->bytes, r->state.size, copy, new_size);
    r->state.size = new_size;
  }
  free(copy);
  r->state.position = end;
  *error = 0;
  return size;
}

// Host persistence serializes platform resources, never reconstructed RAM or
// caller continuations. A failed import leaves the existing disk untouched.

u64 pt_file_disk_revision(void) {
  return disk_revision;
}
bool pt_file_disk_quiet(void) {
  for (unsigned i = 0; i < resource_count; ++i)
    if (resources[i].mode)
      return false;
  return true;
}
static bool disk_put_u32(FILE *f, u32 value) {
  u8 bytes[4];
  for (unsigned i = 0; i < 4; ++i)
    bytes[i] = (u8)(value >> (8 * i));
  return fwrite(bytes, 1, 4, f) == 4;
}
static bool disk_get_u32(FILE *f, u32 *value) {
  u8 bytes[4];
  if (fread(bytes, 1, 4, f) != 4)
    return false;
  *value = 0;
  for (unsigned i = 0; i < 4; ++i)
    *value |= (u32)bytes[i] << (8 * i);
  return true;
}
static bool disk_put_name(FILE *f, const char *name) {
  u32 size = (u32)strlen(name) + 1u;
  return disk_put_u32(f, size) && fwrite(name, 1, size, f) == size;
}
static char *disk_get_name(FILE *f, bool directory) {
  u32 size;
  if (!disk_get_u32(f, &size) || !size || size > 1024)
    return NULL;
  char name[1024], normalized[1024];
  if (fread(name, 1, size, f) != size || name[size - 1] || strlen(name) + 1u != size ||
      !canonical_name(name, normalized) || strcmp(name, normalized) || (!directory && !name[0]))
    return NULL;
  char *copy = malloc(size);
  if (copy)
    memcpy(copy, name, size);
  return copy;
}
bool pt_file_disk_write(FILE *f) {
  if (!f || !pt_file_disk_quiet() || fwrite("PTDISK01", 1, 8, f) != 8 || !disk_put_u32(f, directory_count) ||
      !disk_put_u32(f, asset_count))
    return false;
  for (unsigned i = 0; i < directory_count; ++i)
    if (!disk_put_name(f, directories[i]))
      return false;
  for (unsigned i = 0; i < asset_count; ++i) {
    const AssetResource *a = &assets[i];
    if (!disk_put_name(f, a->name) || !disk_put_u32(f, a->size) || !disk_put_u32(f, a->attributes) ||
        !disk_put_u32(f, (u32)a->write_time | ((u32)a->write_date << 16)) ||
        (a->size && fwrite(a->bytes, 1, a->size, f) != a->size))
      return false;
  }
  return !ferror(f);
}
bool pt_file_disk_load(FILE *f) {
  if (!f || resource_count || game_ptr || ptr_apple)
    return false;
  char magic[8];
  u32 dirs, count;
  if (fread(magic, 1, 8, f) != 8 || memcmp(magic, "PTDISK01", 8) || !disk_get_u32(f, &dirs) ||
      !disk_get_u32(f, &count) || dirs > 65536 || count > 65536)
    return false;
  char **incoming_dirs = calloc(dirs ? dirs : 1, sizeof(*incoming_dirs));
  AssetResource *incoming = calloc(count ? count : 1, sizeof(*incoming));
  bool okay = incoming_dirs && incoming;
  u64 total = 0;
  for (unsigned i = 0; okay && i < dirs; ++i) {
    incoming_dirs[i] = disk_get_name(f, true);
    okay = incoming_dirs[i] != NULL;
    for (unsigned j = 0; okay && j < i; ++j)
      okay = strcmp(incoming_dirs[i], incoming_dirs[j]) != 0;
  }
  for (unsigned i = 0; okay && i < count; ++i) {
    AssetResource *a = &incoming[i];
    u32 stamp;
    a->name = disk_get_name(f, false);
    okay = a->name && disk_get_u32(f, &a->size) && disk_get_u32(f, &a->attributes) && a->attributes <= 65535 &&
        disk_get_u32(f, &stamp);
    if (!okay)
      break;
    for (unsigned j = 0; okay && j < i; ++j)
      okay = strcmp(a->name, incoming[j].name) != 0;
    total += a->size;
    if (a->size > 64u * 1024u * 1024u || total > 256u * 1024u * 1024u)
      okay = false;
    if (!okay)
      break;
    a->write_time = (u16)stamp;
    a->write_date = (u16)(stamp >> 16);
    if (a->size) {
      a->bytes = malloc(a->size);
      okay = a->bytes && fread(a->bytes, 1, a->size, f) == a->size;
    }
  }
  if (okay)
    okay = fgetc(f) == EOF && !ferror(f);
  if (okay) {
    for (unsigned i = 0; i < asset_count; ++i) {
      free(assets[i].name);
      free(assets[i].bytes);
    }
    free(assets);
    assets = incoming;
    asset_count = count;
    incoming = NULL;
    for (unsigned i = 0; i < directory_count; ++i)
      free(directories[i]);
    free(directories);
    directories = incoming_dirs;
    directory_count = dirs;
    incoming_dirs = NULL;
    ++disk_revision;
  }
  if (incoming) {
    for (unsigned i = 0; i < count; ++i) {
      free(incoming[i].name);
      free(incoming[i].bytes);
    }
    free(incoming);
  }
  if (incoming_dirs) {
    for (unsigned i = 0; i < dirs; ++i)
      free(incoming_dirs[i]);
    free(incoming_dirs);
  }
  return okay;
}

// File write.

#include "game.h"

// Stock sprintf writes into fixed guest stack locals. Reject an overflowing
// filename before formatting; no truncated diagnostic can establish parity.

static void file_error(u8 *local, u32 capacity, const u8 *filename, const char *format) {
  snprintf((char *)local, capacity, format, (const char *)filename);

  DisplayErrorMessage(0, local);
}

// Watcom's original unsigned arithmetic and FILE representation. The DOS
// adapter owns independent file bytes; none of these CRT routines are hooked
// out of the original machine-code execution.

static u32 dos_write(u32 handle, const void *source, u32 size) {
  u32 error;
  u32 result = pt_file_platform_write(handle, size ? source : NULL, size, &error);
  return result == UINT32_MAX ? GuestErrnoFromDOS(error) : result;
}
static u32 append_position(u32 handle) {
  u32 position = GuestLSeek(handle, 0, 2);
  // The original treats positions >= 2 GiB as signed error results.

  return position <= INT32_MAX ? 0 : GuestErrnoFromDOS(position & 0xffffu);
}
u32 GuestQuickWrite(u32 handle, const void *source, u32 size) {
  if ((GuestIOMode(handle) & 0x80u) && append_position(handle))
    return UINT32_MAX;
  u32 result = dos_write(handle, source, size);
  if (result != UINT32_MAX && result != size)
    GuestSetErrno(12);
  return result;
}
u32 GuestWrite(u32 handle, const void *source, u32 size) {
  u32 result = UINT32_MAX, mode = GuestIOMode(handle);
  if (!mode)
    GuestSetErrno(4);
  else if (!(mode & 2u))
    GuestSetErrno(6);
  // errno already set
  else if ((mode & 0x80u) && append_position(handle)) {
  } else if (mode & 0x40u) {
    result = dos_write(handle, source, size);
    if (result != UINT32_MAX && result != size)
      GuestSetErrno(12);
  } else {
    u32 chunk = 512;
    u8 scratch_storage[512];
    u8 *scratch = scratch_storage;
    u32 used = 0, consumed = 0, last = 0;
    result = size;
    while (consumed < size) {
      u8 byte = ((const u8 *)source)[consumed];
      if (byte == 10) {
        scratch[used++] = 13;
        if (used == chunk) {
          u32 amount = dos_write(handle, scratch, used);
          if (amount != used) {
            if (amount != UINT32_MAX)
              GuestSetErrno(12);
            result = amount == UINT32_MAX ? amount : last + amount;
            goto done_text;
          }
          used = 0;
          last = consumed;
        }
      }
      scratch[used++] = byte;
      ++consumed;
      if (used == chunk) {
        u32 amount = dos_write(handle, scratch, used);
        if (amount != used) {
          if (amount != UINT32_MAX)
            GuestSetErrno(12);
          result = amount == UINT32_MAX ? amount : last + amount;
          goto done_text;
        }
        used = 0;
        last = consumed;
      }
    }
    if (used) {
      u32 amount = dos_write(handle, scratch, used);
      if (amount != used) {
        if (amount != UINT32_MAX)
          GuestSetErrno(12);
        result = amount == UINT32_MAX ? amount : last + amount;
      }
    }
  done_text:;
  }

  return result;
}
u32 GuestFlush(GuestFileStream *stream) {
  u32 result = 0;
  if (stream->buffer_flags & 0x10u) {
    stream->buffer_flags &= 0xefu;
    if ((stream->flags & 2u) && stream->buffer) {
      u32 amount = GuestQuickWrite(stream->handle, stream->buffer, stream->buffered_bytes);
      if (amount == UINT32_MAX) {
        stream->flags |= 0x20u;
        result = UINT32_MAX;
      } else if (amount != stream->buffered_bytes) {
        GuestSetErrno(12);
        stream->flags |= 0x20u;
        result = UINT32_MAX;
      }
    }
  } else if (stream->buffer) {
    stream->flags &= 0xefu;
    if (!(stream->buffer_flags & 0x20u) && stream->buffered_bytes &&
        GuestLSeek(stream->handle, 0u - stream->buffered_bytes, 1) == UINT32_MAX) {
      stream->flags |= 0x20u;
      result = UINT32_MAX;
    }
  }
  stream->buffered_bytes = 0;
  stream->cursor = (PTBufferCursor){stream->buffer, 0};
  return result;
}
u32 GuestFputc(u32 value, GuestFileStream *stream) {
  if (!(stream->flags & 2u)) {
    GuestSetErrno(4);
    stream->flags |= 0x20u;
    return UINT32_MAX;
  }
  if (!stream->buffer)
    GuestIOAllocate(stream);
  u8 flush_mask = 4;
  if (value == 10) {
    flush_mask = 6;
    if (!(stream->flags & 0x40u)) {
      stream->buffer_flags |= 0x10u;
      pt_file_buffer_append(stream, 13);
      stream->buffered_bytes += 1;
      if (stream->buffered_bytes == stream->buffer_size && GuestFlush(stream))
        return UINT32_MAX;
    }
  }
  stream->buffer_flags |= 0x10u;
  pt_file_buffer_append(stream, (u8)value);
  stream->buffered_bytes += 1;
  if ((stream->buffer_flags & flush_mask) || stream->buffered_bytes == stream->buffer_size)
    if (GuestFlush(stream))
      return UINT32_MAX;
  return (u8)value;
}
u32 GuestFwrite(const void *source, u32 size, u32 count, GuestFileStream *stream) {
  if (!(stream->flags & 2u)) {
    GuestSetErrno(4);
    stream->flags |= 0x20u;
    return 0;
  }
  u32 remaining = size * count;
  if (!remaining)
    return 0;

  if (!stream->buffer)
    GuestIOAllocate(stream);
  const u8 *cursor = source;
  u32 saved = stream->flags & 0x30u, written = 0;
  stream->flags &= 0xcfu;
  if (stream->flags & 0x40u)
    do {
      u32 amount;
      if (stream->buffered_bytes || remaining < stream->buffer_size) {
        amount = stream->buffer_size - stream->buffered_bytes;
        if (amount > remaining)
          amount = remaining;
        memmove(pt_file_buffer_data(stream), cursor, amount);

        stream->cursor.position += amount;
        stream->buffered_bytes += amount;
        stream->buffer_flags |= 0x10u;
        if (stream->buffered_bytes == stream->buffer_size || (stream->buffer_flags & 4u))
          GuestFlush(stream);
      } else {
        amount = remaining & ~UINT32_C(511);
        if (!amount)
          amount = remaining;

        u32 actual = GuestWrite(stream->handle, cursor, amount);

        if (actual == UINT32_MAX)
          stream->flags |= 0x20u;
        else if (actual != amount) {
          GuestSetErrno(12);
          stream->flags |= 0x20u;
        }
      }
      cursor += amount;
      written += amount;
      remaining -= amount;
    } while (remaining && !(stream->flags & 0x20u));
  else {
    bool temporary = (stream->buffer_flags & 4u) != 0;
    if (temporary)
      stream->buffer_flags = (stream->buffer_flags & 0xfau) | 1u;
    do {
      GuestFputc(*cursor++, stream);
      if (stream->flags & 0x30u)
        break;
      ++written;
    } while (written != remaining);
    if (temporary) {
      stream->buffer_flags = (stream->buffer_flags & 0xfau) | 4u;
      GuestFlush(stream);
    }
  }
  if (stream->flags & 0x20u)
    written = 0;
  stream->flags |= saved;

  return written / size;
}
static i64 signed32(u32 value) {
  return value <= INT32_MAX ? value : (i64)value - INT64_C(0x100000000);
}
u32 GuestUpdateBuffer(u32 offset, GuestFileStream *stream) {
  stream->flags &= 0xefu;
  if (signed32(offset) > signed32(stream->buffered_bytes) ||
      signed32(offset) < signed32(pt_file_buffer_distance(stream))) {
    stream->buffered_bytes = 0;
    stream->cursor = (PTBufferCursor){stream->buffer, 0};
    return 1;
  }
  stream->cursor.position += offset;
  stream->buffered_bytes -= offset;
  return 0;
}
u32 GuestFseek(GuestFileStream *stream, u32 offset, u32 whence) {
  if (stream->flags & 6u) {
    if (stream->buffer_flags & 0x10u) {
      if (GuestFlush(stream)) {
        if (!whence && offset > INT32_MAX)
          GuestSetErrno(9);
        return UINT32_MAX;
      }
    } else {
      if (whence == 1)
        offset -= stream->buffered_bytes;
      stream->buffered_bytes = 0;
      stream->cursor = (PTBufferCursor){stream->buffer, 0};
    }
    stream->flags &= 0xebu;
  } else if (!whence) {
    u32 position = GuestTell(stream->handle);
    if (!GuestUpdateBuffer(offset - (position - stream->buffered_bytes), stream))
      return 0;
  } else if (whence == 1) {
    u32 count = stream->buffered_bytes;
    if (!GuestUpdateBuffer(offset, stream))
      return 0;
    offset -= count;
  } else if (whence == 2) {
    stream->buffered_bytes = 0;
    stream->flags &= 0xefu;
    stream->cursor = (PTBufferCursor){stream->buffer, 0};
  } else {
    GuestSetErrno(9);
    return UINT32_MAX;
  }
  return GuestLSeek(stream->handle, offset, whence) == UINT32_MAX ? UINT32_MAX : 0;
}
u32 WriteFile(GuestFileStream *stream, const void *source, u32 size) {
  u32 result = GuestFwrite(source, 1, size, stream);

  return result;
}

u32 OpenFileForWritingAsCurrent(const u8 *filename) {
  u8 error_message[200];
  pt_file_set_current_filename(filename);

  cur_writing_filehandle = OpenFile(filename, 1);

  if (!cur_writing_filehandle) {
    file_error(error_message, 200, filename, "Cannot find file '%s'!");

    return 0;
  }
  num_written_bytes = 0;

  return 1;
}
u32 WriteBytesToCurrentFile(const void *source, u32 size) {
  u8 error_message[100];
  u32 result = 0;
  if (cur_writing_filehandle && source) {
    result = WriteFile(cur_writing_filehandle, source, size);

    num_written_bytes += result;
    if (result != size) {
      file_error(error_message, 100, pt_file_current_filename(), "Error writing file %s!");
      result = 0;
    }
  }

  return result;
}
u32 CloseCurrentWritingFile(void) {
  u32 result = CloseFile(cur_writing_filehandle);

  cur_writing_filehandle = NULL;
  return result;
}
u32 WriteHighscoresFile(const void *table) {
  u32 result = OpenFileForWritingAsCurrent(highscores_filename);

  if (!(u16)result)
    return result;

  WriteBytesToCurrentFile(table, 860);

  // Original tail jump, no extra return slot.
  return CloseCurrentWritingFile();
}

// Asset loader.

// The file header is two little-endian words followed by a dword. The
// descriptor supplied by the caller keeps its dimensions and flags.

Game *LoadBackground(const u8 *filename, Bitmap *bitmap) {
  u8 *destination = bitmap->surface.pixels;
  u16 width, height;
  u32 compressed;
  Bitmap temporary;
  bool screen = pt_bitmap_is_display_page(bitmap);
  DisableDiskReadErrors();
  OpenFileAsCurrent(filename);
  ReadBytesFromCurrentFile(&width, sizeof(width));
  ReadBytesFromCurrentFile(&height, sizeof(height));
  ReadBytesFromCurrentFile(&compressed, sizeof(compressed));
  u32 pixels = (u32)width * height;
  if (screen) {
    InitializeBitmap(&temporary, (i16)width, (i16)height);
    pt_bitmap_bind_pixels(&temporary, pt_buffer_allocate(pixels, false), pixels);
    destination = temporary.surface.pixels;
  }
  if (compressed) {
    u8 *data = pt_buffer_allocate(compressed, false);
    ReadBytesFromCurrentFile(data, compressed);
    DecompressBackground(data, destination);
    pt_buffer_release(data);
  } else
    ReadBytesFromCurrentFile(destination, pixels);
  if (screen) {
    BitBlt(&temporary, 0, 0, bitmap, 0, 0, width, height);
    pt_buffer_release(destination);
  }
  CloseCurrentFile();
  return EnableDiskReadErrors();
}

// Record the background index after the image has finished loading.

static void background_name(u8 index, u8 name[16]) {
  memcpy(name, "gfx\\back", 8);
  unsigned pos = 8;
  if (index >= 100)
    name[pos++] = (u8)('0' + index / 100u);
  if (index >= 10)
    name[pos++] = (u8)('0' + index / 10u % 10u);
  name[pos++] = (u8)('0' + index % 10u);
  memcpy(name + pos, ".vga", 5);
}
Game *LoadGameBackground(u8 index) {
  u8 name[40];
  background_name(index, name);
  LoadBackground(name, game_ptr->background);
  game_ptr->last_loaded_background = index;
  return game_ptr;
}

// File browser.

#include "text.h"
#include "restaurant.h"
#include <assert.h>
#include "calendar.h"
#include "world.h"
#include <stdio.h>

_Static_assert(sizeof(selected_filename) == PT_SELECTOR_FILENAME_CAPACITY, "selector filename capacity");

// directory runtime

u8 *GuestStrncpy(u8 *destination, const u8 *source, u32 count) {
  u8 *result = destination;
  while (count) {
    u8 byte = *source;
    if (!byte)
      break;
    ++source;
    --count;
    *destination++ = byte;
  }
  while (count) {
    --count;
    *destination++ = 0;
  }

  return result;
}
u8 *GuestGetcwd(u8 *destination, u32 capacity) {
  u8 path[144];
  const PTDirectoryState *directory = pt_directory_state();
  if (directory->getcwd_error) {
    GuestSetErrno(1);
    return NULL;
  }
  u32 n = (u32)strlen(directory->cwd);
  for (unsigned i = 0; i < n; ++i) {
    u8 c = (u8)directory->cwd[i];
    path[3 + i] = c == '/' ? '\\' : c >= 'a' && c <= 'z' ? c - 32u : c;
  }
  path[3 + n] = 0;
  path[0] = 'C';
  path[1] = ':';
  path[2] = '\\';
  if (!destination) {
    capacity = n + 4u;
    destination = pt_buffer_allocate(capacity, false);
    if (!destination) {
      GuestSetErrno(5);
      return NULL;
    }
  }
  u8 *result = GuestStrncpy(destination, path, capacity);
  return result;
}
u32 GuestChdir(const u8 *filename) {
  char result[64];
  u32 error;
  if (!pt_directory_plan((const char *)filename, result, &error))
    abort();
  if (error) {
    u32 value = GuestErrnoFromDOS(error);
    return value;
  }
  pt_directory_commit(result);
  return 0;
}

// file search

u32 GuestDOSError(u32 value, bool carry) {
  if (!carry)
    return 0;
  value &= UINT16_MAX;
  GuestErrnoFromDOS(value);
  return value;
}

u32 GuestDOSFindNextError(u32 value, bool carry) {
  u32 result = GuestDOSError(value, carry);
  return result;
}

static void copy_dta(GuestFindRecord *buffer, bool out) {
  if (state_slot_012 != 9)
    return;
  GuestFindRecord *dta = pt_dos_find_record();
  u8 *destination = (u8 *)(out ? buffer : dta);
  const u8 *source = (const u8 *)(out ? dta : buffer);
  // The original copies forward even for partially overlapping records.
  // memcpy/memmove or structure assignment would change that behavior.

  for (size_t i = 0; i < sizeof(*buffer); ++i)
    destination[i] = source[i];
}

void GuestCopyDTAOut(GuestFindRecord *buffer) {
  copy_dta(buffer, true);
}

void GuestCopyDTAIn(GuestFindRecord *buffer) {
  copy_dta(buffer, false);
}

u32 GuestDOSFindFirst(const u8 *filename, u32 attributes, GuestFindRecord *buffer) {
  pt_dos_set_find_record(buffer);
  u32 error = pt_dos_find_execute(true, (const char *)filename, attributes);
  u32 result = GuestDOSError(error, error != 0);
  GuestCopyDTAOut(buffer);
  return result;
}

u32 GuestDOSFindNext(GuestFindRecord *buffer) {
  pt_dos_set_find_record(buffer);
  GuestCopyDTAIn(buffer);
  u32 error = pt_dos_find_execute(false, NULL, 0);
  u32 result = GuestDOSFindNextError(error, error != 0);
  GuestCopyDTAOut(buffer);
  return result;
}

// file menu ui

static void draw_menu_sprite(u16 image, i16 x, i16 y) {
  SetupCanvasTempIglooLink();

  SetupHazelWithBobSprite(&temp_hazel, 0, &menu_bob, image);

  temp_igloo.draw_x = x;
  temp_igloo.draw_y = y;

  Canvas_DrawAllIgloos(temp_canvas, active_palette_pointer);
}
static void caption_tail(const u8 *text) {
  draw_menu_sprite(1, 22, 0);
  temp_canvas->color = 1;

  SetSomeFieldInCanvas(0);

  SetCurrentFont(loaded_font);

  DrawText2(147, 235, 51, text);

  CopyBitmap2to1();
}
void DrawIntroCaption(const u8 *text) {
  FillRect(0, 0, 0, 319, 199);
  temp_canvas->color = 13;

  SetTempCanvasPosition(10, 56);

  DefaultLineTo(310, 56);

  caption_tail(text);
}
void DrawFileSelectorCaption(const u8 *text) {
  FillRect(0, 0, 0, 319, 55);
  caption_tail(text);
}
void ClearMissionGoalRect(void) {
  FillRect(0, 20, 75, 128, 185);
}
void DrawMainMenuListboxes(u8 right) {
  SetCurrentFont(loaded_font);

  FillRect(14, 16, 67, 128, 200);

  ClearMissionGoalRect();

  for (unsigned side = 0; side < 1u + (unsigned)(right != 0); ++side) {
    u16 x = side ? 198 : 18;

    Canvas_DrawRect(1, x, 64, side ? 310 : 130, 197);

    draw_menu_sprite(3, (i16)x, 63);
    draw_menu_sprite(4, (i16)x, 186);
  }
}
u8 CheckMouseInsideListbox(void) {
  static const i16 r[6][4] = {{18, 63, 130, 73},
    {18, 186, 130, 196},
    {198, 63, 310, 73},
    {198, 186, 310, 196},
    {18, 73, 130, 186},
    {198, 73, 310, 186}};
  for (unsigned i = 0; i < 6; ++i) {
    bool hit = IsMouseInside(r[i][0], r[i][1], r[i][2], r[i][3]);

    if (hit)
      return (u8)(i + 1);
  }
  return 0;
}
i32 GetSelectedListItem(i16 top, i16 spacing) {
  assert(spacing != 0);
  i32 result = ((i32)mouse_y - top) / spacing;

  return result;
}

// file selector ui

static u8 *row(PTFileSelector *f, u32 index) {
  if (index >= PT_FILE_SELECTOR_ROWS)
    pt_resource_bounds_failure("file selector row", index, PT_FILE_SELECTOR_ROWS);
  return f->filenames[index];
}
static void copy_found(PTFileSelector *f) {
  const u8 *end = memchr(f->find_record.name, 0, sizeof(f->find_record.name));
  if (!end)
    pt_resource_bounds_failure("file selector name", sizeof(f->find_record.name), sizeof(f->find_record.name));

  u8 *dest = row(f, f->file_count);
  memcpy(dest, f->find_record.name, (size_t)(end - f->find_record.name) + 1u);
  ++f->file_count;
}
static void highlight(PTFileSelector *f, i8 selected, u16 background) {
  if (selected < 0 || selected >= f->visible_rows)
    return;
  u16 y = (u16)(10u * (u8)selected);
  FillRect(background, 21, y + 75u, 128, y + 83u);
  (void)SetTempCanvasPosition(21, (i16)(y + 82u));
  temp_canvas->color = 1;
  DrawListElement(row(f, (u8)selected + f->scroll_offset));
}
// This invocation owns the live selector and its search record.

u8 GetSaveOrLoadFilename(u8 mode) {
  PTFileSelector storage = {0};
  PTFileSelector *f = &storage;
  u8 result = 254;
  u32 error;
  memcpy(f->patterns, selector_patterns, 24);
  f->scroll_offset = 0;
  f->visible_rows = 11;
  memset(f->filenames, 0, sizeof(f->filenames));
  Delay(2);
  (void)GuestGetcwd(f->saved_cwd, sizeof(f->saved_cwd));
  Delay(2);
  const u8 *family = selector_family_titles[mode / 2u], *action = mode & 1u ? selector_save_label : selector_load_label;

  (void)snprintf((char *)f->title, 24, "%s %s", family, action);
  Delay(2);
  DrawFileSelectorCaption(f->title);
  Delay(2);
  Delay(2);
  (void)GuestChdir(selector_game_directory);
  Delay(2);
  DrawMainMenuListboxes(0);
  Delay(2);
  SetCurrentFont(font_piznumb);
  Delay(2);
  f->file_count = 0;
  GuestFindRecord *record = &f->find_record;
  error = GuestDOSFindFirst(f->patterns[mode / 2u], 0, record);
  if (!error) {
    copy_found(f);
    for (;;) {
      error = GuestDOSFindNext(record);
      if (error)
        break;
      if (f->file_count >= 167u - (mode & 1u)) {
        goto restore;
      }
      copy_found(f);
    }
  }
  Delay(2);
  if (!f->file_count) {
    Delay(2);
    temp_canvas->color = 1;
    SetCurrentFont(font_piznumb);
    (void)SetTempCanvasPosition(30, 100);
    static const u8 *const empty[] = {selector_no_games, selector_no_characters, selector_no_missions};
    DrawListElement(empty[mode / 2u]);
    if (!(mode & 1u)) {
      ReadMouseEvent();
      result = 0;
      goto restore;
    }
  }
  if (mode & 1u) {
    for (u8 i = (u8)f->file_count; i; --i) {
      memcpy(row(f, i), row(f, i - 1u), sizeof(f->filenames[0]));
    }
    memcpy(f->filenames[0], selector_new_name, 13);
    ++f->file_count;
  }
  i8 selected = -1;
redraw:
  CopyBitmap2to1();
  SetCurrentBitmap(old_bitmap);
  FillRect(0, 20, 75, 128, 185);
  u32 rows = f->file_count - f->scroll_offset;
  if (rows > f->visible_rows)
    rows = f->visible_rows;
  for (u32 j = 0; j < rows; ++j) {
    (void)SetTempCanvasPosition(21, (i16)(10u * j + 82u));
    temp_canvas->color = 1;
    DrawListElement(row(f, j + f->scroll_offset));
  }
  CommitBitmapAndSetPalette(0);
  mouse_pressed = 0;
  do {
    if (mouse_pressed)
      goto restore;
    temp_canvas->color = 1;
    i8 before = selected;
    bool inside;
    u32 count = f->file_count < f->visible_rows ? f->file_count : f->visible_rows;
    inside = IsMouseInside(20, 74, 128, (i16)(10u * count + 74u));
    if (inside) {
      i32 item;
      item = GetSelectedListItem(74, 10);
      selected = (i8)item;
    } else
      selected = -1;
    if (before != selected) {
      highlight(f, before, 0);
      highlight(f, selected, 14);
    }
    mouse_pressed = WaitForMouseClick();
  } while (mouse_pressed != 1);
  u8 box;
  box = CheckMouseInsideListbox();
  if (box == 2) {
    if (f->scroll_offset + f->visible_rows < f->file_count)
      ++f->scroll_offset;
    goto redraw;
  }
  box = CheckMouseInsideListbox();
  if (box == 1) {
    if (f->scroll_offset)
      --f->scroll_offset;
    goto redraw;
  }
  if (selected < 0)
    goto restore;
  if (mode & 1u) {
    if ((u8)selected + f->scroll_offset == 0) {
      cur_list_elem_y = 74;
      SetupCanvasTempIglooLink();
      SetupHazelWithBobSprite(&temp_hazel, 0, &menu_bob, 6);
      temp_igloo.draw_x = 18;
      temp_igloo.draw_y = cur_list_elem_y;
      Canvas_DrawAllIgloos(temp_canvas, active_palette_pointer);
      cur_list_elem_x = 23;
      cur_list_elem_y += 12;
      temp_canvas->color = 1;
      SetCurrentFont(font_piznumb);
      memcpy(selected_filename, selector_filename_prefix, 6);
      HandleTextEditing(selected_filename + 5, 208);
      if (!selected_filename[5]) {
        memcpy(selected_filename + 5, selector_error_name, 6);
      }
      result = 255;
      goto restore;
    }
    u8 overwrite;
    overwrite = ShowMessage2(0, selector_overwrite_prompt);
    if (!overwrite)
      goto restore;
  }
  memcpy(selected_filename, selector_filename_prefix, 6);
  const u8 *name_bytes = row(f, (u8)selected + f->scroll_offset);
  const u8 *name_end = memchr(name_bytes, 0, sizeof(f->filenames[0]));

  memcpy(selected_filename + 5, name_bytes, (size_t)(name_end - name_bytes) + 1u);
  if (mode & 1u) {
    u32 length;
    length = pt_strlen(selected_filename);
    // Strip the extension four bytes before the filename terminator.

    *pt_selector_extension_end(length) = 0;
  }
  result = 255;
restore:
  (void)GuestChdir(f->saved_cwd);

  return result;
}
