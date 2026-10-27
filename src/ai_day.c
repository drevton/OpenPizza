// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
// Ai day.

#include "objects.h"
#include "arithmetic.h"
#include "game.h"
#include "ai_day.h"
#include "finance.h"
#include "frame.h"
#include "graphics.h"
#include "messages.h"
#include "world.h"
#include "character.h"
#include "restaurant.h"
#include "menu.h"
#include <fenv.h>
#include <math.h>
#include <stdio.h>

static u64 fist64(double value) {
  return isfinite(value) && value >= -9223372036854775808.0 && value < 9223372036854775808.0
      ? (u64)(i64)value
      : UINT64_C(0x8000000000000000);
}

// 2B3E3 repair: initialize EDI before the list scan. Stock returns incoming
// EDI when no quotient is positive; only known positive stock scans pass.

// Equal scores keep the earlier property. Multiplication and division use unsigned 32-bit values.

static void format_closure(Rest *rest, const Zone *zone) {
  char text[sizeof(strbuf)];
  int n = snprintf(text,
      sizeof(text),
      "%s closes restaurant no. %d in %s!",
      (const char *)player_ptr->name_text,
      (unsigned)rest->restaurant_id + 1u,
      (const char *)zone->name);

  memcpy(strbuf, text, (size_t)n + 1u);
}
// Loading a language may free the previous descriptor's allocation. That
// lifetime cannot own either restaurant still read by this controller.

// The active count is sampled before any changes and increments only on generation.

void AiSetDefaultRecipeRating(void) {
  AiRecipe *recipe = player_ptr->ai_recipes;
  for (unsigned i = 0; i < 10; ++i, ++recipe) {
    memmove(recipe->rating, recipe->rating + 1, 6 * sizeof(recipe->rating[0]));
    if (recipe->recipe_kind)
      recipe->rating[6] = (i16)((u16)((u32)recipe->default_rating * player_ptr->num_restaurants));
  }
}
// Two separate unsigned divisions, then DWORD wrap.

u32 AiComputeRestaurantCost(Rest *rest, u32 customers) {
  u32 base = 50u * rest->default_rating, sum;
  sum = GetSumOfBytes(rest->menu_rating, 6);
  u32 cost = base + customers * (sum / 6u / 20u);

  return cost;
}

// CF/ZF, so stock's JBE accepts NaN money.

u16 AiHandleRestaurant_Helper2(u32 amount) {
  double money = character_ptr->money;
  u16 result = isnan(money) || (double)amount <= money;

  return result;
}
// Capacity is compared as an unsigned WORD. The return is 1 when the furniture chance
// succeeds, 0 when the counter is increased.

u32 RestMaybeAddByteA2(Rest *rest) {
  u8 increase;
  increase = (u8)(RandomNumber(5) + 1u);
  if ((u16)rest->capacity_of_kind[0] < 500u)
    rest->capacity_of_kind[0] = (i16)((u16)((u16)rest->capacity_of_kind[0] + RandomNumber(40)));
  if (rest->some_furniture_counter < 10)
    increase = 20;
  u32 result = RandomChance(rest->some_furniture_counter) ? 1 : 0;
  if (!result)
    rest->some_furniture_counter = (u8)(rest->some_furniture_counter + increase);

  return result;
}
// Passes unsigned rating bytes to abs. Its unused AL result is the final abs value, or the
// adjustment byte when the last row changes.

u8 AiHandleRestaurant_Helper(Rest *rest, i8 amount) {
  u8 result = 0;
  for (unsigned i = 0; i < 6; ++i) {
    // abs(unsigned BYTE) is identity.
    result = rest->menu_rating[i];
    if (result < 100) {
      result = (u8)amount;
      rest->menu_rating[i] = (u8)(rest->menu_rating[i] + (u8)amount);
    }
  }

  return result;
}
// Consumes both initial RNG draws even for AI targets. Human damage follows the actual
// destruction, rating and notice children.

