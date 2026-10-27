// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
#ifndef PT_CHARACTER_H
#define PT_CHARACTER_H
#include "messages.h"
#include "restaurant.h"
#include "state_types.h"
#include "layout.h"

// Character loader.

void pt_character_filename(u8 index, u8 name[16]);
void DummyFunc1(const u8 *unused_name);
Game *LoadLangFile(LangFileDesc *descriptor);
void FreeLangFile(LangFileDesc *descriptor);
u8 *GetTranslatedString(u8 index);
u32 LoadBitmapFromCharacterFile(void);
u32 LoadCharacter(u8 index, u8 name_override);
u32 PaintCharacterFace(u8 index, u8 background, i16 x, i16 y);

// Animated stats.

void SetAnimatedReputationAndQuality(Rest *rest);
void HandleAnimatedStatsMenu(Rest *rest);
u8 ClearAnimatedstatsReputation(void);
void DrawAnimatedExpensesBg(void);
i32 ClampValueToGraphSize(i16 value);
void ShowAnimatedProduction(Rest *rest);
void ShowAnimatedCapacity(Rest *rest);
void ShowAnimatedQuality(Rest *rest, i8 initial);
void ShowAnimatedReputation(Rest *rest, i8 initial);
void ShowAnimatedPopularity(Rest *rest);
void ShowAnimatedTurnover(Rest *rest);
void ShowAnimatedQualityInner(u8 kind);
u8 ShowAnimatedHelper1(i8 initial);
u16 ShowAnimatedHelper2(i8 amount, i16 x, i16 y);
void ShowAnimatedHelper3(i16 x, i16 y, i16 width, u8 direction);
void ShowAnimatedHelper4(u8 style);
void ShowAnimatedHelper5(i16 x, i16 y, i16 width, u8 direction);
u8 ShowAnimatedProduction_Helper1(u8 count);
void ShowAnimatedProduction_Helper2(u8 count);
u8 ShowAnimatedProduction_Helper3(u8 count);
void ShowAnimatedProduction_Helper4(u8 item, u8 visible);
void ShowAnimatedProduction_Helper5(i16 x, i16 y);
u8 ShowAnimatedProduction_Helper6(i16 x, i16 y);
void ShowAnimatedProduction_Helper7(u8 count, const u8 *amounts);
void ShowAnimatedCapacity_Helper(i16 y, i16 width);

// Guest walking.

typedef struct PTSpriteDimensions PTSpriteDimensions;
i32 DivideBy6(i16 value);
i32 DivideBy6_(i16 value);
void DummyProc1(void);
u8 GuestOppositeDirection(u8 direction);
u8 GuestChooseLegacyDirection(Rest *rest, Pig *pig, u8 avoid_first, u8 avoid_second);
u8 GuestChooseLegacyDetour(Pig *pig);
u8 GuestAdvanceLegacyAction(Pig *pig);
u8 GuestHandleLegacyCollision(Pig *pig, u8 furniture_index);
u32 ResetFloormapNonzeroToOne(void);
void RestInitFloormap(Rest *rest);
u32 SetFloormapForFurniture(Furniture *furniture);
void ClearFloormapForFurniture(Furniture *furniture);
void GuestHelper8(u8 result[4], u8 x, u8 y);
u8 GuestHelper7(u8 x, u8 y);
u8 GuestHelper6(u8 x, u8 y, u8 target_x, u8 target_y);
u32 GuestHelper4(Pig *pig);
u16 GuestHelper5(u8 type);
u32 GuestHelper3(Rest *rest, u8 type, i16 image, u32 direction, u8 action, i16 x, i16 y, i16 target_x, i16 target_y);
void GuestHelper2(Rest *rest, u8 action);
PTActionResult GuestHelper1(void);
u8 RestGetNewGuestType(Rest *rest);
u8 RestCreateAllGuests(Rest *rest);
void RestCreateNewGuest(Rest *rest, u8 type, u8 furniture);
u8 RestDrawOnScreen_Helper1(Rest *rest);
u8 RestDrawOnScreen_Helper2(Pig *pig);
void RestDrawOnScreen_Helper3(Pig *pig);

// Character menu.

void DrawInitialCharacterUi(u8 player);
void DrawCurriculumEntry(u8 player, u8 entry, u8 target_mode);
void DrawCurriculumOnLetter(u8 target_mode);
void DrawAwardOnLetter(u8 category, u8 tier, u8 position);
u32 DrawOnCharacterLetter(u8 player, u8 mode, u8 extra);
u32 ScrollInLetter(void);
u32 ScrollOutLetter(void);
void HandleCharacterMenu(void);

// Guest dialogue.

void DrawGuestEatingFrame_Helper1(u8 portrait);
void DrawGuestEatingFrame_Helper2(void);
u16 ShowGuestEatingFrame(u8 frame);
u16 ShowGuestBlinkEyesInner(u8 frame);
void ShowGuestEatingAnimation(u8 cycles);
void ShowGuestBlinkEyes(void);
void DrawGuestEatingFrame(u8 portrait);
PTActionResult PrintGuestEating(Guest *guest);
// Only the null-restaurant path has a meaningful return (guest type / 6).

u32 HandleClickOnGuest(Rest *rest, Guest *guest);

#endif
