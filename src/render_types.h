// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
#ifndef PT_RENDER_TYPES_H
#define PT_RENDER_TYPES_H
#include "layout.h"
#include "types.h"

// Font objects.

#include <stdbool.h>
struct GuestFileStream;
// Native resource descriptors own real string pointers. Unused original
// descriptor words remain exclusively in the saved image.

typedef struct LoadFontDesc {
  const u8 *filename;
} LoadFontDesc;
extern LoadFontDesc menu_font;
extern LoadFontDesc ruby_font;
extern LoadFontDesc font_desc_cstandar;
extern LoadFontDesc font_desc_piznumb;
extern LoadFontDesc font_desc_astandar;
extern LoadFontDesc font_desc_supersma;

typedef struct __attribute__((packed, may_alias)) PTFontAdvance {
  u16 pixels;
} PTFontAdvance;
_Static_assert(sizeof(PTFontAdvance) == 2, "font advance record");
struct Font {
  u16 height;
  u8 field_2, spacing_mode;
  u16 fixed_advance;
  i16 baseline;
  u8 first_char, last_char;
  u16 row_stride;
  u8 *bit_rows;
  FontGlyph *glyphs;
  PTFontAdvance *advances;
};
Font *pt_font_create(void);
void pt_font_release(Font *font);
bool pt_font_read_header(struct GuestFileStream *stream, Font *font);

// Graphics buffers.

#include <stddef.h>

// Startup reuses these deliberately overlapping views of one allocation.

typedef union __attribute__((packed, may_alias)) PTGraphicsScratch {
  u8 bytes[16000];
  struct {
    u8 ingredient_prefix[1856];
    u8 ingredient_pixels[1];
  };
  struct {
    u8 bitmap26_prefix[11136];
    u8 bitmap26[48 * 36];
  };
  struct {
    u8 large_map_prefix[3200];
    u8 large_map_alternate[192 * 16];
  };
  struct {
    u8 map_row_prefix[1280];
    u8 map_row_alternate[192 * 4];
  };
  struct {
    u8 map_column_prefix[2048];
    u8 map_column_alternate[8 * 128];
  };
} PTGraphicsScratch;
_Static_assert(sizeof(PTGraphicsScratch) == 16000, "graphics scratch allocation");
_Static_assert(offsetof(PTGraphicsScratch, ingredient_pixels) == 1856, "ingredient scratch view");
_Static_assert(offsetof(PTGraphicsScratch, bitmap26) == 11136, "bitmap26 scratch view");
_Static_assert(offsetof(PTGraphicsScratch, large_map_alternate) == 3200, "large map alternate view");
_Static_assert(offsetof(PTGraphicsScratch, map_row_alternate) == 1280, "map row alternate view");
_Static_assert(offsetof(PTGraphicsScratch, map_column_alternate) == 2048, "map column alternate view");

// Font data.

static inline u8 pt_font_glyph_index(const Font *font, u8 character) {
  if (character < font->first_char || character > font->last_char)
    character = (u8)(font->last_char + 1u);
  return (u8)(character - font->first_char);
}
static inline u16 pt_font_glyph_advance(const Font *font, u8 index) {
  if (!font->advances)
    return font->fixed_advance;
  const PTFontAdvance *advance = &font->advances[index];
  return advance->pixels;
}

// Framebuffer.

#include <stdbool.h>
#include <stddef.h>

typedef struct PTRGB {
  u8 red, green, blue;
} PTRGB;
_Static_assert(sizeof(PTRGB) == 3 && _Alignof(PTRGB) == 1, "RGB byte layout");
_Static_assert(offsetof(PTRGB, green) == 1 && offsetof(PTRGB, blue) == 2, "RGB fields");
typedef struct PTPaletteColors {
  PTRGB colors[256];
} PTPaletteColors;
_Static_assert(sizeof(PTPaletteColors) == 768, "palette color array size");

enum {
  PT_SCREEN_WIDTH = 320,
  PT_SCREEN_HEIGHT = 200,
  PT_SCREEN_PIXELS = PT_SCREEN_WIDTH * PT_SCREEN_HEIGHT,
  PT_DISPLAY_PAGES = 4,
  PT_SURFACE_CAPACITY = PT_DISPLAY_PAGES * PT_SCREEN_PIXELS
};
// A surface borrows a byte range from its owner. Rendering needs only its
// pixels, explicit row stride and byte capacity; no address codec or access callback.

typedef struct PTSurface {
  u8 *pixels;
  unsigned row_stride;
  size_t byte_capacity;
} PTSurface;
typedef struct PTMaskView {
  PTSurface surface;
  unsigned repeat;
} PTMaskView;
// Pixel ownership is native. Snapshot codecs translate legacy storage IDs.

