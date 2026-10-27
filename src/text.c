// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
// Text.

#include "objects.h"
#include "text.h"
#include "graphics.h"
#include "render_types.h"
#include "messages.h"
#include "calendar.h"

u32 pt_strlen(const u8 *text) {
  u32 size = 0;
  while (text[size])
    ++size;
  return size;
}
u16 Canvas_TextWidth_Helper(const Font *font, PTTextSpan text) {
  u16 width = 0;
  for (unsigned i = 0; i < text.count; ++i)
    width = (u16)(width + pt_font_glyph_advance(font, pt_font_glyph_index(font, text.data[i])));
  return width;
}
u16 Canvas_TextWidth(const Canvas *canvas, PTTextSpan text) {
  return Canvas_TextWidth_Helper(text.count ? canvas->font : NULL, text);
}
u16 Canvas_TextWidth0(const u8 *text) {
  return Canvas_TextWidth(temp_canvas, pt_text_span(text, (u16)pt_strlen(text)));
}
u16 Canvas_DrawText_Helper(
    const Bitmap *target, u16 x, u16 y, u8 color, u8 background, u8 opaque, const Font *font, PTTextSpan text) {
  u16 saved_blit = in_bitblt;
  if (!in_bitblt)
    in_bitblt = 1;
  for (unsigned i = 0; i < text.count; ++i) {
    u8 index = pt_font_glyph_index(font, text.data[i]);
    u16 advance = pt_font_glyph_advance(font, index), height = font->height;
    pt_bitmap_bind_pixels(&glyph_bitmap, glyph_pixels, sizeof(glyph_pixels));
    glyph_bitmap.width = (i16)advance;
    glyph_bitmap.height = (i16)height;
    glyph_bitmap.surface.row_stride = advance;
    u8 fill = opaque ? background : UINT8_MAX;
    if (!opaque)
      transparent_color = UINT8_MAX;
    size_t tile_bytes = (size_t)advance * height;
    if (tile_bytes > sizeof(glyph_pixels))
      pt_resource_bounds_failure("glyph tile", tile_bytes, sizeof(glyph_pixels));
    memset(glyph_pixels, fill, (u16)((u32)advance * height));
    const FontGlyph *glyph = &font->glyphs[index];
    u16 ink_width = glyph->ink_width, bit_x = glyph->bit_x;
    size_t ink_end = height && ink_width ? (size_t)(height - 1u) * advance + ink_width : 0;
    if (ink_end > sizeof(glyph_pixels))
      pt_resource_bounds_failure("glyph ink", ink_end, sizeof(glyph_pixels));
    const u8 *row = ink_width && height ? font->bit_rows + (bit_x >> 3) : NULL;
    u16 stride = font->row_stride;

    u8 initial_mask = (u8)(0x80u >> (bit_x & 7));

    if (ink_width) {
      for (unsigned h = 0; h < height; ++h) {
        const u8 *bits = row;
        u8 value = *bits, mask = initial_mask;
        for (unsigned w = 0; w < ink_width; ++w) {
          if (value & mask)
            glyph_pixels[h * advance + w] = color;
          mask >>= 1;
          if (!mask) {
            mask = 0x80;
            value = *++bits;
          }
        }

        row += stride;
      }
    }
    u16 draw_x = x, draw_y = (u16)(y - (u16)font->baseline);
    x = (u16)(x + advance);
    if (opaque)
      MaybeGraphicsBitBlt(&glyph_bitmap, 0, 0, target, draw_x, draw_y, advance, height);
    else
      TransparentBitmapCopy(&glyph_bitmap, 0, 0, target, draw_x, draw_y, advance, height);
  }
  transparent_color = 0;
  if (mouse_move_pending == 1) {
    in_bitblt = 2;
    mouse_move_pending = 0;
    MoveMousePosition(pending_mouse_x, pending_mouse_y);
  }
  in_bitblt = saved_blit;
  return x;
}
u16 Canvas_DrawText(Canvas *canvas, PTTextSpan text) {
  // With count zero the helper does not dereference font or target.

  u16 x = Canvas_DrawText_Helper(text.count ? canvas->bitmap : NULL,
      (u16)canvas->position_x,
      (u16)canvas->position_y,
      canvas->color,
      canvas->background_color,
      canvas->text_background_enabled,
      text.count ? canvas->font : NULL,
      text);
  canvas->position_x = (i16)x;
  return x;
}
Canvas *SetTempCanvasPosition(i16 x, i16 y) {
  cur_list_elem_x = x;
  temp_canvas->position_x = x;
  cur_list_elem_y = y;
  temp_canvas->position_y = y;
  return temp_canvas;
}
Canvas *SetTempCanvasPositionX(i16 x) {
  cur_list_elem_x = x;
  temp_canvas->position_x = x;
  return temp_canvas;
}
void SetFontColor(u8 color) {
  temp_canvas->color = color;
}
void SetSomeFieldInCanvas(u8 value) {
  temp_canvas->text_background_enabled = value;
}
void ClearCanvasField14(void) {
  temp_canvas->bitmap_mask_flag = 0;
}
void SetCurrentFont(const Font *pointer) {
  if (pointer)
    Canvas_SetFont(temp_canvas, pointer);
  u8 spacing;
  if (pointer == font_ruby)
    spacing = 14;
  else if (pointer == some_font_ptr || pointer == font_supersma)
    spacing = 6;
  else if (pointer == font_piznumb || pointer == font_astandar)
    spacing = 9;
  else if (pointer == loaded_font)
    spacing = 15;
  else {
    ShowMessage(16, font_error_text);
    Delay(100);
    return;
  }
  game_ptr->list_element_spacing = spacing;
}
u16 DrawText(i16 x, i16 y, const u8 *text) {
  SetTempCanvasPosition(x, y);
  return Canvas_DrawText(temp_canvas, pt_text_span(text, (u16)pt_strlen(text)));
}
u16 DrawText2(i16 left, i16 right, i16 y, const u8 *text) {
  if (!left && !right) {
    left = 12;
    right = 203;
  }
  if (!y)
    y = temp_canvas->position_y;
  u32 length = pt_strlen(text);
  u16 width = Canvas_TextWidth(temp_canvas, pt_text_span(text, (u16)length));
  i32 centered = ((i32)right - left - (i16)width) / 2;
  i16 x = (i16)((u16)left + (u16)centered);
  Canvas_SetPosition(temp_canvas, x, y);
  length = pt_strlen(text);
  u16 result = Canvas_DrawText(temp_canvas, pt_text_span(text, (u16)length));
  return result;
}

