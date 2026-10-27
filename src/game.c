// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
// Game init.

#include "objects.h"
#include "game.h"
#include "runtime_state.h"
#include "frame.h"
#include "text.h"

void pt_list_init(LinkedHead *head) {
  head->begin.next = &head->end;
  head->begin.previous = NULL;
  head->end.next = NULL;
  head->end.previous = &head->begin;
}

// Each list ends at its own end node.

void InitTown(Town *town) {
  pt_list_init(&town->poi);
  pt_list_init(&town->zones);
  pt_list_init(&town->suppliers);
}

// Return the selected player.

Player *InitRestaurantsInPlayer(u8 index) {
  Player *p = pt_player(index);
  pt_list_init(&p->restaurants);
  p->num_restaurants = 0;
  ramtown.num_restaurant_per_player[index] = 0;
  pt_list_init(&p->contracts);
  p->num_contracts = 0;
  ramtown.num_contracts_per_player[index] = 0;
  Player *result = p;

  return result;
}

// MOVZX establishes unsigned property bytes.

void InitDepotsInPlayer(u8 index) {
  Player *p = pt_player(index);
  Character *c = pt_character(index);
  c->social_status = (u16)(c->popularity / 2u + 220u);
  c->underworld_status = (u16)(c->courage / 2u + 220u);
  for (unsigned i = 0; i < 10; ++i) {
    p->depots[i] = NULL;
    p->num_restaurants_in_city[i] = 0;
  }
}

void InitLinkedListsInPlayer(u8 index) {
  Player *p = pt_player(index);
  pt_list_init(&p->recipes);
  p->num_recipes = 0;
  p->index_of_next_goal = 0;
  (void)InitRestaurantsInPlayer(index);
}

void SetCurrentPlayer(u8 index) {
  ptr_apple->player_index = index;
  current_player_index = index;
  player_ptr = pt_player(index);
  character_ptr = pt_character(index);
}

void SetGameFlag8(void) {
  game_ptr->dword8 = 1;
}

// Leaves the sentinel forward link untouched.

void InitAppleFields(void) {
  Apple *r = ptr_apple;
  r->links.begin.next = &r->links.end;
  r->links.end.previous = &r->links.begin;
}

void InitBanana(Banana *b) {
  (void)memset(b, 0, sizeof(*b));
}
void MarkBananaFree(Banana *b) {
  b->is_free = 1;
}
Banana *CreateBanana(void) {
  for (unsigned i = 0; i < 16; ++i) {
    if (bananas[i].is_free) {
      InitBanana(&bananas[i]);
      Banana *result = &bananas[i];

      return result;
    }
  }

  return NULL;
}

void InitBananas(void) {
  u8 slot;
  slot = 0;
  while (slot < 16) {
    u8 i = slot;
    InitBanana(&bananas[i]);
    bananas[i].is_free = 1;
    ++slot;
  }
  // The original inlines the room links instead of calling InitAppleFields.

  Apple *r = ptr_apple;
  r->links.begin.next = &r->links.end;
  r->links.end.previous = &r->links.begin;
  r->links.end.next = NULL;
}

void InitCarrot(void) {
  memset(message_slots_free, 1, sizeof(message_slots_free));
}
u8 AllocateMessageSlot(void) {
  for (u8 i = 1; i < 16; ++i) {
    if (message_slots_free[i]) {
      message_slots_free[i] = 0;
      return i;
    }
  }
  return 0;
}
u8 MarkMessageSlotFree(u8 slot) {
  if (slot >= 16)
    abort();
  message_slots_free[slot] = 1;
  return slot;
}

u32 *initrandnext(void) {
  return &pt_random_state;
}
u32 pt_rand(void) {
  u32 *random;
  random = initrandnext();
  (void)random;
  pt_random_state = UINT32_C(1103515245) * pt_random_state + UINT32_C(12345);
  return (pt_random_state >> 16) & UINT32_C(0x7fff);
}
u32 *pt_srand(u32 seed) {
  u32 *result;
  result = initrandnext();
  pt_random_state = seed;
  return result;
}

// Retain the full 32-bit result; callers may use its low 16 bits.

u32 RandomNumber(u32 range) {
  u32 value;
  value = pt_rand();
  return (range * value) >> 15;
}
u32 RandomNumber_(u32 range) {
  return RandomNumber(range);
}
u8 RandomChance(i32 probability) {
  if (probability <= 0)
    return 0;
  if (probability >= 100)
    return 1;
  u32 value;
  value = pt_rand();
  return (u8)((100u * value >> 15) < (u32)probability);
}

// Keep the weapon-price flag and random draws in their original order.

void Game_Init2(Game *target) {
  // Original 1995 starts after every bundled campaign deadline. The
  // labelled original repair changes this to 1994. The native release
  // uses that explicit policy; pristine behavior belongs to the reference.

  // Repair behavior is selected explicitly in native_profile.h.

  u16 year = PT_GAME_START_YEAR;
  u16 index;
  u8 weapon_price_flags;
  (void)memset(ptr_apple, 0, sizeof(Apple));
  Apple *r = ptr_apple;
  r->day = 1;
  r->month = 3;
  r->year = year;
  r->weekday = 0;
  target->byteB2 = 0;
  target->paragraph_render_mode = 0;
  (void)memset(array_size_8, 0, sizeof(array_size_8));
  InitBananas();
  InitCarrot();
  InitTown(&durian);
  index = 0;
  while (index < 8) {
    InitLinkedListsInPlayer((u8)index);
    index = (u16)(index + 1);
  }
  index = 0;
  while (index < 10) {
    for (u16 j = 0; j < 4; ++j) {
      u8 chance;
      u32 random;
      chance = RandomChance(50);
      weapon_price_flags = (u8)(chance << 7);
      random = RandomNumber(80);
      weapon_base_price[4u * index + j] = (u8)((u8)(random + 40u) | weapon_price_flags);
    }
    index = (u16)(index + 1);
  }
  r->byte35 = target->num_interactive_players;
  r->town_index = UINT8_MAX;
  r->byte38 = 18;
  savegame_sect_0.extra_flooring_enabled = 0;
  index = 0;
  while (index < 8) {
    for (u16 j = 0; j < 8; ++j) {
      u32 random;
      random = RandomNumber(3);
      players[index].ai_recipes[j].recipe_kind = (u8)(random + 1u);
    }
    index = (u16)(index + 1);
  }
}

// Game objects.

#include "state_types.h"
#include "initial_data.h"
#include "memory.h"
#include <stdlib.h>
#include <string.h>

_Static_assert(offsetof(Recipe, linked) == 0, "recipe list node is its record prefix");
_Static_assert(offsetof(Supplier, link) == 0, "supplier list node is its record prefix");

Player players[8];
Town durian;
Rest temp_rest;
Apple room;

