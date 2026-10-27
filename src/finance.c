// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
// Finance.

#include "objects.h"
#include "arithmetic.h"
#include "game.h"
#include "messages.h"
#include "finance.h"
#include "frame.h"
#include <fenv.h>
#include <math.h>

#pragma STDC FP_CONTRACT OFF
double pt_finance_add(double a, double b) {
  volatile double result = a + b;
  return result;
}
double pt_finance_subtract(double a, double b) {
  volatile double result = a - b;
  return result;
}
u32 pt_finance_integral_bits(double value) {
  value = TruncateX87Integral(value);
  return isfinite(value) && value >= -2147483648.0 && value < 2147483648.0 ? (u32)(i32)value : UINT32_C(0x80000000);
}

u8 pt_cash_goal_at(unsigned i) {
  static const u8 kinds[6] = {52, 53, 54, 55, 124, 125};
  return kinds[i];
}
u32 ComputeCharacterChecksum(Character *character) {
  character->checksum = 0;

  u32 result = GetSumOfBytes(character, sizeof(Character));

  return result;
}
// Validate the previous checksum, then store the new one. The result uses 16 bits.

u16 ValidateCharacterChecksum(u8 player) {
  Character *character = pt_character_record_slot(player);
  u32 previous = character->checksum;

  u32 checksum = ComputeCharacterChecksum(character);

  character->checksum = checksum;
  u16 valid = checksum == previous;

  return valid;
}
u32 UpdateCharacterChecksum(u8 player) {
  Character *character = pt_character_record_slot(player);
  u32 result = ComputeCharacterChecksum(character);

  character->checksum = result;

  return result;
}

void ComputeWeeklyProfit(void) {
  u32 sum = 0;
  Player *player = player_ptr;
  unsigned town = ptr_apple->town_index;
  if (town >= sizeof(player->weekly_profit_by_town) / sizeof(player->weekly_profit_by_town[0]))
    pt_resource_bounds_failure(
        "weekly profit town", town, sizeof(player->weekly_profit_by_town) / sizeof(player->weekly_profit_by_town[0]));
  for (unsigned i = 23; i < 30; ++i)
    sum += (u32)player->profit_last_30_days[i];
  player->weekly_profit_by_town[town] = pt_signed_dword(sum);
  if (pt_signed_dword(sum) >= 500000) {
    MarkGoalReached(139);
  }
}
static void payment_display(void) {
  if (current_player_index < game_ptr->num_interactive_players) {
    DisplayStuffAfterPayment();
  }
}
static void refresh_cash_copy(void) {
  UpdateCharacterChecksum(current_player_index);

  const Character *character = character_ptr;

  player_ptr->money = character->money;
}
Player *TransferPlayerMoney_Helper(double amount) {
  Character *character = character_ptr;
  double previous = character->money;
  character->money = pt_finance_add(previous, amount);
  payment_display();
  for (unsigned i = 0; i < 6; ++i) {
    character = character_ptr;
    if (previous < cash_goal_thresholds[i] && character->money >= cash_goal_thresholds[i]) {
      MarkGoalReached(pt_cash_goal_at(i));
    }
  }
  refresh_cash_copy();
  Player *result = player_ptr;

  return result;
}
static void penalty(u16 amount) {
  Player *player = player_ptr;
  player->debt_penalty = (i16)((u16)player->debt_penalty - amount);
}
u16 TransferPlayerMoney_Helper2(double amount) {
  Character *character = character_ptr;
  if (pt_finance_subtract(character->money, amount) < 0) {
    penalty(10);
    if (!special_debt_flag) {
      return 0;
    }
  }
  character = character_ptr;
  character->money = pt_finance_subtract(character->money, amount);
  payment_display();
  character = character_ptr;
  if (character->money < 0)
    penalty(10);
  if (character->money < amount)
    penalty(5);
  refresh_cash_copy();

  return 1;
}
u32 TransferPlayerMoney(u8 player, u8 credit, u8 debit, float amount) {
  if (player >= sizeof(global_balance) / sizeof(global_balance[0]))
    pt_resource_bounds_failure("balance player", player, sizeof(global_balance) / sizeof(global_balance[0]));
  if (credit >= sizeof(global_balance[0].entries) / sizeof(global_balance[0].entries[0]))
    pt_resource_bounds_failure(
        "credit category", credit, sizeof(global_balance[0].entries) / sizeof(global_balance[0].entries[0]));
  if (debit >= sizeof(global_balance[0].entries) / sizeof(global_balance[0].entries[0]))
    pt_resource_bounds_failure(
        "debit category", debit, sizeof(global_balance[0].entries) / sizeof(global_balance[0].entries[0]));
  u32 integral = pt_finance_integral_bits(amount);
  u8 previous = current_player_index;

  SetCurrentPlayer(player);

  GlobalBalance *balance = &global_balance[player];
  u32 success = 0;
  bool paid = true;
  if (debit == 5) {
    paid = TransferPlayerMoney_Helper2(pt_signed_dword(integral)) != 0;
  }
  if (paid) {
    if (credit == 5 && debit != 6) {
      TransferPlayerMoney_Helper(pt_signed_dword(integral));
    }
    u32 value = (u32)balance->entries[credit];
    if (credit >= 6)
      value -= integral;
    else
      value = pt_finance_add(pt_signed_dword(value), amount) <= ledger_limit ? value + integral : 2000000000u;
    balance->entries[credit] = pt_signed_dword(value);
    value = (u32)balance->entries[debit];
    if (debit < 6)
      value -= integral;
    else
      value = pt_finance_add(pt_signed_dword(value), amount) <= ledger_limit ? value + integral : 2000000000u;
    balance->entries[debit] = pt_signed_dword(value);

    u32 total = SumArrayOfDword(balance->financing, 4);

    balance->total_assets = pt_signed_dword(total);
    if (balance->profit > 0)
      balance->total_assets = pt_signed_dword((u32)balance->total_assets + (u32)balance->profit);
    success = 1;
  } else if (current_player_index < game_ptr->num_interactive_players) {
    ShowMessage(15, insufficient_cash_text);
  }
  if (state_slot_021) {
    ComputeWeeklyProfit();
  }

  SetCurrentPlayer(previous);

  return success;
}

// Money.

#include "text.h"
#include "graphics.h"

// The booted stock program uses x87 CW=0x127f: 53-bit arithmetic, nearest.
// Volatile stores keep each original arithmetic rounding point and prevent
// ARM fused multiply/subtract from replacing its two x87 operations.

#pragma STDC FP_CONTRACT OFF
static double multiply53(double a, double b) {
  volatile double result = a * b;
  return result;
}
static double divide53(double a, double b) {
  volatile double result = a / b;
  return result;
}
static double subtract53(double a, double b) {
  volatile double result = a - b;
  return result;
}

// an exactly widened binary32 stay-time input. Its
// general extended-register input ABI is not a direct native command.

double TruncateX87Integral(double value) {
  return trunc(value);
}
unsigned pt_money_text(u8 text[312], double value, size_t *length) {
  if (!isfinite(value) || fegetround() != FE_TONEAREST)
    return 1;
  bool negative = value < 0;
  double remaining = negative ? -value : value;
  if (remaining >= 1.0) {
    u64 base_bits;
    memcpy(&base_bits, &decimal_scale_base, sizeof(base_bits));
    if (base_bits != UINT64_C(0x4024000000000000))
      return 1;
  }
  double reduced = remaining, divisor = 0.1;
  unsigned digits = 0;
  while (reduced >= 1.0) {
    reduced = divide53(reduced, decimal_scale_base);
    divisor = multiply53(divisor, decimal_scale_base);
    if (++digits > 309 || !isfinite(divisor))
      return 1;
  }
  size_t count = 0;
  if (digits) {
    if (negative)
      text[count++] = '-';
    while (digits--) {
      // Original __CHP temporarily selects truncation and FRNDINT;
      // its FIST stores the integral low byte, not a rounded digit.

      double integral = TruncateX87Integral(divide53(remaining, divisor));
      u32 stored = integral >= INT32_MIN && integral <= INT32_MAX ? (u32)(i32)integral : UINT32_C(0x80000000);
      remaining = subtract53(remaining, multiply53(integral, divisor));
      divisor = divide53(divisor, decimal_scale_base);
      text[count++] = (u8)(stored + '0');
    }
  } else
    text[count++] = '0';
  text[count] = 0;
  *length = count;
  return 0;
}
void DoubleToString(u8 *buffer, double value) {
  u8 text[312];
  size_t length;
  if (pt_money_text(text, value, &length))
    abort();
  memcpy(buffer, text, length + 1);
}
u32 DisplayStuffAfterPaymentInner(void) {
  return BitBlt(current_bitmap, 219, 0, old_bitmap, 219, 0, 99, 199);
}
void DisplayStuffAfterPayment(void) {
  if (!game_ptr->byte107 || !game_ptr->byte87)
    return;
  u8 number[312], formatted[320];
  size_t length;
  Canvas *canvas = canvas_1;
  canvas->color = 0;
  Canvas_FillRect(canvas, 236, 94, 306, 98);
  canvas->text_background_enabled = 1;
  Canvas_SetFont(canvas, some_font_ptr);
  canvas->color = 16;
  canvas->background_color = 0;
  if (pt_money_text(number, character_ptr->money, &length))
    abort();
  formatted[0] = '$';
  formatted[1] = ' ';
  memcpy(formatted + 2, number, length + 1);
  i16 width = (i16)(Canvas_TextWidth(canvas, pt_text_span(formatted, (u16)(length + 2))));
  canvas->position_x = (i16)(270 - width / 2);
  canvas->position_y = 98;
  DrawInfoBoxText(formatted);
  Canvas_SetFont(canvas, font_supersma);
  canvas->text_background_enabled = 0;
  DisplayStuffAfterPaymentInner();
}