void HandleMouseOver3x3Buttons(void) {
  if ((u8)game_ptr->current_button_menu == UINT8_MAX)
    return;
  const Mouse *mouse = mouse_result;
  mouse_x = mouse->x;
  mouse_y = mouse->y;
  if (IsMouseInside(228, 118, 321, 201)) {
    u8 button = MouseInsideFlagsToButton(GetMouseInsideFlags(255, 283, 143, 170));
    const u8 *hint = button_texts[(u8)game_ptr->current_button_menu][button];
    if (hint == current_button_hint)
      return;
    current_button_hint = hint;
  } else {
    if (!current_button_hint)
      return;
    current_button_hint = NULL;
  }
  Canvas *canvas = temp_canvas;
  if (current_button_hint) {
    const Font *saved_font = canvas->font;
    u8 saved_color = canvas->color, saved_mode = canvas->text_background_enabled;
    i16 saved_x = canvas->position_x, saved_y = canvas->position_y;
    canvas->text_background_enabled = 0;
    Canvas_SetFont(temp_canvas, some_font_ptr);
    temp_canvas->color = 0;
    BitBlt(temp_bitmap_84x6, 0, 0, temp_canvas->bitmap, 228, 111, 84, 6);
    DrawText2(228, 312, 115, current_button_hint);
    if (game_ptr->did_set_bitmap) {
      canvas = temp_canvas;
      Bitmap *saved_bitmap = canvas->bitmap;
      canvas->bitmap = old_bitmap;
      BitBlt(temp_bitmap_84x6, 0, 0, temp_canvas->bitmap, 228, 111, 84, 6);
      DrawText2(228, 312, 115, current_button_hint);
      temp_canvas->bitmap = saved_bitmap;
    }
    temp_canvas->text_background_enabled = saved_mode;
    Canvas_SetFont(temp_canvas, saved_font);
    temp_canvas->color = saved_color;
    temp_canvas->position_x = saved_x;
    temp_canvas->position_y = saved_y;
  } else {
    BitBlt(temp_bitmap_84x6, 0, 0, canvas->bitmap, 228, 111, 84, 6);
    if (game_ptr->did_set_bitmap)
      BitBlt(temp_bitmap_84x6, 0, 0, old_bitmap, 228, 111, 84, 6);
  }
}

