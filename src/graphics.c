// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
// Graphics.

#include "game.h"
#include "render_types.h"

void pt_bitmap_bind_pixels(Bitmap *bitmap, u8 *pixels, size_t capacity) {
  pt_bitmap_bind_surface(
      bitmap, (PTSurface){.pixels = pixels, .row_stride = (u16)bitmap->width, .byte_capacity = pixels ? capacity : 0});
}
void pt_bitmap_bind_source(Bitmap *bitmap, const u8 *pixels, size_t capacity) {
  pt_bitmap_bind_pixels(bitmap, (u8 *)pixels, capacity);
}

// The source count is also the output capacity. A zero return preserves
// partial writes. Lookahead reads two bytes after the logical input.

u32 CompressBackground(const u8 *source, u8 *target, u32 size) {

  u8 *output = target, *end = size ? target + size : target;
  u32 remaining = size, result = 0;
  for (;;) {
    u8 *header = output;
    if (output == end) {
      break;
    }
    ++output;
    u8 value = *source;
    bool repeated = value == source[1] && value == source[2];
    u16 count = 0;
    bool run = repeated && remaining != 1;
    if (run) {
      --remaining;
      if (output == end) {
        break;
      }
      *output++ = value;
      for (;;) {
        ++source;
        ++count;
        --remaining;
        if (!remaining || count == 127 || value != source[1])
          break;
      }
      if (remaining)
        ++source;
    } else {
      for (;;) {
        if (output == end)
          goto packet_failed;
        *output++ = value;
        ++source;
        ++count;
        --remaining;
        if (!remaining || count == 128)
          break;
        value = *source;
        if (value == source[1] && value == source[2])
          break;
      }
    }
    if (!remaining) {
      if (output == end)
        goto packet_failed;
      *output = 0x80;
      result = (u32)(output + 1 - target);
    }
    *header = run ? (u8)(count | 0x80u) : (u8)(count - 1u);
    if (!remaining)
      break;
    continue;
  packet_failed:
    break;
  }

  return result;
}

// Literal runs copy forward. The terminator returns zero.

u8 DecompressBackground(const u8 *input, u8 *output) {

  for (;;) {
    u8 control = *input++;
    if (control == 0x80) {

      return 0;
    }
    unsigned count = (control & 0x7f) + 1u;
    if (control & 0x80) {
      u8 value = *input++;
      for (unsigned i = 0; i < count; ++i)
        *output++ = value;
    } else {
      for (unsigned i = 0; i < count; ++i)
        *output++ = *input++;
    }
  }
}

// Blit.

#include "objects.h"
#include "graphics.h"
#include "input.h"
#include <assert.h>

static bool clip_axis(u16 *source, u16 *target, u16 *extent, u16 source_limit, u16 target_limit) {
  if ((i16)*source < 0) {
    u16 end = (u16)(*extent + *source);
    if ((i16)end <= 0)
      return false;
    *extent = end;
    *target = (u16)(*target - *source);
    *source = 0;
  }
  if ((i16)*target >= (i16)target_limit)
    return false;
  u16 end = (u16)(*source + *extent);
  if ((i16)end > (i16)source_limit)
    *extent = (u16)(*extent - (u16)(end - source_limit));
  if ((i16)*target < 0) {
    end = (u16)(*extent + *target);
    if ((i16)end <= 0)
      return false;
    *extent = end;
    *source = (u16)(*source - *target);
    *target = 0;
  }
  if ((i16)*source >= (i16)source_limit)
    return false;
  end = (u16)(*target + *extent);
  if ((i16)end > (i16)target_limit)
    *extent = (u16)(*extent - (u16)(end - target_limit));
  return true;
}

bool ClipBitmapCopy(const Bitmap *source, const Bitmap *target, PTBlitRect *r) {
  return source && target && clip_axis(&r->sx, &r->dx, &r->width, source->width, target->width) &&
      clip_axis(&r->sy, &r->dy, &r->height, source->height, target->height);
}

// Returns AX=0/1; its original callers use the resulting ZF.

u16 IntersectsMouseCursor(u16 x, u16 y, u16 width, u16 height) {
  const Bitmap *cursor = cursor_bitmap;
  u16 cx = (u16)(mouse_pos_x + cursor_offset_x);
  if ((i16)x < (i16)cx) {
    if ((i16)cx >= (i16)((u16)(x + width)))
      return 0;
  } else if ((i16)x >= (i16)((u16)(cx + cursor->width)))
    return 0;
  u16 cy = (u16)(mouse_pos_y + cursor_offset_y);
  if ((i16)y < (i16)cy) {
    if ((i16)cy >= (i16)((u16)(y + height)))
      return 0;
  } else if ((i16)y >= (i16)((u16)(cy + cursor->height)))
    return 0;
  return 1;
}

static void copy_pixels(const Bitmap *source, unsigned sx, unsigned sy, const Bitmap *target, unsigned dx, unsigned dy,
    unsigned width, unsigned height, bool transparent) {
  PTSurface input = source->surface, output = target->surface;
  bool masked = transparent && pt_bitmap_is_screen(target) && !pt_bitmap_is_screen(source) && target->mask_enabled;
  u8 mask_row[PT_SCREEN_WIDTH];
  for (unsigned row = 0; row < height; ++row) {
    const u8 *from = pt_surface_row(input, sx, sy + row, width);
    u8 *to = pt_surface_row(output, dx, dy + row, width);
    const u8 *coverage = masked ? pt_mask_row(pt_transparency_mask(), dx, dy + row, width, mask_row) : NULL;
    u8 key = transparent_color;

    for (unsigned col = 0; col < width; ++col) {
      u8 value = from[col];
      if ((!transparent || value != key) && (!masked || coverage[col]))
        to[col] = value;
    }
  }
}

// No clipping; the source parameter comes first despite misleading pre-existing disassembly
// argument labels. Cursor and deferred movement use the corresponding reconstructed original
// entries.

u16 BitBlt320Width(const Bitmap *source, u16 sx, u16 sy, const Bitmap *target, u16 dx, u16 dy, u16 width, u16 height) {
  assert(pt_bitmap_is_screen(source) && pt_bitmap_is_screen(target));
  u16 saved_blit = in_bitblt, saved_hidden = did_hide_mouse_cursor;
  if (!in_bitblt)
    in_bitblt = 1;
  did_hide_mouse_cursor = 0;
  u8 *from = source->surface.pixels, *to = target->surface.pixels;
  // Stock checks both rectangles even for a copy within the visible screen.
  // Popup rows can cover the cursor without their source strip touching it.

  if (cursor_is_visible && (from == pt_graphics_pixels()->cursor || to == pt_graphics_pixels()->cursor)) {
    bool source_hit = IntersectsMouseCursor(sx, sy, width, height);
    if (source_hit || IntersectsMouseCursor(dx, dy, width, height)) {
      did_hide_mouse_cursor = 1;
      HideMouseCursor();
    }
  }
  copy_pixels(source, sx & ~3u, sy, target, dx & ~3u, dy, width & ~3u, height, false);
  if (did_hide_mouse_cursor == 1)
    RestoreMouseCursor();
  did_hide_mouse_cursor = saved_hidden;
  in_bitblt = saved_blit;
  if (mouse_move_pending == 1) {
    in_bitblt = 2;
    mouse_move_pending = 0;
    MoveMousePosition(pending_mouse_x, pending_mouse_y);
    in_bitblt = saved_blit;
  }
  return 1;
}

// . Screen and resource bitmaps share the same pixel-copy path.

u16 MaybeGraphicsBitBlt(
    const Bitmap *source, u16 sx, u16 sy, const Bitmap *target, u16 dx, u16 dy, u16 width, u16 height) {
  u16 saved_blit = in_bitblt;
  if (!in_bitblt)
    in_bitblt = 1;
  PTBlitRect r = {sx, sy, dx, dy, width, height};
  if (!ClipBitmapCopy(source, target, &r)) {
    if (mouse_move_pending == 1) {
      in_bitblt = 2;
      mouse_move_pending = 0;
      MoveMousePosition(pending_mouse_x, pending_mouse_y);
    }
    in_bitblt = saved_blit;
    return 0;
  }
  u16 saved_hidden = did_hide_mouse_cursor;
  did_hide_mouse_cursor = 0;
  bool source_screen = pt_bitmap_is_screen(source), target_screen = pt_bitmap_is_screen(target);
  u8 *from = source->surface.pixels, *to = target->surface.pixels;
  u16 result = 1;
  if (source_screen && target_screen && (r.sx != r.dx || r.sy != r.dy)) {
    result = UINT16_MAX;
    goto done;
  }
  if (cursor_is_visible) {
    bool hit = source_screen && target_screen
        ? (from == pt_graphics_pixels()->cursor || to == pt_graphics_pixels()->cursor) &&
            IntersectsMouseCursor(r.dx, r.dy, r.width, r.height)
        : source_screen
        ? from == pt_graphics_pixels()->cursor && IntersectsMouseCursor(r.sx, r.sy, r.width, r.height)
        : target_screen && to == pt_graphics_pixels()->cursor && IntersectsMouseCursor(r.dx, r.dy, r.width, r.height);
    if (hit) {
      did_hide_mouse_cursor = 1;
      HideMouseCursor();
    }
  }
  copy_pixels(source, r.sx, r.sy, target, r.dx, r.dy, r.width, r.height, false);
done:
  if (did_hide_mouse_cursor == 1)
    RestoreMouseCursor();
  did_hide_mouse_cursor = saved_hidden;
  in_bitblt = saved_blit;
  if (mouse_move_pending == 1) {
    in_bitblt = 2;
    mouse_move_pending = 0;
    MoveMousePosition(pending_mouse_x, pending_mouse_y);
    in_bitblt = saved_blit;
  }
  return result;
}

// Compares source pixels with the retained transparent color.

u16 TransparentBitmapCopy(
    const Bitmap *source, u16 sx, u16 sy, const Bitmap *target, u16 dx, u16 dy, u16 width, u16 height) {
  u16 saved_blit = in_bitblt;
  if (!in_bitblt)
    in_bitblt = 1;
  PTBlitRect r = {sx, sy, dx, dy, width, height};
  u16 result = 0;
  bool clipped = ClipBitmapCopy(source, target, &r);
  if (!clipped)
    goto finish;
  u16 saved_hidden = did_hide_mouse_cursor;
  did_hide_mouse_cursor = 0;
  bool sv = pt_bitmap_is_screen(source), tv = pt_bitmap_is_screen(target);
  u8 *from = source->surface.pixels, *to = target->surface.pixels;
  // Stock skips a two-byte pop and corrupts RET here. Only the explicitly
  // labelled repaired fixture supports this original error return.

  if (sv && tv && (r.sx != r.dx || r.sy != r.dy)) {
    did_hide_mouse_cursor = saved_hidden;
    result = UINT16_MAX;
    goto finish;
  }
  if (cursor_is_visible) {
    bool hit = sv && tv ? (from == pt_graphics_pixels()->cursor || to == pt_graphics_pixels()->cursor) &&
            IntersectsMouseCursor(r.dx, r.dy, r.width, r.height)
        : sv ? from == pt_graphics_pixels()->cursor && IntersectsMouseCursor(r.sx, r.sy, r.width, r.height)
             : tv && to == pt_graphics_pixels()->cursor && IntersectsMouseCursor(r.dx, r.dy, r.width, r.height);
    if (hit) {
      did_hide_mouse_cursor = 1;
      HideMouseCursor();
    }
  }
  if (sv && tv) {
    // The original screen-to-screen transparent routine has a sparse
    // traversal: four starts, sixteen pixels between successive samples.
    // Preserve its actual output until a separately labelled game repair.

    PTSurface input = source->surface, output = target->surface;
    unsigned alignment = r.dx & 3u;
    for (unsigned start = 0; start < 4; ++start) {
      unsigned x = (r.dx & ~3u) + start * 4 + ((alignment + start) & 3u);
      unsigned columns = (r.width / 4) + (start < r.width % 4);
      u8 key = transparent_color;
      for (unsigned row = 0; row < r.height; ++row)
        for (unsigned col = 0; col < columns; ++col) {
          u8 value = *pt_surface_row(input, x + col * 16, r.dy + row, 1);
          if (value != key)
            *pt_surface_row(output, x + col * 16, r.dy + row, 1) = value;
        }
    }
  } else {
    copy_pixels(source, r.sx, r.sy, target, r.dx, r.dy, r.width, r.height, true);
  }
  if (did_hide_mouse_cursor == 1)
    RestoreMouseCursor();
  did_hide_mouse_cursor = saved_hidden;
  result = 1;
finish:
  if (mouse_move_pending == 1) {
    in_bitblt = 2;
    mouse_move_pending = 0;
    MoveMousePosition(pending_mouse_x, pending_mouse_y);
  }
  in_bitblt = saved_blit;
  return result;
}

u16 HideMouseCursor(void) {
  cursor_is_visible = 0;
  const Bitmap *backup = cursor_background_bitmap;
  return MaybeGraphicsBitBlt(backup,
      0,
      0,
      cursor_target_bitmap,
      (u16)(mouse_pos_x + cursor_offset_x),
      (u16)(mouse_pos_y + cursor_offset_y),
      backup->width,
      backup->height);
}

