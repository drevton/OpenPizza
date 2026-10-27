// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
#ifndef PT_MESSAGES_H
#define PT_MESSAGES_H
#include "layout.h"

// Message.

u8 *pt_strcpy(u8 *target, const u8 *source);
void BackupCanvasAttr(void);
void RestoreCanvasAttrs(void);
void FillGenericInfoBox(void);
Canvas *DrawInfoBoxText(const u8 *text);
void ShowMessage(u8 color, u8 *text);

// Info.

#include <stdbool.h>

const u8 *GetRestaurantName(const Rest *restaurant);
unsigned pt_restaurant_name_text(const Rest *restaurant, u8 text[37], size_t *length);
void PrintGenericInfoBox(void);
typedef struct InfoLine {
  u8 text[37];
  u16 length;
  const u8 *source;
} InfoLine;
typedef struct InfoPlan {
  InfoLine lines[12];
  unsigned count;
  bool restaurant;
} InfoPlan;

unsigned pt_info_plan(InfoPlan *plan);

#endif
