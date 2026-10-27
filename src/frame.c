// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
// Frame.

#include "world.h"
#include "input.h"
#include "player.h"
#include "objects.h"
#include "frame.h"
#include "game.h"
#include "messages.h"
#include "graphics.h"
#include "calendar.h"
#include "sound.h"
static void map_setup(void) {
  did_terminal_stuff = 0;
  mouse_over_zone = NULL;

  PrintGenericInfoBox();

  Do3x3ButtonStuff(0);

  CopyBitmap2to1();

  SetCurrentBitmap(old_bitmap);

  DrawMapAnyZoom(1, 0, 0);
}
void map_after_draw(void);
u8 map_loop(void) {
  for (;;) {
    if (game_exit_requested)
      return 0;
    has_many_players = 0;
    ptr_apple->view_state = 1;

    mouse_pressed = WaitForMouseClick();

    if (!mouse_pressed) {
      if (IsMouseOverMap()) {
        u16 zone_x = (u16)(mouse_x - 32u), zone_y = (u16)(mouse_y - 61u);

        last_mouse_over_district = FindZoneFromCoordinates((i16)zone_x, (i16)zone_y);
        if (last_mouse_over_district != mouse_over_zone) {
          mouse_over_zone = last_mouse_over_district;

          PrintGenericInfoBox();
        }
      }
      continue;
    }
    // The original checks pending missions, goals and phone calls before
    // processing every nonzero event, including timer code ten.

    u8 goal_result = CheckGoalStuff();

    if (goal_result) {
      map_setup();
      map_after_draw();
      continue;
    }
    if (mouse_pressed == 10) {
      DrawMapMarkers();

    } else if (pt_is_menu_button_event(mouse_pressed)) {
      game_ptr->in_menu_hierarchy = 0;

      u8 menu_result = HandleClickOnMapMenuButtons();

      if (menu_result)
        return menu_result;
      game_ptr->in_menu_hierarchy = 1;
      map_setup();
      map_after_draw();
      continue;
    } else if (mouse_pressed == 2)
      map_marker_toggle ^= 1;
    else if (mouse_pressed == 3117) {
      SetCurrentBitmap(old_bitmap);

      CommitBitmapAndSetPalette(0);
    } else if (mouse_pressed == 1) {
      DrawMapFromOldToCurrent();

      u32 controls_result = HandleClickOnMapControls();

      if (controls_result) {
        map_setup();
        map_after_draw();
        continue;
      }

      const Mouse *mouse = mouse_result;
      u32 x = (u32)(i32)mouse->x, y = (u32)(i32)mouse->y;

      u16 selection = (u16)HandleMapZoomLevel_Helper(x, y);

      if (selection != UINT16_MAX) {
        map_scroll_x = (u8)selection;
        map_scroll_y = (u8)(selection >> 8);
        return 0xd1;
      }
    }
  }
}
u8 HandleMapZoomLevel1(void) {
  game_ptr->map_zoom_level = 0;
  map_scroll_y = 1;
  map_scroll_x = 1;
  map_setup();
  map_after_draw();
  return map_loop();
}
void map_after_draw(void) {
  SelectPalette(62);

  CommitBitmapAndSetPalette(62);

  DrawMapFromCurrentToOld();
  SaveAndSetInputEventFilter(7);
}

// Overview and middle-scale handlers share callees, but deliberately keep
// their original setup and event ordering separate.

static void middle_draw(void) {
  game_ptr->current_palette = 255;

  PrintGenericInfoBox();

  SetCurrentBitmap(old_bitmap);

  DrawMapAnyZoom(2, map_scroll_x, map_scroll_y);

  CommitBitmapAndSetPalette(62);

  DrawMapFromCurrentToOld();
}
static void middle_after_draw(void) {
  CopyBitmap2to1();

  temp_canvas->bitmap = current_bitmap;
  canvas_1->bitmap = current_bitmap;
  SaveAndSetInputEventFilter(7);
}
static bool middle_setup(void) {
  if (has_many_players)
    return false;
  u16 zone_x = (u16)(map_scroll_x + 5u), zone_y = (u16)(map_scroll_y + 5u);

  last_mouse_over_district = FindZoneFromCoordinates((i16)zone_x, (i16)zone_y);
  mouse_over_zone = last_mouse_over_district;

  Do3x3ButtonStuff(0);

  InitGraphicsMode();

  middle_draw();
  middle_after_draw();
  return true;
}
static Bitmap *middle_strip(bool column, u8 index, u8 x, u8 y, u8 count) {
  _Static_assert(sizeof(map_row_bitmaps) / sizeof(map_row_bitmaps[0]) == 2, "two native bitmap pointers");
  _Static_assert(sizeof(map_column_bitmaps) / sizeof(map_column_bitmaps[0]) == 2, "two native bitmap pointers");

  if (index >= 2)
    pt_map_unsupported(column ? PT_MAP_MIDDLE_COLUMN : PT_MAP_MIDDLE_ROW);
  Bitmap *bitmap = column ? map_column_bitmaps[index] : map_row_bitmaps[index];

  (column ? HandleMapZoomLevel2_Helper2 : HandleMapZoomLevel2_Helper1)(bitmap, x, y, 0, count);

  return bitmap;
}
// A held scan code keeps scrolling between page commits.

