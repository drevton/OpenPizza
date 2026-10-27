// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
// Command.

#include "command.h"
#include "frame.h"
#include "input.h"
#include "calendar.h"
#include "sound.h"
#include <inttypes.h>

static FILE *record_file;
static PTCommandStatus validate_audio(const PTCommand *command) {
  if (command->key || command->modifiers || command->pointer_count || command->platform_clock > UINT32_MAX)
    return PT_COMMAND_INVALID_ARGUMENT;
  bool timer = command->kind == PT_CMD_SOUND_TIMER;
  if (timer && command->platform_clock != pt_platform_clock()->ticks)
    return PT_COMMAND_INVALID_ARGUMENT;
  for (unsigned i = timer ? 0 : 1; i < 8; ++i)
    if (command->args[i])
      return PT_COMMAND_INVALID_ARGUMENT;
  if (pt_frame_active() && !pt_frame_suspended())
    return PT_COMMAND_UNSUPPORTED;
  return timer ? pt_sound_timer_ready() : pt_pcm_samples_ready(command->args[0]);
}
static bool frame_command(u32 kind) {
  switch (kind) {
  case PT_CMD_FRAME:
  case PT_CMD_FRAME_POINTER:
  case PT_CMD_FRAME_KEY:
  case PT_CMD_FRAME_KEYBOARD_PORT:
  case PT_CMD_FRAME_JOYSTICK:
    return true;
  default:
    return false;
  }
}
static PTCommandStatus validate_frame(const PTCommand *command) {
  if (command->platform_clock > UINT32_MAX || command->key > UINT16_MAX)
    return PT_COMMAND_INVALID_ARGUMENT;
  if (!pt_frame_ready() || (command->key && !pt_input_can_accept_key()) || !pt_input_queue_valid())
    return PT_COMMAND_UNSUPPORTED;
  bool current_clock = command->platform_clock == pt_platform_clock()->ticks;
  switch (command->kind) {
  case PT_CMD_FRAME_JOYSTICK: {
    PTJoystickPlatform state = {
      command->args[0], command->args[1], command->args[2], command->args[3], command->args[4], command->args[5]};
    if (command->key || command->pointer_count || command->args[6] > 1 || command->args[7] ||
        (command->args[6] && !state.present) || !current_clock || !pt_joystick_valid(&state))
      return PT_COMMAND_INVALID_ARGUMENT;
    break;
  }
  case PT_CMD_FRAME_KEYBOARD_PORT:
    if (command->args[0] > UINT8_MAX || command->key || command->pointer_count || !current_clock)
      return PT_COMMAND_INVALID_ARGUMENT;
    for (unsigned i = 1; i < 8; ++i)
      if (command->args[i])
        return PT_COMMAND_INVALID_ARGUMENT;
    break;
  case PT_CMD_FRAME_KEY:
    if (!command->key || command->pointer_count || !current_clock)
      return PT_COMMAND_INVALID_ARGUMENT;
    break;
  case PT_CMD_FRAME_POINTER: {
    if (command->pointer_count != 1 || command->key || !current_clock)
      return PT_COMMAND_INVALID_ARGUMENT;
    const PTPointerSample *point = &command->pointer_path[0];
    if (point->x < 0 || point->x > UINT16_MAX || point->y < 0 || point->y > UINT16_MAX || point->buttons > 3)
      return PT_COMMAND_INVALID_ARGUMENT;
    if (!pt_mouse_ready_for_events())
      return PT_COMMAND_UNSUPPORTED;
    break;
  }
  case PT_CMD_FRAME:
    break;
  default:
    return PT_COMMAND_UNSUPPORTED;
  }
  return PT_COMMAND_OK;
}
void pt_command_set_record_file(FILE *file) {
  record_file = file;
}

static bool record(const PTCommand *c) {
  if (!record_file)
    return true;
  if (fprintf(record_file, "{\"kind\":%" PRIu32 ",\"args\":[", c->kind) < 0)
    return false;
  for (unsigned i = 0; i < 8; ++i)
    if (fprintf(record_file, "%s%" PRIu32, i ? "," : "", c->args[i]) < 0)
      return false;
  if (fputs("],\"pointer_path\":[", record_file) == EOF)
    return false;
  for (unsigned i = 0; i < c->pointer_count; ++i) {
    const PTPointerSample *p = &c->pointer_path[i];
    if (fprintf(record_file,
            "%s{\"x\":%" PRId32 ",\"y\":%" PRId32 ",\"buttons\":%" PRIu32 "}",
            i ? "," : "",
            p->x,
            p->y,
            p->buttons) < 0)
      return false;
  }
  if (fprintf(record_file,
          "],\"modifiers\":%" PRIu32 ",\"key\":%" PRIu32 ",\"platform_clock\":%" PRIu64 ",\"frame\":%" PRIu64 "}\n",
          c->modifiers,
          c->key,
          c->platform_clock,
          c->frame) < 0)
    return false;
  return fflush(record_file) == 0;
}

PTCommandResult pt_command_apply(const PTCommand *c) {
  if (!c)
    return (PTCommandResult){PT_COMMAND_INVALID_ARGUMENT, 0};
  if (c->pointer_count > sizeof(c->pointer_path) / sizeof(c->pointer_path[0]))
    return (PTCommandResult){PT_COMMAND_INVALID_ARGUMENT, 0};
  bool frame = frame_command(c->kind);
  bool audio = c->kind == PT_CMD_PCM_SAMPLES || c->kind == PT_CMD_SOUND_TIMER;
  PTCommandStatus status = frame ? validate_frame(c) : audio ? validate_audio(c) : PT_COMMAND_UNSUPPORTED;
  if (status != PT_COMMAND_OK)
    return (PTCommandResult){status, 0};
  if (!record(c))
    return (PTCommandResult){PT_COMMAND_RECORD_FAILED, 0};
  if (frame)
    return pt_frame_step(c);
  if (audio)
    return (PTCommandResult){
      PT_COMMAND_OK, c->kind == PT_CMD_PCM_SAMPLES ? pt_pcm_driver_samples(c->args[0]) : pt_sound_timer_tick()};
  return (PTCommandResult){PT_COMMAND_UNSUPPORTED, 0};
}
