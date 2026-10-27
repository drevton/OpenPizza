// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
// Menu.

#include "objects.h"
#include "graphics.h"
#include "menu.h"

u32 CopyBitmapPredefinedRect(u8 index, u16 x, u16 y) {
  const SpriteRect *rect = &smallrects[index];
  return BitBlt(&bmp17,
      rect->left,
      rect->top,
      current_bitmap,
      x,
      y,
      (u16)(rect->right + 1u - rect->left),
      (u16)(rect->bottom + 1u - rect->top));
}
u32 CopySomePredefinedRect1(u16 x, u16 y, u16 pressed) {
  return CopyBitmapPredefinedRect((u8)(10u + (pressed != 0)), x, y);
}
u32 CopySomePredefinedRect2(u16 x, u16 y, u16 pressed) {
  return CopyBitmapPredefinedRect((u8)(6u + (pressed != 0)), x, y);
}
u32 CopySomePredefinedRect3(u16 x, u16 y, u16 pressed) {
  return CopyBitmapPredefinedRect((u8)(8u + (pressed != 0)), x, y);
}
void Do3x3ButtonStuff_Helper(u8 icon, u16 x, u16 y) {
  u16 sx = (u16)((icon & 15u) * 16u), sy = (u16)((icon >> 4) * 13u);
  BitBlt(bmp16_ptr, sx, sy, current_bitmap, x, y, 16, 13);
  if (game_ptr->did_set_bitmap) {
    const Bitmap *alternate = display_bitmaps[game_ptr->current_bitmap_index ^ 1u];
    BitBlt(bmp16_ptr, sx, sy, alternate, x, y, 16, 13);
  }
}
void Do3x3ButtonStuff_Helper2(u8 icon, u8 kind, u16 x, u16 y, u16 pressed) {
  switch (kind) {
  case 1:
    CopySomePredefinedRect1(x, y, pressed);
    break;
  case 2:
    CopySomePredefinedRect2(x, y, pressed);
    break;
  case 3:
    CopySomePredefinedRect3(x, y, pressed);
    break;
  default:
    return;
  }
  Do3x3ButtonStuff_Helper(icon, (u16)(x + (kind == 2 ? 21u : 6u)), (u16)(y + (kind == 3 ? 19u : 7u) - pressed));
}

// Popup menu.

#include "world.h"
#include "text.h"
#include "input.h"
#include <stdlib.h>

void ExtractMapPopupMenuItems(const u8 *const items[], u8 count) {
  if (count > sizeof(popup_menu_items) / sizeof(popup_menu_items[0]))
    abort();
  for (unsigned i = 0; i < count; ++i) {
    popup_menu_items[i] = items[i];
  }
  // The original full menu puts its terminator in the adjacent furniture
  // value. Name that field explicitly instead of indexing outside the array.

  if (count == sizeof(popup_menu_items) / sizeof(popup_menu_items[0]))
    value_of_crashed_furniture_1 = 0;
  else
    popup_menu_items[count] = NULL;
}
void HandleDepotWeapons_DrawShade(i16 x, i16 y, i16 width) {
  static const u8 colors[] = {9, 11, 12, 13, 14, 15, 14, 12, 10, 8};
  i16 right = (i16)((u16)x + (u16)width - 1u);
  for (unsigned row = 0; row < 10; ++row) {
    i16 line_y = (i16)((u16)y + row);
    Canvas_SetPosition(temp_canvas, (u16)x, (u16)line_y);
    Canvas_SetColor(temp_canvas, colors[row]);
    Canvas_LineTo(temp_canvas, (u16)right, (u16)line_y);
  }
}
u8 DisplayMapPopupMenu_Helper(i16 x, i16 y, u32 unused, u8 count) {
  (void)unused;
  u8 previous = 255, current;
  u16 spacing = game_ptr->list_element_spacing;
  SetSomeFieldInCanvas(1);
  SaveAndSetInputEventFilter(5);
  do {
    bool inside = IsMouseInside(x, y, (i16)((u16)x + 96u), (i16)((u16)y + spacing * count));
    current = inside ? (u8)(((i32)mouse_y - y) / (i32)spacing) : 255;
    if (previous != current) {
      u8 indices[] = {previous, current}, colors[] = {13, 12};
      for (unsigned i = 0; i < 2; ++i)
        if ((i8)indices[i] >= 0) {
          if (indices[i] >= sizeof(popup_menu_items) / sizeof(popup_menu_items[0]))
            abort();
          Canvas_SetFieldE(temp_canvas, colors[i]);
          (void)SetTempCanvasPosition((i16)((u16)x + 3u), (i16)((u16)y + spacing * (indices[i] + 1u)));
          DrawListElement(popup_menu_items[indices[i]]);
        }
      previous = current;
    }
    u16 event = WaitForMouseClick();
    mouse_pressed = event == 1;
  } while (!mouse_pressed);
  SaveAndSetInputEventFilter(1);
  SetSomeFieldInCanvas(0);
  return current;
}
static void copy_row(i16 x, i16 source_y, i16 target_y) {
  BitBlt320Width(current_bitmap, (u16)x, (u16)source_y, current_bitmap, (u16)x, (u16)target_y, 96, 10);
}

u8 DisplayMapPopupMenu(i16 x, i16 y, u8 count) {
  SetCurrentFont(some_font_ptr);
  u8 spacing = game_ptr->list_element_spacing;
  SetSomeFieldInCanvas(0);
  CopyBitmap2to1();
  HandleDepotWeapons_DrawShade(x, (i16)((u16)y - 10u), 96);
  FillRect(13, (u16)x, (u16)y, (u16)(i16)((u16)x + 95u), (u16)(i16)((u16)y + 1u));
  for (unsigned i = 0; i < count; ++i) {
    for (unsigned row = 0; row < spacing; ++row) {
      u32 row_offset = spacing * i;
      copy_row(x, (i16)((u16)y - 10u), (i16)((u16)y + row_offset + row + 2u));
      temp_canvas->color = 13;
      (void)SetTempCanvasPosition(x, (i16)((u16)y + row_offset + row + 1u));
      RelativeLineTo(95, 0);
    }
    temp_canvas->color = 0;
    (void)SetTempCanvasPosition((i16)((u16)x + 3u), (i16)((u16)y + spacing * (i + 1u)));
    DrawListElement(popup_menu_items[i]);
  }
  u8 result = DisplayMapPopupMenu_Helper(x, y, 96, count);
  i32 remaining = spacing * count;
  i16 right = (i16)((u16)x + 96u), bottom = (i16)((u16)y + 10u), source_y = (i16)((u16)y - 10u);
  while (remaining > -1) {
    copy_row(x, source_y, (i16)((u16)y + (u32)remaining - 1u));
    i16 line = (i16)((u16)bottom + (u32)remaining);
    (void)BitBltOldToCurrent((u16)(i16)(x), (u16)(i16)(line), (u16)(i16)(right), (u16)(i16)(line));
    --remaining;
  }
  (void)BitBltOldToCurrent((u16)(i16)(x), (u16)(i16)(source_y), (u16)(i16)(right), (u16)(i16)(bottom));
  return result;
}

// Confirmation.

#include "frame.h"
#include "character.h"

i16 pt_dialog_word_offset(i16 v, i32 n) {
  return (i16)((u16)v + (u16)n);
}
u32 pt_dialog_coordinate(i16 v) {
  return (u32)(i32)v;
}

// . The 92 bytes are the original saved registers and locals.

