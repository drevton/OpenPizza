// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
#ifndef PT_SAVEGAME_FORMAT_H
#define PT_SAVEGAME_FORMAT_H
#include "types.h"
typedef u32 guest_ptr32;
_Static_assert(sizeof(guest_ptr32) == 4, "guest pointers are 32 bits");
#include "memory.h"
#include "sound_layout.h"
typedef union __attribute__((packed, may_alias)) PTSoundFarPointer {
  u32 encoded;
  struct __attribute__((packed)) {
    u16 offset, segment;
  };
} PTSoundFarPointer;
_Static_assert(sizeof(PTSoundFarPointer) == 4, "real-mode sound pointer");
typedef struct __attribute__((packed, may_alias)) PTSoundImageTag {
  u8 text[6];
} PTSoundImageTag;
enum { PT_SOUND_IMAGE_TAG_OFFSET = 3 };
_Static_assert(sizeof(PTSoundImageTag) == 6, "resident image tag extent");

#pragma pack(push, 1)
// BEGIN GENERATED SAVED RECORDS

typedef struct PTSavedPalette {
  // 0x0
  u8 is_dirty;
  // 0x1
  u8 field_1;
  // 0x2
  i16 num_entries;
  // 0x4
  union {
    u8 serialized_data[4];
    u32 serialized_data_word;
  };
} PTSavedPalette;
_Static_assert(sizeof(PTSavedPalette) == 8, "PTSavedPalette size");
_Static_assert(offsetof(PTSavedPalette, is_dirty) == 0, "PTSavedPalette.is_dirty");
_Static_assert(offsetof(PTSavedPalette, field_1) == 1, "PTSavedPalette.field_1");
_Static_assert(offsetof(PTSavedPalette, num_entries) == 2, "PTSavedPalette.num_entries");
_Static_assert(offsetof(PTSavedPalette, serialized_data) == 4, "PTSavedPalette.data");

typedef struct PTSavedMouseInitializer {
  // 0x0
  i32 field_0;
  // 0x4
  union {
    u8 serialized_jatoba[4];
    u32 serialized_jatoba_word;
  };
} PTSavedMouseInitializer;
_Static_assert(sizeof(PTSavedMouseInitializer) == 8, "PTSavedMouseInitializer size");
_Static_assert(offsetof(PTSavedMouseInitializer, field_0) == 0, "PTSavedMouseInitializer.field_0");
_Static_assert(offsetof(PTSavedMouseInitializer, serialized_jatoba) == 4, "PTSavedMouseInitializer.jatoba");

typedef struct PTSavedJuice {
  // 0x0
  i32 dword0;
  // 0x4
  union {
    u8 serialized_bitmap[4];
    u32 serialized_bitmap_word;
  };
  // 0x8
  i16 word8;
  // 0xA
  i16 wordA;
} PTSavedJuice;
_Static_assert(sizeof(PTSavedJuice) == 12, "PTSavedJuice size");
_Static_assert(offsetof(PTSavedJuice, dword0) == 0, "PTSavedJuice.dword0");
_Static_assert(offsetof(PTSavedJuice, serialized_bitmap) == 4, "PTSavedJuice.bitmap");
_Static_assert(offsetof(PTSavedJuice, word8) == 8, "PTSavedJuice.word8");
_Static_assert(offsetof(PTSavedJuice, wordA) == 10, "PTSavedJuice.wordA");

typedef struct PTSavedPalettePointer {
  // 0x0
  union {
    u8 serialized_palette[4];
    u32 serialized_palette_word;
  };
  // 0x4
  union {
    u8 serialized_juice[4];
    u32 serialized_juice_word;
  };
} PTSavedPalettePointer;
_Static_assert(sizeof(PTSavedPalettePointer) == 8, "PTSavedPalettePointer size");
_Static_assert(offsetof(PTSavedPalettePointer, serialized_palette) == 0, "PTSavedPalettePointer.palette");
_Static_assert(offsetof(PTSavedPalettePointer, serialized_juice) == 4, "PTSavedPalettePointer.juice");

typedef struct __attribute__((may_alias)) PTSavedCanvas {
  // 0x0
  union {
    u8 serialized_bitmap[4];
    u32 serialized_bitmap_word;
  };
  // 0x4
  union {
    u8 serialized_igloo_link[4];
    u32 serialized_igloo_link_word;
  };
  // 0x8
  union {
    u8 serialized_fillrect_pattern_ptr[4];
    u32 serialized_fillrect_pattern_ptr_word;
  };
  // The setter writes a WORD; drawing also uses its low byte.

  union {
    struct {
      u8 color;
      u8 gapD;
    };
    u16 color_word;
  };
  union {
    struct {
      u8 byteE;
      u8 gapF;
    };
    u16 field_e_word;
  };
  // 0x10
  u8 byte10;
  // 0x11
  u8 fillrect_pattern_value;
  // 0x12
  u16 line_pattern;
  // 0x14
  u16 bitmap_mask_flag;
  // 0x16
  i16 position_x;
  // 0x18
  i16 position_y;
  // 0x1A
  union {
    u8 serialized_font_ptr[4];
    u32 serialized_font_ptr_word;
  };
  // 0x1E
  u8 font_spacing_mode;
  // 0x1F
  i16 font_height;
  // 0x21
  i16 font_baseline;
} PTSavedCanvas;
_Static_assert(sizeof(PTSavedCanvas) == 35, "PTSavedCanvas size");
_Static_assert(offsetof(PTSavedCanvas, serialized_bitmap) == 0, "PTSavedCanvas.bitmap");
_Static_assert(offsetof(PTSavedCanvas, serialized_igloo_link) == 4, "PTSavedCanvas.igloo_link");
_Static_assert(offsetof(PTSavedCanvas, serialized_fillrect_pattern_ptr) == 8, "PTSavedCanvas.fillrect_pattern_ptr");
_Static_assert(offsetof(PTSavedCanvas, color) == 12, "PTSavedCanvas.color");
_Static_assert(offsetof(PTSavedCanvas, color_word) == 12, "PTSavedCanvas.color_word");
_Static_assert(offsetof(PTSavedCanvas, gapD) == 13, "PTSavedCanvas.gapD");
_Static_assert(offsetof(PTSavedCanvas, byteE) == 14, "PTSavedCanvas.byteE");
_Static_assert(offsetof(PTSavedCanvas, field_e_word) == 14, "PTSavedCanvas.field_e_word");
_Static_assert(offsetof(PTSavedCanvas, gapF) == 15, "PTSavedCanvas.gapF");
_Static_assert(offsetof(PTSavedCanvas, byte10) == 16, "PTSavedCanvas.byte10");
_Static_assert(offsetof(PTSavedCanvas, fillrect_pattern_value) == 17, "PTSavedCanvas.fillrect_pattern_value");
_Static_assert(offsetof(PTSavedCanvas, line_pattern) == 18, "PTSavedCanvas.line_pattern");
_Static_assert(offsetof(PTSavedCanvas, bitmap_mask_flag) == 20, "PTSavedCanvas.bitmap_mask_flag");
_Static_assert(offsetof(PTSavedCanvas, position_x) == 22, "PTSavedCanvas.position_x");
_Static_assert(offsetof(PTSavedCanvas, position_y) == 24, "PTSavedCanvas.position_y");
_Static_assert(offsetof(PTSavedCanvas, serialized_font_ptr) == 26, "PTSavedCanvas.font_ptr");
_Static_assert(offsetof(PTSavedCanvas, font_spacing_mode) == 30, "PTSavedCanvas.font_spacing_mode");
_Static_assert(offsetof(PTSavedCanvas, font_height) == 31, "PTSavedCanvas.font_height");
_Static_assert(offsetof(PTSavedCanvas, font_baseline) == 33, "PTSavedCanvas.font_baseline");

typedef struct PTSavedBitmap {
  // 0x0
  i16 x;
  // 0x2
  i16 y;
  // 0x4
  union {
    u8 serialized_buffer[4];
    u32 serialized_buffer_word;
  };
  // 0x8
  u8 flag;
} PTSavedBitmap;
_Static_assert(sizeof(PTSavedBitmap) == 9, "PTSavedBitmap size");
_Static_assert(offsetof(PTSavedBitmap, x) == 0, "PTSavedBitmap.x");
_Static_assert(offsetof(PTSavedBitmap, y) == 2, "PTSavedBitmap.y");
_Static_assert(offsetof(PTSavedBitmap, serialized_buffer) == 4, "PTSavedBitmap.buffer");
_Static_assert(offsetof(PTSavedBitmap, flag) == 8, "PTSavedBitmap.flag");

typedef struct PTSavedJatoba {
  // 0x0
  union {
    u8 serialized_mouse[4];
    u32 serialized_mouse_word;
  };
  // 0x4
  union {
    i32 field_4;
    struct {
      i16 bound_mouse_y, bound_mouse_x;
    };
  };
  // 0x8
  i32 dword8;
  // 0xC
  PTSavedPalettePointer pal_p;
  // 0x14
  PTSavedCanvas canvas;
  // 0x37
  PTSavedBitmap bitmap;
} PTSavedJatoba;
_Static_assert(sizeof(PTSavedJatoba) == 64, "PTSavedJatoba size");
_Static_assert(offsetof(PTSavedJatoba, serialized_mouse) == 0, "PTSavedJatoba.mouse");
_Static_assert(offsetof(PTSavedJatoba, field_4) == 4, "PTSavedJatoba.field_4");
_Static_assert(offsetof(PTSavedJatoba, bound_mouse_y) == 4, "PTSavedJatoba mouse Y alias");
_Static_assert(offsetof(PTSavedJatoba, bound_mouse_x) == 6, "PTSavedJatoba mouse X alias");
_Static_assert(offsetof(PTSavedJatoba, dword8) == 8, "PTSavedJatoba.dword8");
_Static_assert(offsetof(PTSavedJatoba, pal_p) == 12, "PTSavedJatoba.pal_p");
_Static_assert(offsetof(PTSavedJatoba, canvas) == 20, "PTSavedJatoba.canvas");
_Static_assert(offsetof(PTSavedJatoba, bitmap) == 55, "PTSavedJatoba.bitmap");

typedef struct PTSavedLoadFontDesc {
  // 0x0
  union {
    u8 serialized_filename[4];
    u32 serialized_filename_word;
  };
  // 0x4
  i32 field_4;
} PTSavedLoadFontDesc;
_Static_assert(sizeof(PTSavedLoadFontDesc) == 8, "PTSavedLoadFontDesc size");
_Static_assert(offsetof(PTSavedLoadFontDesc, serialized_filename) == 0, "PTSavedLoadFontDesc.filename");
_Static_assert(offsetof(PTSavedLoadFontDesc, field_4) == 4, "PTSavedLoadFontDesc.field_4");

typedef struct PTSavedIglooLink {
  // 0x0
  u8 field_0;
  // 0x1
  union {
    u8 serialized_first_igloo[4];
    u32 serialized_first_igloo_word;
  };
  // 0x5
  union {
    u8 serialized_last_igloo[4];
    u32 serialized_last_igloo_word;
  };
} PTSavedIglooLink;
_Static_assert(sizeof(PTSavedIglooLink) == 9, "PTSavedIglooLink size");
_Static_assert(offsetof(PTSavedIglooLink, field_0) == 0, "PTSavedIglooLink.field_0");
_Static_assert(offsetof(PTSavedIglooLink, serialized_first_igloo) == 1, "PTSavedIglooLink.first_igloo");
_Static_assert(offsetof(PTSavedIglooLink, serialized_last_igloo) == 5, "PTSavedIglooLink.last_igloo");

typedef struct PTSavedMouse {
  // 0x0
  i16 y;
  // 0x2
  i16 x;
  // 0x4
  union {
    u8 serialized_jatoba[4];
    u32 serialized_jatoba_word;
  };
  // 0x8
  union {
    u8 serialized_canvas[4];
    u32 serialized_canvas_word;
  };
  // 0xC
  union {
    i32 dwordC;
    u32 input_filter;
  };
} PTSavedMouse;
_Static_assert(sizeof(PTSavedMouse) == 16, "PTSavedMouse size");
_Static_assert(offsetof(PTSavedMouse, y) == 0, "PTSavedMouse.y");
_Static_assert(offsetof(PTSavedMouse, x) == 2, "PTSavedMouse.x");
_Static_assert(offsetof(PTSavedMouse, serialized_jatoba) == 4, "PTSavedMouse.jatoba");
_Static_assert(offsetof(PTSavedMouse, serialized_canvas) == 8, "PTSavedMouse.canvas");
_Static_assert(offsetof(PTSavedMouse, dwordC) == 12, "PTSavedMouse.dwordC");
_Static_assert(offsetof(PTSavedMouse, input_filter) == 12, "PTSavedMouse input filter alias");

typedef struct __attribute__((may_alias)) PTSavedLinkedElem {
  // 0x0
  union {
    u8 serialized_next[4];
    u32 serialized_next_word;
  };
  // 0x4
  union {
    u8 serialized_prevp[4];
    u32 serialized_prevp_word;
  };
} PTSavedLinkedElem;
_Static_assert(sizeof(PTSavedLinkedElem) == 8, "PTSavedLinkedElem size");
_Static_assert(offsetof(PTSavedLinkedElem, serialized_next) == 0, "PTSavedLinkedElem.next");
_Static_assert(offsetof(PTSavedLinkedElem, serialized_prevp) == 4, "PTSavedLinkedElem.prevp");

typedef struct __attribute__((may_alias)) PTSavedLinkedHead {
  // 0x0
  union {
    u8 serialized_first[4];
    u32 serialized_first_word;
  };
  // 0x4
  PTSavedLinkedElem elem;
} PTSavedLinkedHead;
_Static_assert(sizeof(PTSavedLinkedHead) == 12, "PTSavedLinkedHead size");
_Static_assert(offsetof(PTSavedLinkedHead, serialized_first) == 0, "PTSavedLinkedHead.first");
_Static_assert(offsetof(PTSavedLinkedHead, elem) == 4, "PTSavedLinkedHead.elem");

typedef struct PTSavedHazelnut {
  // 0x0
  i16 flag;
  // 0x2
  union {
    u8 serialized_data_pointer[4];
    u32 serialized_data_pointer_word;
  };
  // 0x6
  union {
    u8 serialized_igloo[4];
    u32 serialized_igloo_word;
  };
} PTSavedHazelnut;
_Static_assert(sizeof(PTSavedHazelnut) == 10, "PTSavedHazelnut size");
_Static_assert(offsetof(PTSavedHazelnut, flag) == 0, "PTSavedHazelnut.flag");
_Static_assert(offsetof(PTSavedHazelnut, serialized_data_pointer) == 2, "PTSavedHazelnut.data_pointer");
_Static_assert(offsetof(PTSavedHazelnut, serialized_igloo) == 6, "PTSavedHazelnut.igloo");

typedef struct PTSavedBobHeader {
  // 0x0
  u8 w;
  // 0x1
  u8 h;
  // 0x2
  u8 field_2;
  // 0x3
  union {
    u8 serialized_pointer[4];
    u32 serialized_pointer_word;
  };
} PTSavedBobHeader;
_Static_assert(sizeof(PTSavedBobHeader) == 7, "PTSavedBobHeader size");
_Static_assert(offsetof(PTSavedBobHeader, w) == 0, "PTSavedBobHeader.w");
_Static_assert(offsetof(PTSavedBobHeader, h) == 1, "PTSavedBobHeader.h");
_Static_assert(offsetof(PTSavedBobHeader, field_2) == 2, "PTSavedBobHeader.field_2");
_Static_assert(offsetof(PTSavedBobHeader, serialized_pointer) == 3, "PTSavedBobHeader.pointer");

typedef struct PTSavedLangFileDesc {
  // 0x0
  u8 num_entries;
  // 0x1
  u8 field_1;
  // 0x2
  i16 size;
  // 0x4
  union {
    u8 serialized_buffer[4];
    u32 serialized_buffer_word;
  };
  // 0x8
  u8 name[10];
} PTSavedLangFileDesc;
_Static_assert(sizeof(PTSavedLangFileDesc) == 18, "PTSavedLangFileDesc size");
_Static_assert(offsetof(PTSavedLangFileDesc, num_entries) == 0, "PTSavedLangFileDesc.num_entries");
_Static_assert(offsetof(PTSavedLangFileDesc, field_1) == 1, "PTSavedLangFileDesc.field_1");
_Static_assert(offsetof(PTSavedLangFileDesc, size) == 2, "PTSavedLangFileDesc.size");
_Static_assert(offsetof(PTSavedLangFileDesc, serialized_buffer) == 4, "PTSavedLangFileDesc.buffer");
_Static_assert(offsetof(PTSavedLangFileDesc, name) == 8, "PTSavedLangFileDesc.name");