static bool map_scroll_key_held(u16 key) {
  u16 port = (u16)ReadKeyboardPort();

  if (port == (key >> 8))
    return true;

  bool held = WantSkipGameIntro() != 0;

  return held;
}
static void middle_scroll(u16 key) {
  bool column = key == 0x4b00 || key == 0x4d00;
  bool forward = key == 0x5000 || key == 0x4d00;
  u8 current = game_ptr->current_bitmap_index, old = game_ptr->old_bitmap_index;
  u8 x = map_scroll_x, y = map_scroll_y;
  if (column)
    x = (u8)(x + (forward ? 40 : -1));
  else
    y = (u8)(y + (forward ? 30 : -1));
  Bitmap *strip = middle_strip(column, current, x, y, column ? 30 : 40);
  for (;;) {
    u8 coordinate = column ? map_scroll_x : map_scroll_y;
    if (forward ? coordinate >= (column ? 120 : 90) : !coordinate)
      break;
    if (!map_scroll_key_held(key))
      break;
    if (column) {
      map_scroll_x = (u8)(map_scroll_x + (forward ? 1 : -1));
      x = (u8)(map_scroll_x + (forward ? 40 : -1));
      y = map_scroll_y;
    } else {
      map_scroll_y = (u8)(map_scroll_y + (forward ? 1 : -1));
      y = (u8)(map_scroll_y + (forward ? 30 : -1));
      x = map_scroll_x;
    }
    middle_strip(column, old, x, y, column ? 32 : 40);
    PTBlitRect copy = {
      .sx = 32, .sy = 61, .dx = 32, .dy = 61, .width = column ? 156 : 160, .height = column ? 120 : 116};
    if (column) {
      if (forward)
        copy.sx = 36;
      else
        copy.dx = 36;
    } else {
      if (forward)
        copy.sy = 65;
      else
        copy.dy = 65;
    }

    BitBlt320Width(current_bitmap, copy.sx, copy.sy, old_bitmap, copy.dx, copy.dy, copy.width, copy.height);
    PTBlitRect edge = {.sx = 0,
      .sy = 0,
      .dx = column && forward ? 188 : 32,
      .dy = !column && forward ? 177 : 61,
      .width = column ? 4 : 160,
      .height = column ? 120 : 4};

    BitBlt(strip, 0, 0, old_bitmap, edge.dx, edge.dy, edge.width, edge.height);

    CommitBitmapAndSetPalette(0);

    DequeueInputEvent();
    current ^= 1;
    old ^= 1;
    if (current >= 2)
      pt_map_unsupported(column ? PT_MAP_MIDDLE_COLUMN : PT_MAP_MIDDLE_ROW);
    strip = column ? map_column_bitmaps[current] : map_row_bitmaps[current];
  }
  middle_after_draw();
}
u8 HandleMapZoomLevel2(void) {
  game_ptr->map_zoom_level = 1;
  if (!map_scroll_x)
    map_scroll_x = 1;
  if (!map_scroll_y)
    map_scroll_y = 1;
  if (map_scroll_x >= 120)
    map_scroll_x = 120;
  if (map_scroll_y >= 90)
    map_scroll_y = 90;
  u8 result = 0xd0;
  if (!middle_setup())
    goto done;
  for (;;) {
    ptr_apple->view_state = 1;

    mouse_pressed = WaitForMouseClick();

    if (!mouse_pressed) {
      if (IsMouseOverMap()) {
        u16 zone_x = (u16)(map_scroll_x + (u16)((mouse_x - 32) / 4)),
            zone_y = (u16)(map_scroll_y + (u16)((mouse_y - 61) / 4));

        last_mouse_over_district = FindZoneFromCoordinates((i16)zone_x, (i16)zone_y);
        if (last_mouse_over_district != mouse_over_zone) {
          mouse_over_zone = last_mouse_over_district;

          PrintGenericInfoBox();
        }
      }
      continue;
    }
    if (mouse_pressed == 10) {
      u8 goal_result = CheckGoalStuff();

      if (goal_result) {
        if (!middle_setup())
          goto done;
        continue;
      }

      DrawMapMarkers();

      continue;
    }
    if (pt_is_menu_button_event(mouse_pressed)) {
      game_ptr->in_menu_hierarchy = 0;

      u8 menu_result = HandleClickOnMapMenuButtons();

      if (menu_result) {
        result = menu_result;
        goto done;
      }
      if (has_many_players)
        goto done;
      game_ptr->in_menu_hierarchy = 1;
      middle_draw();
      middle_after_draw();
      continue;
    }
    if (mouse_pressed == 2 || mouse_pressed == 0x3b00)
      goto done;
    if (mouse_pressed == 0x3d00) {
      result = 0xd2;
      goto done;
    }
    if (mouse_pressed == 1) {
      DrawMapFromOldToCurrent();

      u32 controls_result = HandleClickOnMapControls();

      if (controls_result) {
        middle_draw();
        middle_after_draw();
        continue;
      }

      const Mouse *mouse = mouse_result;

      u16 selection = (u16)HandleMapZoomLevel_Helper((u32)(i32)mouse->x, (u32)(i32)mouse->y);

      if (selection != UINT16_MAX) {
        map_scroll_x = (u8)selection;
        map_scroll_y = (u8)(selection >> 8);
        result = 0xd2;
        goto done;
      }
    }
    if ((mouse_pressed == 0x4800 || mouse_pressed == 0x5000 || mouse_pressed == 0x4b00 || mouse_pressed == 0x4d00) &&
        !curr_map_mode) {
      DrawMapFromOldToCurrent();
      middle_scroll(mouse_pressed);
    }
  }
done:

  return result;
}
static bool largest_setup(void) {
  InitGraphicsMode();

  Do3x3ButtonStuff(0);
  game_ptr->current_palette = 255;
  if (has_many_players)
    return false;
  u16 zone_x = (u16)(map_scroll_x + 5u), zone_y = (u16)(map_scroll_y + 5u);

  last_mouse_over_district = FindZoneFromCoordinates((i16)zone_x, (i16)zone_y);
  mouse_over_zone = last_mouse_over_district;
  game_ptr->did_set_bitmap = 1;
  if (map_scroll_x > 148)
    map_scroll_x = 148;
  if (map_scroll_y > 109)
    map_scroll_y = 109;

  DrawMapAnyZoom(3, map_scroll_x, map_scroll_y);

  PrintCurrentDateAndTime();
  DummyFunc3();

  CopyBitmap2to1();
  SaveAndSetInputEventFilter(3);

  map_actor_count = DrawMapHelper3();

  AdvanceClock(1);
  return true;
}
static Bitmap *largest_strip(bool column, u8 index, u8 x, u8 y, u8 first, u8 count) {
  if (index >= 2)
    pt_map_unsupported(column ? PT_MAP_DETAILED_COLUMN : PT_MAP_DETAILED_ROW);
  Bitmap *bitmap = column ? largest_map_column_bitmaps[index] : largest_map_row_bitmaps[index];

  (column ? DrawMapHelper2 : DrawMapHelper1)(bitmap, x, y, first, count);

  return bitmap;
}
static void largest_scroll(u16 key) {
  bool column = key == 0x4b00 || key == 0x4d00, forward = key == 0x5000 || key == 0x4d00;
  u8 coordinate = column ? map_scroll_x : map_scroll_y;
  // The original down-arrow prefill gate is 148, although movement stops
  // at 109. A tap at 109 still rebuilds the viewport with distance zero.

  if (forward ? coordinate >= 148 : !coordinate)
    return;
  u8 direction = column ? (forward ? 0 : 1) : (forward ? 3 : 2), distance = 0;
  u8 current = game_ptr->current_bitmap_index, old = game_ptr->old_bitmap_index;
  u8 x = map_scroll_x, y = map_scroll_y;
  if (column)
    x = (u8)(x + (forward ? 12 : -1));
  else
    y = (u8)(y + (forward ? 11 : -1));
  Bitmap *strip = largest_strip(column, current, x, y, 0, column ? 11 : 12);
  for (;;) {
    coordinate = column ? map_scroll_x : map_scroll_y;
    if (forward ? coordinate >= (column ? 148 : 109) : !coordinate)
      break;
    if (!map_scroll_key_held(key))
      break;
    if (column) {
      map_scroll_x = (u8)(map_scroll_x + (forward ? 1 : -1));
      x = (u8)(map_scroll_x + (forward ? 12 : -1));
      y = map_scroll_y;
    } else {
      map_scroll_y = (u8)(map_scroll_y + (forward ? 1 : -1));
      y = (u8)(map_scroll_y + (forward ? 11 : -1));
      x = map_scroll_x;
    }
    ++distance;
    for (u8 pixel = 0; pixel < 16; pixel += 4) {
      largest_strip(column, old, x, y, pixel, 4);
      PTBlitRect copy = {
        .sx = 12, .sy = 14, .dx = 12, .dy = 14, .width = column ? 188 : 192, .height = column ? 176 : 172};
      if (column) {
        if (forward)
          copy.sx = 16;
        else
          copy.dx = 16;
      } else {
        if (forward)
          copy.sy = 18;
        else
          copy.dy = 18;
      }

      BitBlt320Width(current_bitmap, copy.sx, copy.sy, old_bitmap, copy.dx, copy.dy, copy.width, copy.height);

      u16 source = forward ? pixel : (u16)(12u - pixel);
      PTBlitRect edge = {.sx = column ? source : 0,
        .sy = column ? 0 : source,
        .dx = column && forward ? 200 : 12,
        .dy = !column && forward ? 186 : 14,
        .width = column ? 4 : 192,
        .height = column ? 176 : 4};

      BitBlt(strip, edge.sx, edge.sy, old_bitmap, edge.dx, edge.dy, edge.width, edge.height);

      CommitBitmapAndSetPalette(0);
    }
    DequeueInputEvent();
    current ^= 1;
    old ^= 1;
    if (current >= 2)
      pt_map_unsupported(column ? PT_MAP_DETAILED_COLUMN : PT_MAP_DETAILED_ROW);
    strip = column ? largest_map_column_bitmaps[current] : largest_map_row_bitmaps[current];
  }
  state_slot_023 = 1;

  SoundStuff4();

  u16 zone_x = (u16)(map_scroll_x + 6u), zone_y = (u16)(map_scroll_y + 5u);

  mouse_over_zone = FindZoneFromCoordinates((i16)zone_x, (i16)zone_y);

  HandleMapZoomLevel3_Helper1(direction, distance);

  state_slot_023 = 0;
}
u8 HandleMapZoomLevel3(void) {
  bool pages_differ = false;
  u8 result = 0xd0;
  game_ptr->map_zoom_level = 2;
  if (!largest_setup())
    goto done;
  for (;;) {
    game_ptr->last_loaded_background = 0;
    game_ptr->did_set_bitmap = 1;
    ptr_apple->view_state = 0;

    SelectPalette(62);

    mouse_pressed = WaitForMouseClick();

    u8 goal_result = CheckGoalStuff();

    if (goal_result) {
      if (!largest_setup())
        goto done;
      continue;
    }
    if (mouse_pressed == 10) {
      HandleMapZoomLevel3_Helper2();

      if (map_animation_palette_ticks == 9) {
        // Rotate a four-color window through the palette and its repeated tail.

        const PTRGB *colors = pt_map_animation_colors(map_animation_palette_index);

        UploadPalette(140, 4, colors);

        map_animation_palette_ticks = 0;
        if (++map_animation_palette_index == 4)
          map_animation_palette_index = 0;
      }
      ++map_animation_palette_ticks;
      continue;
    }
    if (mouse_pressed > 4) {
      if (pages_differ) {
        Copy320x200Bitmap(current_bitmap, old_bitmap);
        pages_differ = false;
      }
      if (mouse_pressed == 0x4800 || mouse_pressed == 0x5000 || mouse_pressed == 0x4b00 || mouse_pressed == 0x4d00)
        largest_scroll(mouse_pressed);
      if (!pt_is_menu_button_event(mouse_pressed))
        continue;
    }
    pages_differ = true;
    ptr_apple->view_state = 3;
    if (pt_is_menu_button_event(mouse_pressed)) {
      game_ptr->in_menu_hierarchy = 0;

      u8 menu_result = HandleClickOnMapMenuButtons();

      if (menu_result) {
        result = menu_result;
        goto done;
      }
      if (has_many_players) {
        result = 0xd0;
        goto done;
      }

      SwapBitmapPages();

      DrawMapAnyZoom(3, map_scroll_x, map_scroll_y);

      DummyFunc3();

      map_actor_count = DrawMapHelper3();

      Do3x3ButtonStuff(0);
      game_ptr->in_menu_hierarchy = 1;

      InitGraphicsMode();

      SaveAndSetInputEventFilter(3);
      continue;
    }
    if (mouse_pressed == 1) {
      const Mouse *mouse = mouse_result;
      u8 x = (u8)(map_scroll_x + (u8)(((i32)mouse->x - 12) / 16));
      u8 y = (u8)(map_scroll_y + (u8)(((i32)mouse->y - 14) / 16));

      u8 action = HandleMapZoomLevel3_Helper5(y, x);

      if (action == 255) {
        result = action;
        goto done;
      }
      if (has_many_players) {
        result = 0xd0;
        goto done;
        // Original loc_33734.
      }
      if (action) {
        InitGraphicsMode();

        LoadMusic(7);

        SwapBitmapPages();

        DrawMapAnyZoom(3, map_scroll_x, map_scroll_y);

        DummyFunc3();

        map_actor_count = DrawMapHelper3();

        Do3x3ButtonStuff(0);

        InitGraphicsMode();
      }
    } else if (mouse_pressed == 2) {
      map_scroll_x = (u8)(map_scroll_x - (map_scroll_x <= 14 ? map_scroll_x : 14));
      map_scroll_y = (u8)(map_scroll_y - (map_scroll_y <= 9 ? map_scroll_y : 9));
      result = 0xd1;
      goto done;
    }
    SaveAndSetInputEventFilter(3);
  }
done:

  return result;
}