u16 DrawPushButton(const u8 *text, i16 x, i16 y, i16 pressed) {
  SetTempCanvasPosition(x, y);

  SetCurrentFont(font_astandar);

  i16 x1 = pt_dialog_word_offset(x, 1), x2 = pt_dialog_word_offset(x, 2), x33 = pt_dialog_word_offset(x, 33),
      x34 = pt_dialog_word_offset(x, 34), x35 = pt_dialog_word_offset(x, 35);
  i16 y1 = pt_dialog_word_offset(y, 1), y2 = pt_dialog_word_offset(y, 2), y15 = pt_dialog_word_offset(y, 15),
      y16 = pt_dialog_word_offset(y, 16), y17 = pt_dialog_word_offset(y, 17);
  temp_canvas->color = (u8)(pressed == 1 ? 13 : 14);
  Canvas_FillRect(temp_canvas,
      pt_dialog_coordinate(x2),
      pt_dialog_coordinate(y2),
      pt_dialog_coordinate(x33),
      pt_dialog_coordinate(y15));
  temp_canvas->color = (u8)(pressed == 1 ? 10 : 15);
  Canvas_SetPosition(temp_canvas, x34, y);
  Canvas_LineTo(temp_canvas, (u16)x, (u16)y);
  Canvas_LineTo(temp_canvas, (u16)x, (u16)y17);
  Canvas_SetPosition(temp_canvas, x1, y16);
  Canvas_LineTo(temp_canvas, (u16)x1, (u16)y1);
  Canvas_LineTo(temp_canvas, (u16)x33, (u16)y1);
  temp_canvas->color = (u8)(pressed == 1 ? 15 : 10);
  Canvas_SetPosition(temp_canvas, x1, y17);
  Canvas_LineTo(temp_canvas, (u16)x35, (u16)y17);
  Canvas_LineTo(temp_canvas, (u16)x35, (u16)y);
  Canvas_SetPosition(temp_canvas, x34, y1);
  Canvas_LineTo(temp_canvas, (u16)x34, (u16)y16);
  Canvas_LineTo(temp_canvas, (u16)x2, (u16)y16);

  temp_canvas->color = 9;

  u16 result = DrawText2(x, pt_dialog_word_offset(x, 36), pt_dialog_word_offset(y, 12 - (i32)pressed), text);

  return result;
}

// . The first argument is forwarded to Canvas_DrawRect, which deliberately ignores it. All
// five edges wrap at the original short.

Game *AskForPizzaPriceHelper(const void *unused_canvas, i16 left, i16 top, i16 right, i16 bottom) {
  (void)unused_canvas;
  static const u8 colors[5] = {13, 14, 15, 13, 11};
  for (i32 i = 1; i <= 5; ++i) {
    Canvas_DrawRect(colors[i - 1],
        (u16)pt_dialog_word_offset(left, -i),
        (u16)pt_dialog_word_offset(top, -i),
        (u16)pt_dialog_word_offset(right, i),
        (u16)pt_dialog_word_offset(bottom, i));
  }
  game_ptr->paragraph_first_indent_height = 50;
  return game_ptr;
}
// . SetCurrentFont also chooses the primary line spacing.

Game *ResetListAttrParameters(void) {
  SetCurrentFont(font_astandar);

  game_ptr->paragraph_background_enabled = 1;
  game_ptr->list_element_spacing_2 = 0;
  game_ptr->paragraph_first_indent_height = 60;
  game_ptr->paragraph_indent_y_threshold = 0;
  return game_ptr;
}

// Input waits suspend the retained worker; the command API records each sample.

u8 ShowMessage2(i16 portrait, u8 *text) {
  ClearInputEvents();

  bool hidden = !game_ptr->mouse_cursor_visible;
  if (hidden) {
    GraphicsRestoreCursor2();
  }

  BackupCanvasAttr();

  game_ptr->disable_staff_disasters = (u8)((u32)game_ptr->disable_staff_disasters + 1u);
  game_ptr->list_element_spacing_2 = 1;
  did_terminal_stuff = 1;

  SaveAndSetInputEventFilter(3);

  Copy320x200Bitmap(display_bitmaps[0], display_bitmaps[1]);

  SetCurrentBitmap(old_bitmap);

  temp_canvas->color = 14;
  Canvas_FillRect(temp_canvas,
      pt_dialog_coordinate(18),
      pt_dialog_coordinate(25),
      pt_dialog_coordinate(198),
      pt_dialog_coordinate(35));
  if (portrait)
    Canvas_FillRect(temp_canvas,
        pt_dialog_coordinate(18),
        pt_dialog_coordinate(25),
        pt_dialog_coordinate(78),
        pt_dialog_coordinate(101));

  ResetListAttrParameters();

  u16 left = portrait ? 76 : 20, right = portrait ? 200 : 203;
  u16 y = Canvas_TextRect(temp_canvas, 10, 14, text, left, 20, right, 35);

  u16 bottom = portrait && (i32)y - 25 < 55 ? 111 : (u16)(y + 30u);

  FillRect(14, 18, (u16)(y - 6u), 198, bottom);

  if (portrait) {
    PaintCharacterFace((u8)portrait, 0, 18, 25);
  }
  i16 dialog_button_y = (i16)(bottom - 22u);
  const u8 *labels[2] = {no_text, yes_text};
  for (unsigned i = 0; i < 2; ++i) {
    unsigned choice = 1 - i;
    i16 x = choice ? 28 : 152;

    DrawPushButton(labels[choice], x, dialog_button_y, 0);
  }

  AskForPizzaPriceHelper(temp_canvas, 18, 25, 198, (i16)bottom);

  CommitBitmapAndSetPalette((i8)game_ptr->current_palette);

  u8 choice = 2;
  do {
    WaitForMouseClick2();

    bool yes = IsMouseInside(28, (u16)dialog_button_y, 64, (u16)pt_dialog_word_offset(dialog_button_y, 18)) != 0;

    if (yes)
      choice = 1;

    bool no = IsMouseInside(152, (u16)dialog_button_y, 188, (u16)pt_dialog_word_offset(dialog_button_y, 18)) != 0;

    if (no)
      choice = 0;
  } while (choice == 2);
  i16 x = choice ? 28 : 152;

  DrawPushButton(labels[choice], x, dialog_button_y, 1);

  WaitForMouseLeave();

  SetCurrentBitmap(old_bitmap);

  CommitBitmapAndSetPalette(0);

  Copy320x200Bitmap(current_bitmap, old_bitmap);

  RestoreCanvasAttrs();

  game_ptr->disable_staff_disasters = (u8)((u32)game_ptr->disable_staff_disasters - 1u);

  SaveAndSetInputEventFilter(game_ptr->dwordF9);

  if (hidden) {
    HideMouseCursor_();
  }

  return choice;
}

// . Unlike ShowMessage2, stock preserves the queued input and redraws its OK button after
// release. The empty string has no side effects.

void DisplayErrorMessage(i16 portrait, u8 *text) {
  if (!*text) {
    return;
  }
  bool hidden = !game_ptr->mouse_cursor_visible;
  if (hidden) {
    GraphicsRestoreCursor2();
  }

  BackupCanvasAttr();

  game_ptr->disable_staff_disasters = (u8)((u32)game_ptr->disable_staff_disasters + 1u);
  game_ptr->list_element_spacing_2 = 1;
  did_terminal_stuff = 1;

  SaveAndSetInputEventFilter(3);

  Copy320x200Bitmap(current_bitmap, old_bitmap);

  SetCurrentBitmap(old_bitmap);

  ResetListAttrParameters();

  temp_canvas->color = 14;
  Canvas_FillRect(temp_canvas,
      pt_dialog_coordinate(18),
      pt_dialog_coordinate(25),
      pt_dialog_coordinate(197),
      pt_dialog_coordinate(35));
  if (portrait)
    Canvas_FillRect(temp_canvas,
        pt_dialog_coordinate(18),
        pt_dialog_coordinate(25),
        pt_dialog_coordinate(78),
        pt_dialog_coordinate(101));

  u16 y = Canvas_TextRect(temp_canvas, 10, 14, text, portrait ? 76 : 20, 20, portrait ? 203 : 202, 35);

  u16 bottom = portrait && (i32)y - 25 < 55 ? 95 : (u16)(y + 30u);

  FillRect(14, 18, pt_dialog_coordinate((i16)(y - 6u)), 197, pt_dialog_coordinate((i16)bottom));

  if (portrait) {
    PaintCharacterFace((u8)portrait, 0, 18, 25);
  }
  i16 dialog_button_y = (i16)(bottom - 22u);

  DrawPushButton(ok_text, 89, dialog_button_y, 0);

  AskForPizzaPriceHelper(temp_canvas, 18, 25, 197, (i16)bottom);

  CommitBitmapAndSetPalette((i8)game_ptr->current_palette);

  bool inside;
  do {
    do {
      mouse_pressed = 0;

      mouse_pressed = ReadMouseEvent();

    } while (mouse_pressed != 1);

    inside = IsMouseInside(89, (u16)dialog_button_y, 125, (u16)pt_dialog_word_offset(dialog_button_y, 18)) != 0;

  } while (!inside);

  DrawPushButton(ok_text, 89, dialog_button_y, 1);

  WaitForMouseLeave();

  DrawPushButton(ok_text, 89, dialog_button_y, 0);

  SetCurrentBitmap(old_bitmap);

  CommitBitmapAndSetPalette(0);

  Copy320x200Bitmap(current_bitmap, old_bitmap);

  RestoreCanvasAttrs();

  game_ptr->disable_staff_disasters = (u8)((u32)game_ptr->disable_staff_disasters - 1u);

  SaveAndSetInputEventFilter(game_ptr->dwordF9);

  if (hidden) {
    HideMouseCursor_();
  }
}

