// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
#ifndef PT_GAME_H
#define PT_GAME_H
#include "memory.h"
#include "arithmetic.h"
#include "state_types.h"

enum { PT_GAME_START_YEAR = 1994 };

void InitTown(Town *town);
Player *InitRestaurantsInPlayer(u8 index);
void InitDepotsInPlayer(u8 index);
void InitLinkedListsInPlayer(u8 index);
void SetCurrentPlayer(u8 index);
void SetGameFlag8(void);
void InitAppleFields(void);
void InitBanana(Banana *banana);
void MarkBananaFree(Banana *banana);
Banana *CreateBanana(void);
void InitBananas(void);
void InitCarrot(void);
u8 AllocateMessageSlot(void);
u8 MarkMessageSlotFree(u8 slot);
u32 *initrandnext(void);
u32 pt_rand(void);
u32 *pt_srand(u32 seed);
u32 RandomNumber(u32 range);
u32 RandomNumber_(u32 range);
u8 RandomChance(i32 probability);
void Game_Init2(Game *target);
u16 IsValidIngredient(u8 ingredient);
void FindMajorRecipeIngredients(Recipe *recipe);
u32 CompressBackground(const u8 *source, u8 *target, u32 size);
u8 DecompressBackground(const u8 *source, u8 *target);
#endif