typedef struct PTSavedContract {
  // 0x0
  PTSavedLinkedElem link;
  // 0x8
  union {
    u8 serialized_supplier[4];
    u32 serialized_supplier_word;
  };
  // 0xC
  i16 amount_of_each[24];
  // 0x3C
  i32 sum;
  // 0x40
  u8 discount;
  // 0x41
  u8 term_length;
  // 0x42
  i16 charge;
  // 0x44
  i32 penalty;
  // 0x48
  u8 gap48[4];
  // 0x4C
  u8 amount_mode;
  // 0x4D
  u8 restaurant_id;
  // 0x4E
  u8 byte4E;
  u8 unknown_4F[1];
  // 0x50
  u8 term_length_total;
  u8 unknown_51[1];
  // 0x52
  u8 player_index;
  // 0x53
  u8 field_53;
} PTSavedContract;
_Static_assert(sizeof(PTSavedContract) == 84, "PTSavedContract size");
_Static_assert(offsetof(PTSavedContract, link) == 0, "PTSavedContract.link");
_Static_assert(offsetof(PTSavedContract, serialized_supplier) == 8, "PTSavedContract.supplier");
_Static_assert(offsetof(PTSavedContract, amount_of_each) == 12, "PTSavedContract.amount_of_each");
_Static_assert(offsetof(PTSavedContract, sum) == 60, "PTSavedContract.sum");
_Static_assert(offsetof(PTSavedContract, discount) == 64, "PTSavedContract.discount");
_Static_assert(offsetof(PTSavedContract, term_length) == 65, "PTSavedContract.term_length");
_Static_assert(offsetof(PTSavedContract, charge) == 66, "PTSavedContract.charge");
_Static_assert(offsetof(PTSavedContract, penalty) == 68, "PTSavedContract.penalty");
_Static_assert(offsetof(PTSavedContract, gap48) == 72, "PTSavedContract.gap48");
_Static_assert(offsetof(PTSavedContract, amount_mode) == 76, "PTSavedContract.amount_mode");
_Static_assert(offsetof(PTSavedContract, restaurant_id) == 77, "PTSavedContract.restaurant_id");
_Static_assert(offsetof(PTSavedContract, byte4E) == 78, "PTSavedContract.byte4E");
_Static_assert(offsetof(PTSavedContract, term_length_total) == 80, "PTSavedContract.term_length_total");
_Static_assert(offsetof(PTSavedContract, player_index) == 82, "PTSavedContract.player_index");
_Static_assert(offsetof(PTSavedContract, field_53) == 83, "PTSavedContract.field_53");

typedef struct PTSavedRestoRecipe {
  // 0x0
  u8 sum_of_qualities;
  // 0x1
  u8 byte1;
  // 0x2
  i16 last_price_per_pizza;
  // 0x4
  union {
    u8 serialized_recipe[4];
    u32 serialized_recipe_word;
  };
  // 0x8
  i16 price_per_pizza;
  // 0xA
  i16 costs_per_pizza;
  // 0xC
  u8 orders;
  // 0xD
  u8 sales;
  // 0xE
  i16 sold_items;
} PTSavedRestoRecipe;
_Static_assert(sizeof(PTSavedRestoRecipe) == 16, "PTSavedRestoRecipe size");
_Static_assert(offsetof(PTSavedRestoRecipe, sum_of_qualities) == 0, "PTSavedRestoRecipe.sum_of_qualities");
_Static_assert(offsetof(PTSavedRestoRecipe, byte1) == 1, "PTSavedRestoRecipe.byte1");
_Static_assert(offsetof(PTSavedRestoRecipe, last_price_per_pizza) == 2, "PTSavedRestoRecipe.last_price_per_pizza");
_Static_assert(offsetof(PTSavedRestoRecipe, serialized_recipe) == 4, "PTSavedRestoRecipe.recipe");
_Static_assert(offsetof(PTSavedRestoRecipe, price_per_pizza) == 8, "PTSavedRestoRecipe.price_per_pizza");
_Static_assert(offsetof(PTSavedRestoRecipe, costs_per_pizza) == 10, "PTSavedRestoRecipe.costs_per_pizza");
_Static_assert(offsetof(PTSavedRestoRecipe, orders) == 12, "PTSavedRestoRecipe.orders");
_Static_assert(offsetof(PTSavedRestoRecipe, sales) == 13, "PTSavedRestoRecipe.sales");
_Static_assert(offsetof(PTSavedRestoRecipe, sold_items) == 14, "PTSavedRestoRecipe.sold_items");

typedef struct PTSavedIgloo {
  // 0x0
  union {
    u8 serialized_next[4];
    u32 serialized_next_word;
  };
  // 0x4
  union {
    u8 serialized_prev[4];
    u32 serialized_prev_word;
  };
  // 0x8
  i16 last_y;
  // 0xA
  i16 last_x;
  // 0xC
  i16 flag;
  // 0xE
  i16 draw_y;
  // 0x10
  i16 draw_x;
  // 0x12
  i16 h;
  // 0x14
  i16 w;
  // 0x16
  union {
    u8 serialized_bob_pointer[4];
    u32 serialized_bob_pointer_word;
  };
  // 0x1A
  union {
    u8 serialized_hazelnut[4];
    u32 serialized_hazelnut_word;
  };
  // 0x1E
  u8 unknown_1E[18];
  // 0x30
  u8 movement_remaining;
  // 0x31
  u8 tile_column;
  // 0x32
  u8 tile_row;
  // 0x33
  u8 orientation;
  // 0x34
  u8 movement_pause;
  // 0x35
  u8 actor_index;
  // 0x36
  u8 shortened_turn;
  // 0x37
  u8 is_unused;
} PTSavedIgloo;
_Static_assert(sizeof(PTSavedIgloo) == 56, "PTSavedIgloo size");
_Static_assert(offsetof(PTSavedIgloo, serialized_next) == 0, "PTSavedIgloo.next");
_Static_assert(offsetof(PTSavedIgloo, serialized_prev) == 4, "PTSavedIgloo.prev");
_Static_assert(offsetof(PTSavedIgloo, last_y) == 8, "PTSavedIgloo.last_y");
_Static_assert(offsetof(PTSavedIgloo, last_x) == 10, "PTSavedIgloo.last_x");
_Static_assert(offsetof(PTSavedIgloo, flag) == 12, "PTSavedIgloo.flag");
_Static_assert(offsetof(PTSavedIgloo, draw_y) == 14, "PTSavedIgloo.draw_y");
_Static_assert(offsetof(PTSavedIgloo, draw_x) == 16, "PTSavedIgloo.draw_x");
_Static_assert(offsetof(PTSavedIgloo, h) == 18, "PTSavedIgloo.h");
_Static_assert(offsetof(PTSavedIgloo, w) == 20, "PTSavedIgloo.w");
_Static_assert(offsetof(PTSavedIgloo, serialized_bob_pointer) == 22, "PTSavedIgloo.bob_pointer");
_Static_assert(offsetof(PTSavedIgloo, serialized_hazelnut) == 26, "PTSavedIgloo.hazelnut");
_Static_assert(offsetof(PTSavedIgloo, unknown_1E) == 30, "PTSavedIgloo.unknown_1E");
_Static_assert(offsetof(PTSavedIgloo, movement_remaining) == 48, "PTSavedIgloo.movement_remaining");
_Static_assert(offsetof(PTSavedIgloo, tile_column) == 49, "PTSavedIgloo.tile_column");
_Static_assert(offsetof(PTSavedIgloo, tile_row) == 50, "PTSavedIgloo.tile_row");
_Static_assert(offsetof(PTSavedIgloo, orientation) == 51, "PTSavedIgloo.orientation");
_Static_assert(offsetof(PTSavedIgloo, movement_pause) == 52, "PTSavedIgloo.movement_pause");
_Static_assert(offsetof(PTSavedIgloo, actor_index) == 53, "PTSavedIgloo.actor_index");
_Static_assert(offsetof(PTSavedIgloo, shortened_turn) == 54, "PTSavedIgloo.shortened_turn");
_Static_assert(offsetof(PTSavedIgloo, is_unused) == 55, "PTSavedIgloo.is_unused");

typedef struct PTSavedBobHost {
  // 0x0
  i16 num_entries;
  // 0x2
  i32 size;
  // 0x6
  union {
    u8 serialized_data[4];
    u32 serialized_data_word;
  };
  // 0xA
  union {
    u8 serialized_next[4];
    u32 serialized_next_word;
  };
  // 0xE
  union {
    u8 serialized_start_ptr[4];
    u32 serialized_start_ptr_word;
  };
  // 0x12
  union {
    u8 serialized_data_ptr[4];
    u32 serialized_data_ptr_word;
  };
  // 0x16
  i32 dword16;
  // 0x1A
  u8 flags;
  // 0x1B
  u8 filename[26];
} PTSavedBobHost;
_Static_assert(sizeof(PTSavedBobHost) == 53, "PTSavedBobHost size");
_Static_assert(offsetof(PTSavedBobHost, num_entries) == 0, "PTSavedBobHost.num_entries");
_Static_assert(offsetof(PTSavedBobHost, size) == 2, "PTSavedBobHost.size");
_Static_assert(offsetof(PTSavedBobHost, serialized_data) == 6, "PTSavedBobHost.data");
_Static_assert(offsetof(PTSavedBobHost, serialized_next) == 10, "PTSavedBobHost.next");
_Static_assert(offsetof(PTSavedBobHost, serialized_start_ptr) == 14, "PTSavedBobHost.start_ptr");
_Static_assert(offsetof(PTSavedBobHost, serialized_data_ptr) == 18, "PTSavedBobHost.data_ptr");
_Static_assert(offsetof(PTSavedBobHost, dword16) == 22, "PTSavedBobHost.dword16");
_Static_assert(offsetof(PTSavedBobHost, flags) == 26, "PTSavedBobHost.flags");
_Static_assert(offsetof(PTSavedBobHost, filename) == 27, "PTSavedBobHost.filename");

typedef struct PTSavedPig {
  // 0x0
  i16 x;
  // 0x2
  i16 y;
  // 0x4
  u8 byte4;
  // 0x5
  u8 byte5;
  // 0x6
  u8 gap6;
  // 0x7
  u8 byte7;
  // 0x8
  i16 x_coord;
  // 0xA
  i16 y_coord;
  // 0xC
  u8 weapon_type;
  // 0xD
  u8 byteD;
  // 0xE
  u8 byteE;
  // 0xF
  u8 byteF;
  // 0x10
  u8 direction_unk;
  // 0x11
  u8 gap11[7];
  // 0x18
  u8 field_18;
  // 0x19
  u8 gap19[6];
  // 0x1F
  u8 field_1F;
  // 0x20
  u8 byte20;
  // 0x21
  u8 byte21;
  // 0x22
  u8 byte22;
  // 0x23
  u8 dead_unk;
  // 0x24
  u8 personality_attrib;
  // 0x25
  u8 byte25;
  // 0x26
  i16 word26;
  // 0x28
  u8 byte28;
  // 0x29
  u8 byte29;
  // 0x2A
  u8 byte2A;
  // 0x2B
  u8 field_2B;
  // 0x2C
  u8 gap2C[2];
  // 0x2E
  i16 byte2E;
  // 0x30
  i16 word30;
  // 0x32
  i16 word32;
  // 0x34
  i16 word34;
  // 0x36
  union {
    u8 serialized_other_pig[4];
    u32 serialized_other_pig_word;
  };
  // 0x3A
  union {
    u8 serialized_character[4];
    u32 serialized_character_word;
  };
} PTSavedPig;
_Static_assert(sizeof(PTSavedPig) == 62, "PTSavedPig size");
_Static_assert(offsetof(PTSavedPig, x) == 0, "PTSavedPig.x");
_Static_assert(offsetof(PTSavedPig, y) == 2, "PTSavedPig.y");
_Static_assert(offsetof(PTSavedPig, byte4) == 4, "PTSavedPig.byte4");
_Static_assert(offsetof(PTSavedPig, byte5) == 5, "PTSavedPig.byte5");
_Static_assert(offsetof(PTSavedPig, gap6) == 6, "PTSavedPig.gap6");
_Static_assert(offsetof(PTSavedPig, byte7) == 7, "PTSavedPig.byte7");
_Static_assert(offsetof(PTSavedPig, x_coord) == 8, "PTSavedPig.x_coord");
_Static_assert(offsetof(PTSavedPig, y_coord) == 10, "PTSavedPig.y_coord");
_Static_assert(offsetof(PTSavedPig, weapon_type) == 12, "PTSavedPig.weapon_type");
_Static_assert(offsetof(PTSavedPig, byteD) == 13, "PTSavedPig.byteD");
_Static_assert(offsetof(PTSavedPig, byteE) == 14, "PTSavedPig.byteE");
_Static_assert(offsetof(PTSavedPig, byteF) == 15, "PTSavedPig.byteF");
_Static_assert(offsetof(PTSavedPig, direction_unk) == 16, "PTSavedPig.direction_unk");
_Static_assert(offsetof(PTSavedPig, gap11) == 17, "PTSavedPig.gap11");
_Static_assert(offsetof(PTSavedPig, field_18) == 24, "PTSavedPig.field_18");
_Static_assert(offsetof(PTSavedPig, gap19) == 25, "PTSavedPig.gap19");
_Static_assert(offsetof(PTSavedPig, field_1F) == 31, "PTSavedPig.field_1F");
_Static_assert(offsetof(PTSavedPig, byte20) == 32, "PTSavedPig.byte20");
_Static_assert(offsetof(PTSavedPig, byte21) == 33, "PTSavedPig.byte21");
_Static_assert(offsetof(PTSavedPig, byte22) == 34, "PTSavedPig.byte22");
_Static_assert(offsetof(PTSavedPig, dead_unk) == 35, "PTSavedPig.dead_unk");
_Static_assert(offsetof(PTSavedPig, personality_attrib) == 36, "PTSavedPig.personality_attrib");
_Static_assert(offsetof(PTSavedPig, byte25) == 37, "PTSavedPig.byte25");
_Static_assert(offsetof(PTSavedPig, word26) == 38, "PTSavedPig.word26");
_Static_assert(offsetof(PTSavedPig, byte28) == 40, "PTSavedPig.byte28");
_Static_assert(offsetof(PTSavedPig, byte29) == 41, "PTSavedPig.byte29");
_Static_assert(offsetof(PTSavedPig, byte2A) == 42, "PTSavedPig.byte2A");
_Static_assert(offsetof(PTSavedPig, field_2B) == 43, "PTSavedPig.field_2B");
_Static_assert(offsetof(PTSavedPig, gap2C) == 44, "PTSavedPig.gap2C");
_Static_assert(offsetof(PTSavedPig, byte2E) == 46, "PTSavedPig.byte2E");
_Static_assert(offsetof(PTSavedPig, word30) == 48, "PTSavedPig.word30");
_Static_assert(offsetof(PTSavedPig, word32) == 50, "PTSavedPig.word32");
_Static_assert(offsetof(PTSavedPig, word34) == 52, "PTSavedPig.word34");
_Static_assert(offsetof(PTSavedPig, serialized_other_pig) == 54, "PTSavedPig.other_pig");
_Static_assert(offsetof(PTSavedPig, serialized_character) == 58, "PTSavedPig.character");

