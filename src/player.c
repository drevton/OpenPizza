// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
// Player turn.

#include "objects.h"
#include "game.h"
#include "player.h"
#include "graphics.h"
#include "finance.h"
#include "frame.h"
#include "render_types.h"

// Consumes the shared RNG through RandomChance.

u16 CharacterPropertyChance(u8 stat) {
  u8 probability = characters[current_player_index].prop[stat];

  u8 result = RandomChance(probability);

  return result != 0;
}
u32 UpdateCharacterProp(u8 stat, i8 amount) {
  u8 *property = &characters[current_player_index].prop[stat];

  *property = (u8)Clamp((i32)*property + amount, 0, 100);

  u32 result = UpdateCharacterChecksum(current_player_index);

  return result;
}
void UpdateCharacterPropRandomly(u8 stat, i8 source, i8 amount) {
  unsigned property = source == -100 ? 0 : source < 0 ? (unsigned)(-(int)source) : (unsigned)source;

  u8 probability = GetCharacterProperty((u8)property);

  if (source < 0)
    probability = (u8)(100u - probability);

  int direction = amount < 0 ? -1 : 1;
  int remaining = amount;
  while (remaining) {
    u8 update = RandomChance(probability);

    if (update) {
      UpdateCharacterProp(stat, (i8)direction);
    }
    remaining -= direction;
  }
}
u8 ResetPeachAndBanana(void) {
  Peach *peaches = player_ptr->peaches;
  for (unsigned i = 0; i < 4; ++i)
    peaches[i].at_what_time = 0;
  for (unsigned i = 0; i < 16; ++i)
    bananas[i].is_free = 1;
  return 16;
}
u8 IsOnHoliday(void) {
  if ((u32)game_ptr->mask_of_players & (UINT32_C(1) << (current_player_index & 31u)))
    return 0;
  return (u8)(player_ptr->days_away / 100u + 1u);
}
u8 CountNumActivePlayers(void) {
  u8 count = 0;
  for (unsigned i = 0; i < 8; ++i)
    if (players[i].days_away < 200 && i < game_ptr->num_interactive_players)
      ++count;

  return count;
}
u8 DetermineNextInteractivePlayer(void) {
  // The frame worker may execute a valid prefix before an unported daily
  // callback; direct commands prove the complete path before any write.

  u8 index = current_player_index < game_ptr->num_interactive_players ? current_player_index + 1u : 0;
  for (; index < game_ptr->num_interactive_players; ++index) {
    if (game_ptr->mask_of_players & (1u << index))
      break;
    u8 *away = &players[index].days_away;
    if (!*away || *away >= 200)
      continue;
    if (--(*away) == 100)
      *away = 0;
    if (!*away)
      game_ptr->mask_of_players = (u8)((u32)game_ptr->mask_of_players | (u32)((u8)(1u << index)));
    else {
      SetCurrentPlayerAndCity(index);

      PreRunDay();

      RunDay();
    }
  }

  return index;
}

// Character property 5 belongs to the current player, even when the argument selects another player's staff mask and
// city counters.

u8 *RunPlayerNewWeek(u8 index) {
  Player *target = pt_player(index);
  u8 keep = (u8) ~(u8)(1u << index);
  for (unsigned staff = 0; staff < 200; ++staff) {
    u8 probability = GetCharacterProperty(PT_CHARACTER_CHARISMA);

    u8 clear = RandomChance(probability);

    if (clear)
      staff_hates_player[staff] &= keep;
  }
  u8 *result = target->num_days_passed_in_town;

  memset(result, 0, sizeof(target->num_days_passed_in_town));

  return result;
}
// Stack-check-only hook, no meaningful return or argument read.

void RunPlayerNewMonth(u8 index) {
  (void)index;
}

// Player day.

#include "restaurant.h"
#include "calendar.h"
#include "state_types.h"
#include <stdio.h>

// The outer turn loop changes the selected BYTE before updating cached
// player/character pointers. Cards legally render in this transient state.

static void debts(Supplier *supplier, bool paid) {
  if (paid)
    (void)snprintf((char *)strbuf, sizeof(strbuf), "Your debts of %s have now been paid.", supplier->name);
  else
    (void)snprintf((char *)strbuf,
        sizeof(strbuf),
        "This is %s! You still owe me $%d!!! If you do not pay this amount by the end of this week, I will sue you!",
        supplier->name,
        supplier->current_account[current_player_index]);

  ShowDelayedMessage(
      current_player_index, 100, (u8)((u8)(pt_character(current_player_index)->start_of_day + 5u)), strbuf);
  if (paid)
    supplier->current_account[current_player_index] = 0;
}
// No meaningful return.