// Paragraph.

Canvas *Canvas_TextRect_Helper3(i16 dx, i16 dy) {
  cur_list_elem_x = (i16)((u16)cur_list_elem_x + (u16)dx);
  cur_list_elem_y = (i16)((u16)cur_list_elem_y + (u16)dy);
  Canvas *canvas = temp_canvas;
  canvas->position_x = cur_list_elem_x;
  canvas->position_y = cur_list_elem_y;
  return temp_canvas;
}
// Animated paragraphs draw each byte twice, then poll the recorded clock.

void Canvas_TextRect_Helper2(PTTextRange text) {
  for (u32 i = 0; i < text.count; ++i) {
    Canvas_TextRect_Helper3(-1, 1);
    temp_canvas->color = (u8)(goal_ui_colors[3]);
    Canvas_DrawText(temp_canvas, pt_text_span(text.data + i, 1));
    Canvas_TextRect_Helper3(1, -1);
    temp_canvas->color = (u8)(paragraph_highlight_override ? 7 : goal_ui_colors[2]);
    Canvas_DrawText(temp_canvas, pt_text_span(text.data + i, 1));
    cur_list_elem_x = temp_canvas->position_x;
    temp_canvas->position_x = cur_list_elem_x;
    Delay(1);
  }
}

u16 Canvas_TextRect_Helper(Canvas *canvas, PTTextRange text) {
  // The empty loop has no meaningful original return.
  u16 result = 0;
  for (u32 i = 0; i < text.count; ++i)
    result = Canvas_DrawText(canvas, pt_text_span(text.data + i, 1));
  return result;
}

static void draw_line(Canvas *canvas, const u8 *text, u16 count) {
  u8 mode = game_ptr->paragraph_render_mode;
  if (!mode)
    Canvas_DrawText(canvas, pt_text_span(text, count));
  else if (mode == 1)
    Canvas_TextRect_Helper(canvas, pt_text_range(text, count));
  else if (mode == 2) {
    SetTempCanvasPosition(canvas->position_x, canvas->position_y);
    Canvas_TextRect_Helper2(pt_text_range(text, count));
  }
  // The original deliberately skips rendering for every value above 2.
}

static void line_background(Canvas *canvas, u32 left, u32 right, u16 y, u16 spacing, u8 background, unsigned extra) {
  if (!game_ptr->paragraph_background_enabled)
    return;
  canvas->color = background;
  Canvas_FillRect(canvas,
      (u32)(i32)(i16)(left - 2u),
      (u32)(i32)(i16)(y - (u16)canvas->font_height + 2u),
      (u32)(i32)(i16)right,
      (u32)(i32)(i16)(y + spacing + extra));
}