typedef struct PTSavedTown {
  // 0x0
  i32 field_0;
  // 0x4
  i16 num_poi;
  // 0x6
  i16 field_6;
  // 0x8
  u8 num_zones;
  // 0x9
  u8 num_supplier;
  u8 unknown_A[2];
  // 0xC
  i32 field_C;
  // 0x10
  i32 field_10;
  // 0x14
  u8 gap14[20];
  // 0x28
  u8 num_phone_nr_of_kind[6];
  // 0x2E
  u8 phone_number_flags[6][10];
  // 0x6A
  i32 phone_numbers[6][10];
  // 0x15A
  u8 realtors[240];
  // 0x24A
  u8 bank_names[240];
  // 0x33A
  u8 insurance_company_names[240];
  // 0x42A
  u8 authorities_names[240];
  // 0x51A
  u8 others_names[240];
  // 0x60A
  u8 gap60A[240];
  // 0x6FA
  RealtorDetails realtor_details[10];
  // 0x722
  u8 loan_details[10][4];
  // 0x74A
  u8 insurance_details[10][4];
  // 0x772
  u8 gap772[40];
  // 0x79A
  TownMafia town_mafia[2];
  // 0x7A2
  u8 gap7A2[72];
  // 0x7EA
  u16 base_stock_prices[4][24];
  // 0x8AA
  i8 ingredient_trend[96];
  // 0x90A
  i8 stockarr90A[96];
  // 0x96A
  i16 field_96A;
  // 0x96C
  u8 gap96C[38];
  // 0x992
  i16 word992;
  // 0x994
  u8 gap994[52];
  // 0x9C8
  PTSavedLinkedHead poi;
  // 0x9D4
  PTSavedLinkedHead zones;
  // 0x9E0
  PTSavedLinkedHead suppliers;
} PTSavedTown;
_Static_assert(sizeof(PTSavedTown) == 2540, "PTSavedTown size");
_Static_assert(offsetof(PTSavedTown, field_0) == 0, "PTSavedTown.field_0");
_Static_assert(offsetof(PTSavedTown, num_poi) == 4, "PTSavedTown.num_poi");
_Static_assert(offsetof(PTSavedTown, field_6) == 6, "PTSavedTown.field_6");
_Static_assert(offsetof(PTSavedTown, num_zones) == 8, "PTSavedTown.num_zones");
_Static_assert(offsetof(PTSavedTown, num_supplier) == 9, "PTSavedTown.num_supplier");
_Static_assert(offsetof(PTSavedTown, field_C) == 12, "PTSavedTown.field_C");
_Static_assert(offsetof(PTSavedTown, field_10) == 16, "PTSavedTown.field_10");
_Static_assert(offsetof(PTSavedTown, gap14) == 20, "PTSavedTown.gap14");
_Static_assert(offsetof(PTSavedTown, num_phone_nr_of_kind) == 40, "PTSavedTown.num_phone_nr_of_kind");
_Static_assert(offsetof(PTSavedTown, phone_number_flags) == 46, "PTSavedTown.phone_number_flags");
_Static_assert(offsetof(PTSavedTown, phone_numbers) == 106, "PTSavedTown.phone_numbers");
_Static_assert(offsetof(PTSavedTown, realtors) == 346, "PTSavedTown.realtors");
_Static_assert(offsetof(PTSavedTown, bank_names) == 586, "PTSavedTown.bank_names");
_Static_assert(offsetof(PTSavedTown, insurance_company_names) == 826, "PTSavedTown.insurance_company_names");
_Static_assert(offsetof(PTSavedTown, authorities_names) == 1066, "PTSavedTown.authorities_names");
_Static_assert(offsetof(PTSavedTown, others_names) == 1306, "PTSavedTown.others_names");
_Static_assert(offsetof(PTSavedTown, gap60A) == 1546, "PTSavedTown.gap60A");
_Static_assert(offsetof(PTSavedTown, realtor_details) == 1786, "PTSavedTown.realtor_details");
_Static_assert(offsetof(PTSavedTown, loan_details) == 1826, "PTSavedTown.loan_details");
_Static_assert(offsetof(PTSavedTown, insurance_details) == 1866, "PTSavedTown.insurance_details");
_Static_assert(offsetof(PTSavedTown, gap772) == 1906, "PTSavedTown.gap772");
_Static_assert(offsetof(PTSavedTown, town_mafia) == 1946, "PTSavedTown.town_mafia");
_Static_assert(offsetof(PTSavedTown, gap7A2) == 1954, "PTSavedTown.gap7A2");
_Static_assert(offsetof(PTSavedTown, base_stock_prices) == 2026, "PTSavedTown.base_stock_prices");
_Static_assert(offsetof(PTSavedTown, ingredient_trend) == 2218, "PTSavedTown.ingredient_trend");
_Static_assert(offsetof(PTSavedTown, stockarr90A) == 2314, "PTSavedTown.stockarr90A");
_Static_assert(offsetof(PTSavedTown, field_96A) == 2410, "PTSavedTown.field_96A");
_Static_assert(offsetof(PTSavedTown, gap96C) == 2412, "PTSavedTown.gap96C");
_Static_assert(offsetof(PTSavedTown, word992) == 2450, "PTSavedTown.word992");
_Static_assert(offsetof(PTSavedTown, gap994) == 2452, "PTSavedTown.gap994");
_Static_assert(offsetof(PTSavedTown, poi) == 2504, "PTSavedTown.poi");
_Static_assert(offsetof(PTSavedTown, zones) == 2516, "PTSavedTown.zones");
_Static_assert(offsetof(PTSavedTown, suppliers) == 2528, "PTSavedTown.suppliers");

typedef struct PTSavedSupplier {
  // 0x0
  PTSavedLinkedElem link;
  // 0x8
  u8 stock_kind;
  // 0x9
  u8 quality;
  // 0xA
  u8 supplier_id;
  // 0xB
  u8 base_price_addon;
  // 0xC
  u8 name[26];
  // 0x26
  i16 minimum_order;
  // 0x28
  i32 bonus_levels;
  // 0x2C
  i32 total_bought_by_player[8];
  // 0x4C
  u8 gap4C[64];
  // 0x8C
  u8 last_term_length[8];
  // 0x94
  u8 num_contracts_with_player[8];
  // 0x9C
  i16 last_amount[8];
  // 0xAC
  i32 current_account[8];
  // 0xCC
  u8 last_discount[8];
  // 0xD4
  u8 had_shortages[8];
  // 0xDC
  u8 unpaid_bill_level[8];
  // 0xE4
  u8 annoyed;
  // 0xE5
  u8 gapE5[2];
  // 0xE7
  u8 byteE7;
  // 0xE8
  u8 num_times_said[8];
  // 0xF0
  u8 gapF0;
  // 0xF1
  u8 bitmaps;
  // 0xF2
  u8 gapF2[5];
  // 0xF7
  u8 price_addon[24];
  // 0x10F
  i16 num_items_left[24];
} PTSavedSupplier;
_Static_assert(sizeof(PTSavedSupplier) == 319, "PTSavedSupplier size");
_Static_assert(offsetof(PTSavedSupplier, link) == 0, "PTSavedSupplier.link");
_Static_assert(offsetof(PTSavedSupplier, stock_kind) == 8, "PTSavedSupplier.stock_kind");
_Static_assert(offsetof(PTSavedSupplier, quality) == 9, "PTSavedSupplier.quality");
_Static_assert(offsetof(PTSavedSupplier, supplier_id) == 10, "PTSavedSupplier.supplier_id");
_Static_assert(offsetof(PTSavedSupplier, base_price_addon) == 11, "PTSavedSupplier.base_price_addon");
_Static_assert(offsetof(PTSavedSupplier, name) == 12, "PTSavedSupplier.name");
_Static_assert(offsetof(PTSavedSupplier, minimum_order) == 38, "PTSavedSupplier.minimum_order");
_Static_assert(offsetof(PTSavedSupplier, bonus_levels) == 40, "PTSavedSupplier.bonus_levels");
_Static_assert(offsetof(PTSavedSupplier, total_bought_by_player) == 44, "PTSavedSupplier.total_bought_by_player");
_Static_assert(offsetof(PTSavedSupplier, gap4C) == 76, "PTSavedSupplier.gap4C");
_Static_assert(offsetof(PTSavedSupplier, last_term_length) == 140, "PTSavedSupplier.last_term_length");
_Static_assert(
    offsetof(PTSavedSupplier, num_contracts_with_player) == 148, "PTSavedSupplier.num_contracts_with_player");
_Static_assert(offsetof(PTSavedSupplier, last_amount) == 156, "PTSavedSupplier.last_amount");
_Static_assert(offsetof(PTSavedSupplier, current_account) == 172, "PTSavedSupplier.current_account");
_Static_assert(offsetof(PTSavedSupplier, last_discount) == 204, "PTSavedSupplier.last_discount");
_Static_assert(offsetof(PTSavedSupplier, had_shortages) == 212, "PTSavedSupplier.had_shortages");
_Static_assert(offsetof(PTSavedSupplier, unpaid_bill_level) == 220, "PTSavedSupplier.unpaid_bill_level");
_Static_assert(offsetof(PTSavedSupplier, annoyed) == 228, "PTSavedSupplier.annoyed");
_Static_assert(offsetof(PTSavedSupplier, gapE5) == 229, "PTSavedSupplier.gapE5");
_Static_assert(offsetof(PTSavedSupplier, byteE7) == 231, "PTSavedSupplier.byteE7");
_Static_assert(offsetof(PTSavedSupplier, num_times_said) == 232, "PTSavedSupplier.num_times_said");
_Static_assert(offsetof(PTSavedSupplier, gapF0) == 240, "PTSavedSupplier.gapF0");
_Static_assert(offsetof(PTSavedSupplier, bitmaps) == 241, "PTSavedSupplier.bitmaps");
_Static_assert(offsetof(PTSavedSupplier, gapF2) == 242, "PTSavedSupplier.gapF2");
_Static_assert(offsetof(PTSavedSupplier, price_addon) == 247, "PTSavedSupplier.price_addon");
_Static_assert(offsetof(PTSavedSupplier, num_items_left) == 271, "PTSavedSupplier.num_items_left");

typedef struct PTSavedZone {
  // 0x0
  PTSavedLinkedElem link;
  // 0x8
  u8 traffic_density;
  // 0x9
  u8 zone_id;
  // 0xA
  u8 name[10];
  // 0x14
  u8 gap14[18];
  // 0x26
  u8 active_mafia;
  // 0x27
  u8 gap27[17];
  // 0x38
  u8 byte38;
  // 0x39
  u8 crime_statistics;
  // 0x3A
  u8 byte3A;
  // 0x3B
  u8 how_low_income;
  // 0x3C
  u8 byte3C;
  u8 unknown_3D[1];
  // 0x3E
  u8 byte3E;
  // 0x3F
  u8 byte3F;
  // 0x40
  u8 gap40;
  // 0x41
  u8 type_of_zone;
  // 0x42
  i16 base_size;
  // 0x44
  i16 rand_size;
  // 0x46
  i16 base_price_level;
  // 0x48
  i16 base_rent_level;
  // 0x4A
  u8 gap4A[4];
  // 0x4E
  u8 premises_availability;
  u8 unknown_4F[1];
  // 0x50
  float population_distribution[6];
} PTSavedZone;
_Static_assert(sizeof(PTSavedZone) == 104, "PTSavedZone size");
_Static_assert(offsetof(PTSavedZone, link) == 0, "PTSavedZone.link");
_Static_assert(offsetof(PTSavedZone, traffic_density) == 8, "PTSavedZone.traffic_density");
_Static_assert(offsetof(PTSavedZone, zone_id) == 9, "PTSavedZone.zone_id");
_Static_assert(offsetof(PTSavedZone, name) == 10, "PTSavedZone.name");
_Static_assert(offsetof(PTSavedZone, gap14) == 20, "PTSavedZone.gap14");
_Static_assert(offsetof(PTSavedZone, active_mafia) == 38, "PTSavedZone.active_mafia");
_Static_assert(offsetof(PTSavedZone, gap27) == 39, "PTSavedZone.gap27");
_Static_assert(offsetof(PTSavedZone, byte38) == 56, "PTSavedZone.byte38");
_Static_assert(offsetof(PTSavedZone, crime_statistics) == 57, "PTSavedZone.crime_statistics");
_Static_assert(offsetof(PTSavedZone, byte3A) == 58, "PTSavedZone.byte3A");
_Static_assert(offsetof(PTSavedZone, how_low_income) == 59, "PTSavedZone.how_low_income");
_Static_assert(offsetof(PTSavedZone, byte3C) == 60, "PTSavedZone.byte3C");
_Static_assert(offsetof(PTSavedZone, byte3E) == 62, "PTSavedZone.byte3E");
_Static_assert(offsetof(PTSavedZone, byte3F) == 63, "PTSavedZone.byte3F");
_Static_assert(offsetof(PTSavedZone, gap40) == 64, "PTSavedZone.gap40");
_Static_assert(offsetof(PTSavedZone, type_of_zone) == 65, "PTSavedZone.type_of_zone");
_Static_assert(offsetof(PTSavedZone, base_size) == 66, "PTSavedZone.base_size");
_Static_assert(offsetof(PTSavedZone, rand_size) == 68, "PTSavedZone.rand_size");
_Static_assert(offsetof(PTSavedZone, base_price_level) == 70, "PTSavedZone.base_price_level");
_Static_assert(offsetof(PTSavedZone, base_rent_level) == 72, "PTSavedZone.base_rent_level");
_Static_assert(offsetof(PTSavedZone, gap4A) == 74, "PTSavedZone.gap4A");
_Static_assert(offsetof(PTSavedZone, premises_availability) == 78, "PTSavedZone.premises_availability");
_Static_assert(offsetof(PTSavedZone, population_distribution) == 80, "PTSavedZone.population_distribution");

typedef struct PTSavedPoi {
  // 0x0
  PTSavedLinkedElem link;
  // 0x8
  u8 xcoord;
  // 0x9
  u8 ycoord;
  // 0xA
  PropertyType property_type;
  // 0xB
  u8 current_owner;
  // 0xC
  u8 current_tenant;
  // 0xD
  u8 for_sale_or_rent_flags;
  // 0xE
  u8 map_value;
  // 0xF
  u8 realtor_index;
  // 0x10
  union {
    u8 serialized_restaurant[4];
    u32 serialized_restaurant_word;
  };
  // 0x14
  u8 size_x;
  // 0x15
  u8 size_y;
  // 0x16
  u8 kitchen_width;
  // 0x17
  u8 kitchen_height;
  // 0x18
  u8 door_x_start;
  // 0x19
  u8 left_door_y_end;
  // 0x1A
  u8 byte1A;
  // 0x1B
  u8 months_left_of_lease;
  // 0x1C
  i32 purchase_price;
  // 0x20
  i32 rent_price;
  // 0x24
  u8 field_24;
  // 0x25
  u8 field_25;
  // 0x26
  u8 field_26;
  // 0x27
  u8 field_27;
  // 0x28
  i16 customer_count_in_area[6];
  // 0x34
  i32 total_customer_count;
} PTSavedPoi;
_Static_assert(sizeof(PTSavedPoi) == 56, "PTSavedPoi size");
_Static_assert(offsetof(PTSavedPoi, link) == 0, "PTSavedPoi.link");
_Static_assert(offsetof(PTSavedPoi, xcoord) == 8, "PTSavedPoi.xcoord");
_Static_assert(offsetof(PTSavedPoi, ycoord) == 9, "PTSavedPoi.ycoord");
_Static_assert(offsetof(PTSavedPoi, property_type) == 10, "PTSavedPoi.property_type");
_Static_assert(offsetof(PTSavedPoi, current_owner) == 11, "PTSavedPoi.current_owner");
_Static_assert(offsetof(PTSavedPoi, current_tenant) == 12, "PTSavedPoi.current_tenant");
_Static_assert(offsetof(PTSavedPoi, for_sale_or_rent_flags) == 13, "PTSavedPoi.for_sale_or_rent_flags");
_Static_assert(offsetof(PTSavedPoi, map_value) == 14, "PTSavedPoi.map_value");
_Static_assert(offsetof(PTSavedPoi, realtor_index) == 15, "PTSavedPoi.realtor_index");
_Static_assert(offsetof(PTSavedPoi, serialized_restaurant) == 16, "PTSavedPoi.restaurant");
_Static_assert(offsetof(PTSavedPoi, size_x) == 20, "PTSavedPoi.size_x");
_Static_assert(offsetof(PTSavedPoi, size_y) == 21, "PTSavedPoi.size_y");
_Static_assert(offsetof(PTSavedPoi, kitchen_width) == 22, "PTSavedPoi.kitchen_width");
_Static_assert(offsetof(PTSavedPoi, kitchen_height) == 23, "PTSavedPoi.kitchen_height");
_Static_assert(offsetof(PTSavedPoi, door_x_start) == 24, "PTSavedPoi.door_x_start");
_Static_assert(offsetof(PTSavedPoi, left_door_y_end) == 25, "PTSavedPoi.left_door_y_end");
_Static_assert(offsetof(PTSavedPoi, byte1A) == 26, "PTSavedPoi.byte1A");
_Static_assert(offsetof(PTSavedPoi, months_left_of_lease) == 27, "PTSavedPoi.months_left_of_lease");
_Static_assert(offsetof(PTSavedPoi, purchase_price) == 28, "PTSavedPoi.purchase_price");
_Static_assert(offsetof(PTSavedPoi, rent_price) == 32, "PTSavedPoi.rent_price");
_Static_assert(offsetof(PTSavedPoi, field_24) == 36, "PTSavedPoi.field_24");
_Static_assert(offsetof(PTSavedPoi, field_25) == 37, "PTSavedPoi.field_25");
_Static_assert(offsetof(PTSavedPoi, field_26) == 38, "PTSavedPoi.field_26");
_Static_assert(offsetof(PTSavedPoi, field_27) == 39, "PTSavedPoi.field_27");
_Static_assert(offsetof(PTSavedPoi, customer_count_in_area) == 40, "PTSavedPoi.customer_count_in_area");
_Static_assert(offsetof(PTSavedPoi, total_customer_count) == 52, "PTSavedPoi.total_customer_count");