static void DummyFunc(void) {}
// Shared map dispatch for fresh and resumed games.

void run_game_map_loop(u8 mode) {
  do {
    DoSomeClockPaletteStuff();

    Do3x3ButtonStuff(0);

    game_ptr->in_menu_hierarchy = 1;
    game_ptr->byte86 = 1;
    ptr_apple->view_state = 1;

    CopyBitmap2to1();

    InitGraphicsMode();

    LoadMusic(7);

    if (mode == 208 || mode == 255 || mode == 209) {
      mode = mode == 209 ? HandleMapZoomLevel2() : HandleMapZoomLevel1();

    } else if (mode == 210) {
      ptr_apple->view_state = 0;

      mode = HandleMapZoomLevel3();

      ptr_apple->view_state = 1;
    }
  } while (!game_exit_requested);
}

void RunGame(void) {
  u8 previous_minutes;
  u8 mode = 208;
  previous_minutes = ptr_apple->minutesx10;
  main_menu_canvas_flag = 1;

  InitGraphicsMode();

  HandleNewDay();

  SetCurrentPlayerAndCity(ptr_apple->player_index);

  if (state_slot_009)
    ptr_apple->minutesx10 = previous_minutes;

  DummyFunc();

  run_game_map_loop(mode);
}

u16 IsMouseOverMap(void) {
  return IsMouseInside(32, 61, 192, 181);
}
Zone *FindZoneFromCoordinates(i16 x, i16 y) {
  const Town *town = current_town;
  PTForwardLink *zone = town->zones.begin.next;
  if (x < 0 || y < 0 || x >= 160 || y >= 120)
    return (Zone *)zone;
  u8 steps = loaded_bez_file[y / 8][x / 8] & 15u;
  while (steps--)
    zone = zone->next;
  return (Zone *)zone;
}

