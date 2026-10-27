// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
#ifndef PT_CALENDAR_H
#define PT_CALENDAR_H
#include "layout.h"
#include "types.h"
#include "render_types.h"

// Calendar.

#include <stdbool.h>

u8 *SprintfTimeLeft(u32 seconds);
const u8 *GetMonthName(u8 index);
const u8 *GetStringThExpression(u8 day);
void PrintCurrentDateAndTime(void);
size_t FormatCurrentDateAndTime(u8 text[40], bool write_time_buffer, const Apple *apple);

// Timer.

void WaitForMouseClick_Helper(void);

unsigned pt_delayed_message_format(u8 text[256], const Banana *banana, size_t *length);

// Delay.

#include <stdbool.h>

// DOS clock adapter state is presentation/platform state, outside guest RAM.
// Import only from a common immutable fixture before either side executes.

typedef struct PTDOSClockState {
  u64 samples;
  u32 cx, dx;
  u64 rtc_samples, set_calls;
  u32 rtc_cx, rtc_dx, set_centiseconds, set_ticks;
} PTDOSClockState;
_Static_assert(sizeof(PTDOSClockState) == 48, "DOS time adapter ABI");
void pt_delay_reset_platform(void);
bool pt_delay_load_platform(const PTDOSClockState *state);
bool pt_delay_export_platform(PTDOSClockState *state);
void Delay(u32 units);
void pt_dos_sample_time(void);
void pt_dos_read_rtc(u32 *cx, u32 *dx);
void pt_dos_set_time(u32 cx, u32 dx);
void WatcomDelayThunk(u32 milliseconds);
void WatcomDelay(u32 milliseconds);
void WatcomDelayInit(void);

// Clock.

u8 GetArstidIndex(void);
const u8 *GetNameOfArstid(void);
PTRGB *DoSomeClockPaletteStuff(void);
void AdvanceClock(u8 amount);

// Platform clock.

// Platform clocks are real objects; imported images bind their own storage.

typedef struct __attribute__((packed, may_alias)) PTPlatformClockView {
  u32 ticks;
} PTPlatformClockView;
_Static_assert(sizeof(PTPlatformClockView) == 4, "platform timer field layout");
PTPlatformClockView *pt_platform_clock(void);
void pt_platform_clock_bind(PTPlatformClockView *clock);

#endif