void HandleUndergroundAttack(Rest *target, Rest *source, u8 severe) {
  u8 difficulty_damage, weapon, damage_count, damaged;
  RandomNumber(10);
  weapon = 4;
  difficulty_damage = (u8)(3u * ptr_apple->difficulty_level);
  damage_count = (u8)(RandomNumber(5) + difficulty_damage);
  if (target->player_index < game_ptr->num_interactive_players) {
    if (severe && target->protection_value) {
      const u8 *name;
      name = GetRestaurantName(target);
      char text[sizeof(strbuf)];
      int n = snprintf(text,
          sizeof(text),
          "Your guardian angels from the underworld prevented an attack by %s on your restaurant %s last night!",
          (const char *)player_ptr->name_text,
          (const char *)name);

      memcpy(strbuf, text, (size_t)n + 1u);
      ShowDelayedMessage(target->player_index, 100, 54, strbuf);
    } else {
      source->some_counter = (u8)((severe + 2u) * (u32)((100 - (i32)character_ptr->dexterity) / 10));
      (void)LoadLangFile(&langfile_action);
      if (severe) {
        attack_damage = 0;
        if (RandomChance(10)) {
          CrashWholeRestaurant(target);
          weapon = 7;
        } else if (RandomChance(25)) {
          weapon = 5;
          attack_contamination = (u8)(RandomNumber(60) + 30u);
          target->attack_var = attack_contamination;
        } else {
          damaged = 0;
          while (damaged < damage_count) {
            u32 index = RandomNumber(target->num_furniture);
            const Furniture *item = pt_rest_furniture_identity(target, (u8)index, false);
            (void)CrashFurniture((Furniture *)item);
            ++damaged;
          }
        }
        u8 kind = (u8)RandomNumber(4);
        HandleRestaurantAttackedStink(target, kind);
      } else
        HandleRestaurantAttackedWeapon(target, weapon);
      FreeLangFile(&langfile_action);
    }
  }
}
// EBP and EDI remain zero throughout, making both menu-adjustment branches unreachable; their
// RNG still runs.

u8 AiHandleRestaurant(Rest *rest) {
  const Poi *poi = rest->property;
  if (rest->some_counter)
    rest->some_counter = (u8)((u32)rest->some_counter - 1u);
  if ((rest->attack_var_1 < -50 || rest->byte8F > 3) && RandomChance(character_ptr->courage) &&
      RandomChance(character_ptr->courage)) {
    Zone *zone;
    zone = FindZoneFromCoordinates(poi->xcoord, poi->ycoord);
    format_closure(rest, zone);
    ShowDelayedMessage(0, 100, 78, strbuf);
    FreeRestaurant(rest, 1);

    return 2;
  }
  if (!rest->some_counter && RandomChance(50) && RandomChance(character_ptr->courage)) {
    RandomNumber(100);
    Poi *target;
    target = FindPoiFromCoords(rest->target_coordinate_x, rest->target_coordinate_y);
    if (target) {
      Rest *restaurant = target->restaurant;
      if (restaurant) {
        u8 severe = (u16)rest->word91 > 100u;
        HandleUndergroundAttack(restaurant, rest, severe);
        if (severe) {
          return 1;
        }
      }
    }
  }
  u32 base = 1000u * rest->default_rating;

  double base_money = base;

  volatile double product = character_ptr->money * ai_restaurant_upgrade_money_factor;
  double projected = product;

  double amount = isnan(projected) || projected <= base_money ? base_money : projected;

  double integral;
  integral = TruncateX87Integral(amount);
  u64 value = fist64(integral);

  i32 budget;
  budget = MulDiv100(pt_signed_dword((u32)value), character_ptr->courage);
  if (RandomChance(character_ptr->management)) {
    u16 afford = AiHandleRestaurant_Helper2((u32)budget);
    if (afford)
      (void)RestMaybeAddByteA2(rest);
  }
  if (rest->attack_var_1 >= 0)
    RandomChance(10);

  return 0;
}
// The restaurant argument is unused.

void NO_OP2(Rest *rest) {
  (void)rest;
}
// Both payments convert unsigned wrapped DWORD totals through nearest float.