void PreRunDay(void) {
  PrintCurrentDateAndTime();
  curr_rest = NULL;
  if (!did_run_town_day_loop_1[ptr_apple->town_index]) {
    RunCityNewDay();
    did_run_town_day_loop_1[ptr_apple->town_index] = 1;
  }
  if (!ptr_apple->weekday) {
    (void)RunPlayerNewWeek(current_player_index);
  }
  if (ptr_apple->day == 1) {
    RunPlayerNewMonth(current_player_index);
  }
  UpdateCityDepotNeedValues(current_player_index);
  MarkGoalReached((u8)(ptr_apple->town_index + 100u));
  if (!players[current_player_index].index_of_next_goal) {
    u8 share = savegame_sect_0.market_share[current_player_index][ptr_apple->town_index];
    if (share >= 5)
      MarkGoalReached(37);
    if (share >= 20)
      MarkGoalReached(38);
    if (share >= 50)
      MarkGoalReached(39);
  }
  if (!players[current_player_index].index_of_next_goal)
    for (unsigned city = 0; city < 10; ++city)
      if (savegame_sect_0.market_share[current_player_index][city] >= 50)
        MarkGoalReached((u8)(city + 110u));
  if (pt_character(current_player_index)->money < 0.)
    MarkGoalReached(33);
  if (pt_player(current_player_index)->num_restaurants >= 5)
    MarkGoalReached(34);
  if (ramtown.recipe_rank[0].player_index == current_player_index && ramtown.recipe_rank[0].recipe_kind)
    MarkGoalReached(47);
  if (ptr_apple->day < 20 && ramtown.next_inspection_zone_id != 100 && RandomChance(5)) {
    for (PTForwardLink *node = current_town->poi.begin.next; node->next; node = node->next) {
      Poi *poi = (Poi *)node;
      if (poi->property_type != 9 || poi->current_tenant != current_player_index)
        continue;
      Zone *zone;
      zone = FindZoneFromCoordinates(poi->xcoord, poi->ycoord);

      if (zone->zone_id != ramtown.next_inspection_zone_id)
        continue;
      u32 last_value = 0;
      for (u8 weapon = 4; weapon < 8; ++weapon) {
        u16 *quantity = pt_depot_weapon_quantity(poi, weapon);
        last_value = (u32)(*quantity) * (u32)pt_player(current_player_index)->average_weapon_price[weapon];
        (*quantity) = 0;
      }
      // Stock overwrites the product on each iteration; only weapon 7
      // decides whether the confiscation phone notice is queued.

      if (last_value) {
        AddPeachEntry(0, 54, 3, 1, 8);
        ramtown.next_inspection_zone_id = 100;
        break;
      }
    }
  }
  int probability = 100 - 2 * characters[current_player_index].start_of_day;
  if (probability < 0)
    probability = 0;
  if (RandomChance(probability)) {
    UpdateCharacterProp(PT_CHARACTER_HEALTH, -2);
  }
  UpdateCharacterPropRandomly(PT_CHARACTER_ENERGY, -2, (i8)(-(probability / 10)));
  ptr_apple->minutesx10 = pt_character(current_player_index)->start_of_day;
  pt_character(current_player_index)->now = (u8)(pt_character(current_player_index)->energy / 2u + 90u);
  pt_character(current_player_index)->start_of_day = 54;
  (void)UpdateCharacterChecksum(current_player_index);
  for (PTForwardLink *node = current_town->suppliers.begin.next; node->next; node = node->next) {
    Supplier *supplier = (Supplier *)node;
    u32 account = (u32)supplier->current_account[current_player_index];
    if (!account)
      continue;

    u32 paid = TransferPlayerMoney(current_player_index, 8, 5, (float)account);
    debts(supplier, (u16)paid != 0);
  }
  for (PTForwardLink *node = pt_player(current_player_index)->contracts.begin.next; node->next; node = node->next) {
    RunContractsDailyLoop((Contract *)node);
  }
  for (PTForwardLink *node = pt_player(current_player_index)->restaurants.begin.next; node->next; node = node->next) {
    RestRunDailyLoop((Rest *)node);
  }
  if (pt_character(current_player_index)->money < 0.)
    ShowDelayedMessage(
        current_player_index, 100, (u8)(pt_character(current_player_index)->start_of_day), overdrawn_account_text);
  Depot *depot = pt_optional_city_depot(current_player_index, ptr_apple->town_index);
  if (depot && RandomChance(10) && RandomChance(10)) {
    u16 set;
    set = RandomNumber(4);

    depot->what_is_stolen = (u8)(set + 1u);

    u32 weight = 0;
    for (unsigned item = 0; item < num_ingredients_in_set[set]; ++item)
      weight += (u16)depot->stock[24u * set + item].stock;
    if (weight) {
      u32 value = 0;
      for (unsigned item = 0; item < num_ingredients_in_set[set]; ++item) {
        const DepotEntry *entry = &depot->stock[24u * set + item];
        u16 amount = (u16)entry->stock;
        value += (u32)amount * (u16)entry->value;
        (void)DepotExport_RemoveOld(current_player_index, ptr_apple->town_index, (u8)set, (u8)item, amount);
      }
      u32 rounded;
      rounded = RoundUpAndDivideBy100(value);

      TransferPlayerMoney(current_player_index, 10, 4, (float)rounded);
      pt_player(current_player_index)->last_insurance_damage_type = 2;
    }
  }
  curr_rest = NULL;
}

// Player run day.

#include "messages.h"
#include "menu.h"

// terminal holiday path, otherwise the current town BYTE.