typedef struct PTSavedRecipe {
  // 0x0
  PTSavedLinkedElem linked;
  // 0x8
  i16 rating[7];
  // 0x16
  u8 cooking_skill_of_char;
  // 0x17
  u8 id;
  // 0x18
  u8 name[10];
  // 0x22
  u8 field_22;
  // 0x23
  u8 field_23;
  // 0x24
  u8 field_24;
  // 0x25
  u8 field_25;
  // 0x26
  u8 field_26;
  // 0x27
  u8 field_27;
  // 0x28
  u8 field_28;
  // 0x29
  u8 field_29;
  // 0x2A
  u8 field_2A;
  // 0x2B
  u8 field_2B;
  // 0x2C
  u8 field_2C;
  // 0x2D
  u8 field_2D;
  // 0x2E
  u8 field_2E;
  // 0x2F
  u8 field_2F;
  // 0x30
  u8 field_30;
  // 0x31
  u8 field_31;
  // 0x32
  u8 field_32;
  // 0x33
  u8 field_33;
  // 0x34
  u8 field_34;
  // 0x35
  u8 field_35;
  // 0x36
  u8 field_36;
  // 0x37
  u8 field_37;
  // 0x38
  u8 field_38;
  // 0x39
  u8 field_39;
  // 0x3A
  u8 field_3A;
  // 0x3B
  u8 field_3B;
  // 0x3C
  i16 word3C;
  // 0x3E
  u8 gap3E[3];
  // 0x41
  u8 imitation;
  u8 unknown_42[2];
  // 0x44
  u8 major_ingredient[4];
  // 0x48
  i16 fat;
  // 0x4A
  i16 sweet;
  // 0x4C
  i16 salty;
  // 0x4E
  i16 hot;
  // 0x50
  u8 appearance;
  // 0x51
  u8 refinement;
  // 0x52
  u8 normality;
  // 0x53
  u8 amount_of_spices;
  // 0x54
  u8 age_type_rating[6];
  // 0x5A
  i16 calories;
  // 0x5C
  u8 field_5C;
  // 0x5D
  u8 field_5D;
  // 0x5E
  u8 field_5E;
  // 0x5F
  u8 field_5F;
  // 0x60
  u8 field_60;
  // 0x61
  u8 field_61;
  // 0x62
  i16 weight_of_type[96];
  // 0x122
  u8 num_unique_ingredients;
  // 0x123
  u8 num_entries;
  // 0x124
  RecipeEntry entries;
} PTSavedRecipe;
_Static_assert(sizeof(PTSavedRecipe) == 296, "PTSavedRecipe size");
_Static_assert(offsetof(PTSavedRecipe, linked) == 0, "PTSavedRecipe.linked");
_Static_assert(offsetof(PTSavedRecipe, rating) == 8, "PTSavedRecipe.rating");
_Static_assert(offsetof(PTSavedRecipe, cooking_skill_of_char) == 22, "PTSavedRecipe.cooking_skill_of_char");
_Static_assert(offsetof(PTSavedRecipe, id) == 23, "PTSavedRecipe.id");
_Static_assert(offsetof(PTSavedRecipe, name) == 24, "PTSavedRecipe.name");
_Static_assert(offsetof(PTSavedRecipe, field_22) == 34, "PTSavedRecipe.field_22");
_Static_assert(offsetof(PTSavedRecipe, field_23) == 35, "PTSavedRecipe.field_23");
_Static_assert(offsetof(PTSavedRecipe, field_24) == 36, "PTSavedRecipe.field_24");
_Static_assert(offsetof(PTSavedRecipe, field_25) == 37, "PTSavedRecipe.field_25");
_Static_assert(offsetof(PTSavedRecipe, field_26) == 38, "PTSavedRecipe.field_26");
_Static_assert(offsetof(PTSavedRecipe, field_27) == 39, "PTSavedRecipe.field_27");
_Static_assert(offsetof(PTSavedRecipe, field_28) == 40, "PTSavedRecipe.field_28");
_Static_assert(offsetof(PTSavedRecipe, field_29) == 41, "PTSavedRecipe.field_29");
_Static_assert(offsetof(PTSavedRecipe, field_2A) == 42, "PTSavedRecipe.field_2A");
_Static_assert(offsetof(PTSavedRecipe, field_2B) == 43, "PTSavedRecipe.field_2B");
_Static_assert(offsetof(PTSavedRecipe, field_2C) == 44, "PTSavedRecipe.field_2C");
_Static_assert(offsetof(PTSavedRecipe, field_2D) == 45, "PTSavedRecipe.field_2D");
_Static_assert(offsetof(PTSavedRecipe, field_2E) == 46, "PTSavedRecipe.field_2E");
_Static_assert(offsetof(PTSavedRecipe, field_2F) == 47, "PTSavedRecipe.field_2F");
_Static_assert(offsetof(PTSavedRecipe, field_30) == 48, "PTSavedRecipe.field_30");
_Static_assert(offsetof(PTSavedRecipe, field_31) == 49, "PTSavedRecipe.field_31");
_Static_assert(offsetof(PTSavedRecipe, field_32) == 50, "PTSavedRecipe.field_32");
_Static_assert(offsetof(PTSavedRecipe, field_33) == 51, "PTSavedRecipe.field_33");
_Static_assert(offsetof(PTSavedRecipe, field_34) == 52, "PTSavedRecipe.field_34");
_Static_assert(offsetof(PTSavedRecipe, field_35) == 53, "PTSavedRecipe.field_35");
_Static_assert(offsetof(PTSavedRecipe, field_36) == 54, "PTSavedRecipe.field_36");
_Static_assert(offsetof(PTSavedRecipe, field_37) == 55, "PTSavedRecipe.field_37");
_Static_assert(offsetof(PTSavedRecipe, field_38) == 56, "PTSavedRecipe.field_38");
_Static_assert(offsetof(PTSavedRecipe, field_39) == 57, "PTSavedRecipe.field_39");
_Static_assert(offsetof(PTSavedRecipe, field_3A) == 58, "PTSavedRecipe.field_3A");
_Static_assert(offsetof(PTSavedRecipe, field_3B) == 59, "PTSavedRecipe.field_3B");
_Static_assert(offsetof(PTSavedRecipe, word3C) == 60, "PTSavedRecipe.word3C");
_Static_assert(offsetof(PTSavedRecipe, gap3E) == 62, "PTSavedRecipe.gap3E");
_Static_assert(offsetof(PTSavedRecipe, imitation) == 65, "PTSavedRecipe.imitation");
_Static_assert(offsetof(PTSavedRecipe, major_ingredient) == 68, "PTSavedRecipe.major_ingredient");
_Static_assert(offsetof(PTSavedRecipe, fat) == 72, "PTSavedRecipe.fat");
_Static_assert(offsetof(PTSavedRecipe, sweet) == 74, "PTSavedRecipe.sweet");
_Static_assert(offsetof(PTSavedRecipe, salty) == 76, "PTSavedRecipe.salty");
_Static_assert(offsetof(PTSavedRecipe, hot) == 78, "PTSavedRecipe.hot");
_Static_assert(offsetof(PTSavedRecipe, appearance) == 80, "PTSavedRecipe.appearance");
_Static_assert(offsetof(PTSavedRecipe, refinement) == 81, "PTSavedRecipe.refinement");
_Static_assert(offsetof(PTSavedRecipe, normality) == 82, "PTSavedRecipe.normality");
_Static_assert(offsetof(PTSavedRecipe, amount_of_spices) == 83, "PTSavedRecipe.amount_of_spices");
_Static_assert(offsetof(PTSavedRecipe, age_type_rating) == 84, "PTSavedRecipe.age_type_rating");
_Static_assert(offsetof(PTSavedRecipe, calories) == 90, "PTSavedRecipe.calories");
_Static_assert(offsetof(PTSavedRecipe, field_5C) == 92, "PTSavedRecipe.field_5C");
_Static_assert(offsetof(PTSavedRecipe, field_5D) == 93, "PTSavedRecipe.field_5D");
_Static_assert(offsetof(PTSavedRecipe, field_5E) == 94, "PTSavedRecipe.field_5E");
_Static_assert(offsetof(PTSavedRecipe, field_5F) == 95, "PTSavedRecipe.field_5F");
_Static_assert(offsetof(PTSavedRecipe, field_60) == 96, "PTSavedRecipe.field_60");
_Static_assert(offsetof(PTSavedRecipe, field_61) == 97, "PTSavedRecipe.field_61");
_Static_assert(offsetof(PTSavedRecipe, weight_of_type) == 98, "PTSavedRecipe.weight_of_type");
_Static_assert(offsetof(PTSavedRecipe, num_unique_ingredients) == 290, "PTSavedRecipe.num_unique_ingredients");
_Static_assert(offsetof(PTSavedRecipe, num_entries) == 291, "PTSavedRecipe.num_entries");
_Static_assert(offsetof(PTSavedRecipe, entries) == 292, "PTSavedRecipe.entries");

typedef struct PTSavedPlayer {
  // 0x0
  i16 field_0;
  // 0x2
  u8 gap2[11];
  // 0xD
  u8 city_index;
  // 0xE
  u8 gapE;
  // 0xF
  u8 num_weapons[8];
  // 0x17
  i32 average_weapon_price[8];
  // 0x37
  PlayerDialledNumbers city_part;
  // 0x79
  u8 num_restaurants_in_city[10];
  // 0x83
  union {
    u8 serialized_depot_in_town[10][4];
    u32 serialized_depot_in_town_word[10];
  };
  // 0xAB
  i32 weekly_profit_by_town[10];
  // 0xD3
  i32 profit_last_30_days[30];
  // 0x14B
  double money;
  // 0x153
  union {
    u8 serialized_selected_poi[4];
    u32 serialized_selected_poi_word;
  };
  // 0x157
  PTSavedLinkedHead restaurants;
  // 0x163
  PTSavedLinkedHead recipes;
  // 0x16F
  PTSavedLinkedHead contracts;
  // 0x17B
  u8 num_recipes;
  // 0x17C
  u8 num_restaurants;
  // 0x17D
  u8 num_contracts;
  // 0x17E
  u8 character_index;
  // 0x17F
  AiRecipe ai_recipes[10];
  // 0x233
  u8 num_days_passed_in_town[10];
  // 0x23D
  u8 num_restaurants_open;
  // 0x23E
  u8 gap23E[13];
  // 0x24B: payment refusal/overdraft penalty
  i16 debt_penalty;
  // 0x24D
  u8 byte24D;
  // 0x24E
  u8 gap24E[3];
  // 0x251
  Mission mission;
  // 0x285
  u8 name;
  // 0x286
  u8 random_name;
  // 0x287
  u8 gap287[18];
  union {
    struct {
      // 0x299
      i32 money_as_integer[2];
      // 0x2A1
      u8 gap2A1[188];
      // 0x35D
      i32 new_money_as_integer;
    };
    // 0x299: original physical weekly history
    PTCashHistoryEntry cash_history[50];
  };
  // 0x361
  u8 gap361;
  // 0x362
  u8 index_of_next_goal;
  // 0x363
  u8 days_away;
  // 0x364
  u8 gap364;
  // 0x365
  i16 sales_per_day_monthly[30];
  // 0x3A1
  i32 accum_sales_per_day_monthly[30];
  // 0x419
  i32 expenditure_monthly[30];
  // 0x491
  Peach peaches[4];
  // 0x4A9
  u8 last_insurance_damage_type;
  // 0x4AA: stock writes monthly=1, supplier=4; stock never reads
  u8 bankruptcy_reason_hint;
  // 0x4AB
  i32 last_crashed_furniture_value;
} PTSavedPlayer;
_Static_assert(offsetof(PTSavedPlayer, cash_history) == 665, "PTSavedPlayer.cash_history offset");
_Static_assert(sizeof(((PTSavedPlayer *)0)->cash_history) == 200, "fifty cash history slots");
_Static_assert(offsetof(PTSavedPlayer, cash_history) + 49 * sizeof(PTCashHistoryEntry) ==
        offsetof(PTSavedPlayer, new_money_as_integer),
    "final history slot overlaps current cash");
_Static_assert(sizeof(PTSavedPlayer) == 1199, "PTSavedPlayer size");
_Static_assert(offsetof(PTSavedPlayer, field_0) == 0, "PTSavedPlayer.field_0");
_Static_assert(offsetof(PTSavedPlayer, gap2) == 2, "PTSavedPlayer.gap2");
_Static_assert(offsetof(PTSavedPlayer, city_index) == 13, "PTSavedPlayer.city_index");
_Static_assert(offsetof(PTSavedPlayer, gapE) == 14, "PTSavedPlayer.gapE");
_Static_assert(offsetof(PTSavedPlayer, num_weapons) == 15, "PTSavedPlayer.num_weapons");
_Static_assert(offsetof(PTSavedPlayer, average_weapon_price) == 23, "PTSavedPlayer.average_weapon_price");
_Static_assert(offsetof(PTSavedPlayer, city_part) == 55, "PTSavedPlayer.city_part");
_Static_assert(offsetof(PTSavedPlayer, num_restaurants_in_city) == 121, "PTSavedPlayer.num_restaurants_in_city");
_Static_assert(offsetof(PTSavedPlayer, serialized_depot_in_town) == 131, "PTSavedPlayer.depot_in_town");
_Static_assert(offsetof(PTSavedPlayer, weekly_profit_by_town) == 171, "PTSavedPlayer.weekly_profit_by_town");
_Static_assert(offsetof(PTSavedPlayer, profit_last_30_days) == 211, "PTSavedPlayer.profit_last_30_days");
_Static_assert(offsetof(PTSavedPlayer, money) == 331, "PTSavedPlayer.money");
_Static_assert(offsetof(PTSavedPlayer, serialized_selected_poi) == 339, "PTSavedPlayer.selected_poi");
_Static_assert(offsetof(PTSavedPlayer, restaurants) == 343, "PTSavedPlayer.restaurants");
_Static_assert(offsetof(PTSavedPlayer, recipes) == 355, "PTSavedPlayer.recipes");
_Static_assert(offsetof(PTSavedPlayer, contracts) == 367, "PTSavedPlayer.contracts");
_Static_assert(offsetof(PTSavedPlayer, num_recipes) == 379, "PTSavedPlayer.num_recipes");
_Static_assert(offsetof(PTSavedPlayer, num_restaurants) == 380, "PTSavedPlayer.num_restaurants");
_Static_assert(offsetof(PTSavedPlayer, num_contracts) == 381, "PTSavedPlayer.num_contracts");
_Static_assert(offsetof(PTSavedPlayer, character_index) == 382, "PTSavedPlayer.character_index");
_Static_assert(offsetof(PTSavedPlayer, ai_recipes) == 383, "PTSavedPlayer.ai_recipes");
_Static_assert(offsetof(PTSavedPlayer, num_days_passed_in_town) == 563, "PTSavedPlayer.num_days_passed_in_town");
_Static_assert(offsetof(PTSavedPlayer, num_restaurants_open) == 573, "PTSavedPlayer.num_restaurants_open");
_Static_assert(offsetof(PTSavedPlayer, gap23E) == 574, "PTSavedPlayer.gap23E");
_Static_assert(offsetof(PTSavedPlayer, debt_penalty) == 587, "PTSavedPlayer.debt_penalty");
_Static_assert(offsetof(PTSavedPlayer, byte24D) == 589, "PTSavedPlayer.byte24D");
_Static_assert(offsetof(PTSavedPlayer, gap24E) == 590, "PTSavedPlayer.gap24E");
_Static_assert(offsetof(PTSavedPlayer, mission) == 593, "PTSavedPlayer.mission");
_Static_assert(offsetof(PTSavedPlayer, name) == 645, "PTSavedPlayer.name");
_Static_assert(offsetof(PTSavedPlayer, random_name) == 646, "PTSavedPlayer.random_name");
_Static_assert(offsetof(PTSavedPlayer, gap287) == 647, "PTSavedPlayer.gap287");
_Static_assert(offsetof(PTSavedPlayer, money_as_integer) == 665, "PTSavedPlayer.money_as_integer");
_Static_assert(offsetof(PTSavedPlayer, gap2A1) == 673, "PTSavedPlayer.gap2A1");
_Static_assert(offsetof(PTSavedPlayer, new_money_as_integer) == 861, "PTSavedPlayer.new_money_as_integer");
_Static_assert(offsetof(PTSavedPlayer, gap361) == 865, "PTSavedPlayer.gap361");
_Static_assert(offsetof(PTSavedPlayer, index_of_next_goal) == 866, "PTSavedPlayer.index_of_next_goal");
_Static_assert(offsetof(PTSavedPlayer, days_away) == 867, "PTSavedPlayer.days_away");
_Static_assert(offsetof(PTSavedPlayer, gap364) == 868, "PTSavedPlayer.gap364");
_Static_assert(offsetof(PTSavedPlayer, sales_per_day_monthly) == 869, "PTSavedPlayer.sales_per_day_monthly");
_Static_assert(
    offsetof(PTSavedPlayer, accum_sales_per_day_monthly) == 929, "PTSavedPlayer.accum_sales_per_day_monthly");