// Frame scheduler.

#include "render_types.h"
#include <pthread.h>
#include <stdlib.h>

static pthread_mutex_t frame_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t frame_condition = PTHREAD_COND_INITIALIZER;
static pthread_t frame_thread;
static bool created, resume_requested, cancel_requested, waiting, finished;
static PTFrameState frame_state;
static _Thread_local bool worker_thread;
static void (*task_cleanup)(bool, void *);
static void *cleanup_context;
static void *(*task_entry)(void *);
static void *task_context;

bool pt_frame_worker(void) {
  return worker_thread;
}
bool pt_frame_active(void) {
  pthread_mutex_lock(&frame_mutex);
  bool result = created;
  pthread_mutex_unlock(&frame_mutex);
  return result;
}
bool pt_frame_suspended(void) {
  pthread_mutex_lock(&frame_mutex);
  bool result = created && waiting && !frame_state.status;
  pthread_mutex_unlock(&frame_mutex);
  return result;
}
bool pt_frame_ready(void) {
  pthread_mutex_lock(&frame_mutex);
  bool result = created && waiting && !frame_state.status;
  pthread_mutex_unlock(&frame_mutex);
  return result;
}
bool pt_frame_set_cleanup(void (*cleanup)(bool, void *), void *context) {
  pthread_mutex_lock(&frame_mutex);
  bool okay = !created;
  if (okay) {
    task_cleanup = cleanup;
    cleanup_context = context;
  }
  pthread_mutex_unlock(&frame_mutex);
  return okay;
}
bool pt_frame_export(PTFrameState *state) {
  if (!state)
    return false;
  pthread_mutex_lock(&frame_mutex);
  *state = frame_state;
  pthread_mutex_unlock(&frame_mutex);
  return true;
}

