// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
#ifndef PT_FRAME_H
#define PT_FRAME_H
#include "types.h"
#include "layout.h"
#include "command.h"

// Game start.

#include <stdbool.h>
typedef struct __attribute__((packed)) PTStartupSoundConfig {
  u8 unknown_0[8];
  u16 mode;
  u8 unknown_a[8];
} PTStartupSoundConfig;
_Static_assert(sizeof(PTStartupSoundConfig) == 18, "startup sound.cfg record extent");
_Static_assert(offsetof(PTStartupSoundConfig, mode) == 8, "startup sound.cfg mode offset");
void ReadJoystick(void);
void GameMain(i32 argc, const char *const *argv, const char *const *envp);
// Start GameMain with owned C argument strings. Reset joins the worker and

bool pt_game_start(i32 argc, const char *const *argv);

// Frame.

#include <stdbool.h>

// One host owner drives a suspended C worker. Boundary and stop values are
// opaque tags: the scheduler never interprets executable addresses.

typedef struct PTFrameState {
  u32 active, boundary, status, stopped_at;
  u64 serial;
} PTFrameState;
_Static_assert(sizeof(PTFrameState) == 24, "frame state ABI");
void pt_frame_reset(void);
bool pt_frame_active(void);
bool pt_frame_export(PTFrameState *state);
bool pt_frame_worker(void);
void pt_frame_wait_retrace_boundary(void);
void pt_frame_display_retrace_boundary(void);
typedef enum PTFramePoll {
  PT_FRAME_POLL_CLOCK,
  PT_FRAME_POLL_KEYBOARD,
  PT_FRAME_POLL_STOCK_LIMIT,
  PT_FRAME_POLL_SOUND_ENTER,
  PT_FRAME_POLL_SOUND_LEAVE,
  PT_FRAME_POLL_DELAY_WAIT,
  PT_FRAME_POLL_DELAY_FIRST,
  PT_FRAME_POLL_DELAY_CHANGE,
  PT_FRAME_POLL_DELAY_COUNT
} PTFramePoll;
void pt_frame_poll(PTFramePoll poll);
void pt_frame_boundary(u32 tag);
_Noreturn void pt_frame_unsupported(u32 tag);
_Noreturn void pt_frame_process_exit(u32 tag);
PTCommandResult pt_frame_step(const PTCommand *command);
// Partial handler: unported event branches stop explicitly. Only
// its proven bootstrap continuation is exposed through frame commands.

void RunGame(void);
u8 HandleMapZoomLevel1(void);
u8 HandleMapZoomLevel2(void);
u8 HandleMapZoomLevel3(void);
u16 IsMouseOverMap(void);
Zone *FindZoneFromCoordinates(i16 x, i16 y);

// Game loop.

typedef enum PTMapUnsupported {
  PT_MAP_MIDDLE_ROW,
  PT_MAP_MIDDLE_COLUMN,
  PT_MAP_DETAILED_ROW,
  PT_MAP_DETAILED_COLUMN
} PTMapUnsupported;
_Noreturn void pt_map_unsupported(PTMapUnsupported operation);
u8 map_loop(void);
void map_after_draw(void);
void run_game_map_loop(u8 mode);

// Frame scheduler.

// A task is a C callback with caller-owned context, retained until reset.

bool pt_frame_start(void *(*entry)(void *), void *context);
bool pt_frame_ready(void);
bool pt_frame_suspended(void);
typedef enum PTFrameBoundary {
  PT_FRAME_RETRACE,
  PT_FRAME_DISPLAY,
  PT_FRAME_CLOCK,
  PT_FRAME_KEYBOARD,
  PT_FRAME_STOCK_LIMIT,
  PT_FRAME_SOUND_ENTER,
  PT_FRAME_SOUND_LEAVE,
  PT_FRAME_STARTUP,
  PT_FRAME_DELAY_WAIT,
  PT_FRAME_DELAY_FIRST,
  PT_FRAME_DELAY_CHANGE,
  PT_FRAME_DELAY_COUNT,
  PT_FRAME_PROCESS_EXIT,
  PT_FRAME_INTERRUPT_UNAVAILABLE,
  PT_FRAME_DOS_MEMORY_UNAVAILABLE,
  PT_FRAME_MIDI_UNAVAILABLE,
  PT_FRAME_MAP_MIDDLE_ROW_UNAVAILABLE,
  PT_FRAME_MAP_MIDDLE_COLUMN_UNAVAILABLE,
  PT_FRAME_MAP_DETAILED_ROW_UNAVAILABLE,
  PT_FRAME_MAP_DETAILED_COLUMN_UNAVAILABLE,
  PT_FRAME_TIME_MONTH_INVALID,
  PT_FRAME_TIME_CLOCK_UNAVAILABLE,
  PT_FRAME_TIME_RULE_YEAR_INVALID
} PTFrameBoundary;
// Caller-owned task resources are released only after the worker has joined.
// Cleanup may be changed only while the scheduler is idle.

bool pt_frame_set_cleanup(void (*cleanup)(bool joined, void *context), void *context);
void pt_frame_require_boundary(unsigned status, PTFrameBoundary reason);
// A caller may attach its own opaque diagnostic tag to a terminal failure.

_Noreturn void pt_frame_fail(unsigned status, u32 tag);

#endif