u16 Canvas_TextRect(Canvas *canvas, u8 foreground, u8 background, u8 *text, u32 first_left, u32 subsequent_left,
    u32 right, u32 initial_y) {
  u8 line[80];
  u16 spacing = (u16)(game_ptr->list_element_spacing + (i8)game_ptr->list_element_spacing_2);
  u8 first_width_character = paragraph_width_characters[0];
  u8 second_width_character = paragraph_width_characters[1];
  Canvas_TextWidth(canvas, pt_text_span(&first_width_character, 1));
  Canvas_TextWidth(canvas, pt_text_span(&second_width_character, 1));
  u16 y = (u16)initial_y;
  if (!*text) {
    return y;
  }
  u32 last = pt_strlen(text) - 1u;
  while (text[last] == ' ' && last)
    --last;
  text[last + 1u] = 0;
  canvas->text_background_enabled = 0;
  u16 offset = 0, width = 0;
  u32 words = 0, used = 0, left = first_left;
  const u8 *cursor = text, *end = text;
  bool terminated = false;
  while (!terminated) {
    left = pt_paragraph_left_edge(game_ptr, first_left, subsequent_left, y, offset);
    u16 available = (u16)(right + 1u - left);
    end = cursor;
    if (*cursor)
      do {
        do {
          ++end;
        } while (*end != ' ' && *end != '-' && *end != '\\' && *end);
      } while (*end == '-' && end[-1] == '.');
    if (end - cursor > 79u)
      end = cursor + 79u;
    u8 delimiter = *end;
    terminated = delimiter == 0;
    u16 word_width = Canvas_TextWidth(canvas, pt_text_span(cursor, (u16)(end - cursor)));
    u16 delimiter_width = Canvas_TextWidth(canvas, pt_text_span(cursor, (u16)(end - cursor + 1u)));
    u32 kept = used - (used && line[used - 1u] == '-');
    bool full = kept + (u32)(end - cursor) + 1u > 80u;
    if (((i32)(i16)width + (i16)delimiter_width > (i16)available || full) && (u16)words) {
      if (full)
        terminated = false;
      canvas->position_x = (i16)left;
      canvas->position_y = (i16)y;
      line_background(canvas, left, right, y, spacing, background, 4);
      canvas->color = foreground;
      canvas->position_x = (i16)left;
      canvas->position_y = (i16)y;
      draw_line(canvas, line, (u16)used);
      used = 0;
      width = 0;
      words = 0;
      y = (u16)(y + spacing);
      offset = (u16)(offset + spacing);
    } else {
      ++words;
      if (used && line[used - 1u] == '-')
        --used;
      do {
        line[used++] = *cursor++;
      } while (cursor <= end);
      width = (u16)(width + (delimiter == '-' ? word_width : delimiter_width));
    }
  }
  canvas->position_x = (i16)left;
  canvas->position_y = (i16)y;
  line_background(canvas, left, right, y, spacing, background, 0);
  canvas->color = foreground;
  canvas->position_x = (i16)left;
  canvas->position_y = (i16)y;
  draw_line(canvas, used ? line : cursor, (u16)(used ? used - 1u : end - cursor));
  // Return the next baseline with 16-bit wrap.

  return (u16)(y + spacing);
}

// List text.

// The repaired release handles empty input and zero-width clipping.

void DrawClippedListElement(const u8 *source, u16 maximum) {
  u8 buffer[200];
  size_t n = pt_strlen(source);
  if (n >= sizeof(buffer))
    abort();
  pt_strcpy(buffer, source);
  (void)pt_strlen(buffer);
  if (!n) {
    goto draw;
  }
  u32 i = (u32)n;
  do {
    --i;
  } while (buffer[i] == ' ' && i);
  buffer[i + 1] = 0;
  n = pt_strlen(buffer);
  if (n > maximum) {
    if (!maximum) {
      buffer[0] = 0;
    } else {
      buffer[maximum - 1] = '.';
      buffer[maximum] = 0;
    }
  }
draw:
  DrawListElement(buffer);
}