void pt_frame_boundary(u32 tag) {
  if (!worker_thread)
    return;
  pthread_mutex_lock(&frame_mutex);
  frame_state.boundary = tag;
  frame_state.status = PT_COMMAND_OK;
  ++frame_state.serial;
  waiting = true;
  pthread_cond_broadcast(&frame_condition);
  while (!resume_requested && !cancel_requested)
    pthread_cond_wait(&frame_condition, &frame_mutex);
  if (cancel_requested) {
    waiting = false;
    pthread_mutex_unlock(&frame_mutex);
    pthread_exit(NULL);
  }
  resume_requested = false;
  waiting = false;
  pthread_mutex_unlock(&frame_mutex);
}
_Noreturn void pt_frame_fail(unsigned status, u32 tag) {
  if (!worker_thread)
    abort();
  pthread_mutex_lock(&frame_mutex);
  frame_state.status = status;
  frame_state.stopped_at = tag;
  frame_state.boundary = 0;
  ++frame_state.serial;
  waiting = false;
  pthread_cond_broadcast(&frame_condition);
  pthread_mutex_unlock(&frame_mutex);
  pthread_exit(NULL);
  abort();
}
_Noreturn void pt_frame_unsupported(u32 tag) {
  pt_frame_fail(PT_COMMAND_UNSUPPORTED, tag);
}
_Noreturn void pt_map_unsupported(PTMapUnsupported operation) {
  if ((unsigned)operation > PT_MAP_DETAILED_COLUMN)
    abort();
  PTFrameBoundary reason = (PTFrameBoundary)(PT_FRAME_MAP_MIDDLE_ROW_UNAVAILABLE + operation);
  pt_frame_unsupported((u32)reason);
}
void pt_frame_require_boundary(unsigned status, PTFrameBoundary reason) {
  if (worker_thread && status)
    pt_frame_fail(status, (u32)reason);
}
_Noreturn void pt_frame_process_exit(u32 tag) {
  if (!worker_thread)
    abort();
  pthread_mutex_lock(&frame_mutex);
  frame_state.active = 0;
  frame_state.boundary = tag;
  frame_state.status = 0;
  frame_state.stopped_at = 0;
  ++frame_state.serial;
  waiting = false;
  pthread_cond_broadcast(&frame_condition);
  pthread_mutex_unlock(&frame_mutex);
  pthread_exit(NULL);
  abort();
}
static void worker_finished(void *context) {
  (void)context;
  pthread_mutex_lock(&frame_mutex);
  finished = true;
  waiting = false;
  pthread_cond_broadcast(&frame_condition);
  pthread_mutex_unlock(&frame_mutex);
  worker_thread = false;
}
static void *run_task(void *context) {
  (void)context;
  worker_thread = true;
  pthread_cleanup_push(worker_finished, NULL);
  (void)task_entry(task_context);
  pt_frame_process_exit(0);
  pthread_cleanup_pop(1);
  return NULL;
}
void pt_frame_reset(void) {
  if (worker_thread)
    abort();
  pthread_mutex_lock(&frame_mutex);
  bool join = created;
  if (join) {
    cancel_requested = true;
    pthread_cond_broadcast(&frame_condition);
  }
  pthread_mutex_unlock(&frame_mutex);
  if (join)
    pthread_join(frame_thread, NULL);
  if (task_cleanup)
    task_cleanup(join, cleanup_context);
  pthread_mutex_lock(&frame_mutex);
  created = waiting = resume_requested = cancel_requested = finished = false;
  frame_state = (PTFrameState){0};
  task_entry = NULL;
  task_context = NULL;
  task_cleanup = NULL;
  cleanup_context = NULL;
  pthread_mutex_unlock(&frame_mutex);
}
bool pt_frame_start(void *(*entry)(void *), void *context) {
  if (!entry || worker_thread)
    return false;
  pthread_mutex_lock(&frame_mutex);
  if (created) {
    pthread_mutex_unlock(&frame_mutex);
    return false;
  }
  frame_state = (PTFrameState){.active = 1};
  resume_requested = cancel_requested = waiting = finished = false;
  task_entry = entry;
  task_context = context;
  // Save/load plans use substantial host scratch storage across nested calls.

  pthread_attr_t attributes;
  int error = pthread_attr_init(&attributes);
  if (!error) {
    error = pthread_attr_setstacksize(&attributes, 8u * 1024u * 1024u);
    if (!error)
      error = pthread_create(&frame_thread, &attributes, run_task, NULL);
    pthread_attr_destroy(&attributes);
  }
  if (error) {
    frame_state = (PTFrameState){0};
    task_entry = NULL;
    task_context = NULL;
    pthread_mutex_unlock(&frame_mutex);
    return false;
  }
  created = true;
  while (!waiting && !frame_state.status && !finished)
    pthread_cond_wait(&frame_condition, &frame_mutex);
  bool success = waiting;
  pthread_mutex_unlock(&frame_mutex);
  if (!success)
    pt_frame_reset();
  return success;
}
PTCommandResult pt_frame_step(const PTCommand *command) {
  if (!command || !pt_frame_ready())
    return (PTCommandResult){PT_COMMAND_UNSUPPORTED, 0};
  if (command->kind == PT_CMD_FRAME_JOYSTICK) {
    PTJoystickPlatform state = {
      command->args[0], command->args[1], command->args[2], command->args[3], command->args[4], command->args[5]};
    pt_joystick_supply_platform(&state);
    if (command->args[6]) {

      ReadJoystick();
    }
    return (PTCommandResult){0, 0};
  }
  if (command->kind == PT_CMD_FRAME_KEYBOARD_PORT) {
    pt_input_supply_keyboard_port((u8)command->args[0]);
    return (PTCommandResult){0, 0};
  }
  if (command->kind == PT_CMD_FRAME_KEY) {
    pt_input_supply((u32)command->platform_clock, (u16)command->key);
    return (PTCommandResult){0, 0};
  }
  if (command->kind == PT_CMD_FRAME_POINTER) {
    const PTPointerSample *p = &command->pointer_path[0];
    (void)pt_mouse_dispatch_event((u16)p->x, (u16)p->y, p->buttons);
    return (PTCommandResult){0, 0};
  }
  pt_input_supply((u32)command->platform_clock, (u16)command->key);
  pthread_mutex_lock(&frame_mutex);
  if (!created || !waiting || finished) {
    pthread_mutex_unlock(&frame_mutex);
    return (PTCommandResult){PT_COMMAND_UNSUPPORTED, 0};
  }
  u64 previous = frame_state.serial;
  resume_requested = true;
  pthread_cond_broadcast(&frame_condition);
  while (frame_state.serial == previous && !finished)
    pthread_cond_wait(&frame_condition, &frame_mutex);
  PTCommandResult result = {frame_state.status, frame_state.boundary};
  pthread_mutex_unlock(&frame_mutex);
  return result;
}
static void boundary(PTFrameBoundary value) {
  pt_frame_boundary((u32)value);
}
void pt_frame_wait_retrace_boundary(void) {
  boundary(PT_FRAME_RETRACE);
}
void pt_frame_display_retrace_boundary(void) {
  boundary(PT_FRAME_DISPLAY);
}
void pt_frame_poll(PTFramePoll poll) {
  static const PTFrameBoundary points[] = {PT_FRAME_CLOCK,
    PT_FRAME_KEYBOARD,
    PT_FRAME_STOCK_LIMIT,
    PT_FRAME_SOUND_ENTER,
    PT_FRAME_SOUND_LEAVE,
    PT_FRAME_DELAY_WAIT,
    PT_FRAME_DELAY_FIRST,
    PT_FRAME_DELAY_CHANGE,
    PT_FRAME_DELAY_COUNT};
  if ((unsigned)poll >= sizeof(points) / sizeof(points[0]))
    abort();
  boundary(points[poll]);
}