void RestoreMouseCursor(void) {
  u16 saved_blit = in_bitblt, saved_pending = mouse_move_pending;
  u8 saved_color = transparent_color;
  transparent_color = 0;
  in_bitblt = 2;
  mouse_move_pending = 0;
  cursor_is_visible = 0;
  const Bitmap *backup = cursor_background_bitmap;
  MaybeGraphicsBitBlt(cursor_target_bitmap,
      (u16)(mouse_pos_x + cursor_offset_x),
      (u16)(mouse_pos_y + cursor_offset_y),
      backup,
      0,
      0,
      backup->width,
      backup->height);
  const Bitmap *cursor = cursor_bitmap;
  TransparentBitmapCopy(cursor,
      0,
      0,
      cursor_target_bitmap,
      (u16)(mouse_pos_x + cursor_offset_x),
      (u16)(mouse_pos_y + cursor_offset_y),
      cursor->width,
      cursor->height);
  cursor_is_visible = 1;
  transparent_color = saved_color;
  mouse_move_pending = saved_pending;
  in_bitblt = saved_blit;
}

// Any nonzero halve_mouse_x doubles driver X. Deferred movement changes
// game coordinates only; the hardware remains at the requested position.

void ClampAndWarpMousePosition(i16 x, i16 y) {
  u16 saved_blit = in_bitblt;
  if (!in_bitblt)
    in_bitblt = 1;
  const Bitmap *target = cursor_target_bitmap;
  u16 cx = x >= target->width ? (u16)((u16)target->width - 1u) : x < 0 ? 0 : (u16)x;
  u16 cy = y >= target->height ? (u16)((u16)target->height - 1u) : y < 0 ? 0 : (u16)y;

  MoveMousePosition(cx, cy);

  pt_mouse_position_platform(halve_mouse_x ? (u16)(cx << 1) : cx, cy);
  if (mouse_move_pending == 1) {
    in_bitblt = 2;
    mouse_move_pending = 0;

    MoveMousePosition(pending_mouse_x, pending_mouse_y);
  }
  in_bitblt = saved_blit;
}

void MoveMousePosition(u16 x, u16 y) {
  u16 saved_visible = cursor_is_visible;
  if (saved_visible)
    HideMouseCursor();
  mouse_pos_x = x;
  mouse_pos_y = y;
  unsigned count = mouse_binding_count;
  for (unsigned i = 0; i < count; ++i) {
    PTMouseCoordinate *x_coordinate = mouse_position_bindings[i].x;
    x_coordinate->value = x;

    PTMouseCoordinate *y_coordinate = mouse_position_bindings[i].y;
    y_coordinate->value = y;
  }
  if (saved_visible) {
    RestoreMouseCursor();
    cursor_is_visible = saved_visible;
  }
}

// Movement is deferred only at signed blit depth one.

u16 HandleMouseDriverEvent(u16 mask, u16 x, u16 y) {
  if (halve_mouse_x == 1)
    x >>= 1;
  mouse_move_pending = 0;
  if ((mask & 1) && (x != mouse_pos_x || y != mouse_pos_y)) {
    if ((i16)in_bitblt < 1) {
      MoveMousePosition(x, y);
      mask &= 0x1e;
      if (!mask)
        mask |= 0x20;
    } else if (in_bitblt == 1) {
      mouse_move_pending = 1;
      pending_mouse_x = x;
      pending_mouse_y = y;
    }
  }
  if (input_event_filter & 2)
    PollKeyboardInput();
  if (mask & 0x1e) {
    u16 value = mask & 0x1e;
    if (value & 2)
      mouse_buttons |= 1;
    if (value & 4)
      mouse_buttons &= 2;
    if (value & 8)
      mouse_buttons |= 2;
    if (value & 16)
      mouse_buttons &= 1;
    return (u16)EnqueueInputEvent(1, value, mouse_pos_x, mouse_pos_y);
  }
  if (mask & 0x20)
    return (u16)EnqueueInputEvent(4, 1, mouse_pos_x, mouse_pos_y);
  return mask;
}

// Framebuffer.

#include "layout.h"
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>

static PTFramebufferState framebuffer;

static PTGraphicsPixels graphics_pixels;
Bitmap glyph_bitmap;
Bitmap bmp6;
Bitmap bmp15;
Bitmap bmp5;
Bitmap bmp14;
Bitmap bmp3;
Bitmap bmp11;
Bitmap bmp2;
Bitmap bmp1;
Bitmap bmp10;
Bitmap bmp12;
Bitmap bmp16;
Bitmap bmp25;
Bitmap bmp24;
Bitmap bmp_background;
Bitmap bmp_awards;
Bitmap bmp17;
Bitmap bmp_blitmap;
Bitmap mouse_bitmap_1;
Bitmap mouse_bitmap_2;

Bitmap *pt_bitmap_create(void) {
  Bitmap *object = calloc(1, sizeof(*object));
  return object;
}
void InitializeBitmap(Bitmap *bitmap, i16 width, i16 height) {
  bitmap->width = width;
  bitmap->height = height;
  pt_bitmap_bind_pixels(bitmap, NULL, 0);
  bitmap->mask_enabled = 0;
}

bool pt_framebuffer_owns(const u8 *pixels) {
  uintptr_t offset = (uintptr_t)pixels - (uintptr_t)framebuffer.pixels;
  return offset < sizeof(framebuffer.pixels);
}
void pt_bitmap_bind_surface(Bitmap *bitmap, PTSurface surface) {
  if (!bitmap || (!surface.pixels && surface.byte_capacity))
    abort();
  if (pt_framebuffer_owns(surface.pixels)) {
    size_t offset = (size_t)(surface.pixels - framebuffer.pixels);
    if (surface.row_stride != PT_SCREEN_WIDTH || surface.byte_capacity > sizeof(framebuffer.pixels) - offset)
      abort();
  }
  bitmap->surface = surface;
}

const PTGraphicsPixels *pt_graphics_pixels(void) {
  return &graphics_pixels;
}
void pt_graphics_pixels_import(PTGraphicsPixels state) {
  graphics_pixels = state;
}

void pt_graphics_set_cursor(u8 *pixels) {
  graphics_pixels.cursor = pixels;
}
void pt_graphics_set_background(PTSurface surface) {
  graphics_pixels.background = surface;
}
void pt_graphics_set_mask(PTMaskView mask) {
  graphics_pixels.mask = mask;
}

PTSurface pt_framebuffer_surface(u32 pixel_offset) {
  if (pixel_offset >= sizeof(framebuffer.pixels))
    abort();
  return (PTSurface){.pixels = framebuffer.pixels + pixel_offset,
    .row_stride = PT_SCREEN_WIDTH,
    .byte_capacity = sizeof(framebuffer.pixels) - pixel_offset};
}
static _Noreturn void surface_bounds_failure(PTSurface surface, unsigned x, unsigned y, unsigned count) {
  fprintf(stderr,
      "Pizza Tycoon: pixel range exceeds surface (x=%u y=%u count=%u stride=%u capacity=%zu)\n",
      x,
      y,
      count,
      surface.row_stride,
      surface.byte_capacity);
  abort();
}
u8 *pt_surface_row(PTSurface surface, unsigned x, unsigned y, unsigned count) {
  if (y && surface.row_stride > SIZE_MAX / y)
    surface_bounds_failure(surface, x, y, count);
  size_t displacement = (size_t)y * surface.row_stride;
  if (x > SIZE_MAX - displacement)
    surface_bounds_failure(surface, x, y, count);
  displacement += x;
  if (displacement > surface.byte_capacity || count > surface.byte_capacity - displacement)
    surface_bounds_failure(surface, x, y, count);
  if (!surface.pixels) {
    if (displacement || count)
      surface_bounds_failure(surface, x, y, count);
    return NULL;
  }
  // Even a caller-supplied view cannot extend the owned frame array.

  if (pt_framebuffer_owns(surface.pixels)) {
    size_t offset = (size_t)(surface.pixels - framebuffer.pixels);
    size_t remaining = sizeof(framebuffer.pixels) - offset;
    if (displacement > remaining || count > remaining - displacement)
      surface_bounds_failure(surface, x, y, count);
  }
  return surface.pixels + displacement;
}
const u8 *pt_mask_row(PTMaskView mask, unsigned x, unsigned y, unsigned count, u8 *scratch) {
  if (mask.repeat == 1)
    return pt_surface_row(mask.surface, x, y, count);
  if (mask.repeat != 4)
    abort();
  if (!count)
    return scratch;
  if (!scratch)
    abort();
  if (count - 1u > UINT_MAX - x)
    surface_bounds_failure(mask.surface, x, y, count);
  unsigned first = x / mask.repeat, last = (x + count - 1) / mask.repeat;
  const u8 *row = pt_surface_row(mask.surface, first, y, last - first + 1);
  for (unsigned i = 0; i < count; ++i)
    scratch[i] = row[(x + i) / mask.repeat - first];
  return scratch;
}
void pt_framebuffer_reset(void) {
  graphics_pixels = (PTGraphicsPixels){0};
  memset(&framebuffer, 0, sizeof(framebuffer));
}
void pt_framebuffer_start(void) {
  memset(framebuffer.pixels, 0, sizeof(framebuffer.pixels));
  memset(framebuffer.palette_known, 0, sizeof(framebuffer.palette_known));
  framebuffer.display_offset = 0;
  framebuffer.active = 1;
}
void pt_framebuffer_stop(void) {
  framebuffer.active = 0;
}
bool pt_framebuffer_ready(void) {
  return framebuffer.active == 1;
}
void pt_framebuffer_select(PTSurface surface) {
  uintptr_t offset = (uintptr_t)surface.pixels - (uintptr_t)framebuffer.pixels;
  if (surface.row_stride != PT_SCREEN_WIDTH || offset > PT_SURFACE_CAPACITY - PT_SCREEN_PIXELS ||
      surface.byte_capacity < PT_SCREEN_PIXELS)
    abort();
  framebuffer.display_offset = (u32)offset;
}
void pt_framebuffer_palette(unsigned start, const void *source, size_t components) {
  if (!source && components)
    abort();
  const u8 *colors = source;

  unsigned component = (start & 255u) * 3u;
  for (size_t i = 0; i < components; ++i) {
    framebuffer.palette[component / 3u][component % 3u] = colors[i] & 63u;
    framebuffer.palette_known[component / 3u][component % 3u] = 1;
    component = (component + 1) % sizeof(framebuffer.palette);
  }
}
int pt_framebuffer_load(const void *data, size_t size) {
  if (!data || size != sizeof(framebuffer))
    return 0;
  PTFramebufferState next;
  memcpy(&next, data, sizeof(next));
  if (next.active > 1 || next.display_offset > PT_SURFACE_CAPACITY - PT_SCREEN_PIXELS)
    return 0;
  for (unsigned i = 0; i < 768; ++i)
    if (next.palette[i / 3u][i % 3u] > 63 || next.palette_known[i / 3u][i % 3u] > 1)
      return 0;
  framebuffer = next;

  return 1;
}

const u8 *pt_framebuffer_pixels(void) {
  return framebuffer.pixels + framebuffer.display_offset;
}
size_t pt_framebuffer_rgb_size(void) {
  return PT_SCREEN_PIXELS * 3u;
}
int pt_framebuffer_rgb(u8 *rgb, size_t size) {
  if (!rgb || size != pt_framebuffer_rgb_size() || !pt_framebuffer_ready())
    return 0;
  const u8 *pixels = pt_framebuffer_pixels();
  for (unsigned i = 0; i < PT_SCREEN_PIXELS; ++i) {
    unsigned index = pixels[i];
    for (unsigned c = 0; c < 3; ++c) {
      if (!framebuffer.palette_known[index][c])
        return 0;
      unsigned value = framebuffer.palette[index][c];
      *rgb++ = (u8)((value << 2) | (value >> 4));
    }
  }
  return 1;
}

// Graphics objects.

#include "initial_data.h"
#include "native_globals.h"

// Native graphics object ownership; independent of saved process images.

MousePositionBinding mouse_position_bindings[PT_MOUSE_POSITION_BINDINGS];
void pt_mouse_bindings_reset(void) {
  for (unsigned i = 0; i < PT_MOUSE_POSITION_BINDINGS; ++i)
    mouse_position_bindings[i] = (MousePositionBinding){0};
}

Font *loaded_font;
Bitmap bitmap26;
Bitmap bitmap21;
Bitmap bitmap_text_edit;
Bitmap bitmap20;
Bitmap bitmap22;
Bitmap bitmap_character_face;
Font *font_cstandar;
Font *exit_optional_font;
Font *font_astandar;
Font *some_font_ptr;
Font *font_supersma;
Font *font_ruby;
Font *font_piznumb;
Canvas *temp_canvas;
Canvas *canvas1_ptr;
Canvas *canvas_1;
Canvas *canvas_3;
Bitmap *bmp25_ptr;
Bitmap *bmp24_ptr;
Bitmap *bmp_character_face;
Bitmap *bmp22_ptr;
Bitmap *bmp21_ptr;
Bitmap *bmp20_ptr;
Bitmap *bmp26_ptr;
Bitmap *text_edit_bitmap;
Bitmap *temp_bitmap_84x6;
Bitmap *bmp16_ptr;
Bitmap *current_bitmap;
Bitmap *old_bitmap;
Bitmap *cursor_bitmap;
Bitmap *cursor_background_bitmap;
Bitmap *cursor_target_bitmap;
Bitmap *largest_map_column_bitmaps[2];
Bitmap *map_row_bitmaps[2];
Bitmap *largest_map_row_bitmaps[2];
Bitmap *map_column_bitmaps[2];
Bitmap *display_bitmaps[2];