static PTSimulationStorage simulation_storage;
void pt_simulation_storage(const PTSimulationStorage *storage) {
  if (storage && (!!storage->bind != !!storage->release))
    abort();
  simulation_storage = storage ? *storage : (PTSimulationStorage){0};
}
void *pt_simulation_allocate(PTSimulationKind kind, size_t size) {
  static const size_t minimum[PT_SIM_KIND_COUNT] = {[PT_SIM_CONTRACT] = sizeof(Contract),
    [PT_SIM_RESTORECIPE] = sizeof(RestoRecipe),
    [PT_SIM_TOWN] = sizeof(Town),
    [PT_SIM_SUPPLIER] = sizeof(Supplier),
    [PT_SIM_ZONE] = sizeof(Zone),
    [PT_SIM_POI] = sizeof(Poi),
    [PT_SIM_RECIPE] = sizeof(Recipe),
    [PT_SIM_PLAYER] = sizeof(Player),
    [PT_SIM_APPLE] = sizeof(Apple),
    [PT_SIM_REST] = sizeof(Rest) + 1u,
    [PT_SIM_MOUSE] = sizeof(Mouse)};
  if ((unsigned)kind >= PT_SIM_KIND_COUNT || size > UINT32_MAX)
    abort();
  if (kind == PT_SIM_RECIPE &&
      (size < sizeof(Recipe) + sizeof(RecipeEntry) || (size - sizeof(Recipe)) % sizeof(RecipeEntry)))
    abort();
  if (size < minimum[kind] &&
      !(kind == PT_SIM_REST && (size == sizeof(Rest) || size == offsetof(Rest, some_furniture_counter) + 1u)))
    abort();
  // AI restaurants expose a shorter logical prefix, but still own a complete
  // native record. No native member points into a serialized allocation.

  size_t backing = size < minimum[kind] ? minimum[kind] : size;
  void *object = calloc(1, backing);
  if (!object)
    return NULL;
  if (simulation_storage.bind && !simulation_storage.bind(kind, size, object)) {
    free(object);
    return NULL;
  }
  if (kind == PT_SIM_RECIPE)
    ((Recipe *)object)->entry_capacity = (size - sizeof(Recipe)) / sizeof(RecipeEntry);
  return object;
}
void pt_simulation_release(const void *object, size_t size) {
  if (size > UINT32_MAX)
    abort();
  if (simulation_storage.release) {
    simulation_storage.release(object, size);
    return;
  }
  free((void *)object);
}

Recipe *pt_recipe_copy(Recipe *target, const Recipe *source, u32 entries) {
  if (entries > UINT8_MAX)
    abort();
  // The original record copy includes its one allocated trailing entry.

  size_t count = (size_t)entries + 1u, capacity = target->entry_capacity;
  if (!capacity || !source->entry_capacity || count > capacity || count > source->entry_capacity)
    abort();
  if (target != source) {
    *target = *source;
    target->entry_capacity = capacity;
    memmove(target->entries, source->entries, count * sizeof(*target->entries));
  }

  return target;
}
Recipe *pt_recipe_reset(Recipe *recipe) {
  size_t capacity = recipe->entry_capacity;
  if (!capacity || capacity > 256)
    abort();
  *recipe = (Recipe){.entry_capacity = capacity};
  memset(recipe->entries, 0, capacity * sizeof(*recipe->entries));

  return recipe;
}
Game game;
Town *current_town;

Game *pt_game_initialize(Game *owner) {
  LangFileDesc *storage = owner->language_storage;
  *owner = (Game){0};
  owner->language_storage = storage;
  return owner;
}

Rect *rect_1;
Zone *mouse_over_zone;
Zone *last_mouse_over_district;
const u8 *unit_for_number_query;
const u8 *button_texts[34][9];
Player *player_ptr;
Character *character_ptr;
Poi *last_customer_count_poi;
const u8 *current_button_hint;
const u8 *popup_menu_items[24];
GuestFileStream *cur_loading_filehandle;
u8 *ingredient_background;
GuestFileStream *cur_writing_filehandle;
u8 *buffer_backup;
Recipe *last_recipe_ptr;
Rest *restaurant_with_guests;
Rest *restaurant_hit_test_owner;
InputEvent *last_input_event;
Recipe *recipe_lookup_fallback;
u8 *buffer_2;
Apple *ptr_apple;
Rest *curr_rest;
Game *game_ptr;
Mouse *mouse_result;
Jatoba *jatoba_1;
Character *interactive_attack_attacker;
Character *interactive_attack_victim;
Character *recipe_rank_character;
u8 *newspaper_article_buffer;

i32 GetRecipeId(const Player *player, const Recipe *recipe) {
  (void)player;
  return (i8)recipe->id;
}

const Recipe *ResolveRecipe(const Player *player, u8 id) {
  for (PTForwardLink *node = player->recipes.begin.next; node->next; node = node->next) {
    const Recipe *recipe = (const Recipe *)node;
    if (recipe->id == id)
      return recipe;
  }
  return recipe_lookup_fallback;
}

Supplier *ResolveSupplier(u8 id) {
  for (PTForwardLink *node = current_town->suppliers.begin.next; node->next; node = node->next) {
    Supplier *supplier = (Supplier *)node;
    if (supplier->supplier_id == id)
      return supplier;
  }
  return NULL;
}

LinkedHead *LinkList_InsertFirst(LinkedHead *head, LinkedElem *element) {
  element->next = head->begin.next;
  element->previous = &head->begin;
  if (!head->begin.next)
    head->end.previous = element;
  else
    head->begin.next->previous = element;
  head->begin.next = element;
  return head;
}

LinkedHead *LinkList_InsertLast(LinkedHead *head, LinkedElem *element) {
  PTForwardLink *last = head->end.previous;
  element->next = &head->end;
  element->previous = last;
  last->next = element;
  head->end.previous = element;
  return head;
}

PTForwardLink *LinkList_RemoveFirst(LinkedHead *head) {
  PTForwardLink *first = head->begin.next;
  PTForwardLink *next = first->next;
  if (next) {
    head->begin.next = next;
    next->previous = &head->begin;
  }
  return next ? first : NULL;
}

PTForwardLink *LinkList_Remove(LinkedElem *element) {
  LinkedElem *next = element->next;
  PTForwardLink *previous = element->previous;
  next->previous = previous;
  previous->next = next;
  return next;
}

PTListResult LinkList_ByteAt(const LinkedElem *element, PTListKey key) {
  const u8 *field;
  switch (key) {
  case PT_LIST_RECIPE_ID:
    field = &((const Recipe *)element)->id;
    break;
  case PT_LIST_SUPPLIER_QUALITY:
    field = &((const Supplier *)element)->quality;
    break;
  default:
    abort();
  }
  return (PTListResult){PT_LIST_KEY, field, *field};
}

void *LinkList_InsertAfter(LinkedHead *head, LinkedElem *element, PTForwardLink *previous) {
  PTForwardLink *next = previous->next;
  void *result = element;
  if (!next || !next->next) {
    result = next ? LinkList_InsertLast(head, element) : LinkList_InsertFirst(head, element);
  } else {
    element->next = next;
    element->previous = previous;
    previous->next = element;
    element->next->previous = element;
  }
  return result;
}

PTListResult LinkList_InsertSorted(LinkedHead *head, LinkedElem *element, PTListKey key) {
  PTForwardLink *current = head->begin.next;
  PTForwardLink *next = current->next;
  PTForwardLink *item = element;
  u8 comparison_key;
  PTListResult result;
  if (!next) {
    result = pt_list_object_result(LinkList_InsertFirst(head, element));
  } else {
    comparison_key = LinkList_ByteAt(current, key).value;
    result = LinkList_ByteAt(element, key);
    if (result.value < comparison_key) {
      result = pt_list_object_result(LinkList_InsertFirst(head, element));
    } else
      for (;;) {
        if (!next) {
          result = pt_list_object_result(LinkList_InsertLast(head, element));
          break;
        }
        if (next == item || current == item)
          break;
        comparison_key = next->next ? LinkList_ByteAt(next, key).value : 0;
        result = LinkList_ByteAt(element, key);
        if (result.value < comparison_key && next->next) {
          result = pt_list_object_result(LinkList_InsertAfter(head, element, current));
          break;
        }
        current = next;
        next = next->next;
      }
  }
  return result;
}