void DrawListElement(const u8 *text) {
  if (!text)
    return;
  u8 *shared = temp_string;
  size_t length = pt_strlen(text);
  if (length >= sizeof(temp_string))
    pt_resource_bounds_failure("list text", length, sizeof(temp_string));
  pt_strcpy(shared, text);
  u8 *next = shared;
  bool split = false;
  // The stock scan tests the following byte: a leading bar is literal.

  while (*next++) {
    if (*next == '|') {
      *next = 0;
      split = true;
    }
  }
  temp_canvas->position_x = cur_list_elem_x;
  temp_canvas->position_y = cur_list_elem_y;
  Canvas_DrawText(temp_canvas, pt_text_span(shared, (u16)pt_strlen(shared)));
  if (split) {
    i32 spacing = game_ptr->list_element_spacing + (i8)game_ptr->list_element_spacing_2;
    cur_list_elem_y = (i16)((u16)cur_list_elem_y + (u32)spacing);
    DrawListElement(next);
  }
  if (draw_list_elem_style == 2)
    cur_list_elem_y = (i16)((u16)cur_list_elem_y + game_ptr->list_element_spacing);
  if (draw_list_elem_style == 1) {
    u8 spacing = game_ptr->list_element_spacing;
    // Recursion overwrites the shared string. Ancestors use the final
    // segment's width, including an empty trailing segment.

    u16 width = Canvas_TextWidth(temp_canvas, pt_text_span(shared, (u16)pt_strlen(shared)));
    cur_list_elem_x = (i16)((u16)cur_list_elem_x + spacing + width);
  }
}

// Text edit.

#include "menu.h"
#include "input.h"
#include "world.h"

void BackupMenuHierarchyFlag(void) {
  menu_hierarchy_backup = game_ptr->in_menu_hierarchy;
}
void RestoreMenuHierarchyFlag(void) {
  (game_ptr->in_menu_hierarchy = menu_hierarchy_backup);
}

// Cursor-hidden pixels use the background HEIGHT as packed stride.

u16 ReadBitmapPixel(const Bitmap *bitmap, u16 x, u16 y) {

  u16 saved = in_bitblt;

  if (!in_bitblt)
    in_bitblt = 1;
  u16 result = UINT16_MAX;
  if (BitmapContainsPoint(bitmap, x, y)) {
    u16 cx = (u16)(x - mouse_pos_x - cursor_offset_x), cy = (u16)(y - mouse_pos_y - cursor_offset_y);
    if (pt_bitmap_is_screen(bitmap) && bitmap->surface.pixels == pt_graphics_pixels()->cursor && cursor_is_visible &&
        BitmapContainsPoint(cursor_bitmap, cx, cy)) {
      const Bitmap *bg = cursor_background_bitmap;
      unsigned offset = (u16)((u32)cy * (u16)bg->height + cx);
      result = *pt_surface_row(bg->surface, offset, 0, 1);
    } else {
      result = *pt_surface_row(bitmap->surface, x, y, 1);
    }
  }
  // Preserve the sampled pixel while pending motion redraws the cursor.

  if (mouse_move_pending == 1) {
    in_bitblt = 2;
    mouse_move_pending = 0;
    MoveMousePosition(pending_mouse_x, pending_mouse_y);
  }
  in_bitblt = saved;

  return result;
}
u16 Canvas_ReadPixel(const Canvas *canvas, i16 x, i16 y) {

  u16 result = ReadBitmapPixel(canvas->bitmap, (u16)x, (u16)y);

  return result;
}
static u16 text_width(const Canvas *canvas, const u8 *text, u16 count) {
  const Canvas *c = canvas;

  u16 result = Canvas_TextWidth(c, pt_text_span(text, count));

  return result;
}
// The input holds up to 100 bytes and three cursor underscores.