static bool borrows_buffer(const void *pointer, const void *buffer, size_t size) {
  return pointer && (uintptr_t)pointer - (uintptr_t)buffer < size;
}
static void retire_surface(PTSurface *surface, const void *buffer, size_t size) {
  if (borrows_buffer(surface->pixels, buffer, size))
    *surface = (PTSurface){0};
}
void pt_graphics_retire_buffer(const void *buffer, size_t size) {
  Bitmap *const bitmaps[] = {&glyph_bitmap,
    &bmp6,
    &bitmap26,
    &bitmap21,
    &bitmap_text_edit,
    &bitmap20,
    &bitmap22,
    &bitmap_character_face,
    &bmp15,
    &bmp5,
    &bmp14,
    &bmp3,
    &bmp11,
    &bmp2,
    &bmp1,
    &bmp10,
    &bmp12,
    &bmp16,
    &bmp25,
    &bmp24,
    &bmp_background,
    &bmp_awards,
    &bmp17,
    &bmp_blitmap,
    &mouse_bitmap_1,
    &mouse_bitmap_2,
    temp_bitmap_84x6,
    jatoba_1 ? jatoba_1->bitmap : NULL};
  for (size_t i = 0; i < sizeof(bitmaps) / sizeof(*bitmaps); ++i)
    if (bitmaps[i])
      retire_surface(&bitmaps[i]->surface, buffer, size);
  Canvas *const canvases[] = {&canvas1, canvas_1, canvas_3};
  for (size_t i = 0; i < sizeof(canvases) / sizeof(*canvases); ++i)
    if (canvases[i] && borrows_buffer(canvases[i]->pattern, buffer, size))
      canvases[i]->pattern = NULL;
  PTGraphicsPixels pixels = *pt_graphics_pixels();
  retire_surface(&pixels.background, buffer, size);
  retire_surface(&pixels.mask.surface, buffer, size);
  if (borrows_buffer(pixels.cursor, buffer, size))
    pixels.cursor = NULL;
  pt_graphics_pixels_import(pixels);
  if (jatoba_1 && jatoba_1->pal_p && jatoba_1->pal_p->palette &&
      borrows_buffer(jatoba_1->pal_p->palette->colors, buffer, size))
    jatoba_1->pal_p->palette->colors = NULL;
}
void pt_native_graphics_release(void) {
  free(canvas_1);
  canvas_1 = NULL;
  free(temp_bitmap_84x6);
  temp_bitmap_84x6 = NULL;
  temp_canvas = NULL;
  canvas_3 = NULL;
  if (jatoba_1) {
    if (jatoba_1->mouse) {
      jatoba_1->mouse->parent = NULL;
      jatoba_1->mouse->canvas = NULL;
    }
    pt_mouse_bindings_reset();
    free(jatoba_1->pal_p->palette);
    free(jatoba_1->pal_p->juice);
    free(jatoba_1);
    jatoba_1 = NULL;
    mouse_initializer.parent = NULL;
    pt_palette_state_reset();
  }
}

// Fresh drawing state. A stopped game may discard its old storage bindings
// before these objects are rebound; cold imports use their own decoding path.

void pt_native_graphics_defaults_apply(void) {
  glyph_bitmap = (Bitmap){.surface = {.pixels = glyph_pixels, .byte_capacity = sizeof(glyph_pixels)}};
  bmp6 = (Bitmap){0};
  bitmap26 = (Bitmap){0};
  bitmap21 = (Bitmap){0};
  bitmap_text_edit = (Bitmap){0};
  bitmap20 = (Bitmap){0};
  bitmap22 = (Bitmap){0};
  bitmap_character_face = (Bitmap){0};
  bmp15 = (Bitmap){0};
  bmp5 = (Bitmap){0};
  bmp14 = (Bitmap){0};
  bmp3 = (Bitmap){0};
  bmp11 = (Bitmap){0};
  bmp2 = (Bitmap){0};
  bmp1 = (Bitmap){0};
  bmp10 = (Bitmap){0};
  bmp12 = (Bitmap){0};
  bmp16 = (Bitmap){0};
  bmp25 = (Bitmap){0};
  bmp24 = (Bitmap){0};
  bmp_background = (Bitmap){0};
  bmp_awards = (Bitmap){0};
  bmp17 = (Bitmap){0};
  bmp_blitmap = (Bitmap){0};
  mouse_bitmap_1 = (Bitmap){0};
  mouse_bitmap_2 = (Bitmap){0};
  canvas1 = (Canvas){0};
  some_juice = (Juice){0};
  pal_pointer = (PalettePointer){0};
}

void pt_native_graphics_root_defaults_apply(void) {
  loaded_font = NULL;
  font_cstandar = NULL;
  exit_optional_font = NULL;
  font_astandar = NULL;
  some_font_ptr = NULL;
  font_supersma = NULL;
  font_ruby = NULL;
  font_piznumb = NULL;
  temp_canvas = NULL;
  canvas1_ptr = NULL;
  canvas_1 = NULL;
  canvas_3 = NULL;
  bmp25_ptr = &bmp25;
  bmp24_ptr = &bmp24;
  bmp_character_face = &bitmap_character_face;
  bmp22_ptr = &bitmap22;
  bmp21_ptr = &bitmap21;
  bmp20_ptr = &bitmap20;
  bmp26_ptr = &bitmap26;
  text_edit_bitmap = &bitmap_text_edit;
  temp_bitmap_84x6 = NULL;
  bmp16_ptr = NULL;
  current_bitmap = NULL;
  old_bitmap = NULL;
  cursor_bitmap = NULL;
  cursor_background_bitmap = NULL;
  cursor_target_bitmap = NULL;
  for (size_t i = 0; i < sizeof(largest_map_column_bitmaps) / sizeof(*largest_map_column_bitmaps); ++i)
    largest_map_column_bitmaps[i] = NULL;
  for (size_t i = 0; i < sizeof(map_row_bitmaps) / sizeof(*map_row_bitmaps); ++i)
    map_row_bitmaps[i] = NULL;
  for (size_t i = 0; i < sizeof(largest_map_row_bitmaps) / sizeof(*largest_map_row_bitmaps); ++i)
    largest_map_row_bitmaps[i] = NULL;
  for (size_t i = 0; i < sizeof(map_column_bitmaps) / sizeof(*map_column_bitmaps); ++i)
    map_column_bitmaps[i] = NULL;
  for (size_t i = 0; i < sizeof(display_bitmaps) / sizeof(*display_bitmaps); ++i)
    display_bitmaps[i] = NULL;
}

// Game bitmaps.

#include "runtime_state.h"
#include "world.h"
#include "text.h"
#include "character.h"
#include "files.h"
#include "sound.h"
#include "menu.h"
#include "frame.h"
#include "calendar.h"

void SetBitmapBuffer(Bitmap *bitmap, u8 *buffer) {
  if (pt_framebuffer_owns(buffer)) {
    // A smaller image still uses the frame page's row stride and extent.

    size_t offset = (size_t)(buffer - pt_display_surface(PT_DISPLAY_PRIMARY).pixels);
    pt_bitmap_bind_surface(bitmap, pt_framebuffer_surface((u32)offset));
  } else
    pt_bitmap_bind_pixels(bitmap, buffer, (size_t)(u16)bitmap->width * (u16)bitmap->height);
}

// The descriptor remains owned by its caller; release only its payload.

void FreeBitmap(Bitmap *bitmap) {
  if (bitmap && bitmap->surface.pixels) {
    pt_buffer_release(bitmap->surface.pixels);
    pt_bitmap_bind_pixels(bitmap, NULL, 0);
  }
}

static _Noreturn void exit_process_cleanup(void) {
  FreeBitmap(bmp24_ptr);
  FreeBitmap(bmp25_ptr);
  FreeBitmap(bmp16_ptr);
  FreeBitmap(&bmp_awards);
  FreeBitmap(bmp20_ptr);
  FreeBitmap(&bmp17);
  if (buffer_backup)
    pt_buffer_release(buffer_backup);
  if (mouse_result)
    MouseFunc16(mouse_result);
  if (jatoba_1)
    ResetVideoMode();
  Font *const fonts[] = {exit_optional_font, font_cstandar, font_piznumb, font_astandar, font_supersma};
  for (unsigned i = 0; i < 5; ++i)
    if (fonts[i])
      FreeFont(fonts[i]);
  if (buffer_2)
    pt_buffer_release(buffer_2);
  if (recipe_lookup_fallback)
    pt_simulation_release(recipe_lookup_fallback, pt_recipe_allocation_bytes(PT_RECIPE_EDITOR_MAX_ENTRIES));

  GuestExit(exit_code == 0 ? 1 : 0);
}

// The labelled startup repair skips the unsafe pre-canvas reset.

_Noreturn void ExitProcessAfterGraphicsMode(void) {
  exit_process_cleanup();
}

_Noreturn void ExitProcess(i32 unused) {
  (void)unused;
  InitGraphicsMode();
  exit_process_cleanup();
}

void InitializeAndAllocBitmap(Bitmap *bitmap, i8 unused, i16 width, i16 height) {
  (void)unused;
  InitializeBitmap(bitmap, width, height);
  u32 size = (u32)(u16)bitmap->width * (u16)bitmap->height;
  u8 *pixels = AllocateBufferOrMessage(size);
  if (pixels)
    SetBitmapBuffer(bitmap, pixels);
}