// Bank.

#include "character.h"
#include "restaurant.h"
#include <stdio.h>
#pragma STDC FP_CONTRACT OFF

i32 GuestAtoi(const char *text) {
  u32 n = 0;
  for (;;) {
    if (!(watcom_character_classes[(u8)((u8)*text + 1u)] & 2))
      break;
    ++text;
  }
  u8 sign = (u8)*text;
  if (sign == '+' || sign == '-')
    ++text;
  for (;;) {
    u8 c = (u8)*text;
    if (!(watcom_character_classes[(u8)(c + 1u)] & 0x20))
      break;
    n = n * 10u + c - 48u;
    ++text;
  }
  if (sign == '-')
    n = 0u - n;

  return pt_signed_dword(n);
}
i32 AskForNumericInput(i32 *output) {
  (void)HandleTextEditing(strbuf, 8);
  i32 result;
  result = GuestAtoi((const char *)strbuf);
  *output = result;
  return result;
}
u32 PlayerPaidMoneyToSpecialAccount2(u32 amount) {
  u32 n = ramtown.food_quality_by_player[current_player_index] + amount / 1000u;
  if ((i16)n > 200)
    n = 200;
  ramtown.food_quality_by_player[current_player_index] = (u8)n;

  return current_player_index;
}
u8 HandleTransferMoney(void) {

  i32 amount, account;
  u8 i, worked;

  Mission *mission = &player_ptr->mission;

  worked = 0;
  CopyBitmap2to1();
  SetCurrentBitmap(old_bitmap);

  (void)FillLetterBackground(4, 36, 28);
  CommitBitmapAndSetPalette(37);
  SetCurrentFont(font_piznumb);
  temp_canvas->color = 0;
  (void)SetTempCanvasPosition(104, 88);
  DrawListElement((const u8 *)((const char *)bank_transfer_amount_text));
  (void)SetTempCanvasPosition(138, 88);
  (void)AskForNumericInput(&amount);

  if (amount >= 0) {
    (void)SetTempCanvasPosition(55, 104);
    DrawListElement((const u8 *)((const char *)bank_transfer_account_text));
    (void)SetTempCanvasPosition(125, 104);
    (void)AskForNumericInput(&account);

    if (account && amount) {
      if (account == ramtown.pending_transfer_account_1 &&
          (u16)TransferPlayerMoney(current_player_index, 10, 5, (float)amount)) {
        ramtown.players_paid_to_account_1 |= (u8)(UINT32_C(1) << current_player_index);
        worked = 1;
        goto done;
      }
      if (account == ramtown.pending_transfer_account_2) {
        if ((u16)TransferPlayerMoney(current_player_index, 10, 5, (float)amount)) {
          worked = 1;
          (void)PlayerPaidMoneyToSpecialAccount2((u32)amount);
        }
      } else {
        i = 0;
        while (i < 8) {
          if ((u32)account == valid_bank_account_numbers[i])
            break;
          ++i;
        }
        if (i < 8) {
          if ((u16)TransferPlayerMoney(current_player_index, 10, 5, (float)amount)) {
            TransferPlayerMoney(i, 5, 10, (float)amount);
            worked = 1;
          }
        } else if (mission->type == 4 && mission->num_accounts_in_set && mission->town_index == ptr_apple->town_index) {
          i = 0;
          while (i < 4) {
            if (mission->account_amount[i] == account &&
                (u16)TransferPlayerMoney(current_player_index, 10, 5, (float)amount)) {
              worked = 1;
              if (!mission->did_pay_to_acct[i] && (mission->amount >> 2) <= (u32)amount) {
                mission->did_pay_to_acct[i] = 1;
                mission->account_amount[i] = 0;
                --mission->num_accounts_in_set;
              }
            }
            ++i;
          }
        }
      }
    }
  }
done:

  DisplayStuffAfterPayment();
  SetCurrentBitmap(old_bitmap);
  CommitBitmapAndSetPalette(28);
  CopyBitmap2to1();
  u8 result = worked;

  return result;
}
u8 HandleBanksCall(u8 index, const char *name, u8 reason) {
  (void)reason;

  u8 base, have, greeting, reply, interest;
  base = current_town->loan_details[index][1];
  have = 0;
  greeting = 34;
  game_ptr->list_element_spacing_2 = 1;

  LoadLangFile(&langfile_bank);
  Character *character = character_ptr;
  if (character->money < 0.)
    greeting = 1;
  Loan *loan = NULL;
  for (unsigned i = 0; i < 4; ++i) {
    Loan *p = pt_player_loan(current_player_index, (u8)i);
    if (p->months_left) {
      if (p->bank_index == index && p->town_index == ptr_apple->town_index) {
        have = 1;
        loan = p;
        break;
      }
    } else if (!loan)
      loan = p;
  }
  const char *translation = (const char *)GetTranslatedString(greeting);
  snprintf((char *)strbuf, sizeof(strbuf), "%s. %s", name, translation);
  reply = DrawStrbufPhoneMessage();
  while (reply) {
    reply = ShowPhonecallAlternatives(2, 3, 4, 5, 0, 0);
    switch (reply) {
    case 0:
      goto done;
    case 1: {
      if (have) {
        DrawPhoneMessageFromLangfile(35);
        break;
      }
      if (!loan) {
        DrawPhoneMessageFromLangfile(29);
        break;
      }
      if (character->money > bank_loan_cash_limit) {
        DrawPhoneMessageFromLangfile(16);
        break;
      }
      u32 factor = 5u * character->social_status +
          100u * (character->experience + character->management + character->personality + character->age);
      if (global_balance[current_player_index].profit >= 0)
        factor += (u32)global_balance[current_player_index].profit + (u32)global_balance[current_player_index].capital;
      u32 size = (8u * base * (factor / 100u)) >> (ptr_apple->difficulty_level & 31u);
      size = RoundNumberToZeros(size, 3);
      interest = (u8)(current_town->loan_details[index][0] + ramtown.loan_interest_extra);

      u32 principal = size / 12u;
      i32 interest_year;
      interest_year = MulDiv100(pt_signed_dword(size), interest);
      i32 monthly = interest_year / 12;
      u32 total = principal + (u32)monthly;

      translation = (const char *)GetTranslatedString(6);
      snprintf((char *)strbuf,
          sizeof(strbuf),
          "%s $%d, Interest: %d%%, Period:%d months, payment per month: $%d.",
          translation,
          pt_signed_dword(size),
          interest,
          12,
          pt_signed_dword(total));

      reply = AskPhoneYesNoQuestionFromBuffer();
      if (reply == 102) {
        have = 1;
        loan->months_left = 12;
        loan->loan_size = pt_signed_dword(size);
        loan->interest_rate = interest;
        loan->monthly_payment = pt_signed_dword(principal);
        loan->interest_per_month = monthly;
        loan->total_pay_per_month = pt_signed_dword(total);
        loan->bank_index = index;
        loan->town_index = ptr_apple->town_index;

        TransferPlayerMoney(current_player_index, 5, 7, (float)size);
        DrawPhoneMessageFromLangfile(30);
        MarkGoalReached(32);
      }
      if (reply == 101)
        reply = DrawPhoneMessageFromLangfile(31);
      break;
    }
    case 2: {
      Investment *investment = NULL;
      for (unsigned i = 0; i < 4; ++i)
        if (!pt_player_investment(current_player_index, (u8)i)->investment_size) {
          investment = pt_player_investment(current_player_index, (u8)i);
          break;
        }
      if (character->money < bank_investment_minimum || isnan(character->money) || !investment) {
        reply = DrawPhoneMessageFromLangfile(32);
        break;
      }
      u32 size;
      if (character->money > bank_investment_cash_limit)
        size = 2000000000;
      else {
        double integral;
        volatile double product = character->money * bank_investment_fraction;
        integral = TruncateX87Integral(product);

        u64 bits = (u64)(i64)integral;

        size = RoundNumberToZeros((u32)bits, 3);
      }

      translation = (const char *)GetTranslatedString(8);
      snprintf((char *)strbuf,
          sizeof(strbuf),
          "%s $%d, Period:%d months, interest:%d%%p.a. OK?",
          translation,
          pt_signed_dword(size),
          2,
          8);

      reply = AskPhoneYesNoQuestionFromBuffer();
      if (reply == 102) {
        investment->investment_size = pt_signed_dword(size);
        investment->months_left = 2;
        investment->bank_index = index;
        investment->interest_rate = 8;
        investment->town_index = ptr_apple->town_index;
        TransferPlayerMoney(current_player_index, 10, 5, (float)pt_signed_dword(size));
        reply = DrawPhoneMessageFromLangfile(33);
      }
      break;
    }
    case 3: {
      u8 worked;
      worked = HandleTransferMoney();

      (void)LoadPhoneBitmaps(16, current_town->phone_number_flags[1][index] % 25);
      DrawPhoneMessageFromLangfile((u8)(37u - worked));
      break;
    }
    case 4: {
      if (!have) {
        DrawPhoneMessageFromLangfile(24);
        break;
      }
      u32 total = (u32)loan->total_pay_per_month * loan->months_left;

      if ((double)total > character->money) {
        DrawPhoneMessageFromLangfile(23);
        break;
      }
      const char *tail = (const char *)GetTranslatedString(22);

      translation = (const char *)GetTranslatedString(21);
      snprintf((char *)strbuf, sizeof(strbuf), "%s%d. %s", translation, pt_signed_dword(total), tail);

      reply = DrawStrbufPhoneMessage();
      if (reply == 102) {
        u32 amount = (u32)loan->monthly_payment * loan->months_left;

        TransferPlayerMoney(current_player_index, 7, 5, (float)amount);
        amount = (u32)loan->interest_per_month * loan->months_left;

        TransferPlayerMoney(current_player_index, 10, 5, (float)amount);
        loan->months_left = 0;
      }
      break;
    }
    default:
      break;
    }
  }
done:
  FreeLangFile(&langfile_bank);

  return 1;
}