_Static_assert(offsetof(PTSavedPlayer, expenditure_monthly) == 1049, "PTSavedPlayer.expenditure_monthly");
_Static_assert(offsetof(PTSavedPlayer, peaches) == 1169, "PTSavedPlayer.peaches");
_Static_assert(offsetof(PTSavedPlayer, last_insurance_damage_type) == 1193, "PTSavedPlayer.last_insurance_damage_type");
_Static_assert(offsetof(PTSavedPlayer, bankruptcy_reason_hint) == 1194, "PTSavedPlayer.bankruptcy_reason_hint");
_Static_assert(
    offsetof(PTSavedPlayer, last_crashed_furniture_value) == 1195, "PTSavedPlayer.last_crashed_furniture_value");

typedef struct PTSavedGame {
  // 0x0
  u8 field_0;
  // 0x1
  u8 field_1;
  // 0x2
  u8 field_2;
  // 0x3
  u8 field_3;
  // 0x4
  u8 field_4;
  // 0x5
  u8 field_5;
  // 0x6
  u8 field_6;
  u8 unknown_7[1];
  // 0x8
  i32 dword8;
  // 0xC
  u8 gapC;
  // 0xD
  u8 is_mission_mode;
  // 0xE
  u8 in_menu_hierarchy;
  // 0xF
  u8 disable_staff_disasters;
  // 0x10
  u8 gap10[24];
  // 0x28
  u8 num_interactive_players;
  // 0x29
  u8 mask_of_players;
  // 0x2A
  u8 gap2A[4];
  // 0x2E
  u8 byte2E;
  // 0x2F
  u8 paragraph_first_indent_height;
  // 0x30
  u8 paragraph_indent_y_threshold;
  // 0x31
  u8 list_element_spacing;
  // 0x32
  u8 paragraph_background_enabled;
  // 0x33
  u8 paragraph_first_indent_height_backup;
  // 0x34
  u8 paragraph_indent_y_threshold_backup;
  // 0x35
  u8 paragraph_background_enabled_backup;
  // 0x36
  u8 gap36[24];
  // 0x4E
  union {
    u8 serialized_palette_slots[2][4];
    u32 serialized_palette_slots_word[2];
  };
  // 0x56
  u8 gap56[20];
  // 0x6A
  union {
    u8 serialized_background_bitmap[4];
    u32 serialized_background_bitmap_word;
  };
  // 0x6E
  u8 gap6E[19];
  // 0x81
  u8 field_81;
  // 0x82
  u8 last_loaded_rest_bg;
  // 0x83
  u8 mouse_cursor_visible;
  // 0x84
  u8 current_palette;
  // 0x85
  u8 info_box_state;
  // 0x86
  u8 byte86;
  // 0x87
  u8 byte87;
  // 0x88
  u8 gap88;
  // 0x89
  u8 allow_save_game;
  // 0x8A
  u8 did_set_bitmap;
  // 0x8B
  u8 current_button_menu;
  // 0x8C
  u8 some_button_menu;
  // 0x8D
  u8 list_element_spacing_2;
  // 0x8E
  u8 list_element_spacing_backup;
  // 0x8F
  u8 did_choose_tv_thingy;
  // 0x90
  i16 word90;
  // 0x92
  i16 word92;
  // 0x94
  u8 byte94;
  // 0x95
  u8 legacy_box_text_color;
  // 0x96
  u8 legacy_box_top_left_color;
  // 0x97
  u8 legacy_box_bottom_right_color;
  // 0x98
  u16 legacy_box_width;
  // 0x9A
  u16 legacy_box_height;
  // 0x9C
  u16 legacy_box_text_offset_x;
  // 0x9E
  u8 last_loaded_blitmap_index;
  // 0x9F
  u8 last_loaded_background;
  // 0xA0
  u8 gapA0[2];
  // 0xA2
  u8 sound_enabled;
  // 0xA3
  u8 gapA3[3];
  // 0xA6
  u8 canvas_color_backup;
  // 0xA7
  u8 canvas_byteE_backup;
  // 0xA8
  u8 canvas_byte10_backup;
  u8 unknown_A9[1];
  // 0xAA
  u8 byteAA;
  // 0xAB
  u8 byteAB;
  // 0xAC
  u8 current_bitmap_index;
  // 0xAD
  u8 old_bitmap_index;
  // 0xAE
  u8 swapped_when_setting_bitmap;
  // 0xAF
  u8 gapAF[2];
  // 0xB1
  u8 debug_mode;
  // 0xB2
  u8 byteB2;
  // 0xB3
  u8 force_freestyle_jury;
  // 0xB4
  u8 byteB4;
  // 0xB5
  i16 canvas_position_x_backup;
  // 0xB7
  i16 canvas_position_y_backup;
  // 0xB9
  u8 flagB9;
  // 0xBA
  u8 show_time_left_instead_of_date;
  // 0xBB
  u32 legacy_timer_value;
  // 0xBF
  i32 seconds_left;
  // 0xC3
  i32 minutes_left;
  // 0xC7
  u8 gapC7;
  // 0xC8
  u8 map_zoom_level;
  // 0xC9
  u8 gapC9[11];
  // 0xD4
  u8 field_D4;
  // 0xD5
  u8 gapD5[7];
  // 0xDC
  u8 field_DC;
  // 0xDD
  union {
    u8 serialized_canvas_font_backup[4];
    u32 serialized_canvas_font_backup_word;
  };
  // 0xE1
  union {
    u8 serialized_ptr_to_players[4];
    u32 serialized_ptr_to_players_word;
  };
  // 0xE5
  union {
    u8 serialized_ptr_to_garlics[4];
    u32 serialized_ptr_to_garlics_word;
  };
  union {
    // 0xE9
    u8 gapE9[16];
    struct {
      u8 graphics_reserved[8];
      union {
        u8 serialized_graphics_scratch_first[4];
        u32 serialized_graphics_scratch_first_word;
      };
      union {
        u8 serialized_graphics_scratch_second[4];
        u32 serialized_graphics_scratch_second_word;
      };
    };
  };
  // 0xF9
  i32 dwordF9;
  // 0xFD
  Rect phone_text_rect;
  // 0x105
  u8 phone_text_rect_flag;
  // 0x106
  u8 phone_text_rect_color;
  // 0x107
  u8 byte106;
  // 0x108
  u8 byte107;
  // 0x109
  u16 legacy_word109;
  // 0x10B
  u16 legacy_word10B;
  union {
    struct {
      // 0x10D
      u8 field_10C;
      // 0x10E
      u8 unknown_10E;
    };
    u16 legacy_word10D;
  };
  // 0x10F
  u8 text_edit_max_width;
  // 0x110
  u8 byte10F;
  // 0x111
  u8 skip_missing_file_dialog;
  // 0x112
  u8 byte111;
  // 0x113
  u8 byte112;
  // 0x114
  u8 paragraph_render_mode;
  // 0x115
  u8 byte114;
  // 0x116
  u8 show_disk_read_errors;
  // 0x117
  u8 is_loading_from_buffer;
  // 0x118
  union {
    u8 serialized_loading_buffer[4];
    u32 serialized_loading_buffer_word;
  };
  // 0x11C
  union {
    u8 serialized_loading_from_buffer_ptr[4];
    u32 serialized_loading_from_buffer_ptr_word;
  };
  // 0x120
  u32 loading_buffer_size;
  // 0x124
  i32 dword124;
  // 0x128
  union {
    u8 serialized_active_langfile[4];
    u32 serialized_active_langfile_word;
  };
  // 0x12C
  union {
    u8 serialized_langfile[4];
    u32 serialized_langfile_word;
  };
} PTSavedGame;
_Static_assert(sizeof(PTSavedGame) == 304, "PTSavedGame size");
_Static_assert(offsetof(PTSavedGame, field_0) == 0, "PTSavedGame.field_0");
_Static_assert(offsetof(PTSavedGame, field_1) == 1, "PTSavedGame.field_1");
_Static_assert(offsetof(PTSavedGame, field_2) == 2, "PTSavedGame.field_2");
_Static_assert(offsetof(PTSavedGame, field_3) == 3, "PTSavedGame.field_3");
_Static_assert(offsetof(PTSavedGame, field_4) == 4, "PTSavedGame.field_4");
_Static_assert(offsetof(PTSavedGame, field_5) == 5, "PTSavedGame.field_5");
_Static_assert(offsetof(PTSavedGame, field_6) == 6, "PTSavedGame.field_6");
_Static_assert(offsetof(PTSavedGame, dword8) == 8, "PTSavedGame.dword8");
_Static_assert(offsetof(PTSavedGame, gapC) == 12, "PTSavedGame.gapC");
_Static_assert(offsetof(PTSavedGame, is_mission_mode) == 13, "PTSavedGame.is_mission_mode");
_Static_assert(offsetof(PTSavedGame, in_menu_hierarchy) == 14, "PTSavedGame.in_menu_hierarchy");
_Static_assert(offsetof(PTSavedGame, disable_staff_disasters) == 15, "PTSavedGame.disable_staff_disasters");
_Static_assert(offsetof(PTSavedGame, gap10) == 16, "PTSavedGame.gap10");
_Static_assert(offsetof(PTSavedGame, num_interactive_players) == 40, "PTSavedGame.num_interactive_players");
_Static_assert(offsetof(PTSavedGame, mask_of_players) == 41, "PTSavedGame.mask_of_players");
_Static_assert(offsetof(PTSavedGame, gap2A) == 42, "PTSavedGame.gap2A");
_Static_assert(offsetof(PTSavedGame, byte2E) == 46, "PTSavedGame.byte2E");
_Static_assert(offsetof(PTSavedGame, paragraph_first_indent_height) == 47, "PTSavedGame.paragraph_first_indent_height");
_Static_assert(offsetof(PTSavedGame, paragraph_indent_y_threshold) == 48, "PTSavedGame.paragraph_indent_y_threshold");
_Static_assert(offsetof(PTSavedGame, list_element_spacing) == 49, "PTSavedGame.list_element_spacing");
_Static_assert(offsetof(PTSavedGame, paragraph_background_enabled) == 50, "PTSavedGame.paragraph_background_enabled");
_Static_assert(offsetof(PTSavedGame, paragraph_first_indent_height_backup) == 51,
    "PTSavedGame.paragraph_first_indent_height_backup");
_Static_assert(offsetof(PTSavedGame, paragraph_indent_y_threshold_backup) == 52,
    "PTSavedGame.paragraph_indent_y_threshold_backup");
_Static_assert(offsetof(PTSavedGame, paragraph_background_enabled_backup) == 53,
    "PTSavedGame.paragraph_background_enabled_backup");
_Static_assert(offsetof(PTSavedGame, gap36) == 54, "PTSavedGame.gap36");
_Static_assert(offsetof(PTSavedGame, serialized_palette_slots) == 78, "PTSavedGame.pal_p");
_Static_assert(offsetof(PTSavedGame, gap56) == 86, "PTSavedGame.gap56");
_Static_assert(offsetof(PTSavedGame, serialized_background_bitmap) == 106, "PTSavedGame.background_bitmap");
_Static_assert(offsetof(PTSavedGame, gap6E) == 110, "PTSavedGame.gap6E");
_Static_assert(offsetof(PTSavedGame, field_81) == 129, "PTSavedGame.field_81");
_Static_assert(offsetof(PTSavedGame, last_loaded_rest_bg) == 130, "PTSavedGame.last_loaded_rest_bg");
_Static_assert(offsetof(PTSavedGame, mouse_cursor_visible) == 131, "PTSavedGame.mouse_cursor_visible");
_Static_assert(offsetof(PTSavedGame, current_palette) == 132, "PTSavedGame.current_palette");
_Static_assert(offsetof(PTSavedGame, info_box_state) == 133, "PTSavedGame.info_box_state");
_Static_assert(offsetof(PTSavedGame, byte86) == 134, "PTSavedGame.byte86");
_Static_assert(offsetof(PTSavedGame, byte87) == 135, "PTSavedGame.byte87");
_Static_assert(offsetof(PTSavedGame, gap88) == 136, "PTSavedGame.gap88");
_Static_assert(offsetof(PTSavedGame, allow_save_game) == 137, "PTSavedGame.allow_save_game");
_Static_assert(offsetof(PTSavedGame, did_set_bitmap) == 138, "PTSavedGame.did_set_bitmap");
_Static_assert(offsetof(PTSavedGame, current_button_menu) == 139, "PTSavedGame.current_button_menu");
_Static_assert(offsetof(PTSavedGame, some_button_menu) == 140, "PTSavedGame.some_button_menu");
_Static_assert(offsetof(PTSavedGame, list_element_spacing_2) == 141, "PTSavedGame.list_element_spacing_2");
_Static_assert(offsetof(PTSavedGame, list_element_spacing_backup) == 142, "PTSavedGame.list_element_spacing_backup");
_Static_assert(offsetof(PTSavedGame, did_choose_tv_thingy) == 143, "PTSavedGame.did_choose_tv_thingy");
_Static_assert(offsetof(PTSavedGame, word90) == 144, "PTSavedGame.word90");
_Static_assert(offsetof(PTSavedGame, word92) == 146, "PTSavedGame.word92");
_Static_assert(offsetof(PTSavedGame, byte94) == 148, "PTSavedGame.byte94");
_Static_assert(offsetof(PTSavedGame, legacy_box_text_color) == 149, "PTSavedGame.legacy_box_text_color");
_Static_assert(offsetof(PTSavedGame, legacy_box_top_left_color) == 150, "PTSavedGame.legacy_box_top_left_color");
_Static_assert(
    offsetof(PTSavedGame, legacy_box_bottom_right_color) == 151, "PTSavedGame.legacy_box_bottom_right_color");
