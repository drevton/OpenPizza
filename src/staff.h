// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
#ifndef PT_STAFF_H
#define PT_STAFF_H
#include "layout.h"
#include "state_types.h"

// Staff controller.

PTActionResult HandleStaffMenu(Rest *restaurant);
u16 HandleStaffMenu_Helper2(Rest *restaurant);

// Staff render.

void DrawStaffRow(const Staff *staff, u8 row, u8 selected);
u8 RestDrawStaffSelector(Rest *restaurant, u8 role, u8 selected);
void RestDrawManagerConfig_Helper(u8 tile, i32 left, i32 top, i32 right, i32 bottom);
void RestDrawManagerConfig(Rest *restaurant);
u16 DrawStaffFileBackground(u8 face);
void ShowStaffFile(const Staff *staff, u8 face);
void DrawLetterOnBitmap(const Bitmap *source);
u32 HandleStaffMenu_Helper1(void);
void AnimateStaffFileHelper(i16 y);
void AnimateStaffFile(void);

// Staff hire.

void ShowStaffApplication(u8 face);
void HandleHireStaff(Rest *restaurant, u8 role);

// Staff courses.

void DrawCoursesList(const Staff *staff, u8 role);
u8 SendStaffOnCourse(Staff *staff, u8 role);

// Staff menu.

u16 ComputeCharacterSalary(const Character *character);
u8 HireStaff(u8 face, u8 assignment);
void RestRemoveStaff(Rest *restaurant, u8 role, u8 index);
u32 DrawStaffRowBg(i16 y, u8 style);

// Furniture menu.

typedef struct FurnitureInventoryRow {
  u8 text[36];
} FurnitureInventoryRow;
_Static_assert(sizeof(FurnitureInventoryRow) == 36, "inventory row layout");

void SetupSomeSpacingStuff(i16 left, i16 spacing);
PTActionResult FillCanvasAndSetButtons(u8 menu);
u16 DrawInventoryListHeader(void);
void DrawInventoryListContents(FurnitureInventoryRow *rows, u8 *count);
u32 ComputeFurnitureCapacityPerDay(Rest *rest, FurnitureInventoryRow *rows, u8 *count, Furniture *items, u8 category);
void DrawSampleFloor(Floor *floor, i8 x, i8 y);
void DrawFurnitureMainPage(Rest *rest, FurnitureInventoryRow *rows, u8 *count, u8 mode);
void HandleFurnitureStuff1(const u8 *text, i8 direction, u8 rows, u8 color, u8 background, i16 x, u16 y, u16 width);
void HandleBuyStuff_Helper(const FurnitureCatalogueEntry *entry);
u8 HandleBuyStuff(u8 supplier);
u8 HandleSupplierStuff_Helper2(const FloorPattern *pattern, FloorPurchase *purchase);
u8 HandleSupplierStuff_Helper(Rest *restaurant, u8 supplier);
u8 HandleSupplierStuff(Rest *restaurant, u8 category);
u8 HandleFurnitureMenu(Rest *rest);

// Staff disaster.

u8 GetManagerPolicy1(Rest *rest);
u8 AskStaffDisasterQuestion(Rest *rest, u8 face, u8 message_set);
u8 HandleStaffDisasterInner(Rest *rest, Staff *staff, u8 index);
u8 HandleStaffDisaster(Rest *rest, Staff *staff, u8 role, u8 index);
u8 HandleAllStaffDisaster(Rest *rest);

#endif