void Game_CreateBitmaps(Game *state) {
  state->allow_save_game = 1;
  state->current_palette = 254;
  state->paragraph_background_enabled = 1;
  state->byte2E = 0;
  game_ptr->legacy_word10D = 308;
  state->text_edit_max_width = 149;
  state->byte106 = 1;
  jatoba_1 = CreateJatoba(&jatoba_initializer);
  mouse_initializer.parent = jatoba_1;
  mouse_result = InitializeMouse(&mouse_initializer);
  if (!mouse_result) {
    Delay(100);
    // The original failure precedes canvas initialization. The labelled
    // repair redirects only this CALL to the safe cleanup label.

    ExitProcessAfterGraphicsMode();
  }
  InitializeBitmap(&bmp2, 320, 200);
  pt_bitmap_bind_surface(&bmp2, pt_display_surface(PT_DISPLAY_PRIMARY));
  InitializeBitmap(&bmp1, 320, 200);
  pt_bitmap_bind_surface(&bmp1, pt_display_surface(PT_DISPLAY_SECONDARY));
  display_bitmaps[0] = &bmp2;
  current_bitmap = &bmp2;
  display_bitmaps[1] = &bmp1;
  old_bitmap = &bmp1;

  temp_canvas = mouse_result->canvas;

  canvas_1 = pt_canvas_create();
  Canvas_Init(canvas_1);

  canvas_1->bitmap = temp_canvas->bitmap;
  Canvas_SetFont(canvas_1, font_cstandar);
  GameBitmapSetupNoOp();
  pt_palette_set_active(jatoba_1->pal_p);
  pt_game_bind_palette_slot(state, 0, GetPalettePointerAddy());
  pt_game_bind_palette_slot(state, 1, &temp_palette_slot);
  SetupTempPalPointer();
  Canvas_SetByte10(temp_canvas, 0);
  state->current_palette = 0;
  state->current_bitmap_index = 1;
  InitGraphicsMode();
  PTGraphicsScratch *scratch = (PTGraphicsScratch *)(buffer_2);
  ingredient_background = scratch->ingredient_pixels;
  SetFontColor(0);
  Canvas_SetPosition(temp_canvas, 0, 0);
  Canvas_ClearWholeBitmap_(temp_canvas);
  SelectPalette(70);

  CopyBitmap2to1();
  temp_hazel.actor = &temp_igloo;
  temp_igloo.sprite = &temp_hazel;
  for (unsigned i = 0; i < 150; ++i) {
    pt_hazelnut_pool_at(i)->actor = pt_igloo_pool_at(i);
    pt_igloo_pool_at(i)->sprite = pt_hazelnut_pool_at(i);
  }
  LoadBobFile(&bob_stanbobs);
  state->byte111 = 0;
  u32 seed = GuestTime(0);
  (void)pt_srand(seed);
  recipe_lookup_fallback =
      pt_simulation_allocate(PT_SIM_RECIPE, pt_recipe_allocation_bytes(PT_RECIPE_EDITOR_MAX_ENTRIES));
  bmp16_ptr = &bmp16;
  InitializeAndAllocBitmap(&bmp17, 5, 88, 120);
  (void)LoadBackground(startup_permap_filename, &bmp17);
  InitializeAndAllocBitmap(bmp20_ptr, 5, 64, 64);
  InitializeAndAllocBitmap(&bmp_awards, 5, 256, 256);
  InitializeBitmap(&bmp_blitmap, 160, 114);
  SetBitmapBuffer(&bmp_blitmap, bmp_awards.surface.pixels);
  state->last_loaded_blitmap_index = 100;
  canvas1_ptr = &canvas1;
  Canvas_Init(&canvas1);
  canvas1.bitmap = &bmp_blitmap;
  InitializeAndAllocBitmap(bmp16_ptr, 3, 256, 78);
  (void)LoadBackground(startup_icons_filename, bmp16_ptr);
  InitializeBitmap(bmp_character_face, 64, 60);
  SetBitmapBuffer(bmp_character_face, buffer_2);
  InitializeBitmap(bmp26_ptr, 48, 36);
  SetBitmapBuffer(bmp26_ptr, scratch->bitmap26);
  InitializeBitmap(text_edit_bitmap, 150, 20);
  SetBitmapBuffer(text_edit_bitmap, buffer_2);
  largest_map_row_bitmaps[0] = &bmp5;
  largest_map_row_bitmaps[1] = &bmp14;
  InitializeBitmap(&bmp5, 192, 16);
  InitializeBitmap(largest_map_row_bitmaps[1], 192, 16);
  SetBitmapBuffer(largest_map_row_bitmaps[0], buffer_2);
  SetBitmapBuffer(largest_map_row_bitmaps[1], scratch->large_map_alternate);
  largest_map_column_bitmaps[0] = &bmp6;
  largest_map_column_bitmaps[1] = &bmp15;
  InitializeBitmap(&bmp6, 16, 192);
  SetBitmapBuffer(largest_map_column_bitmaps[0], buffer_2);
  InitializeBitmap(largest_map_column_bitmaps[1], 16, 192);
  SetBitmapBuffer(largest_map_column_bitmaps[1], scratch->large_map_alternate);
  map_row_bitmaps[0] = &bmp3;
  map_row_bitmaps[1] = &bmp11;
  map_column_bitmaps[0] = &bmp10;
  map_column_bitmaps[1] = &bmp12;
  InitializeBitmap(&bmp3, 192, 4);
  SetBitmapBuffer(map_row_bitmaps[0], buffer_2);
  InitializeBitmap(map_row_bitmaps[1], 192, 4);
  SetBitmapBuffer(map_row_bitmaps[1], scratch->map_row_alternate);
  InitializeBitmap(map_column_bitmaps[0], 8, 128);
  SetBitmapBuffer(map_column_bitmaps[0], buffer_2);
  InitializeBitmap(map_column_bitmaps[1], 8, 128);
  SetBitmapBuffer(map_column_bitmaps[1], scratch->map_column_alternate);
  state->background = &bmp_background;
  InitializeBitmap(state->background, 192, 176);
  SetBitmapBuffer(&bmp_background, AllocateBufferOrMessage(33792));
  InitializeAndAllocBitmap(bmp24_ptr, 5, 96, 22);
  InitializeAndAllocBitmap(bmp25_ptr, 5, 96, 11);
  InitializeBitmap(bmp22_ptr, 64, 46);
  SetBitmapBuffer(bmp22_ptr, buffer_2);
  InitializeBitmap(bmp21_ptr, 100, 48);
  SetBitmapBuffer(bmp21_ptr, buffer_2);
  (void)LoadBackground(startup_k0_filename, bmp25_ptr);
  state->sound_enabled = 0;
  InitSound();
  u16 skip_intro = WantSkipGameIntro();
  if (!skip_intro)
    ShowGameIntro();
  static BobHost *const firsts[] = {&bob_restaurant,
    &bob_restaurant_open,
    &bob_restaurant_open,
    &bob_restaurant_open,
    &bob_restaurant_open,
    &bob_restaurant_open,
    &bob_restaurant_open};
  static BobHost *const seconds[] = {
    &bob_ingredients, &bob_kitchen, &menu_bob, &bob_typ, &bob7, &bob_restaurant_stats, &bob9};
  for (unsigned i = 0; i < 7; ++i) {
    ShareBobSpace(firsts[i], seconds[i]);
  }
  state->byteAA = 255;
  bob11.data = bmp_blitmap.surface.pixels;
  bob11.flags = 2;
  bob_walk.data = bmp_blitmap.surface.pixels;
  state->byteAB = 1;
  (void)LoadBackground(startup_med1_filename, bmp20_ptr);
  u32 allocated = AllocateSoundBuffer();
  state->sound_enabled = (allocated & 65535u) != 0;
}

void GameBitmapSetupNoOp(void) {}

// Presentation.

void WaitForRetrace(void) {
  pt_frame_wait_retrace_boundary();
}

u16 UploadPalette(u16 start, u16 count, const PTRGB *data) {
  if (!count)
    // Stock LOOP wraps to 2^32 iterations and reads unowned data.
    abort();
  size_t components = (size_t)count * 3;
  const PTRGB *colors = data;
  pt_framebuffer_palette(start, colors, components);
  u8 last = colors[count - 1u].blue;
  return (u16)((start & 0xff00) | last);
}

// Selects the displayed pixels and moves the cursor's retained background between screen
// descriptors, preserving all blit flags.

void SelectDisplayBitmap(const Bitmap *bitmap) {
  u16 saved_blit = in_bitblt;
  const Bitmap *previous = cursor_target_bitmap;
  const Bitmap *next = bitmap;
  if (next != previous) {
    pt_framebuffer_select(bitmap->surface);
    pt_frame_display_retrace_boundary();
    in_bitblt = 2;
    cursor_target_bitmap = (Bitmap *)next;
    if (cursor_is_visible) {
      cursor_target_bitmap = (Bitmap *)previous;
      HideMouseCursor();
      cursor_target_bitmap = (Bitmap *)next;
      RestoreMouseCursor();
    }
  }
  pt_graphics_set_cursor(bitmap->surface.pixels);
  in_bitblt = saved_blit;
}

void AssignPalettePointerField(PTPaletteSlot *slot, PalettePointer *value) {
  slot->pointer = value;
}

Palette *Palette_CopyFrom(PalettePointer *pointer, const PTRGB *source, i16 count) {
  Palette *palette = pointer->palette;
  PTRGB *colors = palette->colors;
  u32 bytes = (u32)((i32)count * 3);
  if (bytes)
    memmove(colors, source, bytes);

  // Reload the retained pointer in the same order as stock.

  palette = pointer->palette;
  palette->is_dirty = 1;
  return palette;
}

void WaitAndSetPalette(const PTPaletteSlot *slot) {

  const PalettePointer *pointer = slot->pointer;
  Palette *palette = pointer->palette;
  if (palette->is_dirty) {

    WaitForRetrace();

    UploadPalette(0, (u16)palette->num_entries, palette->colors);

    palette->is_dirty = 0;
  }
  const Juice *juice = pointer->juice;

  SelectDisplayBitmap(juice->bitmap);
}

void SelectPalette(u8 index) {
  if (game_ptr->current_palette == index)
    return;

  Palette_CopyFrom(active_palette_pointer, pt_palette_bank(index), 256);

  AssignPalettePointerField(pt_game_palette_slot(game_ptr, 1), &pal_pointer);

  game_ptr->current_palette = index;

  WaitForRetrace();

  PalettePointer *pointer = active_palette_pointer;
  Palette *palette = pointer->palette;

  UploadPalette(0, (u16)palette->num_entries, palette->colors);

  palette->is_dirty = 0;
}

Game *PrepareBackBufferPalette(i8 index) {
  PalettePointer *pointer;
  if (game_ptr->current_bitmap_index)
    pointer = active_palette_pointer;
  else {
    AssignPalettePointerField(pt_game_palette_slot(game_ptr, 1), &pal_pointer);
    pointer = &pal_pointer;
  }
  Palette_CopyFrom(pointer, pt_palette_bank(index), 256);
  game_ptr->current_palette = (u8)index;
  return game_ptr;
}

void CommitBitmapAndSetPalette(i8 index) {
  game_ptr->swapped_when_setting_bitmap = 0;
  if (index) {

    PrepareBackBufferPalette(index);
  }

  WaitAndSetPalette(pt_game_palette_slot(game_ptr, game_ptr->old_bitmap_index));

  game_ptr->current_bitmap_index = (u8)((u32)game_ptr->current_bitmap_index ^ (u32)1);
  game_ptr->old_bitmap_index = (u8)((u32)game_ptr->old_bitmap_index ^ (u32)1);
  current_bitmap = display_bitmaps[game_ptr->current_bitmap_index];
  old_bitmap = display_bitmaps[game_ptr->old_bitmap_index];
}

void SetCurrentBitmap(const Bitmap *bitmap) {
  old_bitmap = current_bitmap;
  current_bitmap = (Bitmap *)bitmap;
  canvas_1->bitmap = current_bitmap;
  temp_canvas->bitmap = current_bitmap;
  game_ptr->did_set_bitmap = 1;
  game_ptr->swapped_when_setting_bitmap = game_ptr->swapped_when_setting_bitmap == 0;
}
// Changes page ownership without presenting or copying pixels.

Bitmap *SwapBitmapPages(void) {
  game_ptr->current_bitmap_index = (u8)((u32)game_ptr->current_bitmap_index ^ (u32)1);
  game_ptr->old_bitmap_index = game_ptr->current_bitmap_index ^ 1;
  current_bitmap = display_bitmaps[game_ptr->current_bitmap_index];
  canvas_1->bitmap = current_bitmap;
  temp_canvas->bitmap = current_bitmap;
  old_bitmap = display_bitmaps[game_ptr->old_bitmap_index];
  return old_bitmap;
}

u32 BitBlt(const Bitmap *source, u16 sx, u16 sy, const Bitmap *target, u16 dx, u16 dy, u16 width, u16 height) {
  Bitmap temporary;
  u32 result = 255;
  if (MaybeGraphicsBitBlt(source, sx, sy, target, dx, dy, width, height) == UINT16_MAX) {
    InitializeBitmap(&temporary, (i16)width, (i16)height);
    // Signed original extents determine allocation before clipping.

    u32 bytes = (u32)(i16)width * (u32)(i16)height;
    pt_bitmap_bind_pixels(&temporary, pt_buffer_allocate(bytes, false), bytes);
    if (!temporary.surface.pixels)
      result = 0;
    else {
      MaybeGraphicsBitBlt(source, sx, sy, &temporary, 0, 0, width, height);
      MaybeGraphicsBitBlt(&temporary, 0, 0, target, dx, dy, width, height);
      pt_buffer_release(temporary.surface.pixels);
    }
  }
  return result;
}

void Copy320x200Bitmap(const Bitmap *source, const Bitmap *target) {
  BitBlt320Width(source, 0, 0, target, 0, 0, 320, 200);
}

void CopyBitmap2to1(void) {
  Copy320x200Bitmap(current_bitmap, old_bitmap);
}

u32 DrawFromCurrentToOld(u16 left, u16 top, u16 right, u16 bottom) {
  return BitBlt(current_bitmap, left, top, old_bitmap, left, top, (u16)(right + 1u - left), (u16)(bottom + 1u - top));
}
u32 BitBltOldToCurrent(u16 left, u16 top, u16 right, u16 bottom) {
  return BitBlt(old_bitmap, left, top, current_bitmap, left, top, (u16)(right + 1u - left), (u16)(bottom + 1u - top));
}
u32 DrawMapFromCurrentToOld(void) {
  return DrawFromCurrentToOld(32, 61, 191, 180);
}
u32 DrawMapFromOldToCurrent(void) {
  return BitBltOldToCurrent(32, 61, 191, 180);
}

// Raster.