_Static_assert(offsetof(PTSavedGame, legacy_box_width) == 152, "PTSavedGame.legacy_box_width");
_Static_assert(offsetof(PTSavedGame, legacy_box_height) == 154, "PTSavedGame.legacy_box_height");
_Static_assert(offsetof(PTSavedGame, legacy_box_text_offset_x) == 156, "PTSavedGame.legacy_box_text_offset_x");
_Static_assert(offsetof(PTSavedGame, last_loaded_blitmap_index) == 158, "PTSavedGame.last_loaded_blitmap_index");
_Static_assert(offsetof(PTSavedGame, last_loaded_background) == 159, "PTSavedGame.last_loaded_background");
_Static_assert(offsetof(PTSavedGame, gapA0) == 160, "PTSavedGame.gapA0");
_Static_assert(offsetof(PTSavedGame, sound_enabled) == 162, "PTSavedGame.sound_enabled");
_Static_assert(offsetof(PTSavedGame, gapA3) == 163, "PTSavedGame.gapA3");
_Static_assert(offsetof(PTSavedGame, canvas_color_backup) == 166, "PTSavedGame.canvas_color_backup");
_Static_assert(offsetof(PTSavedGame, canvas_byteE_backup) == 167, "PTSavedGame.canvas_byteE_backup");
_Static_assert(offsetof(PTSavedGame, canvas_byte10_backup) == 168, "PTSavedGame.canvas_byte10_backup");
_Static_assert(offsetof(PTSavedGame, byteAA) == 170, "PTSavedGame.byteAA");
_Static_assert(offsetof(PTSavedGame, byteAB) == 171, "PTSavedGame.byteAB");
_Static_assert(offsetof(PTSavedGame, current_bitmap_index) == 172, "PTSavedGame.current_bitmap_index");
_Static_assert(offsetof(PTSavedGame, old_bitmap_index) == 173, "PTSavedGame.old_bitmap_index");
_Static_assert(offsetof(PTSavedGame, swapped_when_setting_bitmap) == 174, "PTSavedGame.swapped_when_setting_bitmap");
_Static_assert(offsetof(PTSavedGame, gapAF) == 175, "PTSavedGame.gapAF");
_Static_assert(offsetof(PTSavedGame, debug_mode) == 177, "PTSavedGame.debug_mode");
_Static_assert(offsetof(PTSavedGame, byteB2) == 178, "PTSavedGame.byteB2");
_Static_assert(offsetof(PTSavedGame, force_freestyle_jury) == 179, "PTSavedGame.force_freestyle_jury");
_Static_assert(offsetof(PTSavedGame, byteB4) == 180, "PTSavedGame.byteB4");
_Static_assert(offsetof(PTSavedGame, canvas_position_x_backup) == 181, "PTSavedGame.canvas_position_x_backup");
_Static_assert(offsetof(PTSavedGame, canvas_position_y_backup) == 183, "PTSavedGame.canvas_position_y_backup");
_Static_assert(offsetof(PTSavedGame, flagB9) == 185, "PTSavedGame.flagB9");
_Static_assert(
    offsetof(PTSavedGame, show_time_left_instead_of_date) == 186, "PTSavedGame.show_time_left_instead_of_date");
_Static_assert(offsetof(PTSavedGame, legacy_timer_value) == 187, "PTSavedGame.legacy_timer_value");
_Static_assert(offsetof(PTSavedGame, seconds_left) == 191, "PTSavedGame.seconds_left");
_Static_assert(offsetof(PTSavedGame, minutes_left) == 195, "PTSavedGame.minutes_left");
_Static_assert(offsetof(PTSavedGame, gapC7) == 199, "PTSavedGame.gapC7");
_Static_assert(offsetof(PTSavedGame, map_zoom_level) == 200, "PTSavedGame.map_zoom_level");
_Static_assert(offsetof(PTSavedGame, gapC9) == 201, "PTSavedGame.gapC9");
_Static_assert(offsetof(PTSavedGame, field_D4) == 212, "PTSavedGame.field_D4");
_Static_assert(offsetof(PTSavedGame, gapD5) == 213, "PTSavedGame.gapD5");
_Static_assert(offsetof(PTSavedGame, field_DC) == 220, "PTSavedGame.field_DC");
_Static_assert(offsetof(PTSavedGame, serialized_canvas_font_backup) == 221, "PTSavedGame.canvas_font_backup");
_Static_assert(offsetof(PTSavedGame, serialized_ptr_to_players) == 225, "PTSavedGame.ptr_to_players");
_Static_assert(offsetof(PTSavedGame, serialized_ptr_to_garlics) == 229, "PTSavedGame.ptr_to_garlics");
_Static_assert(offsetof(PTSavedGame, gapE9) == 233, "PTSavedGame.gapE9");
_Static_assert(offsetof(PTSavedGame, serialized_graphics_scratch_first) == 241, "PTSavedGame.graphics_scratch_first");
_Static_assert(offsetof(PTSavedGame, serialized_graphics_scratch_second) == 245, "PTSavedGame.graphics_scratch_second");
_Static_assert(offsetof(PTSavedGame, dwordF9) == 249, "PTSavedGame.dwordF9");
_Static_assert(offsetof(PTSavedGame, phone_text_rect) == 253, "PTSavedGame.phone_text_rect");
_Static_assert(offsetof(PTSavedGame, phone_text_rect_flag) == 261, "PTSavedGame.phone_text_rect_flag");
_Static_assert(offsetof(PTSavedGame, phone_text_rect_color) == 262, "PTSavedGame.phone_text_rect_color");
_Static_assert(offsetof(PTSavedGame, byte106) == 263, "PTSavedGame.byte106");
_Static_assert(offsetof(PTSavedGame, byte107) == 264, "PTSavedGame.byte107");
_Static_assert(offsetof(PTSavedGame, legacy_word109) == 265, "PTSavedGame.legacy_word109");
_Static_assert(offsetof(PTSavedGame, legacy_word10B) == 267, "PTSavedGame.legacy_word10B");
_Static_assert(offsetof(PTSavedGame, field_10C) == 269, "PTSavedGame.field_10C");
_Static_assert(offsetof(PTSavedGame, legacy_word10D) == 269, "PTSavedGame.legacy_word10D");
_Static_assert(offsetof(PTSavedGame, unknown_10E) == 270, "PTSavedGame.unknown_10E");
_Static_assert(offsetof(PTSavedGame, text_edit_max_width) == 271, "PTSavedGame.text_edit_max_width");
_Static_assert(offsetof(PTSavedGame, byte10F) == 272, "PTSavedGame.byte10F");
_Static_assert(offsetof(PTSavedGame, skip_missing_file_dialog) == 273, "PTSavedGame.skip_missing_file_dialog");
_Static_assert(offsetof(PTSavedGame, byte111) == 274, "PTSavedGame.byte111");
_Static_assert(offsetof(PTSavedGame, byte112) == 275, "PTSavedGame.byte112");
_Static_assert(offsetof(PTSavedGame, paragraph_render_mode) == 276, "PTSavedGame.paragraph_render_mode");
_Static_assert(offsetof(PTSavedGame, byte114) == 277, "PTSavedGame.byte114");
_Static_assert(offsetof(PTSavedGame, show_disk_read_errors) == 278, "PTSavedGame.show_disk_read_errors");
_Static_assert(offsetof(PTSavedGame, is_loading_from_buffer) == 279, "PTSavedGame.is_loading_from_buffer");
_Static_assert(offsetof(PTSavedGame, serialized_loading_buffer) == 280, "PTSavedGame.loading_buffer");
_Static_assert(offsetof(PTSavedGame, serialized_loading_from_buffer_ptr) == 284, "PTSavedGame.loading_from_buffer_ptr");
_Static_assert(offsetof(PTSavedGame, loading_buffer_size) == 288, "PTSavedGame.loading_buffer_size");
_Static_assert(offsetof(PTSavedGame, dword124) == 292, "PTSavedGame.dword124");
_Static_assert(offsetof(PTSavedGame, serialized_active_langfile) == 296, "PTSavedGame.active_langfile");
_Static_assert(offsetof(PTSavedGame, serialized_langfile) == 300, "PTSavedGame.langfile");

typedef struct PTSavedApple {
  // 0x0
  u8 field_0;
  // 0x1
  u8 field_1;
  // 0x2
  u8 field_2;
  // 0x3
  u8 field_3;
  // 0x4
  u8 field_4;
  // 0x5
  u8 field_5;
  // 0x6
  u8 field_6;
  // 0x7
  u8 field_7;
  // 0x8
  u8 field_8;
  // 0x9
  u8 field_9;
  // 0xA
  u8 field_A;
  // 0xB
  u8 field_B;
  // 0xC
  u8 field_C;
  // 0xD
  u8 clock_flag;
  // 0xE
  union {
    u8 serialized_restaurant[4];
    u32 serialized_restaurant_word;
  };
  // 0x12
  PTSavedLinkedHead links;
  // 0x1E
  u8 arr1E;
  // 0x1F
  u8 town_index;
  // 0x20
  u8 weekday;
  // 0x21
  u8 is_town_inited[10];
  // 0x2B
  u8 minutesx10;
  // 0x2C
  u8 day;
  // 0x2D
  u8 month;
  // 0x2E
  i16 year;
  // 0x30
  u8 field_30;
  // 0x31
  u8 field_31;
  // 0x32
  u8 is_in_kitchen;
  // 0x33
  u8 message_ticks_left;
  // 0x34
  u8 field_34;
  // 0x35
  u8 byte35;
  // 0x36
  u8 player_index;
  // 0x37
  u8 difficulty_level;
  // 0x38
  u8 byte38;
  // 0x39
  u8 byte39;
  // 0x3A
  u8 num_days_with_proper_stats;
  // 0x3B
  u8 num_valid_daily_profits_entries;
  // 0x3C
  u8 gap3C;
  // 0x3D
  i32 town_sizes[10];
  // 0x65
  u8 field_65;
  // 0x66
  u8 view_state;
} PTSavedApple;
_Static_assert(sizeof(PTSavedApple) == 103, "PTSavedApple size");
_Static_assert(offsetof(PTSavedApple, field_0) == 0, "PTSavedApple.field_0");
_Static_assert(offsetof(PTSavedApple, field_1) == 1, "PTSavedApple.field_1");
_Static_assert(offsetof(PTSavedApple, field_2) == 2, "PTSavedApple.field_2");
_Static_assert(offsetof(PTSavedApple, field_3) == 3, "PTSavedApple.field_3");
_Static_assert(offsetof(PTSavedApple, field_4) == 4, "PTSavedApple.field_4");
_Static_assert(offsetof(PTSavedApple, field_5) == 5, "PTSavedApple.field_5");
_Static_assert(offsetof(PTSavedApple, field_6) == 6, "PTSavedApple.field_6");
_Static_assert(offsetof(PTSavedApple, field_7) == 7, "PTSavedApple.field_7");
_Static_assert(offsetof(PTSavedApple, field_8) == 8, "PTSavedApple.field_8");
_Static_assert(offsetof(PTSavedApple, field_9) == 9, "PTSavedApple.field_9");
_Static_assert(offsetof(PTSavedApple, field_A) == 10, "PTSavedApple.field_A");
_Static_assert(offsetof(PTSavedApple, field_B) == 11, "PTSavedApple.field_B");
_Static_assert(offsetof(PTSavedApple, field_C) == 12, "PTSavedApple.field_C");
_Static_assert(offsetof(PTSavedApple, clock_flag) == 13, "PTSavedApple.clock_flag");
_Static_assert(offsetof(PTSavedApple, serialized_restaurant) == 14, "PTSavedApple.restaurant");
_Static_assert(offsetof(PTSavedApple, links) == 18, "PTSavedApple.links");
_Static_assert(offsetof(PTSavedApple, links) + offsetof(PTSavedLinkedHead, elem) == 22, "room sentinel");
_Static_assert(
    offsetof(PTSavedApple, links) + offsetof(PTSavedLinkedHead, elem) + offsetof(PTSavedLinkedElem, serialized_prevp) ==
        26,
    "room tail link");
_Static_assert(offsetof(PTSavedApple, arr1E) == 30, "PTSavedApple.arr1E");
_Static_assert(offsetof(PTSavedApple, town_index) == 31, "PTSavedApple.town_index");
_Static_assert(offsetof(PTSavedApple, weekday) == 32, "PTSavedApple.weekday");
_Static_assert(offsetof(PTSavedApple, is_town_inited) == 33, "PTSavedApple.is_town_inited");
_Static_assert(offsetof(PTSavedApple, minutesx10) == 43, "PTSavedApple.minutesx10");
_Static_assert(offsetof(PTSavedApple, day) == 44, "PTSavedApple.day");
_Static_assert(offsetof(PTSavedApple, month) == 45, "PTSavedApple.month");
_Static_assert(offsetof(PTSavedApple, year) == 46, "PTSavedApple.year");
_Static_assert(offsetof(PTSavedApple, field_30) == 48, "PTSavedApple.field_30");
_Static_assert(offsetof(PTSavedApple, field_31) == 49, "PTSavedApple.field_31");
_Static_assert(offsetof(PTSavedApple, is_in_kitchen) == 50, "PTSavedApple.is_in_kitchen");
_Static_assert(offsetof(PTSavedApple, message_ticks_left) == 51, "PTSavedApple.message_ticks_left");
_Static_assert(offsetof(PTSavedApple, field_34) == 52, "PTSavedApple.field_34");
_Static_assert(offsetof(PTSavedApple, byte35) == 53, "PTSavedApple.byte35");
_Static_assert(offsetof(PTSavedApple, player_index) == 54, "PTSavedApple.player_index");
_Static_assert(offsetof(PTSavedApple, difficulty_level) == 55, "PTSavedApple.difficulty_level");
_Static_assert(offsetof(PTSavedApple, byte38) == 56, "PTSavedApple.byte38");
_Static_assert(offsetof(PTSavedApple, byte39) == 57, "PTSavedApple.byte39");
_Static_assert(offsetof(PTSavedApple, num_days_with_proper_stats) == 58, "PTSavedApple.num_days_with_proper_stats");
_Static_assert(
    offsetof(PTSavedApple, num_valid_daily_profits_entries) == 59, "PTSavedApple.num_valid_daily_profits_entries");
_Static_assert(offsetof(PTSavedApple, gap3C) == 60, "PTSavedApple.gap3C");
_Static_assert(offsetof(PTSavedApple, town_sizes) == 61, "PTSavedApple.town_sizes");
_Static_assert(offsetof(PTSavedApple, field_65) == 101, "PTSavedApple.field_65");
_Static_assert(offsetof(PTSavedApple, view_state) == 102, "PTSavedApple.view_state");

