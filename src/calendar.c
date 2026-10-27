// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
// Calendar.

#include "objects.h"
#include "calendar.h"
#include "text.h"

// These formats contain only positive decimal integers and byte strings.
// The unsigned division limits minutes to 71582788, so the original
// twelve-byte result allocation includes its longest string's terminator.

static u8 *decimal(u8 *output, u32 value) {
  u8 reverse[10];
  unsigned count = 0;
  do {
    reverse[count++] = (u8)('0' + value % 10u);
    value /= 10u;
  } while (value);
  while (count)
    *output++ = reverse[--count];
  return output;
}
static size_t time_text(u8 text[12], u32 seconds) {
  u8 *end = decimal(text, seconds / 60u);
  *end++ = ':';
  if (seconds % 60u < 10)
    *end++ = '0';
  end = decimal(end, seconds % 60u);
  *end = 0;
  return (size_t)(end - text);
}
u8 *SprintfTimeLeft(u32 seconds) {
  u8 text[12];
  size_t length = time_text(text, seconds);
  memcpy(time_left_text, text, length + 1);
  return time_left_text;
}
const u8 *GetMonthName(u8 index) {
  return pt_month_name_identity(index);
}
// The original ordinal helper truncates the argument to a BYTE. For zero, DEC AL
// wraps to 255 and returns the corresponding address without dereferencing it.

const u8 *GetStringThExpression(u8 day) {
  u8 index = 3;
  if (day < 4)
    index = (u8)(day - 1u);
  if (day > 20 && day < 24)
    index = (u8)(day - 21u);
  if (day == 31)
    index = 0;
  return pt_ordinal_suffix_identity(index);
}
static u8 *copy_text(u8 *output, const u8 *text) {
  while (*text)
    *output++ = *text++;
  return output;
}
static u32 seconds_remaining(void) {
  // Defined wrapping ADD/SHL arithmetic, even when the stored words have
  // their sign bit set. SprintfTimeLeft then performs unsigned division.

  return (u32)game_ptr->seconds_left + 60u * (u32)game_ptr->minutes_left;
}
size_t FormatCurrentDateAndTime(u8 text[40], bool write_time_buffer, const Apple *apple) {
  u8 *end;
  if (game_ptr->show_time_left_instead_of_date) {
    u8 time[12];
    const u8 *formatted;
    if (write_time_buffer)
      formatted = SprintfTimeLeft(seconds_remaining());
    else {
      time_text(time, seconds_remaining());
      formatted = time;
    }
    end = copy_text(text, (const u8 *)"Time: ");
    end = copy_text(end, formatted);
  } else {
    end = decimal(text, apple->day);
    *end++ = '-';
    end = copy_text(end, GetMonthName((u8)(apple->month - 1u)));
    *end++ = '-';
    end = decimal(end, (u16)apple->year);
    *end++ = ' ';
    end = decimal(end, (u8)apple->minutesx10 / 6u);
    *end++ = ':';
    end = decimal(end, (u8)apple->minutesx10 % 6u);
    *end++ = '0';
  }
  *end = 0;
  return (size_t)(end - text);
}
void PrintCurrentDateAndTime(void) {
  canvas_1->color = 0;
  canvas_1->background_color = 0;
  Canvas_FillRect(canvas_1, 230, 12, 306, 19);
  Canvas_SetFont(canvas_1, font_supersma);
  canvas_1->position_x = 234;
  canvas_1->position_y = 19;
  u8 formatted[40];
  size_t length =
      FormatCurrentDateAndTime(formatted, true, game_ptr->show_time_left_instead_of_date ? NULL : ptr_apple);
  canvas_1->color = 16;
  Canvas_DrawText(canvas_1, pt_text_span(formatted, (u16)length));
  if (game_ptr->byte86) {
    canvas_1->color = 0;
    canvas_1->bitmap = display_bitmaps[game_ptr->current_bitmap_index ^ 1u];
    Canvas_FillRect(canvas_1, 230, 12, 306, 19);
    canvas_1->position_x = 234;
    canvas_1->position_y = 19;
    canvas_1->color = 16;
    Canvas_DrawText(canvas_1, pt_text_span(formatted, (u16)length));
    canvas_1->bitmap = display_bitmaps[game_ptr->current_bitmap_index];
  }
}

