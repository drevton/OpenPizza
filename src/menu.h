// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
#ifndef PT_MENU_H
#define PT_MENU_H
#include "layout.h"
#include "types.h"
#include "memory.h"
#include "state_types.h"

// Confirmation.

u16 DrawPushButton(const u8 *text, i16 x, i16 y, i16 pressed);
Game *AskForPizzaPriceHelper(const void *unused_canvas, i16 left, i16 top, i16 right, i16 bottom);
Game *ResetListAttrParameters(void);
u8 ShowMessage2(i16 portrait, u8 *text);
void DisplayErrorMessage(i16 portrait, u8 *text);

i16 pt_dialog_word_offset(i16 v, i32 n);
u32 pt_dialog_coordinate(i16 v);

// Character selection.

#include <stdbool.h>
void SetPlayerCharacter(u8 character_index, u8 player_index);
void DrawPlayerListElement(bool draw_name, u8 player_index);
void DrawPlayerStatsList(const Character *character);
u16 DrawPlayerStatsListEntry(const u8 *label, i32 value);
u16 DrawPlayerStatsListMoney(const u8 *label, double value);
u8 HandleSelectCharacter(void);
u8 HandleNewGame(void);

// Quickstart.

void LoadRestaurantFromFile(Rest *restaurant, const u8 *filename);
void InitQuickstartRestaurant(void);

// Mission selection.

const u8 *GetNameOfGoal(u8 goal);
void DrawMissionModeListElements(u8 mission, u8 row);
void DrawMissionGoalListElem(u32 relative_y, const u8 *label);
void DrawMissionGoalStuff(u8 player, u8 offset);
u8 HandleMissionModeMenu(void);

// Menu.

u32 CopyBitmapPredefinedRect(u8 index, u16 x, u16 y);
u32 CopySomePredefinedRect1(u16 x, u16 y, u16 pressed);
u32 CopySomePredefinedRect2(u16 x, u16 y, u16 pressed);
u32 CopySomePredefinedRect3(u16 x, u16 y, u16 pressed);
void Do3x3ButtonStuff_Helper(u8 icon, u16 x, u16 y);
void Do3x3ButtonStuff_Helper2(u8 icon, u8 kind, u16 x, u16 y, u16 pressed);

// Intro.

void ShowGameIntro(void);
u8 ShowIntroTextSet(u8 index);

// Country selection.

u8 FindMouseInsideWhichCountry(void);
u16 ShowCountryInfoBox(u8 city);
u32 DrawBoxAndWaitForClick(const u8 *text, i16 left, i16 right, i16 top);
u32 SetCurrentCity(u8 city);
u8 CountrySelector(u8 cost_mode, i8 palette);

// Number query.

void ShowAskForQuery_Helper(u8 icon, i16 x, i16 y, i16 pressed);
i32 ShowAskForNumberQuery(const u8 *prompt, i32 value, i32 increment, i32 minimum, i32 maximum);
u8 ShowMultichoiceQuestion(u8 count, u8 *first, u8 *second, u8 *third, u8 *fourth);

// Main menu.

void LoadGameBackgroundImage(void);
void SetGameByte107Flag(void);
void DrawMainMenu(u8 clear_first);
void LoadAndDrawMainMenu(u8 clear_first);
u8 HandleMainMenu(void);
u8 HandleOldGame(void);
void HandleDiskOptionsMenu(void);
void HandleOptions(void);
u16 HandleHighScores(void);
u8 DrawMenuFirstTime(u8 menu, i16 top, i16 spacing);
u16 DrawShadowText(i16 y, u8 string_id, u8 color);
u8 HandleMainMenuOfType(u8 menu, i16 top, i16 spacing);
u8 HandleMainMenuOfTypeOptional(u8 menu, i16 top, i16 spacing);
Game *ClearGameByte107Flag(void);
void FreeFont(Font *font);

// Popup menu.

enum { PT_POPUP_MENU_CAPACITY = sizeof(popup_menu_items) / sizeof(popup_menu_items[0]) };
void ExtractMapPopupMenuItems(const u8 *const items[], u8 count);
void HandleDepotWeapons_DrawShade(i16 x, i16 y, i16 width);
u8 DisplayMapPopupMenu_Helper(i16 x, i16 y, u32 unused, u8 count);
u8 DisplayMapPopupMenu(i16 x, i16 y, u8 count);

#endif