// Insurance.

static Insurance *policy(u8 type) {
  // Novelty offers retain types 7..11 in the same five owned slots.

  unsigned slot = type >= 7 ? type - 7u : type - 1u;
  if (current_player_index >= 8)
    pt_resource_bounds_failure("insurance player", current_player_index, 8);
  if (slot >= 5)
    pt_resource_bounds_failure("insurance policy", slot, 5);
  return &ramtown.insurance[current_player_index][slot];
}
u8 HandleInsuranceCall(u8 index, const char *name, u8 reason) {
  (void)name;
  (void)reason;

  u8 type, reply;
  Character *character = character_ptr;
  Player *player = player_ptr;
  i32 premium = 0;

  game_ptr->list_element_spacing_2 = -1;

  LoadLangFile(&langfile_insurance);
  if (character->underworld_status > 500) {
    premium = 10;
  }
  if (character->underworld_status > 1500) {
    premium += 10;
  }
  if (character->underworld_status > character->social_status) {
    premium += 20;
  }
  if (character->social_status > 2000) {
    premium -= 10;
  }
  u32 monthly = 0, insured = 0;
  Insurance *insurance = NULL;
  reply = DrawPhoneMessageFromLangfile(50);
  while (reply) {
    reply = ShowPhonecallAlternatives(1, 2, 3, 39, 0, 0);
    switch (reply) {
    case 1: {
      if (current_town->insurance_details[index][1] < (i16)((u32)premium)) {
        reply = DrawPhoneMessageFromLangfile(49);
        break;
      }
      DrawPhoneMessageFromLangfile(35);
      reply = ShowPhonecallAlternatives(4, 5, 6, 7, 8, 0);
      type = reply;
      if (!type || type > 5)
        break;
      monthly = 12u * ((u32)premium + current_town->insurance_details[index][0]) * type;
      insured = (800u * current_town->insurance_details[index][1] * type) >> (ptr_apple->difficulty_level & 31u);
      const char *tail = (const char *)GetTranslatedString(30);

      const char *head = (const char *)GetTranslatedString(29);

      snprintf((char *)strbuf,
          sizeof(strbuf),
          "%s $%d %s $%d. Would you sign please?",
          head,
          (u16)monthly,
          tail,
          pt_signed_dword(insured));

      reply = DrawStrbufPhoneMessage();
      if (reply == 102)
        goto accept;
      if (reply == 101)
        goto done;
      break;
    }
    case 2: {
      DrawPhoneMessageFromLangfile(38);
      reply = ShowPhonecallAlternatives(4, 5, 6, 7, 8, 0);
      type = reply;
      if (!type || type > 5)
        break;
      insurance = policy(type);
      if (!insurance->type || insurance->insurance_company != index) {
        const char *tail = (const char *)GetTranslatedString((u8)(type + 3));
        const char *head = (const char *)GetTranslatedString(37);

        snprintf((char *)strbuf, sizeof(strbuf), "%s%s with us!", head, tail);

        reply = DrawStrbufPhoneMessage();
        break;
      }
      if (type != player->last_insurance_damage_type) {
        snprintf((char *)strbuf, sizeof(strbuf), "I am sorry, we have not been informed of any damages.");
        DrawStrbufPhoneMessage();
        break;
      }
      if (RandomChance(current_town->insurance_details[index][2])) {
        DrawPhoneMessageFromLangfile(28);
        player->last_insurance_damage_type = 0;
        goto done;
      }
      if (RandomChance(current_town->insurance_details[index][0]))
        goto payout;
      player->last_insurance_damage_type = 0;
      DrawPhoneMessageFromLangfile(14);
      if (DrawPhoneMessageFromLangfile((u8)(insurance_claim_reason_by_type[type - 1] + 14u)) != 101)
        goto farewell;
      reply = ShowPhonecallAlternatives((u8)(insurance_claim_reason_by_type[type - 1] + 39u), 21, 22, 0, 0, 0);
      if (reply == 1) {
        DrawPhoneMessageFromLangfile((u8)(insurance_claim_reason_by_type[type - 1] + 43u));
        goto farewell;
      }
      if (reply == 2) {
        if (!RandomChance(character->personality)) {
          DrawPhoneMessageFromLangfile(26);
          goto farewell;
        }
        TransferPlayerMoney(current_player_index, 10, 5, 10000.f);
        goto payout;
      }
      if (reply != 3)
        goto farewell;
      if (RandomChance(5)) {
        DrawPhoneMessageFromLangfile(24);

        TransferPlayerMoney(current_player_index, 5, 10, (float)(u32)insurance->insured_amount);
        goto done;
      }
      if (!RandomChance(50)) {
        DrawPhoneMessageFromLangfile(25);
        goto farewell;
      }
      reply = DrawPhoneMessageFromLangfile(23);
      if (reply != 102) {
        DrawPhoneMessageFromLangfile(25);
        goto farewell;
      }
      TransferPlayerMoney(current_player_index, 10, 5, 20000.f);
    payout:
      DrawPhoneMessageFromLangfile(13);
      player->last_insurance_damage_type = 0;

      TransferPlayerMoney(current_player_index, 5, 10, (float)(u32)insurance->insured_amount);
      if (RandomChance(50)) {
        insurance->monthly_sum = (i16)((u16)insurance->monthly_sum * 2u);
        const char *head = (const char *)GetTranslatedString(48);

        snprintf((char *)strbuf, sizeof(strbuf), "%s $%d.", head, (u16)insurance->monthly_sum);

        DrawStrbufPhoneMessage();
      }
      break;
    farewell:
      reply = DrawPhoneMessageFromLangfile(27);
      break;
    }
    case 3: {
      DrawPhoneMessageFromLangfile(12);
      u32 random;
      random = RandomNumber(5);
      u8 offer = (u8)(random + 1u);
      monthly = 100u * offer;
      insured = 5000u * offer;
      type = (u8)(offer + 6u);
      reply = DrawPhoneMessageFromLangfile((u8)(offer + 50u));
      if (reply == 102)
        goto accept;
      if (reply == 101)
        goto done;
      break;
    }
    case 4: {
      reply = ShowPhonecallAlternatives(4, 5, 6, 7, 8, 0);
      if (!reply || reply > 5)
        break;
      insurance = policy(reply);
      if (!insurance->type || insurance->insurance_company != index) {
        const char *tail = (const char *)GetTranslatedString((u8)(reply + 3));
        const char *head = (const char *)GetTranslatedString(10);

        snprintf((char *)strbuf, sizeof(strbuf), "%s%s with us!", head, tail);

        reply = DrawStrbufPhoneMessage();
        break;
      }
      reply = DrawPhoneMessageFromLangfile(20);
      if ((u16)TransferPlayerMoney(current_player_index, 10, 5, 2000.f))
        insurance->type = 0;
      break;
    }
    default:
      break;
    }
    continue;
  accept:
    insurance = policy(type);
    player->last_insurance_damage_type = 0;
    insurance->type = type;
    insurance->insurance_company = index;
    insurance->monthly_sum = (i16)monthly;
    insurance->insured_amount = pt_signed_dword(insured);
    AnimatePhonePleased();
    reply = DrawPhoneMessageFromLangfile(31);
  }
done:
  FreeLangFile(&langfile_insurance);

  return 1;
}

// Realtor.

#include "missions.h"
#include "player.h"
#pragma STDC FP_CONTRACT OFF