// Validate all drawing in a helper before its first mutation. The host
// locals below project descriptors only; neither guest state is refreshed.

// Number query.

#include "messages.h"
#include "restaurant.h"
#include <stdio.h>
#include <string.h>

static i16 add(i16 x, unsigned n) {
  return (i16)((u16)x + n);
}
static u32 extended(i16 x) {
  return (u32)(i32)x;
}

// Draw one raised or pressed query control.

void ShowAskForQuery_Helper(u8 icon, i16 x, i16 y, i16 pressed) {
  SetCurrentFont(font_astandar);
  i16 x1 = add(x, 1), x2 = add(x, 2), x12 = add(x, 12), x13 = add(x, 13), x14 = add(x, 14);
  i16 y1 = add(y, 1), y13 = add(y, 13), y14 = add(y, 14);
  if (pressed == 1)
    FillRect(13, (u16)x, (u16)y, (u16)x14, (u16)y14);
  else {
    temp_canvas->color = 14;
    Canvas_FillRect(temp_canvas, extended(x), extended(y), extended(x14), extended(y14));
  }
  temp_canvas->color = pressed == 1 ? 10 : 15;
  Canvas_SetPosition(temp_canvas, x13, y);
  Canvas_LineTo(temp_canvas, (u16)x, (u16)y);
  Canvas_LineTo(temp_canvas, (u16)x, (u16)y14);
  Canvas_SetPosition(temp_canvas, x1, y13);
  Canvas_LineTo(temp_canvas, (u16)x1, (u16)y1);
  Canvas_LineTo(temp_canvas, (u16)x12, (u16)y1);
  temp_canvas->color = pressed == 1 ? 15 : 10;
  Canvas_SetPosition(temp_canvas, x1, y14);
  Canvas_LineTo(temp_canvas, (u16)x14, (u16)y14);
  Canvas_LineTo(temp_canvas, (u16)x14, (u16)y);
  Canvas_SetPosition(temp_canvas, x13, y1);
  Canvas_LineTo(temp_canvas, (u16)x13, (u16)y13);
  Canvas_LineTo(temp_canvas, (u16)x2, (u16)y13);
  if (icon) {
    i16 bx = add(x, 4), by = (i16)((u16)y + 3u - (u16)pressed);
    u16 image = (u16)icon + 38u;
    DrawBob(&bob_stanbobs, image, bx, by);
  }
}

i32 ShowAskForNumberQuery(const u8 *prompt, i32 value, i32 increment, i32 minimum, i32 maximum) {
  u8 formatted[60];
  mouse_pressed = 0;
  BackupMenuHierarchyFlag();
  game_ptr->in_menu_hierarchy = 0;
  game_ptr->disable_staff_disasters = (u8)((u32)game_ptr->disable_staff_disasters + 1u);
  SetCurrentFont(font_astandar);
  Copy320x200Bitmap(current_bitmap, old_bitmap);
  FillRect(14, 20, 42, 194, 98);
  AskForPizzaPriceHelper(temp_canvas, 20, 42, 194, 98);
  SetSomeFieldInCanvas(0);
  (void)DrawText2(20, 194, 52, prompt);
  DrawPushButton(ok_text, 150, 66, 0);
  for (;;) {
    ShowAskForQuery_Helper(1, 108, 57, 0);
    ShowAskForQuery_Helper(2, 108, 77, 0);
    FillRect(13, 50, 67, 100, 78);
    SetFontColor(0);
    int n = snprintf((char *)formatted, sizeof(formatted), "%d%s", value, (const char *)unit_for_number_query);
    if (n < 0 || (size_t)n >= sizeof(formatted))
      abort();
    (void)DrawText2(50, 100, 75, formatted);
    mouse_pressed = WaitForMouseClick();
    if (mouse_pressed != 1)
      continue;
    if ((IsMouseInside(150, 66, 185, 82) != 0))
      break;
    if ((IsMouseInside(110, 57, 130, 75) != 0)) {
      ShowAskForQuery_Helper(1, 108, 57, 1);
      value = pt_signed_dword((u32)value + (u32)increment);
      if (value > maximum)
        value = pt_signed_dword((u32)value - (u32)increment);
    }
    if ((IsMouseInside(110, 76, 130, 93) != 0)) {
      ShowAskForQuery_Helper(2, 108, 77, 1);
      value = pt_signed_dword((u32)value - (u32)increment);
      if (value < minimum)
        value = pt_signed_dword((u32)value + (u32)increment);
    }
    WaitForMouseLeave();
  }
  DrawPushButton(ok_text, 150, 66, 1);
  WaitForMouseLeave();
  DrawPushButton(ok_text, 150, 66, 0);
  Copy320x200Bitmap(old_bitmap, current_bitmap);
  game_ptr->disable_staff_disasters = (u8)((u32)game_ptr->disable_staff_disasters - 1u);
  return value;
}

u8 ShowMultichoiceQuestion(u8 count, u8 *first, u8 *second, u8 *third, u8 *fourth) {
  if (!count || count > 4)
    abort();
  u8 *choices[4] = {first, second, third, fourth};
  i16 rows[4];
  u8 index, choice = 0;
  BackupCanvasAttr();
  game_ptr->disable_staff_disasters = (u8)((u32)game_ptr->disable_staff_disasters + 1u);
  did_terminal_stuff = 1;
  Copy320x200Bitmap(current_bitmap, old_bitmap);
  SetCurrentBitmap(old_bitmap);
  FillRect(14, 20, 26, 196, 70);
  ResetListAttrParameters();
  game_ptr->list_element_spacing_2 = 0;
  i16 y = 34;
  for (index = 0; index < count; ++index) {
    rows[index] = y;
    if (index)
      FillRect(14, 20, (u16)(add(rows[index - 1], 15)), 41, (u16)y);
    FillRect(14, 20, (u16)y, 196, (u16)(add(y, 18)));
    ShowAskForQuery_Helper(0, 24, y, 0);
    u16 bottom = Canvas_TextRect(temp_canvas, 11, 14, choices[index], 42, 42, 196, add(y, 9));
    FillRect(14, 42, (u16)(add((i16)bottom, 1)), 196, (u16)(add((i16)bottom, 10)));
    FillRect(14, 20, (u16)(add(rows[index], 15)), 41, (u16)(add((i16)bottom, 9)));
    y = add((i16)bottom, 8);
  }
  AskForPizzaPriceHelper(temp_canvas, 20, 26, 196, y);
  CommitBitmapAndSetPalette((i8)game_ptr->current_palette);
  while (!choice) {
    WaitForMouseClick2();
    for (index = 0; index < count; ++index) {
      y = rows[index];
      if ((IsMouseInside(24, y, 36, add(y, 12)) != 0)) {
        ShowAskForQuery_Helper(0, 24, y, 1);
        choice = (u8)(index + 1u);
      }
    }
  }
  SetCurrentBitmap(old_bitmap);
  CommitBitmapAndSetPalette(0);
  Copy320x200Bitmap(current_bitmap, old_bitmap);
  RestoreCanvasAttrs();
  game_ptr->disable_staff_disasters = (u8)((u32)game_ptr->disable_staff_disasters - 1u);
  u8 result = (u8)(choice - 1u);
  return result;
}

// Main menu.

#include "files.h"
#include "savegame.h"
#include "missions.h"
#include "render_types.h"
#include "reports.h"
#include "sound.h"