u8 RunDay(void) {
  u8 died_this_week;
  u32 campaigns = 0, van_value = 0;
  died_this_week = 0;
  restaurant_with_guests = NULL;
  (player_ptr)->num_days_passed_in_town[ptr_apple->town_index] =
      (u8)((u32)((player_ptr)->num_days_passed_in_town[ptr_apple->town_index]) + (u32)1u);
  Player *history_player = pt_player(current_player_index);
  memmove(history_player->profit_last_30_days,
      history_player->profit_last_30_days + 1,
      sizeof(history_player->profit_last_30_days) - sizeof(history_player->profit_last_30_days[0]));
  savegame_sect_0.past_sales_per_day_for_players[current_player_index][ptr_apple->town_index] = 0;
  player_ptr->profit_last_30_days[29] = 0;
  for (u32 index = 0; index < 16; ++index) {
    Banana *banana = &bananas[index];
    if (banana->is_free || banana->player_index != current_player_index)
      continue;
    if (banana->byte8 == 1 && banana->restaurant == 100) {
      u8 slot = banana->message_slot;

      u8 *text = pt_delayed_message_text(slot);
      DisplayErrorMessage(banana->face, text);
    }
    MarkBananaFree(banana);
    MarkMessageSlotFree(banana->message_slot);
  }
  u8 away = IsOnHoliday();
  if (away == 1) {
    UpdateCharacterPropRandomly(PT_CHARACTER_HEALTH, -100, 4);
    UpdateCharacterPropRandomly(PT_CHARACTER_ENERGY, 2, 4);
  } else if (away == 2)
    UpdateCharacterPropRandomly(PT_CHARACTER_HEALTH, 0, -1);
  else if (away == 3) {
    return away;
  }
  CheckMafiaLoanDue();
  for (u16 i = 0; i < 164; ++i)
    if (furniture_desc[i].amount_in_van) {
      u32 amount = furniture_desc[i].amount_in_van;

      u16 price;
      price = GetPriceOfFurniture(i);
      van_value += amount * price;
      furniture_desc[i].amount_in_van = 0;
    }
  if (van_value) {
    snprintf((char *)strbuf,
        sizeof(strbuf),
        "You had furniture on your van to the value of $%d in total!",
        pt_signed_dword(van_value));
    ShowMessage(15, strbuf);

    (void)TransferPlayerMoney(current_player_index, 2, 10, (float)van_value);
  }
  for (unsigned i = 0; i < 4; ++i) {
    Campaign *campaign = &ramtown.campaigns[current_player_index][i];
    if (campaign->days) {
      campaigns += (u16)campaign->dollars;
      --campaign->days;
    } else
      campaign->reach = campaign->reach > 8 ? (u8)(campaign->reach - 8u) : 0;
  }
  special_debt_flag = 1;

  (void)TransferPlayerMoney(current_player_index, 10, 5, (float)campaigns);
  special_debt_flag = 0;
  history_player = pt_player(current_player_index);
  memmove(history_player->sales_per_day_monthly,
      history_player->sales_per_day_monthly + 1,
      sizeof(history_player->sales_per_day_monthly) - sizeof(history_player->sales_per_day_monthly[0]));

  memmove(history_player->accum_sales_per_day_monthly,
      history_player->accum_sales_per_day_monthly + 1,
      sizeof(history_player->accum_sales_per_day_monthly) - sizeof(history_player->accum_sales_per_day_monthly[0]));

  memmove(history_player->expenditure_monthly,
      history_player->expenditure_monthly + 1,
      sizeof(history_player->expenditure_monthly) - sizeof(history_player->expenditure_monthly[0]));
  for (PTForwardLink *link = player_ptr->recipes.begin.next; link->next; link = link->next) {
    Recipe *recipe = (Recipe *)link;
    memmove(recipe->rating, &recipe->rating[1], 12);
    recipe->rating[6] = 0;
  }
  for (PTForwardLink *link = player_ptr->restaurants.begin.next; link->next; link = link->next) {
    PTForwardLink *node = link;
    RunRestaurantDay((Rest *)node);
  }
  Player *player = player_ptr;
  player->profit_last_30_days[29] = pt_signed_dword((u32)player->profit_last_30_days[29] - campaigns);
  for (PTForwardLink *link = current_town->poi.begin.next; link->next; link = link->next) {
    PTForwardLink *node = link;
    const Poi *poi = (const Poi *)node;
    if (poi->property_type == 9 && poi->current_tenant == current_player_index &&
        poi->current_owner != poi->current_tenant) {
      player = player_ptr;
      player->profit_last_30_days[29] =
          pt_signed_dword((u32)player->profit_last_30_days[29] - (u32)poi->rent_price / 30u);
    }
  }
  ComputeWeeklyProfit();
  PTForwardLink *contract = player_ptr->contracts.begin.next;
  while (contract->next) {
    Contract *record = (Contract *)contract;
    if (record->term_length) {
      record->term_length = (u8)((u32)record->term_length - (u32)1u);
      contract = record->link.next;
    } else
      contract = ContractFree(record);
  }
  if (ptr_apple->weekday == 6) {
    u8 died;
    died = RunWeeklyThings(current_player_index);
    died_this_week = died;
  }

  if (ptr_apple->day == days_in_month[ptr_apple->month] && !died_this_week)
    RunMonthlyThings(current_player_index);
  u8 town = ptr_apple->town_index;
  if (!did_run_town_day_loop_2[town]) {
    RunTownDailyLoop();
    town = ptr_apple->town_index;

    did_run_town_day_loop_2[town] = 1;
  }

  return town;
}

// Player settlement.

#include "arithmetic.h"
#include "files.h"
#include "reports.h"
#include "text.h"
#include "input.h"
#include "missions.h"
#include <math.h>