static u32 integral(double value) {
  double truncated = TruncateX87Integral(value);

  u64 n = (u64)(i64)truncated;

  return (u32)n;
}
static u32 multiply(u32 n, double factor) {
  volatile double product = (double)n * factor;
  return integral(product);
}
static u32 pay(u8 credit, u8 debit, u32 amount) {
  return TransferPlayerMoney(current_player_index, credit, debit, (float)amount);
}
u8 CheckEnoughRealtorsSocialSkills(void) {
  u8 rank = GetPlayerSocialRank();
  if (rank)
    return 1;
  DrawPhoneMessageFromLangfile(51);
  return 0;
}
u8 HandleRealtorsCall(u8 index, const u8 *name, u8 reason) {
  (void)reason;

  u8 iterations, tenant, last_bargain, haggled, reply, shady;
  Poi *selected = player_ptr->selected_property;

  iterations = 0;
  haggled = 0;
  shady = 0;
  reply = 1;
  game_ptr->list_element_spacing_2 = 1;
  LoadLangFile(&langfile_realtors);

  const Town *town = current_town;
  if (RandomChance(town->realtor_details[index].shady_probability))
    shady = 1;
  if (!selected) {
    DrawPhoneMessageFromLangfile(14);
    goto done;
  }

  Poi *poi = selected;
  if (poi->realtor_index != index) {
    DrawPhoneMessageFromLangfile(5);
    goto done;
  }
  snprintf((char *)strbuf, sizeof(strbuf), "Realtors %s. May I help you?", (const char *)name);
  if (!DrawStrbufPhoneMessage())
    goto done;
  for (;;) {
    if (!reply)
      goto done;
    reply = ShowPhonecallAlternatives(29, 30, 31, 32, 0, 0);
    switch (reply) {
    case 0:
      goto done;
    case 1: {
      u8 enough = CheckEnoughRealtorsSocialSkills();
      if (!enough)
        goto done;
      if (poi->current_owner == current_player_index) {
        DrawPhoneMessageFromLangfile(44);
        continue;
      }
      tenant = poi->current_tenant;
      u32 price = (u32)poi->purchase_price;
      if (tenant < 8) {
        u32 remaining = 2u * price - (price * poi->months_left_of_lease) / 12u;
        snprintf((char *)strbuf,
            sizeof(strbuf),
            "This store is presently being rented to %s %s. The lease has another %d months to run. You can purchase "
            "it for %d.",
            (const char *)character_ptr->first_name,
            (const char *)character_ptr->last_name,
            poi->months_left_of_lease,
            pt_signed_dword(remaining));
      }
      if (poi->for_sale_or_rent_flags != 1 && poi->for_sale_or_rent_flags != 3 && tenant != current_player_index) {
        DrawPhoneMessageFromLangfile(3);
        continue;
      }

      if ((double)price > character_ptr->money)
        goto no_money;
      if (tenant == current_player_index) {
        price = (u32)MulDiv100(pt_signed_dword(price), 70);
      } else if (shady)
        price *= 2u;
    purchase_offer:
      snprintf((char *)strbuf, sizeof(strbuf), "Do you wish to purchase this store for %d?", pt_signed_dword(price));
      reply = DrawStrbufPhoneMessage();
      if (!reply)
        goto done;
      if (reply != 101) {
        if (!(u16)pay(0, 5, price))
          goto no_money;
        AnimatePhonePleased();

        ramtown.total_expense_per_player[current_player_index] =
            pt_signed_dword((u32)ramtown.total_expense_per_player[current_player_index] + price);
        if (poi->property_type == kPropertyDepot && tenant > 7) {
          u16 created = CreateDepotForPoi(current_player_index, poi);
          if (created)
            DrawPhoneMessageFromLangfile(20);
        }
        if (poi->property_type == kPropertyShop && tenant > 7) {
          Rest *created;
          created = CreateRestaurantForPoi(current_player_index, poi);
          if (created)
            DrawPhoneMessageFromLangfile(24);
        }
        poi->current_owner = current_player_index;
        poi->purchase_price = pt_signed_dword(price);
        continue;
      }
      if (haggled) {
        DrawPhoneMessageFromLangfile(43);
        goto disconnected;
      }
      reply = ShowPhonecallAlternatives(37, 34, 36, 0, 0, 0);
    bargain_reply:
      if (reply < 2) {
        if (!reply)
          goto done;
        DrawPhoneMessageFromLangfile(40);
        continue;
      }
      if (reply == 2) {
        if (RandomChance(character_ptr->charisma)) {
          price = multiply(price, realtor_discount_factor);
          DrawPhoneMessageFromLangfile(41);
        } else {
          DrawPhoneMessageFromLangfile(42);
          price = multiply(price, realtor_increase_factor);
          poi->purchase_price = pt_signed_dword(price);
        }
        ++haggled;
        goto purchase_offer;
      }
      if (reply > 3) {
        if (reply >= 101 && reply <= 102)
          goto bargain_reply;
        continue;
      }
      if (shady) {
        AnimatePhonePleased();
        DrawPhoneMessageFromLangfile(38);
        price = (u32)poi->purchase_price;
        goto purchase_offer;
      }
      AnimatePhoneDispleased();
      DrawPhoneMessageFromLangfile(39);
      goto disconnected;
    }
    case 2: {
      if (current_player_index != poi->current_owner) {
        DrawPhoneMessageFromLangfile(16);
        continue;
      }
      u32 original_price = (u32)poi->purchase_price;

      volatile double base = (double)original_price * realtor_sell_factor;

      u32 random;
      random = RandomNumber(500);
      u32 deduction = random * 10u;

      volatile double offer = base - (double)deduction;
      u32 price = integral(offer);
      last_bargain = 0;
      while (reply != 102) {
        volatile double maximum = (double)(u32)poi->purchase_price * realtor_maximum_factor;
        if ((double)price <= maximum) {
          if (last_bargain) {
            // Original pushes the price before calling the translator.

            const u8 *translation;

            translation = GetTranslatedString(28);
            snprintf((char *)strbuf, sizeof(strbuf), "Worth %s %d.", (const char *)translation, pt_signed_dword(price));

          } else
            snprintf((char *)strbuf, sizeof(strbuf), "I would give you %d for it.", pt_signed_dword(price));
          last_bargain = 0;
          reply = DrawStrbufPhoneMessage();
          if (!reply)
            goto done;
          if (reply == 101) {
            reply = ShowPhonecallAlternatives(46, 47, 48, 49, 0, 0);
            u8 probability;
            if (reply == 4) {
              i32 value = (i32)character_ptr->personality - 30;
              i32 bounded = Clamp(value, 0, 100);
              probability = (u8)bounded;
            } else
              probability = (u8)(70u - 20u * reply);
            if (RandomChance(probability)) {
              price = (price * (reply + 10u)) / 10u;
              last_bargain = 1;
              DoPhoneFaces(0);
            } else
              price = multiply(price, realtor_discount_factor);
            ++iterations;
            if (iterations > 2) {
              AnimatePhoneDispleased();
              DrawPhoneMessageFromLangfile(50);
              goto disconnected;
            }
          }
        } else {
          reply = DrawPhoneMessageFromLangfile(21);
          if (reply != 102)
            goto done;
        }
      }
      poi->purchase_price = pt_signed_dword(price);

      pay(5, 0, price);
      poi->realtor_index = index;
      if (poi->property_type == kPropertyRestaurant)
        ReSell(poi->restaurant);
      if (poi->property_type == kPropertyDepot)
        SellPlayerDepot(current_player_index, poi);
      reply = DrawPhoneMessageFromLangfile(26);
      continue;
    }
    case 3: {
      u8 enough = CheckEnoughRealtorsSocialSkills();
      if (!enough)
        goto done;
      if (poi->for_sale_or_rent_flags != 2 && poi->for_sale_or_rent_flags != 3) {
        reply = DrawPhoneMessageFromLangfile(9);
        continue;
      }
      u32 rent = (u32)poi->rent_price;

      if ((double)rent > character_ptr->money) {
        DrawPhoneMessageFromLangfile(45);
        continue;
      }
      const u8 *translation;

      translation = GetTranslatedString(13);
      snprintf((char *)strbuf, sizeof(strbuf), "%s $%d.", (const char *)translation, pt_signed_dword(rent));

      reply = AskPhoneYesNoQuestionFromBuffer();
      if (reply != 102)
        continue;

      ramtown.total_expense_per_player[current_player_index] =
          pt_signed_dword((u32)ramtown.total_expense_per_player[current_player_index] + rent);
      if (poi->property_type == kPropertyShop) {
        curr_rest = CreateRestaurantForPoi(current_player_index, poi);
        if (curr_rest)
          reply = DrawPhoneMessageFromLangfile(23);
      }
      if (poi->property_type == kPropertyDepot) {
        u16 created = CreateDepotForPoi(current_player_index, poi);
        if (created)
          reply = DrawPhoneMessageFromLangfile(6);
      }
      DeductPriceForPoi(poi);
      continue;
    }
    case 4:
      if (poi->current_tenant != current_player_index || poi->current_owner == current_player_index) {
        reply = DrawPhoneMessageFromLangfile(10);
        continue;
      }
      reply = AskPhoneYesNoQuestionFromLangfile(12);
      if (reply != 102)
        continue;
      if (poi->property_type == kPropertyRestaurant)
        ReSell(poi->restaurant);
      if (poi->property_type == kPropertyDepot)
        SellPlayerDepot(current_player_index, poi);
      reply = DrawPhoneMessageFromLangfile(1);
      if (ptr_apple->view_state >= 128) {
        has_many_players = 1;

        return 255;
      }
      continue;
    default:
      continue;
    }
  }
no_money:
  AnimatePhoneDispleased();
  DrawPhoneMessageFromLangfile(25);
  goto done;
disconnected:
  ramtown.number_has_no_connection[0][index] = 1;
done:
  FreeLangFile(&langfile_realtors);

  return 1;
}

