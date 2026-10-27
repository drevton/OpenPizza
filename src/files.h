// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
#ifndef PT_FILES_H
#define PT_FILES_H
#include "layout.h"
#include "types.h"
#include "memory.h"

// File read.

#include <stdbool.h>

enum { PT_FILE_READ_HANDLES = 128 };
typedef struct PTReadHandle {
  u32 handle, size, position;
} PTReadHandle;
_Static_assert(sizeof(PTReadHandle) == 12, "read handle snapshot ABI");
// Host file ownership and positions stay outside serialized game memory.

typedef struct PTFileResource {
  PTReadHandle state;
  u8 *bytes;
  char *name;
  u32 mode;
} PTFileResource;

// Handle loading seeds native platform storage; each handle advances independently.

void pt_file_read_reset(void);
bool pt_file_read_load_handle(u32 handle, const u8 *data, u32 size, u32 position);

u32 *GuestErrnoAddress(void);
u32 *GuestSetErrno(u32 value);
u32 GuestIsTTY(u32 handle);
void GuestCheckTTY(GuestFileStream *stream);
u8 *GuestIOAllocate(GuestFileStream *stream);
u32 GuestQuickRead(u32 handle, void *destination, u32 size);
u32 GuestFlushMatchingStreams(u32 mask);
u32 GuestGetche(void);
u32 GuestFillBuffer(GuestFileStream *stream);
u32 GuestFread(void *destination, u32 size, u32 count, GuestFileStream *stream);
u32 ReadFile(GuestFileStream *stream, void *destination, u32 size);
u32 ReadBytesFromCurrentFile(void *destination, u32 size);
Game *EnableDiskReadErrors(void);
Game *DisableDiskReadErrors(void);
bool pt_file_reserved_character(u32 handle);

bool pt_file_redirected_character_available(void);

// File label.

#include <stdbool.h>
#include <stddef.h>
// File operations and imports retain their own filename spelling.

void pt_file_set_current_filename(const u8 *filename);
const u8 *pt_file_current_filename(void);
// Copy an imported spelling without an operation, sequence increment or alias.

void pt_file_label_import(const u8 *filename, size_t length);

void pt_file_label_reset(void);

// File lifecycle.

#include <stdio.h>
typedef struct PTFileModeSlot PTFileModeSlot;

// Host disk persistence is read-only during play. Import is initialization-only,
// before GameMain and with no open DOS handles. The format retains DOS order.

u64 pt_file_disk_revision(void);
bool pt_file_disk_quiet(void);
bool pt_file_disk_write(FILE *output);
bool pt_file_disk_load(FILE *input);
bool pt_file_asset_load(const char *name, const u8 *data, u32 size);
// Borrow the current resource bytes without opening a game handle or changing disk state.
bool pt_file_asset_contents(const char *name, const u8 **data, size_t *size);
bool pt_file_load_profile(u32 limit, u32 reserved);
bool pt_file_seek_ready(u32 handle, u32 offset, u32 whence);
u32 pt_file_platform_seek(u32 handle, u32 offset, u32 whence, u32 *dos_error);
void pt_file_platform_close(u32 handle);
bool pt_file_platform_open_ready(void);
u32 pt_file_platform_unlink(const u8 *filename, u32 *error);
u32 pt_file_platform_open_mode(const u8 *filename, u32 access, u32 *error);
u32 pt_file_platform_create(const u8 *filename, u32 attributes, u32 *error);
u32 pt_file_platform_write(u32 handle, const u8 *bytes, u32 size, u32 *error);
bool pt_file_bind_handle(u32 handle, const char *name, u32 mode);
bool pt_file_set_asset_attributes(const char *name, u32 attributes);
bool pt_file_directory_load(const char *name);
bool pt_file_write_platform_ready(u32 handle, u32 size);

u32 GuestToLower(u32 value);
u32 *GuestDOSErrnoAddress(void);
u32 *GuestSetDOSErrno(u32 value);
u32 GuestErrnoFromDOS(u32 value);
u32 GuestIOMode(u32 handle);
PTFileModeSlot *GuestSetIOMode(u32 handle, u32 mode);
GuestFileStream *GuestAllocFileStream(void);
void GuestFreeFileStream(GuestFileStream *stream);
u32 GuestOpenFlags(const u8 *mode);
u32 GuestSOpen(const u8 *filename, u32 flags, u32 sharing, u32 permissions);
GuestFileStream *GuestDoOpen(const u8 *filename, const u8 *mode, u32 sharing, GuestFileStream *stream);
GuestFileStream *GuestFSOpen(const u8 *filename, const u8 *mode, u32 sharing);
GuestFileStream *GuestFopen(const u8 *filename, const u8 *mode);
u32 GuestLSeek(u32 handle, u32 offset, u32 whence);
u32 GuestTell(u32 handle);
u32 GuestFtell(GuestFileStream *stream);
u32 GuestQuickClose(u32 handle);
u32 GuestProcessID(void);
u32 GuestHexDigit(u32 digit);
u32 GuestMakeTempFilename(u8 *destination, u32 number);
u32 GuestDOSUnlink(const u8 *filename);
u32 GuestUnlink(const u8 *filename);
u32 GuestDoClose(GuestFileStream *stream, u32 close_handle);
u32 GuestShutdownStream(GuestFileStream *stream, u32 close_handle);
u32 GuestFclose(GuestFileStream *stream);
GuestFileStream *OpenFile(const u8 *filename, u32 flags);
u32 CloseFile(GuestFileStream *stream);
u32 OpenFileAsCurrent(const u8 *filename);
void CloseCurrentFile(void);

// File browser.

// Native selector state has no DOS stack offsets or retained padding.