// Has no meaningful return consumed by RunDay. Due WORD decrements wrap, including negative
// values. Due calls resume this loop in the worker.

void CheckMafiaLoanDue(void) {
  if (!IsOnHoliday())
    for (u8 i = 0; i < 2; ++i) {
      i16 *days = &ramtown.ram_mafia[i].loan_due_days[current_player_index];
      if (*days) {
        *days = (i16)((u16)*days - 1u);
        if (!*days) {
          (void)HandlePhoneCall(4, i, end_turn_phone_text, 4);
        }
      }
    }
}
// Caches its full successor before unlink/free and returns that pointer.

PTForwardLink *ContractFree(Contract *contract) {

  PTForwardLink *next = contract->link.next;

  (void)LinkList_Remove(&contract->link);
  --ramtown.num_contracts_per_player[contract->player_index];
  players[contract->player_index].num_contracts = (u8)((u32)(players[contract->player_index].num_contracts) - (u32)1u);
  pt_simulation_release(contract, sizeof(Contract));

  return next;
}

// The original kills the saved current player, even when the displayed bankruptcy belongs to
// a different room player.

void DoBankrupcy(u8 player, u8 reason) {
  u8 saved_player = current_player_index;
  ptr_apple->player_index = player;
  u8 goal = (u8)(reason + 87u);
  MarkGoalReached(goal);
  ShowStuffForReachedGoal(goal);

  player_ptr->index_of_next_goal = 0;
  current_player_index = saved_player;
  HandlePlayerDead(saved_player);
  (void)DoEndTurn();
}
// The stock loop enters the world ending after the first qualifying city.
// The labelled repair checks all ten cities before awarding goal 140.

// BUG: stock reason 3 awards goal 90 instead of Fall's final goal 88.
// Monthly settlement writes hint 1 unconditionally; honoring it restores
// monthly insolvency classification, not causal tracing of each expense.
// Values outside the four bankruptcy reasons retain generic-debt reason 3.
// The native rule honors the hint, matching the repaired reference.

// AL is one after bankruptcy, zero after regular weekly settlement.

u8 RunWeeklyThings(u8 player) {
  bool invoice_started;
  Player *target = pt_player(player);
  invoice_started = 0;
  target->last_insurance_damage_type = 0;

  for (PTForwardLink *node = current_town->suppliers.begin.next; node->next; node = node->next) {
    if ((((Supplier *)node))->current_account[player]) {
      DoBankrupcy(player, 4);

      return 1;
    }
  }

  if (character_ptr->money < 0.) {
    u8 reason = 3;
    if (target->bankruptcy_reason_hint >= 1 && target->bankruptcy_reason_hint <= 4)
      reason = target->bankruptcy_reason_hint;
    DoBankrupcy(player, reason);

    return 1;
  }
  unsigned town;
  for (town = 0; town < 10 && savegame_sect_0.market_share[player][town] >= 5; ++town)
    ;
  if (town == 10) {
    MarkGoalReached(140);
  }
  double money = pt_character(player)->money;
  u32 cash;
  if (money > weekly_cash_history_limit)
    cash = 2000000000u;
  else {
    double integral;
    integral = TruncateX87Integral(money);
    cash = isfinite(integral) && integral >= -2147483648. && integral < 2147483648. ? (u32)(i32)integral
                                                                                    : UINT32_C(0x80000000);
  }

  // Physical history words may occupy unidentified bytes in this Player.
  // Crossing its ownership boundary remains unproved.

  pt_player_cash_history_entry(player, ptr_apple->num_valid_daily_profits_entries)->value = pt_signed_dword(cash);
  u32 towns;
  towns = GetSumOfBytes(ptr_apple->is_town_inited, 10);
  if (towns > 1)
    for (u8 town = 0; town < 10; ++town)
      if (ptr_apple->is_town_inited[town]) {
        u32 dollars = (u32)target->weekly_profit_by_town[town];

        if (target->num_days_passed_in_town[town]) {
          if (target->num_days_passed_in_town[town] > 7)
            target->num_days_passed_in_town[town] = 7;
          u32 product = dollars * target->num_days_passed_in_town[town];
          dollars -= (u32)(pt_signed_dword(product) / 7);
        }
        if (!invoice_started) {
          if (!IsOnHoliday())
            (void)ShowInvoice(NULL, weekly_profit_caption);
          draw_list_elem_style = 0;
          invoice_started = 1;
        }
        cur_list_elem_x = 45;
        if (!IsOnHoliday()) {
          const u8 *name = town_names[town];
          DrawListElement(name);
          cur_list_elem_x = 165;
          DrawDollarsInsideList(pt_signed_dword(dollars));
        }
        special_debt_flag = 1;
        cur_list_elem_y = (i16)((u16)cur_list_elem_y + 8u);
        u8 credit = pt_signed_dword(dollars) > 0 ? 5 : 10, debit = credit == 5 ? 10 : 5;
        u32 amount = credit == 5 ? dollars : 0u - dollars;

        float payment = (float)pt_signed_dword(amount);
        (void)TransferPlayerMoney(player, credit, debit, payment);
        special_debt_flag = 0;
      }
  if (invoice_started && !IsOnHoliday()) {
    (void)WaitForMouseClick2();
  }

  return 0;
}

// Player monthly.

#include "world.h"

