// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
#ifndef PT_GRAPHICS_H
#define PT_GRAPHICS_H
#include "layout.h"
#include "state_types.h"
#include "files.h"
#include "render_types.h"

// Background.

InputEvent *FillLetterBackground(u8 mode, u8 color, u8 palette);

// Events.

u32 IsDayTime(void);
u8 GetCharacterProperty(u8 property);
PTForwardLink *LinkList_getNext(const PTForwardLink *node);
Zone *FindZoneFromCoordinates(i16 x, i16 y);
Poi *FindPoiFromCoords(u8 x, u8 y);
void PhoneCallStub(const Peach *call);
void AddPeachEntry(const Rest *restaurant, u8 time, u8 kind, u8 index, u8 argument);
u8 ShowDelayedMessages(void);
void ShowDelayedMessage(u8 player, u8 restaurant, u8 minutes, const u8 *message);
void ShowManagerMessage(u8 player, u8 restaurant, u8 face, u8 minutes, u8 probability, const u8 *message);
u8 CheckPeriodicPhoneCalls(void);
void ReportToThePolice(u8 crime);
u8 CheckGoalStuff(void);
void MarkGoalReached(u8 kind);
void HandleGameEnding(void);
void ShowStuffForReachedGoal(u8 kind);
void FillRect1OfGraphicsRam(void);
void FillSomeRectInGraphicsRam(void);

// Bob loader.

typedef enum PTBobShareResultKind {
  PT_BOB_SHARE_BUFFER,
  PT_BOB_SHARE_GAME,
  PT_BOB_SHARE_INPUT_FILTER
} PTBobShareResultKind;
typedef struct PTBobShareResult {
  PTBobShareResultKind kind;
  union {
    u8 *buffer;
    Game *owner;
    u32 input_filter;
  };
} PTBobShareResult;
void UnloadLinkedBob(BobHost *bob);
PTBobShareResult ShareBobSpace(BobHost *first, BobHost *second);
u16 LoadBobFile(BobHost *bob);

// Blit.

#include <stdbool.h>
// Host-local view of six low words in the original blitter's stack arguments.
// This is not an additional guest allocation or a replacement guest layout.

typedef struct PTBlitRect {
  u16 sx, sy, dx, dy, width, height;
} PTBlitRect;
bool ClipBitmapCopy(const Bitmap *source, const Bitmap *target, PTBlitRect *rect);
u16 IntersectsMouseCursor(u16 x, u16 y, u16 width, u16 height);
u16 BitBlt320Width(const Bitmap *source, u16 sx, u16 sy, const Bitmap *target, u16 dx, u16 dy, u16 width, u16 height);
u16 MaybeGraphicsBitBlt(
    const Bitmap *source, u16 sx, u16 sy, const Bitmap *target, u16 dx, u16 dy, u16 width, u16 height);
u16 TransparentBitmapCopy(
    const Bitmap *source, u16 sx, u16 sy, const Bitmap *target, u16 dx, u16 dy, u16 width, u16 height);
u16 HideMouseCursor(void);
void RestoreMouseCursor(void);
void ClampAndWarpMousePosition(i16 x, i16 y);
void MoveMousePosition(u16 x, u16 y);
u16 HandleMouseDriverEvent(u16 mask, u16 x, u16 y);

// Presentation.

void WaitForRetrace(void);
u16 UploadPalette(u16 start, u16 count, const PTRGB *data);
void SelectDisplayBitmap(const Bitmap *bitmap);
void AssignPalettePointerField(PTPaletteSlot *slot, PalettePointer *value);
Palette *Palette_CopyFrom(PalettePointer *pointer, const PTRGB *source, i16 count);
void WaitAndSetPalette(const PTPaletteSlot *slot);
void SelectPalette(u8 index);
Game *PrepareBackBufferPalette(i8 index);
void CommitBitmapAndSetPalette(i8 index);
void SetPalColor(i16 index, u8 red, u8 green, u8 blue);
void SetPalColor2(PalettePointer *pointer, i16 index, u8 red, u8 green, u8 blue);
void SetPalColor3(i16 index, u8 red, u8 green, u8 blue);
void SetCurrentBitmap(const Bitmap *bitmap);
Bitmap *SwapBitmapPages(void);
u32 BitBlt(const Bitmap *source, u16 sx, u16 sy, const Bitmap *target, u16 dx, u16 dy, u16 width, u16 height);
void Copy320x200Bitmap(const Bitmap *source, const Bitmap *target);
void CopyBitmap2to1(void);
u32 DrawFromCurrentToOld(u16 left, u16 top, u16 right, u16 bottom);
u32 BitBltOldToCurrent(u16 left, u16 top, u16 right, u16 bottom);
u32 DrawMapFromCurrentToOld(void);
u32 DrawMapFromOldToCurrent(void);

// Sprites.

Igloo *InitIglooLink(Igloo *head, Igloo *tail, IglooLink *link);
Igloo *AddHazelnutToCanvas(Hazelnut *hazel, Canvas *canvas);
void SortCanvasIgloo(Canvas *canvas);
// The lazy Bob loader remains an explicit unsupported branch.

const BobHeader *SetupHazelWithBobSprite(Hazelnut *hazel, u8 *background, BobHost *bob, u16 index);
u16 DrawMapHelper6(Igloo *igloo, u8 steps);
u16 DrawMapHelper4(const Bitmap *bitmap);
u16 RepositionFurnitureHelper2(const Bitmap *bitmap);
void DummyFunc2(u8 column, u8 row);
void DummyFunc3(void);
u16 DrawMapHelper5(Igloo *igloo, u8 index, u8 column, u8 row, u8 direction);
u8 DrawMapHelper3(void);
u8 FindFreeMapIgloo(void);
PTActionResult HandleMapZoomLevel3_Helper1(u8 direction, u8 distance);
u32 MapIgloosBlockMovement(const Igloo *moving, const Igloo *other);
u32 CanMapIglooMove(const Igloo *igloo);
// Only AX is meaningful after the optional background blit. The last
// argument and drawing's palette argument are unused by these originals.

PTActionResult RepositionFurnitureHelper1(Hazelnut *hazel, Canvas *canvas, const PalettePointer *unused);
PTActionResult HandleMapZoomLevel3_Helper4(Igloo *igloo);
u8 Canvas_DrawAllIgloos(Canvas *canvas, const void *unused_palette);
PTActionResult HandleMapZoomLevel3_Helper3(Igloo *igloo);
void HandleMapZoomLevel3_Helper2(void);

// Graphics startup.

struct PTPaletteSlot;
Font *LoadFontFile(const LoadFontDesc *descriptor);
void *AllocateBufferOrMessage(u32 size);
u8 *LoadGraphicsFiles(void);
void InitPaletteColors(void);
Jatoba *CreateJatoba(const JatobaInitializer *initializer);
void InitializeDisplay(void);
void SetupTempPalPointer(void);
struct PTPaletteSlot *GetPalettePointerAddy(void);
void SetVideMode(void);
void ResetVideoMode(void);

#endif