// Property.

#include "ai_day.h"
#include "files.h"
#include "world.h"
#include "staff.h"
#include "trade.h"

u32 AllocateDepot(u8 player, Poi *poi) {
  Depot *depot = pt_optional_city_depot(player, ptr_apple->town_index);
  poi->current_tenant = player;
  poi->current_owner = (u8)(poi->realtor_index + 8u);
  poi->for_sale_or_rent_flags = 0;
  poi->restaurant = NULL;
  if (!depot) {
    depot = AllocateBufferOrMessage(sizeof(Depot));
    if (!depot) {
      return 0;
    }

    if (ptr_apple->town_index >= sizeof(player_ptr->depots) / sizeof(*player_ptr->depots))
      pt_resource_bounds_failure(
          "player depots", ptr_apple->town_index, sizeof(player_ptr->depots) / sizeof(*player_ptr->depots));
    player_ptr->depots[ptr_apple->town_index] = depot;

    depot->refcount = 0;
  }
  for (unsigned i = 0; i < 8; ++i)
    poi->weapon_quantity[i] = 0;
  (void)ChangeCurrentPlayerSocialRating(80);

  Depot *d = depot;
  u32 size;
  size = GetSizeOfPoi(poi);
  d->total_size = pt_signed_dword((u32)d->total_size + size);
  ++d->refcount;

  return 1;
}
void SellDepotInTown(Poi *poi) {
  Depot *depot = pt_optional_city_depot(current_player_index, ptr_apple->town_index);
  poi->current_tenant = 8;
  poi->current_owner = (u8)(poi->realtor_index + 8u);
  (void)ChangeCurrentPlayerSocialRating(-70);
  u32 size;
  size = GetSizeOfPoi(poi);

  Depot *d = depot;
  d->total_size = pt_signed_dword((u32)d->total_size - size);
  --d->refcount;
  if (!d->refcount) {
    pt_buffer_release(depot);
    if (ptr_apple->town_index >= sizeof(player_ptr->depots) / sizeof(*player_ptr->depots))
      pt_resource_bounds_failure(
          "player depots", ptr_apple->town_index, sizeof(player_ptr->depots) / sizeof(*player_ptr->depots));
    player_ptr->depots[ptr_apple->town_index] = NULL;
  }
}
u16 CreateDepotForPoi(u8 player, Poi *poi) {
  u32 result;
  result = AllocateDepot(player, poi);
  if ((u16)result) {
    ChangeSocialOrUnderworldRating((u8)player, 0, 100);
    unsigned count = 0;

    Player *p = player_ptr;
    for (unsigned i = 0; i < 10; ++i)
      if (pt_player_depot(p, i))
        ++count;
    if (count == 5)
      MarkGoalReached(136);
    if (count == 10)
      MarkGoalReached(137);
    result = 1;
  }

  return (u16)result;
}
Rest *CreateRestaurantForPoi(u8 player, Poi *poi) {
  Rest *rest;

  rest = AllocateNewRestaurant(player, poi);
  if (rest && player < game_ptr->num_interactive_players)
    ChangeSocialOrUnderworldRating((u8)rest->player_index, 0, 50);
  unsigned count = 0;
  for (unsigned i = 0; i < 10; ++i)
    if (pt_player(player)->num_restaurants_in_city[i])
      ++count;
  if (count == 10)
    MarkGoalReached(51);

  return rest;
}
void SellPlayerDepot(u8 player, Poi *poi) {
  SellDepotInTown(poi);
  ChangeSocialOrUnderworldRating((u8)player, 0, -100);
}
void ReSell(Rest *restaurant) {
  u8 player = restaurant->player_index;

  u32 furniture_total = 0;
  u32 ovens = 0;
  if (player < game_ptr->num_interactive_players) {
    Rest *r = restaurant;
    for (unsigned n = 0; n < 13; ++n) {
      unsigned i = 12u - n;
      u8 valid;
      valid = ReIsValidStaffIndex(r, (u8)i);
      if (valid)
        RestRemoveStaff(r, (u8)(i / 6), (u8)(i % 6));
    }
    for (unsigned i = 0; i < r->num_furniture; ++i) {
      const Furniture *item = pt_rest_furniture_identity(r, (u8)i, false);

      u16 price;
      price = GetPriceOfFurniture(item->type);
      furniture_total += price;
    }
    for (unsigned i = 0; i < r->num_ovens; ++i) {
      const Furniture *item = pt_rest_furniture_identity(r, (u8)i, true);

      u16 price;
      price = GetPriceOfFurniture(item->type);
      ovens += price;
    }
    u32 furniture = furniture_total;
    if (furniture + ovens) {
      snprintf((char *)strbuf,
          sizeof(strbuf),
          "Articles to a total of %d. were left in your restaurant. The scrap-dealer is happy now!!",
          pt_signed_dword(furniture + ovens));
      ShowMessage(15, strbuf);

      TransferPlayerMoney(player, 10, 2, (float)furniture);

      TransferPlayerMoney(player, 10, 1, (float)ovens);
    }

    const PTForwardLink *node = player_ptr->contracts.begin.next;

    for (;;) {
      if (!node->next)
        break;

      Rest *matched;
      matched = FindRestaurantById(((const Contract *)node)->restaurant_id);
      if (matched == restaurant) {
        const PTForwardLink *next = node->next;
        ContractCancel((Contract *)node);
        node = next;
      } else
        node = node->next;
    }
  }
  FreeRestaurant(restaurant, 1);
  ChangeSocialOrUnderworldRating((u8)player, 0, -50);
}

// Phone.

#include "reports.h"
#include "calendar.h"
#include <stdlib.h>
#include "input.h"
#include "runtime_state.h"

// phone

// The phone rectangle is a packed field in Game.

void SetPhoneTextRect(i16 left, i16 top, i16 right, i16 bottom, u8 flag, u8 color) {
  game_ptr->phone_text_rect = (Rect){left, top, right, bottom};
  game_ptr->phone_text_rect_flag = flag;
  game_ptr->phone_text_rect_color = color;
}
u16 LoadPhoneBitmaps(u8 background, u8 portrait) {
  (void)LoadGameBackground(background);
  palette_in_callbackground = (u8)(background - 10u);
  // The original sprintf writes only through its NUL; retain filename tail.

  (void)pt_bob_format_filename(&bob_typ, "bob\\typ%ue.bob", (unsigned)portrait + 1u);
  last_loaded_bob_typ = portrait;
  bob_typ.num_entries = 0;
  return LoadBobFile(&bob_typ);
}
// Face five uses frame zero's width and independent portrait offsets.

void DrawPhonecallFaces(u8 expression) {
  const BobHeader *headers = pt_resource_header_at(&bob_typ, (u16)0);
  u32 x = (u32)phone_background_face_x[palette_in_callbackground] + 12u - headers[0].w / 2u;

  u32 y =
      (u32)phone_face_y[last_loaded_bob_typ] + phone_background_face_y[palette_in_callbackground] + 14u - headers[0].h;

  SetupCanvasTempIglooLink();

  (void)SetupHazelWithBobSprite(&temp_hazel, 0, &bob_typ, expression);
  if (expression == 5) {
    temp_igloo.draw_x = (i16)(x + phone_face_five_x[last_loaded_bob_typ]);
    y += phone_face_five_y[last_loaded_bob_typ];
  } else
    temp_igloo.draw_x = (i16)((u32)phone_background_face_x[palette_in_callbackground] + 12u -
        pt_resource_header_at(&bob_typ, (u16)expression)->w / 2u);
  temp_igloo.draw_y = (i16)y;
}
void DrawPhonecallStuff(void) {
  CopyBitmap2to1();
  DrawPhonecallFaces(0);

  SetCurrentBitmap(old_bitmap);

  (void)CopyBmp6AToHere(current_bitmap);
  (void)Canvas_DrawAllIgloos(temp_canvas, active_palette_pointer);
  i8 palette = (i8)((u8)(palette_in_callbackground + 22u));

  CommitBitmapAndSetPalette(palette);
}
PTActionResult Restore3x3ButtonMenu(void) {
  u8 menu = game_ptr->some_button_menu;
  return Do3x3ButtonStuff(menu);
}
u8 HandlePhoneCall(u8 kind, u8 index, const u8 *argument, u8 reason) {
  u8 previous_menu;
  previous_menu = game_ptr->in_menu_hierarchy;
  u8 background = (u8)(kind + 15u);
  if (kind == 3 && index < 2)
    background = (u8)(18u + index);
  if (kind == 4) {
    if (index < 2)
      background = 20;
    else if (index <= 7)
      background = 22;
  }

  (void)FillLetterBackground(0, 0, 7);
  SetPhoneTextRect(24, 139, 196, 189, 0, 2);
  game_ptr->list_element_spacing_2 = 0;
  game_ptr->did_choose_tv_thingy = 9;
  game_ptr->byte114 = 1;
  game_ptr->in_menu_hierarchy = 4;
  u8 portrait = current_town->phone_number_flags[kind][index] % 25;
  (void)LoadPhoneBitmaps(background, portrait);

  (void)Do3x3ButtonStuff(3);
  DrawPhonecallStuff();
  AdvanceClock(1);
  u8 result = 1;
  if (kind == 3 && index == 0) {
    result = HandleTownhallCall(index, argument, reason);
  } else if (kind == 0) {
    result = HandleRealtorsCall(index, argument, reason);
  } else if (kind == 1) {
    result = HandleBanksCall(index, (const char *)argument, reason);
  } else if (kind == 2) {
    result = HandleInsuranceCall(index, (const char *)argument, reason);
  } else if (kind == 3 && index == 1) {
    result = HandlePoliceCall(index, argument, reason);
  } else if (kind == 4 && index < 2) {
    result = HandleMafiaCall(index, argument, reason);
  } else if (kind <= 4) {
    result = HandleWeaponstoreCall(index, argument, reason);
  }
  (void)Restore3x3ButtonMenu();
  game_ptr->byte114 = 0;
  game_ptr->in_menu_hierarchy = previous_menu;
  AdvanceClock(5);

  return result;
}

