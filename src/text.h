// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
#ifndef PT_TEXT_H
#define PT_TEXT_H
#include "types.h"
#include "layout.h"
#include "input.h"
#include "graphics.h"
#include "render_types.h"

// List text.

void DrawListElement(const u8 *text);
void DrawClippedListElement(const u8 *source, u16 maximum);

// Text span.

// Native byte text: DOS fonts use byte character codes, not UTF-8.

typedef struct PTTextSpan {
  const u8 *data;
  u16 count;
} PTTextSpan;
static inline PTTextSpan pt_text_span(const u8 *data, u16 count) {
  return (PTTextSpan){data, count};
}
// Animated paragraph helpers retain the original full 32-bit count.

typedef struct PTTextRange {
  const u8 *data;
  u32 count;
} PTTextRange;
static inline PTTextRange pt_text_range(const u8 *data, u32 count) {
  return (PTTextRange){data, count};
}

// Paragraph.

u16 Canvas_TextRect_Helper(Canvas *canvas, PTTextRange text);
void Canvas_TextRect_Helper2(PTTextRange text);
Canvas *Canvas_TextRect_Helper3(i16 dx, i16 dy);
u16 Canvas_TextRect(
    Canvas *canvas, u8 foreground, u8 background, u8 *text, u32 first_left, u32 subsequent_left, u32 right, u32 y);

// Text edit.

u16 ReadBitmapPixel(const Bitmap *bitmap, u16 x, u16 y);
u16 Canvas_ReadPixel(const Canvas *canvas, i16 x, i16 y);
void BackupMenuHierarchyFlag(void);
void RestoreMenuHierarchyFlag(void);
Game *HandleTextEditing(u8 *output, u8 limit);
Game *AskForPizzaPrice(const u8 *prompt, u8 *output, u8 limit);
static inline unsigned pt_text_edit_capacity(u8 limit) {
  return limit <= 100 ? limit : limit <= 200 ? limit - 100u : limit - 200u;
}

// Text.

Canvas *Canvas_Init(Canvas *canvas);
u32 pt_strlen(const u8 *text);
u16 Canvas_TextWidth_Helper(const Font *font, PTTextSpan text);
u16 Canvas_TextWidth(const Canvas *canvas, PTTextSpan text);
u16 Canvas_TextWidth0(const u8 *text);
u16 Canvas_DrawText_Helper(
    const Bitmap *target, u16 x, u16 y, u8 color, u8 background, u8 opaque, const Font *font, PTTextSpan text);
u16 Canvas_DrawText(Canvas *canvas, PTTextSpan text);
Canvas *SetTempCanvasPosition(i16 x, i16 y);
Canvas *SetTempCanvasPositionX(i16 x);
void SetFontColor(u8 color);
void SetSomeFieldInCanvas(u8 value);
void ClearCanvasField14(void);
void SetCurrentFont(const Font *font);
u16 DrawText(i16 x, i16 y, const u8 *text);
u16 DrawText2(i16 left, i16 right, i16 y, const u8 *text);
void HandleMouseOver3x3Buttons(void);

// Raster.

u16 BitmapContainsPoint(const Bitmap *bitmap, u16 x, u16 y);
u16 DrawBitmapPixel(const Bitmap *bitmap, u16 x, u16 y, u8 color);
u16 DrawBitmapLine(const Bitmap *bitmap, u16 x0, u16 y0, u16 x1, u16 y1, u8 color, u16 pattern);
void ClearWholeBitmap(const Bitmap *bitmap, u8 color);
void FillBitmapRect(const Bitmap *bitmap, u16 x, u16 y, u16 width, u16 height, u8 color);
void FillBitmapPatternRect(
    const Bitmap *bitmap, u16 x, u16 y, u16 width, u16 height, u8 color, const PTPatternRow *pattern, u8 row_exponent);
void Canvas_ClearWholeBitmap(const Canvas *canvas, u8 color);
void Canvas_ClearWholeBitmap_(const Canvas *canvas);
u8 Canvas_LineTo(Canvas *canvas, u16 x, u16 y);
u16 Canvas_DrawPixel(const Canvas *canvas, u16 x, u16 y);
void Canvas_DrawCirclePoints(const Canvas *canvas, u32 x, u32 y, u32 center_x, u32 center_y);
void Canvas_DrawCircle(const Canvas *canvas, u16 center_x, u16 center_y, u32 radius);
void Canvas_FillRect(const Canvas *canvas, u32 left, u32 top, u32 right, u32 bottom);
u8 DefaultLineTo(u16 x, u16 y);
u8 RelativeLineTo(u32 x, u32 y);
u8 TempCanvasDrawLine(i32 sx, i32 sy, i32 dx, i32 dy);
void FillRect(u16 color, u16 left, u16 top, u16 right, u16 bottom);
u8 Canvas_DrawRect(u8 color, u16 left, u16 top, u16 right, u16 bottom);

#endif
