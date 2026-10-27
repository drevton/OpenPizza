// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
// Map.

#include "objects.h"
#include "files.h"
#include "world.h"
#include "text.h"
#include "graphics.h"
#include "finance.h"
#include "frame.h"
#include "input.h"
#include <stdlib.h>
#include "menu.h"
#include "game.h"

// map render

u32 CopyBitmapToBuffer(const Bitmap *source, u8 *buffer) {
  Bitmap target;
  InitializeBitmap(&target, 160, 120);
  pt_bitmap_bind_pixels(&target, buffer, 160u * 120u);
  u32 result = BitBlt(source, 32, 61, &target, 0, 0, 160, 120);
  return result;
}
u32 CopyBufferToBitmap(const Bitmap *target, const u8 *buffer) {
  Bitmap source;
  InitializeBitmap(&source, 160, 120);
  pt_bitmap_bind_source(&source, buffer, 160u * 120u);
  u32 result = BitBlt(&source, 0, 0, target, 32, 61, 160, 120);
  return result;
}
u32 CopyBmp6AToHere(const Bitmap *target) {
  const Bitmap *source = game_ptr->background;
  return BitBlt(source, 0, 0, target, 12, 14, 192, 176);
}
u32 CopyFromBackgroundBitmap(const Bitmap *target, u16 x, u16 y, u16 right, u16 bottom) {
  const Bitmap *source = game_ptr->background;
  return BitBlt(source, x, y, target, (u16)(x + 12u), (u16)(y + 14u), (u16)(right + 1u - x), (u16)(bottom + 1u - y));
}
u8 GetBezColor(u8 mode, u8 x, u8 y) {
  u8 value = loaded_bez_file[y][x];
  return owner_to_color[mode ? value >> 4 : value & 15u];
}
void DrawMapBegin(void) {
  Canvas_SetPattern(temp_canvas, district_map_pattern, 3);
  temp_canvas->background_color = 0;
  temp_canvas->text_background_enabled = 1;
}
void DrawMapEnd(void) {
  Canvas_SetPattern(temp_canvas, NULL, 0);
  temp_canvas->text_background_enabled = 0;
}
void DrawMapZoomLevel1(u8 mode) {
  DrawMapBegin();
  for (u8 y = 0; y < 15; ++y)
    for (u8 x = 0; x < 20; ++x)
      FillRect(
          GetBezColor(mode, x, y), (u16)(x * 8u + 32u), (u16)(y * 8u + 61u), (u16)(x * 8u + 39u), (u16)(y * 8u + 68u));
  DrawMapEnd();
}
void DrawMapZoomLevel2(u8 mode) {
  DrawMapBegin();
  for (u8 y = 0; y < 30; ++y)
    for (u8 x = 0; x < 40; ++x)
      FillRect(GetBezColor(mode, (u8)((map_scroll_x + x) >> 3), (u8)((map_scroll_y + y) >> 3)),
          (u16)(x * 4u + 32u),
          (u16)(y * 4u + 61u),
          (u16)(x * 4u + 35u),
          (u16)(y * 4u + 64u));
  DrawMapEnd();
}
// Callers interpret the low 16 bits as signed.

i16 GetMapTabX(u8 tab) {
  u16 offset = (u16)((2u - tab) * map_tab_step);
  return (i16)(offset + map_tab_base + (u16)(20u * (u16)map_tabs_order[tab]));
}
void DrawMapTab(i16 x, i16 y, u8 color, const u8 *text) {
  FillRect(color, (u16)x, (u16)y, (u16)(x + 50), (u16)(y + 10));
  temp_canvas->color = 247;
  Canvas_DrawPixel(temp_canvas, (u16)x, (u16)y);
  SetTempCanvasPosition((i16)((u16)x - 1u), y);
  RelativeLineTo(0, 6);
  SetTempCanvasPosition(x, (i16)((u16)y - 1u));
  temp_canvas->color = 247;
  RelativeLineTo(48, 0);
  SetTempCanvasPosition((i16)((u16)x + 4u), (i16)((u16)y + 7u));
  temp_canvas->color = 0;
  SetCurrentFont(some_font_ptr);
  DrawListElement(text);
}
void DrawMapTabs(void) {
  // Tab colors in back-to-front order.

  static const u8 colors[3] = {246, 249, 251};
  CopyFromBackgroundBitmap(current_bitmap, 0, 0, 190, 44);
  for (u8 i = 0; i < 3; ++i) {
    u8 tab = (u8)(2u - i);
    DrawMapTab(GetMapTabX(tab), tabs_y_values[i], colors[i], pt_map_tab_label(map_tabs_order[tab]));
  }
}

// map ui

// Installs the fixed pair, rather than swapping the retained current/old pointers. The
// inactive branch preserves did_set_bitmap.

Game *InitGraphicsMode(void) {
  game_ptr->swapped_when_setting_bitmap = 0;
  game_ptr->old_bitmap_index = 1;
  current_bitmap = display_bitmaps[0];
  canvas_1->bitmap = current_bitmap;
  temp_canvas->bitmap = current_bitmap;
  old_bitmap = display_bitmaps[1];
  if (game_ptr->current_bitmap_index) {
    Copy320x200Bitmap(display_bitmaps[1] ? display_bitmaps[1] : NULL, display_bitmaps[0] ? display_bitmaps[0] : NULL);

    WaitAndSetPalette(pt_game_palette_slot(game_ptr, 0));

    game_ptr->current_bitmap_index = 0;
    game_ptr->did_set_bitmap = 0;
  }
  return game_ptr;
}

// Icon columns count supported shapes, rather than physical slots.

PTActionResult Do3x3ButtonStuff(u8 menu) {
  game_ptr->some_button_menu = game_ptr->current_button_menu;
  game_ptr->current_button_menu = menu;
  if (menu == UINT8_MAX) {
    game_ptr->current_button_menu = menu;

    return pt_action_object(game_ptr);
  }
  HandleMouseOver3x3Buttons();
  canvas_1->color = 17;
  Canvas_FillRect(canvas_1, 228, 118, 310, 194);
  if (!menu) {
    CopySomePredefinedRect1(228, 118, 0);
    CopySomePredefinedRect1(256, 118, 0);
    CopySomePredefinedRect1(284, 118, 0);
    BitBlt(current_bitmap, 228, 118, current_bitmap, 228, 144, 82, 25);
    BitBlt(current_bitmap, 228, 118, current_bitmap, 228, 170, 82, 25);
  }
  u8 icon = 0;
  for (unsigned slot = 0; slot < 9; ++slot) {
    u8 shape = g_button_config[menu][slot];
    if (shape && shape <= 3) {
      switch (shape) {
      case 1:
        CopySomePredefinedRect1(button_x[slot][0], button_y[slot][0], 0);
        break;
      case 2:
        CopySomePredefinedRect2(button_x[slot][0], button_y[slot][0], 0);
        break;
      case 3:
        CopySomePredefinedRect3(button_x[slot][0], button_y[slot][0], 0);
        break;
      }
      Do3x3ButtonStuff_Helper(button_icon_ids[menu][icon],
          (u16)(button_x[slot][0] + (shape == 2 ? 21u : 6u)),
          (u16)(button_y[slot][0] + (shape == 3 ? 19u : 7u)));
      ++icon;
    }
  }
  u32 result = DisplayStuffAfterPaymentInner();

  return pt_action_value(result);
}

// map markers

u16 CheckPoiAttrEquals(const Poi *poi, u8 attribute, u8 value) {
  u8 field;
  if (attribute < 2)
    field = poi->current_tenant;
  else if (attribute == 2)
    field = poi->property_type;
  else if (attribute == 3)
    field = poi->for_sale_or_rent_flags;
  else {
    abort();
  }
  return field == value;
}
static void draw_marker(unsigned x, unsigned y, bool coverage) {
  u16 left, top, right, bottom;
  if (game_ptr->map_zoom_level) {
    left = (u16)(4 * ((int)x - map_scroll_x) + 32);
    top = (u16)(4 * ((int)y - map_scroll_y) + 61);
    if (!coverage && ((i16)left < 32 || (i16)left >= 192 || (i16)top < 61 || (i16)top >= 181))
      return;
    right = (u16)(left + 3u);
    bottom = (u16)(top + 3u);
    if (map_marker_visible)
      FillRect(coverage ? 15 : 16, left, top, right, bottom);
    else
      BitBltOldToCurrent(left, top, right, bottom);
  } else {
    left = (u16)(x + 31);
    top = (u16)(y + 60);
    right = (u16)(x + 32);
    bottom = (u16)(y + 61);
    if (map_marker_visible) {
      temp_canvas->color = (u8)(coverage ? 15 : 16);
      Canvas_DrawPixel(temp_canvas, left, top);
      Canvas_DrawPixel(temp_canvas, right, top);
      Canvas_DrawPixel(temp_canvas, left, bottom);
      Canvas_DrawPixel(temp_canvas, right, bottom);
    } else
      BitBltOldToCurrent(left, top, right, bottom);
  }
}
void DrawMapMarkers(void) {
  unsigned endx = 160, endy = 120;
  if (game_ptr->map_zoom_level) {
    endx = map_scroll_x + 40u;
    endy = map_scroll_y + 30u;
  }
  if (++map_marker_phase < 8)
    return;
  map_marker_phase = 0;
  map_marker_visible ^= 1;
  if (selected_poi_attr == 1) {
    for (unsigned y = map_scroll_y; y < endy; ++y)
      for (unsigned x = map_scroll_x; x < endx; ++x) {
        PTForwardLink *node = players[selected_poi_player].restaurants.begin.next;
        while (node->next) {
          const Rest *rest = ((Rest *)node);
          const Poi *poi = rest->property;
          unsigned distance = (unsigned)(abs((int)x - poi->xcoord) + abs((int)y - poi->ycoord));
          if (distance <= rest->restaurant_range)
            draw_marker(x, y, true);
          node = rest->linked.next;
        }
      }
  } else {
    PTForwardLink *node = current_town->poi.begin.next;
    while (node->next) {
      const Poi *poi = ((Poi *)node);
      if (CheckPoiAttrEquals(poi, selected_poi_attr, selected_poi_player))
        draw_marker(poi->xcoord, poi->ycoord, false);
      node = poi->link.next;
    }
  }
}