// Fresh simulation records own C pointers and arrays from their first use.

void pt_native_game_defaults_apply(void) {
  game = (Game){0};
  for (size_t i = 0; i < sizeof(players) / sizeof(*players); ++i)
    players[i] = (Player){0};
  durian = (Town){0};
  temp_rest = (Rest){0};
  room = (Apple){0};
  current_town = &durian;
}

PTInterfaceStrings pt_interface_strings;

static const u8 *const initial_button_texts[34][9] = {
  {pt_interface_strings.hint_map,
    pt_interface_strings.hint_character,
    pt_interface_strings.hint_disk_options,
    pt_interface_strings.hint_advertising,
    pt_interface_strings.hint_market_research,
    pt_interface_strings.hint_newspaper,
    pt_interface_strings.hint_global_stats,
    pt_interface_strings.hint_terminal,
    pt_interface_strings.hint_end_turn},
  {pt_interface_strings.hint_open_close,
    pt_interface_strings.hint_stock,
    pt_interface_strings.hint_menu,
    pt_interface_strings.hint_furnishing,
    pt_interface_strings.hint_staff,
    pt_interface_strings.hint_recipe_list,
    pt_interface_strings.hint_restaurant_stats,
    pt_interface_strings.hint_terminal,
    pt_interface_strings.hint_animated_stats},
  {pt_interface_strings.hint_vegetables,
    pt_interface_strings.hint_meat,
    pt_interface_strings.hint_contracts,
    pt_interface_strings.hint_fruit,
    pt_interface_strings.hint_cheese,
    pt_interface_strings.hint_contracts,
    pt_interface_strings.hint_shop,
    pt_interface_strings.hint_shop,
    pt_interface_strings.hint_back},
  {NULL,
    NULL,
    pt_interface_strings.hint_reject,
    NULL,
    NULL,
    pt_interface_strings.hint_reject,
    pt_interface_strings.hint_agree,
    pt_interface_strings.hint_agree,
    pt_interface_strings.hint_back},
  {pt_interface_strings.hint_furniture,
    pt_interface_strings.hint_kitchenware,
    pt_interface_strings.hint_customer_ratings,
    pt_interface_strings.hint_extras,
    pt_interface_strings.hint_flooring,
    pt_interface_strings.hint_customer_ratings,
    pt_interface_strings.hint_shop,
    pt_interface_strings.hint_shop,
    pt_interface_strings.hint_back},
  {pt_interface_strings.hint_vegetables,
    pt_interface_strings.hint_meat,
    NULL,
    pt_interface_strings.hint_fruit,
    pt_interface_strings.hint_cheese,
    NULL,
    pt_interface_strings.hint_weapons,
    pt_interface_strings.hint_weapons,
    pt_interface_strings.hint_back},
  {pt_interface_strings.hint_single_order,
    pt_interface_strings.hint_single_order,
    NULL,
    pt_interface_strings.hint_make_contract,
    pt_interface_strings.hint_make_contract,
    NULL,
    NULL,
    NULL,
    pt_interface_strings.hint_back},
  {pt_interface_strings.hint_turn_page,
    pt_interface_strings.hint_menu,
    pt_interface_strings.hint_menu,
    pt_interface_strings.hint_turn_page,
    pt_interface_strings.hint_pizza_sales,
    pt_interface_strings.hint_customer_ratings,
    pt_interface_strings.hint_remove_pizza,
    pt_interface_strings.hint_remove_pizza,
    pt_interface_strings.hint_back},
  {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
  {pt_interface_strings.hint_view_pizza,
    pt_interface_strings.hint_ingredients,
    pt_interface_strings.hint_pizza_info,
    NULL,
    NULL,
    NULL,
    pt_interface_strings.hint_o_k,
    pt_interface_strings.hint_o_k,
    pt_interface_strings.hint_back},
  {NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    pt_interface_strings.hint_o_k,
    pt_interface_strings.hint_o_k,
    pt_interface_strings.hint_back},
  {pt_interface_strings.hint_turn_page,
    pt_interface_strings.hint_turn_page,
    pt_interface_strings.hint_flip_page,
    pt_interface_strings.hint_turn_page,
    pt_interface_strings.hint_turn_page,
    pt_interface_strings.hint_flip_page,
    pt_interface_strings.hint_cancel_contract,
    pt_interface_strings.hint_cancel_contract,
    pt_interface_strings.hint_back},
  {pt_interface_strings.hint_chefs,
    pt_interface_strings.hint_waiters,
    pt_interface_strings.hint_managers,
    pt_interface_strings.hint_fire,
    pt_interface_strings.hint_wages,
    pt_interface_strings.hint_training,
    pt_interface_strings.hint_recruit,
    pt_interface_strings.hint_recruit,
    pt_interface_strings.hint_back},
  {pt_interface_strings.hint_production,
    pt_interface_strings.hint_production,
    pt_interface_strings.hint_reputation,
    pt_interface_strings.hint_capacity,
    pt_interface_strings.hint_capacity,
    pt_interface_strings.hint_pizza_quality,
    pt_interface_strings.hint_popularity,
    pt_interface_strings.hint_turnover,
    pt_interface_strings.hint_back},
  {pt_interface_strings.hint_chefs,
    pt_interface_strings.hint_waiters,
    pt_interface_strings.hint_managers,
    pt_interface_strings.hint_turn_page,
    pt_interface_strings.hint_data,
    pt_interface_strings.hint_applications,
    pt_interface_strings.hint_turn_page,
    pt_interface_strings.hint_hire,
    pt_interface_strings.hint_back},
  {pt_interface_strings.hint_turn_page,
    pt_interface_strings.hint_turn_page,
    NULL,
    pt_interface_strings.hint_turn_page,
    pt_interface_strings.hint_turn_page,
    NULL,
    pt_interface_strings.hint_buy,
    pt_interface_strings.hint_buy,
    pt_interface_strings.hint_back},
  {pt_interface_strings.hint_dial_1,
    pt_interface_strings.hint_dial_2,
    pt_interface_strings.hint_dial_3,
    pt_interface_strings.hint_dial_4,
    pt_interface_strings.hint_dial_5,
    pt_interface_strings.hint_dial_6,
    pt_interface_strings.hint_dial_7,
    pt_interface_strings.hint_dial_8,
    pt_interface_strings.hint_dial_9},
  {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, pt_interface_strings.hint_back},
  {pt_interface_strings.hint_character_values,
    pt_interface_strings.hint_company,
    pt_interface_strings.hint_curriculum_vitae,
    pt_interface_strings.hint_position,
    pt_interface_strings.hint_awards,
    pt_interface_strings.hint_target_cv,
    pt_interface_strings.hint_turn_page,
    pt_interface_strings.hint_turn_page,
    pt_interface_strings.hint_back},
  {pt_interface_strings.hint_challenge,
    pt_interface_strings.hint_challenge,
    NULL,
    pt_interface_strings.hint_sabotage,
    pt_interface_strings.hint_sabotage,
    NULL,
    NULL,
    NULL,
    pt_interface_strings.hint_back},
  {pt_interface_strings.hint_view_pizza,
    pt_interface_strings.hint_ingredients,
    pt_interface_strings.hint_pizza_info,
    pt_interface_strings.hint_turn_page,
    pt_interface_strings.hint_recipe_list,
    pt_interface_strings.hint_transfer,
    pt_interface_strings.hint_turn_page,
    pt_interface_strings.hint_bake_pizza,
    pt_interface_strings.hint_back},
  {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
  {pt_interface_strings.hint_vegetables,
    pt_interface_strings.hint_meat,
    pt_interface_strings.hint_export,
    pt_interface_strings.hint_fruit,
    pt_interface_strings.hint_cheese,
    pt_interface_strings.hint_priorities,
    pt_interface_strings.hint_buy,
    pt_interface_strings.hint_sell,
    pt_interface_strings.hint_back},
  {pt_interface_strings.hint_next_item,
    pt_interface_strings.hint_next_item,
    pt_interface_strings.hint_throw_away,
    pt_interface_strings.hint_previous_item,
    pt_interface_strings.hint_previous_item,
    pt_interface_strings.hint_throw_away,
    NULL,
    NULL,
    NULL},
  {pt_interface_strings.hint_month_overview,
    pt_interface_strings.hint_restaurants,
    pt_interface_strings.hint_cash_flow,
    pt_interface_strings.hint_loans,
    pt_interface_strings.hint_insurance,
    pt_interface_strings.hint_property,
    pt_interface_strings.hint_balance,
    pt_interface_strings.hint_daily_profits,
    pt_interface_strings.hint_back},
  {pt_interface_strings.hint_daily_balance,
    pt_interface_strings.hint_daily_balance,
    pt_interface_strings.hint_week_review,
    pt_interface_strings.hint_month_review,
    pt_interface_strings.hint_week_s_takings,
    pt_interface_strings.hint_customer_stats,
    pt_interface_strings.hint_sales_curve,
    pt_interface_strings.hint_sales_curve,
    pt_interface_strings.hint_back},
  {pt_interface_strings.hint_area_info,
    market_property_title,
    pt_interface_strings.hint_economy,
    pt_interface_strings.hint_customer_taste,
    pt_interface_strings.hint_trends,
    pt_interface_strings.hint_pizza_charts,
    pt_interface_strings.hint_competitors,
    pt_interface_strings.hint_market_share,
    pt_interface_strings.hint_back},
  {pt_interface_strings.hint_turn_page,
    pt_interface_strings.hint_turn_page,
    NULL,
    pt_interface_strings.hint_turn_page,
    pt_interface_strings.hint_turn_page,
    NULL,
    NULL,
    NULL,
    pt_interface_strings.hint_back},
  {pt_interface_strings.hint_vegetables,
    pt_interface_strings.hint_meat,
    NULL,
    pt_interface_strings.hint_fruit,
    pt_interface_strings.hint_cheese,
    NULL,
    NULL,
    NULL,
    pt_interface_strings.hint_back},
  {pt_interface_strings.hint_furniture,
    pt_interface_strings.hint_kitchenware,
    NULL,
    pt_interface_strings.hint_extras,
    pt_interface_strings.hint_flooring,
    NULL,
    NULL,
    NULL,
    pt_interface_strings.hint_back},
  {NULL, NULL, NULL, NULL, NULL, NULL, NULL, pt_interface_strings.hint_continue, pt_interface_strings.hint_continue},
  {pt_interface_strings.hint_throw_away,
    pt_interface_strings.hint_throw_away,
    NULL,
    pt_interface_strings.hint_move_elsewhere,
    pt_interface_strings.hint_move_elsewhere,
    NULL,
    pt_interface_strings.hint_sell_it,
    pt_interface_strings.hint_sell_it,
    NULL},
  {NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    pt_interface_strings.hint_dial_new_number,
    pt_interface_strings.hint_dial_new_number,
    pt_interface_strings.hint_back},
  {pt_interface_strings.hint_turn_page,
    pt_interface_strings.hint_turn_page,
    pt_interface_strings.hint_view_ads,
    pt_interface_strings.hint_turn_page,
    pt_interface_strings.hint_turn_page,
    pt_interface_strings.hint_view_ads,
    pt_interface_strings.hint_buy,
    pt_interface_strings.hint_buy,
    pt_interface_strings.hint_back},
};

void pt_native_game_root_defaults_apply(void) {
  rect_1 = &restaurant_mask_rect;
  mouse_over_zone = NULL;
  last_mouse_over_district = NULL;
  unit_for_number_query = pt_interface_strings.empty_unit;
  player_ptr = players;
  character_ptr = characters;
  last_customer_count_poi = NULL;
  current_button_hint = NULL;
  cur_loading_filehandle = NULL;
  ingredient_background = NULL;
  cur_writing_filehandle = NULL;
  buffer_backup = NULL;
  last_recipe_ptr = NULL;
  restaurant_with_guests = NULL;
  restaurant_hit_test_owner = NULL;
  last_input_event = NULL;
  recipe_lookup_fallback = NULL;
  buffer_2 = NULL;
  ptr_apple = NULL;
  curr_rest = NULL;
  game_ptr = NULL;
  mouse_result = NULL;
  jatoba_1 = NULL;
  interactive_attack_attacker = NULL;
  interactive_attack_victim = NULL;
  recipe_rank_character = NULL;
  newspaper_article_buffer = NULL;
  memcpy(button_texts, initial_button_texts, sizeof(button_texts));
  for (size_t i = 0; i < sizeof(popup_menu_items) / sizeof(*popup_menu_items); ++i)
    popup_menu_items[i] = NULL;
}

// Resource objects.

// canvas objects

#include "render_types.h"
#include <stdbool.h>
PalettePointer pal_pointer;

Palette *pt_palette_create(void) {
  Palette *object = calloc(1, sizeof(*object));
  return object;
}

Canvas *Canvas_Init(Canvas *canvas) {
  canvas->bitmap = NULL;
  canvas->color = UINT16_MAX;
  canvas->background_color = 0;
  canvas->text_background_enabled = 1;
  canvas->font = NULL;
  canvas->actors = NULL;
  canvas->pattern = NULL;
  canvas->line_pattern = -1;
  canvas->position_y = 0;
  canvas->position_x = canvas->position_y;
  canvas->bitmap_mask_flag = 0;
  return canvas;
}

Jatoba *pt_jatoba_create(void) {
  typedef struct PTDisplayOwner {
    Jatoba owner;
    PalettePointer palette;
    Canvas canvas;
    Bitmap bitmap;
  } PTDisplayOwner;
  PTDisplayOwner *display = calloc(1, sizeof(*display));
  if (!display)
    return NULL;
  display->owner.pal_p = &display->palette;
  display->owner.canvas = &display->canvas;
  display->owner.bitmap = &display->bitmap;
  return &display->owner;
}

Canvas canvas1;
Juice some_juice;

Canvas *pt_canvas_create(void) {
  Canvas *object = calloc(1, sizeof(*object));
  return object;
}

Juice *pt_juice_create(void) {
  Juice *object = calloc(1, sizeof(*object));
  return object;
}
void Juice_Init(Juice *juice, Bitmap *bitmap) {
  juice->dword0 = 0;
  juice->bitmap = bitmap;
  juice->wordA = 0;
  juice->word8 = juice->wordA;
}

// font objects

#include "files.h"
LoadFontDesc menu_font;
LoadFontDesc ruby_font;
LoadFontDesc font_desc_cstandar;
LoadFontDesc font_desc_piznumb;
LoadFontDesc font_desc_astandar;
LoadFontDesc font_desc_supersma;

Font *pt_font_create(void) {
  Font *font = calloc(1, sizeof(*font));
  return font;
}
void pt_font_release(Font *font) {
  void *buffers[] = {font->advances, font->glyphs, font->bit_rows};
  for (unsigned i = 0; i < sizeof(buffers) / sizeof(*buffers); ++i)
    pt_buffer_release(buffers[i]);
  free(font);
}
bool pt_font_read_header(struct GuestFileStream *stream, Font *font) {
  // Font files contain three obsolete pointer slots. They are padding in
  // this asset header; native buffers are allocated after it is decoded.

  typedef struct __attribute__((packed)) PTFontFileHeader {
    u16 height;
    u8 field_2, spacing_mode;
    u16 fixed_advance;
    i16 baseline;
    u8 first_char, last_char, unused_rows[4];
    u16 row_stride;
    u8 unused_buffers[8];
  } PTFontFileHeader;
  _Static_assert(sizeof(PTFontFileHeader) == 24 && offsetof(PTFontFileHeader, row_stride) == 14, "font file header");
  PTFontFileHeader header = {.height = font->height,
    .field_2 = font->field_2,
    .spacing_mode = font->spacing_mode,
    .fixed_advance = font->fixed_advance,
    .baseline = font->baseline,
    .first_char = font->first_char,
    .last_char = font->last_char,
    .row_stride = font->row_stride};
  u32 count = ReadFile(stream, &header, sizeof(header));
  if (count > sizeof(header))
    abort();
  font->height = header.height;
  font->field_2 = header.field_2;
  font->spacing_mode = header.spacing_mode;
  font->fixed_advance = header.fixed_advance;
  font->baseline = header.baseline;
  font->first_char = header.first_char;
  font->last_char = header.last_char;
  font->row_stride = header.row_stride;
  // The pointer words in font files are discarded before buffer allocation.

  font->bit_rows = NULL;
  font->glyphs = NULL;
  font->advances = NULL;
  return count == sizeof(header);
}

void pt_native_font_defaults_apply(void) {
  menu_font = (LoadFontDesc){.filename = pt_interface_strings.file_menu_font};
  ruby_font = (LoadFontDesc){.filename = pt_interface_strings.file_ruby_font};
  font_desc_cstandar = (LoadFontDesc){.filename = pt_interface_strings.file_font_desc_cstandar};
  font_desc_piznumb = (LoadFontDesc){.filename = pt_interface_strings.file_font_desc_piznumb};
  font_desc_astandar = (LoadFontDesc){.filename = pt_interface_strings.file_font_desc_astandar};
  font_desc_supersma = (LoadFontDesc){.filename = pt_interface_strings.file_font_desc_supersma};
}

// world objects

#include <stdio.h>
// Native resource records own their fields; snapshots observe mutations.

Pig walking_guest, g_pigs[2];
MouseInitializer mouse_initializer;
Mouse *pt_mouse_create(void) {
  return pt_simulation_allocate(PT_SIM_MOUSE, sizeof(Mouse));
}

IglooLink temp_igloo_link;
IglooLink map_igloo_link;
Hazelnut hazelnuts[150];
Hazelnut temp_hazel;
LangFileDesc langfile_artikel3;
LangFileDesc langfile_credits;
LangFileDesc langfile_ende;
LangFileDesc langfile_gtalk0;
LangFileDesc langfile_gtalk1;
LangFileDesc langfile_markt;
LangFileDesc langfile_action;
LangFileDesc langfile_werbung;
LangFileDesc langfile_levent;
LangFileDesc langfile_mafia;
LangFileDesc langfile_waffen;
LangFileDesc langfile_realtors;
LangFileDesc langfile_bank;
LangFileDesc langfile_insurance;
LangFileDesc langfile_townhall;
LangFileDesc langfile_police;
LangFileDesc langfile_lief;
LangFileDesc langfile_leben;
LangFileDesc langfile_pers;
LangFileDesc langfile_persf;
LangFileDesc langfile_staff_applications[5];
LangFileDesc langfile_double;
Igloo igloos[150];
Igloo map_igloo_tail;
Igloo map_igloo_head;
Igloo temp_igloo_1;
Igloo temp_igloo_2;
Igloo temp_igloo;
BobHost bob_restaurant;
BobHost bob_ingredients;
BobHost bob_kitchen;
BobHost bob_restaurant_open;
BobHost bob_stanbobs;
BobHost bob_typ;
BobHost bob_walk;
BobHost bob7;
BobHost bob11;
BobHost menu_bob;
BobHost bob_restaurant_stats;
BobHost bob9;
BobHeader *pt_bob_headers_set(BobHost *owner, const BobHeader *value) {
  if (owner->owns_headers && owner->headers != value) {
    free(owner->headers);
    owner->owns_headers = false;
    owner->header_capacity = 0;
  }
  owner->headers = (BobHeader *)value;
  if (!value) {
    owner->header_source = NULL;
    owner->header_capacity = 0;
    owner->owns_headers = false;
  }

  return (BobHeader *)value;
}
int pt_bob_format_filename(BobHost *owner, const char *format, unsigned number) {
  int length = snprintf((char *)owner->filename, sizeof(owner->filename), format, number);
  if (length < 0 || (size_t)length >= sizeof(owner->filename))
    pt_resource_bounds_failure("resource filename", length, sizeof(owner->filename));

  return length;
}

_Noreturn void pt_resource_bounds_failure(const char *resource, i64 index, size_t count) {
  fprintf(stderr, "Pizza Tycoon: invalid %s index %lld (count %zu)\n", resource, (long long)index, count);
  abort();
}
BobHeader *pt_bob_decode_headers(BobHost *owner, u16 count) {
  typedef struct __attribute__((packed)) PTBobDiskHeader {
    u8 w, h, retained;
    u8 unused_pointer[4];
  } PTBobDiskHeader;
  _Static_assert(sizeof(PTBobDiskHeader) == 7, "sprite file record width");
  if (owner->size < 0 || 6u + (u32)count * sizeof(PTBobDiskHeader) > (u32)owner->size ||
      (count && !owner->header_source))
    pt_resource_bounds_failure(
        "sprite file headers", count, owner->size < 6 ? 0 : ((u32)owner->size - 6u) / sizeof(PTBobDiskHeader));
  // Size the native table from the file extent. A later load can reuse it
  // without relying on the placement of the serialized file allocation.

  size_t capacity = owner->size >= 6 ? ((size_t)owner->size - 6) / sizeof(PTBobDiskHeader) : 0;
  if (!capacity)
    capacity = 1;
  // Linked descriptors share one file buffer. Transfer the native table's
  // ownership along that ring instead of allocating a copy on each load.

  if (!owner->owns_headers && owner->data && owner->header_source) {
    for (BobHost *member = owner->next; member && member != owner; member = member->next) {
      if (!member->owns_headers || member->data != owner->data || member->header_source != owner->header_source ||
          member->header_capacity < capacity)
        continue;
      owner->headers = member->headers;
      owner->header_capacity = member->header_capacity;
      owner->owns_headers = true;
      member->owns_headers = false;
      break;
    }
  }
  if (!owner->owns_headers || owner->header_capacity < capacity) {
    if (owner->owns_headers) {
      free(owner->headers);
    }
    BobHeader *headers = calloc(capacity, sizeof(*headers));
    if (!headers)
      abort();
    owner->headers = headers;
    owner->header_capacity = capacity;
    owner->owns_headers = true;
  }
  const PTBobDiskHeader *disk = (const PTBobDiskHeader *)owner->header_source;
  for (u32 i = 0; i < count; ++i) {
    owner->headers[i].w = disk[i].w;
    owner->headers[i].h = disk[i].h;
    owner->headers[i].field_2 = disk[i].retained;
  }
  return owner->headers;
}
const BobHeader *pt_resource_header_at(const BobHost *owner, i32 index) {
  if (!owner->headers || index < 0 || (u32)index >= (u16)owner->num_entries || (u32)index >= owner->header_capacity)
    pt_resource_bounds_failure("sprite header", index, (u16)owner->num_entries);
  return &owner->headers[index];
}
const u8 *pt_resource_actor_availability(unsigned index) {
  if (index >= sizeof(igloos) / sizeof(*igloos))
    pt_resource_bounds_failure("actor availability", index, sizeof(igloos) / sizeof(*igloos));
  return &igloos[index].is_unused;
}

// Startup resource metadata is ordinary C state. This reset runs only after
// the previous resource owners have been released; disk loads keep their own
// imported values. Omitted members, including payload pointers, start at zero.

void pt_bob_headers_storage_reset(void) {
  BobHost *owners[] = {&bob_restaurant,
    &bob_ingredients,
    &bob_kitchen,
    &bob_restaurant_open,
    &bob_stanbobs,
    &bob_typ,
    &bob_walk,
    &bob7,
    &bob11,
    &menu_bob,
    &bob_restaurant_stats,
    &bob9};
  for (unsigned i = 0; i < sizeof(owners) / sizeof(*owners); ++i)
    pt_bob_headers_set(owners[i], NULL);
}

void pt_native_resource_defaults_apply(void) {
  pt_bob_headers_storage_reset();
  bob_restaurant = (BobHost){.size = 225000, .filename = "bob\\moebel.bob"};
  bob_ingredients = (BobHost){.size = 251000, .filename = "bob\\zutaten.bob"};
  bob_kitchen = (BobHost){.size = 40000, .filename = "bob\\kuebobs.bob"};
  bob_restaurant_open = (BobHost){.size = 40000, .filename = "bob\\gaeste.bob"};
  bob_stanbobs = (BobHost){.size = 7000, .flags = 1, .filename = "bob\\stanbobs.bob"};
  bob_typ = (BobHost){.size = 30000, .filename = "bob\\typ99.bob"};
  bob_walk = (BobHost){.size = 11000, .flags = 2, .filename = "bob\\walk.bob"};
  bob7 = (BobHost){.size = 38000, .filename = "bob\\nightp.bob"};
  bob11 = (BobHost){.size = 26000, .flags = 2, .filename = "bob\\nightw0.bob"};
  menu_bob = (BobHost){.size = 17000, .filename = "bob\\menubobs.bob"};
  bob_restaurant_stats = (BobHost){.size = 6000, .filename = "bob\\stabo.bob"};
  bob9 = (BobHost){.size = 24000, .filename = "bob\\lebbo.bob"};
  langfile_artikel3 =
      (LangFileDesc){.field_1 = 1, .size = 1300, .name = {'a', 'r', 't', 'i', 'k', 'e', 'l', '3', '.', 'e'}};
  langfile_credits = (LangFileDesc){.field_1 = 1, .size = 1300, .name = "credits.e"};
  langfile_ende = (LangFileDesc){.field_1 = 1, .size = 5000, .name = "ende.e"};
  langfile_gtalk0 = (LangFileDesc){.field_1 = 2, .size = 2500, .name = "gtalk0.e"};
  langfile_gtalk1 = (LangFileDesc){.field_1 = 2, .size = 3000, .name = "gtalk1.e"};
  langfile_markt = (LangFileDesc){.field_1 = 2, .size = 2500, .name = "markt.e"};
  langfile_action = (LangFileDesc){.field_1 = 3, .size = 1000, .name = "action.e"};
  langfile_werbung = (LangFileDesc){.field_1 = 3, .size = 2500, .name = "werbung.e"};
  langfile_levent = (LangFileDesc){.field_1 = 3, .size = 7500, .name = "levent.e"};
  langfile_mafia = (LangFileDesc){.size = 5000, .name = "mafia.e"};
  langfile_waffen = (LangFileDesc){.size = 1600, .name = "waffen.e"};
  langfile_realtors = (LangFileDesc){.size = 3100, .name = "makler.e"};
  langfile_bank = (LangFileDesc){.size = 2500, .name = "bank.e"};
  langfile_insurance = (LangFileDesc){.size = 5200, .name = "versich.e"};
  langfile_townhall = (LangFileDesc){.size = 5100, .name = "bg.e"};
  langfile_police = (LangFileDesc){.size = 2600, .name = "polizei.e"};
  langfile_lief = (LangFileDesc){.size = 4200, .name = "lief.e"};
  langfile_leben = (LangFileDesc){.size = 3600, .name = "leben.e"};
  langfile_pers = (LangFileDesc){.size = 4300, .name = "pers.e"};
  langfile_persf = (LangFileDesc){.size = 6200, .name = "persf.e"};
  langfile_staff_applications[0] = (LangFileDesc){.size = 5000, .name = "bewerb0.e"};
  langfile_staff_applications[1] = (LangFileDesc){.size = 5000, .name = "bewerb1.e"};
  langfile_staff_applications[2] = (LangFileDesc){.size = 5000, .name = "bewerb2.e"};
  langfile_staff_applications[3] = (LangFileDesc){.size = 5000, .name = "bewerb3.e"};
  langfile_staff_applications[4] = (LangFileDesc){.size = 5800, .name = "bewerb4.e"};
  langfile_double = (LangFileDesc){.size = 1500, .name = "double.e"};
}

// Static actors and list heads start empty; the mouse initializer is enabled.

void pt_native_world_defaults_apply(void) {
  mouse_initializer = (MouseInitializer){.field_0 = 1};
  walking_guest = (Pig){0};
  for (size_t i = 0; i < sizeof(g_pigs) / sizeof(*g_pigs); ++i)
    g_pigs[i] = (Pig){0};
  temp_igloo_link = (IglooLink){0};
  map_igloo_link = (IglooLink){0};
  for (size_t i = 0; i < sizeof(hazelnuts) / sizeof(*hazelnuts); ++i)
    hazelnuts[i] = (Hazelnut){0};
  temp_hazel = (Hazelnut){0};
  for (size_t i = 0; i < sizeof(igloos) / sizeof(*igloos); ++i)
    igloos[i] = (Igloo){0};
  map_igloo_tail = (Igloo){0};
  map_igloo_head = (Igloo){0};
  temp_igloo_1 = (Igloo){0};
  temp_igloo_2 = (Igloo){0};
  temp_igloo = (Igloo){0};
}

// placement state

static PTPlacementContact contacts[256];
static size_t row(i32 index) {
  if (index < INT8_MIN || index > INT8_MAX)
    abort();
  return (size_t)(index - INT8_MIN);
}

void pt_placement_contacts_reset(void) {
  memset(contacts, 0, sizeof(contacts));
}
PTPlacementContact *pt_placement_contact_state(i32 index) {
  return &contacts[row(index)];
}

// palette

#include "graphics.h"

// palette state

PalettePointer *active_palette_pointer;
PTPaletteSlot temp_palette_slot, some_pal_p;

void pt_palette_state_reset(void) {
  active_palette_pointer = NULL;
  temp_palette_slot = (PTPaletteSlot){0};
  some_pal_p = (PTPaletteSlot){0};
}
void pt_palette_set_active(PalettePointer *pointer) {
  active_palette_pointer = pointer;
}

static void require_game_palette_index(unsigned index) {
  if (index >= 2) {
    fprintf(stderr, "Pizza Tycoon: invalid Game palette slot index %u (count 2)\n", index);
    abort();
  }
}
PTPaletteSlot *pt_game_palette_slot(const Game *owner, unsigned index) {
  require_game_palette_index(index);
  return owner->palette_slots[index];
}
void pt_game_bind_palette_slot(Game *owner, unsigned index, PTPaletteSlot *slot) {
  require_game_palette_index(index);
  owner->palette_slots[index] = slot;
}

// palette color

// Palette indices wrap to one byte.

void SetPalColor(i16 index, u8 red, u8 green, u8 blue) {
  const PTRGB color = {red, green, blue};
  pt_framebuffer_palette((u8)index, &color, sizeof(color));
}
// Signed palette indices and scaled components retain byte wrapping.
// This operation does not mark the palette dirty.

void SetPalColor2(PalettePointer *pointer, i16 index, u8 red, u8 green, u8 blue) {
  PTRGB *color = pt_palette_color_at(pointer->palette, index);
  red = (u8)(4u * red);
  green = (u8)(4u * green);
  blue = (u8)(4u * blue);
  color->red = red;
  color->green = green;
  color->blue = blue;

  SetPalColor(index, red, green, blue);
}
// Read the second descriptor only after updating the active palette.

void SetPalColor3(i16 index, u8 red, u8 green, u8 blue) {
  SetPalColor2(active_palette_pointer, index, red, green, blue);

  SetPalColor2(&pal_pointer, index, red, green, blue);
}

// Resource views.

#include "sound_layout.h"

// data views

// Native pointer views; diagnostic range checks are generated separately.

Loan *pt_player_loan(u8 player, u8 index) {
  if (player >= 8)
    pt_resource_bounds_failure("player", player, 8);
  if (index >= 4)
    pt_resource_bounds_failure("bank account slot", index, 4);
  return &player_loans[player][index];
}

Investment *pt_player_investment(u8 player, u8 index) {
  if (player >= 8)
    pt_resource_bounds_failure("player", player, 8);
  if (index >= 4)
    pt_resource_bounds_failure("bank account slot", index, 4);
  return &player_investments[player][index];
}

u8 pt_map_tile_read(u32 column, u32 row) {
  u32 offset = (u32)sizeof(loaded_map_file[0]) * row + column;
  if (offset >= sizeof(map_tiles))
    pt_resource_bounds_failure("map tile", pt_signed_dword(offset), sizeof(map_tiles));
  return map_tiles[offset];
}

u8 pt_map_tile_read_signed(i32 column, i32 row) {
  i64 offset = (i64)sizeof(loaded_map_file[0]) * row + column;
  if (offset < 0 || (u64)offset >= sizeof(map_tiles))
    pt_resource_bounds_failure("map tile", offset, sizeof(map_tiles));
  return map_tiles[offset];
}

u8 pt_supplier_ingredient_count(u8 kind) {
  if (kind >= sizeof(num_ingredients_in_set))
    pt_resource_bounds_failure("ingredient set", kind, sizeof(num_ingredients_in_set));

  return num_ingredients_in_set[kind];
}

i8 pt_supplier_ingredient_trend(u8 ingredient) {
  const Town *town = current_town;
  const u8 *trend;
  // The four nonfood supplier kinds use the second, adjacent trend table
  // in the original record. Select that named field explicitly.

  if (ingredient < sizeof(town->ingredient_trend))
    trend = (const u8 *)&town->ingredient_trend[ingredient];
  else if (ingredient < sizeof(town->ingredient_trend) + sizeof(town->stockarr90A))
    trend = (const u8 *)&town->stockarr90A[ingredient - sizeof(town->ingredient_trend)];
  else
    pt_resource_bounds_failure(
        "supplier trend", ingredient, sizeof(town->ingredient_trend) + sizeof(town->stockarr90A));
  return (i8)*trend;
}

// object views

// Native pointer views; diagnostic range checks are generated separately.

const PTRGB *pt_map_animation_colors(u16 phase) {
  if (phase >= 4)
    pt_resource_bounds_failure("map palette phase", phase, 4);
  return &map_animation_colors[phase];
}

u16 pt_furniture_image(const FurnitureDesc *descriptor, i16 orientation) {
  if ((u64)orientation >= 4)
    pt_resource_bounds_failure("furniture orientation", orientation, 4);
  return descriptor->images[orientation];
}

bool pt_furniture_van_amount(i16 row, u8 *amount) {
  if (row < 0 || (size_t)row >= sizeof(furniture_desc) / sizeof(*furniture_desc))
    return false;
  *amount = furniture_desc[row].amount_in_van;
  return true;
}

const u8 *pt_furniture_van_quality(i16 index) {
  if ((u64)index >= sizeof(furniture_quality_in_van))
    pt_resource_bounds_failure("furniture van quality", index, sizeof(furniture_quality_in_van));
  return &furniture_quality_in_van[index];
}

PTAgeTastePreference *pt_age_taste(unsigned age) {
  if (age >= sizeof(age_taste_preferences) / sizeof(*age_taste_preferences))
    pt_resource_bounds_failure("customer age", age, sizeof(age_taste_preferences) / sizeof(*age_taste_preferences));
  return &age_taste_preferences[age];
}

u8 pt_calendar_month_days(i8 month) {
  if ((u64)month >= sizeof(days_in_month))
    pt_resource_bounds_failure("calendar month", month, sizeof(days_in_month));

  return days_in_month[(u8)month];
}

const PTMissionReceiptLedger *pt_monthly_receipts(u8 player) {
  if (player >= 8)
    pt_resource_bounds_failure("player", player, 8);
  return &previous_month_receipts[player];
}

const PTMissionExpenseLedger *pt_monthly_expenses(u8 player) {
  if (player >= 8)
    pt_resource_bounds_failure("player", player, 8);
  return &previous_month_expenses[player];
}

const RecipeEntry *pt_recipe_entry(const Recipe *recipe, unsigned index) {
  if (index >= recipe->entry_capacity)
    pt_resource_bounds_failure("recipe entries", index, recipe->entry_capacity);
  return &recipe->entries[index];
}

u8 pt_ingredient_amount(u8 ingredient, u8 variation) {
  if (variation >= 3)
    pt_resource_bounds_failure("ingredient amount variation", variation, 3);
  const Ingredient *record = pt_ingredient_record(ingredient);
  return record->amount[variation];
}

u16 pt_recipe_weight(const Recipe *recipe, unsigned ingredient) {
  if (ingredient >= sizeof(recipe->weight_of_type) / sizeof(*recipe->weight_of_type))
    pt_resource_bounds_failure(
        "ingredient weight", ingredient, sizeof(recipe->weight_of_type) / sizeof(*recipe->weight_of_type));

  return recipe->weight_of_type[ingredient];
}

const BobHeader *pt_bob_signed_header(const BobHost *bob, u16 image) {
  return pt_resource_header_at(bob, (i16)image);
}

const u8 *pt_map_igloo_availability(unsigned index) {
  return pt_resource_actor_availability(index);
}

Guest *pt_guest_at(unsigned index) {
  if (index >= sizeof(guests) / sizeof(*guests))
    pt_resource_bounds_failure("customer", index, sizeof(guests) / sizeof(*guests));

  return &guests[index];
}

u8 pt_rest_floor_cell(u8 row, u8 column) {
  if (row >= sizeof(rest_floormap) / sizeof(*rest_floormap))
    pt_resource_bounds_failure("restaurant map row", row, sizeof(rest_floormap) / sizeof(*rest_floormap));
  if (column >= sizeof(rest_floormap[0]))
    pt_resource_bounds_failure("restaurant map column", column, sizeof(rest_floormap[0]));

  return rest_floormap[row][column];
}

const PTManagerRoleText *pt_manager_role_text(u8 role) {
  if (!role || role > sizeof(manager_role_names) / sizeof(*manager_role_names))
    pt_resource_bounds_failure("manager role", role, sizeof(manager_role_names) / sizeof(*manager_role_names));
  return (const PTManagerRoleText *)manager_role_names[role - 1u];
}

u8 pt_furniture_beauty(const FurnitureDesc *descriptor, u8 age) {
  if (age >= sizeof(descriptor->beautiness))
    pt_resource_bounds_failure("customer age", age, sizeof(descriptor->beautiness));

  return descriptor->beautiness[age];
}

u8 pt_interactive_player_count(void) {
  return game_ptr->num_interactive_players;
}

u16 pt_town_stock_price(unsigned set, unsigned ingredient) {
  if (set >= 4)
    pt_resource_bounds_failure("ingredient price set", set, 4);
  if (ingredient >= 24)
    pt_resource_bounds_failure("ingredient price", ingredient, 24);

  return current_town->base_stock_prices[set][ingredient];
}

Staff *pt_rest_staff_member(const Rest *rest, u32 slot) {
  if (slot >= sizeof(rest->staff_indexes))
    pt_resource_bounds_failure("restaurant staff slot", slot, sizeof(rest->staff_indexes));

  const u8 *index = &rest->staff_indexes[slot];
  if (*index >= sizeof(g_staff) / sizeof(*g_staff))
    pt_resource_bounds_failure("staff member", *index, sizeof(g_staff) / sizeof(*g_staff));
  return &g_staff[*index];
}

const PTGoalSceneStyle *pt_goal_scene_style(u8 bitmap) {
  if (bitmap >= sizeof(goal_bitmap_flags) / sizeof(*goal_bitmap_flags))
    pt_resource_bounds_failure("goal scene", bitmap, sizeof(goal_bitmap_flags) / sizeof(*goal_bitmap_flags));
  return &goal_scene_styles[bitmap];
}

u8 *pt_next_language_text(u8 *text) {
  for (;;) {
    u8 value = *text;
    ++text;
    if (!value)
      return text;
  }
}

u8 *pt_character_property(Character *character, u16 index) {
  if (index >= sizeof(character->prop))
    pt_resource_bounds_failure("character property", index, sizeof(character->prop));

  return &character->prop[index];
}

const Ingredient *pt_ingredient_record(u32 index) {
  if (index >= (4u * 24u))
    pt_resource_bounds_failure("ingredient", index, 4u * 24u);

  return &ingredients[index / 24u].vegs[index % 24u];
}

u8 pt_player_restaurant_count(u32 index) {
  if (index >= sizeof(players) / sizeof(*players))
    pt_resource_bounds_failure("player", index, sizeof(players) / sizeof(*players));

  return players[index].num_restaurants;
}

u8 pt_customer_population_factor(u8 group, u8 age) {
  if (group >= sizeof(customer_types) / sizeof(*customer_types))
    pt_resource_bounds_failure("customer group", group, sizeof(customer_types) / sizeof(*customer_types));
  if (age >= sizeof(customer_types[0]))
    pt_resource_bounds_failure("customer age", age, sizeof(customer_types[0]));

  return customer_types[group][age];
}

const u8 *pt_player_announcement_name(u8 player, size_t capacity) {
  if (player >= sizeof(players) / sizeof(*players))
    pt_resource_bounds_failure("player", player, sizeof(players) / sizeof(*players));

  (void)capacity;
  return players[player].name_text;
}

bool pt_music_is_active(void) {
  if (!pt_sound_resources.fm_status)
    pt_resource_bounds_failure("music status", 0, 0);
  return *(pt_sound_resources.fm_status) != 0;
}

const u8 *pt_town_name(u8 index) {
  if (index >= sizeof(town_names) / sizeof(*town_names))
    pt_resource_bounds_failure("town_name", index, sizeof(town_names) / sizeof(*town_names));
  return town_names[index];
}

const u8 *pt_finance_category_name(u8 index) {
  if (index >= sizeof(finance_stats_categories) / sizeof(*finance_stats_categories))
    pt_resource_bounds_failure(
        "finance_category_name", index, sizeof(finance_stats_categories) / sizeof(*finance_stats_categories));
  return finance_stats_categories[index];
}

const u8 *pt_insurance_category_name(u8 index) {
  if (index >= sizeof(insurance_category_labels) / sizeof(*insurance_category_labels))
    pt_resource_bounds_failure(
        "insurance_category_name", index, sizeof(insurance_category_labels) / sizeof(*insurance_category_labels));
  return insurance_category_labels[index];
}

const u8 *pt_terminal_category_name(u8 index) {
  if (index >= sizeof(terminal_category_names) / sizeof(*terminal_category_names))
    pt_resource_bounds_failure(
        "terminal_category_name", index, sizeof(terminal_category_names) / sizeof(*terminal_category_names));
  return terminal_category_names[index];
}

// sound views

// Native pointer views; diagnostic range checks are generated separately.

u8 *pt_fm_status_byte(const PTFMResident *driver) {
  PTFMBuffer buffer = driver->status;
  if (!buffer.base)
    abort();
  const u8 *pointer = pt_fm_stream_bytes(buffer, buffer.offset, 1);
  return (u8 *)pointer;
}

const u8 *pt_fm_stream_bytes(PTFMBuffer buffer, u16 offset, size_t size) {
  if (offset > buffer.capacity || size > buffer.capacity - offset) {
    fputs("Pizza Tycoon: FM read exceeds its native buffer\n", stderr);
    abort();
  }
  if (!buffer.base) {
    if (offset || size) {
      fprintf(stderr, "Missing native FM stream storage\n");
      abort();
    }
    return NULL;
  }
  return buffer.base + offset;
}

PTSoundDMATransfer *pt_current_sound_dma_transfer(void) {
  if (!pt_sound_resources.dma_record) {
    fprintf(stderr, "Missing native PCM transfer\n");
    abort();
  }
  return pt_sound_resources.dma_record;
}
const PTSoundBank *pt_loaded_sound_bank(void) {
  if (!pt_sound_resources.bank)
    pt_resource_bounds_failure("bank storage", 0, 0);
  return (const PTSoundBank *)pt_sound_resources.bank;
}

// selector views

// Native pointer views; diagnostic range checks are generated separately.

u8 *pt_selector_extension_end(u32 length) {
  if (length < 4 || length - 4 >= sizeof(selected_filename))
    pt_resource_bounds_failure("filename extension", length, sizeof(selected_filename) + 4);
  return &selected_filename[length - 4];
}