Game *ClearGameByte107Flag(void) {
  game_ptr->byte106 = 0;
  return game_ptr;
}
void FreeFont(Font *f) {
  pt_font_release(f);
}
void DrawMainMenu(u8 clear_first) {
  (void)LoadBackground(main_menu_background_filename, old_bitmap);
  SetCurrentBitmap(old_bitmap);
  if (clear_first) {
    FillRect(0, 0, 0, 319, 199);
  }
  CommitBitmapAndSetPalette(38);

  CopyBitmap2to1();
}
void LoadAndDrawMainMenu(u8 clear_first) {
  game_ptr->byte87 = 0;
  game_ptr->current_button_menu = 255;
  game_ptr->byte107 = 0;
  (void)ClearGameByte107Flag();
  main_menu_canvas_flag = 0;

  SetSomeFieldInCanvas(0);
  ClearCanvasField14();
  if (font_ruby) {
    FreeFont(font_ruby);
  }
  DrawMainMenu(clear_first);
  loaded_font = LoadFontFile(&menu_font);
  if (!loaded_font) {
    loaded_font = LoadFontFile(&ruby_font);
  }
  SetCurrentFont(loaded_font);

  (void)LoadBobFile(&menu_bob);
  SaveAndSetInputEventFilter(7);
}
u16 DrawShadowText(i16 y, u8 string_id, u8 foreground) {
  const u8 *text = main_menu_strings[string_id];
  temp_canvas->color = 14;
  i16 shadow = (i16)((u16)((u16)y + 3u));
  DrawText2(1, 320, shadow, text);
  temp_canvas->color = (u8)foreground;
  u16 result = DrawText2(6, 320, y, text);
  return result;
}
static u8 selected(u8 menu, i16 top, i16 spacing) {
  const Mouse *m = mouse_result;
  i16 relative = (i16)((u16)((u16)m->y - (u16)((u16)top - (u16)spacing)));
  if (m->x > 130 && m->x < 197 && relative > 0 && relative < (i32)spacing * main_menu_desc[menu].num_items)
    return (u8)(relative / spacing);
  return 255;
}
static void shadow(u8 menu, u8 item, i16 top, i16 spacing, u8 foreground) {
  i16 y = (i16)((u16)((u16)top + (u32)(u16)spacing * item));
  DrawShadowText(y, main_menu_desc[menu].string_id[item], foreground);
}
u8 DrawMenuFirstTime(u8 menu, i16 top, i16 spacing) {
  u8 selection = 255;
  SetCurrentFont(loaded_font);
  CopyBitmap2to1();
  SetCurrentBitmap(old_bitmap);
  selection = selected(menu, top, spacing);
  for (u8 i = 0; i < main_menu_desc[menu].num_items; ++i) {
    shadow(menu, i, top, spacing, i == selection ? 29 : 1);
  }
  CommitBitmapAndSetPalette(0);
  u8 result = selection;
  return result;
}
u8 HandleMainMenuOfType(u8 menu, i16 top, i16 spacing) {
  mouse_pressed = 0;
  SaveAndSetInputEventFilter(5);
  u8 item;
  item = DrawMenuFirstTime(menu, top, spacing);
  u8 previous = item;
  while (item == 255 || !mouse_pressed) {
    item = selected(menu, top, spacing);
    if (item != previous) {
      if (previous != 255)
        shadow(menu, previous, top, spacing, 1);
      if (item != 255)
        shadow(menu, item, top, spacing, 29);
      previous = item;
    }
    mouse_pressed = WaitForMouseClick();
  }
  return item;
}
u8 HandleMainMenuOfTypeOptional(u8 menu, i16 top, i16 spacing) {
  mouse_pressed = 0;
  u8 item;
  item = DrawMenuFirstTime(menu, top, spacing);
  u8 previous = item;
  while (!mouse_pressed) {
    item = selected(menu, top, spacing);
    if (item != previous) {
      if (previous != 255)
        shadow(menu, previous, top, spacing, 1);
      if (item != 255)
        shadow(menu, item, top, spacing, 29);
      previous = item;
    }
    mouse_pressed = WaitForMouseClick();
  }
  return item;
}
void SetGameByte107Flag(void) {
  game_ptr->byte106 = 1;
  last_second_tick = ReadPlatformTicks();
}
void LoadGameBackgroundImage(void) {
  game_ptr->byte107 = 1;
  if (loaded_font) {
    FreeFont(loaded_font);
  }
  loaded_font = NULL;
  font_ruby = LoadFontFile(&ruby_font);
  SetCurrentFont(font_astandar);

  (void)LoadBackground(game_screen_background_filename, old_bitmap);
  if (!temp_bitmap_84x6) {
    temp_bitmap_84x6 = pt_bitmap_create();

    InitializeAndAllocBitmap(temp_bitmap_84x6, 0, 84, 6);

    (void)BitBlt(old_bitmap, 228, 111, temp_bitmap_84x6, 0, 0, 84, 6);
  }
  SetCurrentBitmap(old_bitmap);
  CommitBitmapAndSetPalette(62);
  CopyBitmap2to1();
  game_ptr->byte87 = 1;
  SetGameByte107Flag();

  FillSomeRectInGraphicsRam();
  SaveAndSetInputEventFilter(3);
  main_menu_canvas_flag = 1;
}
u16 HandleHighScores(void) {
  HighScoreRecord records[10];
  LoadHighscoresFile(records);
  DrawIntroCaption(main_menu_strings[2]);
  temp_canvas->color = 13;
  for (unsigned column = 0; column < 2; ++column) {
    i16 x = column ? 200 : 26;

    (void)SetTempCanvasPosition(x, 60);

    (void)RelativeLineTo(0, 140);
  }
  temp_canvas->color = 1;
  for (u8 i = 0; i < 8; ++i) {
    (void)SetTempCanvasPosition(16, (i16)(17u * i + 75u));
    DrawNumberInsideListRight(i + 1u);
    cur_list_elem_x = 36;
    (void)DrawTextInsideList(records[i].record_name);
    cur_list_elem_x = 318;
    DrawNumberInsideListRight(pt_signed_dword(records[i].score));
  }
  return WaitForMouseClick2();
}
void HandleOptions(void) {
  u8 row_index, selected_row;
  DrawIntroCaption(main_menu_strings[4]);
  prefs.field_A = 0;
  prefs.field_B = 0;
  SetCurrentFont(font_piznumb);
redraw:
  if (prefs.field_B) {
    return;
  }
  selected_row = 9;
  SetCurrentBitmap(old_bitmap);

  FillRect(0, 60, 68, 300, 195);
  for (row_index = 0; row_index < 12; ++row_index) {
    i32 row;
    row = GetSelectedListItem(68, 10);
    u8 i = row_index;
    if ((u8)row == i) {
      selected_row = (u8)row;
      temp_canvas->color = 29;
    } else
      temp_canvas->color = 1;

    SetTempCanvasPosition(60, (i16)(10u * i + 77u));
    const u8 *text = main_menu_strings[i + 50u];
    DrawListElement(text);
    cur_list_elem_x = 200;
    if (i < 4) {
      text = prefs.flags[i] ? options_on_label : options_off_label;
      DrawListElement(text);
    }
  }
  CommitBitmapAndSetPalette(0);
  mouse_pressed = WaitForMouseClick();
process_click:
  if (mouse_pressed == 1) {
    u8 selected = selected_row;
    if (selected < 2 && !prefs.field_2)
      goto redraw;
    prefs.flags[selected] ^= 1u;
    if (selected == 2) {
      prefs.flags[0] = prefs.field_2 ? 1 : 0;
      prefs.flags[1] = prefs.field_2 ? 1 : 0;
    }
  }
  if (!prefs.field_A)
    goto redraw;
  (void)OpenFileForWritingAsCurrent(options_prefs_filename);
  (void)WriteBytesToCurrentFile(pt_preferences_file_image(), sizeof(PTPreferencesFileImage));
  (void)CloseCurrentWritingFile();
  goto process_click;
}
u8 HandleOldGame(void) {
  DrawIntroCaption(main_menu_strings[0]);
  u8 selected;
  selected = GetSaveOrLoadFilename(0);
  if (selected != 255)
    return 0;
  (void)LoadSavegame(selected_filename);
  return 1;
}
void HandleDiskOptionsMenu(void) {
  game_ptr->byteB4 = 0;
  for (;;) {
    InitGraphicsMode();
    LoadAndDrawMainMenu(1);
    DrawIntroCaption(disk_menu_caption);
    u8 item;
    item = HandleMainMenuOfType(4, 110, 19);
    switch (item) {
    case 0: {
      DrawIntroCaption(main_menu_strings[14]);
      u8 family;
      family = HandleMainMenuOfType(6, 100, 19);
      if (family)
        family = 2;
      DrawIntroCaption(main_menu_strings[16u + family]);
      u8 selected;
      selected = GetSaveOrLoadFilename((u8)(2u * family + 1u));
      if (selected == 255 && selected_filename[0]) {
        if (family == 2) {
          (void)SaveGamePersonal(selected_filename, current_player_index);
        } else if (game_ptr->allow_save_game) {
          (void)SaveGame(selected_filename);
        }
      }
      break;
    }
    case 1: {
      u8 loaded;
      loaded = HandleOldGame();
      if (loaded)
        goto done;
      break;
    }
    case 2:
      HandleOptions();
      break;
    case 3: {
      u8 yes;
      yes = ShowMessage2(0, disk_quit_prompt);
      if (yes) {
        exit_code = 1;
        for (u8 player = 0; player < game_ptr->num_interactive_players; ++player) {
          HandlePlayerDead(player);
        }
        ShowGameOverExitMsg();
        // Stock retains these calls after the nonreturning game-over
        // destructor. Preserve the boundaries even though it exits.

        StopSound();
        ExitProcess(1);
      }
      break;
    }
    case 4:
      goto done;
    default:
      break;
    }
  }
done:
  LoadGameBackgroundImage();
}
u8 HandleMainMenu(void) {
  u8 new_game = 1;
  game_ptr->byteB4 = 0;
  LoadAndDrawMainMenu(0);
  for (;;) {
    (void)InitGraphicsMode();
    u8 item;
    item = HandleMainMenuOfType(0, 122, 18);
    switch (item) {
    case 0: {
      u8 selected;
      selected = HandleOldGame();
      if (selected) {
        new_game = 0;
        goto selected_game;
      }
      break;
    }
    case 1: {
      u8 selected;
      selected = HandleNewGame();
      if (selected)
        goto selected_game;
      break;
    }
    case 2:
      (void)HandleHighScores();
      break;
    case 3:
      HandleOptions();
      break;
    default:
      break;
    }
    DrawMainMenu(0);
  }
selected_game:
  game_ptr->mask_of_players = 0;
  for (u8 i = 0; i < game_ptr->num_interactive_players; ++i)
    game_ptr->mask_of_players = (u8)((u32)game_ptr->mask_of_players | (u32)((u8)(1u << (i & 31u))));

  (void)InitGraphicsMode();
  LoadGameBackgroundImage();

  return new_game;
}