// phone message

// Unloading keeps the descriptor's count until return.

void UnloadContractsBobFile(void) {
  UnloadLinkedBob(&bob_typ);
  bob_typ.num_entries = 0;
}
void ClearPhoneTextRect(void) {
  const Game *g = game_ptr;
  Rect r = g->phone_text_rect;

  FillRect(g->phone_text_rect_flag, (u16)r.left, (u16)r.top, (u16)r.right, (u16)r.bottom);
}
static void copy_phone_rect(u16 x, u16 y, u16 width, u16 height) {
  // Stock pushes eleven arguments; the last three are ignored by BitBlt.

  (void)BitBlt(old_bitmap, x, y, current_bitmap, x, y, width, height);
}
void RedrawPhoneMessage(void) {
  SetCurrentBitmap(old_bitmap);

  (void)CopyBmp6AToHere(current_bitmap);
  copy_phone_rect(12, 137, 192, 52);
  if (palette_in_callbackground < 5)
    copy_phone_rect(112, 20, 86, 68);

  (void)Canvas_DrawAllIgloos(temp_canvas, active_palette_pointer);
  // Commit after restoring both text and portrait regions.

  CommitBitmapAndSetPalette(0);
}

u8 BlinkPhoneFace(void) {
  DrawPhonecallFaces(5);
  Canvas_DrawAllIgloos(temp_canvas, active_palette_pointer);
  Delay(15);
  DrawPhonecallFaces(0);
  return Canvas_DrawAllIgloos(temp_canvas, active_palette_pointer);
}
PTActionResult MaybeBlinkPhoneFace(void) {
  if (!game_ptr->byte114)
    return pt_action_object(game_ptr);
  u8 chance;
  chance = RandomChance(4);
  if (!chance)
    return pt_action_value(chance);
  (void)BlinkPhoneFace();
  chance = RandomChance(30);
  return pt_action_value(chance ? BlinkPhoneFace() : chance);
}
static bool skip_animation(u8 *armed) {
  u16 skip;
  skip = WantSkipGameIntro();
  if (!skip)
    *armed = 1;
  skip = WantSkipGameIntro();
  return skip && *armed;
}
static void expression(u8 face) {
  DrawPhonecallFaces(face);
  RedrawPhoneMessage();
}
void AnimatePhoneTalk(u8 repetitions) {
  u8 armed = 0;
  static const u8 faces[4] = {1, 2, 1, 0}, delays[4] = {3, 4, 2, 5};
  for (unsigned i = 0; i < repetitions; ++i)
    for (unsigned j = 0; j < 4; ++j) {
      expression(faces[j]);
      if (skip_animation(&armed))
        goto done;
      Delay(delays[j]);
    }
done:;
}
void AnimatePhonePleased(void) {
  u8 armed = 0;
  static const u8 faces[3] = {3, 0, 3}, delays[3] = {30, 20, 50};
  for (unsigned i = 0; i < 3; ++i) {
    expression(faces[i]);
    if (skip_animation(&armed))
      goto done;
    Delay(delays[i]);
  }
  expression(0);
done:;
}
void AnimatePhoneDispleased(void) {
  expression(4);
  Delay(50);
  DrawPhonecallFaces(0);
  RedrawPhoneMessage();
}
u8 DrawPhoneMessage(u8 *message, u8 dismiss_on_click) {
  u8 result;

  SetCurrentFont(font_piznumb);
  ClearPhoneTextRect();
  game_ptr->paragraph_background_enabled = 0;
  Rect r = game_ptr->phone_text_rect;
  u32 left = (u32)(i32)r.left, right = (u32)(i32)r.right, y = (u32)(i32)(i16)((u16)r.top + 8u);

  (void)Canvas_TextRect(temp_canvas, 15, game_ptr->phone_text_rect_flag, message, left, left, right, y);

  CopyBitmap2to1();
  AnimatePhoneTalk(5);
  (void)ClearInputEvents();
  for (;;) {
    mouse_pressed = WaitForMouseClick();
    if (mouse_pressed == 10) {
      (void)MaybeBlinkPhoneFace();
    }
    if (mouse_pressed == PT_MENU_BUTTON_1) {
      result = 101;
      break;
    }
    if (mouse_pressed == PT_MENU_BUTTON_2) {
      result = 102;
      break;
    }
    if (mouse_pressed == PT_MENU_BUTTON_3) {
      result = 0;
      break;
    }
    if (mouse_pressed == 1) {
      bool inside;
      inside = IsMouseInside(12, game_ptr->phone_text_rect.top, 203, 189);
      if (inside && dismiss_on_click) {
        result = 100;
        break;
      }
    }
  }

  return result;
}
u8 DrawPhoneMessageFromLangfile(u8 index) {
  u8 *text;

  u8 result;

  text = GetTranslatedString(index);
  result = DrawPhoneMessage(text, 1);

  return result;
}
u8 DrawStrbufPhoneMessage(void) {
  u8 result;
  // Draw the phone message through the shared call path.

  result = DrawPhoneMessage(strbuf, 1);
  return result;
}

u8 AskPhoneYesNoQuestionFromLangfile(u8 index) {
  u8 result;
  do {
    u8 *text;

    text = GetTranslatedString(index);
    result = DrawPhoneMessage(text, 0);

  } while (result != 102 && result != 101);

  return result;
}
u8 AskPhoneYesNoQuestionFromBuffer(void) {
  u8 result;
  do {
    result = DrawPhoneMessage(strbuf, 0);
  } while (result != 102 && result != 101);
  return result;
}
// Three short gestures, each followed by the same expression redraw.

void DoPhoneFaces(u8 face) {
  DrawPhonecallFaces(face);
  temp_igloo.draw_y = (i16)((u16)temp_igloo.draw_y + 2u);
  RedrawPhoneMessage();
  Delay(5);
  expression(face);
  Delay(5);
  DrawPhonecallFaces(face);
  temp_igloo.draw_y = (i16)((u16)temp_igloo.draw_y + 2u);
  RedrawPhoneMessage();
  Delay(5);
  expression(face);
}
static void choice_background(u8 value) {
  temp_canvas->background_color = value;
}
static void choice_text(const u8 *const *table, i8 index) {
  const u8 *text = table[(u8)index];
  (void)DrawTextInsideList(text);
}
// Preserve the inner wait loop: idle and an ignored
// outside click do not recompute the selected row until another event.

