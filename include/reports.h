// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
#ifndef PT_REPORTS_H
#define PT_REPORTS_H
#include "layout.h"
#include "state_types.h"
#include "restaurant.h"
#include "memory.h"

// Global stats.

void ShowGlobalBalance(void);
void ShowLoansInsurancePropertyStats(u8 player, u8 category);
void HandleGlobalStatsMenu(u8 player);

// Report background.

u32 DrawBitmapStuff1(u8 index, i16 x, i16 y);
u8 DrawMarketCompetitorsBackground(i16 left, i16 top, i16 right, i16 bottom);
void DrawBackground_Helper(i16 left, i16 top, i16 right, i16 bottom, u8 grid);
u8 DrawAnimatedExpensesBgInner(void);
void DrawMarketCompetitorsCorners(void);
void DrawCustomersBackground(void);
void FillSomeRectWithColor(u8 color);
u32 DrawNotebookBackground(u8 grid);
PTCanvasResult DrawLegacyBackground(u8 mode);
Canvas *DrawLegacyGridBackground(void);

// Weekly report.

u16 ShowWeeksTakingStats(Rest *restaurant);

// Invoice.

u16 ShowInvoice_Helper(i16 x, i16 y, u8 character, u8 count);
u16 ShowInvoice(Rest *restaurant, const u8 *caption);
void DrawStaffExpenses(const Staff *staff);
Canvas *DrawDollarsInsideList(i32 amount);
PTCanvasResult DrawTextInsideList(const u8 *text);
void DrawNumberInsideListRight(i32 value);
void DrawPercentageNumberInList(i32 value);
u16 DrawCaptionShadowed(i16 x, i16 y, const u8 *text);
void DrawStringBufferAsListElement(void);

// Profit graphs.

void DrawGlobalProfitsBg(void);
u8 DrawGlobalDailyProfitsEntry(const Player *player, u8 index, i32 value, u8 scale);
void ShowGlobalCashFlow(u8 player);
void ShowGlobalDailyProfits(void);

// Daily report.

void DrawLetterPins(i16 left, i16 top, i16 right, i16 bottom, u8 grid);
u16 ShowDailyBalanceStats(Rest *restaurant);
void ShowDailyBalanceStuff2(Rest *restaurant);

// Newspaper.

Game *LoadNewspaperBackground(u8 background);
Game *LoadNewspaperArticlesFromDisk(u8 set);
void LoadNewspaperArticleFromFile(u8 set, u8 article);
void LoadNewspaperArticle(u8 article);
u8 *LineBreakString(u8 *text);
void InsertStringInTemplate(u8 *target, const u8 *pattern, const u8 *insert);
void DrawNewspaperArticle(void);
u8 HandleNewspaperMenu(void);

#endif