// High scores.

void LoadHighscoresFile(HighScoreRecord table[10]) {
  memset(table, 0, 10 * sizeof(*table));

  u32 opened = OpenFileAsCurrent(highscores_filename);
  if (!(opened & UINT16_MAX))
    return;
  ReadBytesFromCurrentFile(table, 10 * sizeof(*table));
  CloseCurrentFile();
}

// Intro.

#include "runtime_state.h"
#include "player.h"
#include "calendar.h"

static void set_color(i16 index, u8 value) {
  SetPalColor2(active_palette_pointer, index, value, value, value);
}
static void white_first_colors(void) {
  for (unsigned i = 0; i < 32; ++i)
    set_color((i16)i, 15);
}

u8 ShowIntroTextSet(u8 index) {
  FillRect(0, 0, 0, 319, 199);
  temp_canvas->color = 1;
  DrawText2(0, 320, 78, index ? GetTranslatedString(index) : intro_presents);
  for (u8 i = 0; (i32)(i8)i < intro_text_groups[index].count; ++i) {
    DrawText2(0, 320, (i16)(17 * (i32)(i8)i + 100), GetTranslatedString((u8)(i + intro_text_groups[index].first)));
  }
  for (i32 i = 15; i >= 0; --i) {
    set_color(1, (u8)i);
    Delay(3);
    if ((WantSkipGameIntro() != 0)) {
      return 1;
    }
  }
  for (unsigned i = 0; i < 15; ++i) {
    Delay(8);
    if ((WantSkipGameIntro() != 0)) {
      return 1;
    }
  }
  for (unsigned i = 0; i < 16; ++i) {
    set_color(1, (u8)i);
    if ((WantSkipGameIntro() != 0)) {
      return 1;
    }
    Delay(2);
  }

  return 0;
}
// Credits repeat until an actual recorded mouse-button decision exits the original loop.

void ShowGameIntro(void) {
  PTRGB black_palette[256];
  (void)InitGraphicsMode();
  (void)memset(black_palette, 0, sizeof(black_palette));

  WaitForRetrace();
  (void)UploadPalette(0, 256, black_palette);
  palette_fade_algo = 0;
  LoadBackground(intro_logo_filename, current_bitmap);
  FadePalette(1);
  for (unsigned i = 0; i < 30; ++i)
    Delay(5);
  FadePalette(-1);
  temp_canvas->color = 0;
  Canvas_SetPosition(temp_canvas, 0, 0);
  Canvas_ClearWholeBitmap_(temp_canvas);
  palette_fade_algo = 1;
  SelectPalette(66);
  white_first_colors();
  loaded_font = LoadFontFile(&menu_font);
  if (!loaded_font) {
    loaded_font = LoadFontFile(&ruby_font);
  }
  LoadBackground(intro_cyber_filename, current_bitmap);
  SetCurrentFont(font_supersma);
  temp_canvas->color = 1;
  (void)DrawText(2, 198, intro_version);
  FadePalette(1);
  SetCurrentFont(loaded_font);
  (void)LoadLangFile(&langfile_credits);
  game_ptr->sound_enabled = 1;
  u32 allocated;
  allocated = AllocateIntroBuffer();
  if (allocated & 65535u) {
    (void)LoadIntroMusic();
    SoundStuff2(0);
  }
  for (unsigned i = 0; i < 8; ++i) {
    if ((WantSkipGameIntro() != 0))
      goto done;
    Delay(5);
  }
  FadePalette(-1);
  u8 result;
  result = ShowIntroTextSet(0);
  if (result)
    goto done;
  white_first_colors();
  SelectPalette(64);
  white_first_colors();
  LoadBackground(intro_title1_filename, current_bitmap);
  FadePalette(1);
  for (unsigned i = 0; i < 8; ++i) {
    if ((WantSkipGameIntro() != 0))
      goto done;
    Delay(50);
  }
  FadePalette(-1);
  FillRect(0, 0, 0, 319, 199);
  SelectPalette(65);
  white_first_colors();
  LoadBackground(intro_title2_filename, current_bitmap);
  FadePalette(1);
  for (unsigned i = 0; i < 50; ++i) {
    if ((WantSkipGameIntro() != 0))
      break;
    Delay(10);
  }
  white_first_colors();
  FadePalette(-1);
  for (;;)
    for (unsigned i = 1; i < 22; ++i) {
      result = ShowIntroTextSet((u8)i);
      if (result)
        goto done;
    }
done:
  game_ptr->sound_enabled = 0;
  SoundStuff1();
  FreeIntroBuffer();
  FreeLangFile(&langfile_credits);
  FillRect(0, 0, 0, 319, 199);
  palette_fade_algo = 0;
  GraphicsRestoreCursor();
}

// Mission selection.

// Literal goal names take priority over translated text.

const u8 *GetNameOfGoal(u8 goal) {
  switch (goal) {
  case 35:
    return goal_name_1000_pizzas;
  case 36:
    return goal_name_10000_pizzas;
  case 121:
    return goal_name_profit_25000;
  case 122:
    return goal_name_profit_50000;
  case 140:
    return goal_name_world;
  default: {
    return GetTranslatedString(goal);
  }
  }
}
// Built-in mission names occupy the menu-string prefix.

void DrawMissionModeListElements(u8 mission, u8 row) {
  u16 y = 14u * row;
  FillRect(14, 199, y + 74u, 309, y + 86u);
  temp_canvas->color = 1;
  SetCurrentFont(font_piznumb);
  cur_list_elem_x = 201;
  cur_list_elem_y = y + 83u;
  DrawListElement(main_menu_strings[(unsigned)mission + 20u]);
}
// Preserve wrapped vertical coordinates before clipping to the list.

void DrawMissionGoalListElem(u32 relative_y, const u8 *label) {
  u32 value = relative_y + 82u;

  i16 y = (i16)((u16)value);
  if (y < 82 || y > 179)
    return;
  temp_canvas->color = 1;
  (void)SetTempCanvasPosition(21, y);
  draw_list_elem_style = 2;
  game_ptr->list_element_spacing_2 = 255;
  DrawListElement(label);
  game_ptr->list_element_spacing_2 = 0;
}

// A zero year terminates the target list; the byte index wraps.

void DrawMissionGoalStuff(u8 player, u8 offset) {
  u8 goal_index;
  goal_index = 0;
  ClearMissionGoalRect();
  SetCurrentFont(font_piznumb);
  for (;;) {
    u8 index = goal_index;
    const PTMissionGoals *goal = pt_goal_target_at(player, index);

    if (!goal->year[index % 100u])
      break;
    u8 day = goal->day[index % 100u];
    u8 month = goal->month[index % 100u];

    (void)snprintf((char *)strbuf, sizeof(strbuf), "%u-%u-%u", day, month, goal->year[index % 100u]);
    i32 y = 25 * (i32)index - 8 * (i32)offset;
    DrawMissionGoalListElem((u32)(i32)(i16)((i16)((u16)y)), strbuf);
    const u8 *label;
    label = GetNameOfGoal(goal->kind[index % 100u]);
    DrawMissionGoalListElem((u32)(i32)(i16)((i16)((u16)(y + 8))), label);
    ++goal_index;
  };
}
// Disk and built-in missions share the target loader.