// map controls

// The selected slot narrows to a byte before moving the remaining words.

u32 HandleClickOnMapTabs(u8 tab) {
  u8 selected = (u8)map_tabs_order[tab];
  u32 changed = 0;
  if (tab == 1) {
    map_tabs_order[1] = map_tabs_order[0];
    changed = 1;
  }
  if (tab == 2) {
    map_tabs_order[2] = map_tabs_order[1];
    map_tabs_order[1] = map_tabs_order[0];
    changed = 1;
  }
  map_tabs_order[0] = selected;
  return changed;
}

// The first matching tab wins, including overlapping/wrapped rectangles.

int GetClickedMapTab(void) {
  for (u8 tab = 0; tab < 3; ++tab) {
    i16 x = GetMapTabX(tab);
    i16 y = tabs_y_values[2u - tab];
    i16 right = (i16)((u16)x + 40u);
    i16 bottom = (i16)((u16)y + 8u);
    if (IsMouseInside(x, y, right, bottom))
      return tab;
  }
  return -1;
}

// Tabs take priority over the four toolbar filter categories.

u32 HandleClickOnMapControls(void) {
  u32 result = 0;
  int tab = GetClickedMapTab();
  if (tab >= 0) {
    u32 changed;
    changed = HandleClickOnMapTabs((u8)tab);
    if (changed)
      DrawMapTabs();
    result = 1;
    goto done;
  }
  if (mouse_y < 32) {
    u8 category = (u8)(((i32)mouse_x - 12) / 48), count;
    // Original bug: an out-of-column click reads an uninitialized row
    // count and indexes past ingredient_popup_x. The labelled two-byte
    // repair sends that branch to the original zero-return epilogue.

    if (category > 3) {
      goto done;
    }
    const u8 *items[PT_POPUP_MENU_CAPACITY];
    if (category < 2) {
      count = 8;
      for (unsigned i = 0; i < count; ++i)
        items[i] = players[i].name_text;
    } else {
      count = category == 2 ? 11 : 4;
      for (unsigned i = 0; i < count; ++i)
        items[i] = category == 2 ? property_names[i] : property_availability_names[i];
    }
    ExtractMapPopupMenuItems(items, count);
    u8 selected;
    selected = DisplayMapPopupMenu(ingredient_popup_x[category], 30, count);
    if ((i8)selected > -1) {
      selected_poi_attr = category;
      selected_poi_player = selected;
      result = 1;
    }
  }
done:
  return result;
}

// map zoom

// Is a tail jump and ignores the caller's argument.

void WaitForRetrace_(void) {
  WaitForRetrace();
}

u8 HandleMapZoomLevel_Helper2(i16 x, i16 y, i16 width, i16 height) {
  temp_canvas->position_x = x;
  temp_canvas->position_y = y;
  u16 right = (u16)((u16)x + (u16)width - 1u);
  u16 bottom = (u16)((u16)y + (u16)height - 1u);
  Canvas_LineTo(temp_canvas, right, (u16)y);
  Canvas_LineTo(temp_canvas, right, bottom);
  Canvas_LineTo(temp_canvas, (u16)x, bottom);
  return Canvas_LineTo(temp_canvas, (u16)x, (u16)y);
}

// . Keep the high halves of both incoming dwords when the next Mouse sample replaces only
// AX/BX, then use unsigned x86 shifts.

u32 HandleMapZoomLevel_Helper(u32 x, u32 y) {
  u16 width = 40, height = 30;
  if (game_ptr->map_zoom_level == 1) {
    width = 48;
    height = 44;
  }
  SaveAndSetInputEventFilter(5);
  temp_canvas->text_background_enabled = 0;
  temp_canvas->color = 16;

  HideMouseCursor_();
  bool selected = false;
  for (;;) {
    i16 left = (i16)x, top = (i16)y;
    if (left < 32 || top < 61 || left > 192 - width || top > 181 - height)
      break;
    u32 rect[8] = {(u16)x, (u16)y, (u16)(x + width), (u16)(y + height)};

    DrawFromCurrentToOld(rect[0], rect[1], rect[2], rect[3]);

    HandleMapZoomLevel_Helper2(left, top, (i16)width, (i16)height);
    u8 saved_x = (u8)x, saved_y = (u8)y;

    mouse_pressed = ReadMouseEvent();

    x = (x & 0xffff0000u) | (u16)mouse_x;
    y = (y & 0xffff0000u) | (u16)mouse_y;

    WaitForRetrace_();

    u32 restore[8] = {saved_x, saved_y, (u16)(saved_x + width), (u16)(saved_y + height)};

    BitBltOldToCurrent(restore[0], restore[1], restore[2], restore[3]);
    if (mouse_pressed == 1) {
      selected = true;
      break;
    }
    if (mouse_pressed == 2)
      break;
  }
  SaveAndSetInputEventFilter(3);

  GraphicsRestoreCursor2();
  u32 result = UINT16_MAX;
  if (selected) {
    if (game_ptr->map_zoom_level) {
      i32 dx = (i32)(i16)x - 32, dy = (i32)(i16)y - 61;
      // Floor division reproduces SAR even for negative coordinates.

      dx = dx >= 0 ? dx / 4 : -((-dx + 3) / 4);
      dy = dy >= 0 ? dy / 4 : -((-dy + 3) / 4);
      result = ((u32)dy + map_scroll_y) << 8;
      result |= (u32)dx + map_scroll_x;
    } else
      result = (x - 32u) | ((y - 61u) << 8);
  }

  return result;
}

// map scroll

// Each map byte selects a 4x4 atlas tile. Coordinate sums use promoted byte arguments.

u32 HandleMapZoomLevel2_Helper1(const Bitmap *target, u8 x, u8 y, u8 first, u8 count) {
  u32 end = (u32)first + count;
  for (u32 column = first; column < end; ++column) {
    u8 tile = pt_map_tile_read(x + column, y);
    BitBlt(bmp20_ptr, (tile & 240u) >> 2, (tile & 15u) * 4u, target, 4u * column, 0, 4, 4);
  }

  return end;
}
u32 HandleMapZoomLevel2_Helper2(const Bitmap *target, u8 x, u8 y, u8 first, u8 count) {
  u32 end = (u32)first + count;
  for (u32 row = first; row < end; ++row) {
    u8 tile = pt_map_tile_read(x, y + row);
    BitBlt(bmp20_ptr, (tile & 240u) >> 2, (tile & 15u) * 4u, target, 0, 4u * row, 4, 4);
  }

  return end;
}
// Use BL as a byte loop counter. A promoted endpoint above 255 cannot terminate; preflight
// stops that branch. The caller pushes three extra arguments which BitBlt never reads.

static u32 large_strip(const Bitmap *target, u8 x, u8 y, u8 first, u8 count, bool vertical) {
  u32 end = (u32)first + count;
  u8 i = first;
  while (i < end) {
    u8 tile = pt_map_tile_read(vertical ? x : x + i, vertical ? y + i : y);

    BitBlt(
        &bmp_awards, tile & 240u, (tile & 15u) * 16u, target, vertical ? 0 : 16u * i, vertical ? 16u * i : 0, 16, 16);

    ++i;
  }

  return vertical ? i : end;
}
u32 DrawMapHelper1(const Bitmap *target, u8 x, u8 y, u8 first, u8 count) {
  return large_strip(target, x, y, first, count, false);
}
u32 DrawMapHelper2(const Bitmap *target, u8 x, u8 y, u8 first, u8 count) {
  return large_strip(target, x, y, first, count, true);
}

// large map

// Both original sprintf calls write this shared filename without clearing its
// unused tail. The argument is a byte, promoted to the stock nonnegative int.

static void asset_name(u8 index, bool offsets, u8 name[19]) {
  const char *prefix = offsets ? "gfx\\offsets" : "gfx\\blitmap";
  size_t pos = strlen(prefix);
  memcpy(name, prefix, pos);
  if (index >= 100)
    name[pos++] = (u8)('0' + index / 100u);
  if (index >= 10)
    name[pos++] = (u8)('0' + index / 10u % 10u);
  name[pos++] = (u8)('0' + index % 10u);
  if (!offsets) {
    memcpy(name + pos, ".vga", 4);
    pos += 4;
  }
  name[pos] = 0;
}
// Reload both atlas resources before recording the selected index.

void LoadBlitmapAndOffsets(u8 index) {

  if (game_ptr->last_loaded_blitmap_index != index) {
    if (!index) {
      LoadBackground(land_symbol_filename, &bmp_awards);
    } else {
      u8 name[19];
      asset_name(index, false, name);
      memcpy(strbuf, name, strlen((const char *)name) + 1);
      LoadBackground(strbuf, &bmp_blitmap);
      asset_name(index, true, name);
      memcpy(strbuf, name, strlen((const char *)name) + 1);
      OpenFileAsCurrent(strbuf);
      ReadBytesFromCurrentFile(&blitmap_offsets, sizeof(blitmap_offsets));
      CloseCurrentFile();
    }
    game_ptr->last_loaded_blitmap_index = index;
  }
}