typedef struct PTSavedRest {
  // 0x0
  PTSavedLinkedElem linked;
  // 0x8
  union {
    u8 serialized_poi[4];
    u32 serialized_poi_word;
  };
  // 0xC
  u8 player_index;
  u8 unknown_D[1];
  // 0xE
  u8 restaurant_range_squared;
  // 0xF
  u8 restaurant_range;
  // 0x10
  u8 num_stars;
  // 0x11
  u8 delivcapa_per_day[7];
  // 0x18
  u8 restaurant_id;
  // 0x19
  u8 days_until_major_visit;
  // 0x1A
  u8 protection_value;
  // 0x1B
  i16 average_pizza_price;
  // 0x1D
  i16 word1D;
  // 0x1F
  i16 word1F;
  // 0x21
  u8 popularity_with_customer[6];
  // 0x27
  i16 sum_of_reputations;
  // 0x29
  i16 byte29;
  // 0x2B
  u8 byte2B;
  // 0x2C
  u8 byte2C;
  // 0x2D
  u8 quality_of_things[4];
  // 0x31
  u8 reputation[8];
  // 0x39
  u8 range_stuff_1[4];
  // 0x3D
  u8 range_stuff_2[4];
  // 0x41
  u8 gap41[4];
  // 0x45
  i16 customer_count[6];
  // 0x51
  u8 furniture_rating[6];
  // 0x57
  u8 menu_rating[6];
  // 0x5D
  i16 total_num_seats;
  // 0x5F
  u8 gap5F[4];
  // 0x63
  i16 oven_capacity;
  // 0x65
  u8 gap65[2];
  // 0x67
  i16 demand_of_kind[5];
  // 0x71
  i16 capacity_of_kind[5];
  // 0x7B
  u8 demand_capacity_ratio[5];
  union {
    struct {
      // 0x80
      u8 furniture_quality;
      u8 unknown_81[1];
      // 0x82
      u8 average_cooking_rating;
      // 0x83
      u8 oven_quality;
      // 0x84
      u8 average_waiter_rating;
    };
    u8 quality_block[5];
  };
  // 0x85
  u8 original_map_value;
  // 0x86
  i16 attack_var_1;
  // 0x88
  u8 local_share;
  // 0x89
  u8 restaurant_closed_for_days;
  // 0x8A
  u8 gap8A[2];
  // 0x8C
  u8 byte8C;
  // 0x8D
  u8 target_coordinate_x;
  // 0x8E
  u8 target_coordinate_y;
  // 0x8F
  u8 byte8F;
  // 0x90
  u8 gap90;
  // 0x91
  i16 word91;
  // 0x93
  u8 gap93[4];
  // 0x97
  u8 restaurant_opened;
  // 0x98
  u8 use_real_demand;
  // 0x99
  u8 byte99;
  // 0x9A
  u8 gap9A;
  // 0x9B
  u8 pizza_quality;
  // 0x9C
  u8 attack_var;
  // 0x9D
  u8 some_counter;
  // 0x9E
  u8 attacked_by_whom;
  // 0x9F
  u8 ai_preferred_age_group;
  // 0xA0
  u8 default_rating;
  // 0xA1
  u8 template_rest;
  // 0xA2
  u8 some_furniture_counter;
  // 0xA3
  u8 gapA3[4];
  // 0xA7
  i16 orders_per_day[7];
  // 0xB5
  i16 pizza_count_per_day[7];
  // 0xC3
  i16 demand_value_per_day[7];
  // 0xD1
  i16 sales_per_day[7];
  // 0xDF
  i32 expenditure_per_day[7];
  // 0xFB
  i16 expenses_per_day[7];
  // 0x109
  u8 gap109[2];
  // 0x10B
  i32 curr_month_expenses[12];
  // 0x13B
  i32 curr_month_receipts[6];
  // 0x153
  i32 last_month_expenses[12];
  // 0x183
  i32 last_month_receipts[6];
  // 0x19B
  i16 guests_per_day[7];
  // 0x1A9
  u8 gap1A9[2];
  // 0x1AB
  float average_chef_time;
  // 0x1AF
  float average_oven_time;
  // 0x1B3
  float average_waiter_time;
  // 0x1B7
  float average_eating_time;
  // 0x1BB
  float average_stay_time;
  // 0x1BF
  u8 num_chefs;
  // 0x1C0
  u8 num_waiters;
  // 0x1C1
  u8 staff_indexes[13];
  // 0x1CE
  u8 gap1CE[13];
  // 0x1DB
  u8 mgr_shopping;
  // 0x1DC
  u8 mgr_change_prices;
  // 0x1DD
  u8 mgr_overdraw_acct;
  // 0x1DE
  u8 mgr_turnover_decrease;
  // 0x1DF
  u8 mgr_turnover_increase;
  // 0x1E0
  u8 mgr_supply_shortage;
  // 0x1E1
  u8 mgr_staff_shortage;
  // 0x1E2
  u8 mgr_staff_behavior;
  // 0x1E3
  u8 manager_flag_1;
  // 0x1E4
  u8 manager_had_to_help;
  // 0x1E5
  u8 gap1E5[6];
  // 0x1EB
  u8 waiting_time;
  // 0x1EC
  u8 num_recipes;
  // 0x1ED
  u8 num_standard_recipes;
  // 0x1EE
  u8 num_ovens;
  // 0x1EF
  u8 gap1EF[5];
  // 0x1F4
  i32 dword1F4;
  // 0x1F8
  PTSavedRestoRecipe recipes[24];
  // 0x378
  u8 gap378[7];
  // 0x37F
  u8 average_purchase_quality;
  // 0x380
  u8 gap380;
  // 0x381
  u8 num_furniture;
  // 0x382
  u8 gap382[6];
  // 0x388
  u8 byte388;
  // 0x389
  u8 gap389[6];
  // 0x38F
  u8 byte38F;
  // 0x390
  u8 gap390[5];
  // 0x395
  u8 scrollpos_x;
  // 0x396
  u8 scrollpos_y;
  // 0x397
  u8 kitchen_scroll_x;
  // 0x398
  u8 kitchen_scroll_y;
  union {
    struct {
      union {
        struct {
          // 0x399
          Floor floor;
          // 0x3A0
          u8 base_beautiness[6];
        };
        FloorPurchase floor_purchase;
      };
      // 0x3A6
      u8 field_3A6[19];
    };
    RestaurantFloorState floor_state;
  };
  // 0x3B9
  u8 field_3B9;
  // 0x3BA
  u8 is_in_kitchen;
  // 0x3BB
  u8 jukebox_selection;
  // 0x3BC
  u8 tv_selection;
  // 0x3BD
  RestaurantStock stock[96];
  // 0x5FD
  Furniture furniture[100];
  // 0x981
  Furniture ovens[12];
  // 0x9ED
  i16 weekly_ingredient_type_cost[4];
  // 0x9F5
  u8 last_supplier_of_stock[4];
  // 0x9F9
  u8 field_9F9;
} PTSavedRest;
_Static_assert(sizeof(PTSavedRest) == 2554, "PTSavedRest size");
_Static_assert(offsetof(PTSavedRest, floor_state) == 921, "PTSavedRest.floor_state");
_Static_assert(offsetof(PTSavedRest, linked) == 0, "PTSavedRest.linked");
_Static_assert(offsetof(PTSavedRest, serialized_poi) == 8, "PTSavedRest.poi");
_Static_assert(offsetof(PTSavedRest, player_index) == 12, "PTSavedRest.player_index");
_Static_assert(offsetof(PTSavedRest, restaurant_range_squared) == 14, "PTSavedRest.restaurant_range_squared");
_Static_assert(offsetof(PTSavedRest, restaurant_range) == 15, "PTSavedRest.restaurant_range");
_Static_assert(offsetof(PTSavedRest, num_stars) == 16, "PTSavedRest.num_stars");
_Static_assert(offsetof(PTSavedRest, delivcapa_per_day) == 17, "PTSavedRest.delivcapa_per_day");
_Static_assert(offsetof(PTSavedRest, restaurant_id) == 24, "PTSavedRest.restaurant_id");
_Static_assert(offsetof(PTSavedRest, days_until_major_visit) == 25, "PTSavedRest.days_until_major_visit");
_Static_assert(offsetof(PTSavedRest, protection_value) == 26, "PTSavedRest.protection_value");
_Static_assert(offsetof(PTSavedRest, average_pizza_price) == 27, "PTSavedRest.average_pizza_price");
_Static_assert(offsetof(PTSavedRest, word1D) == 29, "PTSavedRest.word1D");
_Static_assert(offsetof(PTSavedRest, word1F) == 31, "PTSavedRest.word1F");
_Static_assert(offsetof(PTSavedRest, popularity_with_customer) == 33, "PTSavedRest.popularity_with_customer");
_Static_assert(offsetof(PTSavedRest, sum_of_reputations) == 39, "PTSavedRest.sum_of_reputations");
_Static_assert(offsetof(PTSavedRest, byte29) == 41, "PTSavedRest.byte29");
_Static_assert(offsetof(PTSavedRest, byte2B) == 43, "PTSavedRest.byte2B");
_Static_assert(offsetof(PTSavedRest, byte2C) == 44, "PTSavedRest.byte2C");
_Static_assert(offsetof(PTSavedRest, quality_of_things) == 45, "PTSavedRest.quality_of_things");
_Static_assert(offsetof(PTSavedRest, reputation) == 49, "PTSavedRest.reputation");
_Static_assert(offsetof(PTSavedRest, range_stuff_1) == 57, "PTSavedRest.range_stuff_1");
_Static_assert(offsetof(PTSavedRest, range_stuff_2) == 61, "PTSavedRest.range_stuff_2");
_Static_assert(offsetof(PTSavedRest, gap41) == 65, "PTSavedRest.gap41");
_Static_assert(offsetof(PTSavedRest, customer_count) == 69, "PTSavedRest.customer_count");
_Static_assert(offsetof(PTSavedRest, furniture_rating) == 81, "PTSavedRest.furniture_rating");
_Static_assert(offsetof(PTSavedRest, menu_rating) == 87, "PTSavedRest.menu_rating");
_Static_assert(offsetof(PTSavedRest, total_num_seats) == 93, "PTSavedRest.total_num_seats");
_Static_assert(offsetof(PTSavedRest, gap5F) == 95, "PTSavedRest.gap5F");
_Static_assert(offsetof(PTSavedRest, oven_capacity) == 99, "PTSavedRest.oven_capacity");
_Static_assert(offsetof(PTSavedRest, gap65) == 101, "PTSavedRest.gap65");
_Static_assert(offsetof(PTSavedRest, demand_of_kind) == 103, "PTSavedRest.demand_of_kind");
_Static_assert(offsetof(PTSavedRest, capacity_of_kind) == 113, "PTSavedRest.capacity_of_kind");
_Static_assert(offsetof(PTSavedRest, demand_capacity_ratio) == 123, "PTSavedRest.demand_capacity_ratio");
_Static_assert(offsetof(PTSavedRest, furniture_quality) == 128, "PTSavedRest.furniture_quality");
_Static_assert(offsetof(PTSavedRest, quality_block) == 128, "PTSavedRest.quality_block");
_Static_assert(offsetof(PTSavedRest, average_cooking_rating) == 130, "PTSavedRest.average_cooking_rating");
_Static_assert(offsetof(PTSavedRest, oven_quality) == 131, "PTSavedRest.oven_quality");
_Static_assert(offsetof(PTSavedRest, average_waiter_rating) == 132, "PTSavedRest.average_waiter_rating");
_Static_assert(offsetof(PTSavedRest, original_map_value) == 133, "PTSavedRest.original_map_value");
_Static_assert(offsetof(PTSavedRest, attack_var_1) == 134, "PTSavedRest.attack_var_1");
_Static_assert(offsetof(PTSavedRest, local_share) == 136, "PTSavedRest.local_share");
_Static_assert(offsetof(PTSavedRest, restaurant_closed_for_days) == 137, "PTSavedRest.restaurant_closed_for_days");
_Static_assert(offsetof(PTSavedRest, gap8A) == 138, "PTSavedRest.gap8A");
_Static_assert(offsetof(PTSavedRest, byte8C) == 140, "PTSavedRest.byte8C");
_Static_assert(offsetof(PTSavedRest, target_coordinate_x) == 141, "PTSavedRest.target_coordinate_x");
_Static_assert(offsetof(PTSavedRest, target_coordinate_y) == 142, "PTSavedRest.target_coordinate_y");
_Static_assert(offsetof(PTSavedRest, byte8F) == 143, "PTSavedRest.byte8F");
_Static_assert(offsetof(PTSavedRest, gap90) == 144, "PTSavedRest.gap90");
_Static_assert(offsetof(PTSavedRest, word91) == 145, "PTSavedRest.word91");
_Static_assert(offsetof(PTSavedRest, gap93) == 147, "PTSavedRest.gap93");
_Static_assert(offsetof(PTSavedRest, restaurant_opened) == 151, "PTSavedRest.restaurant_opened");
_Static_assert(offsetof(PTSavedRest, use_real_demand) == 152, "PTSavedRest.use_real_demand");
_Static_assert(offsetof(PTSavedRest, byte99) == 153, "PTSavedRest.byte99");
_Static_assert(offsetof(PTSavedRest, gap9A) == 154, "PTSavedRest.gap9A");
_Static_assert(offsetof(PTSavedRest, pizza_quality) == 155, "PTSavedRest.pizza_quality");
_Static_assert(offsetof(PTSavedRest, attack_var) == 156, "PTSavedRest.attack_var");
_Static_assert(offsetof(PTSavedRest, some_counter) == 157, "PTSavedRest.some_counter");
_Static_assert(offsetof(PTSavedRest, attacked_by_whom) == 158, "PTSavedRest.attacked_by_whom");
_Static_assert(offsetof(PTSavedRest, ai_preferred_age_group) == 159, "PTSavedRest.ai_preferred_age_group");
_Static_assert(offsetof(PTSavedRest, default_rating) == 160, "PTSavedRest.default_rating");
_Static_assert(offsetof(PTSavedRest, template_rest) == 161, "PTSavedRest.template_rest");
_Static_assert(offsetof(PTSavedRest, some_furniture_counter) == 162, "PTSavedRest.some_furniture_counter");
_Static_assert(offsetof(PTSavedRest, gapA3) == 163, "PTSavedRest.gapA3");
_Static_assert(offsetof(PTSavedRest, orders_per_day) == 167, "PTSavedRest.orders_per_day");
_Static_assert(offsetof(PTSavedRest, pizza_count_per_day) == 181, "PTSavedRest.pizza_count_per_day");
_Static_assert(offsetof(PTSavedRest, demand_value_per_day) == 195, "PTSavedRest.demand_value_per_day");
_Static_assert(offsetof(PTSavedRest, sales_per_day) == 209, "PTSavedRest.sales_per_day");
_Static_assert(offsetof(PTSavedRest, expenditure_per_day) == 223, "PTSavedRest.expenditure_per_day");
_Static_assert(offsetof(PTSavedRest, expenses_per_day) == 251, "PTSavedRest.expenses_per_day");
_Static_assert(offsetof(PTSavedRest, gap109) == 265, "PTSavedRest.gap109");
_Static_assert(offsetof(PTSavedRest, curr_month_expenses) == 267, "PTSavedRest.curr_month_expenses");
_Static_assert(offsetof(PTSavedRest, curr_month_receipts) == 315, "PTSavedRest.curr_month_receipts");
_Static_assert(offsetof(PTSavedRest, last_month_expenses) == 339, "PTSavedRest.last_month_expenses");
_Static_assert(offsetof(PTSavedRest, last_month_receipts) == 387, "PTSavedRest.last_month_receipts");
_Static_assert(offsetof(PTSavedRest, guests_per_day) == 411, "PTSavedRest.guests_per_day");
_Static_assert(offsetof(PTSavedRest, gap1A9) == 425, "PTSavedRest.gap1A9");
_Static_assert(offsetof(PTSavedRest, average_chef_time) == 427, "PTSavedRest.average_chef_time");
_Static_assert(offsetof(PTSavedRest, average_oven_time) == 431, "PTSavedRest.average_oven_time");
_Static_assert(offsetof(PTSavedRest, average_waiter_time) == 435, "PTSavedRest.average_waiter_time");
_Static_assert(offsetof(PTSavedRest, average_eating_time) == 439, "PTSavedRest.average_eating_time");
_Static_assert(offsetof(PTSavedRest, average_stay_time) == 443, "PTSavedRest.average_stay_time");
_Static_assert(offsetof(PTSavedRest, num_chefs) == 447, "PTSavedRest.num_chefs");
_Static_assert(offsetof(PTSavedRest, num_waiters) == 448, "PTSavedRest.num_waiters");
_Static_assert(offsetof(PTSavedRest, staff_indexes) == 449, "PTSavedRest.staff_indexes");
_Static_assert(offsetof(PTSavedRest, gap1CE) == 462, "PTSavedRest.gap1CE");
_Static_assert(offsetof(PTSavedRest, mgr_shopping) == 475, "PTSavedRest.mgr_shopping");
_Static_assert(offsetof(PTSavedRest, mgr_change_prices) == 476, "PTSavedRest.mgr_change_prices");
_Static_assert(offsetof(PTSavedRest, mgr_overdraw_acct) == 477, "PTSavedRest.mgr_overdraw_acct");
_Static_assert(offsetof(PTSavedRest, mgr_turnover_decrease) == 478, "PTSavedRest.mgr_turnover_decrease");
_Static_assert(offsetof(PTSavedRest, mgr_turnover_increase) == 479, "PTSavedRest.mgr_turnover_increase");
_Static_assert(offsetof(PTSavedRest, mgr_supply_shortage) == 480, "PTSavedRest.mgr_supply_shortage");
_Static_assert(offsetof(PTSavedRest, mgr_staff_shortage) == 481, "PTSavedRest.mgr_staff_shortage");
_Static_assert(offsetof(PTSavedRest, mgr_staff_behavior) == 482, "PTSavedRest.mgr_staff_behavior");
_Static_assert(offsetof(PTSavedRest, manager_flag_1) == 483, "PTSavedRest.manager_flag_1");
_Static_assert(offsetof(PTSavedRest, manager_had_to_help) == 484, "PTSavedRest.manager_had_to_help");
_Static_assert(offsetof(PTSavedRest, gap1E5) == 485, "PTSavedRest.gap1E5");
_Static_assert(offsetof(PTSavedRest, waiting_time) == 491, "PTSavedRest.waiting_time");
_Static_assert(offsetof(PTSavedRest, num_recipes) == 492, "PTSavedRest.num_recipes");
_Static_assert(offsetof(PTSavedRest, num_standard_recipes) == 493, "PTSavedRest.num_standard_recipes");
_Static_assert(offsetof(PTSavedRest, num_ovens) == 494, "PTSavedRest.num_ovens");
_Static_assert(offsetof(PTSavedRest, gap1EF) == 495, "PTSavedRest.gap1EF");
_Static_assert(offsetof(PTSavedRest, dword1F4) == 500, "PTSavedRest.dword1F4");
_Static_assert(offsetof(PTSavedRest, recipes) == 504, "PTSavedRest.recipes");
_Static_assert(offsetof(PTSavedRest, gap378) == 888, "PTSavedRest.gap378");
_Static_assert(offsetof(PTSavedRest, average_purchase_quality) == 895, "PTSavedRest.average_purchase_quality");
_Static_assert(offsetof(PTSavedRest, gap380) == 896, "PTSavedRest.gap380");
_Static_assert(offsetof(PTSavedRest, num_furniture) == 897, "PTSavedRest.num_furniture");
_Static_assert(offsetof(PTSavedRest, gap382) == 898, "PTSavedRest.gap382");
_Static_assert(offsetof(PTSavedRest, byte388) == 904, "PTSavedRest.byte388");
_Static_assert(offsetof(PTSavedRest, gap389) == 905, "PTSavedRest.gap389");
_Static_assert(offsetof(PTSavedRest, byte38F) == 911, "PTSavedRest.byte38F");
_Static_assert(offsetof(PTSavedRest, gap390) == 912, "PTSavedRest.gap390");
_Static_assert(offsetof(PTSavedRest, scrollpos_x) == 917, "PTSavedRest.scrollpos_x");
_Static_assert(offsetof(PTSavedRest, scrollpos_y) == 918, "PTSavedRest.scrollpos_y");
_Static_assert(offsetof(PTSavedRest, kitchen_scroll_x) == 919, "PTSavedRest.kitchen_scroll_x");
_Static_assert(offsetof(PTSavedRest, kitchen_scroll_y) == 920, "PTSavedRest.kitchen_scroll_y");
_Static_assert(offsetof(PTSavedRest, floor_purchase) == 921, "PTSavedRest.floor_purchase offset");
_Static_assert(offsetof(PTSavedRest, floor) == 921, "PTSavedRest.floor");
_Static_assert(offsetof(PTSavedRest, base_beautiness) == 928, "PTSavedRest.base_beautiness");
_Static_assert(offsetof(PTSavedRest, field_3A6) == 934, "PTSavedRest.field_3A6");
_Static_assert(offsetof(PTSavedRest, field_3B9) == 953, "PTSavedRest.field_3B9");
_Static_assert(offsetof(PTSavedRest, is_in_kitchen) == 954, "PTSavedRest.is_in_kitchen");
_Static_assert(offsetof(PTSavedRest, jukebox_selection) == 955, "PTSavedRest.jukebox_selection");
_Static_assert(offsetof(PTSavedRest, tv_selection) == 956, "PTSavedRest.tv_selection");
_Static_assert(offsetof(PTSavedRest, stock) == 957, "PTSavedRest.stock");
_Static_assert(offsetof(PTSavedRest, furniture) == 1533, "PTSavedRest.furniture");
_Static_assert(offsetof(PTSavedRest, ovens) == 2433, "PTSavedRest.ovens");
_Static_assert(offsetof(PTSavedRest, weekly_ingredient_type_cost) == 2541, "PTSavedRest.weekly_ingredient_type_cost");
_Static_assert(offsetof(PTSavedRest, last_supplier_of_stock) == 2549, "PTSavedRest.last_supplier_of_stock");
_Static_assert(offsetof(PTSavedRest, field_9F9) == 2553, "PTSavedRest.field_9F9");

