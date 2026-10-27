// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
#ifndef PT_AI_DAY_H
#define PT_AI_DAY_H
#include "state_types.h"
#include "restaurant.h"
void AiSetDefaultRecipeRating(void);
u32 AiComputeRestaurantCost(Rest *restaurant, u32 customers);
u16 AiHandleRestaurant_Helper2(u32 amount);
u32 RestMaybeAddByteA2(Rest *restaurant);
u8 AiHandleRestaurant_Helper(Rest *restaurant, i8 amount);
u8 AiHandleRestaurant(Rest *restaurant);
void HandleUndergroundAttack(Rest *target, Rest *source, u8 severe);
void NO_OP2(Rest *restaurant);
void AiRestaurantProc1(Rest *restaurant);
PTForwardLink *GetNextLinkedElem(const PTForwardLink *element);
Poi *GetRandomPoiOfKind(u8 kind);
u32 GetDistanceBetweenPoi(const Poi *a, const Poi *b);
u8 RestResetAverageRatings(Rest *restaurant, u8 rating);
Rest *AllocateNewRestaurant(u8 owner, Poi *poi);
u32 AiCreateNewRestaurant(Poi *poi, u8 owner, u8 buy);
u8 AiBuildRestaurant(void);
void AiMaybeBuildRestaurant(void);
void AiRunTownDayLoop(void);
#endif
