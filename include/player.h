// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
#ifndef PT_PLAYER_H
#define PT_PLAYER_H
#include "layout.h"
#include "state_types.h"
#include "types.h"

// Player turn.

void PreRunDay(void);
u8 RunDay(void);
u8 *RunPlayerNewWeek(u8 player_index);
void RunPlayerNewMonth(u8 player_index);
u32 UpdateCharacterProp(u8 stat, i8 amount);
void UpdateCharacterPropRandomly(u8 stat, i8 probability_property, i8 amount);
u8 ResetPeachAndBanana(void);
u8 IsOnHoliday(void);
u8 CountNumActivePlayers(void);
u8 DetermineNextInteractivePlayer(void);
u16 CharacterPropertyChance(u8 stat);

// End turn.

u8 HandleEndTurnButton(void);
u8 HandleClickOnMapMenuButtons(void);
u8 DoEndTurn(void);
u8 DoAllPlayerActions(void);
void DoAllPlayerActions_Helper(void);
void DoAiPlayerStuff(u8 player);
u8 UpdateWeaponBasePrice(void);
u8 RescalePaletteValue(u8 component, u8 scale);
void FadePalette(i8 direction);
void HandleNewDay(void);
void MaybeChangeRandomTastePreference(void);
void ComputeTastePreference(u8 age);
void RecipeComputeAllAgeRatings(void);
void RecipeComputeAgeRatings(Recipe *recipe);
u32 RecipeGetWeight(const Recipe *recipe);
void RecipeReduceByte52(Recipe *recipe, u8 reduction);
void UpdateAppleByte3B(void);
void RunLastDayOfWeekStuff(void);
void RunLastDayOfMonthStuff(void);
void RunLastDayOfYearStuff(void);

// Player settlement.

void CheckMafiaLoanDue(void);
PTForwardLink *ContractFree(Contract *contract);
void DoBankrupcy(u8 player, u8 reason);
u8 RunWeeklyThings(u8 player);
u8 DeductPriceForPoi(Poi *poi);
void DoEconomicMonthlyStuff(u8 player);
void RunMonthlyThings(u8 player);
PTCanvasResult DrawMonthlyOverviewCaptions(u8 extended);
u8 ShowMonthlyOverview(u8 player);

// Player transition.

void ShowPlayerGameCard(u8 player);
void SetCurrentCityInner(u8 town);
void SetCurrentPlayerAndCity(u8 player);
void UnusedFunc11(u8 town);

#endif