u8 HandleMissionModeMenu(void) {
  u8 goal_scroll, copy_player, mission_scroll, selected;
  mission_scroll = 0;
  goal_scroll = 0;
  selected = 0;
  selected_filename[0] = 0;
  DrawIntroCaption(mission_selection_caption);
  (void)LoadLangFile(&langfile_leben);
  SetCurrentFont(loaded_font);
  DrawMainMenuListboxes(1);
redraw:
  for (u8 i = 0; i < 8; ++i) {
    DrawMissionModeListElements(i, i);
  }
  temp_canvas->color = 1;
clear_press:
  mouse_pressed = 0;
  for (;;) {
    u8 item;
    item = HandleMainMenuOfTypeOptional(5, 80, 26);
    if (item == 1)
      break;
    if (!item)
      goto browse;
    if (item == 255) {
      u8 region;
      region = CheckMouseInsideListbox();
      switch (region) {
      case 1:
        if (goal_scroll)
          --goal_scroll;
        break;
      case 2:
        if (goal_scroll < 200)
          ++goal_scroll;
        break;
      case 3:
        if (mission_scroll)
          --mission_scroll;
        goto redraw;
      case 4:
        goto redraw;
      case 5:
        goto browse;
      case 6: {
        i32 row;
        row = GetSelectedListItem(73, 14);

        (void)snprintf((char *)selected_filename, sizeof(selected_filename), "m\\%u.mis", (u8)row);
        goal_scroll = 0;
        break;
      }
      default:
        continue;
      }
    }
  load_selected:
    if (!selected_filename[0])
      goto clear_press;
    selected = 1;
    LoadMissionTargetsFile(selected_filename, savegame_slot);
    copy_player = 1;
    for (; copy_player < 8; ++copy_player) {
      PTMissionGoals *destination = &pt_player_goals(copy_player)->targets;
      const PTMissionGoals *source = &pt_player_goals(0)->targets;
      u8 *targets[] = {destination->kind, destination->day, destination->month, destination->year};
      const u8 *sources[] = {source->kind, source->day, source->month, source->year};
      for (unsigned i = 0; i < 4; ++i) {
        (void)memmove(targets[i], sources[i], sizeof(destination->kind));
      }
    }
    DrawMissionGoalStuff(current_player_index, goal_scroll);
    goto clear_press;
  browse:
    (void)GetSaveOrLoadFilename(4);
    goal_scroll = 0;
    goto load_selected;
  }
  FreeLangFile(&langfile_leben);
  u8 result = selected;

  return result;
}

// Character selection.

#include <assert.h>
#include "finance.h"
#include "game.h"
#include <math.h>

void SetPlayerCharacter(u8 character, u8 player) {
  u8 previous_player;

  assert(player < 8 && current_player_index < 8);
  previous_player = current_player_index;
  SetCurrentPlayer(player);
  (void)LoadCharacter(character, 0);
  (void)memmove(&characters[player], &temp_character, sizeof(Character));
  player_ptr->money = character_ptr->money;
  players[player].character_index = character;
  characters[player].start_of_day = 54;
  characters[player].now = 126;
  SetCurrentPlayer(previous_player);
}

void DrawPlayerListElement(bool show_name, u8 player) {
  if (player <= 7) {
    if (player < game_ptr->num_interactive_players || player == savegame_slot) {
      SetupCanvasTempIglooLink();
      (void)SetupHazelWithBobSprite(&temp_hazel, 0, &menu_bob, 5u + (player == savegame_slot));
      temp_igloo.draw_x = 198;
      temp_igloo.draw_y = 14u * player + 73u;
      (void)Canvas_DrawAllIgloos(temp_canvas, active_palette_pointer);
    } else {
      FillRect(12, 199, 14u * player + 73u, 309, 14u * player + 86u);
      FillRect(14, 199, 14u * player + 74u, 309, 14u * player + 86u);
    }
    temp_canvas->color = 1;
    SetCurrentFont(font_piznumb);
    cur_list_elem_x = 201;
    cur_list_elem_y = 14u * player + 83u;
    if (show_name) {
      DrawListElement(pt_player(player)->name_text);
    }
  }
}
static u16 next_stats_row(void) {
  u16 spacing = (u16)(game_ptr->list_element_spacing + (i8)game_ptr->list_element_spacing_2);
  cur_list_elem_y = (i16)((u16)((u16)cur_list_elem_y + spacing));
  return spacing;
}
u16 DrawPlayerStatsListEntry(const u8 *label, i32 value) {
  u16 saved_x;
  saved_x = (u16)cur_list_elem_x;
  DrawListElement(label);
  (void)SetTempCanvasPositionX(player_stats_value_x);
  DrawNumberInsideListRight(value);
  cur_list_elem_x = (i16)saved_x;
  u16 result = next_stats_row();

  return result;
}
u16 DrawPlayerStatsListMoney(const u8 *label, double value) {
  u8 formatted[312];
  u16 saved_money_x;

  saved_money_x = (u16)cur_list_elem_x;
  DrawListElement(label);
  (void)SetTempCanvasPositionX(player_stats_value_x);
  DoubleToString(formatted, value);
  u32 count = pt_strlen(formatted);
  count = (u8)count;
  u16 width = Canvas_TextWidth(temp_canvas, pt_text_span(formatted, (u16)count));
  temp_canvas->position_x = (i16)((u16)((u16)cur_list_elem_x - width));
  (void)Canvas_DrawText(temp_canvas, pt_text_span(formatted, (u16)count));
  cur_list_elem_x = (i16)saved_money_x;
  u16 result = next_stats_row();

  return result;
}
void DrawPlayerStatsList(const Character *c) {
  game_ptr->list_element_spacing_2 = 0;
  draw_list_elem_style = 0;
  SetCurrentFont(font_piznumb);
  FillRect(0, 19, 74, 128, 184);
  temp_canvas->color = 1;
  (void)SetTempCanvasPosition(21, 82);
  (void)snprintf((char *)strbuf, sizeof(strbuf), "%s %s", c->first_name, c->last_name);
  DrawStringBufferAsListElement();
  player_stats_value_x = 120;
  (void)SetTempCanvasPosition(21, 91);
  (void)DrawPlayerStatsListMoney(player_stats_money_label, c->money);
  static const u8 *const labels[] = {player_stats_age_label,
    player_stats_popularity_label,
    player_stats_health_label,
    player_stats_energy_label,
    player_stats_intelligence_label,
    player_stats_charisma_label,
    player_stats_dexterity_label,
    player_stats_personality_label,
    player_stats_cooking_label,
    player_stats_courage_label};
  static const u8 properties[] = {PT_CHARACTER_AGE,
    PT_CHARACTER_POPULARITY,
    PT_CHARACTER_HEALTH,
    PT_CHARACTER_ENERGY,
    PT_CHARACTER_INTELLIGENCE,
    PT_CHARACTER_CHARISMA,
    PT_CHARACTER_DEXTERITY,
    PT_CHARACTER_PERSONALITY,
    PT_CHARACTER_COOKING,
    PT_CHARACTER_COURAGE};
  for (unsigned i = 0; i < 10; ++i) {
    (void)DrawPlayerStatsListEntry(labels[i], c->prop[properties[i]]);
  }
}