// Clock.

#include "messages.h"
#include "restaurant.h"

u8 GetArstidIndex(void) {
  return month_to_arstid[ptr_apple->month];
}
const u8 *GetNameOfArstid(void) {
  return pt_season_name_identity(GetArstidIndex());
}
PTRGB *DoSomeClockPaletteStuff(void) {
  u8 season = GetArstidIndex();
  u8 palette = season ? (u8)(season + 66u) : 0;
  memcpy(&palette_e[62], &palette_e[palette], sizeof(palette_e[62]));
  // The first copy can replace the room's month through an aliased palette.
  // Preserve the original second lookup after that copy.

  season = GetArstidIndex();
  return memcpy(&palette_e[62].colors[28], season_map_colors[season], sizeof(season_map_colors[season]));
}
static void restaurant_clock_refresh(void) {
  if (ptr_apple->view_state >= 128 && !ptr_apple->is_in_kitchen) {
    Rest *restaurant = curr_rest;
    if (restaurant->restaurant_opened) {
      DrawRestaurant(restaurant, 1);
    }
    did_terminal_stuff = 1;
  }
}
void AdvanceClock(u8 amount) {
  ptr_apple->clock_flag = 0;
  while (amount) {
    const Character *character = character_ptr;
    if (ptr_apple->minutesx10 < character->now)
      ptr_apple->minutesx10 = (u8)((u32)ptr_apple->minutesx10 + (u32)1u);
    if (ptr_apple->minutesx10 == 54) {
      DoSomeClockPaletteStuff();
      ShowMessage(16, restaurant_opening_text);
      restaurant_clock_refresh();
    }
    if (ptr_apple->minutesx10 == 126) {
      DoSomeClockPaletteStuff();
      ShowMessage(16, restaurant_closing_text);
      restaurant_with_guests = NULL;
      guest_count = 0;
      restaurant_clock_refresh();
    }
    --amount;
  }
  PrintCurrentDateAndTime();
}

// Timer.

#include "game.h"
#include <stdio.h>
#include "frame.h"

unsigned pt_delayed_message_format(u8 text[256], const Banana *banana, size_t *length) {
  if (banana->message_slot >= 16)
    return 1;
  const u8 *message = delayed_msg_contents[banana->message_slot];
  if (!memchr(message, 0, sizeof(delayed_msg_contents[0])))
    return 1;
  int count = snprintf(
      (char *)text, 256, (const char *)delayed_message_format, (u8)banana->restaurant + 1, (const char *)message);
  if (count < 0 || count >= 256)
    return 1;
  *length = (size_t)count;
  return 0;
}
void WaitForMouseClick_Helper(void) {
  if (!game_ptr->byte106 || !game_ptr->byte87)
    return;
  u32 seconds = (u32)game_ptr->seconds_left + 1u;
  game_ptr->seconds_left = pt_signed_dword(seconds);
  if (seconds >= 60u) {
    game_ptr->seconds_left = 0;
    game_ptr->minutes_left = pt_signed_dword((u32)game_ptr->minutes_left + 1u);
  }
  if (game_ptr->show_time_left_instead_of_date)
    PrintCurrentDateAndTime();
  if (ptr_apple->message_ticks_left &&
      !(ptr_apple->message_ticks_left = (u8)((u32)(ptr_apple->message_ticks_left) - (u32)1u)))
    PrintGenericInfoBox();
  ptr_apple->clock_flag = (u8)((u32)ptr_apple->clock_flag + (u32)1u);
  if (game_ptr->flagB9 && ptr_apple->clock_flag > 10) {
    AdvanceClock(1);
  }
  for (unsigned i = 0; i < 16; ++i) {
    Banana *banana = &bananas[i];
    if (banana->is_free || banana->byte8 || banana->player_index != current_player_index ||
        banana->when > ptr_apple->minutesx10 || !RandomChance(banana->byteB))
      continue;
    // The chance test consumes RNG even when another message is active.
    // A successful chance then returns from the entire callback.

    if (ptr_apple->message_ticks_left)
      return;
    u8 *text = pt_delayed_message_text(banana->message_slot);
    if (banana->restaurant < 100) {
      u8 formatted[256];
      size_t length;
      if (pt_delayed_message_format(formatted, banana, &length))
        abort();
      memcpy(strbuf, formatted, length + 1);
      text = strbuf;
    }
    ShowMessage(14, text);
    MarkBananaFree(banana);
    MarkMessageSlotFree(banana->message_slot);
  }
}