static u16 rotate_right(u16 value, unsigned count) {
  return (u16)(((u32)value >> count) | ((u32)value << (16 - count)));
}
static void finish_raster(u16 saved_blit) {
  if (mouse_move_pending == 1) {
    in_bitblt = 2;
    mouse_move_pending = 0;
    MoveMousePosition(pending_mouse_x, pending_mouse_y);
  }
  in_bitblt = saved_blit;
}
u16 BitmapContainsPoint(const Bitmap *bitmap, u16 x, u16 y) {
  return (i16)x >= 0 && (i16)x < bitmap->width && (i16)y >= 0 && (i16)y < bitmap->height;
}
u16 DrawBitmapPixel(const Bitmap *bitmap, u16 x, u16 y, u8 color) {
  u16 saved_blit = in_bitblt, result = UINT16_MAX;
  if (!in_bitblt)
    in_bitblt = 1;
  if (BitmapContainsPoint(bitmap, x, y)) {
    result = 0;
    PTSurface surface = bitmap->surface;
    bool screen = pt_bitmap_is_screen(bitmap), write = true;
    u8 coverage;
    if (screen && bitmap->mask_enabled)
      write = *pt_mask_row(pt_transparency_mask(), x, y, 1, &coverage) != 0;
    u16 cx = (u16)(x - mouse_pos_x - cursor_offset_x);
    u16 cy = (u16)(y - mouse_pos_y - cursor_offset_y);
    if (write && screen && bitmap->surface.pixels == pt_graphics_pixels()->cursor && cursor_is_visible &&
        BitmapContainsPoint(cursor_bitmap, cx, cy)) {
      const Bitmap *background = cursor_background_bitmap;
      // Stock uses the background height as this packed row stride.

      unsigned offset = (u16)((u32)cy * (u16)background->height + cx);
      PTSurface backup = background->surface;
      u8 *pixel = pt_surface_row(backup, offset, 0, 1);
      *pixel = color;

      if (*pt_surface_row(cursor_bitmap->surface, offset, 0, 1))
        write = false;
    }
    if (write) {
      u8 *pixel = pt_surface_row(surface, x, y, 1);
      *pixel = color;
    }
  }
  finish_raster(saved_blit);
  return result;
}
u16 DrawBitmapLine(const Bitmap *bitmap, u16 x0, u16 y0, u16 x1, u16 y1, u8 color, u16 pattern) {
  u16 saved_blit = in_bitblt, dx = (u16)(x1 - x0), dy = (u16)(y1 - y0);
  u16 sx = 1, sy = 1;
  if (dx & 0x8000) {
    dx = (u16)(0u - dx);
    sx = UINT16_MAX;
  }
  if (dy & 0x8000) {
    dy = (u16)(0u - dy);
    sy = UINT16_MAX;
  }
  bool vertical = (i16)dy >= (i16)dx;
  u16 major = vertical ? dy : dx, minor = vertical ? dx : dy, error = 0, mask = 0x8000;
  DrawBitmapPixel(bitmap, x0, y0, pattern & mask ? color : 0);
  mask = rotate_right(mask, 1);
  // Stock reloads AX with the major extent before its decrement/test loop.

  u16 result = major;
  for (unsigned i = 0; i < major; ++i) {
    if (vertical)
      y0 = (u16)(y0 + sy);
    else
      x0 = (u16)(x0 + sx);
    error = (u16)(error + minor);
    if (error >= major) {
      error = (u16)(error - major);
      if (vertical)
        x0 = (u16)(x0 + sx);
      else
        y0 = (u16)(y0 + sy);
    }
    result = DrawBitmapPixel(bitmap, x0, y0, pattern & mask ? color : 0);
    mask = rotate_right(mask, 1);
  }
  in_bitblt = saved_blit;
  return result;
}
void ClearWholeBitmap(const Bitmap *bitmap, u8 color) {
  u16 saved_blit = in_bitblt;
  if (!in_bitblt)
    in_bitblt = 1;
  bool screen = pt_bitmap_is_screen(bitmap);
  if (screen) {
    did_hide_mouse_cursor = 0;
    if (bitmap->surface.pixels == pt_graphics_pixels()->cursor && cursor_is_visible) {
      did_hide_mouse_cursor = 1;
      HideMouseCursor();
    }
  }
  PTSurface surface = bitmap->surface;
  unsigned width = screen ? PT_SCREEN_WIDTH : (u16)bitmap->width;
  unsigned height = screen ? PT_SCREEN_HEIGHT : (u16)bitmap->height;
  for (unsigned y = 0; y < height; ++y) {
    u8 *row = pt_surface_row(surface, 0, y, width);
    memset(row, color, width);
  }
  if (screen && did_hide_mouse_cursor == 1)
    RestoreMouseCursor();
  finish_raster(saved_blit);
}
typedef struct FillRectWords {
  u16 x, y, width, height;
} FillRectWords;
static bool clip_fill(const Bitmap *bitmap, FillRectWords *r) {
  if ((i16)r->x < 0) {
    r->width = (u16)(r->width + r->x);
    r->x = 0;
  }
  u16 end = (u16)(r->x + r->width);
  if ((i16)end > bitmap->width)
    r->width = (u16)(r->width - (u16)(end - bitmap->width));
  if ((i16)r->width <= 0)
    return false;
  if ((i16)r->y < 0) {
    r->height = (u16)(r->height + r->y);
    r->y = 0;
  }
  end = (u16)(r->y + r->height);
  if ((i16)end > bitmap->height)
    r->height = (u16)(r->height - (u16)(end - bitmap->height));
  return (i16)r->height > 0;
}
static void begin_screen_fill(const Bitmap *bitmap, FillRectWords r) {
  did_hide_mouse_cursor = 0;
  if (bitmap->surface.pixels == pt_graphics_pixels()->cursor && cursor_is_visible &&
      IntersectsMouseCursor(r.x, r.y, r.width, r.height)) {
    did_hide_mouse_cursor = 1;
    HideMouseCursor();
  }
}
void FillBitmapRect(const Bitmap *bitmap, u16 x, u16 y, u16 width, u16 height, u8 color) {
  u16 saved_blit = in_bitblt;
  if (!in_bitblt)
    in_bitblt = 1;
  FillRectWords r = {x, y, width, height};
  if (clip_fill(bitmap, &r)) {
    bool screen = pt_bitmap_is_screen(bitmap);
    if (screen)
      begin_screen_fill(bitmap, r);
    PTSurface surface = bitmap->surface;
    for (unsigned row = 0; row < r.height; ++row) {
      u8 *pixels = pt_surface_row(surface, r.x, r.y + row, r.width);
      memset(pixels, color, r.width);
    }
    if (screen && did_hide_mouse_cursor == 1)
      RestoreMouseCursor();
  }
  finish_raster(saved_blit);
}
void FillBitmapPatternRect(
    const Bitmap *bitmap, u16 x, u16 y, u16 width, u16 height, u8 color, const PTPatternRow *pattern, u8 row_exponent) {
  u16 saved_blit = in_bitblt;
  if (!in_bitblt)
    in_bitblt = 1;
  FillRectWords r = {x, y, width, height};
  if (clip_fill(bitmap, &r)) {
    u8 period_byte = (u8)(UINT32_C(1) << (row_exponent & 31));
    unsigned period = period_byte ? period_byte : 256;
    bool screen = pt_bitmap_is_screen(bitmap);
    if (screen)
      begin_screen_fill(bitmap, r);
    PTSurface surface = bitmap->surface;
    const PTPatternRow *rows = pattern;
    for (unsigned row = 0; row < r.height; ++row) {
      u8 *pixels = pt_surface_row(surface, r.x, r.y + row, r.width);
      u16 mask = 0x8000, bits = rows[row % period].bits;
      for (unsigned col = 0; col < r.width; ++col) {
        pixels[col] = bits & mask ? color : 0;
        mask = rotate_right(mask, 1);
      }
    }
    if (screen && did_hide_mouse_cursor == 1)
      RestoreMouseCursor();
  }
  finish_raster(saved_blit);
}
void Canvas_ClearWholeBitmap(const Canvas *canvas, u8 color) {
  ClearWholeBitmap(canvas->bitmap, color);
}
void Canvas_ClearWholeBitmap_(const Canvas *canvas) {
  Canvas_ClearWholeBitmap(canvas, 0);
}
u8 Canvas_LineTo(Canvas *canvas, u16 x, u16 y) {
  Bitmap *bitmap = canvas->bitmap;
  u8 saved_flag = bitmap->mask_enabled;
  bitmap->mask_enabled = (u8)canvas->bitmap_mask_flag;
  DrawBitmapLine(
      bitmap, (u16)canvas->position_x, (u16)canvas->position_y, x, y, canvas->color, (u16)canvas->line_pattern);
  canvas->position_x = (i16)x;
  canvas->position_y = (i16)y;
  bitmap->mask_enabled = saved_flag;
  return saved_flag;
}
u16 Canvas_DrawPixel(const Canvas *canvas, u16 x, u16 y) {
  Bitmap *bitmap = canvas->bitmap;
  u8 saved_flag = bitmap->mask_enabled;
  bitmap->mask_enabled = (u8)canvas->bitmap_mask_flag;
  u16 result = DrawBitmapPixel(bitmap, x, y, canvas->color);
  bitmap->mask_enabled = saved_flag;
  return result;
}
// Plot the eight symmetric points with wrapping coordinate arithmetic.

void Canvas_DrawCirclePoints(const Canvas *canvas, u32 x, u32 y, u32 center_x, u32 center_y) {
  u32 left = center_x - x, right = center_x + x;
  u32 top = center_y - y, bottom = center_y + y;
  u32 swapped_left = center_x - y, swapped_right = center_x + y;
  u32 swapped_top = center_y - x, swapped_bottom = center_y + x;
  u16 px[8] = {right, right, left, left, swapped_right, swapped_right, swapped_left, swapped_left};
  u16 py[8] = {bottom, top, bottom, top, swapped_bottom, swapped_top, swapped_bottom, swapped_top};
  for (unsigned i = 0; i < 8; ++i) {
    Canvas_DrawPixel(canvas, px[i], py[i]);
  }
}
// Tests signed low WORDs while updating full DWORDs. Unsigned operations reproduce x86 wrap
// without signed C overflow.

void Canvas_DrawCircle(const Canvas *canvas, u16 center_x, u16 center_y, u32 radius) {
  u32 x = 0, y = radius, decision = 3u - 2u * radius;
  while ((i16)((u16)x) < (i16)((u16)y)) {
    Canvas_DrawCirclePoints(
        canvas, (u32)(i32)(i16)((u16)x), (u32)(i32)(i16)((u16)y), (u32)(i32)(i16)center_x, (u32)(i32)(i16)center_y);

    if (decision & 0x8000)
      decision += 4u * x + 6u;
    else {
      decision += 4u * (x - y) + 10u;
      --y;
    }
    ++x;
  }
  if ((u16)x == (u16)y) {
    Canvas_DrawCirclePoints(
        canvas, (u32)(i32)(i16)((u16)y), (u32)(i32)(i16)((u16)y), (u32)(i32)(i16)center_x, (u32)(i32)(i16)center_y);
  }
}
void Canvas_FillRect(const Canvas *canvas, u32 left, u32 top, u32 right, u32 bottom) {
  const Bitmap *bitmap = canvas->bitmap;
  u16 width = (u16)(right - left + 1u), height = (u16)(bottom - top + 1u);
  if (canvas->pattern)
    FillBitmapPatternRect(bitmap,
        left,
        top,
        width,
        height,
        canvas->color,
        ((const PTPatternRow *)(canvas->pattern)),
        canvas->fillrect_pattern_value);
  else
    FillBitmapRect(bitmap, left, top, width, height, canvas->color);
}
u8 DefaultLineTo(u16 x, u16 y) {
  return Canvas_LineTo(temp_canvas, x, y);
}
// A zero low word in a destination coordinate selects its source coordinate.

u8 TempCanvasDrawLine(i32 sx, i32 sy, i32 dx, i32 dy) {
  if (!(u16)dx)
    dx = sx;
  if (!(u16)dy)
    dy = sy;
  Canvas_SetPosition(temp_canvas, (i16)((u16)sx), (i16)((u16)sy));
  return Canvas_LineTo(temp_canvas, (u16)dx, (u16)dy);
}
u8 RelativeLineTo(u32 x, u32 y) {
  const Canvas *canvas = temp_canvas;
  return Canvas_LineTo(temp_canvas, (u16)((u16)canvas->position_x + x), (u16)((u16)canvas->position_y + y));
}
void FillRect(u16 color, u16 left, u16 top, u16 right, u16 bottom) {
  Canvas *canvas = temp_canvas;
  u16 saved_color = canvas->color;
  if ((i16)right >= (i16)left && (i16)bottom >= (i16)top) {
    canvas->color = color;
    Canvas_FillRect(temp_canvas, (u32)(i32)(i16)left, (u32)(i32)(i16)top, (u32)(i32)(i16)right, (u32)(i32)(i16)bottom);
    temp_canvas->color = saved_color;
  }
}
// Draw through the current canvas, as the original implementation does.

u8 Canvas_DrawRect(u8 color, u16 left, u16 top, u16 right, u16 bottom) {
  temp_canvas->color = (u8)color;
  SetTempCanvasPosition((i16)left, (i16)top);
  DefaultLineTo(right, top);
  DefaultLineTo(right, bottom);
  DefaultLineTo(left, bottom);
  return DefaultLineTo(left, top);
}

// Background.

#include "messages.h"

InputEvent *FillLetterBackground(u8 mode, u8 color, u8 palette) {
  BackupCanvasAttr();
  temp_canvas->color = color;
  switch (mode) {
  case 0:
    Canvas_FillRect(temp_canvas, 12, 14, 203, 189);
    break;
  case 1:
    canvas_1->color = color;
    Canvas_FillRect(canvas_1, 228, 20, 309, 92);
    break;
  case 2:
    Canvas_FillRect(temp_canvas, 208, 70, 319, 199);
    break;
  case 3:
    Canvas_FillRect(temp_canvas, 12, 24, 206, 189);
    break;
  case 4:
    if (game_ptr->last_loaded_background != color) {
      LoadGameBackground(color);
    }
    CopyBmp6AToHere(current_bitmap);
    break;
  case 5: {
    const Bitmap *saved = current_bitmap;
    current_bitmap = game_ptr->background;
    game_ptr->last_loaded_background = 0;
    for (u8 x = 0; x < 11; ++x)
      CopyBitmapPredefinedRect(4, (u16)(16u * x), 0);
    CopyBitmapPredefinedRect(5, 176, 0);
    for (u8 y = 1; y < 11; ++y)
      CopyBitmapPredefinedRect(3, 176, (u16)(16u * y));
    for (u8 y = 1; y < 11; ++y)
      for (u8 x = 0; x < 11; ++x)
        CopyBitmapPredefinedRect((u8)RandomNumber(3), (u16)(16u * x), (u16)(16u * y));
    current_bitmap = (Bitmap *)saved;
    CopyBmp6AToHere(saved);
    break;
  }
  default:
    temp_canvas->position_x = 0;
    temp_canvas->position_y = 0;
    Canvas_ClearWholeBitmap_(temp_canvas);
    break;
  }
  if (palette < 100) {
    SelectPalette(palette);
  }
  RestoreCanvasAttrs();
  InputEvent *result = ClearInputEvents();

  return result;
}