typedef struct PTSavedMousePositionBinding {
  // 0x0
  union {
    u8 serialized_x[4];
    u32 serialized_x_word;
  };
  // 0x4
  union {
    u8 serialized_y[4];
    u32 serialized_y_word;
  };
} PTSavedMousePositionBinding;
_Static_assert(sizeof(PTSavedMousePositionBinding) == 8, "PTSavedMousePositionBinding size");
_Static_assert(offsetof(PTSavedMousePositionBinding, serialized_x) == 0, "PTSavedMousePositionBinding.x");
_Static_assert(offsetof(PTSavedMousePositionBinding, serialized_y) == 4, "PTSavedMousePositionBinding.y");

typedef struct PTSavedFileStreamRecord {
  // 0x0
  union {
    u8 serialized_current[4];
    u32 serialized_current_word;
  };
  // 0x4
  u32 buffered_bytes;
  // 0x8
  union {
    u8 serialized_buffer[4];
    u32 serialized_buffer_word;
  };
  union {
    // Original full-width status operations.
    u32 status_word;
    struct {
      // 0xC
      u8 flags;
      // 0xD
      u8 buffer_flags;
      // 0xE
      u8 unknown_e[2];
    };
  };
  // 0x10
  u32 handle;
  // 0x14
  u32 buffer_size;
  // 0x18
  u8 inline_buffer;
  // 0x19
  u8 temporary_index;
} PTSavedFileStreamRecord;
_Static_assert(sizeof(PTSavedFileStreamRecord) == 26, "PTSavedFileStreamRecord size");
_Static_assert(offsetof(PTSavedFileStreamRecord, status_word) == 12, "PTSavedFileStreamRecord.status_word");
_Static_assert(sizeof(((PTSavedFileStreamRecord *)0)->status_word) == 4, "PTSavedFileStreamRecord status width");
_Static_assert(offsetof(PTSavedFileStreamRecord, serialized_current) == 0, "PTSavedFileStreamRecord.current");
_Static_assert(offsetof(PTSavedFileStreamRecord, buffered_bytes) == 4, "PTSavedFileStreamRecord.buffered_bytes");
_Static_assert(offsetof(PTSavedFileStreamRecord, serialized_buffer) == 8, "PTSavedFileStreamRecord.buffer");
_Static_assert(offsetof(PTSavedFileStreamRecord, flags) == 12, "PTSavedFileStreamRecord.flags");
_Static_assert(offsetof(PTSavedFileStreamRecord, buffer_flags) == 13, "PTSavedFileStreamRecord.buffer_flags");
_Static_assert(offsetof(PTSavedFileStreamRecord, unknown_e) == 14, "PTSavedFileStreamRecord.unknown_e");
_Static_assert(offsetof(PTSavedFileStreamRecord, handle) == 16, "PTSavedFileStreamRecord.handle");
_Static_assert(offsetof(PTSavedFileStreamRecord, buffer_size) == 20, "PTSavedFileStreamRecord.buffer_size");
_Static_assert(offsetof(PTSavedFileStreamRecord, inline_buffer) == 24, "PTSavedFileStreamRecord.inline_buffer");
_Static_assert(offsetof(PTSavedFileStreamRecord, temporary_index) == 25, "PTSavedFileStreamRecord.temporary_index");

typedef struct PTSavedFileNodeRecord {
  // 0x0
  union {
    u8 serialized_next[4];
    u32 serialized_next_word;
  };
  // 0x4
  union {
    u8 serialized_stream[4];
    u32 serialized_stream_word;
  };
} PTSavedFileNodeRecord;
_Static_assert(sizeof(PTSavedFileNodeRecord) == 8, "PTSavedFileNodeRecord size");
_Static_assert(offsetof(PTSavedFileNodeRecord, serialized_next) == 0, "PTSavedFileNodeRecord.next");
_Static_assert(offsetof(PTSavedFileNodeRecord, serialized_stream) == 4, "PTSavedFileNodeRecord.stream");
// END GENERATED SAVED RECORDS

#pragma pack(pop)

#pragma pack(push, 1)
typedef struct EncodedFileHeader {
  // 0x0
  guest_ptr32 source_pointer;
} EncodedFileHeader;
_Static_assert(sizeof(EncodedFileHeader) == 4, "EncodedFileHeader size");
_Static_assert(offsetof(EncodedFileHeader, source_pointer) == 0, "EncodedFileHeader.source_pointer");
typedef struct PersonalCharacterFile {
  // 0x0
  u8 character_index;
  // 0x1
  EncodedFileHeader encoded_header;
  // 0x5
  u8 encoded_character[104];
} PersonalCharacterFile;
_Static_assert(sizeof(PersonalCharacterFile) == 109, "PersonalCharacterFile size");
_Static_assert(offsetof(PersonalCharacterFile, character_index) == 0, "PersonalCharacterFile.character_index");
_Static_assert(offsetof(PersonalCharacterFile, encoded_header) == 1, "PersonalCharacterFile.encoded_header");
_Static_assert(offsetof(PersonalCharacterFile, encoded_character) == 5, "PersonalCharacterFile.encoded_character");
#pragma pack(pop)
typedef enum PTSavegameBlockId {
#define SAVE_BLOCK(id, data, size, encoded, after_read) PT_SAVEGAME_BLOCK_##id,
#include "src/savegame_blocks.def"
#undef SAVE_BLOCK
  PT_SAVEGAME_BLOCKS
} PTSavegameBlockId;
typedef struct PTSavegameBlock {
  guest_ptr32 address;
  u32 size, encoded;
} PTSavegameBlock;
_Static_assert(sizeof(PTSavegameBlock) == 12, "savegame block host ABI");
// BEGIN GENERATED SOUND DMA RECORD

typedef struct SoundDMARecord {
  // 0x0
  union {
    u8 serialized_buffer[4];
    u32 serialized_buffer_word;
  };
  // 0x4
  u16 length;
  // 0x6
  u16 field_6;
  // 0x8
  u16 field_8;
  // 0xA
  u16 frequency;
} SoundDMARecord;
_Static_assert(sizeof(SoundDMARecord) == 12, "SoundDMARecord size");
_Static_assert(offsetof(SoundDMARecord, serialized_buffer) == 0, "SoundDMARecord.serialized_buffer");
_Static_assert(offsetof(SoundDMARecord, length) == 4, "SoundDMARecord.length");
_Static_assert(offsetof(SoundDMARecord, field_6) == 6, "SoundDMARecord.field_6");
_Static_assert(offsetof(SoundDMARecord, field_8) == 8, "SoundDMARecord.field_8");
_Static_assert(offsetof(SoundDMARecord, frequency) == 10, "SoundDMARecord.frequency");
// END GENERATED SOUND DMA RECORD

// Transfer words are opaque in native code; the owner binds real samples.

typedef struct __attribute__((packed, may_alias)) PTSavedSoundDMATransfer {
  union {
    u8 serialized_buffer[4];
    u32 serialized_buffer_word;
  };
  u16 length;
  union {
    u8 serialized_status[4];
    u32 serialized_status_word;
  };
  u16 frequency;
} PTSavedSoundDMATransfer;
_Static_assert(sizeof(PTSavedSoundDMATransfer) == sizeof(SoundDMARecord) &&
        offsetof(PTSavedSoundDMATransfer, length) == offsetof(SoundDMARecord, length) &&
        offsetof(PTSavedSoundDMATransfer, serialized_status) == offsetof(SoundDMARecord, field_6) &&
        offsetof(PTSavedSoundDMATransfer, frequency) == offsetof(SoundDMARecord, frequency),
    "DMA transfer layout");

// Bundled DIGPAK resident fields are offsets within its DOS code segment;
// the COM image begins at segment offset 0x100.

enum {
  PT_DIGPAK_irq = 0x208,
  PT_DIGPAK_dma8 = 0x20a,
  PT_DIGPAK_dma16 = 0x20c,
  PT_DIGPAK_dispatch = 0x242,
  PT_DIGPAK_playing = 0x244,
  PT_DIGPAK_flat_pointers = 0x254,
  PT_DIGPAK_previous_vector = 0x25d,
  PT_DIGPAK_dispatch_active = 0x77b,
  PT_DIGPAK_previous_irq = 0x7ff,
  PT_DIGPAK_dma_mask = 0x843,
  PT_DIGPAK_extended_dsp = 0x909,
  PT_DIGPAK_previous_pic_mask = 0xaba,
  PT_DIGPAK_install_vector = 0x1089
};
// Layout starts at resident CS:0240. Gaps include unrelated driver code and
// unidentified data; they remain mapped and are compared without exclusion.

typedef struct __attribute__((packed, may_alias)) PTSavedDigpakPlayback {
  u16 auto_dma, dispatch, playing, unknown246, callback_enabled;
  u32 callback;
  u16 callback_ds, stereo, sample_format, flat_pointers;
  u8 dma_half, unknown257[0x25d - 0x257];
  u32 previous_vector;
  u8 unknown261[0x77b - 0x261];
  u16 dispatch_active;
  u8 unknown77d[0x785 - 0x77d];
  u16 unknown785;
  u8 unknown787[0x797 - 0x787];
  u16 queued;
  PTSavedSoundDMATransfer queued_record;
  u8 unknown7a5[0x7ff - 0x7a5];
  PTSoundFarPointer previous_irq;
  u8 unknown803[0x813 - 0x803], dma_page;
  u16 dma_offset, block_count;
  u8 banks_left;
  u16 remaining, remaining_high, end_offset;
  u8 unknown81f[0x843 - 0x81f], dma_mask;
  u8 unknown844[0x909 - 0x844];
  u16 extended_dsp;
  u8 unknown90b[0x9b0 - 0x90b];
  u8 dma_channel, page_port, address_port, count_port, mask_port, mode_port, clear_port;
  u8 unknown9b7[0xaba - 0x9b7];
  u8 previous_primary_mask, previous_secondary_mask;
  u8 unknownabc[0xe53 - 0xabc], speaker;
} PTSavedDigpakPlayback;
_Static_assert(sizeof(PTSavedDigpakPlayback) == 0xe54 - 0x240, "DIGPAK playback extent");
_Static_assert(offsetof(PTSavedDigpakPlayback, callback) == 0x24a - 0x240 &&
        offsetof(PTSavedDigpakPlayback, dispatch_active) == 0x77b - 0x240 &&
        offsetof(PTSavedDigpakPlayback, queued_record) == 0x799 - 0x240 &&
        offsetof(PTSavedDigpakPlayback, dma_page) == 0x813 - 0x240 &&
        offsetof(PTSavedDigpakPlayback, remaining) == 0x819 - 0x240 &&
        offsetof(PTSavedDigpakPlayback, dma_channel) == 0x9b0 - 0x240 &&
        offsetof(PTSavedDigpakPlayback, speaker) == 0xe53 - 0x240,
    "DIGPAK playback offsets");

// Resident instance starts at CS:0100, exactly where the COM payload is
// loaded. Unknown gaps retain the original bytes; C never executes them.

typedef struct __attribute__((packed, may_alias)) PTSavedDigpakResident {
  u8 entry_bytes[3], signature[6], unknown109[0x208 - 0x109];
  u16 irq, dma8, dma16;
  u8 unknown20e[0x240 - 0x20e];
  PTSavedDigpakPlayback playback;
  u8 unknowne54[0x1089 - 0xe54];
  u16 install_interrupt;
  PTSoundFarPointer install_handler;
} PTSavedDigpakResident;
_Static_assert(sizeof(PTSavedDigpakResident) == 0x108f - 0x100, "DIGPAK resident extent");
#define PT_DIGPAK_FIELD_ASSERT(field, offset)                                                                          \
  _Static_assert(offsetof(PTSavedDigpakResident, field) == (offset) - 0x100, "DIGPAK resident " #field)
PT_DIGPAK_FIELD_ASSERT(signature, 0x103);
PT_DIGPAK_FIELD_ASSERT(irq, PT_DIGPAK_irq);
PT_DIGPAK_FIELD_ASSERT(dma8, PT_DIGPAK_dma8);
PT_DIGPAK_FIELD_ASSERT(dma16, PT_DIGPAK_dma16);
PT_DIGPAK_FIELD_ASSERT(playback.dispatch, PT_DIGPAK_dispatch);
PT_DIGPAK_FIELD_ASSERT(playback.playing, PT_DIGPAK_playing);
PT_DIGPAK_FIELD_ASSERT(playback.flat_pointers, PT_DIGPAK_flat_pointers);
PT_DIGPAK_FIELD_ASSERT(playback.previous_vector, PT_DIGPAK_previous_vector);
PT_DIGPAK_FIELD_ASSERT(playback.dispatch_active, PT_DIGPAK_dispatch_active);
PT_DIGPAK_FIELD_ASSERT(playback.queued, 0x797);
PT_DIGPAK_FIELD_ASSERT(playback.previous_irq, PT_DIGPAK_previous_irq);
PT_DIGPAK_FIELD_ASSERT(playback.dma_mask, PT_DIGPAK_dma_mask);
PT_DIGPAK_FIELD_ASSERT(playback.extended_dsp, PT_DIGPAK_extended_dsp);
PT_DIGPAK_FIELD_ASSERT(playback.previous_primary_mask, PT_DIGPAK_previous_pic_mask);
PT_DIGPAK_FIELD_ASSERT(install_interrupt, PT_DIGPAK_install_vector);
#undef PT_DIGPAK_FIELD_ASSERT

#endif