u8 HandleSelectCharacter(void) {
  u8 selected_characters[8];
  u8 previous_selection;

  previous_selection = 0;
  max_num_characters = 100;
  game_ptr->byteB4 = 1;
  game_ptr->num_interactive_players = 1;
  savegame_slot = 0;
  DrawIntroCaption(main_menu_strings[11]);
  DrawMainMenuListboxes(1);
  for (u8 i = 0; i < 8; ++i) {
    bool duplicate;
    do {
      u32 name = RandomNumber(20);
      players[i].random_name = (u8)name;
      duplicate = false;
      for (u8 j = 0; j < i; ++j)
        if (players[i].random_name == players[j].random_name) {
          duplicate = true;
          break;
        }
    } while (duplicate);
    selected_characters[i] = i;
    SetPlayerCharacter(i, i);
  }
  for (u8 i = 0; i < 8; ++i) {
    (void)pt_strcpy(pt_player(i)->name_text, main_menu_strings[30u + players[i].random_name]);
  }
reload_character:

  assert(savegame_slot < 8);
  SetPlayerCharacter(selected_characters[savegame_slot], savegame_slot);
redraw_players:
  for (u8 i = 0; i < 8; ++i) {
    DrawPlayerListElement(true, i);
  }
redraw_stats:

  assert(savegame_slot < 8);
  if (previous_selection != savegame_slot) {
    u8 prior = previous_selection;
    DrawPlayerListElement(true, prior);
    DrawPlayerListElement(true, savegame_slot);
  }
  previous_selection = savegame_slot;
  DrawPlayerStatsList(&characters[savegame_slot]);
  (void)PaintCharacterFace(players[savegame_slot].character_index, 0, 137, 63);
  Canvas_DrawRect(1, 137, 63, 189, 122);
  CopyBitmap2to1();
  temp_canvas->color = 1;
read_input:
  SetCurrentFont(loaded_font);
  mouse_pressed = 0;
  u8 item = HandleMainMenuOfTypeOptional(3, 145, 26);
  if (item == 255) {
    u8 region = CheckMouseInsideListbox();
    switch (region) {
    case 1:
      if (selected_characters[savegame_slot] >= (int)max_num_characters - 1)
        goto read_input;
      selected_characters[savegame_slot]++;
      goto reload_character;
    case 2:
      if (!selected_characters[savegame_slot])
        goto read_input;
      selected_characters[savegame_slot]--;
      goto reload_character;
    case 3:
      if (game_ptr->num_interactive_players < 2)
        goto read_input;
      game_ptr->num_interactive_players = (u8)((u32)game_ptr->num_interactive_players - 1u);
      goto redraw_players;
    case 4:
      if (game_ptr->num_interactive_players > 7)
        goto read_input;
      game_ptr->num_interactive_players = (u8)((u32)game_ptr->num_interactive_players + 1u);
      goto redraw_players;
    case 5: {
      FillRect(0, 20, 75, 126, 82);
      (void)SetTempCanvasPosition(21, 82);
      temp_canvas->color = 1;
      SetCurrentFont(font_piznumb);
      strbuf[0] = 0;
      game_ptr->text_edit_max_width = 100;
      (void)HandleTextEditing(strbuf, 122);
      game_ptr->text_edit_max_width = 149;
      u8 *surname = strbuf;
      {
        u8 *space = memchr(strbuf, ' ', strlen((char *)strbuf));
        if (space)
          surname = space;
      }
      Character *c = &characters[savegame_slot];
      if (surname != strbuf || strbuf[0] == ' ') {
        *surname = 0;
        surname++;
        (void)GuestStrncpy(c->first_name, strbuf, 15);
        c->first_name[15] = 0;
      } else
        c->first_name[0] = 0;
      (void)GuestStrncpy(c->last_name, surname, 19);
      c->field_23 = 0;
      goto redraw_stats;
    }
    case 6: {
      i32 slot = GetSelectedListItem(73, 14);
      savegame_slot = (u8)slot;
      if (savegame_slot == previous_selection) {
        DrawPlayerListElement(false, savegame_slot);
        (void)SetTempCanvasPosition(202, 14u * savegame_slot + 83u);
        SetCurrentFont(font_piznumb);
        temp_canvas->color = 1;
        (void)HandleTextEditing(pt_player(savegame_slot)->name_text, 14);
        previous_selection = 8;
      }
      goto redraw_stats;
    }
    default:
      goto redraw_stats;
    }
  }
  if (item == 2) {
    return 0;
  }
  if (item)
    goto redraw_stats;
  for (u8 i = 0; i < 8; ++i) {
    InitLinkedListsInPlayer(i);
    InitDepotsInPlayer(i);
    float money = (float)characters[i].money;

    TransferPlayerMoney(i, 5, 6, money);

    double value = characters[i].money;
    if (value > character_selection_money_limit)
      players[i].money_as_integer[0] = 2000000000;
    else {
      value = TruncateX87Integral(value);
      i32 integral = pt_integral_i32(value);

      players[i].money_as_integer[0] = integral;
    }
  }

  return 1;
}
u8 HandleNewGame(void) {
  u8 control = 0;
  do {
    DrawIntroCaption(main_menu_strings[1]);
    SetCurrentFont(loaded_font);
    temp_canvas->color = 1;
    while (!control) {
      days_in_month[0] = 0;
      DrawIntroCaption(main_menu_strings[1]);
      u8 mode = HandleMainMenuOfType(1, 80, 26);
      game_ptr->is_mission_mode = (u8)((i8)mode % 2);
      if (mode == 1) {
        control = HandleMissionModeMenu();
      } else
        control = 1;
      if (mode == 2) {
        DrawIntroCaption(main_menu_strings[6]);
        u8 quickstart = HandleMainMenuOfType(7, 80, 26);
        days_in_month[0] = (u8)(quickstart + 1u);
      }
      DrawIntroCaption(character_selection_level_caption);
      u8 level = HandleMainMenuOfType(8, 80, 26);
      ptr_apple->difficulty_level = (u8)(level + 1u);
    }
    control = HandleSelectCharacter();
  } while (!control);

  return control;
}

// Country selection.

#include "state_types.h"
#include "staff.h"

u8 FindMouseInsideWhichCountry(void) {
  for (u8 i = 0; i < 10; ++i) {
    bool inside;
    inside = IsMouseInside((i16)((u16)(country_selector_x[i] - 5)),
        (i16)((u16)(country_selector_y[i] - 5)),
        (i16)((u16)(country_selector_x[i] + 5)),
        (i16)((u16)(country_selector_y[i] + 5)));
    if (inside) {
      return i;
    }
  }

  return 255;
}
u16 ShowCountryInfoBox(u8 city) {
  u8 leader;
  leader = 8;
  u8 best = 0;
  for (u8 i = 0; i < 8; ++i)
    if (savegame_sect_0.market_share[i][city] > best) {
      leader = i;
      best = savegame_sect_0.market_share[i][city];
    }
  (void)DrawCountryInfoBoxBg(70, 60, 230, 150);
  temp_canvas->color = 20;
  (void)DrawText2(70, 230, 85, town_names[city]);
  (void)DrawText2(70, 230, 95, country_market_leader_label);
  (void)DrawText2(70, 230, 105, leader < 8 ? pt_player(leader)->name_text : country_no_market_leader_label);
  SaveAndSetInputEventFilter(1);
  (void)WaitForMouseClick2();
  (void)DrawGameBitmap(70, 60, 230, 150);
  SaveAndSetInputEventFilter(5);
  u8 found;
  found = FindMouseInsideWhichCountry();

  return (u16)((i8)found == city);
}

static i16 box_width(const u8 *p, u16 n) {
  u16 w;
  w = Canvas_TextWidth(temp_canvas, pt_text_span(p, n));
  return (i16)w;
}
u32 DrawBoxAndWaitForClick(const u8 *source, i16 left, i16 right, i16 top) {
  u8 wrapped_text[200];
  u8 last_break, line_count;
  u32 clicked_inside;

  last_break = 255;
  line_count = 0;
  clicked_inside = 0;
  bool done = false;
  SaveAndSetInputEventFilter(1);
  memset(wrapped_text, 0, sizeof(wrapped_text));
  (void)pt_strcpy(wrapped_text, source);
  while (!done) {
    u8 start = (u8)(last_break + 1u);

    u8 scan = (u8)(start + 1u);
    while (box_width(wrapped_text + start, (u16)(scan - start)) < (i32)right - left - 10 && !done) {
      last_break = scan;
      do {
        ++scan;
      } while (wrapped_text[scan] != 32 && wrapped_text[scan]);
      if (!wrapped_text[scan])
        done = true;
    }
    if (!done || box_width(wrapped_text + start, (u16)(scan - start)) >= (i32)right - left - 10)
      wrapped_text[last_break] = 0;
    ++line_count;
  }
  ++line_count;
  u8 lines = line_count;
  i16 bottom = (i16)((u16)(top + 10 + 11 * lines));
  (void)DrawCountryInfoBoxBg(left, top, right, bottom);
  temp_canvas->color = 21;
  const u8 *line = wrapped_text;
  for (u8 i = 0; i < lines; ++i) {
    (void)DrawText2(left, right, (i16)((u16)(top + 11 * (i + 1))), line);
    u32 length;
    length = pt_strlen(line);
    line += length + 1;
  }
  (void)WaitForMouseClick2();
  bool inside;
  inside = IsMouseInside(left, top, right, bottom);
  if (inside)
    clicked_inside = 1;
  (void)DrawGameBitmap(left, top, right, bottom);
  SaveAndSetInputEventFilter(5);
  u32 result = clicked_inside;

  return result;
}
u32 SetCurrentCity(u8 city) {
  MarkGoalReached((u8)(city + 100u));
  i32 profit = pt_player(current_player_index)->weekly_profit_by_town[city] / 7;
  for (unsigned i = 0; i < 30; ++i)
    pt_player(current_player_index)->profit_last_30_days[i] = profit;
  SetCurrentCityInner(city);
  u8 town = ptr_apple->town_index;
  if (!did_run_town_day_loop_1[town]) {
    RunCityNewDay();
    town = ptr_apple->town_index;
    did_run_town_day_loop_1[town] = 1;
  }

  return town;
}
static u16 transfer(u16 amount) {
  float value = (float)amount;

  u32 result;

  result = TransferPlayerMoney(current_player_index, 10, 5, value);

  return (u16)result;
}