u8 AskForNewspaperChoice(
    u8 count, u8 allow_outside, const u8 *const *choices, i16 x, i16 y, u8 color, u8 background, u8 highlight) {
  u8 i, previous, spacing;
  u16 max_width;

  spacing = game_ptr->did_choose_tv_thingy;
  max_width = 0;
  previous = 255;

  temp_canvas->color = (u8)color;
  choice_background(background);
  SetSomeFieldInCanvas(1);
  i = 0;
  while (i < count) {
    const Canvas *canvas = temp_canvas;
    (void)SetTempCanvasPosition(x, (i16)((u16)y + (u16)canvas->font_baseline + (u16)(i * spacing)));
    const u8 *text = choices[i];
    (void)DrawTextInsideList(text);
    u32 length;
    length = pt_strlen(text);
    u16 width;

    width = Canvas_TextWidth(temp_canvas, pt_text_span(text, (u16)length));

    if (width > max_width)
      max_width = width;
    ++i;
  }
  (void)SaveAndSetInputEventFilter(7);
  i8 selected = -1;
  for (;;) {
    u8 inside;
    inside = IsMouseInside(x, y, (i16)((u16)x + max_width), (i16)((u16)y + (u16)(spacing * count)));
    if (inside) {
      selected = (i8)((u8)(((i32)mouse_y - y) / spacing));
    } else
      selected = -1;
    if ((u8)selected != previous) {
      i8 old = (i8)previous;
      const Canvas *canvas = temp_canvas;
      if (old >= 0) {
        choice_background(background);
        (void)SetTempCanvasPosition(x, (i16)((u16)y + (u16)canvas->font_baseline + (u16)((i32)spacing * old)));
        choice_text(choices, old);
      }
      if (selected >= 0) {
        choice_background(highlight);
        (void)SetTempCanvasPosition(x, (i16)((u16)y + (u16)canvas->font_baseline + (u16)((i32)spacing * selected)));
        choice_text(choices, selected);
      }
      previous = (u8)selected;
    }
    if (game_ptr->byte114) {
      CopyBitmap2to1();
    }
    u16 event;
    for (;;) {
      event = WaitForMouseClick();
      mouse_pressed = event;
      if (event == 1) {
        if (selected != -1 || allow_outside)
          goto done;
        continue;
      }
      if (event == 10) {
        (void)MaybeBlinkPhoneFace();
        continue;
      }
      break;
    }
    if (pt_is_menu_button_event(event)) {
      selected = (i8)((u8)(mouse_pressed - 140u));
      goto done;
    }
  }
done:;
  (void)SaveAndSetInputEventFilter(3);
  SetSomeFieldInCanvas(0);

  return (u8)selected;
}
// Translate all six slots, including zero indices.

u8 ShowPhonecallAlternatives(u8 first, u8 second, u8 third, u8 fourth, u8 fifth, u8 sixth) {
  const u8 *choices[6];
  const u8 indices[6] = {first, second, third, fourth, fifth, sixth};
  u8 count = 0;
  for (unsigned i = 0; i < 6; ++i) {
    u8 *text;

    text = GetTranslatedString(indices[i]);
    choices[i] = text;
  }
  for (unsigned i = 0; i < 6; ++i) {
    const u8 *text = choices[i];

    if (text[0])
      ++count;
  }
  u8 result;
  for (;;) {
    SetCurrentFont(font_piznumb);
    ClearPhoneTextRect();
    const Game *g = game_ptr;
    result = AskForNewspaperChoice(count,
        0,
        choices,
        g->phone_text_rect.left,
        (i16)((u16)g->phone_text_rect.top + 5u),
        15,
        g->phone_text_rect_flag,
        13);
    if ((i8)result < 100) {
      ++result;
      break;
    }
    if (result == 102) {
      result = 0;
      break;
    }
  }

  return result;
}

// phone queue

void AddPeachEntry(const Rest *restaurant, u8 time, u8 kind, u8 index, u8 argument) {
  for (u8 slot = 0; slot < 4; ++slot) {
    Peach *entry = pt_player_phone_entry(slot);
    if (entry->at_what_time)
      continue;
    entry->at_what_time = time;

    entry->phone_kind = kind;

    entry->phone_index = index;

    entry->phone_arg = argument;

    if (restaurant) {
      entry->x_coord = restaurant->property->xcoord;

      // The X store can change the serialized POI link. Reload it for Y.

      entry->y_coord = restaurant->property->ycoord;
    }
    break;
  }
}

// Townhall.

#include "recipe.h"
#include <string.h>

u8 RollFoodQualityDice(u8 player) {
  u8 result;
  result = RandomChance(ramtown.food_quality_by_player[player]);
  return result != 0;
}
u16 GetRestaurantServiceTime(const Rest *restaurant) {
  u32 result;
  if (restaurant->player_index >= game_ptr->num_interactive_players) {
    result = RandomNumber(20);
    result += 8;
  } else
    result = restaurant->waiting_time;

  return (u16)result;
}
u32 AwardPrize(u8 player, u8 kind) {
  u8 previous;
  previous = current_player_index;
  u32 old = characters[player].awards, shift = (2u * kind) & 31u, count = (old >> shift) & 3u;
  if (count < 3)
    ++count;
  SetCurrentPlayer(player);
  u8 goal = (u8)(kind + 126u);
  MarkGoalReached(goal);
  SetCurrentPlayer(previous);
  // x86 masks each shift separately, including a shift of 31 plus one.

  characters[player].awards =
      pt_signed_dword((old & ~(UINT32_C(1) << shift) & ~(UINT32_C(1) << ((shift + 1u) & 31u))) | (count << shift));

  return (u32)player * sizeof(Character);
}
// sprintf's two %s arguments are passed in reverse push order. Keep the
// pending first push live while translating the second argument.

static void format_pair(const u8 *first, const u8 *second) {
  u8 text[256];
  size_t used = 0;
  for (unsigned part = 0; part < 2; ++part) {
    const u8 *p = part ? second : first;
    for (;;) {
      u8 c = *p++;
      if (!c)
        break;

      text[used++] = c;
    }
    if (!part) {
      text[used++] = ' ';
    }
  }
  text[used] = 0;
  memcpy(strbuf, text, used + 1);
}
u8 ShowDonationMessage(void) {
  u32 random;
  const u8 *second, *first;
  random = RandomNumber(5);
  second = GetTranslatedString((u8)(random + 10u));

  first = GetTranslatedString(9);
  format_pair(first, second);

  // Stock falls into DrawStrbufPhoneMessage with the same return address.

  return DrawStrbufPhoneMessage();
}
static void inspect_restaurant(bool mayor) {
  Rest *restaurant;

  const u8 *name, *translation;
  restaurant = curr_rest;

  name = GetRestaurantName(restaurant);

  translation = GetTranslatedString(4);
  format_pair(translation, name);

  (void)DrawStrbufPhoneMessage();
  u8 dice;
  dice = RollFoodQualityDice(restaurant->player_index);
  u8 changed = 0;
  if (dice && restaurant->num_stars < 5) {
    (void)ShowDonationMessage();
    changed = 1;
  } else if (restaurant->num_stars < 5 && restaurant->pizza_quality > 16u * restaurant->num_stars + 24u)
    changed = 1;
  else if (restaurant->pizza_quality < 16u * restaurant->num_stars + 8u && restaurant->num_stars)
    changed = 2;
  if (changed == 1) {
    MarkGoalReached((u8)(restaurant->num_stars + 40u));
    restaurant->num_stars = (u8)((u32)restaurant->num_stars + (u32)1u);
    (void)ChangeCurrentPlayerSocialRating(40);
  } else if (changed == 2) {
    restaurant->num_stars = (u8)((u32)restaurant->num_stars - (u32)1u);
    (void)ChangeCurrentPlayerSocialRating(-30);
  }
  if (changed == 1) {
    AnimatePhonePleased();
    (void)ChangeSocialOrUnderworldRating(restaurant->player_index, 0, 40);
    (void)DrawPhoneMessageFromLangfile(mayor ? 5 : 2);
  } else if (changed == 2) {
    AnimatePhoneDispleased();
    (void)ChangeSocialOrUnderworldRating(restaurant->player_index, 0, -30);
    (void)DrawPhoneMessageFromLangfile(mayor ? 7 : 3);
    if (mayor) {
      restaurant->restaurant_closed_for_days = 7;
      restaurant->restaurant_opened = 0;
    }
  } else
    (void)DrawPhoneMessageFromLangfile(mayor ? 6 : 1);
  (void)DrawPhoneMessageFromLangfile(8);
}
static void announce_winner(u8 player, bool congratulations) {
  const char *suffix = congratulations ? "!!! Congratulations!" : "";

  size_t suffix_size = strlen(suffix), capacity = sizeof(strbuf) - suffix_size;

  // Read only the source bytes that fit the original 256-byte target.
  // The Player owns this name span even though its fields remain unidentified.

  const u8 *text = pt_player_announcement_name(player, capacity), *end = memchr(text, 0, capacity);

  size_t size = (size_t)(end - text);
  memcpy(strbuf, text, size);
  memcpy(strbuf + size, suffix, suffix_size + 1);

  (void)DrawStrbufPhoneMessage();
}

static void announce_character(u8 player) {
  const u8 *first = characters[player].first_name, *last = characters[player].last_name;
  const u8 *first_end = memchr(first, 0, sizeof(characters[player].first_name)),
           *last_end = memchr(last, 0, sizeof(characters[player].last_name));

  size_t first_size = (size_t)(first_end - first), last_size = (size_t)(last_end - last);
  memcpy(strbuf, first, first_size);
  strbuf[first_size] = ' ';
  memcpy(strbuf + first_size + 1, last, last_size);
  strbuf[first_size + last_size + 1] = '!';
  strbuf[first_size + last_size + 2] = 0;

  (void)DrawStrbufPhoneMessage();
}

// Prove the current player's complete chain before its first score call.
// Only next==0 identifies the end element; no fields of it are scored.

