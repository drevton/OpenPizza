// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
#ifndef PT_DISPLAY_H
#define PT_DISPLAY_H
#include "types.h"
#include <stddef.h>
enum {
  PT_DISPLAY_QUIT = 1,
  PT_DISPLAY_POINTER = 2,
  PT_DISPLAY_KEY = 3,
  PT_DISPLAY_UNSUPPORTED_KEY = 4,
  PT_DISPLAY_KEYBOARD_PORT = 5,
  PT_DISPLAY_JOYSTICK = 6
};
typedef struct PTDisplayEvent {
  u32 kind;
  i32 x, y;
  // Pointer buttons and physical keyboard scan occupy the same event slot.

  union {
    u32 buttons;
    u32 keyboard_port;
  };
  u32 key, modifiers;
  u64 milliseconds;
} PTDisplayEvent;
int pt_display_open(const char *title, unsigned scale, int hidden);
int pt_display_audio_write(const u8 *samples, size_t size);
int pt_display_present(const u8 *rgb, size_t size, u8 *readback);
int pt_display_poll(PTDisplayEvent *event);
u64 pt_display_milliseconds(void);
void pt_display_delay(unsigned milliseconds);
const char *pt_display_error(void);
void pt_display_close(void);
#endif