u8 CountrySelector(u8 cost_mode, i8 palette) {
  u32 warning_result;
  u8 result_city, hovered_city, saved_player, previous_hover;

  previous_hover = 255;
  hovered_city = 255;
  result_city = 255;
  warning_result = 0;
  game_ptr->byte87 = 0;
  game_ptr->byte107 = 0;
  (void)ClearGameByte107Flag();
  SetCurrentFont(font_astandar);
  SetSomeFieldInCanvas(0);
  (void)InitGraphicsMode();
  ClearCanvasField14();
  (void)LoadBackground(country_map_filename, old_bitmap);
  SetCurrentBitmap(old_bitmap);
  ptr_apple->view_state = 2;
  CommitBitmapAndSetPalette(2);
  CopyBitmap2to1();
  (void)InitGraphicsMode();
  (void)BitBlt(current_bitmap, 100, 101, game_ptr->background, 0, 140, 90, 28);
  SaveAndSetInputEventFilter(5);
read_mouse:
  mouse_pressed = ReadMouseEvent();
  if (!mouse_pressed) {
  hover:
    hovered_city = FindMouseInsideWhichCountry();
    u8 selected = hovered_city;
    if (selected != previous_hover) {
      if (selected != 255)
        temp_canvas->color = 24;
      (void)BitBlt(game_ptr->background, 0, 140, current_bitmap, 100, 101, 90, 28);
      if (selected != 255) {
        const u8 *label = town_names[selected];
        (void)DrawText(104, 112, label);
      }
      previous_hover = selected;
    }
  }
  if (mouse_pressed != 1)
    goto read_mouse;
  u8 selected = hovered_city;
  if (selected == 255) {
    DrawBoxAndWaitForClick(country_no_city_message, 105, 225, 115);
    goto read_mouse;
  }
  u16 cost = 0;
  if (cost_mode) {
    u8 previous = ptr_apple->town_index;
    if (previous >= 10)
      pt_resource_bounds_failure("previous city", previous, 10);
    i32 dx = (i32)country_selector_x[previous] - country_selector_x[selected];
    i32 dy = (i32)country_selector_y[previous] - country_selector_y[selected];
    u32 ax = (u32)(dx < 0 ? -dx : dx), ay = (u32)(dy < 0 ? -dy : dy);
    cost = (u16)(((ax + ay) & 16383u) * ((unsigned)ptr_apple->difficulty_level + 1u));
  }
  if (cost_mode < 2) {
    u16 accepted;
    accepted = ShowCountryInfoBox(selected);
    if (!accepted) {
      mouse_pressed = 0;
      goto hover;
    }
    if (selected == ptr_apple->town_index)
      goto accepted_city;
    if (cost_mode) {
      if (!transfer(cost)) {
        DrawBoxAndWaitForClick(country_insufficient_money_message, 105, 225, 115);
        goto restore;
      }
      (void)snprintf((char *)strbuf, sizeof(strbuf), "The flight costs $%u.", cost);
      warning_result = DrawBoxAndWaitForClick(strbuf, 105, 225, 115);
    } else
      for (u8 i = 0; i < 8; ++i)
        if (i < game_ptr->num_interactive_players)
          pt_player(i)->city_index = selected;
    saved_player = current_player_index;
    if (cost_mode) {
      UpdateCharacterPropRandomly(PT_CHARACTER_ENERGY, -2, -6);
      AddCostForPlayer(current_player_index, cost, 8);
    }
    if (!(u16)warning_result && cost_mode) {
      mouse_pressed = 0;
      goto hover;
    }
    (void)SetCurrentCity(selected);
    SetCurrentPlayer(saved_player);
  } else {
    if (cost_mode != 2)
      goto read_mouse;
    if (selected != ptr_apple->town_index) {
      if (!pt_player_depot(pt_player(current_player_index), selected)) {
        (void)snprintf((char *)strbuf,
            sizeof(strbuf),
            "You do not have a central depot in %s!",
            (const char *)town_names[selected]);
        DrawBoxAndWaitForClick(strbuf, 105, 225, 115);
        goto read_mouse;
      }
      cost = (u16)(3u * cost);
      (void)snprintf((char *)strbuf, sizeof(strbuf), "The transport costs $%u.", cost);
      (void)transfer(cost);
      DrawBoxAndWaitForClick(strbuf, 105, 225, 115);
    }
  }
accepted_city:
  result_city = selected;
restore:
  (void)InitGraphicsMode();
  LoadGameBackgroundImage();
  FillSomeRectInGraphicsRam();
  SaveAndSetInputEventFilter(3);
  SetCurrentBitmap(old_bitmap);
  FillSomeRectWithColor(0);
  DrawLetterOnBitmap(current_bitmap);
  CommitBitmapAndSetPalette(palette);
  CopyBitmap2to1();
  game_ptr->byte107 = 1;
  ptr_apple->view_state = 1;
  game_ptr->byte87 = 1;
  SetGameByte107Flag();
  u8 result = result_city;

  return result;
}

// Quickstart.

#include "ai_day.h"

// This format has six consecutive unencoded fields; each read retains its own independent
// stream position.

void LoadRestaurantFromFile(Rest *rest, const u8 *filename) {
  OpenFileAsCurrent(filename);

  (void)ReadBytesFromCurrentFile(&rest->property->size_x, 7);
  (void)ReadBytesFromCurrentFile(&rest->floor, 32);
  (void)ReadBytesFromCurrentFile(&rest->num_furniture, 1);
  (void)ReadBytesFromCurrentFile(&rest->num_ovens, 1);

  (void)ReadBytesFromCurrentFile(rest->furniture, 9u * rest->num_furniture);

  (void)ReadBytesFromCurrentFile(rest->ovens, 9u * rest->num_ovens);
  CloseCurrentFile();
}
// . Four saved registers and sixteen local bytes. DWORD cost accumulation and BYTE counters
// reproduce the stock widths explicitly.

void InitQuickstartRestaurant(void) {
  u8 player_index, item_index;
  player_index = 0;

  while (player_index < game_ptr->num_interactive_players) {
    u8 player = player_index;
    SetCurrentPlayer(player);
    u32 furniture_cost = 0, oven_cost = 0;
    Poi *poi;
    Rest *address;
    poi = GetRandomPoiOfKind(2);
    address = AllocateNewRestaurant(player, poi);
    if (address) {
      u32 random;
      random = RandomNumber(4);
      snprintf((char *)strbuf, sizeof(strbuf), "q\\f%d.pc", (i32)(2u * random + days_in_month[0]));
      LoadRestaurantFromFile(address, strbuf);
      Rest *rest = address;
      item_index = 0;
      while (item_index < rest->num_furniture) {
        u16 price;
        price = GetPriceOfFurniture(rest->furniture[item_index].type);
        furniture_cost += price;
        ++item_index;
      }

      (void)TransferPlayerMoney(player, 2, 6, (float)furniture_cost);
      item_index = 0;
      while (item_index < rest->num_ovens) {
        u16 price;
        price = GetPriceOfFurniture(rest->ovens[item_index].type);
        oven_cost += price;
        ++item_index;
      }

      (void)TransferPlayerMoney(player, 1, 6, (float)oven_cost);
    }
    item_index = 0;
    while ((i32)item_index < 5 - (i32)days_in_month[0]) {
      InitializeQuickstartRecipes(item_index);
      ++item_index;
    }
    ++player_index;
  }
}