Game *HandleTextEditing(u8 *output, u8 limit) {

  u8 text[103];
  u8 used = 0, key = 0;

  Canvas *canvas = temp_canvas;
  u8 saved_color = (u8)canvas->color;

  u16 height = (u16)(canvas->font_height + 2u);

  u16 width = (u16)(game_ptr->text_edit_max_width + 10u);

  HideMouseCursor_();

  BackupMenuHierarchyFlag();

  game_ptr->in_menu_hierarchy = 0;

  SetTempCanvasPosition(cur_list_elem_x, cur_list_elem_y);

  u16 top = (u16)((u16)cur_list_elem_y - temp_canvas->font_height);

  Canvas_ReadPixel(temp_canvas, cur_list_elem_x, cur_list_elem_y);

  (void)BitBlt(current_bitmap, (u16)cur_list_elem_x, top, text_edit_bitmap, 0, 0, width, height);

  SetSomeFieldInCanvas(0);

  SaveAndSetInputEventFilter(2);

  ClearInputEvents();

  for (;;) {

    text[used] = '_';
    text[used + 1] = '_';
    text[used + 2] = '_';

    (void)BitBlt(text_edit_bitmap, 0, 0, current_bitmap, (u16)cur_list_elem_x, top, width, height);

    Canvas_SetPosition(temp_canvas, cur_list_elem_x, cur_list_elem_y);

    temp_canvas->color = (u8)saved_color;

    canvas = temp_canvas;

    Canvas_DrawText(canvas, pt_text_span(text, (u16)(used + 1u)));

    do {

      last_input_event = DequeueInputEvent();

      if (last_input_event)
        break;

      WaitForInputOrTimer(0);

    } while (true);
    key = (u8)last_input_event->value;

    if (key == 13)
      break;
    if (key == 8 && used) {
      used = (u8)(used - 1u);

      continue;
    }
    if (key < 32 || key > 122 ||
        (limit > 200 && !((key >= '0' && key <= '9') || (key >= 'A' && key <= 'Z') || (key >= 'a' && key <= 'z'))))
      continue;
    u16 length = text_width(temp_canvas, text, used);
    u8 next_count = (u8)(used + 1u);

    bool accept = used < pt_text_edit_capacity(limit);
    if (limit > 100 && limit <= 200) {
      length = (u16)(length + text_width(temp_canvas, &key, 1));
      accept = accept && (i16)length < game_ptr->text_edit_max_width;
    }
    if (accept) {
      text[used] = key;

      used = next_count;
    }
  }
  text[used] = 0;

  SaveAndSetInputEventFilter((u32)game_ptr->dwordF9);

  if (used) {

    memcpy(output, text, used + 1u);
  }

  RestoreMenuHierarchyFlag();

  Game *result = GraphicsRestoreCursor2();

  return result;
}
Game *AskForPizzaPrice(const u8 *prompt, u8 *output, u8 limit) {

  game_ptr->disable_staff_disasters = (u8)((u32)game_ptr->disable_staff_disasters + (u32)1u);

  SetCurrentFont(font_astandar);

  Copy320x200Bitmap(current_bitmap, old_bitmap);

  FillRect(14, 30, 42, 190, 98);

  AskForPizzaPriceHelper(temp_canvas, 30, 42, 190, 98);

  SetSomeFieldInCanvas(0);

  temp_canvas->color = 0;

  DrawText2(42, 188, 52, prompt);

  FillRect(13, 40, 67, 180, 78);

  temp_canvas->background_color = 13;

  temp_canvas->color = 0;

  SetTempCanvasPosition(42, 76);

  HandleTextEditing(output, limit);

  Copy320x200Bitmap(old_bitmap, current_bitmap);

  game_ptr->disable_staff_disasters = (u8)((u32)game_ptr->disable_staff_disasters - (u32)1u);
  return game_ptr;
}