// Events.

#include "arithmetic.h"
#include "staff.h"
#include "missions.h"
#include "finance.h"
#include "player.h"
u32 IsDayTime(void) {
  return ptr_apple->minutesx10 >= 54 && ptr_apple->minutesx10 < 126;
}
u8 GetCharacterProperty(u8 property) {
  return characters[current_player_index].prop[property];
}
void PhoneCallStub(const Peach *call) {
  (void)call;
  // The original is a no-op.
}
PTForwardLink *LinkList_getNext(const PTForwardLink *node) {
  PTForwardLink *next = node->next;
  return next->next ? next : pt_list_wrapped_first(next);
}
Poi *FindPoiFromCoords(u8 x, u8 y) {
  for (Poi *poi = pt_town_first_property(); poi; poi = pt_property_next(poi))
    if (poi->ycoord == y && poi->xcoord == x)
      return poi;
  return NULL;
}

u8 ShowDelayedMessages(void) {
  u8 showed = 0;
  u8 index = 0;
  for (; index < 16; ++index) {
    Banana *b = &bananas[index];
    if (b->is_free || b->byte8 != 1)
      continue;

    if (b->when > ptr_apple->minutesx10 || b->player_index != current_player_index)
      continue;
    u8 show = RandomChance(b->byteB);
    if (!show)
      continue;

    u8 *text = pt_delayed_message_text(b->message_slot);
    DisplayErrorMessage(b->face, text);
    MarkBananaFree(b);

    MarkMessageSlotFree(b->message_slot);
    showed = 1;
    did_terminal_stuff = 1;
  }
  u8 result = showed;

  return result;
}

void ReportToThePolice(u8 crime) {
  HandlePhoneCall(3, 1, empty_string, crime);
  did_terminal_stuff = 1;
}

u8 CheckPeriodicPhoneCalls(void) {
  u8 result = 0, cursor_was_visible, phone_index;
  Rest *saved_rest;

  if (ptr_apple->clock_flag & 7u)
    goto finish;
  if (++next_event_to_do == 4)
    next_event_to_do = 0;

  cursor_was_visible = game_ptr->mouse_cursor_visible;
  if (!cursor_was_visible)
    GraphicsRestoreCursor2();
  if (next_event_to_do == 0) {
    Player *player = player_ptr;
    phone_index = 0;
    for (; phone_index < 4; ++phone_index) {
      Peach *p = &player->peaches[phone_index];
      if (!p->at_what_time)
        continue;
      PhoneCallStub(p);
      if (p->at_what_time > ptr_apple->minutesx10)
        continue;
      saved_rest = curr_rest;
      Poi *poi = FindPoiFromCoords(p->x_coord, p->y_coord);
      if (poi)
        curr_rest = poi->restaurant;
      HandlePhoneCall(p->phone_kind, p->phone_index, end_turn_phone_text, p->phone_arg);
      curr_rest = saved_rest;
      p->at_what_time = 0;
      result = 1;
      goto done;
    }
    u32 weapons = GetSumOfBytes(player_ptr->num_weapons + 4, 4);
    if (weapons) {
      u8 detected = RandomChance(2);
      if (detected) {
        u8 dexterity = GetCharacterProperty(PT_CHARACTER_DEXTERITY);
        detected = RandomChance(100 - (i32)dexterity);
        if (detected) {
          ReportToThePolice(7);
          result = 1;
        }
      }
    }
  } else if (next_event_to_do == 1) {
    u32 daytime = IsDayTime();
    if (!(u16)daytime)
      goto done;

    PTForwardLink *node = player_ptr->restaurants.begin.next;
    for (;;) {
      if (!node->next)
        break;

      if (((Rest *)node)->restaurant_opened) {
        result = HandleAllStaffDisaster((Rest *)node);
        if (result)
          goto done;
      }
      node = node->next;
    }
  } else if (next_event_to_do == 2) {
    const Player *player = player_ptr;
    if (player->num_restaurants) {
      if (character_ptr->underworld_status < 500) {
        u8 selected = RandomChance(2);
        if (!selected)
          goto done;
        curr_rest = (Rest *)(player->restaurants).begin.next;
        u32 count = RandomNumber(50);
        for (unsigned i = 0; i < (u8)count; ++i) {
          LinkList_getNext((PTForwardLink *)curr_rest);
        }
        // Stock discards each successor, retaining the first restaurant.

        const Rest *rest = curr_rest;
        if (rest->restaurant_opened && (rest->protection_value == 1 || rest->protection_value == 200)) {
          const Poi *poi = rest->property;
          Zone *zone = FindZoneFromCoordinates(poi->xcoord, poi->ycoord);

          u8 mafia = zone->active_mafia;
          HandlePhoneCall(4, mafia, periodic_extortion_text, 3);

          result = 1;
        }
      }
    }
  }
done:
  if (!cursor_was_visible)
    HideMouseCursor_();
finish: {
  u8 answer = result;
  return answer;
}
}
static void pending_goals(void) {
  u8 goal = players[current_player_index].index_of_next_goal;
  if (goal < 100) {
    ShowStuffForReachedGoal(goal);

  } else
    for (unsigned i = goal - 100u; i < 100; ++i) {
      if (!player_goals[current_player_index].history.year[i])
        break;
      u8 kind = player_goals[current_player_index].history.kind[i];

      ShowStuffForReachedGoal(kind);
    }
  players[current_player_index].index_of_next_goal = 0;
  did_terminal_stuff = 1;
}
u8 CheckGoalStuff(void) {
  Rest *saved_rest = curr_rest;
  u8 changed = 0;

  if (players[current_player_index].index_of_next_goal) {
    pending_goals();

    return 1;
  }

  const Player *player = player_ptr;
  if (player->mission.type) {
    u8 compared = CompareDateTime(player->mission.target_month,
        player->mission.target_day,
        player->mission.target_time,
        ptr_apple->month,
        ptr_apple->day,
        ptr_apple->minutesx10);
    if (compared == 2) {
      HandleMissionCompleted(player_ptr);
      did_terminal_stuff = 1;

      return 1;
    }
  }
  // Retain the second stock test, even though CompareDateTime is pure.

  if (players[current_player_index].index_of_next_goal)
    pending_goals();
  u8 result = ShowDelayedMessages();
  changed |= result;

  if (ptr_apple->minutesx10 >= character_ptr->now) {
    result = DoAllPlayerActions();
    changed = result;
    did_terminal_stuff = 1;
  } else {
    result = CheckPeriodicPhoneCalls();
    changed |= result;
  }
  curr_rest = saved_rest;
  result = changed;

  return result;
}

static void queue_message(
    u8 player, u8 restaurant, u8 face, u8 minutes, u8 probability, const u8 *message, bool manager) {
  if (!players[player].days_away) {
    Banana *event = CreateBanana();

    if (event) {
      u8 slot = AllocateMessageSlot();

      if (!slot)
        event->is_free = 1;
      else {
        u8 *target = delayed_msg_contents[slot];
        const u8 *source = message;
        unsigned i = 0;
        for (; i < 255; ++i) {
          u8 value = source[i];
          target[i] = value;
          if (!value)
            break;
        }
        if (i == 255)
          target[i] = 0;

        event->when = minutes;
        event->byteB = probability;
        event->message_slot = slot;
        event->byte8 = (u8)manager;
        event->player_index = player;
        event->restaurant = restaurant;
        if (manager)
          event->face = face;
      }
    }
  }
}
void ShowDelayedMessage(u8 player, u8 restaurant, u8 minutes, const u8 *message) {
  queue_message(player, restaurant, 0, minutes, 100, message, false);
}
void ShowManagerMessage(u8 player, u8 restaurant, u8 face, u8 minutes, u8 probability, const u8 *message) {
  queue_message(player, restaurant, face, minutes, probability, message, true);
}

// Sprites.

Igloo *InitIglooLink(Igloo *head, Igloo *tail, IglooLink *link) {
  head->previous = NULL;
  head->next = tail;
  tail->previous = head;
  tail->next = NULL;
  link->first = head;
  link->last = tail;

  return head;
}
Igloo *AddHazelnutToCanvas(Hazelnut *hazel, Canvas *canvas) {
  Igloo *p = hazel->actor;
  Igloo *head = canvas->actors->first;
  Igloo *next = head->next;
  head->next = p;
  p->next = next;
  p->previous = head;
  next->previous = p;

  return p;
}

// Compares the signed dword at +14, combining draw_y and draw_x. JL, rather than the

// linked-list order.

static u32 drawing_key(const Igloo *igloo) {
  return (((u32)(u16)igloo->draw_x << 16) | (u16)igloo->draw_y) ^ UINT32_C(0x80000000);
}
void SortCanvasIgloo(Canvas *canvas) {
  const IglooLink *link = canvas->actors;
  Igloo *head = link->first, *tail = link->last;
  Igloo *p = head->next;
  head->next = tail;
  tail->previous = head;
  while (p->next) {
    Igloo *next = p->next, *i = head->next;
    while (i->next && drawing_key(p) >= drawing_key(i))
      i = i->next;
    p->previous = i->previous;
    p->next = i;
    i->previous = p;
    p->previous->next = p;
    p = next;
  }
}
const BobHeader *SetupHazelWithBobSprite(Hazelnut *hazel, u8 *background, BobHost *bob, u16 index) {
  if (!bob->num_entries) {
    LoadBobFile(bob);
  }
  if (index >= (u16)bob->num_entries)
    index = 0;
  const BobHeader *header = pt_resource_header_at(bob, index);
  Igloo *igloo = hazel->actor;
  igloo->w = header->w;
  igloo->h = header->h;
  igloo->flag = 0;
  if (background) {
    igloo->flag = 1;
    igloo->last_y = 320;
    igloo->last_x = igloo->last_y;
  }
  igloo->pixels = header->pixels;
  hazel->flag = 0;
  hazel->background = background;

  AddHazelnutToCanvas(hazel, temp_canvas);

  return header;
}
// Consumes signed tile bytes, wraps the remaining byte and stores only low coordinate words.
// The meaningful return is AX.

u16 DrawMapHelper6(Igloo *igloo, u8 steps) {
  u32 x = (u32)(16 * (i32)(i8)igloo->tile_column + 12);
  u32 y = (u32)(16 * (i32)(i8)igloo->tile_row + 14);
  igloo->movement_remaining = (u8)(igloo->movement_remaining - 5u * steps);
  u32 remaining = igloo->movement_remaining, result = remaining;
  switch (igloo->orientation & 3u) {
  case 0:
    x += 17u - remaining;
    y += 3;
    break;
  case 1:
    result = remaining - 11u;
    x += result;
    y += 8;
    break;
  case 2:
    result = remaining - 10u;
    y += result;
    break;
  case 3:
    x += 8;
    y += 18u - remaining;
    break;
  }
  igloo->draw_x = (i16)x;
  igloo->draw_y = (i16)y;

  return (u16)result;
}
// Initialize only the buffer in their private descriptor. The aligned blitter never reads its
// other fields.

static u16 background_copy(const Bitmap *bitmap, bool restore) {
  Bitmap backup = {0};
  pt_bitmap_bind_surface(&backup, pt_graphics_pixels()->background);

  u16 result = BitBlt320Width(restore ? &backup : bitmap, 12, 14, restore ? bitmap : &backup, 12, 14, 192, 176);

  return result;
}
u16 DrawMapHelper4(const Bitmap *bitmap) {
  return background_copy(bitmap, false);
}
u16 RepositionFurnitureHelper2(const Bitmap *bitmap) {
  return background_copy(bitmap, true);
}

void DummyFunc2(u8 column, u8 row) {
  (void)column;
  (void)row;
}
void DummyFunc3(void) {
  for (unsigned row = 0; row < 11; ++row)
    for (unsigned column = 0; column < 12; ++column)
      if ((pt_map_tile_read(map_scroll_x + column, map_scroll_y + row) & 15u) == 10)
        DummyFunc2(column, row);
}
bool pt_sprite_tile_direction(u8 tile, u8 *direction) {
  switch (tile) {
  case 0x06:
  case 0x17:
  case 0x47:
  case 0x97:
  case 0xb7:
  case 0xc6:
    *direction = 1;
    return true;
  case 0x07:
  case 0x36:
  case 0x37:
  case 0xc7:
    *direction = 2;
    return true;
  case 0x16:
  case 0xa7:
  case 0xd6:
  case 0xd7:
    *direction = 0;
    return true;
  case 0x26:
  case 0xe7:
  case 0xf7:
    *direction = 3;
    return true;
  default:
    return false;
  }
}
u8 pt_sprite_actor_tile(u8 column, u8 row) {
  return pt_map_tile_read_signed((i32)map_scroll_x + (i8)column, (i32)map_scroll_y + (i8)row);
}
u16 DrawMapHelper5(Igloo *igloo, u8 index, u8 column, u8 row, u8 direction) {
  u8 variant = (u8)RandomNumber(4), tile = pt_sprite_actor_tile(column, row);
  if (direction == 255 && !pt_sprite_tile_direction(tile, &direction))
    abort();
  igloo->actor_index = index;
  igloo->tile_column = column;
  igloo->tile_row = row;
  igloo->orientation = (u8)((4u * variant) | direction);

  SetupHazelWithBobSprite(igloo->sprite, 0, &bob_stanbobs, igloo->orientation);

  igloo->movement_remaining = (u8)(RandomNumber(15) + 1u);
  igloo->is_unused = 0;

  u16 result = DrawMapHelper6(igloo, 0);

  return result;
}
u8 DrawMapHelper3(void) {
  DrawMapHelper4(current_bitmap);

  for (unsigned i = 0; i < 150; ++i) {
    pt_igloo_pool_at(i)->is_unused = 1;
    pt_igloo_pool_at(i)->movement_pause = 0;
  }
  map_actor_count = 0;

  Zone *zone = FindZoneFromCoordinates(map_scroll_x, map_scroll_y);

  u8 density = zone->traffic_density;

  InitIglooLink(&map_igloo_head, &map_igloo_tail, &map_igloo_link);

  temp_canvas->actors = &map_igloo_link;
  for (u8 row = 0; row < 11; ++row)
    for (u8 column = 0; column < 12; ++column) {
      u8 direction;
      if (pt_sprite_tile_direction(pt_map_tile_read(map_scroll_x + column, map_scroll_y + row), &direction) &&
          RandomChance(density >> 1)) {
        DrawMapHelper5(pt_igloo_pool_at(map_actor_count), map_actor_count, column, row, 255);

        ++map_actor_count;
      }
    }

  SortCanvasIgloo(temp_canvas);

  return map_actor_count;
}
// Scans an unchecked byte index. Preserve physical reads beyond the 150-node array, but
// reject a complete 256-byte-index cycle.