void AiRestaurantProc1(Rest *rest) {

  RestCheckIncreaseRange(rest);
  NO_OP2(rest);
  u32 customers = SumListOfWords(rest->customer_count, 6);

  u32 revenue = customers * (u16)rest->average_pizza_price, cost;
  cost = AiComputeRestaurantCost(rest, customers);
  u32 sales = (u32)savegame_sect_0.past_sales_per_day_for_players[rest->player_index][ptr_apple->town_index];
  savegame_sect_0.past_sales_per_day_for_players[rest->player_index][ptr_apple->town_index] =
      pt_signed_dword(sales + revenue);

  (void)TransferPlayerMoney(rest->player_index, 5, 10, (float)revenue);

  (void)TransferPlayerMoney(rest->player_index, 10, 5, (float)cost);

  rest->attack_var_1 = (i16)((u16)((u16)rest->attack_var_1 + 1u));
}
// Wraps the final linked-list sentinel to the first POI.

PTForwardLink *GetNextLinkedElem(const PTForwardLink *element) {
  PTForwardLink *next = element->next;

  return next->next ? next : current_town->poi.begin.next;
}
// Uses low WORD loop counters, including range zero.

Poi *GetRandomPoiOfKind(u8 kind) {
  PTForwardLink *poi = current_town->poi.begin.next;
  u16 remaining = current_town->num_poi, start = (u16)RandomNumber(remaining);
  while (--start != UINT16_MAX)
    poi = GetNextLinkedElem(poi);
  while (--remaining != UINT16_MAX) {
    Poi *record = ((Poi *)(poi));
    if ((!kind && record->property_type < kPropertyRestaurant) || (kind == 1 && record->restaurant) ||
        (kind == 2 && record->property_type == kPropertyShop)) {
      return record;
    }
    poi = GetNextLinkedElem(poi);
  }
  poi = current_town->poi.begin.next;

  return (Poi *)poi;
}
u32 GetDistanceBetweenPoi(const Poi *a, const Poi *b) {
  i32 dx = (i32)a->xcoord - b->xcoord, dy = (i32)a->ycoord - b->ycoord;
  dx = abs(dx);
  dy = abs(dy);

  return (u32)(dx + dy);
}
u8 RestResetAverageRatings(Rest *rest, u8 rating) {
  rest->byte2B = rating;
  memset(rest->quality_block, rating, sizeof(rest->quality_block));

  return 5;
}
static Zone *find_zone(Poi *poi) {
  return FindZoneFromCoordinates(poi->xcoord, poi->ycoord);
}

// Clears precisely 100 scratch bytes. IDs outside that initialized region stop at their
// original scratch access, never pass.