static void announce_restaurant(const Rest *restaurant) {
  size_t length;
  const u8 *name;

  name = GetRestaurantName(restaurant);
  length = strlen((const char *)name);
  memcpy(strbuf, name, length);
  strbuf[length] = '!';
  strbuf[length + 1] = 0;
  (void)DrawStrbufPhoneMessage();
}
static void restaurant_awards(bool service) {
  u32 best = service ? 100 : 0;
  const Rest *winner = NULL;

  for (u32 player = 0; player < 8; ++player) {
    if ((ramtown.players_paid_to_account_1 & (1u << player)) || player >= game_ptr->num_interactive_players) {
      PTForwardLink *node = players[player].restaurants.begin.next;

      while (node->next) {
        u32 score;
        if (service) {
          u16 value;
          value = GetRestaurantServiceTime((const Rest *)node);
          score = value;
        } else {
          score = GetSumOfBytes(((Rest *)node)->furniture_rating, sizeof(((Rest *)0)->furniture_rating));
        }
        if (service ? score < best : score > best) {
          best = score;
          winner = (const Rest *)node;
        }
        node = node->next;
      }
    }
  }
  if (winner) {
    (void)DrawPhoneMessageFromLangfile(service ? 40 : 25);
    announce_restaurant(winner);
    u8 player = winner->player_index;
    (void)AwardPrize(player, service ? 4 : 0);
    (void)DrawPhoneMessageFromLangfile(service ? 41 : 23);
    if (!service)
      (void)DrawPhoneMessageFromLangfile(24);
  }
}

static void append_byte(u8 text[256], size_t *used, u8 value) {
  text[(*used)++] = value;
}
static void append_string(u8 text[256], size_t *used, const u8 *source) {
  for (;;) {
    u8 value = *source++;
    if (!value)
      return;
    append_byte(text, used, value);
  }
}
static void append_signed_number(u8 text[256], size_t *used, i32 value) {
  u32 magnitude = (u32)value;
  u8 reverse[10];
  unsigned count = 0;
  if (value < 0) {
    append_byte(text, used, '-');
    magnitude = 0u - magnitude;
  }
  do {
    reverse[count++] = (u8)('0' + magnitude % 10u);
    magnitude /= 10u;
  } while (magnitude);
  while (count)
    append_byte(text, used, reverse[--count]);
}
static void account_message(bool competition) {
  i32 account = competition ? ramtown.pending_transfer_account_1 : ramtown.pending_transfer_account_2;
  const u8 *translation;

  u8 index = competition ? 20 : 19;
  translation = GetTranslatedString(index);

  u8 text[256];
  size_t used = 0;

  append_string(text, &used, translation);
  if (!competition)
    append_byte(text, &used, ' ');
  append_signed_number(text, &used, account);
  if (competition)
    append_byte(text, &used, '.');
  text[used] = 0;
  memcpy(strbuf, text, used + 1);

  (void)DrawStrbufPhoneMessage();
}
static void mayor_date_message(void) {
  u8 day = ramtown.mayor_available_day;
  const u8 *suffix, *translation;
  suffix = GetStringThExpression(day);

  translation = GetTranslatedString(17);

  u8 text[256];
  size_t used = 0;

  u8 month = ptr_apple->month;

  append_string(text, &used, translation);
  append_byte(text, &used, ' ');
  append_string(text, &used, invoice_month_names[month - 1u]);
  append_byte(text, &used, ' ');
  append_signed_number(text, &used, day);
  append_string(text, &used, suffix);
  append_byte(text, &used, '.');
  text[used] = 0;
  memcpy(strbuf, text, used + 1);

  (void)DrawStrbufPhoneMessage();
}
static void mayor_conversation(u8 reason) {
  if (reason == 20) {
    ramtown.players_paid_to_account_1 = 0;
    u8 kind = ramtown.type_of_competition;

    (void)DrawPhoneMessageFromLangfile(townhall_competition_messages[kind]);
    account_message(true);
    return;
  }
  if (reason == 30) {
    (void)DrawPhoneMessageFromLangfile(50);
    (void)DrawPhoneMessageFromLangfile(51);
    savegame_sect_0.extra_flooring_enabled = 1;
  }
  u8 rank;
  rank = GetPlayerSocialRank();
  if (rank < 2)
    (void)DrawPhoneMessageFromLangfile(15);
  else if (ramtown.mayor_available_day < ptr_apple->day)
    (void)DrawPhoneMessageFromLangfile(16);
  else if (ramtown.mayor_available_day == ptr_apple->day) {
    (void)DrawPhoneMessageFromLangfile(18);
    account_message(false);
  } else
    mayor_date_message();
}

static void annual_pizza_award(void) {
  u8 best = 0;
  Recipe *recipes[8];
  best_score_player = 7;
  memcpy(strbuf, townhall_annual_pizza_announcement, sizeof(townhall_annual_pizza_announcement));
  (void)DrawStrbufPhoneMessage();
  for (u8 i = 0; i < 8; ++i) {
    if (i < game_ptr->num_interactive_players) {
      u8 previous = current_player_index;
      SetCurrentPlayer(i);
      BackupMenuHierarchyFlag();
      game_ptr->in_menu_hierarchy = 0;
      recipe_rank_flag = 1;
      recipe_rank_character = &characters[i];
      u8 last;
      last = GetLastStandardRecipe(player_ptr);
      Recipe *recipe;
      recipe = CreatePizza(NULL, NULL, 1, last).recipe;
      recipes[i] = recipe;
      recipe_rank_flag = 0;
      RestoreMenuHierarchyFlag();
      SetCurrentPlayer(previous);
      if (!recipe)
        continue;
    } else {
      // Clamp's maximum/minimum remain pushed during the RNG call.

      u32 random;

      random = RandomNumber(20);

      i32 score;
      score = Clamp((i32)characters[i].cooking + (i32)random - 10, 0, 100);
      curr_score = (u8)score;
    }
    if (best < curr_score) {
      best = curr_score;
      best_score_player = i;
    }
  }
  if (best_score_player < game_ptr->num_interactive_players) {
    (void)snprintf(
        (char *)strbuf, sizeof(strbuf), (const char *)townhall_annual_recipe_name_format, (u16)ptr_apple->year % 100);
    Recipe *recipe = recipes[best_score_player];
    if (!recipe)
      // Pristine stale-score NULL winner.
      abort();
    (void)pt_strcpy(recipe->name, strbuf);
  }
  (void)AwardPrize(best_score_player, 1);
}

u8 HandleTownhallCall(u8 index, const u8 *argument, u8 reason) {
  (void)index;
  (void)argument;

  u8 most_competitions, competition_winner;
  (void)LoadLangFile(&langfile_townhall);
  if (reason >= 58)
    abort();
  if (reason == 1 || reason == 2) {
    inspect_restaurant(reason == 2);
  } else if (reason == 11) {
    annual_pizza_award();
  } else if (reason == 10 || reason == 14) {
    restaurant_awards(reason == 14);
  } else if (reason >= 50) {
    (void)DrawPhoneMessageFromLangfile(28);
    announce_character((u8)(reason - 50));
    if (best_score_player < game_ptr->num_interactive_players)
      (void)DrawPhoneMessageFromLangfile(29);
  } else if (reason == 13) {
    most_competitions = 0;
    competition_winner = 255;
    (void)DrawPhoneMessageFromLangfile(36);
    (void)DrawPhoneMessageFromLangfile(37);
    for (u8 player = 0; player < 8; ++player) {
      u8 count = savegame_sect_0.num_pizza_competitions[player][ptr_apple->town_index];
      if (count > 4 && count > most_competitions) {
        most_competitions = count;
        competition_winner = player;
      }
    }
    if ((i8)competition_winner >= 0) {
      u8 player = competition_winner;
      announce_winner(player, true);
      (void)AwardPrize(player, 3);
    } else {
      AnimatePhoneDispleased();
      (void)DrawPhoneMessageFromLangfile(38);
    }
  } else if (reason == 12 || reason == 15) {
    u32 best = 0, winner = 0;

    (void)DrawPhoneMessageFromLangfile(reason == 12 ? 34 : 47);
    u32 paid = ramtown.players_paid_to_account_1;
    for (u32 player = 0; player < 8; ++player) {
      if (reason == 12 && !(paid & (1u << player)) && player < game_ptr->num_interactive_players)
        continue;
      u32 score = 100u * ramtown.food_quality_by_player[player] +
          (u32)(reason == 12 ? ramtown.total_expense_per_player[player] : savegame_sect_0.transatlantic_score[player]);
      if (score > best) {
        best = score;
        winner = player;
      }
    }
    if (best) {
      u8 player = (u8)winner;
      announce_winner(player, false);
      (void)DrawPhoneMessageFromLangfile(reason == 12 ? 32 : 44);
      (void)DrawPhoneMessageFromLangfile(reason == 12 ? 33 : 45);
      (void)AwardPrize(player, reason == 12 ? 2 : 5);
    } else {
      AnimatePhoneDispleased();
      (void)DrawPhoneMessageFromLangfile(46);
    }
  } else
    mayor_conversation(reason);
  if (reason >= 10 && reason < 20)
    ramtown.type_of_competition = 0;
  FreeLangFile(&langfile_townhall);

  return 1;
}