// Delay.

#include <stdlib.h>
#include "globals.h"
#include "files.h"

static PTDOSClockState dos_clock;
void pt_delay_reset_platform(void) {
  dos_clock = (PTDOSClockState){0};
}
bool pt_delay_load_platform(const PTDOSClockState *state) {
  if (!state || pt_frame_active() || state->cx > UINT16_MAX || state->dx > UINT16_MAX || (state->cx >> 8) > 23 ||
      (state->cx & 255u) > 59 || (state->dx >> 8) > 59 || (state->dx & 255u) > 99 ||
      (!state->samples && (state->cx || state->dx)) || state->rtc_cx > 65535 || state->rtc_dx > 65535 ||
      state->set_centiseconds >= 8640000u || (!state->set_calls && (state->set_centiseconds || state->set_ticks)))
    return false;
  dos_clock = *state;
  return true;
}
bool pt_delay_export_platform(PTDOSClockState *state) {
  if (!state)
    return false;
  *state = dos_clock;
  return true;
}
void pt_dos_sample_time(void) {
  // Stop before the actual INT, without inventing a call to ReadPlatformTicks.
  // A recorded command may supply a new BIOS clock or input while suspended.

  u64 cs = dos_clock.set_calls
      ? (dos_clock.set_centiseconds + (u64)(pt_platform_clock()->ticks - dos_clock.set_ticks) * 100u / 18u) % 8640000u
      : (u64)pt_platform_clock()->ticks * 100u / 18u;
  ++dos_clock.samples;
  dos_clock.cx = ((dos_clock.set_calls ? (u32)(cs / 360000u) : 9u) << 8) | (u32)((cs / 6000u) % 60u);
  dos_clock.dx = ((u32)((cs / 100u) % 60u) << 8) | (u32)(cs % 100u);
}
static u8 bcd(u32 value) {
  return (u8)((value / 10u) * 16u + value % 10u);
}
void pt_dos_read_rtc(u32 *cx, u32 *dx) {
  u64 seconds = pt_platform_clock()->ticks / 18u;
  *cx = (9u << 8) | bcd((u32)(seconds / 60u) % 60u);
  *dx = (u32)bcd((u32)(seconds % 60u)) << 8;
  ++dos_clock.rtc_samples;
  dos_clock.rtc_cx = *cx;
  dos_clock.rtc_dx = *dx;
}
void pt_dos_set_time(u32 cx, u32 dx) {
  u32 hours = (cx >> 8) & 255u, minutes = cx & 255u, seconds = (dx >> 8) & 255u, hundredths = dx & 255u;
  if (hours > 23 || minutes > 59 || seconds > 59 || hundredths > 99)
    abort();
  ++dos_clock.set_calls;
  dos_clock.set_centiseconds = hours * 360000u + minutes * 6000u + seconds * 100u + hundredths;
  dos_clock.set_ticks = pt_platform_clock()->ticks;
}
static u8 dos_seconds(PTFramePoll poll) {
  pt_frame_poll(poll);
  pt_dos_sample_time();
  return (u8)(dos_clock.dx >> 8);
}
static u32 milliseconds(u32 units) {
  return units == 1 ? 30u : units == 2 ? 50u : units == 3 ? 70u : units * 20u;
}
void Delay(u32 units) {
  // argument + CALL to JMP thunk
  ;
  WatcomDelayThunk(milliseconds(units));
}
void WatcomDelayThunk(u32 ms) {
  WatcomDelay(ms);
  // JMP: no extra guest frame
}
void WatcomDelay(u32 ms) {
  ;
  if (ms) {
    // IMUL/ADD preserve only their low dword. DIV is unsigned.

    u32 count = (ms * delay_polls_per_second + 500u) / 1000u;
    if (!count)
      count = 1;
    do {
      (void)dos_seconds(PT_FRAME_POLL_DELAY_WAIT);
    } while (--count);
  }
}
void WatcomDelayInit(void) {
  u8 first = dos_seconds(PT_FRAME_POLL_DELAY_FIRST), second;
  do {
    second = dos_seconds(PT_FRAME_POLL_DELAY_CHANGE);
  } while (second == first);
  u32 count = 0;
  u8 sample;
  do {
    sample = dos_seconds(PT_FRAME_POLL_DELAY_COUNT);
    ++count;
  } while (sample == second);
  delay_polls_per_second = count;
}

