// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
#ifndef PT_INITIAL_DATA_H
#define PT_INITIAL_DATA_H
// Typed C defaults for the native game, independent of saved memory.

void pt_native_initial_data_apply(void);
// Fixed sprite/language descriptors for fresh native startup, after cleanup.

void pt_native_resource_defaults_apply(void);
// Static drawing objects and the borrowed native glyph buffer.

void pt_native_graphics_defaults_apply(void);
// Standard FILE records and their native mode slots.

void pt_native_file_defaults_apply(void);
void pt_native_process_defaults_apply(void);
// Empty audio resources and a native initial music-status byte.

void pt_native_sound_defaults_apply(void);
// Main game, static simulation records, actor pools and list heads.

void pt_native_game_defaults_apply(void);
void pt_native_world_defaults_apply(void);
// Save-image startup also initializes native defaults for existing loaders.

void pt_initial_data_apply(void);
// Immutable startup filenames and hints; button pointers may later change.

#include "types.h"
enum { PT_INTERFACE_TEXT_CAPACITY = 32 };
typedef struct PTInterfaceStrings {
  u8 file_menu_font[PT_INTERFACE_TEXT_CAPACITY];
  u8 file_ruby_font[PT_INTERFACE_TEXT_CAPACITY];
  u8 file_font_desc_cstandar[PT_INTERFACE_TEXT_CAPACITY];
  u8 file_font_desc_piznumb[PT_INTERFACE_TEXT_CAPACITY];
  u8 file_topaz[PT_INTERFACE_TEXT_CAPACITY];
  u8 file_font_desc_astandar[PT_INTERFACE_TEXT_CAPACITY];
  u8 file_font_desc_supersma[PT_INTERFACE_TEXT_CAPACITY];
  u8 empty_unit[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_map[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_character[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_disk_options[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_advertising[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_market_research[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_newspaper[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_global_stats[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_terminal[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_end_turn[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_open_close[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_stock[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_menu[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_furnishing[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_staff[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_recipe_list[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_restaurant_stats[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_animated_stats[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_vegetables[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_meat[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_contracts[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_fruit[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_cheese[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_shop[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_back[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_reject[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_agree[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_furniture[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_kitchenware[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_customer_ratings[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_extras[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_flooring[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_weapons[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_single_order[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_make_contract[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_turn_page[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_pizza_sales[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_remove_pizza[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_view_pizza[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_ingredients[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_pizza_info[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_o_k[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_flip_page[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_cancel_contract[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_chefs[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_waiters[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_managers[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_fire[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_wages[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_training[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_recruit[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_production[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_reputation[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_capacity[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_pizza_quality[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_popularity[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_turnover[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_data[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_applications[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_hire[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_buy[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_dial_1[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_dial_2[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_dial_3[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_dial_4[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_dial_5[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_dial_6[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_dial_7[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_dial_8[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_dial_9[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_character_values[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_company[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_curriculum_vitae[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_position[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_awards[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_target_cv[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_challenge[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_sabotage[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_transfer[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_bake_pizza[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_export[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_priorities[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_sell[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_next_item[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_throw_away[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_previous_item[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_month_overview[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_restaurants[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_cash_flow[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_loans[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_insurance[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_property[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_balance[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_daily_profits[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_daily_balance[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_week_review[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_month_review[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_week_s_takings[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_customer_stats[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_sales_curve[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_area_info[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_economy[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_customer_taste[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_trends[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_pizza_charts[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_competitors[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_market_share[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_continue[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_move_elsewhere[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_sell_it[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_dial_new_number[PT_INTERFACE_TEXT_CAPACITY];
  u8 hint_view_ads[PT_INTERFACE_TEXT_CAPACITY];
} PTInterfaceStrings;
extern PTInterfaceStrings pt_interface_strings;
void pt_native_font_defaults_apply(void);
void pt_native_game_root_defaults_apply(void);
void pt_native_graphics_root_defaults_apply(void);

#include <stdbool.h>
bool pt_native_defaults_load(const char *directory);
u32 pt_public_heap_limit(void);
void pt_native_audio_start(void);

#endif