u8 pt_sprite_map_igloo_unused(unsigned index) {
  return (*pt_map_igloo_availability(index));
}
u8 FindFreeMapIgloo(void) {
  u8 index = 0;
  while (!pt_sprite_map_igloo_unused(index))
    ++index;

  return index;
}
bool pt_sprite_shift_igloo(Igloo *igloo, u8 direction, u8 distance) {
  u32 pixels = 16u * distance;
  switch (direction) {
  case 0:
    igloo->draw_x = (i16)((u16)igloo->draw_x - pixels);
    igloo->last_x = (i16)((u16)igloo->last_x - pixels);
    igloo->tile_column = (u8)(igloo->tile_column - distance);
    return igloo->draw_x < 0;
  case 1:
    igloo->draw_x = (i16)((u16)igloo->draw_x + pixels);
    igloo->last_x = (i16)((u16)igloo->last_x + pixels);
    igloo->tile_column = (u8)(igloo->tile_column + distance);
    return igloo->draw_x > 203;
  case 2:
    igloo->draw_y = (i16)((u16)igloo->draw_y + pixels);
    igloo->last_y = (i16)((u16)igloo->last_y + pixels);
    igloo->tile_row = (u8)(igloo->tile_row + distance);
    return igloo->draw_y > 189;
  case 3:
    igloo->draw_y = (i16)((u16)igloo->draw_y - pixels);
    igloo->last_y = (i16)((u16)igloo->last_y - pixels);
    igloo->tile_row = (u8)(igloo->tile_row - distance);
    return igloo->draw_y < 0;
  default:
    return false;
  }
}

ScrollSpawn pt_sprite_scroll_spawn(u8 direction, u8 distance) {
  ScrollSpawn area = {.last_column = 12, .last_row = 11, .zone_column = map_scroll_x + 6, .zone_row = map_scroll_y + 5};
  area.reset = distance > (direction < 2 ? 11 : 10);
  switch (direction) {
  case 0:
    area.first_column = (u8)(12u - distance);
    area.zone_column = map_scroll_x + 12 - distance;
    break;
  case 1:
    area.last_column = distance;
    area.zone_column = map_scroll_x;
    break;
  case 2:
    area.last_row = distance;
    area.zone_row = map_scroll_y;
    break;
  case 3:
    area.first_row = (u8)(11u - distance);
    area.zone_row = map_scroll_y + 11 - distance;
    break;
  }
  return area;
}
// Redraws the newly scrolled viewport before shifting the live actor list, then visits the
// exposed region in row-major order.

PTActionResult HandleMapZoomLevel3_Helper1(u8 direction, u8 distance) {
  SetCurrentBitmap(old_bitmap);

  DrawMapAnyZoom(3, map_scroll_x, map_scroll_y);

  DrawMapHelper4(current_bitmap);

  SetCurrentBitmap(old_bitmap);

  for (Igloo *p = (&map_igloo_head)->next; p != &map_igloo_tail; p = p->next)
    if (pt_sprite_shift_igloo(p, direction, distance)) {
      p->is_unused = 1;

      HandleMapZoomLevel3_Helper4(p);
    }
  u8 result = 0;
  if (direction <= 3) {
    ScrollSpawn area = pt_sprite_scroll_spawn(direction, distance);
    if (area.reset) {
      result = DrawMapHelper3();

    } else {
      Zone *zone = FindZoneFromCoordinates(area.zone_column, area.zone_row);

      u8 density = zone->traffic_density;
      result = density;
      for (u8 row = area.first_row; row < area.last_row && row < 11; ++row)
        for (u8 column = area.first_column; column < area.last_column && column < 12; ++column) {
          result = pt_map_tile_read(map_scroll_x + column, map_scroll_y + row);
          u8 ignored;
          if (pt_sprite_tile_direction(result, &ignored)) {
            result = (u8)RandomChance(density);

            if (result) {
              u8 index = FindFreeMapIgloo();

              result = (u8)DrawMapHelper5(pt_igloo_pool_at(index), index, column, row, 255);
            }
          }
        }
    }
  }

  return direction <= 3 ? pt_action_value(result) : pt_action_object(game_ptr);
}

// instead subtracts sign-extended words in 32 bits; these distances cannot overflow int32_t.

static i16 word_distance(i16 a, i16 b) {
  return (i16)((u32)(u16)a - (u16)b);
}
static u32 wide_distance(i16 a, i16 b) {
  i32 distance = (i32)a - b;
  return (u32)(distance < 0 ? -distance : distance);
}
bool pt_sprite_blocks_movement(const Igloo *a, const Igloo *b) {
  if (a == b)
    return false;
  unsigned direction = b->orientation & 3u;
  i16 distance;
  switch (a->orientation & 3u) {
  case 0:
    distance = word_distance(b->draw_x, a->draw_x);
    if (direction == 0)
      return a->draw_y == b->draw_y && distance > 0 && distance < 11;
    return direction == 2 && distance > 10 && distance < 15 && wide_distance(a->draw_y, b->draw_y) <= 10;
  case 1:
    distance = word_distance(a->draw_x, b->draw_x);
    if (direction == 1)
      return a->draw_y == b->draw_y && distance > 0 && distance < 11;
    return direction == 2 && distance > 5 && distance < 10 && wide_distance(a->draw_y, b->draw_y) < 6;
  case 2:
    distance = word_distance(a->draw_y, b->draw_y);
    if (direction < 2)
      return distance > 5 && distance < 10 && wide_distance(a->draw_x, b->draw_x) < 10;
    return direction == 2 && a->draw_x == b->draw_x && distance > 0 && distance < 11;
  case 3:
    distance = word_distance(b->draw_y, a->draw_y);
    if (direction < 2)
      return distance > 10 && distance < 14 && wide_distance(a->draw_x, b->draw_x) < 10;
    // The original includes both opposing vertical directions here.

    return a->draw_x == b->draw_x && distance > 0 && distance < 11;
  }
  abort();
}
u32 MapIgloosBlockMovement(const Igloo *moving, const Igloo *other) {
  u32 result = pt_sprite_blocks_movement(moving, other);

  return result;
}
u32 CanMapIglooMove(const Igloo *igloo) {
  u32 result = 1;
  for (Igloo *p = (&map_igloo_head)->next; p != &map_igloo_tail; p = p->next) {
    u32 blocked = MapIgloosBlockMovement(igloo, p);

    if (blocked) {
      result = 0;
      break;
    }
  }

  return result;
}
PTActionResult RepositionFurnitureHelper1(Hazelnut *hazel, Canvas *canvas, const PalettePointer *unused) {
  (void)unused;
  Bitmap *bitmap = canvas->bitmap;

  Igloo *igloo = hazel->actor;
  Igloo *previous = igloo->previous, *next = igloo->next;
  PTActionResult result = pt_action_object(bitmap);
  if (!(hazel->flag & 1) && (igloo->flag & 1)) {
    Bitmap background;

    InitializeBitmap(&background, igloo->w, igloo->h);

    pt_bitmap_bind_pixels(&background, hazel->background, (size_t)(u16)igloo->w * (u16)igloo->h);

    result = pt_action_value(MaybeGraphicsBitBlt(
        &background, 0, 0, bitmap, (u16)igloo->last_x, (u16)igloo->last_y, (u16)igloo->w, (u16)igloo->h));
  }
  previous->next = next;
  next->previous = previous;
  // Removed nodes retain their own next/prev words.

  return result;
}
PTActionResult HandleMapZoomLevel3_Helper4(Igloo *igloo) {
  ;
  PTActionResult result = RepositionFurnitureHelper1(igloo->sprite, canvas_3, 0);

  return result;
}
u8 Canvas_DrawAllIgloos(Canvas *canvas, const void *unused_palette) {
  (void)unused_palette;

  Bitmap *bitmap = canvas->bitmap;
  Igloo *p = canvas->actors->first->next;
  u8 old_flag = bitmap->mask_enabled;
  bitmap->mask_enabled = (u8)canvas->bitmap_mask_flag;
  while (p->next) {
    Igloo *igloo = p;
    Hazelnut *hazel = igloo->sprite;
    if (!(hazel->flag & 1)) {
      Bitmap sprite;

      InitializeBitmap(&sprite, igloo->w, igloo->h);

      u16 width = (u16)igloo->w, height = (u16)igloo->h;
      if (igloo->flag & 1) {
        pt_bitmap_bind_pixels(&sprite, hazel->background, (size_t)width * height);

        MaybeGraphicsBitBlt(&sprite, 0, 0, bitmap, (u16)igloo->last_x, (u16)igloo->last_y, width, height);

        MaybeGraphicsBitBlt(bitmap, (u16)igloo->draw_x, (u16)igloo->draw_y, &sprite, 0, 0, width, height);
      }
      pt_bitmap_bind_pixels(&sprite, igloo->pixels, (size_t)width * height);

      TransparentBitmapCopy(&sprite, 0, 0, bitmap, (u16)igloo->draw_x, (u16)igloo->draw_y, width, height);

      igloo->last_x = igloo->draw_x;
      igloo->last_y = igloo->draw_y;
    }
    p = igloo->next;
  }
  bitmap->mask_enabled = old_flag;

  return old_flag;
}