void SoundStuff4(void) {
  if (!state_slot_023 && (u8)prefs.field_0) {
    SoundStuff2(0);
    SetSoundFlag(50);
  }
}
void DrawMapRowZoomLevel3(const Bitmap *target, u8 x, u8 y, u16 destination_y, u32 source_y_offset, u16 height) {
  for (u8 column = 0; column < 12; ++column) {
    u8 tile = loaded_map_file[y][x + column];
    // The original shared descriptor is reused for landsym.vga.

    BitBlt(&bmp_awards,
        tile & 240u,
        (u16)((tile & 15u) * 16u + source_y_offset),
        target,
        12u + 16u * column,
        destination_y,
        16,
        height);
  }
}
// Rows extend beyond byte Y; the source-X offset wraps to a word.

u32 DrawMapColumnZoomLevel3(const Bitmap *target, u8 x, u8 y, u8 destination_x, u32 source_x_offset, u16 width) {

  u32 result = 0;
  for (u8 row = 0; row < 11; ++row) {
    u8 tile = pt_map_tile_read(x, (u32)y + row);

    result = BitBlt(&bmp_awards,
        (u16)((tile & 240u) + source_x_offset),
        (tile & 15u) * 16u,
        target,
        destination_x,
        14u + 16u * row,
        width,
        16);
  }

  return result;
}

// cursor visibility

static void apply_pending_motion(void) {
  if (mouse_move_pending == 1) {
    in_bitblt = 2;
    mouse_move_pending = 0;
    MoveMousePosition(pending_mouse_x, pending_mouse_y);
  }
}

// . The nesting word wraps exactly like INC word.

void HideMouseCursorInner(void) {
  u16 saved = in_bitblt;
  if (!in_bitblt)
    in_bitblt = 1;
  if (cursor_is_visible) {
    HideMouseCursor();
    cursor_is_visible = 0;
  }
  cursor_hide_depth = (u16)(cursor_hide_depth + 1u);
  apply_pending_motion();
  in_bitblt = saved;
}

// . Only a transition to depth zero restores pixels.

void GraphicsRestoreCursor(void) {
  u16 saved = in_bitblt;
  if (!in_bitblt)
    in_bitblt = 1;
  cursor_hide_depth = (u16)(cursor_hide_depth - 1u);
  if (!cursor_hide_depth && !cursor_is_visible) {
    RestoreMouseCursor();
    cursor_is_visible = 1;
  }
  apply_pending_motion();
  in_bitblt = saved;
}

Game *HideMouseCursor_(void) {
  if (game_ptr->mouse_cursor_visible) {
    HideMouseCursorInner();
    game_ptr->mouse_cursor_visible = 0;
  }
  return game_ptr;
}

Game *GraphicsRestoreCursor2(void) {
  if (!game_ptr->mouse_cursor_visible) {
    GraphicsRestoreCursor();
    game_ptr->mouse_cursor_visible = 1;
  }
  return game_ptr;
}

// Terrain.

#include "sound.h"

void DrawMapAnyZoom(u8 zoom_level, u8 map_x, u8 map_y) {
  SetSomeFieldInCanvas(0);
  if (zoom_level < 3) {
    curr_map_mode = (u8)map_tabs_order[0];

    FillLetterBackground(4, 21, 100);

    DrawMapTabs();
  }
  switch (zoom_level) {
  case 1:
    SoundStuff1();
    if (curr_map_mode) {
      DrawMapZoomLevel1((u8)(curr_map_mode - 1u));

      return;
    }
    if (did_load_map_and_bez == 1) {
      CopyBitmap2to1();
      SetCurrentBitmap(old_bitmap);
      SetCurrentFont(font_astandar);
      temp_canvas->color = 0;
      DrawText2(12, 203, 100, creating_map_text);
      // Reload old_bitmap after the setter's first swap. The second
      // setter restores the original pair, but changes both canvases.

      SetCurrentBitmap(old_bitmap);
      for (u16 y = 61; y < 181; ++y)
        for (u16 x = 32; x < 192; ++x) {
          u8 tile = loaded_map_file[y - 61][x - 32];
          temp_canvas->color = (u8)(terrain_tile_colors[16u * (tile & 15u) + (tile >> 4)]);
          Canvas_DrawPixel(temp_canvas, x, y);
        }
      CopyBitmapToBuffer(current_bitmap, buffer_backup);
      CopyBitmap2to1();
      did_load_map_and_bez = 0;
    } else {
      CopyBufferToBitmap(current_bitmap, buffer_backup);
    }
    SelectPalette(62);
    break;
  case 2:
    if ((u8)prefs.field_0)
      SoundStuff2(1);
    SetSoundFlag(16);
    if (curr_map_mode) {
      // Read map_scroll_x and map_scroll_y; the extra coordinates are unused.

      DrawMapZoomLevel2((u8)(curr_map_mode - 1u));

      return;
    }
    for (u8 y = 0; y < 30; ++y)
      for (u8 x = 0; x < 40; ++x) {
        u8 tile = loaded_map_file[map_y + y][map_x + x];
        BitBlt(bmp20_ptr, (tile & 240u) >> 2, (tile & 15u) * 4u, current_bitmap, 32u + 4u * x, 61u + 4u * y, 4, 4);
      }
    break;
  case 3:
    LoadBlitmapAndOffsets(0);
    SoundStuff4();
    SetSoundFlag(50);
    for (u8 row = 0; row < 11; ++row)
      DrawMapRowZoomLevel3(current_bitmap, map_x, (u8)(map_y + row), 14u + 16u * row, 0, 16);
    break;
  default:
    break;
  }
  state_slot_009 = 0;
  ClearInputEvents();
}

_Static_assert(sizeof(loaded_map_file) == 120u * 160u, "terrain map dimensions");
_Static_assert(sizeof(terrain_tile_colors) == 256u, "terrain color table size");
_Static_assert(sizeof(creating_map_text) == 16u, "creating-map literal size");

// Poi.

#include "arithmetic.h"
#include "messages.h"
#include "missions.h"
#include "restaurant.h"
#include "calendar.h"
#include "trade.h"
#include <fenv.h>
#include <math.h>

#pragma STDC FP_CONTRACT OFF

static float multiply32(float a, float b) {
  volatile float value = a * b;
  return value;
}
static float add32(float a, float b) {
  volatile float value = a + b;
  return value;
}
static float age_population(float population, u8 age, u8 tile) {
  u8 kind = tile & 15u;
  if (kind < 6)
    // Preserve the original non-arithmetic fast path.
    return population;
  float factor = customer_other_tile_factor;
  if (kind == 10)
    factor = customer_residential_tile_factor;
  else if (kind == 8 && (tile >> 4) >= 8)
    factor = pt_customer_population_factor((u8)((tile >> 4) - 8u), age);
  return multiply32(population, factor);
}

float HandleAgePub(float population, u8 age, u8 tile) {
  float result = age_population(population, age, tile);

  return result;
}
u32 GetSizeOfPoi(const Poi *poi) {
  return 100u * poi->size_x * poi->size_y;
}
size_t pt_poi_integer_text(u8 text[12], u32 bits) {
  u8 reverse[10];
  unsigned digits = 0, count = 0;
  u32 magnitude = bits;
  if (bits > INT32_MAX) {
    text[count++] = '-';
    magnitude = 0u - bits;
  }
  do {
    reverse[digits++] = (u8)('0' + magnitude % 10u);
    magnitude /= 10u;
  } while (magnitude);
  while (digits)
    text[count++] = reverse[--digits];
  text[count] = 0;
  return count;
}
u8 *itoa_(i32 value) {
  u8 text[12];
  size_t length = pt_poi_integer_text(text, (u32)value);
  ;
  memcpy(strbuf, text, length + 1u);

  return strbuf;
}
u32 DrawFlyerOutput(const u8 *label, u32 value) {
  ;
  Canvas *canvas = canvas_1;
  i16 x = canvas->position_x;

  const u8 *text = itoa_(pt_signed_dword(value));

  u16 number_length = (u16)pt_strlen(text), label_length = (u16)pt_strlen(label);

  Canvas_DrawText(canvas, pt_text_span(label, label_length));

  u16 width = Canvas_TextWidth(canvas, pt_text_span(text, number_length));

  canvas->position_x = (i16)(304u - width);

  Canvas_DrawText(canvas, pt_text_span(text, number_length));

  canvas->position_x = x;
  canvas->position_y = (i16)((u16)canvas->position_y + 6u);

  u32 result = DisplayStuffAfterPaymentInner();

  return result;
}

static u32 fist32(float value) {
  double integral = trunc((double)value);
  return !isfinite(integral) || integral < -0x1p31 || integral >= 0x1p31 ? UINT32_C(0x80000000) : (u32)(i32)integral;
}
static u32 fist64_low(float value) {
  double integral = trunc((double)value);
  return !isfinite(integral) || integral < -0x1p63 || integral >= 0x1p63 ? 0 : (u32)(u64)(i64)integral;
}
static bool customer_chance(u8 percent, u32 *seed, bool runtime) {
  if (runtime) {
    bool result = RandomChance(percent) != 0;

    return result;
  }
  if (!percent)
    return false;
  if (percent >= 100)
    return true;
  *seed = UINT32_C(1103515245) * (*seed) + UINT32_C(12345);
  return (100u * ((*seed >> 16) & 0x7fffu) >> 15) < percent;
}
// Read-only preflight and actual execution share arithmetic rounding points.
// Runtime writes remain in original order and sample the real guest RNG.