typedef struct Bitmap {
  i16 width, height;
  u8 mask_enabled;
  PTSurface surface;
} Bitmap;
typedef struct PTGraphicsPixels {
  u8 *cursor;
  PTSurface background;
  PTMaskView mask;
} PTGraphicsPixels;
Bitmap *pt_bitmap_create(void);
void pt_bitmap_bind_surface(struct Bitmap *bitmap, PTSurface surface);
void pt_bitmap_bind_pixels(struct Bitmap *bitmap, u8 *pixels, size_t capacity);
void pt_bitmap_bind_source(struct Bitmap *bitmap, const u8 *pixels, size_t capacity);
bool pt_framebuffer_owns(const u8 *pixels);
const PTGraphicsPixels *pt_graphics_pixels(void);
void pt_graphics_pixels_import(PTGraphicsPixels state);
void pt_graphics_set_cursor(u8 *pixels);
void pt_graphics_set_background(PTSurface surface);
void pt_graphics_set_mask(PTMaskView mask);
typedef struct PTFramebufferState {
  union {
    u8 frames[PT_DISPLAY_PAGES][PT_SCREEN_HEIGHT][PT_SCREEN_WIDTH];
    u8 pixels[PT_SURFACE_CAPACITY];
  };
  u8 palette[256][3];
  u8 palette_known[256][3];
  u32 display_offset;
  u32 active;
} PTFramebufferState;
_Static_assert(sizeof(PTFramebufferState) == PT_SURFACE_CAPACITY + 1544, "native frame storage layout");

// The framebuffer owns linear pixel offsets, independent of bitmap IDs.

PTSurface pt_framebuffer_surface(u32 pixel_offset);
u8 *pt_surface_row(PTSurface surface, unsigned x, unsigned y, unsigned count);
// Compact RAM coverage resources store one value for four adjacent pixels;
// screen-owned coverage already uses ordinary pixels. Decode at the boundary.

const u8 *pt_mask_row(PTMaskView mask, unsigned x, unsigned y, unsigned count, u8 *scratch);
void pt_framebuffer_reset(void);
void pt_framebuffer_start(void);
void pt_framebuffer_stop(void);
void pt_framebuffer_select(PTSurface surface);
void pt_framebuffer_palette(unsigned start, const void *colors, size_t components);
bool pt_framebuffer_ready(void);
int pt_framebuffer_load(const void *data, size_t size);
size_t pt_framebuffer_rgb_size(void);
int pt_framebuffer_rgb(u8 *rgb, size_t size);
const u8 *pt_framebuffer_pixels(void);
typedef struct __attribute__((packed, may_alias)) PTPatternRow {
  u16 bits;
} PTPatternRow;
_Static_assert(sizeof(PTPatternRow) == 2, "pattern resource row size");
void InitializeBitmap(Bitmap *bitmap, i16 width, i16 height);

// Game bitmaps.

void GameBitmapSetupNoOp(void);
void Game_CreateBitmaps(Game *state);
void InitializeAndAllocBitmap(Bitmap *bitmap, i8 unused, i16 width, i16 height);
void SetBitmapBuffer(Bitmap *bitmap, u8 *buffer);
void FreeBitmap(Bitmap *bitmap);
_Noreturn void ExitProcess(i32 unused);
// Interior cleanup label used only by the attested startup repair.

_Noreturn void ExitProcessAfterGraphicsMode(void);

// Canvas objects.

#include <stdbool.h>

struct Palette {
  u8 is_dirty, field_1;
  i16 num_entries;
  PTRGB *colors;
};
struct PalettePointer {
  Palette *palette;
  Juice *juice;
};
extern PalettePointer pal_pointer;
Palette *pt_palette_create(void);
struct Canvas {
  Bitmap *bitmap;
  IglooLink *actors;
  const void *pattern;
  // Drawing uses palette indices; retain the complete supplied color values.

  u16 color, background_color;
  u8 text_background_enabled, fillrect_pattern_value;
  u16 line_pattern, bitmap_mask_flag;
  i16 position_x, position_y;
  const Font *font;
  u8 font_spacing_mode;
  i16 font_height, font_baseline;
};
extern Canvas canvas1;
typedef struct __attribute__((packed, may_alias)) PTMouseCoordinate {
  u16 value;
} PTMouseCoordinate;
_Static_assert(sizeof(PTMouseCoordinate) == 2, "bound coordinate word");
struct Jatoba {
  Mouse *mouse;
  i32 dword8;
  PalettePointer *pal_p;
  Canvas *canvas;
  Bitmap *bitmap;
  PTMouseCoordinate bound_mouse_x, bound_mouse_y;
};
Jatoba *pt_jatoba_create(void);
struct Juice {
  i32 dword0;
  Bitmap *bitmap;
  i16 word8, wordA;
};
extern Juice some_juice;
Juice *pt_juice_create(void);
void Juice_Init(Juice *juice, Bitmap *bitmap);
Canvas *pt_canvas_create(void);