enum { PT_FILE_SELECTOR_ROWS = 167, PT_FILE_SELECTOR_NAME_BYTES = 14 };
typedef struct PTFileSelector {
  u8 filenames[PT_FILE_SELECTOR_ROWS][PT_FILE_SELECTOR_NAME_BYTES];
  u8 saved_cwd[200];
  GuestFindRecord find_record;
  u8 title[24];
  u8 patterns[3][8];
  u32 file_count;
  u8 visible_rows, scroll_offset;
} PTFileSelector;

// directory runtime

// Single C: drive, bounded ASCII DOS directory service profile. Platform
// import/export is diagnostic only; commands mutate an independent copy.

typedef struct PTDirectoryState {
  char cwd[64];
  u32 getcwd_error;
} PTDirectoryState;
const PTDirectoryState *pt_directory_state(void);
bool pt_directory_restore(const PTDirectoryState *state);
bool pt_directory_plan(const char *name, char result[64], u32 *error);
// Read-only queries against a caller-owned future directory profile. They
// consult current directory/assets but never import or commit that profile.

bool pt_file_resolved_name_at(const PTDirectoryState *directory, const char *name, char result[1024]);
bool pt_directory_plan_at(const PTDirectoryState *directory, const char *name, char result[64], u32 *error);
void pt_directory_commit(const char *name);
bool pt_file_resolved_name(const char *name, char result[1024]);
u8 *GuestStrncpy(u8 *destination, const u8 *source, u32 count);
u8 *GuestGetcwd(u8 *destination, u32 capacity);
u32 GuestChdir(const u8 *filename);

// file search

// Bounded local C: volume, ordered asset slots, explicit DOS service faults
// and retained kernel network bytes. Each search record owns its continuation.

typedef struct PTDosSearchState {
  GuestFindRecord *dta;
  GuestFindRecord *fixed_dta;
  u8 network_reserved[4];
  u32 first_error;
  u32 next_error;
} PTDosSearchState;
typedef struct PTDosFindPlan {
  GuestFindRecord record;
  u8 network_reserved[4];
  u32 error;
  bool write;
} PTDosFindPlan;
_Static_assert(sizeof(PTDosFindPlan) == 56, "DOS find plan host ABI");
_Static_assert(offsetof(PTDosFindPlan, record) == 0, "DOS find plan record ABI");
_Static_assert(offsetof(PTDosFindPlan, network_reserved) == 43, "DOS find plan network ABI");
_Static_assert(offsetof(PTDosFindPlan, error) == 48, "DOS find plan error ABI");
_Static_assert(offsetof(PTDosFindPlan, write) == 52, "DOS find plan write ABI");
bool pt_dos_search_load_state(const PTDosSearchState *initial);
bool pt_dos_search_export_state(PTDosSearchState *result);
bool pt_file_asset_load_times(const char *name, u16 time, u16 date);
void pt_dos_set_find_record(GuestFindRecord *record);
GuestFindRecord *pt_dos_find_record(void);
GuestFindRecord *pt_default_find_record(void);
void pt_default_find_record_bind(GuestFindRecord *record);
// Pure query over current assets and a caller-owned continuation. The plan
// retains all untouched result bytes and is committed only by execute.

bool pt_dos_find_plan(
    bool first, const char *filename, u32 attributes, const GuestFindRecord *incoming, PTDosFindPlan *plan);
bool pt_dos_find_plan_in_state(const PTDirectoryState *directory, const PTDosSearchState *search, bool first,
    const char *filename, u32 attributes, const GuestFindRecord *incoming, PTDosFindPlan *plan);
u32 pt_dos_find_execute(bool first, const char *filename, u32 attributes);
u32 GuestDOSError(u32 value, bool carry);
u32 GuestDOSFindNextError(u32 value, bool carry);
void GuestCopyDTAOut(GuestFindRecord *buffer);
void GuestCopyDTAIn(GuestFindRecord *buffer);
u32 GuestDOSFindFirst(const u8 *filename, u32 attributes, GuestFindRecord *buffer);
u32 GuestDOSFindNext(GuestFindRecord *buffer);

// file menu ui

void DrawIntroCaption(const u8 *text);
void DrawFileSelectorCaption(const u8 *text);
void ClearMissionGoalRect(void);
void DrawMainMenuListboxes(u8 right);
u8 CheckMouseInsideListbox(void);
i32 GetSelectedListItem(i16 top, i16 spacing);

// file selector ui

// Original buffers overlap the historic ten-byte IDA filename declaration.
// A DOS 8.3 name plus the five-byte game\\ prefix occupies at most 18 bytes.

enum { PT_SELECTOR_FILENAME_CAPACITY = 18 };
u8 GetSaveOrLoadFilename(u8 mode);
u8 *pt_selector_extension_end(u32 length);

// File write.

u32 GuestQuickWrite(u32 handle, const void *source, u32 size);
u32 GuestWrite(u32 handle, const void *source, u32 size);
u32 GuestFlush(GuestFileStream *stream);
u32 GuestFputc(u32 value, GuestFileStream *stream);
u32 GuestFwrite(const void *source, u32 size, u32 count, GuestFileStream *stream);
u32 GuestUpdateBuffer(u32 offset, GuestFileStream *stream);
u32 GuestFseek(GuestFileStream *stream, u32 offset, u32 whence);
u32 WriteFile(GuestFileStream *stream, const void *source, u32 size);
u32 OpenFileForWritingAsCurrent(const u8 *filename);
u32 WriteBytesToCurrentFile(const void *source, u32 size);
u32 CloseCurrentWritingFile(void);
u32 WriteHighscoresFile(const void *table);

// Asset loader.

Game *LoadBackground(const u8 *filename, Bitmap *bitmap);
Game *LoadGameBackground(u8 index);

#endif