void pt_poi_count_customers(Poi *poi, PTCustomerCounts *plan, bool runtime) {
  u8 radius = 7, outer = 7, percent = 0;
  if (poi->restaurant) {
    Rest *restaurant = poi->restaurant;
    radius = 0;
    while ((radius + 1u) * (radius + 1u) <= restaurant->restaurant_range_squared)
      ++radius;
    outer = (u8)(radius + 1u);
    percent = (u8)(100u * (restaurant->restaurant_range_squared - radius * radius) / (outer * outer - radius * radius));
    if (runtime)
      restaurant->restaurant_range = radius;
  }
  plan->radius = radius;
  int x = poi->xcoord, y = poi->ycoord;
  u8 first_x = (u8)(x < outer ? 0 : x - outer), first_y = (u8)(y < outer ? 0 : y - outer);
  int last_x = x + outer, last_y = y + outer;
  if (last_x >= 160)
    last_x = 159;
  if (last_y >= 120)
    last_y = 119;
  float total = 0;
  for (u8 age = 0; age < 6; ++age) {
    Zone *zone = FindZoneFromCoordinates(poi->xcoord, poi->ycoord);

    plan->zone = zone;
    float population = multiply32(zone->population_distribution[age], customer_distribution_factor), sum = 0;
    for (u8 row = first_y; row <= last_y; ++row)
      for (u8 column = first_x; column <= last_x; ++column) {
        u8 distance = (u8)(abs(x - column) + abs(y - row)), limit = radius;
        if (distance == radius && customer_chance(percent, &plan->seed, runtime))
          ++limit;
        if (distance < limit) {
          float customers;
          if (runtime) {
            customers = HandleAgePub(population, age, pt_map_tile_read(column, row));

          } else
            customers = age_population(population, age, pt_map_tile_read(column, row));
          sum = add32(customers, sum);
        }
      }
    plan->groups[age] = (u16)fist32(sum);
    if (runtime)
      poi->customer_count_in_area[age] = (i16)plan->groups[age];
    total = add32(total, sum);
  }
  plan->total = fist64_low(total);
  if (runtime)
    poi->total_customer_count = pt_signed_dword(plan->total);
}
u32 UpdatePoiCustomerCount(Poi *poi) {
  PTCustomerCounts plan = {.seed = pt_random_state};
  pt_poi_count_customers(poi, &plan, true);

  return plan.total;
}

size_t pt_poi_size_text(u8 text[256], const Poi *poi, bool padded) {
  const char *prefix = padded ? "Size:      " : "Size: ";
  size_t length = strlen(prefix);
  memcpy(text, prefix, length);
  u8 digits[12];
  size_t n = pt_poi_integer_text(digits, poi->size_x);
  memcpy(text + length, digits, n);
  length += n;
  text[length++] = 'x';
  n = pt_poi_integer_text(digits, poi->size_y);
  memcpy(text + length, digits, n + 1u);
  return length + n;
}

static Canvas *info_text(const u8 *text) {
  Canvas *result = DrawInfoBoxText(text);

  return result;
}
static void info_size_text(const Poi *poi, bool padded) {
  u8 text[256];
  size_t length = pt_poi_size_text(text, poi, padded);

  memcpy(strbuf, text, length + 1u);

  info_text(strbuf);
}

void *HandleMapZoomLevel3_Helper6(Poi *poi) {
  const Rest *rest = poi->restaurant;
  u8 kind = poi->property_type;

  FillGenericInfoBox();

  if (kind >= sizeof(property_names) / sizeof(property_names[0]))
    pt_resource_bounds_failure("property name", kind, sizeof(property_names) / sizeof(property_names[0]));
  if (kind >= 8 && poi->current_tenant >= 8 &&
      poi->for_sale_or_rent_flags >= sizeof(property_availability_names) / sizeof(property_availability_names[0]))
    pt_resource_bounds_failure("property availability",
        poi->for_sale_or_rent_flags,
        sizeof(property_availability_names) / sizeof(property_availability_names[0]));
  info_text(property_names[kind]);
  if (kind < 8)
    info_text(pt_extra_town_name(poi->property_type, poi->current_owner));
  else {
    const u8 *name;
    if (poi->current_tenant >= 8)
      name = property_availability_names[poi->for_sale_or_rent_flags];
    else if (poi->restaurant) {
      name = GetRestaurantName(poi->restaurant);

    } else
      name = pt_player(poi->current_tenant)->name_text;
    Canvas *result = info_text(name);
    if (kind == 8) {
      if (!rest) {
        return result;
      }
      info_text(poi_info_formats + POI_SEPARATOR);
      info_size_text(poi, false);
      (void)DrawFlyerOutput(poi_info_formats + POI_OFFER, poi->restaurant->num_recipes);

      u32 guest_total = SumListOfWords(rest->customer_count, 6);

      (void)DrawFlyerOutput(poi_info_formats + POI_GUESTS, guest_total);
    } else if (kind == 9) {
      u32 capacity = GetSizeOfPoi(poi);

      (void)DrawFlyerOutput(poi_capacity_text, capacity);
    } else if (kind == 10) {
      info_text(poi_info_formats + POI_SEPARATOR);
      if (poi == last_customer_count_poi) {
        info_text(poi_info_formats + POI_CUSTOMERS);

        UpdatePoiCustomerCount(poi);

        for (u8 i = 0; i < 6; ++i)
          (void)DrawFlyerOutput(customer_age_names[i], (u16)poi->customer_count_in_area[i]);
        last_customer_count_poi = NULL;
        goto done;
      }
      info_size_text(poi, true);
      last_customer_count_poi = poi;
    }
    (void)DrawFlyerOutput(poi_info_formats + POI_PRICE, (u32)poi->purchase_price);
    (void)DrawFlyerOutput(poi_info_formats + POI_RENT, (u32)poi->rent_price);
    info_text(poi_realtor_text);
    info_text(pt_terminal_caption(current_town, 0, poi->realtor_index));
  }
done:;
  Game *result = game_ptr;
  game_ptr->info_box_state = 2;

  return result;
}

// Retains one saved register across restaurant entry.

u8 HandleClickOnRestaurant(Rest *restaurant) {
  ptr_apple->restaurant = restaurant;
  game_ptr->did_set_bitmap = 0;
  if (restaurant->player_index == current_player_index) {
    if (game_ptr->sound_enabled) {
      SoundStuff3(60, 0);

      // Third ignored sound argument is retained.
    }
    ptr_apple->view_state = 128;
    game_ptr->in_menu_hierarchy = 2;

    u8 result = EnterRestaurant(restaurant, 0);

    game_ptr->in_menu_hierarchy = 1;
    ptr_apple->view_state = 0;
    game_ptr->did_set_bitmap = 1;

    return result;
  }
  if (restaurant->restaurant_closed_for_days) {
    game_ptr->did_set_bitmap = 1;

    ShowMessage(15, closed_restaurant_text);

    return 2;
  }
  if (game_ptr->sound_enabled) {
    SoundStuff3(60, 0);
  }
  game_ptr->in_menu_hierarchy = 2;

  u8 result = EnterCompetitorRestaurant(restaurant);

  game_ptr->in_menu_hierarchy = 1;
  game_ptr->did_set_bitmap = 1;

  return result;
}

// Takes Y before X. It walks left through tile 0x14, including byte wrap, before looking up
// and selecting the property.

u8 HandleMapZoomLevel3_Helper5(u8 y, u8 x) {
  u8 kind;
  for (;;) {
    u8 tile = pt_map_tile_read(x, y);
    kind = tile & 15u;
    if (tile != 20)
      break;
    --x;
  }

  mouse_over_zone = FindZoneFromCoordinates(x, y);

  Poi *p = FindPoiFromCoords(x, y);

  Player *player = player_ptr;
  if (!p) {
    player->selected_property = NULL;

    ShowMessage(14, terrain_names[kind]);

    game_ptr->info_box_state = 0;

    return 0;
  }
  if (p != player->selected_property) {
    player->selected_property = p;

    HandleMapZoomLevel3_Helper6(p);

    return 0;
  }
  const Poi *poi = p;
  u8 result = 0;
  if (x == player->mission.target_x && y == player->mission.target_y) {
    game_ptr->in_menu_hierarchy = 0;

    result = HandleActiveMission(1);

    game_ptr->in_menu_hierarchy = 1;
    goto after_action;
  }
  if (poi->property_type == 9 && poi->current_tenant == current_player_index) {
    game_ptr->in_menu_hierarchy = 0;
    if (game_ptr->sound_enabled) {
      SoundStuff3(60, 0);
    }

    result = HandleCentralDepot(current_player_index);

    game_ptr->in_menu_hierarchy = 1;
  }
  if (poi->restaurant) {
    result = HandleClickOnRestaurant(poi->restaurant);

    if (result == 2) {
      return 0;
    }
  }
after_action:
  if (result) {
    if (!has_many_players) {
      const Poi *target = player_ptr->selected_property;
      i16 px = (i16)((int)target->xcoord - 6), py = (i16)((int)target->ycoord - 5);
      map_scroll_x = (u8)(px < 0 ? 0 : px > 148 ? 148 : px);
      map_scroll_y = (u8)(py < 0 ? 0 : py > 109 ? 109 : py);
    }

    PrintGenericInfoBox();

    PrintCurrentDateAndTime();

    DisplayStuffAfterPayment();

    return result;
  }

  HandleMapZoomLevel3_Helper6(p);

  return 0;
}

// Town assets.

#include <string.h>

size_t pt_town_map_asset_name(u8 town, bool bez, u8 text[16]) {
  const char *prefix = bez ? "dat\\bez" : "dat\\map";
  memcpy(text, prefix, 7);
  size_t n = 7;
  if (town >= 100)
    text[n++] = (u8)('0' + town / 100);
  if (town >= 10)
    text[n++] = (u8)('0' + town / 10 % 10);
  text[n++] = (u8)('0' + town % 10);
  memcpy(text + n, ".dat", 5);
  return n + 4;
}

size_t pt_town_city_asset_name(u8 town, u8 text[16]) {
  memcpy(text, "dat\\city", 8);
  size_t n = 8;
  if (town >= 100)
    text[n++] = (u8)('0' + town / 100);
  if (town >= 10)
    text[n++] = (u8)('0' + town / 10 % 10);
  text[n++] = (u8)('0' + town % 10);
  memcpy(text + n, ".pc", 4);
  return n + 3;
}