// Bitmap layout.

#include <stdlib.h>

// Named native pages share one flat indexed-pixel array.

typedef enum PTDisplayBuffer {
  PT_DISPLAY_PRIMARY,
  PT_DISPLAY_SECONDARY,
  PT_DISPLAY_BACKGROUND,
  PT_DISPLAY_MASK
} PTDisplayBuffer;
static inline PTSurface pt_display_surface(PTDisplayBuffer slot) {
  if ((unsigned)slot > PT_DISPLAY_MASK)
    abort();
  return pt_framebuffer_surface((u32)slot * PT_SCREEN_PIXELS);
}
static inline bool pt_bitmap_is_screen(const Bitmap *bitmap) {
  return pt_framebuffer_owns(bitmap->surface.pixels);
}
static inline bool pt_bitmap_is_display_page(const Bitmap *bitmap) {
  const u8 *pixels = bitmap->surface.pixels;
  return pixels == pt_display_surface(PT_DISPLAY_PRIMARY).pixels ||
      pixels == pt_display_surface(PT_DISPLAY_SECONDARY).pixels;
}
static inline PTMaskView pt_transparency_mask(void) {
  return pt_graphics_pixels()->mask;
}

// Graphics objects.

extern Bitmap glyph_bitmap;
extern Bitmap bmp6;
extern Bitmap bitmap26;
extern Bitmap bitmap21;
extern Bitmap bitmap_text_edit;
extern Bitmap bitmap20;
extern Bitmap bitmap22;
extern Bitmap bitmap_character_face;
extern Bitmap bmp15;
extern Bitmap bmp5;
extern Bitmap bmp14;
extern Bitmap bmp3;
extern Bitmap bmp11;
extern Bitmap bmp2;
extern Bitmap bmp1;
extern Bitmap bmp10;
extern Bitmap bmp12;
extern Bitmap bmp16;
extern Bitmap bmp25;
extern Bitmap bmp24;
extern Bitmap bmp_background;
extern Bitmap bmp_awards;
extern Bitmap bmp17;
extern Bitmap bmp_blitmap;
extern Bitmap mouse_bitmap_1;
extern Bitmap mouse_bitmap_2;
typedef struct MousePositionBinding {
  PTMouseCoordinate *x, *y;
} MousePositionBinding;
enum { PT_MOUSE_POSITION_BINDINGS = 5 };
extern MousePositionBinding mouse_position_bindings[PT_MOUSE_POSITION_BINDINGS];
void pt_mouse_bindings_reset(void);

// Native graphics roots; storage codecs project them at their boundaries.

// Release owned descriptors at game reset and clear borrows before buffer free.

void pt_native_graphics_release(void);
void pt_graphics_retire_buffer(const void *buffer, size_t size);

extern Font *loaded_font;
extern Font *font_cstandar;
extern Font *exit_optional_font;
extern Font *font_astandar;
extern Font *some_font_ptr;
extern Font *font_supersma;
extern Font *font_ruby;
extern Font *font_piznumb;
extern Canvas *temp_canvas;
extern Canvas *canvas1_ptr;
extern Canvas *canvas_1;
extern Canvas *canvas_3;
extern Bitmap *bmp25_ptr;
extern Bitmap *bmp24_ptr;
extern Bitmap *bmp_character_face;
extern Bitmap *bmp22_ptr;
extern Bitmap *bmp21_ptr;
extern Bitmap *bmp20_ptr;
extern Bitmap *bmp26_ptr;
extern Bitmap *text_edit_bitmap;
extern Bitmap *temp_bitmap_84x6;
extern Bitmap *bmp16_ptr;
extern Bitmap *current_bitmap;
extern Bitmap *old_bitmap;
extern Bitmap *cursor_bitmap;
extern Bitmap *cursor_background_bitmap;
extern Bitmap *cursor_target_bitmap;
extern Bitmap *largest_map_column_bitmaps[2];
extern Bitmap *map_row_bitmaps[2];
extern Bitmap *largest_map_row_bitmaps[2];
extern Bitmap *map_column_bitmaps[2];
extern Bitmap *display_bitmaps[2];

// Palette state.

// Palette references are actual objects. An optional codec owns saved-word
// identities; palette selection never reads those words in gameplay.

typedef struct PTPaletteSlot {
  PalettePointer *pointer;
} PTPaletteSlot;
extern PalettePointer *active_palette_pointer;
extern PTPaletteSlot temp_palette_slot, some_pal_p;
void pt_palette_state_reset(void);
void pt_palette_set_active(PalettePointer *pointer);
PTPaletteSlot *pt_game_palette_slot(const Game *game_state, unsigned index);
void pt_game_bind_palette_slot(Game *game_state, unsigned index, PTPaletteSlot *slot);

#endif
