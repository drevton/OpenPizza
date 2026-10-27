// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
#ifndef PT_MISSIONS_H
#define PT_MISSIONS_H
#include "layout.h"
#include "memory.h"
#include "files.h"
#include "game.h"
#include "graphics.h"
#include "state_types.h"

// Active mission.

u8 HandleActiveMission(u32 ignored_argument);
u8 HandleMissionCompleted(Player *player);

// Police.

u8 HandlePoliceCall(u8 index, const u8 *argument, u8 crime);
u8 DoServeJailTime(u8 days);

// Mafia mission.

u32 MakeNewMission(u8 mafia_index, u8 player_index);
u8 *GetStringWithDate(u8 month, u8 day, u8 timeofday);
const u8 *GetNameFromExtraTownData(u8 x, u8 y);
u8 *GetMissionAttackTargetName(u8 x, u8 y, u8 direction);
u8 *GetTextOfMission(const Mission *mission);

// Advertising.

u32 DrawFlyerCosts(const u8 *label, u32 value);
u8 *GetNewspaperAdLine(u8 index, u8 slot);
Game *DrawFlyerText(Campaign *campaign);
u8 ViewOrBuyFlyer(Campaign *campaign, u8 buy);
void DrawPowerAdvertisement(Campaign *campaign);
void ViewOrBuyPoster(Campaign *campaign, u8 buy);
void DrawNewspaperAdInner(Campaign *campaign);
void ViewOrBuyNewspaperAd(Campaign *campaign, u8 buy);
u32 DrawTvAdFrame(u8 index);
u32 DrawTvAdAnim(u8 index);
Game *LoadTvAdBitmap(u8 index);
void ShowNewspaperTvNoise(u8 count);
void ShowTvAd(Campaign *campaign);
void DrawTvAdIntro(void);
void ViewOrBuyTvAd(Campaign *campaign, u8 buy);
void ViewOrBuyCampaign(Campaign *campaign, u8 category, u8 buy);
void DrawAllCampaigns(i8 selected);
void HandleAdvertisingMenu(void);

// Objectives.

// personal mission

u32 WriteMissionHistory(u8 player);
u32 ReadMissionTargets(u8 player);
u32 SaveGamePersonal(const u8 *filename, u8 player);
u32 LoadMissionTargetsFile(const u8 *filename, u8 player);

// mission completion

i32 ComputeMissionGoalPctInner(u8 day, u8 month, u8 year, u8 target_day, u8 target_month, u8 target_year);
u8 ComputeMissionGoalPercentage(u8 player, u8 slot, u8 target_slot);
u8 ComputeTotalMissionPct(u8 player);
void LoadHighscoresFile(HighScoreRecord table[10]);
void HandlePlayerDead(u8 player);
void CheckIfAllPlayersDead(void);
void ShowGameOverExitMsg(void);
void ShowRecipeBookHint(u8 *text);

// Social rating.

u16 GetRankValueFromIndex(u8 index);
u8 GetRankIndexFromValue(u16 value);
u8 GetPlayerSocialRank(void);
u8 GetUnderworldRank(void);
u32 MarkSocialOrUnderworldGoalReached(u8 player, u8 kind);
u32 ChangeSocialOrUnderworldRating(u8 player, u8 kind, i16 delta);
u32 ChangeCurrentPlayerUnderworldRating(i16 delta);
u32 ChangeCurrentPlayerSocialRating(i16 delta);

// Market research.

PTCanvasResult DrawHowMuchTextInsideList(u32 value, u32 low, u32 high);
u32 DrawMarketCompetitorsBitmap(u8 index, i16 x, i16 y);
PTCanvasResult ShowMarketAreaInfo(Zone *zone);
void ShowMarketPropertyInfo(Zone *zone);
// Original consumes incoming BL before its own loop initialization.

void ShowMarketEconomy(u8 initial_history_index);
void ShowMarketCustomerTaste(u8 numeric);
void ShowMarketTrends(void);
void ShowMarketPizzaCharts(void);
u8 DrawMarketCompetitorsEntry(i16 x, u8 height);
void DrawMarketCompetitorsInner(u32 scale);
void ShowMarketCompetitors(void);
void ShowMarketShare(void);
void HandleMarketResearchMenu(void);

// Mafia.

u8 HandleWeaponstoreCall(u8 index, const u8 *argument, u8 reason);
u8 HandleMafiaCall(u8 index, const u8 *argument, u8 reason);
Mission *ClearMission(Mission *mission);
u32 ReduceHalfLife(void);
void DiedFromFailedMission(void);
i32 GetPriceOfWeaponArticle(u8 article);

// Terminal.

u32 TransformDialledNumber(u32 number);
u32 ExtractPhoneNrFromRam(u32 encoded);
u32 ConvertDialledNumberToInt(const u8 *buffer);
void ConvertDialledNumberToString(u32 number, u8 *buffer);
u8 *CopyDialNumberToStringBuffer(const u8 *buffer, u8 last_index);
void DrawTextSlowly(const u8 *text);
u32 DiallingCopyBackgroundToCurrent(void);
void DrawWaitingForConnection(void);
u16 DrawTerminalPhonebookEntry(u8 row, const u8 *text, u32 phone);
u8 TryDialNumber(u32 number);
u8 HandleNumberDialling(u8 digit);
u8 HandleTerminalSubmenu(u8 kind);
u8 HandleTerminalMenu(void);

// Interactive attack.

// Collisions return a boundary classification or an actual furniture object.

typedef enum PTFurnitureHitKind {
  PT_FURNITURE_CLEAR,
  PT_FURNITURE_DOOR,
  PT_FURNITURE_WALL,
  PT_FURNITURE_ITEM
} PTFurnitureHitKind;
typedef struct PTFurnitureHit {
  PTFurnitureHitKind kind;
  Furniture *furniture;
} PTFurnitureHit;
void InteractiveAttackRest(Rest *restaurant, u8 is_player_rest, u8 weapon_type);
void InteractiveAttackRest_ClearScreenStuff(void);
void InteractiveAttackRest_Proc3(void);
void InteractiveAttackRest_DrawHealth(Character *character, u8 index);
void InteractiveAttackRest_LoadWeaponBitmaps(u8 weapon);
void UnloadBob7AndBob11(void);
void InteractiveAttackRest_Init(Rest *restaurant, u8 num_players, u8 weapon_type);
u16 InteractiveAttackRest_AttackChar(Character *character);
void InteractiveAttackRest_Proc16(Pig *pig);
u8 InteractiveAttackRest_Move_Helper1(Rest *restaurant, Pig *pig, u8 direction);
u32 InteractiveAttackRest_Move_Helper2(Pig *pig);
void InteractiveAttackRest_Move(Pig *pig, u8 direction);
u16 InteractiveAttackRest_Proc8(void);
u8 InteractiveAttackRest_Proc17(void);
void InteractiveAttackRest_ShowDamageText(void);
void InteractiveAttackRest_PoliceComing(void);
u8 InteractiveAttackRest_Proc9(Character *attacker, Character *victim, u8 count);
u8 InteractiveAttackRest_Plus1And3(u8 direction);
void InteractiveAttackRest_FurnAttack0(Furniture *furniture);
PTFurnitureHit InteractiveAttackRest_FindFurniture(Rest *restaurant, i16 x, i16 y);
void InteractiveAttackRest_Shoot_Helper(Pig *pig);
u8 InteractiveAttackRest_BeginShoot(Pig *pig);
void InteractiveAttackRest_EndShoot(Pig *pig);

#endif