size_t pt_town_temp_asset_name(u8 town, u8 text[5]) {
  size_t n = 0;
  text[n++] = 'c';
  if (town >= 100)
    text[n++] = (u8)('0' + town / 100);
  if (town >= 10)
    text[n++] = (u8)('0' + town / 10 % 10);
  text[n++] = (u8)('0' + town % 10);
  text[n] = 0;
  return n;
}

// Town init.

#include "ai_day.h"
#include "savegame.h"
#include "character.h"
#include "state_types.h"
#pragma STDC FP_CONTRACT OFF

// Reads a signed BYTE, widens it to a WORD and uses FILD WORD. The high half of its DWORD
// scratch store is dead. Preserve 53-bit x87 division/subtraction rounding before truncation
// and the indefinite FISTP.

i32 GetTownPriceLevelStuff(void) {
  i16 price = (i8)ramtown.price_level_old;

  volatile double quotient = (double)price / market_share_price_divisor;
  volatile double projected = market_share_base_threshold - quotient;
  double integral = TruncateX87Integral(projected);
  u32 result =
      isfinite(integral) && integral >= INT32_MIN && integral <= INT32_MAX ? (u32)(i32)integral : UINT32_C(0x80000000);

  return pt_signed_dword(result);
}
// The threshold is reduced to its unsigned low WORD for every eight-tile cell. A strict
// unsigned greater-than retains the first restaurant on a tie.

void UpdateTownMarketShare(void) {
  u16 controlled_cells[8];
  const Rest *best_restaurant;
  u32 minimum_score;
  u8 tile_x, tile_y, sample_x, sample_y;
  u8 sample_x_offset, sample_y_offset, player_index;
  memset(controlled_cells, 0, sizeof(controlled_cells));
  i32 price = GetTownPriceLevelStuff();
  minimum_score = (u32)price << 6;
  for (tile_y = 0; tile_y < 15; ++tile_y) {
    for (tile_x = 0; tile_x < 20; ++tile_x) {
      best_restaurant = 0;
      u32 best = (u16)minimum_score;
      player_index = 0;

      while (player_index < 8) {
        const Player *player = &players[player_index];
        PTForwardLink *node = player->restaurants.begin.next;
        while (node->next) {
          const Rest *rest = (Rest *)node;
          const Poi *poi = rest->property;
          u32 sum = 0;
          sample_y_offset = 0;
          sample_y = 8 * tile_y;
          while (sample_y_offset < 8) {
            sample_x_offset = 0;
            sample_x = 8 * tile_x;
            while (sample_x_offset < 8) {
              i32 dx, dy;
              dx = abs((i32)poi->xcoord - sample_x);
              dy = abs((i32)poi->ycoord - sample_y);
              i32 score = RestaurantScaledCloseness(rest, (u16)rest->word1F, (u32)(dx + dy));
              sum += (u32)score;
              sample_x_offset += 2;
              sample_x += 2;
            }
            sample_y_offset += 2;
            sample_y += 2;
          }
          if (sum > best) {
            best = sum;
            best_restaurant = rest;
          }
          node = rest->linked.next;
        }
        ++player_index;
      }
      u8 owner;
      const Rest *restaurant = best_restaurant;
      if (restaurant) {
        owner = restaurant->player_index;
        ++controlled_cells[owner];
      } else {
        Zone *zone;
        zone = FindZoneFromCoordinates(8 * tile_x, 8 * tile_y);

        owner = (u8)(zone->active_mafia + 8u);
      }
      u8 *tile = &loaded_bez_file[tile_y][tile_x];
      *tile = (u8)((*tile & 15u) | (owner << 4));
    }
  }
  for (player_index = 0; player_index < 8; ++player_index) {
    unsigned player = player_index;
    savegame_sect_0.market_share[player][ptr_apple->town_index] = (u8)(100u * controlled_cells[player] / 300u);
  }
}
u8 InitLastDialedNumbers(void) {
  u8 player_index;
  u8 number = 0;
  for (player_index = 0; player_index < 8; ++player_index) {
    PlayerDialledNumbers *dial = &pt_player(player_index)->city_part;
    for (unsigned kind = 0; kind < 6; ++kind) {
      u8 count = current_town->num_phone_nr_of_kind[kind];
      dial->num_last_dialled_number[kind] = count;
      for (number = 0; number < count; ++number) {
        if (kind == 4 && number == 3) {
          dial->num_last_dialled_number[kind] = 3;
          break;
        }
        dial->last_dialled_number[kind][number] = number;
      }
    }
  }

  return number;
}
// Random range uses the low WORD, but the final ADD/SUB use the complete pushed DWORDs. These
// distinct widths are intentional.

u32 ComputePoiBaseSize(u32 base, u32 range) {
  u32 a, b;
  a = RandomNumber((u16)range);
  b = RandomNumber_((u16)range);

  return base + a + b - range;
}
void RandomizeAiRecipe(AiRecipe *recipe) {
  u32 value;
  value = RandomNumber(99);
  recipe->recipe_kind = (u8)(value + 1);
  recipe->default_rating = 0;
  value = RandomNumber(character_ptr->cooking);
  recipe->rating_randrange = (u8)(value + 30);
}
void InitAiRecipes(void) {
  u8 recipe_index;
  for (recipe_index = 0; recipe_index < 10; ++recipe_index) {
    AiRecipe *recipe = &player_ptr->ai_recipes[recipe_index];
    u8 randomize = RandomChance(60);
    if (randomize) {
      bool duplicate;
      do {
        RandomizeAiRecipe(recipe);
        duplicate = false;
        for (unsigned j = 0; j < recipe_index; ++j)
          if (player_ptr->ai_recipes[j].recipe_kind == recipe->recipe_kind) {
            duplicate = true;
            break;
          }
      } while (duplicate);
      u32 rating = RandomNumber(recipe->rating_randrange);
      recipe->default_rating = (u8)rating;
    }
  }
  for (recipe_index = 0; recipe_index < 4; ++recipe_index)
    AiSetDefaultRecipeRating();
}
// The original reuses its outer attempt BYTE for the eight-player scan.
// A successful chance therefore ends the attempt loop at nine. Distances
// are calculated and discarded; they never prevent construction.

void CreateDefaultAiRestaurants(void) {
  u8 previous_player, attempt;
  previous_player = current_player_index;
  for (current_player_index = game_ptr->num_interactive_players; current_player_index < 8; ++current_player_index) {
    SetCurrentPlayer(current_player_index);
    pt_player(current_player_index)->byte24D = 3;
    for (attempt = 0; attempt < 8; ++attempt) {
      u8 chance = RandomChance(character_ptr->experience);
      if (chance) {
        Poi *selected;
        selected = GetRandomPoiOfKind(2);

        for (attempt = 0; attempt < 8; ++attempt) {
          unsigned p = attempt;
          if (p < game_ptr->num_interactive_players)
            continue;
          PTForwardLink *node = pt_player(p)->restaurants.begin.next;

          while (true) {
            if (!node->next)
              break;

            const Rest *rest = (Rest *)node;

            (void)GetDistanceBetweenPoi(selected, rest->property);
            node = rest->linked.next;
          }
        }
        (void)AiCreateNewRestaurant(selected, current_player_index, 0);
      }
    }
    InitAiRecipes();
  }
  SetCurrentPlayer(previous_player);
}
void InitializePoiOwner(Poi *poi) {
  u8 kind = poi->property_type;

  if (kind >= sizeof(extra_town_data) / sizeof(extra_town_data[0]))
    pt_resource_bounds_failure("extra town category", kind, sizeof(extra_town_data) / sizeof(extra_town_data[0]));

  if (!*pt_extra_town_name(kind, array_size_8[kind]))
    array_size_8[kind] = 0;
  poi->current_owner = array_size_8[kind]++;
}
Poi *AllocatePoi(Town *town) {
  Poi *poi;

  poi = pt_simulation_allocate(PT_SIM_POI, sizeof(Poi));
  if (poi) {
    (void)LinkList_InsertFirst(&town->poi, &poi->link);
    town->num_poi = (i16)((u32)town->num_poi + 1u);
  }

  return poi;
}