// Game start.

#include "menu.h"
#include "files.h"
#include "text.h"
#include "finance.h"
#include "runtime_state.h"
#include <string.h>

typedef struct PTGameStartup {
  i32 argc;
  char **argv;
} PTGameStartup;
static void release_arguments(PTGameStartup *startup) {
  if (!startup)
    return;
  for (i32 i = 0; i < startup->argc; ++i)
    free(startup->argv[i]);
  free(startup->argv);
  free(startup);
}
static void reset(bool joined, void *context) {
  (void)joined;
  release_arguments(context);
}
static void *run(void *context) {
  PTGameStartup *startup = context;
  pt_frame_boundary(PT_FRAME_STARTUP);
  GameMain(startup->argc, (const char *const *)startup->argv, NULL);
  return NULL;
}
bool pt_game_start(i32 argc, const char *const *argv) {
  if (argc < 0 || argc > 256 || (argc && !argv) || pt_frame_active() || pt_frame_worker())
    return false;
  PTGameStartup *startup = calloc(1, sizeof(*startup));
  if (!startup)
    return false;
  startup->argc = argc;
  startup->argv = calloc((size_t)argc + 1u, sizeof(*startup->argv));
  if (!startup->argv) {
    free(startup);
    return false;
  }
  for (i32 i = 0; i < argc; ++i) {
    if (!argv[i]) {
      release_arguments(startup);
      return false;
    }
    size_t length = strlen(argv[i]);
    if (length > 4095) {
      release_arguments(startup);
      return false;
    }
    startup->argv[i] = malloc(length + 1u);
    if (!startup->argv[i]) {
      release_arguments(startup);
      return false;
    }
    memcpy(startup->argv[i], argv[i], length + 1u);
  }
  pt_frame_reset();
  if (!pt_frame_set_cleanup(reset, startup)) {
    release_arguments(startup);
    return false;
  }
  // A failed start runs cleanup and releases the arguments.

  if (!pt_frame_start(run, startup)) {
    pt_frame_reset();
    return false;
  }
  return true;
}