IglooTurn pt_sprite_advance_tile(const Igloo *igloo) {
  IglooTurn turn = {.column = igloo->tile_column, .row = igloo->tile_row, .direction = igloo->orientation & 3u};
  switch (turn.direction) {
  case 0:
    ++turn.column;
    turn.wrap = (i8)turn.column > 11;
    break;
  case 1:
    --turn.column;
    turn.wrap = (i8)turn.column < 0;
    break;
  case 2:
    --turn.row;
    turn.wrap = (i8)turn.row < 0;
    break;
  case 3:
    ++turn.row;
    turn.wrap = (i8)turn.row > 10;
    break;
  }
  turn.spawn_column = turn.column;
  turn.spawn_row = turn.row;
  if (turn.wrap) {
    switch (turn.direction) {
    case 0:
      --turn.spawn_row;
      turn.direction = 1;
      break;
    case 1:
      ++turn.spawn_row;
      turn.direction = 0;
      break;
    case 2:
      --turn.spawn_column;
      turn.direction = 3;
      break;
    case 3:
      ++turn.spawn_column;
      turn.direction = 2;
      break;
    }
    turn.changes = true;
  }
  return turn;
}
bool pt_sprite_junction_tile(u8 tile) {
  return tile == 0x46 || tile == 0x56 || tile == 0x27 || tile == 0x66 || tile == 0x76;
}
bool pt_sprite_choose_turn(u8 tile, bool branch, u8 *direction) {
  switch (tile) {
  case 0x06:
  case 0x17:
  case 0x47:
  case 0x86:
  case 0x87:
  case 0x97:
  case 0xb7:
  case 0xc6:
    *direction = 1;
    return true;
  case 0x16:
  case 0x77:
  case 0xa6:
  case 0xa7:
  case 0xd6:
  case 0xd7:
    *direction = 0;
    return true;
  case 0x07:
  case 0x36:
  case 0x37:
  case 0x57:
  case 0x96:
  case 0xc7:
    *direction = 2;
    return true;
  case 0x26:
  case 0x67:
  case 0xb6:
  case 0xe7:
  case 0xf7:
    *direction = 3;
    return true;
  case 0x46:
    *direction = branch ? 3 : 1;
    return true;
  case 0x56:
    *direction = branch ? 0 : 3;
    return true;
  case 0x27:
  case 0x66:
    *direction = branch ? 1 : 2;
    return true;
  case 0x76:
    *direction = branch ? 2 : 0;
    return true;
  default:
    // Stock leaves its local direction uninitialized.
    return false;
  }
}
void pt_sprite_finish_turn_plan(IglooTurn *turn, u8 old_direction) {
  turn->changes = turn->direction != old_direction;
  turn->shortening = (old_direction == 0 && turn->direction == 2) || (old_direction == 2 && turn->direction == 1) ||
      (old_direction == 1 && turn->direction == 3) || (old_direction == 3 && turn->direction == 0);
}
PTActionResult HandleMapZoomLevel3_Helper3(Igloo *igloo) {
  u8 old_direction = igloo->orientation & 3u;
  IglooTurn turn = pt_sprite_advance_tile(igloo);
  igloo->tile_column = turn.column;
  igloo->tile_row = turn.row;
  PTActionResult result = pt_action_value(old_direction);
  if (turn.wrap) {
    HandleMapZoomLevel3_Helper4(igloo);

    result = pt_action_value(
        (u8)DrawMapHelper5(igloo, igloo->actor_index, turn.spawn_column, turn.spawn_row, turn.direction));

  } else {
    u8 tile = pt_sprite_actor_tile(turn.column, turn.row);
    bool branch = false;
    if (!igloo->shortened_turn && pt_sprite_junction_tile(tile)) {
      branch = RandomChance(50);
    }
    if (!pt_sprite_choose_turn(tile, branch, &turn.direction))
      abort();
    pt_sprite_finish_turn_plan(&turn, old_direction);
    if (turn.changes) {
      igloo->orientation = (u8)((igloo->orientation & 12u) + turn.direction);

      HandleMapZoomLevel3_Helper4(igloo);

      igloo->shortened_turn = turn.shortening;

      DrawMapHelper6(igloo, turn.shortening);

      result = pt_action_object(SetupHazelWithBobSprite(igloo->sprite, NULL, &bob_stanbobs, igloo->orientation));
    }
  }

  return result;
}
void HandleMapZoomLevel3_Helper2(void) {
  for (Igloo *p = (&map_igloo_head)->next; p != &map_igloo_tail; p = p->next) {
    Igloo *igloo = p;
    if (igloo->movement_pause)
      igloo->movement_pause = (u8)(igloo->movement_pause - (1u));
    else {
      bool can_move = CanMapIglooMove(igloo);

      if (can_move) {
        switch (igloo->orientation & 3u) {
        case 0:
          igloo->draw_x = (i16)((u16)igloo->draw_x + 1u);
          break;
        case 1:
          igloo->draw_x = (i16)((u16)igloo->draw_x - 1u);
          break;
        case 2:
          igloo->draw_y = (i16)((u16)igloo->draw_y - 1u);
          break;
        case 3:
          igloo->draw_y = (i16)((u16)igloo->draw_y + 1u);
          break;
        }
        if (!(igloo->movement_remaining = (u8)(igloo->movement_remaining - (1u)))) {
          igloo->movement_remaining = 16;

          HandleMapZoomLevel3_Helper3(igloo);
        }
      } else
        igloo->movement_pause = 10;
    }
    // The loop follows next AFTER a turn reinserts this node at the head;
    // it can revisit pedestrians already processed during this tick.
  }

  SortCanvasIgloo(temp_canvas);

  SetCurrentBitmap(old_bitmap);

  RepositionFurnitureHelper2(current_bitmap);

  Canvas_DrawAllIgloos(temp_canvas, active_palette_pointer);

  CommitBitmapAndSetPalette(0);
}

// Bob loader.

void UnloadLinkedBob(BobHost *bob) {

  if (bob->num_entries && bob->flags != 1) {
    bob->num_entries = 0;
    if (!bob->flags) {

      pt_buffer_release(bob->data);

      bob->data = NULL;
      bob->payload = NULL;
      pt_bob_headers_set(bob, NULL);
    }
  }
}
PTBobShareResult ShareBobSpace(BobHost *first, BobHost *second) {
  PTBobShareResult result = {.kind = PT_BOB_SHARE_BUFFER, .buffer = NULL};
  if (second->next) {
    bool hidden = !game_ptr->mouse_cursor_visible;
    DisplayErrorMessage(0, bob_second_already_linked_error);
    result = hidden ? (PTBobShareResult){.kind = PT_BOB_SHARE_GAME, .owner = game_ptr}
                    : (PTBobShareResult){.kind = PT_BOB_SHARE_INPUT_FILTER, .input_filter = input_event_filter};
    return result;
  }
  if (!first->next) {
    u32 size = (u32)first->size > (u32)second->size ? (u32)first->size : (u32)second->size;
    second->size = pt_signed_dword(size);
    first->size = pt_signed_dword(size);
    first->next = second;
    second->next = first;
    result.buffer = AllocateBufferOrMessage(size);
    if (!result.buffer) {
      return result;
    }
    second->data = result.buffer;
    first->data = result.buffer;
  } else {
    BobHost *tail = first;
    while (tail->next != first)
      tail = tail->next;
    tail->next = second;
    second->next = first;
    if ((u32)second->size > (u32)first->size)
      DisplayErrorMessage(0, bob_shared_capacity_error);
    second->size = first->size;
    result.buffer = first->data;
    second->data = result.buffer;
  }
  first->flags = 2;
  second->flags = 2;
  return result;
}
u16 LoadBobFile(BobHost *bob) {
  if (!bob->num_entries) {
    for (BobHost *member = bob->next; member && member != bob; member = member->next)
      UnloadLinkedBob(member);
    OpenFileAsCurrent(bob->filename);
    if (!bob->data)
      bob->data = (u8 *)AllocateBufferOrMessage((u32)bob->size);
    if (bob->data) {
      ReadFile(cur_loading_filehandle, bob->data, (u32)bob->size);
      PTBobFileHeader *file = pt_bob_file_header(bob);
      u8 *source = (u8 *)(file + 1);
      bob->header_source = source;
      u16 count = file->num_entries;
      bob->num_entries = (i16)count;
      BobHeader *header = pt_bob_decode_headers(bob, count);
      // Disk descriptors are seven bytes; native records own real pointers.

      u32 payload_offset = sizeof(*file) + (u32)count * PT_BOB_DISK_HEADER_BYTES;
      u32 remaining = (u32)bob->size - payload_offset;
      u8 *pixels = bob->data + payload_offset;
      bob->payload = pixels;
      if (bob->flags < 2)
        bob->dword16 = pt_signed_dword(file->opaque_value);
      for (u16 i = 0; i < (u16)bob->num_entries; ++i, ++header) {
        u16 size = (u16)((u16)header->w * header->h);
        header->pixels = pixels;
        if (size > remaining)
          pt_resource_bounds_failure("sprite pixels", size, remaining);
        pixels += size;
        remaining -= size;
      }
      CloseCurrentFile();
    } else
      bob->num_entries = 0;
  }
  return (u16)bob->num_entries;
}

// Graphics startup.

// Resource buffers are native; retain the original allocation-failure dialog.

void *AllocateBufferOrMessage(u32 size) {
  u8 *result = pt_buffer_allocate(size, true);
  if (!result && temp_canvas) {
    u8 yes = ShowMessage2(0, no_memory_continue_prompt);
    if (!yes)
      ExitProcess(0);
  }
  return result;
}

// Failed reads and allocations retain earlier blocks and the open stream,
// matching the original resource loader's partial ownership.

Font *LoadFontFile(const LoadFontDesc *descriptor) {
  GuestFileStream *stream = OpenFile(descriptor->filename, 0);
  Font *font = NULL;
  void *result = stream;
  if (stream) {
    font = pt_font_create();
    result = font;
    if (font) {
      Font *f = font;
      if (!pt_font_read_header(stream, f)) {
        result = 0;
        goto done;
      }
      u32 rows = (u32)f->height * f->row_stride;
      result = pt_buffer_allocate(rows, false);
      f->bit_rows = result;

      if (result) {
        if (ReadFile(stream, result, rows) != rows) {
          result = 0;
          goto done;
        }
        u32 glyph_size = 4u * (u32)((i32)f->last_char - f->first_char + 2);
        result = pt_buffer_allocate(glyph_size, false);
        f->glyphs = result;

        if (result) {
          if (ReadFile(stream, result, glyph_size) != glyph_size) {
            result = 0;
            goto done;
          }
          u32 advance_size = glyph_size / 2u;
          if (f->spacing_mode == 2) {
            f->advances = NULL;

          } else {
            result = pt_buffer_allocate(advance_size, false);
            f->advances = result;

            if (!result)
              goto done;
            if (ReadFile(stream, result, advance_size) != advance_size) {
              result = 0;
              goto done;
            }
          }
          CloseFile(stream);
          result = font;
        }
      }
    }
  }
done:
  return result;
}

// Only the first allocation failure precedes game cleanup. The labelled
// original-game repair uses the safe cleanup entry before canvases exist.

static _Noreturn void graphics_startup_exit(bool early) {
  if (early)
    GuestExit(0);
  ExitProcessAfterGraphicsMode();
}

u8 *LoadGraphicsFiles(void) {
  buffer_backup = AllocateBufferOrMessage(19200);
  if (!buffer_backup)
    graphics_startup_exit(true);
  font_ruby = LoadFontFile(&ruby_font);
  if (!font_ruby)
    graphics_startup_exit(false);
  font_astandar = LoadFontFile(&font_desc_astandar);
  if (!font_astandar)
    graphics_startup_exit(false);
  font_cstandar = LoadFontFile(&font_desc_cstandar);
  font_piznumb = LoadFontFile(&font_desc_piznumb);
  font_supersma = LoadFontFile(&font_desc_supersma);
  buffer_2 = AllocateBufferOrMessage(16000);
  if (!buffer_2)
    graphics_startup_exit(false);
  game_ptr->scratch_second = buffer_2;
  game_ptr->scratch_first = buffer_2;
  GuestFileStream *stream = OpenFile(graphics_palette_filename, 0);
  if (!stream)
    graphics_startup_exit(false);
  ReadFile(stream, &palette_e, sizeof(palette_e));
  CloseFile(stream);
  // Four animation colors and eleven alternate colors in the palette asset.

  (void)memmove(&map_animation_palette_colors, &palette_e[62].colors[140], sizeof(map_animation_palette_colors));
  (void)memmove(&map_animation_palette_repeat, &palette_e[62].colors[140], sizeof(map_animation_palette_repeat));
  u8 *result = memmove(&palette_e[18].colors[101], &palette_e[13].colors[101], 11u * 3u);
  return result;
}

void InitPaletteColors(void) {
  for (u8 i = 0; i < 31; ++i)
    SetPalColor2(active_palette_pointer, i, 15, 15, 15);
}

void InitializeDisplay(void) {
  pt_framebuffer_start();
  pt_graphics_set_cursor(cursor_target_bitmap ? cursor_target_bitmap->surface.pixels : NULL);
  in_bitblt = 0;
}

void SetVideMode(void) {
  pt_framebuffer_stop();
}

void ResetVideoMode(void) {
  SetVideMode();
}

// Preserve allocation order and partially created owners on failure.

Jatoba *CreateJatoba(const JatobaInitializer *initializer) {
  void *result = pt_jatoba_create();
  Jatoba *object = result;
  if (!result)
    goto done;
  Jatoba *j = object;
  j->bitmap->width = PT_SCREEN_WIDTH;
  j->bitmap->height = PT_SCREEN_HEIGHT;
  pt_bitmap_bind_surface(j->bitmap, pt_display_surface(PT_DISPLAY_PRIMARY));
  j->bitmap->mask_enabled = 0;
  Bitmap *bitmap = j->bitmap;
  cursor_target_bitmap = bitmap;
  pt_graphics_set_mask((PTMaskView){pt_display_surface(PT_DISPLAY_MASK), 1});
  pt_graphics_set_background(pt_display_surface(PT_DISPLAY_BACKGROUND));
  Canvas_Init(j->canvas);
  j->canvas->bitmap = bitmap;
  result = pt_palette_create();
  j->pal_p->palette = result;

  if (!result)
    goto done;
  Palette *palette = j->pal_p->palette;
  palette->num_entries = 256;
  result = pt_buffer_allocate(256u * 3u, false);
  palette->colors = result;

  if (!result)
    goto done;
  some_pal_p.pointer = j->pal_p;
  startup_word0 = 0;
  startup_word1 = 0;
  startup_word2 = 0;
  canvas_3 = j->canvas;
  result = pt_juice_create();
  j->pal_p->juice = result;

  if (!result)
    goto done;
  Juice *juice = result;
  Juice_Init(juice, bitmap);
  j->dword8 = initializer->field_2;

  InitializeDisplay();
  result = object;
done:
  return result;
}

void SetupTempPalPointer(void) {
  temp_palette_slot.pointer = &pal_pointer;
  pal_pointer.juice = &some_juice;
  const PalettePointer *active = active_palette_pointer;
  pal_pointer.palette = active->palette;
  some_juice.dword0 = 0;
  const Juice *juice = active->juice;
  some_juice.word8 = juice->word8;
  some_juice.wordA = juice->wordA;
  some_juice.bitmap = display_bitmaps[1];
  Palette_CopyFrom(&pal_pointer, palette_e[0].colors, 256);
  PTPaletteSlot *slot = pt_game_palette_slot(game_ptr, 1);
  AssignPalettePointerField(slot, &pal_pointer);
}

struct PTPaletteSlot *GetPalettePointerAddy(void) {
  return &some_pal_p;
}
