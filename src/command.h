// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
#ifndef PT_COMMAND_H
#define PT_COMMAND_H
#include "types.h"
#include <stddef.h>
#include <stdio.h>

// Platform commands are independent of original executable addresses.

typedef enum PTCommandKind {
  PT_CMD_TICK = 0x10000001,
  PT_CMD_FRAME = 0x10000002,
  PT_CMD_FRAME_POINTER = 0x10000003,
  PT_CMD_FRAME_KEY = 0x10000004,
  PT_CMD_FRAME_KEYBOARD_PORT = 0x10000005,
  PT_CMD_FRAME_JOYSTICK = 0x10000008,
  PT_CMD_PCM_SAMPLES = 0x10000006,
  PT_CMD_SOUND_TIMER = 0x10000007,
} PTCommandKind;

typedef struct PTPointerSample {
  i32 x, y;
  u32 buttons;
} PTPointerSample;
typedef struct PTCommand {
  u32 kind;
  u32 args[8];
  u32 pointer_count;
  PTPointerSample pointer_path[16];
  u32 modifiers;
  u32 key;
  u64 platform_clock;
  u64 frame;
} PTCommand;
_Static_assert(sizeof(PTCommand) == 256, "command ABI");
_Static_assert(offsetof(PTCommand, pointer_path) == 40, "command pointer path ABI");
_Static_assert(offsetof(PTCommand, platform_clock) == 240, "command clock ABI");

typedef enum PTCommandStatus {
  PT_COMMAND_OK = 0,
  PT_COMMAND_UNSUPPORTED = 1,
  PT_COMMAND_INVALID_ARGUMENT = 2,
  PT_COMMAND_RECORD_FAILED = 3,
} PTCommandStatus;
typedef struct PTCommandResult {
  u32 status, value;
} PTCommandResult;

// Record the supplied inputs before any state mutation.

void pt_command_set_record_file(FILE *file);
PTCommandResult pt_command_apply(const PTCommand *command);
#endif