Rest *AllocateNewRestaurant(u8 owner, Poi *poi) {

  Player *player = pt_player(owner);
  (void)UpdatePoiCustomerCount(poi);
  u8 used_ids[100];
  memset(used_ids, 0, sizeof(used_ids));
  u32 size = owner >= game_ptr->num_interactive_players ? PT_AI_RESTAURANT_SIZE : PT_HUMAN_RESTAURANT_ALLOCATION_SIZE;
  Rest *allocation;
  allocation = pt_simulation_allocate(PT_SIM_REST, size);
  if (!allocation) {
    if (owner < game_ptr->num_interactive_players)
      DisplayErrorMessage(0, new_restaurant_memory_error);

    return NULL;
  }
  Rest *rest = allocation;
  if (owner < game_ptr->num_interactive_players) {
    memmove(&allocation->floor, empty_restaurant_floor, sizeof(empty_restaurant_floor));
    memset(rest->last_supplier_of_stock, 0, 4);
  }
  PTForwardLink *node = player->restaurants.begin.next;
  PTForwardLink *seen[256];
  unsigned count = 0;
  for (;;) {
    PTForwardLink *next = node->next;
    if (!next)
      break;

    u8 id = ((const Rest *)node)->restaurant_id;

    seen[count++] = node;
    used_ids[id] = 1;
    node = next;
  }
  u8 id = 0;
  while (id < 100 && used_ids[id])
    ++id;

  rest->restaurant_id = id;
  (void)LinkList_InsertLast(&pt_player(owner)->restaurants, &rest->linked);
  Zone *zone = find_zone(poi);

  poi->restaurant = allocation;
  poi->property_type = kPropertyRestaurant;
  poi->for_sale_or_rent_flags = 0;
  poi->current_tenant = owner;
  rest->property = poi;
  rest->player_index = owner;
  rest->sum_of_reputations = 0;
  rest->byte29 = 70;
  i32 crime_range = 100 - (i32)zone->crime_statistics;
  crime_range = crime_range >= 0 ? crime_range / 4 : -((-crime_range + 3) / 4);
  rest->protection_value = (u8)(RandomNumber((u32)crime_range) - 54u);
  ++ramtown.num_restaurant_per_player[owner];
  player->num_restaurants = (u8)((u32)player->num_restaurants + 1u);
  player->num_restaurants_in_city[ptr_apple->town_index] =
      (u8)((u32)(player->num_restaurants_in_city[ptr_apple->town_index]) + 1u);

  rest->original_map_value = loaded_map_file[poi->ycoord][poi->xcoord];
  loaded_map_file[poi->ycoord][poi->xcoord] = (u8)((owner << 4) | 10u);
  rest->restaurant_range_squared = 50;
  zone = find_zone(poi);
  rest->word1D = (i16)(zone->unknown_word >> 3);
  for (unsigned i = 0; i < 6; ++i) {
    rest->menu_rating[i] = 1;
    rest->furniture_rating[i] = 1;
    rest->customer_count[i] = 0;
  }
  memset(rest->delivcapa_per_day, 0, 7);
  if (owner >= game_ptr->num_interactive_players)
    rest->restaurant_opened = 1;
  else {
    rest->dword1F4 = pt_signed_dword((RandomNumber(60) + 180u) * (u32)poi->size_x * poi->size_y);
    rest->use_real_demand = 0;
    rest->days_until_major_visit = (u8)(RandomNumber(20) + 140u);
    rest->byte8C = 200;
    rest->average_chef_time = 15.0f;
    rest->average_oven_time = 15.0f;
  }

  return allocation;
}
// The payment precedes allocation and is not refunded on failure. The saved previous player
// comes from Apple, not the global.

u32 AiCreateNewRestaurant(Poi *poi, u8 owner, u8 buy) {
  u8 large_template, previous_player;
  u32 created;
  Character *character = pt_character(owner);
  large_template = 0;
  previous_player = ptr_apple->player_index;
  created = 0;
  SetCurrentPlayer(owner);
  if (!poi->total_customer_count)
    (void)UpdatePoiCustomerCount(poi);
  u32 amount = (u32)(buy ? poi->purchase_price : poi->rent_price);

  (void)TransferPlayerMoney(current_player_index, buy ? 0 : 10, 5, (float)amount);
  if (buy)
    poi->current_owner = owner;
  Rest *allocation;
  allocation = AllocateNewRestaurant(owner, poi);
  if (!allocation) {
    DisplayErrorMessage(0, ai_restaurant_memory_error);
    goto restore_player;
  }
  Rest *rest = allocation;
  created = 1;
  rest->restaurant_opened = 1;
  rest->ai_preferred_age_group = (u8)RandomNumber(3);
  if (poi->total_customer_count && character->intelligence > 40) {
    for (unsigned i = 0; i < 6; i += 2) {
      i32 percentage;
      percentage = Mul100Div((u16)poi->customer_count_in_area[i] + (u32)(u16)poi->customer_count_in_area[i + 1],
          poi->total_customer_count);
      if (percentage > 40)
        rest->ai_preferred_age_group = (u8)(i / 2);
    }
  }
  rest->default_rating = (u8)(RandomNumber(3) + 1u);
  rest->average_pizza_price = (i16)(2u * rest->default_rating + 6u);
  if ((u32)poi->size_x * poi->size_y > 240u)
    large_template = 1;
  rest->template_rest = (u8)(2u * rest->ai_preferred_age_group + 6u * RandomNumber(3) + 1u + large_template);
  (void)RestResetAverageRatings(rest, (u8)(30u * rest->default_rating));
  if (isnan(character->money) || isnan(ai_low_rating_money_threshold) ||
      character->money < ai_low_rating_money_threshold)
    rest->byte2B = (u8)(rest->byte2B - 10u);
  if ((isnan(character->money) || isnan(ai_low_default_rating_money_threshold) ||
          character->money < ai_low_default_rating_money_threshold) &&
      rest->default_rating > 1)
    rest->default_rating = (u8)((u32)rest->default_rating - 1u);
  (void)RestMaybeAddByteA2(rest);
  for (unsigned i = 0; i < 6; ++i) {
    rest->menu_rating[i] = (u8)(RandomNumber(character->cooking) + 20u);
    rest->furniture_rating[i] = (u8)(RandomNumber(character->intelligence) + 20u);
  }
restore_player: { SetCurrentPlayer(previous_player); }
  u32 result = created;

  return result;
}
u8 AiBuildRestaurant(void) {
  Poi *selected;
  u8 player_index, buy;
  selected = GetRandomPoiOfKind(2);
  player_index = current_player_index;
  buy = 0;
  if (selected == (Poi *)current_town->poi.begin.next) {
    return 0;
  }

  const Poi *poi = selected;
  for (unsigned owner = 0; owner < 8; ++owner) {
    if (owner >= game_ptr->num_interactive_players) {
      PTForwardLink *node = pt_player(owner)->restaurants.begin.next;

      for (;;) {
        PTForwardLink *next = node->next;
        if (!next)
          break;

        u32 distance;
        const Poi *other = (const Poi *)(((const Rest *)node))->property;
        distance = GetDistanceBetweenPoi(poi, other);
        if (distance < 6) {
          return 0;
        }
        node = next;
      }
    }
  }
  if (!RandomChance(character_ptr->courage)) {
    return 0;
  }
  u16 afford = AiHandleRestaurant_Helper2((u32)poi->purchase_price);
  if (afford)
    buy = 1;
  if (poi->for_sale_or_rent_flags <= 1u - buy) {
    return 0;
  }
  u32 created = AiCreateNewRestaurant(selected, player_index, buy);
  if (!(u16)created) {
    return 0;
  }
  Player *player = pt_player(player_index);
  player->byte24D = (u8)((100 - (i32)character_ptr->courage) / 2);
  Zone *zone = find_zone(selected);
  char text[sizeof(strbuf)];
  int n = snprintf(text,
      sizeof(text),
      "%s %s a new restaurant in %s!",
      (const char *)(pt_player(player_index)->name_text),
      buy ? "buys" : "rents",
      (const char *)zone->name);

  memcpy(strbuf, text, (size_t)n + 1u);
  ShowDelayedMessage(0, 100, 84, strbuf);

  return 1;
}