void InitializePoi(Poi *poi, u8 x, u8 y) {
  u8 tile_owner, tile_kind;
  Zone *zone = FindZoneFromCoordinates(x, y);
  poi->xcoord = x;
  poi->ycoord = y;
  poi->current_owner = 100;
  poi->current_tenant = 8;
  u8 tile = loaded_map_file[y][x];
  tile_owner = tile >> 4;
  tile_kind = tile & 15u;
  if ((tile & 15u) == 10 || (tile & 15u) == 4) {
    poi->map_value = tile;
    poi->restaurant = NULL;
    u32 size = ComputePoiBaseSize(zone->base_size, zone->rand_size);
    poi->size_x = (u8)size;
    poi->size_y = (u8)(poi->size_x - RandomNumber(zone->rand_size));
    if (poi->size_x < 10)
      poi->size_x = 10;
    if (poi->size_y < 10)
      poi->size_y = 10;
    if (poi->size_x > 20)
      poi->size_x = 20;
    if (poi->size_y > 20)
      poi->size_y = 20;
    size = ComputePoiBaseSize((u16)(zone->base_size - 3u), zone->rand_size);
    poi->kitchen_width = (u8)size;
    if (poi->kitchen_width < 12)
      poi->kitchen_width = 12;
    poi->kitchen_height = (u8)(poi->size_x - RandomNumber(4));
    poi->byte1A = (u8)RandomNumber(poi->size_x - 4u);
    poi->purchase_price = (u32)poi->size_x * poi->size_y * zone->base_price_level;
    poi->rent_price = (u32)poi->size_x * poi->size_y * zone->base_rent_level;
    poi->door_x_start = (u8)(RandomNumber(poi->size_y - 4u) + 1u);
    poi->left_door_y_end = (u8)RandomNumber(poi->kitchen_height - 6u);
    poi->for_sale_or_rent_flags = 0;
  }
  u8 kind = tile_kind;
  if (kind == 8) {
    poi->property_type = (u8)(tile_owner - 8u);
    InitializePoiOwner(poi);
  } else if (kind == 4 || kind == 10) {
    poi->realtor_index = (u8)RandomNumber(current_town->num_phone_nr_of_kind[0]);
    poi->current_owner = (u8)(poi->realtor_index + 8u);
    zone = FindZoneFromCoordinates(poi->xcoord, poi->ycoord);
    if (RandomChance(zone->premises_availability))
      poi->for_sale_or_rent_flags = (u8)(RandomNumber(3) + 1u);
    if (kind == 10) {
      poi->property_type = 10;
      if (tile_owner < 8)
        (void)AiCreateNewRestaurant(poi, tile_owner, 0);
    } else {
      poi->property_type = 9;
      poi->purchase_price = pt_signed_dword((u32)poi->purchase_price * 5u);
      poi->rent_price = pt_signed_dword((u32)poi->rent_price * 5u);
    }
  }
}
static u32 copy(const Bitmap *source, i16 sx, i16 sy, const Bitmap *target, i16 dx, i16 dy, i16 width, i16 height) {
  return BitBlt(source, sx, sy, target, dx, dy, width, height);
}
u8 DrawCountryInfoBoxBg(i16 left, i16 top, i16 right, i16 bottom) {
  (void)copy(current_bitmap,
      left,
      top,
      game_ptr->background,
      0,
      0,
      (i16)((u16)(right - left + 1)),
      (i16)((u16)(bottom - top + 1)));
  FillRect(0, (i16)(left), (i16)(top), (i16)(right), (i16)(bottom));
  FillRect(24, (i16)((u16)(left + 1)), (i16)((u16)(top + 1)), (i16)((u16)(right - 2)), (i16)((u16)(bottom - 2)));
  temp_canvas->color = 23;
  i16 y = (i16)((u16)(bottom - 1));
  SetTempCanvasPosition(left, y);
  u8 result;
  result = DefaultLineTo(left, top);
  i16 x = (i16)((u16)(right - 1));
  result = DefaultLineTo(x, top);

  return result;
}
u32 DrawGameBitmap(i16 left, i16 top, i16 right, i16 bottom) {
  return copy(game_ptr->background,
      0,
      0,
      current_bitmap,
      left,
      top,
      (i16)((u16)(right - left + 1)),
      (i16)((u16)(bottom - top + 1)));
}
void InitializeTown(void) {
  u8 map_y, map_x;
  if (!disable_init_country_progress_msg) {
    (void)DrawCountryInfoBoxBg(90, 77, 230, 120);
    temp_canvas->color = 20;
    (void)DrawText2(90, 230, 90, town_initializing_caption);
    (void)DrawText2(90, 230, 102, pt_town_name(ptr_apple->town_index));
    (void)DrawText2(90, 230, 114, town_initializing_ellipsis);
  }
  memset(&ramtown, 0, sizeof(ramtown));
  memset(g_staff, 0, sizeof(g_staff));
  memset(pt_player(current_player_index)->profit_last_30_days,
      0,
      sizeof(pt_player(current_player_index)->profit_last_30_days));
  memset(&pt_player(current_player_index)->city_part, 0, sizeof(PlayerDialledNumbers));
  pt_player(current_player_index)->num_days_passed_in_town[ptr_apple->town_index] = ptr_apple->weekday;
  ramtown.price_level_new = 0;
  ramtown.mongo_var_3 = 0;
  ramtown.economy_history_index = 0;
  ramtown.inflation = 3;
  ramtown.loan_interest_extra = 8;
  ramtown.pending_transfer_account_2 = pt_signed_dword(RandomNumber(99999998u) + 1u);
  (void)OpenFileAsCurrent(staff_market_filename);

  (void)ReadBytesFromCurrentFile(&max_num_characters, 1);
  (void)ReadBytesFromCurrentFile(&staff_is_type, max_num_characters);

  CloseCurrentFile();
  (void)memmove(&staff_is_type[100], &staff_is_type, 100);
  InitPapayaArrays();

  for (unsigned i = 0; i < max_num_characters; ++i) {
    if (RandomChance(50)) {
      staff_is_type[i] = (u8)(staff_is_type[i] + 3u);
      u8 type = staff_is_type[i];
      // IDA's apparent temp_rest tail access is physically papaya_array.
      // Keep the named three-byte counter, never index outside Rest.

      if (++papaya_array[type - 3] < 10)
        staff_is_type[i] -= 3;
    }
  }
  for (unsigned i = max_num_characters; i < 2u * max_num_characters; ++i)
    staff_is_type[i] += 3;
  for (unsigned i = 200; i < 256; ++i)
    staff_is_type[i] = 255;
  for (unsigned i = 0; i < 8; ++i)
    staff_is_type[pt_player(i)->character_index] = (u8)(i + 16u);
  UpdateTownMarketShare();
  current_town->num_poi = 0;
  for (map_y = 0; map_y < 120; ++map_y) {
    for (map_x = 0; map_x < 160; ++map_x) {
      u8 x = map_x, y = map_y, tile = loaded_map_file[y][x];
      if (((tile & 15u) == 8 && (tile & 128u)) || tile == 4 || (tile & 15u) == 10) {
        Poi *poi;
        poi = AllocatePoi(current_town);
        if (poi)
          InitializePoi(poi, x, y);
      }
    }
  }
  pt_player(current_player_index)->selected_property = NULL;
  PTForwardLink *node = current_town->suppliers.begin.next;

  while (true) {
    if (!node->next)
      break;

    Supplier *supplier = (Supplier *)node;

    for (unsigned i = 0; i < num_ingredients_in_set[supplier->stock_kind]; ++i)
      supplier->num_items_left[i] = (u16)(supplier->price_addon[i] ? RandomNumber(5000) + 8000u : 0);
    node = supplier->link.next;
  }
  if (ramtown.economy_history_index < 10)
    for (unsigned i = 0; i < 10; ++i)
      DoMongoStuff();
  for (unsigned i = 0; i < 96; ++i) {
    u16 valid = IsValidIngredient(i);
    if (!valid)
      continue;
    if (RandomChance(5))
      current_town->ingredient_trend[i] = (u8)(RandomNumber(40) - 20u);
    if (RandomChance(8)) {
      u8 target = (u8)(RandomNumber(60) - 30u);
      SetIngredientTrends(i, 0, target, 1);
    }
  }
  CreateDefaultAiRestaurants();
  for (unsigned i = 0; i < 9; ++i) {
    u8 reach = (u8)(RandomNumber(30) + 30u);
    ramtown.poster_reach[i] = reach;
    ramtown.poster_cost[i] = (i16)((u16)(40u * (reach + RandomNumber(25))));
    reach = (u8)(RandomNumber(50) + 50u);
    ramtown.tv_ad_reach[i] = reach;
    ramtown.tv_ad_dollars[i] = (i16)((u16)(100u * (reach + RandomNumber(20))));
  }
  // Stock leaves the final 56 bytes intact.
  memset(staff_hates_player, 0, 200);
  (void)InitLastDialedNumbers();
  if (!disable_init_country_progress_msg)
    (void)DrawGameBitmap(90, 77, 230, 120);
}

// Town teardown.

void FreeRecipe(const Recipe *recipe) {
  u32 size = pt_recipe_allocation_bytes(recipe->num_entries);
  pt_simulation_release(recipe, size);
}
Town *FreeLotsOfStuff(void) {
  FreeAllRecipes();
  FreePoiContractsRestaurants();
  return TownFreeSuppliersAndZones();
}
void FreeAllRecipes(void) {
  u8 player = 0;
  while (player < 8) {
    Player *owner = pt_player(player);
    if (player < game_ptr->num_interactive_players)
      while (owner->num_recipes) {
        const Recipe *recipe = (Recipe *)owner->recipes.begin.next;
        LinkList_RemoveFirst(&owner->recipes);
        FreeRecipe(recipe);
        owner->num_recipes -= 1;
      }
    ++player;
  }
  for (unsigned i = 0; i < 164; ++i)
    furniture_desc[i].amount_in_van = 0;
}
void FreeRestaurant(Rest *restaurant, u8 update_city_count) {
  Poi *poi = restaurant->property;
  poi->map_value = restaurant->original_map_value;
  u8 map_value = restaurant->original_map_value;
  loaded_map_file[poi->ycoord][poi->xcoord] = map_value;
  poi->current_tenant = 8;
  poi->current_owner = (u8)(poi->realtor_index + 8u);
  poi->property_type = 10;
  poi->restaurant = NULL;
  LinkList_Remove(&restaurant->linked);
  if (update_city_count)
    player_ptr->num_restaurants_in_city[ptr_apple->town_index] =
        (u8)((u32)(player_ptr->num_restaurants_in_city[ptr_apple->town_index]) - (u32)1u);
  --ramtown.num_restaurant_per_player[restaurant->player_index];
  pt_player(restaurant->player_index)->num_restaurants =
      (u8)((u32)(pt_player(restaurant->player_index)->num_restaurants) - 1u);
  u32 size = restaurant->player_index < game_ptr->num_interactive_players ? PT_HUMAN_RESTAURANT_ALLOCATION_SIZE
                                                                          : PT_AI_RESTAURANT_SIZE;
  pt_simulation_release(restaurant, size);
}
void FreePoiContractsRestaurants(void) {
  u8 player = 0;
  while (player < 8) {
    Player *owner = pt_player(player);
    if (player < game_ptr->num_interactive_players)
      while (owner->num_contracts) {
        Contract *contract = (Contract *)owner->contracts.begin.next;
        LinkList_RemoveFirst(&owner->contracts);
        pt_simulation_release(contract, sizeof(*contract));
        owner->num_contracts = (u8)((u32)owner->num_contracts - 1u);
        --ramtown.num_contracts_per_player[player];
      }
    while (owner->num_restaurants) {
      Rest *restaurant = (Rest *)owner->restaurants.begin.next;
      const Poi *poi = restaurant->property;
      loaded_map_file[poi->ycoord][poi->xcoord] = restaurant->original_map_value;
      FreeRestaurant(restaurant, 0);
    }
    ++player;
  }
  // The selected lifetime repair keeps each POI alive until every restaurant
  // has restored its map/property fields and released its reference.

  Town *town = current_town;
  while ((u16)town->num_poi) {
    const Poi *poi = (Poi *)town->poi.begin.next;
    LinkList_RemoveFirst(&town->poi);
    pt_simulation_release(poi, sizeof(*poi));
    town->num_poi = (i16)((u32)town->num_poi - 1u);
  }
}
Town *TownFreeSuppliersAndZones(void) {
  for (unsigned list = 0; list < 2; ++list) {
    Town *town = current_town;
    u8 *count = list ? &town->num_zones : &town->num_supplier;
    LinkedHead *head = list ? &town->zones : &town->suppliers;
    while (*count) {
      const PTForwardLink *record = head->begin.next;
      LinkList_RemoveFirst(head);
      pt_simulation_release(record, list ? sizeof(Zone) : sizeof(Supplier));
      *count = (u8)(*count - 1u);
    }
  }
  return current_town;
}