// Guest time.

#include "graphics.h"
static void checked(bool ok, PTFrameBoundary reason) {
  if (!ok) {
    pt_frame_require_boundary(PT_COMMAND_UNSUPPORTED, reason);
    abort();
  }
}
static i32 add(i32 a, i32 b) {
  return pt_signed_dword((u32)a + (u32)b);
}
static i32 sub(i32 a, i32 b) {
  return pt_signed_dword((u32)a - (u32)b);
}
static i32 mul(i32 a, i32 b) {
  return pt_signed_dword((u32)a * (u32)b);
}
u32 GuestLeapYear(i32 y) {
  u32 r = !(y & 3) && (y % 100 != 0 || y % 400 == 0);

  return r;
}
static const i16 *month_table(i32 y) {
  u32 leap = GuestLeapYear(add(y, 1900));
  return leap ? time_leap_month_days : time_common_month_days;
}
static i32 days(const i16 *table, i32 index) {
  checked(index >= 0 && index <= 12, PT_FRAME_TIME_MONTH_INVALID);
  return table[index];
}
static i32 month_length(i32 y, i32 m) {
  const i16 *t = month_table(y);
  return sub(days(t, add(m, 1)), days(t, m));
}
// Original loop keeps the first computed year length as its divisor.

GuestTM *GuestBreakTime(u32 day, u32 seconds, i32 offset, GuestTM *output) {
  GuestTM *t = output;
  u32 s = seconds - (u32)offset, d = day;
  if (seconds < 43200 && offset > 0) {
    s += 86400;
    --d;
  }
  d += s / 86400;
  s %= 86400;
  t->hour = (i32)(s / 3600);
  s %= 3600;
  t->minute = (i32)(s / 60);
  t->second = (i32)(s % 60);
  u32 year = d / 366, yd = d - 365u * year;
  if (year)
    yd -= (year - 1) / 4;
  u32 leap = GuestLeapYear(pt_signed_dword(year + 1900));
  u32 length = 365 + leap;
  while (yd >= length) {
    ++year;
    yd -= length;
  }
  t->year = pt_signed_dword(year);
  t->yearday = pt_signed_dword(yd);
  const i16 *table = month_table(t->year);

  u32 month = yd / 31;
  if (yd >= (u32)days(table, (i32)month + 1))
    ++month;
  t->month = (i32)month;
  t->day = pt_signed_dword(yd - (u32)days(table, t->month) + 1);
  t->weekday = (i32)((d + 1) % 7);

  return output;
}
u32 GuestGetCurrentTime(GuestTM *output) {
  GuestTM *t = output;
  // Shared DOS profile: Thursday, 1 December 1994.

  t->year = 94;
  t->month = 11;
  t->day = 1;
  pt_dos_sample_time();
  PTDOSClockState c;
  checked(pt_delay_export_platform(&c), PT_FRAME_TIME_CLOCK_UNAVAILABLE);

  t->hour = (i32)(c.cx >> 8);
  t->minute = (i32)(c.cx & 255);
  t->second = (i32)(c.dx >> 8);

  t->isdst = -1;

  return 10u * (c.dx & 255);
}
i32 GuestCaseCompare(const u8 *a, const u8 *b, u32 count) {
  i32 result = 0;
  while (count--) {
    u8 x = *a, y = *b;
    if (x >= 'A' && x <= 'Z')
      x += 32;
    if (y >= 'A' && y <= 'Z')
      y += 32;
    if (x != y) {
      result = (i32)x - y;
      break;
    }
    if (!y)
      break;
    ++a;
    ++b;
  }

  return result;
}
const u8 *GuestGetEnv(const u8 *name) {
  const u8 *result = NULL;

  if (pt_process_environment_present() && name) {
    u32 n = 0;
    while (*(name + n))
      ++n;
    for (u32 slot = 0;; ++slot) {
      const u8 *s = pt_environment_string(slot);
      if (!s)
        break;
      i32 compare = GuestCaseCompare(s, name, n);
      if (!compare && *(s + n) == '=') {
        result = s + n + 1;
        break;
      }
    }
  }

  return result;
}
const u8 *GuestParseTime(const u8 *text, PTTimeValue *output) {
  u32 value = 0;
  while (*text >= '0' && *text <= '9')
    value = 10u * value + *text++ - '0';
  output->value = value;

  return text;
}
const u8 *GuestParseOffset(const u8 *text, u8 *name, PTTimeValue *output) {
  PTTimezoneClock clock;
  if (*text == ':')
    ++text;
  const u8 *start = text;
  u8 c;
  while ((c = *text) && c != ',' && c != '-' && c != '+' && (c < '0' || c > '9'))
    ++text;
  u32 n = text - start;
  if (n > 30)
    n = 30;
  (void)memmove(name, start, n);
  name[n] = 0;
  bool negative = c == '-';
  if (c == '-' || c == '+')
    ++text;
  if (*text >= '0' && *text <= '9') {
    clock.second.value = 0;
    clock.minute.value = 0;
    clock.hour.value = 0;
    text = GuestParseTime(text, &clock.hour);
    if (*text == ':') {
      text = GuestParseTime(text + 1, &clock.minute);
      if (*text == ':') {
        text = GuestParseTime(text + 1, &clock.second);
      }
    }
    u32 value = 60u * (60u * clock.hour.value + clock.minute.value) + clock.second.value;
    output->value = negative ? 0u - value : value;
  }

  return text;
}
const u8 *GuestParseRule(const u8 *text, GuestTM *output) {
  PTTimezoneRuleValues values;
  GuestTM *r = output;
  i32 mode = -1;
  if (*text == 'J') {
    mode = 1;
    ++text;
  }
  if (*text == 'M') {
    mode = 0;
    ++text;
  }
  r->isdst = mode;
  text = GuestParseTime(text, &values.number);
  if (mode)
    r->yearday = pt_signed_dword(values.number.value);
  else {
    r->month = pt_signed_dword(values.number.value - 1);
    if (*text == '.') {
      text = GuestParseTime(text + 1, &values.number);
      r->day = pt_signed_dword(values.number.value);
      if (*text == '.') {
        text = GuestParseTime(text + 1, &values.number);
        r->weekday = pt_signed_dword(values.number.value);
      }
    }
    r->yearday = 0;
  }
  values.clock.hour.value = 2;
  values.clock.second.value = 0;
  values.clock.minute.value = 0;
  if (*text == '/') {
    text = GuestParseTime(text + 1, &values.clock.hour);
    if (*text == ':') {
      text = GuestParseTime(text + 1, &values.clock.minute);
      if (*text == ':') {
        text = GuestParseTime(text + 1, &values.clock.second);
      }
    }
  }
  r->second = pt_signed_dword(values.clock.second.value);
  r->minute = pt_signed_dword(values.clock.minute.value);
  r->hour = pt_signed_dword(values.clock.hour.value);

  return text;
}
PTTimezoneResult GuestParseTimezone(const u8 *text) {
  PTTimeValue daylight;
  bool has_dst = false;
  timezone_has_dst = 0;
  text = GuestParseOffset(text, timezone_standard_name, (PTTimeValue *)&timezone_seconds);
  if (*text) {
    daylight.value = (u32)timezone_seconds - 3600;
    has_dst = true;
    timezone_has_dst = 1;
    text = GuestParseOffset(text, timezone_dst_name, &daylight);
    timezone_dst_seconds = sub(timezone_seconds, pt_signed_dword(daylight.value));
    if (*text == ',') {
      text = GuestParseRule(text + 1, &timezone_start_rule);
    }
    if (*text == ',') {
      text = GuestParseRule(text + 1, &timezone_end_rule);
    }
  } else {
    timezone_dst_name[0] = 0;
  }

  return (PTTimezoneResult){text, has_dst};
}
PTTimezoneResult GuestTzset(void) {
  const u8 *text;
  text = GuestGetEnv(timezone_env_name);
  PTTimezoneResult result = {text, false};
  if (text) {
    result = GuestParseTimezone(text);
  }
  return result;
}
i32 GuestRuleYearDay(const GuestTM *rule, i32 year) {
  GuestTM time;
  const GuestTM *r = rule;
  i32 result;
  if (r->isdst)
    result = r->isdst == 1 ? sub(r->yearday, 1) : r->yearday;
  else {
    i32 length = month_length(year, r->month);
    GuestTM *t = &time;
    t->second = t->minute = t->hour = 0;
    t->day = 1;
    t->month = r->month;
    t->year = year;
    t->isdst = 0;
    i32 normalized_year = add(year, r->month / 12);
    if (r->month % 12 < 0)
      normalized_year = sub(normalized_year, 1);
    checked(year >= -184844639 && normalized_year >= 0, PT_FRAME_TIME_RULE_YEAR_INVALID);
    (void)GuestMktime(t);
    i32 weekday = sub(add(r->weekday, 7), t->weekday) % 7;
    i32 week = r->day == 5 ? (add(weekday, 29) <= length ? 4 : 3) : sub(r->day, 1);
    result = add(add(weekday, t->yearday), mul(week, 7));
  }

  return result;
}
u32 GuestRuleOrder(const GuestTM *first, const GuestTM *second, i32 year) {
  i32 a, b;
  a = GuestRuleYearDay(first, year);
  b = GuestRuleYearDay(second, year);

  return a > b;
}
u32 GuestTimeLess(const GuestTM *first, const GuestTM *second) {
  const GuestTM *a = first, *b = second;
  u32 r = a->hour < b->hour ||
      (a->hour == b->hour && (a->minute < b->minute || (a->minute == b->minute && a->second < b->second)));

  return r;
}
// Preserve the original transition-day inequalities and southern order.

