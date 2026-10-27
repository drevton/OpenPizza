// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
#ifndef PT_FINANCE_H
#define PT_FINANCE_H
#include "layout.h"
#include "world.h"
#include "state_types.h"
#include "graphics.h"
#include "types.h"
#include "memory.h"

// Money.

// Recovered 16-byte read view only; the original record's identity and total
// size are unknown. Byte-factor functions read only through offset 0xD.

void DoubleToString(u8 *buffer, double value);
u32 DisplayStuffAfterPaymentInner(void);
void DisplayStuffAfterPayment(void);
// Original ST0 helper, scoped here to binary64 money quotients and the
// exactly representable binary32 stay-time input.

double TruncateX87Integral(double value);
// Defined x87 integer-store results, including the indefinite value outside
// the signed destination's range. Inputs have already been truncated.

static inline i32 pt_integral_i32(double value) {
  if (!(value >= -2147483648.0 && value < 2147483648.0))
    return INT32_MIN;
  return (i32)value;
}
static inline i64 pt_integral_i64(double value) {
  if (!(value >= -9223372036854775808.0 && value < 9223372036854775808.0))
    return INT64_MIN;
  return (i64)value;
}
// Pure formatting used when composing future drawing operations.

unsigned pt_money_text(u8 text[312], double value, size_t *length);

// Phone.

void SetPhoneTextRect(i16 left, i16 top, i16 right, i16 bottom, u8 flag, u8 color);
u16 LoadPhoneBitmaps(u8 background, u8 portrait);
void DrawPhonecallFaces(u8 expression);
void DrawPhonecallStuff(void);
PTActionResult Restore3x3ButtonMenu(void);
u8 HandlePhoneCall(u8 kind, u8 index, const u8 *argument, u8 reason);
void UnloadContractsBobFile(void);
void ClearPhoneTextRect(void);
void RedrawPhoneMessage(void);
u8 BlinkPhoneFace(void);
PTActionResult MaybeBlinkPhoneFace(void);
void AnimatePhoneTalk(u8 repetitions);
void AnimatePhonePleased(void);
void AnimatePhoneDispleased(void);
u8 DrawPhoneMessage(u8 *message, u8 dismiss_on_click);
u8 DrawStrbufPhoneMessage(void);
u8 DrawPhoneMessageFromLangfile(u8 index);
u8 AskPhoneYesNoQuestionFromLangfile(u8 index);
u8 AskPhoneYesNoQuestionFromBuffer(void);
void DoPhoneFaces(u8 expression);
u8 AskForNewspaperChoice(
    u8 count, u8 allow_outside, const u8 *const *choices, i16 x, i16 y, u8 color, u8 background, u8 highlight);
u8 ShowPhonecallAlternatives(u8 first, u8 second, u8 third, u8 fourth, u8 fifth, u8 sixth);
u8 HandleTownhallCall(u8 index, const u8 *argument, u8 reason);
u8 ShowDonationMessage(void);
u32 AwardPrize(u8 player, u8 kind);
u8 RollFoodQualityDice(u8 player);
u16 GetRestaurantServiceTime(const Rest *restaurant);

// Property.

u32 AllocateDepot(u8 player, Poi *poi);
void SellDepotInTown(Poi *poi);
u16 CreateDepotForPoi(u8 player, Poi *poi);
Rest *CreateRestaurantForPoi(u8 player, Poi *poi);
void SellPlayerDepot(u8 player, Poi *poi);
void ReSell(Rest *restaurant);

// Realtor.

u8 CheckEnoughRealtorsSocialSkills(void);
u8 HandleRealtorsCall(u8 index, const u8 *name, u8 reason);

// Finance.

u32 ComputeCharacterChecksum(Character *character);
u16 ValidateCharacterChecksum(u8 player);
u32 UpdateCharacterChecksum(u8 player);
void ComputeWeeklyProfit(void);
Player *TransferPlayerMoney_Helper(double amount);
u16 TransferPlayerMoney_Helper2(double amount);
u32 TransferPlayerMoney(u8 player, u8 credit, u8 debit, float amount);

double pt_finance_add(double a, double b);
double pt_finance_subtract(double a, double b);
u32 pt_finance_integral_bits(double value);
u8 pt_cash_goal_at(unsigned i);

// Bank.

u8 HandleBanksCall(u8 index, const char *name, u8 reason);
u8 HandleTransferMoney(void);
u32 PlayerPaidMoneyToSpecialAccount2(u32 amount);
i32 AskForNumericInput(i32 *output);
i32 GuestAtoi(const char *text);

// Insurance.

u8 HandleInsuranceCall(u8 index, const char *name, u8 reason);

#endif