static void pay(u8 player, u8 credit, u8 debit, float amount, u32 *result) {
  *result = TransferPlayerMoney(player, credit, debit, amount);
}
// Unsigned QWORD conversion of rent; meaningful AL is 2 for an owner/tenant match, otherwise
// the low WORD payment result as a boolean.

u8 DeductPriceForPoi(Poi *poi) {
  u32 rent = (u32)poi->rent_price;
  u8 tenant = poi->current_tenant;
  if (tenant == poi->current_owner) {
    return 2;
  }
  if (poi->restaurant && poi->property_type != 9) {
    AddCostForRestaurant(poi->restaurant, rent, 2);
  } else {
    AddCostForPlayer(tenant, rent, 2);
  }

  u32 result;
  pay(tenant, 10, 5, (float)rent, &result);

  return (u16)result != 0;
}
static void invoice_line(const u8 *caption, u32 dollars, bool next) {
  cur_list_elem_x = 45;
  if (next)
    cur_list_elem_y = (i16)((u16)cur_list_elem_y + 8u);
  DrawListElement(caption);
  cur_list_elem_x = 165;
  DrawDollarsInsideList(pt_signed_dword(dollars));
}
// All sums and the investment IMUL wrap at 32 bits; DIV then treats that product as unsigned.

void DoEconomicMonthlyStuff(u8 player) {
  u32 repayment = 0, interest = 0, insurance = 0, investment_interest = 0, investment_return = 0, rent = 0;

  for (u8 i = 0; i < 4; ++i) {
    Loan *loan = pt_player_loan(player, i);
    if (loan->months_left) {
      --loan->months_left;
      loan->loan_size = pt_signed_dword((u32)loan->loan_size - (u32)loan->monthly_payment);
      repayment += (u32)loan->monthly_payment;
      interest += (u32)loan->interest_per_month;
    }
  }
  for (u8 i = 0; i < 4; ++i) {
    Investment *investment = pt_player_investment(player, i);
    if (!investment->investment_size)
      continue;
    if (investment->months_left) {
      investment_interest += ((u32)investment->investment_size * investment->interest_rate) / 1200u;
      --investment->months_left;

    } else {
      investment_return += (u32)investment->investment_size;
      investment->investment_size = 0;
    }
  }
  for (u8 i = 0; i < 4; ++i)
    if (ramtown.insurance[player][i].type)
      insurance += (u16)ramtown.insurance[player][i].monthly_sum;

  for (LinkedElem *link = current_town->poi.begin.next; link->next; link = link->next) {
    Poi *node = (Poi *)link;
    Poi *poi = node;
    if (poi->current_tenant != player || poi->current_owner == player)
      continue;
    rent += (u32)poi->rent_price;
    special_debt_flag = 1;
    (void)DeductPriceForPoi(poi);
    special_debt_flag = 0;
  }
  u32 total = interest + repayment + insurance + rent - investment_interest - investment_return;
  u8 holiday;
  holiday = IsOnHoliday();
  if (!holiday) {
    (void)ShowInvoice(NULL, monthly_cost_caption);
    draw_list_elem_style = 0;
    invoice_line(monthly_repayment_caption, repayment, false);
    invoice_line(monthly_interest_caption, interest, true);
    invoice_line(monthly_insurance_caption, insurance, true);
    invoice_line(monthly_investment_interest_caption, investment_interest, true);
    invoice_line(monthly_repayment_caption, investment_return, true);
    invoice_line(monthly_rent_caption, rent, true);
    invoice_line(monthly_total_caption, total, true);
    WaitForMouseClick2();
  }
  AddCostForPlayer(player, insurance + interest + repayment, 7);
  special_debt_flag = 1;
  pt_player(player)->bankruptcy_reason_hint = 1;
  u32 ignored;
  pay(player, 7, 5, (float)pt_signed_dword(repayment), &ignored);
  pay(player, 10, 5, (float)pt_signed_dword(interest), &ignored);
  pay(player, 10, 5, (float)pt_signed_dword(insurance), &ignored);
  pay(player, 5, 10, (float)pt_signed_dword(investment_interest), &ignored);
  pay(player, 5, 10, (float)pt_signed_dword(investment_return), &ignored);
  special_debt_flag = 0;
}
// Interactive players receive the real overview.

void RunMonthlyThings(u8 player) {
  DoEconomicMonthlyStuff(player);
  if (player < game_ptr->num_interactive_players && !pt_player(player)->days_away) {
    (void)Do3x3ButtonStuff(30);
    (void)ShowMonthlyOverview(player);
  }
}

// Player transition.

#include "savegame.h"
#include "character.h"

// This transition delays and returns without invoking monthly settlement.