// City simulation.

#include "reports.h"

// city calendar

// Resolve each field after the preceding store: an aliased saved field
// can change which town owns the next write.

void SetIngredientTrends(u8 index, u8 initial, u8 target, u8 mode) {
  u8 *current = pt_town_current_trend(index);
  (*current) = initial;

  u8 *target_field = pt_town_target_trend(index);
  (*target_field) = target;

  u8 *notice = pt_town_trend_notice(index);
  (*notice) = (u8)(mode | 128u);
}
// Saves two registers and two WORD temporaries in DWORD slots. Price levels are signed BYTE
// inputs, signed WORD intermediates and BYTE outputs. History index 60 appends through the
// trailing byte before memcpy.

void DoMongoStuff(void) {
  i16 old = (i8)ramtown.price_level_old, target = (i8)ramtown.price_level_new;
  u8 counter = ramtown.mongo_var_3;

  u32 random;
  if (!counter) {
    random = RandomNumber(240);
    target = (i16)((u16)(random - 120u));

    random = RandomNumber(40);
    counter = (u8)random;
  }
  int direction = target - old >= 0 ? 1 : -1;
  random = RandomNumber(5);
  i16 updated = (i16)(old + direction * (i32)random);

  (*pt_economy_history_entry(ramtown.economy_history_index)) = (u8)updated;
  --counter;
  if (ramtown.economy_history_index <= 59)
    ++ramtown.economy_history_index;
  else {
    memmove(pt_economy_history_window(0, sizeof(ramtown.economy_history)),
        pt_economy_history_window(1, sizeof(ramtown.economy_history)),
        sizeof(ramtown.economy_history));
  }
  ramtown.price_level_old = (u8)updated;
  ramtown.price_level_new = (u8)target;
  ramtown.mongo_var_3 = counter;
}

void RunCityNewMonth(void) {
  u32 account;
  account = RandomNumber(99999998);
  ramtown.pending_transfer_account_2 = pt_signed_dword(account + 1u);
}
void RunCityNewWeek(void) {
  DoMongoStuff();
  for (unsigned index = 0; index < 96; ++index) {
    u16 valid = IsValidIngredient((u8)index);
    if (!valid)
      continue;
    u8 change = RandomChance(3);
    if (!change)
      continue;
    // Final trend argument already pushed.
    u32 random = RandomNumber(60);
    SetIngredientTrends((u8)index, 0, (u8)(random - 30u), 1);
  }
}

// city new day

// AI executes its real controller; unported AI children stop verification.

void RunCityNewDay(void) {
  if (!ptr_apple->weekday) {
    RunCityNewWeek();
  }
  if (ptr_apple->day == 1) {
    RunCityNewMonth();
  }
  u8 city = ptr_apple->town_index;
  if (!did_run_town_day_loop_3[city]) {
    did_run_town_day_loop_3[city] = 1;
    AiRunTownDayLoop();
  }
  if (ptr_apple->day == 20) {
    u32 zone = RandomNumber(current_town->num_zones);
    ramtown.next_inspection_zone_id = (u8)zone;
  }
  ComputeRecipeRankings();
  if (ramtown.gap137[0] == ptr_apple->day) {
    AddPeachEntry(0, 54, 3, 0, 20);
    ramtown.gap137[0] = 0;
  }
  DoSomeYaffaStuff();

  u8 day = ptr_apple->day;
  if (day >= ramtown.mayor_available_day) {
    u8 change = RandomChance(10);
    if (change) {
      u32 random = RandomNumber(10);
      u8 next = (u8)(day + ptr_apple->day + random);
      if (next > 30)
        next = 0;
      ramtown.mayor_available_day = next;
    }
  }
}

// town day

void RunTownIngredientDailyLoop(u8 ingredient) {
  u8 *flags = &ramtown.stockarr56[ingredient], mode = *flags & 127u;
  if (mode && mode <= 3) {
    Town *town = current_town;
    int target = town->stockarr90A[ingredient], trend = town->ingredient_trend[ingredient];
    if (mode == 3) {
      town->stockarr90A[ingredient] = (i8)((u8)(target - 1));
      if (!town->stockarr90A[ingredient]) {
        town->ingredient_trend[ingredient] = 0;
        *flags = 0;
      }
    } else if (target) {
      int direction = target < 0 ? -1 : 1;
      if (*flags & 128u) {
        int difference = target - trend;
        // SAR rounds negative odd differences towards minus infinity.

        int half = difference < 0 ? -((-difference + 1) / 2) : difference / 2;
        town->ingredient_trend[ingredient] = (i8)((u8)(trend + (mode == 2 ? half : 4 * direction)));

        int distance = abs(target - town->ingredient_trend[ingredient]);

        int threshold = abs(target / 10);

        if (distance <= threshold)
          *flags &= 127u;
      } else {
        town->ingredient_trend[ingredient] = (i8)((u8)(trend - 4 * direction));

        int distance = abs(town->ingredient_trend[ingredient]);

        if (distance < 4) {
          *flags = 0;
          town->stockarr90A[ingredient] = 0;
          town->ingredient_trend[ingredient] = 0;
        }
      }
    }
  }
}
u32 DecreasePlayerFoodQualities(void) {
  // Stock visits precisely three entries, despite the eight-byte array.

  for (unsigned i = 0; i < 3; ++i)
    if (ramtown.food_quality_by_player[i])
      --ramtown.food_quality_by_player[i];
  return 2;
}
void InitPapayaArrays(void) {
  for (unsigned i = 0; i < 3; ++i)
    papaya_array[i] = 0;
  for (unsigned i = 0; i < 200; ++i)
    if (staff_is_type[i] < 3)
      ++papaya_array[staff_is_type[i]];
}
void RunTownWeeklyLoop(void) {
  u8 index;
  index = 0;
  while (index < 4) {
    unsigned i = index;
    if (ramtown.arrF7[i])
      --ramtown.arrF7[i];
    ++index;
  }
  if (ramtown.byteFC)
    --ramtown.byteFC;
  else
    ramtown.base_decrease_ingredient_quality = 1;

  InitPapayaArrays();

  index = 0;
  while (index < 200) {
    unsigned i = index, type = staff_is_type[i];
    if (type < 6) {
      unsigned category = type < 3 ? type : type - 3;
      int threshold = (type < 3 ? 16 : 26) - 2 * ptr_apple->difficulty_level;
      if (type < 3 ? papaya_array[category] > threshold : papaya_array[category] < threshold) {
        u8 change = RandomChance(30);

        if (change) {
          if (type < 3) {
            staff_is_type[i] += 3;
            --papaya_array[category];
          } else {
            staff_is_type[i] -= 3;
            ++papaya_array[category];
          }
        }
      }
    }
    ++index;
  }

  SupplierReduceBoughtByPlayer();
}
void RunTownDailyLoop(void) {
  u8 index;
  if (ptr_apple->day == ramtown.some_phonecall_day) {
    // Stock adds in AL before zero-extending the phone reason.

    u8 reason = (u8)(ramtown.type_of_competition + 10u);

    HandlePhoneCall(3, 0, end_turn_phone_text, reason);

    ramtown.some_phonecall_day = 0;

    FillSomeRectWithColor(0);
  }

  DecreasePlayerFoodQualities();

  u8 kind = (u8)RandomNumber(4);

  RefillSupplierItems(kind);

  RunSuppliersDailyLoop();

  index = 0;
  while (index < 96) {
    u8 i = index;

    u16 valid = IsValidIngredient(i);

    if (valid) {
      u8 change = RandomChance(50);

      if (change) {
        RunTownIngredientDailyLoop(i);
      }
    }
    ++index;
  }
  index = 0;
  while (index < 6) {
    for (unsigned i = 0; i < 10; ++i) {
      u8 *value = &ramtown.number_has_no_connection[index][i];
      if (*value)
        --*value;
    }
    ++index;
  }
  if (ptr_apple->weekday == 6) {
    RunTownWeeklyLoop();
  }
  if (ptr_apple->day >= days_in_month[ptr_apple->month]) {
    RunTownMonthlyLoop();
  }
}
u16 GetPoiPropType(const Poi *poi) {
  return (u16)(poi->property_type & 248u);
}
void RunTownMonthlyLoop(void) {
  for (Poi *poi = pt_town_first_property(); poi; poi = pt_property_next(poi)) {
    u16 type = GetPoiPropType(poi);

    if (!type)
      continue;

    Zone *zone = FindZoneFromCoordinates(poi->xcoord, poi->ycoord);

    u8 probability = zone->premises_availability;

    u8 change = RandomChance(probability);

    if (!change)
      continue;
    if (poi->for_sale_or_rent_flags)
      poi->for_sale_or_rent_flags = 0;
    else if (poi->current_tenant == 8) {
      poi->for_sale_or_rent_flags = (u8)(RandomNumber(3) + 1u);
    }
  }
  if (ptr_apple->month % 3u == 0) {
    u8 competition = 0;

    u8 change = RandomChance(30);

    if (change)
      competition = 2;

    change = RandomChance(20);

    if (change)
      competition = 4;

    change = RandomChance(20);

    if (change)
      competition = 5;
    ramtown.gap137[2] = competition;
    ramtown.type_of_competition = competition;

    ramtown.gap137[0] = (u8)(RandomNumber(5) + 1u);

    ramtown.some_phonecall_day = (u8)(RandomNumber(10) + 10u);

    ramtown.pending_transfer_account_1 = pt_signed_dword(RandomNumber(999999));
  }
}