// No meaningful return to the AI day caller.

void AiMaybeBuildRestaurant(void) {
  u8 count = player_ptr->num_restaurants;
  bool build = !count;
  if (count && count <= 20) {
    bool competitive = true;
    if (count > 10) {
      u32 index = RandomNumber(game_ptr->num_interactive_players);
      u8 other = pt_player_restaurant_count(index);
      competitive = count <= (u32)other + 3u;
    }
    build = competitive && !player_ptr->byte24D;
  }
  if (build)
    (void)AiBuildRestaurant();
}
// Decision result persists across all AI players. Result 2 breaks after a freed restaurant,
// before reloading its invalid next link.

void AiRunTownDayLoop(void) {
  u8 decision_result;
  u8 previous = current_player_index;

  decision_result = 0;

  for (u8 player = game_ptr->num_interactive_players; player < 8; ++player) {
    SetCurrentPlayer(player);
    AiSetDefaultRecipeRating();
    savegame_sect_0.past_sales_per_day_for_players[current_player_index][ptr_apple->town_index] = 0;
    PTForwardLink *node = player_ptr->restaurants.begin.next;
    PTForwardLink *seen[4096];
    unsigned count = 0;
    for (;;) {
      PTForwardLink *next = node->next;
      if (!next)
        break;

      seen[count++] = node;

      Rest *rest = (Rest *)node;
      RestCheckIncreaseRange(rest);
      RestCheckIncreaseRange2(rest);
      if (!rest->restaurant_closed_for_days) {
        rest->restaurant_opened = 1;
        AiRestaurantProc1(rest);
        if (!decision_result) {
          u8 result = AiHandleRestaurant(rest);
          decision_result = result;
        }
        if (decision_result == 2)
          break;
      }
      node = node->next;
    }
    AiMaybeBuildRestaurant();
  }
  SetCurrentPlayer(previous);
}