void ShowPlayerGameCard(u8 player) {
  const u8 *name = pt_player(player)->name_text;

  FillLetterBackground(4, 39, 16);
  PaintCharacterFace(pt_player(player)->character_index, 15, 84, 65);
  SetCurrentFont(font_astandar);
  u32 length;
  length = pt_strlen(name);
  u16 width;
  width = Canvas_TextWidth(temp_canvas, pt_text_span(name, (u16)length));
  i16 x = (i16)((u16)(109 - (i16)width / 2));
  draw_list_elem_style = 0;
  temp_canvas->color = 20;
  (void)SetTempCanvasPosition(x, 52);
  DrawListElement(name);
  temp_canvas->color = 24;
  cur_list_elem_x = (i16)((u16)cur_list_elem_x + 1u);
  cur_list_elem_y = (i16)((u16)cur_list_elem_y - 1u);
  DrawListElement(name);
  temp_canvas->color = 25;
  SetCurrentFont(font_ruby);
  (void)SetTempCanvasPosition(70, 147);
  DrawNumberInsideListRight(pt_signed_dword(ptr_apple->day));
  cur_list_elem_x = 72;
  DrawListElement(player_card_dot);
  temp_canvas->color = 0;
  const u8 *month = pt_month_name_identity((u32)ptr_apple->month - 1u);
  DrawText2(45, 174, 147, month);
  SetTempCanvasPositionX(163);
  DrawNumberInsideListRight(pt_signed_dword((u16)ptr_apple->year % 100u));
  Delay(25);
}
// The compiler-check-only body ignores its real supplied argument.

void UnusedFunc11(u8 town) {
  (void)town;
}
void SetCurrentCityInner(u8 town) {
  u8 previous_player;
  previous_player = current_player_index;
  u8 previous = ptr_apple->town_index;
  ResetPeachAndBanana();
  if (previous != 255) {
    SaveCurrTownToTempFile();
  }
  ptr_apple->town_index = town;
  pt_player(current_player_index)->city_index = town;
  FreePoiContractsRestaurants();
  TownFreeSuppliersAndZones();
  if (ptr_apple->is_town_inited[town]) {
    LoadTempTownFile(town);
    UnusedFunc11(town);
  } else {
    LoadBaseTownFile(town);
    InitializeTown();
  }
  ptr_apple->is_town_inited[town] = 1;
  SetCurrentPlayer(previous_player);
}
void SetCurrentPlayerAndCity(u8 player) {
  SetCurrentPlayer(player);
  if (pt_player(player)->city_index != ptr_apple->town_index) {
    disable_init_country_progress_msg = 1;
    SetCurrentCityInner(pt_player(player)->city_index);
    disable_init_country_progress_msg = 0;
  }
  if (game_ptr->num_interactive_players > 1)
    ShowPlayerGameCard(player);
  PreRunDay();
}

// End turn.

#include "sound.h"
#include "recipe.h"
#include <stdlib.h>

u8 HandleEndTurnButton(void) {
  return DoEndTurn();
}
u8 DoEndTurn(void) {
  u8 chance = (u8)(character_ptr->now - ptr_apple->minutesx10);
  u8 update = RandomChance((i32)chance + 10);
  if (update)
    UpdateCharacterProp(PT_CHARACTER_ENERGY, 1);
  had_holiday = 1;
  ptr_apple->minutesx10 = character_ptr->now;
  (void)CheckGoalStuff();
  return 255;
}
// The high bit is trend direction; arithmetic on the remaining BYTE wraps.