// recipe rankings

static void score_entry(TempRecipeEntry *entry, u8 player, u8 kind, const i16 ratings[7]) {
  entry->player_index = player;
  entry->recipe_kind = kind;
  u32 score = SumListOfWords(ratings, 7);
  entry->score = pt_signed_dword(score);
  entry->in_rank_table = 0;
  // Stock preserves field_3 from the allocator payload.
}
void ComputeRecipeRankings(void) {
  u32 total = 0;
  for (unsigned player = 0; player < 8; ++player) {
    if (player < game_ptr->num_interactive_players)
      total += players[player].num_recipes;
    else {
      for (unsigned i = 0; i < 10; ++i) {
        if (players[player].ai_recipes[i].recipe_kind)
          ++total;
      }
    }
  }
  // The original allocation includes 160 spare bytes.

  u32 size = (u32)sizeof(TempRecipeEntry) * total + 160u;
  TempRecipeEntry *entries;
  entries = (TempRecipeEntry *)AllocateBufferOrMessage(size);
  TempRecipeEntry *entry = entries;
  for (unsigned player = 0; player < 8; ++player) {
    if (player < game_ptr->num_interactive_players) {
      for (PTForwardLink *node = players[player].recipes.begin.next; node->next; node = node->next) {
        const Recipe *recipe = (const Recipe *)node;
        score_entry(entry, (u8)player, recipe->id, recipe->rating);
        ++entry;
      }
    } else {
      for (unsigned i = 0; i < 10; ++i) {
        const AiRecipe *recipe = &players[player].ai_recipes[i];
        if (recipe->recipe_kind) {
          if (player >= 8)
            RecipeRankingsNoOp();
          score_entry(entry, (u8)player, recipe->recipe_kind, recipe->rating);
          ++entry;
        }
      }
    }
  }
  RecipeRankEntry previous[10];
  memcpy(previous, ramtown.recipe_rank, sizeof(previous));
  memset(ramtown.recipe_rank, 0, sizeof(ramtown.recipe_rank));
  for (unsigned rank = 0; rank < (total < 10 ? total : 10); ++rank) {
    unsigned i = 0;
    while (entries[i].in_rank_table) {
      ++i;
    }
    unsigned best = i;
    for (; i < total; ++i) {
      if (!entries[i].in_rank_table && (u32)entries[i].score > (u32)entries[best].score)
        best = i;
    }
    entries[best].in_rank_table = (u8)(rank + 1u);
    RecipeRankEntry *out = &ramtown.recipe_rank[rank];
    out->recipe_kind = entries[best].recipe_kind;
    out->player_index = entries[best].player_index;
    out->last_position = 0;
    for (unsigned old = 0; old < 10; ++old) {
      const RecipeRankEntry *prior = &previous[old];
      if (prior->player_index == out->player_index && prior->recipe_kind == out->recipe_kind)
        out->last_position = (u8)(old + 1u);
    }
  }
  pt_buffer_release(entries);
}
void RecipeRankingsNoOp(void) {}

// City events.

static void trend(const EventTrend *t) {
  SetIngredientTrends(t->index, t->initial, t->target, t->mode);
}
// Event 50 tests the walking guest's low X byte and clears counter zero,
// matching the original overlapping globals without reading a byte window.

u8 DoYaffaThing(u8 event) {
  if (event > 50)
    pt_resource_bounds_failure("city event", event, 51);
  u8 counter = event == 50 ? (u8)walking_guest.x : yaffa_counter[event];
  u8 result = (u8)(event + 50u), delay = 0;
  if (counter == 255) {
    return 0;
  }
  if (event == 50)
    yaffa_counter[0] = 0;
  else
    yaffa_value[event] = 0;
  newspaper_delay = 4;
  const EventTrend *t = trend_for(event);
  if (t)
    trend(t);
  switch (event) {
  case 1:
    delay = (u8)(RandomNumber(10) + 10u);

    break;
  case 6: {
    u8 season = GetArstidIndex();
    if (season > 1) {
      return 0;
    }
    ramtown.base_decrease_ingredient_quality = 8;
    ramtown.byteFC = 3;
    break;
  }
  case 8:
    ramtown.arrF7[0] = 12;
    break;
  case 9:
    ramtown.arrF7[1] = 4;
    break;
  case 10:
    ramtown.arrF7[2] = 8;
    break;
  case 11:
    ramtown.arrF7[3] = 12;
    break;
  case 12: {
    u8 extra = (u8)(RandomNumber(5) + 2u);
    ramtown.loan_interest_extra = (u8)(ramtown.loan_interest_extra + extra);
    ramtown.gapF3[0] = 24;
    ramtown.gapF3[1] = (u8)(0u - extra);
    break;
  }
  case 13: {
    u8 extra = (u8)(RandomNumber(5) + 2u);
    if (extra >= ramtown.loan_interest_extra)
      extra = ramtown.loan_interest_extra;
    ramtown.loan_interest_extra = (u8)(ramtown.loan_interest_extra - extra);
    ramtown.gapF3[0] = 24;
    ramtown.gapF3[1] = extra;
    break;
  }
  case 16:
    ramtown.gapF3[3] = 2;
    ramtown.gapF3[2] = 30;
    break;
  case 17:
    ramtown.gapF3[3] = 255;
    ramtown.gapF3[2] = 30;
    break;
  case 49:
    pt_age_taste(0)->fat = 8;
    pt_age_taste(0)->sweet = 4;
    pt_age_taste(0)->salty = 7;
    pt_age_taste(0)->hot = 9;
    break;
  case 50:
    pt_age_taste(5)->fat = 6;
    pt_age_taste(5)->sweet = 9;
    pt_age_taste(5)->salty = 6;
    pt_age_taste(5)->hot = 7;
    break;
  default:
    break;
  }
  if (event == 1) {
    yaffa_value[1] = 3;
    yaffa_counter[1] = delay;
  }
  if (result)
    newpaper_rand = (u8)(event + 50u);

  return result;
}
// Keep the pending trend arguments unchanged across subsequent random draws.

u8 DoSomeMoreYaffaThing(void) {
  u32 random = RandomNumber(96);

  u8 ingredient = (u8)random;
  u16 valid = IsValidIngredient(ingredient);
  if (!valid) {
    return 0;
  }
  int value = current_town->ingredient_trend[ingredient];
  // Stock abs receives a signed BYTE, so INT_MIN is unreachable.

  int magnitude = value < 0 ? -value : value;
  if (magnitude > 5) {
    return 0;
  }
  newspaper_ingredient = ingredient;
  newspaper_random_mode = (u8)RandomNumber(4);
  newspaper_random_direction = RandomChance(50) ? 1 : 255;
  int direction = (i8)newspaper_random_direction;
  u8 initial = 0, target = 0, mode = 2;
  switch (newspaper_random_mode) {
  case 0:
    target = (u8)(direction * (i32)(RandomNumber(40) + 20u));
    break;
  case 1:
    mode = 1;
    target = (u8)(direction * (i32)(RandomNumber(40) + 20u));
    initial = (u8)(direction * (i32)RandomNumber(20));
    break;
  case 2:
    mode = 3;
    target = (u8)(RandomNumber(20) + 10u);
    initial = (u8)(direction * (i32)(RandomNumber(20) + 20u));
    break;
  case 3:
    target = (u8)(direction * (i32)(RandomNumber(30) + 30u));
    break;
  default:
    break;
  }
  SetIngredientTrends(newspaper_ingredient, initial, target, mode);
  newpaper_rand = 200;

  return 1;
}
// Early success stops the counter scan and preserves all later counters and RNG draws.

void DoSomeYaffaStuff(void) {
  if (newspaper_delay)
    --newspaper_delay;
  for (unsigned i = 0; i < 50; ++i) {
    if (!yaffa_counter[i])
      continue;
    --yaffa_counter[i];
    if (yaffa_counter[i] || !yaffa_value[i])
      continue;
    u8 success = DoYaffaThing(yaffa_value[i]);
    if (success) {
      return;
    }
  }
  if (RandomChance(5)) {
    u8 event = (u8)(RandomNumber(50) + 1u), success;
    success = DoYaffaThing(event);
    if (success) {
      return;
    }
  }
  if (RandomChance(18)) {
    u8 success = DoSomeMoreYaffaThing();
    if (success) {
      return;
    }
  }
  if (!newspaper_delay)
    newpaper_rand = 0;
  if (!newpaper_rand) {
    newpaper_rand = (u8)(RandomNumber(50) + 1u);
    newspaper_delay = 10;
  }
}