i32 GuestIsInDST(GuestTM *output) {
  GuestTM *t = output;
  u32 southern = GuestRuleOrder(&timezone_start_rule, &timezone_end_rule, t->year);
  const GuestTM *first = southern ? &timezone_end_rule : &timezone_start_rule,
                *second = southern ? &timezone_start_rule : &timezone_end_rule;
  const GuestTM *a = first, *b = second;
  i32 length = month_length(t->year, t->month);
  bool in = false, edge = false;
  if (a->isdst) {
    i32 yd = a->yearday;
    if (a->isdst == 1) {
      u32 leap = GuestLeapYear(add(t->year, 1900));
      if (leap && time_common_month_days[2] < yd)
        yd = add(yd, 1);
      yd = sub(yd, 1);
    }
    if (t->yearday >= yd) {
      in = true;
      edge = t->yearday == yd;
    }
  } else if (t->month > a->month)
    in = true;
  else if (t->month == a->month) {
    i32 prev = sub(t->day, sub(add(t->weekday, 7), a->weekday) % 7),
        earlier = sub(sub(t->day, 1), sub(add(t->weekday, 6), a->weekday) % 7);
    i32 threshold = add(mul(7, sub(a->day, 1)), 1);
    if (a->day == 5) {
      if (sub(length, 7) < prev) {
        in = true;
        edge = earlier <= sub(length, 7);
      }
    } else if (threshold <= prev) {
      in = true;
      edge = earlier < threshold;
    }
  }
  if (edge) {
    u32 less = GuestTimeLess(output, first);
    in = !less;
  }
  if (!in) {
    t->isdst = (i32)southern;

    return t->isdst;
  }
  edge = false;
  if (b->isdst) {
    i32 yd = b->yearday;
    if (b->isdst == 1) {
      u32 leap = GuestLeapYear(add(t->year, 1900));
      if (leap && yd > time_common_month_days[2])
        yd = add(yd, 1);
      yd = sub(yd, 1);
    }
    if (t->yearday >= yd) {
      in = false;
      edge = t->yearday == yd;
    }
  } else if (t->month > b->month)
    in = false;
  else if (t->month == b->month) {
    i32 prev = sub(t->day, sub(add(t->weekday, 7), b->weekday) % 7),
        earlier = sub(sub(t->day, 1), sub(add(t->weekday, 6), b->weekday) % 7);
    i32 threshold = add(mul(7, sub(b->day, 1)), 1);
    in = false;
    if (b->day == 5) {
      if (prev <= sub(length, 7))
        in = true;
      else if (earlier <= sub(length, 7))
        edge = true;
    } else {
      if (prev < threshold)
        in = true;
      else if (earlier < threshold)
        edge = true;
    }
  }
  if (edge) {
    u32 less = GuestTimeLess(output, second);
    in = less;
  }
  t->isdst = (i32)southern - (i32)(southern ? in : 0);
  if (!southern)
    t->isdst = in;

  return t->isdst;
}
u32 GuestMktime(GuestTM *output) {
  GuestTM *t = output;
  u32 result = UINT32_MAX;
  i32 m = t->month % 12, y = t->year;
  if (y < -184844639)
    goto done;
  y = add(y, t->month / 12);
  if (m < 0) {
    y = sub(y, 1);
    m += 12;
  }
  if (y < 0)
    goto done;
  const i16 *table = month_table(y);
  i32 d =
      add(add(mul(365, y), pt_signed_dword(((u32)add(y, 3) >> 2) | (add(y, 3) < 0 ? 0xc0000000u : 0))), days(table, m));
  d = sub(add(d, t->day), y ? 2 : 1);
  i32 seconds = add(mul(60, add(mul(60, t->hour), t->minute)), t->second);
  while (seconds < 0) {
    d = sub(d, 1);
    seconds = add(seconds, 86400);
  }
  (void)GuestBreakTime((u32)d, (u32)seconds, 0, output);
  (void)GuestTzset();
  seconds = add(timezone_seconds, seconds);
  if (t->isdst < 0) {
    (void)GuestIsInDST(output);
  }
  if (t->isdst > 0)
    seconds = sub(seconds, timezone_dst_seconds);
  while (seconds < 0) {
    d = sub(d, 1);
    seconds = add(seconds, 86400);
  }
  if (d < 25566)
    goto done;
  if (d == 25566) {
    seconds = sub(seconds, 86400);
    if (timezone_seconds <= 0 || seconds < 0)
      goto done;
    result = (u32)seconds;
  } else
    result = (u32)add(mul(86400, sub(d, 25567)), seconds);
done:

  return result;
}
u32 GuestTime(PTTimeValue *output) {
  GuestTM now;
  u32 ms, result;
  ms = GuestGetCurrentTime(&now);
  if (ms >= 500)
    now.second = pt_signed_dword((u32)now.second + 1u);
  result = GuestMktime(&now);
  if (output) {
    output->value = result;
  }

  return result;
}