u8 UpdateWeaponBasePrice(void) {
  u8 weapon_index, trend;
  u8 result = 0;
  weapon_index = 0;
  for (; weapon_index < 40; ++weapon_index) {
    u8 *price = &weapon_base_price[weapon_index];
    trend = *price & 128u;
    *price &= 127u;
    u32 change = RandomNumber(5);
    *price = (u8)(*price + (u8)change - (trend ? 0 : 5));
    u8 reverse = 0;
    if (trend) {
      if (*price > 120)
        reverse = 1;
      else
        reverse = RandomChance(4);
      if (reverse)
        trend = 0;
    } else {
      if (*price < 10)
        reverse = 1;
      else
        reverse = RandomChance(4);
      if (reverse)
        trend = 128;
    }
    result = trend;
    *price |= result;
  }

  return result;
}
u8 RescalePaletteValue(u8 component, u8 scale) {
  u8 result =
      palette_fade_algo ? (u8)(63u - (u16)((63u - component) * scale) / 15u) : (u8)((u32)component * scale / 15u);

  return result;
}
void FadePalette(i8 direction) {
  PTRGB colors[256];
  u32 stop = direction > 0 ? 16 : UINT32_MAX;
  u16 level = direction > 0 ? 0 : 15;
  u32 color_index;
  u8 green, blue;
  while (level != (u16)stop) {
    const PTRGB *palette = palette_e[game_ptr->current_palette].colors;
    u8 scale = (u8)level;
    for (color_index = 0; color_index < 256; ++color_index) {
      green = palette[color_index].green;
      blue = palette[color_index].blue;
      colors[color_index].red = RescalePaletteValue(palette[color_index].red, scale);
      colors[color_index].green = RescalePaletteValue(green, scale);
      colors[color_index].blue = RescalePaletteValue(blue, scale);
    }
    WaitForRetrace();
    UploadPalette(0, 256, colors);
    level = (u16)(level + direction);
  }
}
u32 RecipeGetWeight(const Recipe *recipe) {
  u32 sum = 0;
  for (unsigned i = 0; i < 96; ++i)
    sum += (u16)recipe->weight_of_type[i];

  return sum;
}
void RecipeReduceByte52(Recipe *recipe, u8 reduction) {
  recipe->normality = (u8)(((100 - (i32)reduction) * recipe->normality) / 100);
}
void RecipeComputeAgeRatings(Recipe *recipe) {
  u8 age_index;
  u32 weight = RecipeGetWeight(recipe);
  age_index = 0;
  for (; age_index < 6; ++age_index) {
    unsigned age = age_index;
    const PTAgeTastePreference *taste = pt_age_taste(age);
    const i16 values[] = {recipe->fat, recipe->sweet, recipe->salty, recipe->hot};
    i32 distance = 0;
    for (unsigned j = 0; j < 4; ++j) {
      i32 delta = taste->flavor[j] - values[j];
      distance += abs(delta);
    }
    i32 score = (i16)((u16)(100 - 4 * distance));
    if ((i8)recipe->id >= 14) {
      i32 appearance = 60 - (i32)recipe->appearance;
      // SAR rounds negative odd differences toward minus infinity.

      i32 penalty = appearance >= 0 ? appearance / 2 : -((-appearance + 1) / 2);
      score = (i16)((u16)(score - penalty));
    }
    if (score < 0)
      score = 0;
    recipe->normality = 100;
    if (recipe->num_unique_ingredients < 5)
      RecipeReduceByte52(recipe, (u8)(70 - 10 * recipe->num_unique_ingredients));
    if (weight < 700)
      RecipeReduceByte52(recipe, 20);
    if (weight < 400)
      RecipeReduceByte52(recipe, 50);
    i32 rating = MulDiv100(score, recipe->normality);
    recipe->age_type_rating[age] = (u8)rating;
    if ((i8)recipe->id < 14) {
      rating = MulDiv100(score, recipe->imitation);
      recipe->age_type_rating[age] = (u8)rating;
    }
  }
}
void RecipeComputeAllAgeRatings(void) {
  u8 player_index;
  player_index = 0;
  for (; player_index < game_ptr->num_interactive_players; ++player_index) {
    PTForwardLink *node = players[player_index].recipes.begin.next;
    PTForwardLink *seen[4096];
    unsigned count = 0;
    while (true) {
      if (!node->next)
        break;

      seen[count++] = node;
      RecipeComputeAgeRatings((Recipe *)node);
      node = node->next;
    }
  }
}
void ComputeTastePreference(u8 age) {
  PTAgeTastePreference *row = pt_age_taste(age);
  u32 value;
  for (unsigned i = 0; i < 4; ++i) {
    value = RandomNumber(10);
    row->flavor[i] = (i16)((u16)value);
  }
  value = RandomNumber(50);
  row->unknown_8 = (u8)(value + 50);
  value = RandomNumber(50);
  row->unknown_9 = (u8)(value + 50);
  value = RandomNumber(10);
  row->unknown_b = (u8)value;
  RecipeComputeAllAgeRatings();
}
void MaybeChangeRandomTastePreference(void) {
  u8 change = RandomChance(30);
  if (change) {
    u32 age = RandomNumber(6);
    ComputeTastePreference((u8)age);
  }
}
void UpdateAppleByte3B(void) {
  if (ptr_apple->month == 1)
    ptr_apple->num_valid_daily_profits_entries = 1;
}
void RunLastDayOfWeekStuff(void) {
  if (ptr_apple->num_valid_daily_profits_entries < 49)
    ptr_apple->num_valid_daily_profits_entries = (u8)((u32)ptr_apple->num_valid_daily_profits_entries + 1u);
}
void RunLastDayOfYearStuff(void) {
  u8 player_index;
  ptr_apple->month = 1;
  ptr_apple->year = (i16)((u16)((u16)ptr_apple->year + 1));
  player_index = 0;
  for (; player_index < 8; ++player_index) {
    unsigned p = player_index;
    ++characters[p].age;
    i32 profit = global_balance[p].profit;
    if (profit) {
      TransferPlayerMoney(p, 10, 5, (float)(profit / 10));
      profit = global_balance[p].profit;
      TransferPlayerMoney(p, 10, 6, (float)profit);
    }
  }
  ptr_apple->num_valid_daily_profits_entries = 1;
  player_index = 0;
  for (; player_index < 8; ++player_index) {
    Player *p = &players[player_index];
    p->money_as_integer[0] = p->new_money_as_integer;
  }
}
void RunLastDayOfMonthStuff(void) {
  if (game_ptr->mask_of_players)
    HandleCompetitionChallenge(0);
  if (ptr_apple->month < 12)
    ptr_apple->month = (u8)((u32)ptr_apple->month + 1u);
  else
    RunLastDayOfYearStuff();
  ptr_apple->day = 1;
  // Stock rollover leaves the final eight expense counters unchanged.

  const size_t expense_bytes = sizeof(mission_expenses) - 8u * sizeof(u32);
  memmove(&previous_month_expenses, &mission_expenses, expense_bytes);
  memmove(&previous_month_receipts, &mission_receipts, sizeof(mission_receipts));
  memset(mission_expenses, 0, expense_bytes);
  memset(mission_receipts, 0, sizeof(mission_receipts));
}
void HandleNewDay(void) {
  PrintCurrentDateAndTime();
  memset(did_run_town_day_loop_1, 0, 10);
  memset(did_run_town_day_loop_2, 0, 10);
  memset(did_run_town_day_loop_3, 0, 10);
  if (!ptr_apple->weekday)
    MaybeChangeRandomTastePreference();
  if (ptr_apple->day == 1)
    UpdateAppleByte3B();

  DoSomeClockPaletteStuff();
}
void DoAllPlayerActions_Helper(void) {
  if ((u16)ptr_apple->year == 1996 && ptr_apple->month == 3 && ptr_apple->day == 1) {
    ShowRecipeBookHint(flooring_unlock_hint);
    savegame_sect_0.extra_flooring_enabled = 1;
    FillSomeRectWithColor(0);
  }
  if (ptr_apple->month == 6 && ptr_apple->day == 30) {
    (void)HandlePhoneCall(3, 0, end_turn_phone_text, 13);
    FillSomeRectWithColor(0);
  }
  if (ptr_apple->month == 12 && ptr_apple->day == 31) {
    (void)HandlePhoneCall(3, 0, end_turn_phone_text, 11);
    FillSomeRectWithColor(0);
    (void)HandlePhoneCall(3, 0, end_turn_phone_text, (u8)(best_score_player + 50u));
  }
  (void)UpdateWeaponBasePrice();
  if (ptr_apple->num_days_with_proper_stats < 30)
    ptr_apple->num_days_with_proper_stats = (u8)((u32)ptr_apple->num_days_with_proper_stats + 1u);
  if (ptr_apple->byte39 < 7)
    ptr_apple->byte39 = (u8)((u32)ptr_apple->byte39 + 1u);
  if (ptr_apple->weekday == 6)
    RunLastDayOfWeekStuff();
  if (ptr_apple->day >= days_in_month[ptr_apple->month])
    RunLastDayOfMonthStuff();
  else
    ptr_apple->day = (u8)((u32)ptr_apple->day + 1u);
}
void DoAiPlayerStuff(u8 player) {
  u8 previous_player;
  previous_player = current_player_index;
  if (!game_ptr->byteB2) {
    SetCurrentPlayer(player);
    ShowPlayerGameCard(current_player_index);
    Player *target = player_ptr;
    if (target->gap364)
      target->gap364 = (u8)((u32)target->gap364 - 1u);
    if (target->byte24D)
      target->byte24D = (u8)((u32)target->byte24D - 1u);
    SetCurrentPlayer(previous_player);
  }
}
u8 DoAllPlayerActions(void) {
  did_terminal_stuff = 1;
  u8 count;
  if (game_ptr->num_interactive_players >= 2) {
    count = CountNumActivePlayers();
    if (count >= 2)
      has_many_players = 1;
  }
  if (!had_holiday)
    UpdateCharacterPropRandomly(PT_CHARACTER_ENERGY, 0, -1);
  had_holiday = 0;
  ptr_apple->gap3C = 0;
  for (unsigned i = 0; i < 8; ++i)
    (void)CheckPeriodicPhoneCalls();
  (void)ResetPeachAndBanana();
  if (game_ptr->sound_enabled) {
    SoundStuff3(60, 0);
  }
  RunDay();
  CheckIfAllPlayersDead();
  current_player_index = DetermineNextInteractivePlayer();
  while (current_player_index >= game_ptr->num_interactive_players) {
    for (unsigned p = game_ptr->num_interactive_players; p < 8; ++p)
      DoAiPlayerStuff((u8)p);
    DoAllPlayerActions_Helper();
    FadePalette(-1);
    ptr_apple->weekday = (u8)((u32)ptr_apple->weekday + 1u);
    if (ptr_apple->weekday > 6)
      ptr_apple->weekday = 0;
    HandleNewDay();
    current_player_index = DetermineNextInteractivePlayer();
    ShowPlayerGameCard(current_player_index);
    FadePalette(1);
    ShowMessage(15, end_turn_wait_text);
    UpdateTownMarketShare();
    PrintGenericInfoBox();
    if (current_player_index < game_ptr->num_interactive_players) {
      SetCurrentPlayerAndCity(current_player_index);

      return 1;
    }
  }
  SetCurrentPlayerAndCity(current_player_index);
  has_many_players = 1;

  return 1;
}
// Map toolbar wrapper. Its other menu children remain explicit boundaries.