void ReadJoystick(void) {

  PTJoystickPlatform state;
  pt_joystick_export_platform(&state);
  if (state.present) {
    joystick_center_x = (u16)state.x;
    joystick_center_y = (u16)state.y;
    if (!(state.x | state.y)) {
      joystick_center_x = (u16)state.x2;
      joystick_center_y = (u16)state.y2;
    }
    command_line_arg = 1;
  }
}

void GameMain(i32 argc, const char *const *argv, const char *const *envp) {
  (void)envp;
  PTStartupSoundConfig config;
  GuestFileStream *stream = OpenFile(main_sound_config_filename, 0);

  if (stream) {
    (void)ReadFile(stream, &config, sizeof(config));

    if (config.mode == 4)
      sound_config_flag = 0;
    (void)CloseFile(stream);
  }
  ReadJoystick();

  if (argc >= 2) {
    i32 option;
    option = GuestAtoi(argv[1]);
    command_line_arg = (u16)option;
  }
  game_ptr = &game;
  (void)pt_game_initialize(&game);
  (void)memset(players, 0, sizeof(players));
  (void)memset(characters, 0, sizeof(characters));
  game_ptr->mouse_cursor_visible = 1;
  game_ptr->player_table = players;
  game_ptr->character_table = characters;
  (void)LoadGraphicsFiles();
  (void)EnableDiskReadErrors();
  ptr_apple = &room;
  display_bitmaps[0] = &bmp2;
  display_bitmaps[1] = &bmp1;
  Game_CreateBitmaps(&game);
  Game_Init2(&game);

  stream = OpenFile(main_prefs_filename, 0);
  if (stream) {
    (void)ReadFile(stream, &prefs, 12);

    (void)CloseFile(stream);
  }
  SetGameFlag8();
  InitAppleFields();
  for (;;) {
    game_exit_requested = 0;
    InitPaletteColors();
    temp_canvas->color = 1;
    Canvas_SetFont(temp_canvas, font_astandar);
    some_font_ptr = font_cstandar;
    ptr_apple->num_valid_daily_profits_entries = 1;
    u8 new_game;
    new_game = HandleMainMenu();
    if (new_game) {
      InitGraphicsMode();
      (void)CountrySelector(0, 1);
      if (days_in_month[0]) {
        InitQuickstartRestaurant();
        SetCurrentPlayer(0);
      }
    }
    temp_canvas->color = 1;
    FillLetterBackground(4, 0, 0);
    InitGraphicsMode();
    PrintGenericInfoBox();
    HandleNewDay();
    game_ptr->flagB9 = 1;
    RunGame();
  }
}