u8 HandleClickOnMapMenuButtons(void) {
  u8 previous_town;
  u8 sound = GetSoundFlag();
  SoundStuff3(sound, 0);

  SaveAndSetInputEventFilter(3);
  InitGraphicsMode();
  switch (mouse_pressed) {
  case PT_MENU_BUTTON_9:
    (void)HandleEndTurnButton();
    break;
  case PT_MENU_BUTTON_1:
    previous_town = ptr_apple->town_index;
    (void)CountrySelector(1, 1);
    if (previous_town != ptr_apple->town_index) {
      did_load_map_and_bez = 1;

      return 208;
    }
    break;
  case PT_MENU_BUTTON_2:
    HandleCharacterMenu();
    break;
  case PT_MENU_BUTTON_3:
    HandleDiskOptionsMenu();
    PrintCurrentDateAndTime();
    break;
  case PT_MENU_BUTTON_4:
    HandleAdvertisingMenu();
    break;
  case PT_MENU_BUTTON_5:
    HandleMarketResearchMenu();
    break;
  case PT_MENU_BUTTON_6:
    (void)HandleNewspaperMenu();
    break;
  case PT_MENU_BUTTON_7:
    HandleGlobalStatsMenu(current_player_index);
    break;
  case PT_MENU_BUTTON_8:
    HandleTerminalMenu();
    break;
  default:
    break;
  }
  InitGraphicsMode();
  LoadMusic(7);
  AdvanceClock(1);
  Do3x3ButtonStuff(0);
  PrintGenericInfoBox();

  return 0;
}
