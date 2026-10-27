// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
// Defaults are read from the user's verified executable, never compiled into the game.
#include "initial_data.h"
#include "native_globals.h"
#include "game_data.h"
#include <stdlib.h>
#include <string.h>
static PTNativeGlobals initial_native_globals;
PTReadOnlyTables pt_read_only_tables;
typedef struct PTDefaultSlice {
  u16 page, offset, size;
  void *destination;
} PTDefaultSlice;
// clang-format off
static const PTDefaultSlice default_slices[] = {
  { 106, 176, 8, (u8 *) &initial_native_globals.restaurant_mask_rect + 0 },
  { 103, 1092, 2, (u8 *) &initial_native_globals.cursor_hide_depth + 0 },
  { 103, 2053, 4, (u8 *) &initial_native_globals.input_queue_capacity + 0 },
  { 103, 2732, 76, (u8 *) &initial_native_globals.smallrects + 0 },
  { 103, 3541, 1579, (u8 *) &initial_native_globals.furniture_desc + 0 },
  { 104, 1024, 4096, (u8 *) &initial_native_globals.furniture_desc + 1579 },
  { 105, 1024, 1541, (u8 *) &initial_native_globals.furniture_desc + 5675 },
  { 105, 2565, 1, (u8 *) &initial_native_globals.furniture_supplier_count + 0 },
  { 105, 2566, 1584, (u8 *) &initial_native_globals.furniture_suppliers + 0 },
  { 106, 59, 13, (u8 *) &initial_native_globals.days_in_month + 0 },
  { 106, 1472, 12, (u8 *) &initial_native_globals.prefs + 0 },
  { 106, 1524, 1, (u8 *) &initial_native_globals.state_slot_007 + 0 },
  { 106, 3789, 1331, (u8 *) &initial_native_globals.ingredients + 0 },
  { 107, 1024, 4096, (u8 *) &initial_native_globals.ingredients + 1331 },
  { 108, 1024, 45, (u8 *) &initial_native_globals.ingredients + 5427 },
  { 108, 1069, 72, (u8 *) &initial_native_globals.age_taste_preferences + 0 },
  { 108, 1998, 4, (u8 *) &initial_native_globals.goal_ui_colors + 0 },
  { 108, 2881, 30, (u8 *) &initial_native_globals.depot_capacity_exhausted_text + 0 },
  { 108, 2977, 11, (u8 *) &initial_native_globals.translation_error_text + 0 },
  { 108, 2988, 121, (u8 *) &initial_native_globals.mouse_bitmap_data_1 + 0 },
  { 108, 3733, 1, (u8 *) &initial_native_globals.pizza_level + 0 },
  { 109, 2095, 1, (u8 *) &initial_native_globals.map_marker_toggle + 0 },
  { 109, 2096, 12, (u8 *) &initial_native_globals.map_tabs_order + 0 },
  { 109, 2126, 2, (u8 *) &initial_native_globals.map_tab_step + 0 },
  { 109, 2128, 2, (u8 *) &initial_native_globals.map_tab_base + 0 },
  { 109, 2492, 320, (u8 *) &initial_native_globals.terrain_names + 0 },
  { 109, 3032, 1, (u8 *) &initial_native_globals.palette_in_callbackground + 0 },
  { 109, 3033, 1, (u8 *) &initial_native_globals.last_loaded_bob_typ + 0 },
  { 110, 1129, 1, (u8 *) &initial_native_globals.sound_config_flag + 0 },
  { 110, 1130, 1, (u8 *) &initial_native_globals.sound_var_3 + 0 },
  { 110, 1147, 1, (u8 *) &initial_native_globals.current_music_index + 0 },
  { 110, 1149, 1, (u8 *) &initial_native_globals.competition_sound_id + 0 },
  { 110, 2164, 4, (u8 *) &initial_native_globals.pt_random_state + 0 },
  { 110, 2877, 4, (u8 *) &initial_native_globals.guest_default_open_mode + 0 },
  { 110, 2944, 2, (u8 *) &initial_native_globals.sound_timer_period + 0 },
  { 110, 2956, 4, (u8 *) &initial_native_globals.guest_io_mode_capacity + 0 },
  { 110, 3048, 36, (u8 *) &initial_native_globals.timezone_start_rule + 0 },
  { 110, 3084, 36, (u8 *) &initial_native_globals.timezone_end_rule + 0 },
  { 110, 3120, 4, (u8 *) &initial_native_globals.timezone_seconds + 0 },
  { 110, 3124, 4, (u8 *) &initial_native_globals.timezone_dst_seconds + 0 },
  { 110, 3128, 4, (u8 *) &initial_native_globals.timezone_has_dst + 0 },
  { 110, 3132, 31, (u8 *) &initial_native_globals.timezone_standard_name + 0 },
  { 110, 3163, 31, (u8 *) &initial_native_globals.timezone_dst_name + 0 },
  { 110, 3352, 4, (u8 *) &initial_native_globals.guest_heap_grow_enabled + 0 },
  { 111, 791, 21, (u8 *) &initial_native_globals.recipe_allocation_failure_text + 0 },
  { 111, 975, 20, (u8 *) &initial_native_globals.restaurant_not_found_text + 0 },
  { 111, 1117, 8, (u8 *) &initial_native_globals.staff_disaster_raise_factor + 0 },
  { 111, 1293, 20, (u8 *) &initial_native_globals.staff_course_confirm_prompt + 0 },
  { 111, 1485, 14, (u8 *) &initial_native_globals.staff_pool_full_error + 0 },
  { 111, 1499, 27, (u8 *) &initial_native_globals.staff_hire_none_error + 0 },
  { 111, 1559, 43, (u8 *) &initial_native_globals.staff_hire_refusal_error + 0 },
  { 111, 1669, 8, (u8 *) &initial_native_globals.staff_expenses_underpaid_factor + 0 },
  { 111, 1677, 8, (u8 *) &initial_native_globals.staff_expenses_overpaid_factor + 0 },
  { 111, 1769, 8, (u8 *) &initial_native_globals.staff_daily_underpaid_factor + 0 },
  { 111, 1777, 8, (u8 *) &initial_native_globals.staff_daily_overpaid_factor + 0 },
  { 111, 1926, 53, (u8 *) &initial_native_globals.staff_fire_selection_error + 0 },
  { 111, 2024, 37, (u8 *) &initial_native_globals.staff_fire_prompt + 0 },
  { 111, 2061, 63, (u8 *) &initial_native_globals.staff_wages_selection_error + 0 },
  { 111, 2141, 65, (u8 *) &initial_native_globals.staff_course_selection_error + 0 },
  { 111, 2840, 37, (u8 *) &initial_native_globals.character_holiday_question + 0 },
  { 111, 3059, 82, (u8 *) &initial_native_globals.depot_quality_rejection_text + 0 },
  { 111, 3141, 8, (u8 *) &initial_native_globals.depot_trade_price_divisor + 0 },
  { 111, 3149, 4, (u8 *) &initial_native_globals.depot_trade_rating_divisor + 0 },
  { 111, 3214, 8, (u8 *) &initial_native_globals.contract_price_divisor + 0 },
  { 111, 3244, 8, (u8 *) &initial_native_globals.contract_purchase_divisor + 0 },
  { 111, 3372, 14, (u8 *) &initial_native_globals.contract_empty_error + 0 },
  { 111, 3396, 101, (u8 *) &initial_native_globals.contract_cancel_prompt + 0 },
  { 111, 3560, 8, (u8 *) &initial_native_globals.shop_price_divisor + 0 },
  { 111, 3946, 8, (u8 *) &initial_native_globals.stock_menu_demand_divisor + 0 },
  { 111, 3954, 8, (u8 *) &initial_native_globals.supplier_price_addon_divisor + 0 },
  { 111, 3962, 8, (u8 *) &initial_native_globals.supplier_price_level5_factor + 0 },
  { 111, 3970, 8, (u8 *) &initial_native_globals.supplier_price_level4_factor + 0 },
  { 111, 3978, 8, (u8 *) &initial_native_globals.supplier_price_level2_factor + 0 },
  { 111, 3986, 8, (u8 *) &initial_native_globals.supplier_price_level1_factor + 0 },
  { 112, 4, 4, (u8 *) &initial_native_globals.contract_penalty_new_factor + 0 },
  { 112, 8, 4, (u8 *) &initial_native_globals.contract_discount_factor + 0 },
  { 112, 12, 4, (u8 *) &initial_native_globals.contract_discount_divisor + 0 },
  { 112, 16, 4, (u8 *) &initial_native_globals.contract_charge_divisor + 0 },
  { 112, 20, 4, (u8 *) &initial_native_globals.contract_fake_sum_factor + 0 },
  { 112, 24, 4, (u8 *) &initial_native_globals.contract_penalty_reduction_factor + 0 },
  { 112, 304, 8, (u8 *) &initial_native_globals.double_720 + 0 },
  { 112, 335, 4, (u8 *) &initial_native_globals.customer_other_tile_factor + 0 },
  { 112, 339, 4, (u8 *) &initial_native_globals.customer_residential_tile_factor + 0 },
  { 112, 343, 4, (u8 *) &initial_native_globals.float_720 + 0 },
  { 112, 347, 8, (u8 *) &initial_native_globals.pizza_income_price_divisor + 0 },
  { 112, 355, 8, (u8 *) &initial_native_globals.pizza_income_price_offset + 0 },
  { 112, 363, 8, (u8 *) &initial_native_globals.pizza_income_base_divisor + 0 },
  { 112, 371, 8, (u8 *) &initial_native_globals.pizza_income_base_offset + 0 },
  { 112, 379, 8, (u8 *) &initial_native_globals.pizza_price_offset + 0 },
  { 112, 387, 8, (u8 *) &initial_native_globals.pizza_price_divisor + 0 },
  { 112, 395, 8, (u8 *) &initial_native_globals.pizza_price_bonus_scale + 0 },
  { 112, 403, 4, (u8 *) &initial_native_globals.customer_distribution_factor + 0 },
  { 112, 459, 4, (u8 *) &initial_native_globals.pizza_order_random_threshold + 0 },
  { 112, 463, 50, (u8 *) &initial_native_globals.restaurant_supplies_shortage_text + 0 },
  { 112, 515, 40, (u8 *) &initial_native_globals.restaurant_ovens_shortage_text + 0 },
  { 112, 556, 40, (u8 *) &initial_native_globals.restaurant_cooks_shortage_text + 0 },
  { 112, 597, 42, (u8 *) &initial_native_globals.restaurant_waiters_shortage_text + 0 },
  { 112, 640, 8, (u8 *) &initial_native_globals.restaurant_supply_percentage_scale + 0 },
  { 112, 706, 4, (u8 *) &initial_native_globals.restaurant_turnover_goal_threshold + 0 },
  { 112, 710, 8, (u8 *) &initial_native_globals.restaurant_sales_currency_divisor + 0 },
  { 112, 980, 15, (u8 *) &initial_native_globals.attack_police_message + 0 },
  { 112, 1012, 10, (u8 *) &initial_native_globals.guest_type_error_text + 0 },
  { 112, 1236, 8, (u8 *) &initial_native_globals.ai_restaurant_upgrade_money_factor + 0 },
  { 112, 1285, 40, (u8 *) &initial_native_globals.ai_restaurant_memory_error + 0 },
  { 112, 1325, 8, (u8 *) &initial_native_globals.ai_low_rating_money_threshold + 0 },
  { 112, 1333, 8, (u8 *) &initial_native_globals.ai_low_default_rating_money_threshold + 0 },
  { 112, 1341, 8, (u8 *) &initial_native_globals.legacy_scale_word_byte_divisor + 0 },
  { 112, 1349, 8, (u8 *) &initial_native_globals.legacy_scale_difference_byte_divisor + 0 },
  { 112, 1357, 8, (u8 *) &initial_native_globals.legacy_scale_word_word_divisor + 0 },
  { 112, 1365, 8, (u8 *) &initial_native_globals.legacy_scale_difference_word_divisor + 0 },
  { 112, 1407, 94, (u8 *) &initial_native_globals.competition_challenge_format + 0 },
  { 112, 1501, 52, (u8 *) &initial_native_globals.competition_ai_challenge_text + 0 },
  { 112, 1577, 40, (u8 *) &initial_native_globals.competition_decline_text + 0 },
  { 112, 1723, 11, (u8 *) &initial_native_globals.competition_draw_caption + 0 },
  { 112, 1734, 5, (u8 *) &initial_native_globals.competition_win_caption + 0 },
  { 112, 1739, 8, (u8 *) &initial_native_globals.competition_minimum_money + 0 },
  { 112, 1747, 8, (u8 *) &initial_native_globals.competition_maximum_bet + 0 },
  { 112, 1755, 8, (u8 *) &initial_native_globals.competition_ai_money_fraction + 0 },
  { 112, 2353, 13, (u8 *) &initial_native_globals.pizza_competition_ready_text + 0 },
  { 112, 2458, 38, (u8 *) &initial_native_globals.ingredients_full_text + 0 },
  { 112, 2551, 4, (u8 *) &initial_native_globals.recipe_cost_divisor + 0 },
  { 112, 2566, 14, (u8 *) &initial_native_globals.disk_quit_prompt + 0 },
  { 112, 2621, 8, (u8 *) &initial_native_globals.character_selection_money_limit + 0 },
  { 112, 2798, 20, (u8 *) &initial_native_globals.selector_overwrite_prompt + 0 },
  { 112, 3076, 36, (u8 *) &initial_native_globals.closed_restaurant_text + 0 },
  { 112, 3144, 11, (u8 *) &initial_native_globals.game_over_retry_prompt + 0 },
  { 112, 3301, 47, (u8 *) &initial_native_globals.personal_record_save_prompt + 0 },
  { 112, 3348, 90, (u8 *) &initial_native_globals.game_over_low_score_text + 0 },
  { 112, 3509, 4, (u8 *) &initial_native_globals.animated_day_minutes + 0 },
  { 112, 3513, 8, (u8 *) &initial_native_globals.animated_percent_scale + 0 },
  { 112, 3831, 8, (u8 *) &initial_native_globals.realtor_sell_factor + 0 },
  { 112, 3839, 8, (u8 *) &initial_native_globals.realtor_maximum_factor + 0 },
  { 112, 3847, 8, (u8 *) &initial_native_globals.realtor_discount_factor + 0 },
  { 112, 3855, 8, (u8 *) &initial_native_globals.realtor_increase_factor + 0 },
  { 112, 4014, 8, (u8 *) &initial_native_globals.bank_loan_cash_limit + 0 },
  { 112, 4022, 8, (u8 *) &initial_native_globals.bank_investment_minimum + 0 },
  { 112, 4030, 8, (u8 *) &initial_native_globals.bank_investment_cash_limit + 0 },
  { 112, 4038, 8, (u8 *) &initial_native_globals.bank_investment_fraction + 0 },
  { 113, 1396, 8, (u8 *) &initial_native_globals.mafia_cost_cash_scale + 0 },
  { 113, 1404, 8, (u8 *) &initial_native_globals.mafia_cost_scale_limit + 0 },
  { 113, 1495, 16, (u8 *) &initial_native_globals.land_symbol_filename + 0 },
  { 113, 1558, 34, (u8 *) &initial_native_globals.no_memory_continue_prompt + 0 },
  { 113, 1706, 8, (u8 *) &initial_native_globals.price_display_divisor + 0 },
  { 113, 1714, 13, (u8 *) &initial_native_globals.font_error_text + 0 },
  { 113, 1753, 8, (u8 *) &initial_native_globals.legacy_round_half + 0 },
  { 113, 2069, 42, (u8 *) &initial_native_globals.bob_second_already_linked_error + 0 },
  { 113, 2111, 57, (u8 *) &initial_native_globals.bob_shared_capacity_error + 0 },
  { 113, 2256, 8, (u8 *) &initial_native_globals.decimal_scale_base + 0 },
  { 113, 2264, 4, (u8 *) &initial_native_globals.market_percent_scale + 0 },
  { 113, 2875, 26, (u8 *) &initial_native_globals.furniture_selection_limit_text + 0 },
  { 113, 2936, 67, (u8 *) &initial_native_globals.furniture_no_selection_text + 0 },
  { 113, 3003, 25, (u8 *) &initial_native_globals.placement_maximum_ovens_text + 0 },
  { 113, 3028, 25, (u8 *) &initial_native_globals.placement_maximum_items_text + 0 },
  { 113, 3053, 120, (u8 *) &initial_native_globals.placement_closed_text + 0 },
  { 113, 3212, 28, (u8 *) &initial_native_globals.placement_invalid_text + 0 },
  { 113, 3240, 28, (u8 *) &initial_native_globals.furniture_no_supplier_text + 0 },
  { 113, 3327, 113, (u8 *) &initial_native_globals.furniture_van_warning + 0 },
  { 113, 3440, 150, (u8 *) &initial_native_globals.furniture_info_formats + 0 },
  { 113, 3590, 18, (u8 *) &initial_native_globals.restaurant_close_confirm_text + 0 },
  { 113, 3608, 57, (u8 *) &initial_native_globals.restaurant_need_ovens_text + 0 },
  { 113, 3665, 65, (u8 *) &initial_native_globals.restaurant_need_chefs_text + 0 },
  { 113, 3730, 59, (u8 *) &initial_native_globals.restaurant_need_waiters_text + 0 },
  { 113, 3789, 75, (u8 *) &initial_native_globals.restaurant_need_recipes_text + 0 },
  { 113, 3864, 40, (u8 *) &initial_native_globals.restaurant_closed_today_text + 0 },
  { 113, 3904, 17, (u8 *) &initial_native_globals.restaurant_open_confirm_text + 0 },
  { 113, 3921, 64, (u8 *) &initial_native_globals.restaurant_rearrange_text + 0 },
  { 113, 3985, 8, (u8 *) &initial_native_globals.restaurant_nothing_text + 0 },
  { 113, 4041, 8, (u8 *) &initial_native_globals.recipe_price_limit + 0 },
  { 113, 4049, 8, (u8 *) &initial_native_globals.recipe_price_scale + 0 },
  { 114, 20, 34, (u8 *) &initial_native_globals.recipe_load_custom_error + 0 },
  { 114, 54, 37, (u8 *) &initial_native_globals.recipe_duplicate_error + 0 },
  { 114, 91, 27, (u8 *) &initial_native_globals.recipe_checksum_error + 0 },
  { 114, 118, 34, (u8 *) &initial_native_globals.recipe_list_save_custom_error + 0 },
  { 114, 175, 17, (u8 *) &initial_native_globals.recipe_list_recreate_prompt + 0 },
  { 114, 192, 37, (u8 *) &initial_native_globals.recipe_list_create_first_error + 0 },
  { 114, 229, 36, (u8 *) &initial_native_globals.recipe_list_already_menu_error + 0 },
  { 114, 265, 13, (u8 *) &initial_native_globals.recipe_price_prompt + 0 },
  { 114, 278, 48, (u8 *) &initial_native_globals.recipe_list_menu_full_error + 0 },
  { 114, 395, 45, (u8 *) &initial_native_globals.pizza_menu_keep_one_error + 0 },
  { 114, 440, 60, (u8 *) &initial_native_globals.pizza_menu_remove_selection_error + 0 },
  { 114, 500, 8, (u8 *) &initial_native_globals.profit_graph_scale_base + 0 },
  { 114, 508, 8, (u8 *) &initial_native_globals.legacy_graph_scale_base + 0 },
  { 114, 545, 8, (u8 *) &initial_native_globals.cash_graph_scale_base + 0 },
  { 114, 572, 8, (u8 *) &initial_native_globals.daily_profit_scale_base + 0 },
  { 114, 1331, 46, (u8 *) &initial_native_globals.advert_already_active + 0 },
  { 114, 1422, 8, (u8 *) &initial_native_globals.market_share_price_divisor + 0 },
  { 114, 1430, 8, (u8 *) &initial_native_globals.market_share_base_threshold + 0 },
  { 114, 1438, 30, (u8 *) &initial_native_globals.insufficient_cash_text + 0 },
  { 114, 1469, 8, (u8 *) &initial_native_globals.ledger_limit + 0 },
  { 114, 1480, 40, (u8 *) &initial_native_globals.restaurant_opening_text + 0 },
  { 114, 1520, 40, (u8 *) &initial_native_globals.restaurant_closing_text + 0 },
  { 114, 1608, 42, (u8 *) &initial_native_globals.new_restaurant_memory_error + 0 },
  { 114, 1785, 8, (u8 *) &initial_native_globals.supplier_price_scale + 0 },
  { 114, 1809, 281, (u8 *) &initial_native_globals.flooring_unlock_hint + 0 },
  { 114, 2426, 8, (u8 *) &initial_native_globals.weekly_cash_history_limit + 0 },
  { 114, 2512, 15, (u8 *) &initial_native_globals.end_turn_wait_text + 0 },
  { 114, 2564, 3, (u8 *) &initial_native_globals.sound_binary_read_mode + 0 },
  { 108, 1144, 44, (u8 *) &pt_read_only_tables.intro_text_groups + 0 },
  { 108, 1314, 648, (u8 *) &pt_read_only_tables.staff_training_courses + 0 },
  { 108, 2002, 556, (u8 *) &pt_read_only_tables.curriculum_scores + 0 },
  { 108, 2657, 101, (u8 *) &pt_read_only_tables.bitmap_to_show_for_goal + 0 },
  { 108, 2758, 36, (u8 *) &pt_read_only_tables.goal_bitmap_flags + 0 },
  { 108, 3734, 280, (u8 *) &pt_read_only_tables.default_recipe_names + 0 },
  { 108, 4017, 54, (u8 *) &pt_read_only_tables.main_menu_desc + 0 },
  { 109, 2142, 30, (u8 *) &pt_read_only_tables.map_tab_labels + 0 },
  { 109, 2444, 35, (u8 *) &pt_read_only_tables.season_names + 0 },
  { 110, 400, 196, (u8 *) &pt_read_only_tables.market_zone_names + 0 },
  { 110, 643, 98, (u8 *) &pt_read_only_tables.furniture_category_names + 0 },
  { 110, 2058, 60, (u8 *) &pt_read_only_tables.month_names + 0 },
  { 110, 2118, 12, (u8 *) &pt_read_only_tables.ordinal_suffixes + 0 },
  { 114, 3300, 80, (u8 *) &pt_read_only_tables.scale10_powers + 0 },
  { 27, 1164, 66, (u8 *) &pt_read_only_tables.staff_hire_role_labels + 0 },
  { 32, 3262, 8, (u8 *) &pt_read_only_tables.contract_justification_probabilities + 0 },
  { 32, 3270, 5, (u8 *) &pt_read_only_tables.contract_days_literal + 0 },
  { 39, 3600, 40, (u8 *) &pt_read_only_tables.manager_turnover_names + 0 },
  { 44, 1872, 4, (u8 *) &pt_read_only_tables.attack_health_left + 0 },
  { 44, 1876, 4, (u8 *) &pt_read_only_tables.attack_health_right + 0 },
  { 49, 2492, 10, (u8 *) &pt_read_only_tables.recipe_jury_x + 0 },
  { 49, 2502, 4, (u8 *) &pt_read_only_tables.recipe_chart_label_x + 0 },
  { 49, 2506, 4, (u8 *) &pt_read_only_tables.recipe_chart_x + 0 },
  { 49, 2510, 50, (u8 *) &pt_read_only_tables.ingredient_trend_names + 0 },
  { 52, 3629, 24, (u8 *) &pt_read_only_tables.selector_patterns + 0 },
  { 58, 3121, 8, (u8 *) &pt_read_only_tables.animated_quality_x + 0 },
  { 58, 3129, 8, (u8 *) &pt_read_only_tables.animated_quality_y + 0 },
  { 58, 3137, 16, (u8 *) &pt_read_only_tables.animated_capacity_colors + 0 },
  { 60, 488, 32, (u8 *) &pt_read_only_tables.valid_bank_account_numbers + 0 },
  { 63, 3597, 20, (u8 *) &pt_read_only_tables.mission_reward_bases + 0 },
  { 63, 3617, 32, (u8 *) &pt_read_only_tables.mission_place_articles + 0 },
  { 63, 3649, 8, (u8 *) &pt_read_only_tables.mission_place_article_groups + 0 },
  { 63, 3657, 8, (u8 *) &pt_read_only_tables.mission_place_name_prefixes + 0 },
  { 65, 3163, 4, (u8 *) &pt_read_only_tables.paragraph_width_characters + 0 },
  { 73, 1385, 88, (u8 *) &pt_read_only_tables.furniture_extras_labels + 0 },
  { 79, 2547, 5, (u8 *) &pt_read_only_tables.recipe_load_tag + 0 },
  { 81, 170, 8, (u8 *) &pt_read_only_tables.profit_graph_colors + 0 },
  { 81, 178, 32, (u8 *) &pt_read_only_tables.profit_graph_thresholds + 0 },
  { 81, 210, 13, (u8 *) &pt_read_only_tables.weekly_default_staff_name + 0 },
  { 103, 2808, 8, (u8 *) &pt_read_only_tables.num_ingredients_in_set + 0 },
  { 103, 2816, 96, (u8 *) &pt_read_only_tables.customer_age_names + 0 },
  { 103, 2912, 48, (u8 *) &pt_read_only_tables.customer_types + 0 },
  { 103, 2960, 198, (u8 *) &pt_read_only_tables.property_names + 0 },
  { 103, 3158, 72, (u8 *) &pt_read_only_tables.property_availability_names + 0 },
  { 103, 3248, 4, (u8 *) &pt_read_only_tables.ingredient_popup_x + 0 },
  { 103, 3253, 64, (u8 *) &pt_read_only_tables.g_stock_name_kind + 0 },
  { 106, 1352, 120, (u8 *) &pt_read_only_tables.town_names + 0 },
  { 106, 1484, 40, (u8 *) &pt_read_only_tables.staff_ui_file_animation + 0 },
  { 106, 1582, 48, (u8 *) &pt_read_only_tables.season_map_colors + 0 },
  { 106, 1632, 306, (u8 *) &pt_read_only_tables.g_button_config + 0 },
  { 106, 1938, 306, (u8 *) &pt_read_only_tables.button_icon_ids + 0 },
  { 106, 3468, 36, (u8 *) &pt_read_only_tables.button_x + 0 },
  { 106, 3504, 36, (u8 *) &pt_read_only_tables.button_y + 0 },
  { 106, 3548, 120, (u8 *) &pt_read_only_tables.invoice_month_names + 0 },
  { 106, 3726, 16, (u8 *) &pt_read_only_tables.pizza_menu_info_separator + 0 },
  { 106, 3751, 6, (u8 *) &pt_read_only_tables.customer_eating_times + 0 },
  { 106, 3757, 32, (u8 *) &pt_read_only_tables.empty_restaurant_floor + 0 },
  { 108, 1188, 42, (u8 *) &pt_read_only_tables.staff_ui_manager_reactions + 0 },
  { 108, 1230, 84, (u8 *) &pt_read_only_tables.staff_ui_manager_behaviors + 0 },
  { 108, 1962, 36, (u8 *) &pt_read_only_tables.staff_course_role_labels + 0 },
  { 108, 2570, 40, (u8 *) &pt_read_only_tables.character_mode_names + 0 },
  { 108, 2610, 28, (u8 *) &pt_read_only_tables.index_to_rank_value + 0 },
  { 108, 2796, 85, (u8 *) &pt_read_only_tables.contract_insolvency_text + 0 },
  { 108, 3232, 16, (u8 *) &pt_read_only_tables.manager_role_names + 0 },
  { 108, 3248, 20, (u8 *) &pt_read_only_tables.country_selector_x + 0 },
  { 108, 3268, 20, (u8 *) &pt_read_only_tables.country_selector_y + 0 },
  { 108, 3288, 4, (u8 *) &pt_read_only_tables.attack_walk_images + 0 },
  { 108, 3292, 4, (u8 *) &pt_read_only_tables.guest_seat_x + 0 },
  { 108, 3296, 4, (u8 *) &pt_read_only_tables.guest_seat_y + 0 },
  { 108, 3300, 4, (u8 *) &pt_read_only_tables.guest_body_x + 0 },
  { 108, 3304, 4, (u8 *) &pt_read_only_tables.guest_body_y + 0 },
  { 108, 3308, 16, (u8 *) &pt_read_only_tables.guest_pose_x + 0 },
  { 108, 3324, 16, (u8 *) &pt_read_only_tables.guest_pose_y + 0 },
  { 108, 3349, 8, (u8 *) &pt_read_only_tables.attack_collision_x + 0 },
  { 108, 3357, 8, (u8 *) &pt_read_only_tables.attack_collision_y + 0 },
  { 108, 3365, 4, (u8 *) &pt_read_only_tables.attack_head0_x + 0 },
  { 108, 3369, 4, (u8 *) &pt_read_only_tables.attack_head0_y + 0 },
  { 108, 3373, 4, (u8 *) &pt_read_only_tables.attack_head1_x + 0 },
  { 108, 3377, 4, (u8 *) &pt_read_only_tables.attack_head1_y + 0 },
  { 108, 3381, 12, (u8 *) &pt_read_only_tables.guest_carry_x + 0 },
  { 108, 3393, 12, (u8 *) &pt_read_only_tables.guest_carry_y + 0 },
  { 108, 3405, 32, (u8 *) &pt_read_only_tables.attack_held_weapon_x + 0 },
  { 108, 3437, 32, (u8 *) &pt_read_only_tables.attack_held_weapon_y + 0 },
  { 108, 3469, 40, (u8 *) &pt_read_only_tables.attack_knife_x + 0 },
  { 108, 3509, 40, (u8 *) &pt_read_only_tables.attack_spray_x + 0 },
  { 108, 3549, 40, (u8 *) &pt_read_only_tables.attack_knife_y + 0 },
  { 108, 3589, 40, (u8 *) &pt_read_only_tables.attack_spray_y + 0 },
  { 108, 3629, 4, (u8 *) &pt_read_only_tables.attack_projectile_x + 0 },
  { 108, 3633, 4, (u8 *) &pt_read_only_tables.attack_projectile_y + 0 },
  { 108, 3637, 10, (u8 *) &pt_read_only_tables.attack_explosion_x + 0 },
  { 108, 3647, 10, (u8 *) &pt_read_only_tables.attack_explosion_y + 0 },
  { 108, 3660, 24, (u8 *) &pt_read_only_tables.guest_idle_script + 0 },
  { 108, 3684, 10, (u8 *) &pt_read_only_tables.attack_weapon_reach + 0 },
  { 108, 3704, 4, (u8 *) &pt_read_only_tables.attack_step_x + 0 },
  { 108, 3708, 4, (u8 *) &pt_read_only_tables.attack_step_y + 0 },
  { 108, 4071, 1049, (u8 *) &pt_read_only_tables.main_menu_strings + 0 },
  { 109, 1024, 961, (u8 *) &pt_read_only_tables.main_menu_strings + 1049 },
  { 109, 1985, 90, (u8 *) &pt_read_only_tables.selector_family_titles + 0 },
  { 109, 2108, 18, (u8 *) &pt_read_only_tables.owner_to_color + 0 },
  { 109, 2136, 6, (u8 *) &pt_read_only_tables.tabs_y_values + 0 },
  { 109, 2172, 16, (u8 *) &pt_read_only_tables.district_map_pattern + 0 },
  { 109, 2188, 256, (u8 *) &pt_read_only_tables.terrain_tile_colors + 0 },
  { 109, 2479, 13, (u8 *) &pt_read_only_tables.month_to_arstid + 0 },
  { 109, 3008, 4, (u8 *) &pt_read_only_tables.animated_production_x + 0 },
  { 109, 3012, 4, (u8 *) &pt_read_only_tables.animated_production_y + 0 },
  { 109, 3034, 25, (u8 *) &pt_read_only_tables.phone_face_five_x + 0 },
  { 109, 3059, 25, (u8 *) &pt_read_only_tables.phone_face_five_y + 0 },
  { 109, 3084, 25, (u8 *) &pt_read_only_tables.phone_face_y + 0 },
  { 109, 3109, 16, (u8 *) &pt_read_only_tables.phone_background_face_x + 0 },
  { 109, 3125, 16, (u8 *) &pt_read_only_tables.phone_background_face_y + 0 },
  { 109, 3141, 130, (u8 *) &pt_read_only_tables.terminal_category_names + 0 },
  { 109, 3271, 144, (u8 *) &pt_read_only_tables.terminal_dial_messages + 0 },
  { 109, 3415, 26, (u8 *) &pt_read_only_tables.terminal_waiting_message + 0 },
  { 109, 3452, 5, (u8 *) &pt_read_only_tables.insurance_claim_reason_by_type + 0 },
  { 109, 3457, 6, (u8 *) &pt_read_only_tables.townhall_competition_messages + 0 },
  { 109, 3718, 32, (u8 *) &pt_read_only_tables.mission_item_pronouns + 0 },
  { 109, 3750, 12, (u8 *) &pt_read_only_tables.mission_pronouns + 0 },
  { 109, 3843, 6, (u8 *) &pt_read_only_tables.mission_completion_goals + 0 },
  { 109, 3851, 400, (u8 *) &pt_read_only_tables.arms_dealer_phone_nr + 0 },
  { 110, 155, 6, (u8 *) &pt_read_only_tables.weapon_store_quantities + 0 },
  { 110, 192, 200, (u8 *) &pt_read_only_tables.info_location_names + 0 },
  { 110, 596, 10, (u8 *) &pt_read_only_tables.market_bar_colors + 0 },
  { 110, 616, 4, (u8 *) &pt_read_only_tables.guest_food_offset_x + 0 },
  { 110, 620, 4, (u8 *) &pt_read_only_tables.guest_food_offset_y + 0 },
  { 110, 624, 4, (u8 *) &pt_read_only_tables.guest_lower_offset_x + 0 },
  { 110, 628, 4, (u8 *) &pt_read_only_tables.guest_lower_offset_y + 0 },
  { 110, 632, 4, (u8 *) &pt_read_only_tables.guest_upper_offset_x + 0 },
  { 110, 636, 4, (u8 *) &pt_read_only_tables.guest_upper_offset_y + 0 },
  { 110, 741, 283, (u8 *) &pt_read_only_tables.restaurant_media_labels + 0 },
  { 110, 1024, 101, (u8 *) &pt_read_only_tables.restaurant_media_labels + 283 },
  { 110, 1156, 90, (u8 *) &pt_read_only_tables.finance_stats_categories + 0 },
  { 110, 1367, 64, (u8 *) &pt_read_only_tables.monthly_receipt_labels + 0 },
  { 110, 1431, 180, (u8 *) &pt_read_only_tables.monthly_expense_labels + 0 },
  { 110, 1631, 100, (u8 *) &pt_read_only_tables.insurance_category_labels + 0 },
  { 110, 1752, 198, (u8 *) &pt_read_only_tables.advert_tv_frames + 0 },
  { 110, 1952, 100, (u8 *) &pt_read_only_tables.newspaper_article_backgrounds + 0 },
  { 110, 2888, 20, (u8 *) &pt_read_only_tables.guest_dos_error_errno + 0 },
  { 111, 467, 14, (u8 *) &pt_read_only_tables.market_property_title + 0 },
  { 111, 606, 14, (u8 *) &pt_read_only_tables.graphics_palette_filename + 0 },
  { 111, 620, 9, (u8 *) &pt_read_only_tables.intro_presents + 0 },
  { 111, 629, 17, (u8 *) &pt_read_only_tables.intro_logo_filename + 0 },
  { 111, 646, 14, (u8 *) &pt_read_only_tables.intro_cyber_filename + 0 },
  { 111, 660, 5, (u8 *) &pt_read_only_tables.intro_version + 0 },
  { 111, 665, 15, (u8 *) &pt_read_only_tables.intro_title1_filename + 0 },
  { 111, 680, 15, (u8 *) &pt_read_only_tables.intro_title2_filename + 0 },
  { 111, 695, 15, (u8 *) &pt_read_only_tables.startup_permap_filename + 0 },
  { 111, 710, 14, (u8 *) &pt_read_only_tables.startup_icons_filename + 0 },
  { 111, 724, 11, (u8 *) &pt_read_only_tables.startup_k0_filename + 0 },
  { 111, 735, 13, (u8 *) &pt_read_only_tables.startup_med1_filename + 0 },
  { 111, 748, 10, (u8 *) &pt_read_only_tables.main_sound_config_filename + 0 },
  { 111, 758, 6, (u8 *) &pt_read_only_tables.main_prefs_filename + 0 },
  { 111, 1008, 16, (u8 *) &pt_read_only_tables.staff_disaster_theft_text + 0 },
  { 111, 1024, 49, (u8 *) &pt_read_only_tables.staff_disaster_theft_text + 16 },
  { 111, 1073, 44, (u8 *) &pt_read_only_tables.staff_disaster_car_format + 0 },
  { 111, 1134, 8, (u8 *) &pt_read_only_tables.staff_ui_payment + 0 },
  { 111, 1142, 11, (u8 *) &pt_read_only_tables.staff_ui_pizzas_day + 0 },
  { 111, 1153, 8, (u8 *) &pt_read_only_tables.staff_ui_cooking + 0 },
  { 111, 1161, 8, (u8 *) &pt_read_only_tables.staff_ui_serving + 0 },
  { 111, 1169, 11, (u8 *) &pt_read_only_tables.staff_ui_management + 0 },
  { 111, 1180, 11, (u8 *) &pt_read_only_tables.staff_ui_experience + 0 },
  { 111, 1191, 13, (u8 *) &pt_read_only_tables.staff_ui_intelligence + 0 },
  { 111, 1204, 16, (u8 *) &pt_read_only_tables.staff_ui_trustworthiness + 0 },
  { 111, 1220, 7, (u8 *) &pt_read_only_tables.staff_ui_health + 0 },
  { 111, 1227, 11, (u8 *) &pt_read_only_tables.staff_ui_motivation + 0 },
  { 111, 1238, 17, (u8 *) &pt_read_only_tables.staff_application_title + 0 },
  { 111, 1255, 8, (u8 *) &pt_read_only_tables.staff_course_title + 0 },
  { 111, 1263, 12, (u8 *) &pt_read_only_tables.staff_course_name_title + 0 },
  { 111, 1275, 7, (u8 *) &pt_read_only_tables.staff_course_scores_title + 0 },
  { 111, 1282, 6, (u8 *) &pt_read_only_tables.staff_course_costs_title + 0 },
  { 111, 1288, 5, (u8 *) &pt_read_only_tables.staff_course_days_title + 0 },
  { 111, 1314, 11, (u8 *) &pt_read_only_tables.staff_ui_no_manager + 0 },
  { 111, 1325, 10, (u8 *) &pt_read_only_tables.staff_ui_authority + 0 },
  { 111, 1335, 10, (u8 *) &pt_read_only_tables.staff_ui_reactions + 0 },
  { 111, 1345, 23, (u8 *) &pt_read_only_tables.staff_ui_behavior_towards_staff + 0 },
  { 111, 1368, 19, (u8 *) &pt_read_only_tables.staff_ui_shopping_if_needed + 0 },
  { 111, 1387, 14, (u8 *) &pt_read_only_tables.staff_ui_change_prices + 0 },
  { 111, 1401, 17, (u8 *) &pt_read_only_tables.staff_ui_overdraw_account + 0 },
  { 111, 1418, 18, (u8 *) &pt_read_only_tables.staff_ui_turnover_decrease + 0 },
  { 111, 1436, 18, (u8 *) &pt_read_only_tables.staff_ui_turnover_increase + 0 },
  { 111, 1454, 16, (u8 *) &pt_read_only_tables.staff_ui_supply_shortage + 0 },
  { 111, 1470, 15, (u8 *) &pt_read_only_tables.staff_ui_staff_shortage + 0 },
  { 111, 1654, 15, (u8 *) &pt_read_only_tables.staff_expenses_label + 0 },
  { 111, 1785, 5, (u8 *) &pt_read_only_tables.staff_ui_name + 0 },
  { 111, 1790, 12, (u8 *) &pt_read_only_tables.staff_ui_days_absent + 0 },
  { 111, 1802, 4, (u8 *) &pt_read_only_tables.staff_ui_capacity + 0 },
  { 111, 1806, 4, (u8 *) &pt_read_only_tables.staff_ui_salary + 0 },
  { 111, 1816, 7, (u8 *) &pt_read_only_tables.staff_ui_course + 0 },
  { 111, 1823, 5, (u8 *) &pt_read_only_tables.staff_ui_sick + 0 },
  { 111, 1828, 9, (u8 *) &pt_read_only_tables.staff_ui_pregnant + 0 },
  { 111, 1837, 6, (u8 *) &pt_read_only_tables.staff_ui_chefs + 0 },
  { 111, 1843, 8, (u8 *) &pt_read_only_tables.staff_ui_waiters + 0 },
  { 111, 1851, 8, (u8 *) &pt_read_only_tables.staff_ui_number + 0 },
  { 111, 1859, 16, (u8 *) &pt_read_only_tables.staff_ui_total_capacity + 0 },
  { 111, 1875, 17, (u8 *) &pt_read_only_tables.staff_ui_average_quality + 0 },
  { 111, 1892, 16, (u8 *) &pt_read_only_tables.staff_ui_time_per_pizza + 0 },
  { 111, 1908, 6, (u8 *) &pt_read_only_tables.staff_ui_work + 0 },
  { 111, 1914, 12, (u8 *) &pt_read_only_tables.staff_ui_total_costs + 0 },
  { 111, 2124, 2, (u8 *) &pt_read_only_tables.staff_wage_unit + 0 },
  { 111, 2126, 15, (u8 *) &pt_read_only_tables.staff_wage_prompt + 0 },
  { 111, 2277, 85, (u8 *) &pt_read_only_tables.mission_completion_format + 0 },
  { 111, 2374, 33, (u8 *) &pt_read_only_tables.goal_name_1000_pizzas + 0 },
  { 111, 2407, 30, (u8 *) &pt_read_only_tables.goal_name_10000_pizzas + 0 },
  { 111, 2437, 39, (u8 *) &pt_read_only_tables.goal_name_profit_25000 + 0 },
  { 111, 2476, 39, (u8 *) &pt_read_only_tables.goal_name_profit_50000 + 0 },
  { 111, 2515, 18, (u8 *) &pt_read_only_tables.goal_name_world + 0 },
  { 111, 2542, 37, (u8 *) &pt_read_only_tables.character_initial_labels + 0 },
  { 111, 2579, 24, (u8 *) &pt_read_only_tables.target_curriculum_heading + 0 },
  { 111, 2603, 21, (u8 *) &pt_read_only_tables.curriculum_mission_only + 0 },
  { 111, 2624, 17, (u8 *) &pt_read_only_tables.curriculum_heading + 0 },
  { 111, 2647, 128, (u8 *) &pt_read_only_tables.character_value_labels + 0 },
  { 111, 2775, 17, (u8 *) &pt_read_only_tables.character_company_heading + 0 },
  { 111, 2805, 14, (u8 *) &pt_read_only_tables.character_company_profit + 0 },
  { 111, 2819, 14, (u8 *) &pt_read_only_tables.character_awards_filename + 0 },
  { 111, 2833, 7, (u8 *) &pt_read_only_tables.character_awards_heading + 0 },
  { 111, 2877, 5, (u8 *) &pt_read_only_tables.character_holiday_days + 0 },
  { 111, 2882, 14, (u8 *) &pt_read_only_tables.character_holiday_prompt + 0 },
  { 111, 2920, 9, (u8 *) &pt_read_only_tables.depot_weapon_category_label + 0 },
  { 111, 2929, 6, (u8 *) &pt_read_only_tables.stock_menu_depot_title + 0 },
  { 111, 2935, 4, (u8 *) &pt_read_only_tables.depot_weapon_bag_label + 0 },
  { 111, 2939, 17, (u8 *) &pt_read_only_tables.depot_town_title_format + 0 },
  { 111, 2956, 23, (u8 *) &pt_read_only_tables.depot_columns_label + 0 },
  { 111, 2979, 10, (u8 *) &pt_read_only_tables.depot_sum_value_label + 0 },
  { 111, 2989, 14, (u8 *) &pt_read_only_tables.depot_export_title_format + 0 },
  { 111, 3003, 5, (u8 *) &pt_read_only_tables.depot_sale_label + 0 },
  { 111, 3008, 7, (u8 *) &pt_read_only_tables.depot_purchase_label + 0 },
  { 111, 3015, 14, (u8 *) &pt_read_only_tables.depot_purchase_title_format + 0 },
  { 111, 3029, 6, (u8 *) &pt_read_only_tables.shop_price_label + 0 },
  { 111, 3035, 6, (u8 *) &pt_read_only_tables.depot_supplier_column_label + 0 },
  { 111, 3041, 7, (u8 *) &pt_read_only_tables.shop_amount_label + 0 },
  { 111, 3048, 6, (u8 *) &pt_read_only_tables.depot_store_column_label + 0 },
  { 111, 3054, 5, (u8 *) &pt_read_only_tables.contract_sum_label + 0 },
  { 111, 3153, 20, (u8 *) &pt_read_only_tables.depot_priorities_title + 0 },
  { 111, 3173, 10, (u8 *) &pt_read_only_tables.contract_discount_label + 0 },
  { 111, 3183, 8, (u8 *) &pt_read_only_tables.contract_charge_label + 0 },
  { 111, 3191, 9, (u8 *) &pt_read_only_tables.contract_terms_penalty_label + 0 },
  { 111, 3222, 5, (u8 *) &pt_read_only_tables.contract_debug_portrait_prompt + 0 },
  { 111, 3227, 17, (u8 *) &pt_read_only_tables.contract_minimum_order_format + 0 },
  { 111, 3252, 18, (u8 *) &pt_read_only_tables.contract_delivery_amounts_title + 0 },
  { 111, 3270, 3, (u8 *) &pt_read_only_tables.stock_menu_kg_label + 0 },
  { 111, 3273, 14, (u8 *) &pt_read_only_tables.contract_with_title + 0 },
  { 111, 3287, 7, (u8 *) &pt_read_only_tables.shop_total_label + 0 },
  { 111, 3294, 16, (u8 *) &pt_read_only_tables.contract_penalty_label + 0 },
  { 111, 3310, 8, (u8 *) &pt_read_only_tables.contract_amount_label + 0 },
  { 111, 3318, 12, (u8 *) &pt_read_only_tables.contract_as_required_label + 0 },
  { 111, 3330, 17, (u8 *) &pt_read_only_tables.contract_as_agreed_label + 0 },
  { 111, 3347, 9, (u8 *) &pt_read_only_tables.contract_quality_label + 0 },
  { 111, 3356, 16, (u8 *) &pt_read_only_tables.contract_remaining_term_label + 0 },
  { 111, 3386, 10, (u8 *) &pt_read_only_tables.contract_term_prompt + 0 },
  { 111, 3497, 2, (u8 *) &pt_read_only_tables.contract_delivery_space_text + 0 },
  { 111, 3530, 7, (u8 *) &pt_read_only_tables.shop_unavailable_price_text + 0 },
  { 111, 3549, 6, (u8 *) &pt_read_only_tables.shop_stock_label + 0 },
  { 111, 3555, 5, (u8 *) &pt_read_only_tables.stock_menu_days_label + 0 },
  { 111, 3895, 7, (u8 *) &pt_read_only_tables.stock_menu_stocks_title + 0 },
  { 111, 3902, 9, (u8 *) &pt_read_only_tables.stock_menu_kilogram_label + 0 },
  { 111, 3911, 6, (u8 *) &pt_read_only_tables.stock_menu_value_label + 0 },
  { 111, 3929, 5, (u8 *) &pt_read_only_tables.stock_menu_quality_label + 0 },
  { 111, 3934, 4, (u8 *) &pt_read_only_tables.stock_menu_supply_label + 0 },
  { 111, 3938, 4, (u8 *) &pt_read_only_tables.stock_menu_need_label + 0 },
  { 111, 3942, 4, (u8 *) &pt_read_only_tables.stock_menu_use_label + 0 },
  { 111, 3994, 8, (u8 *) &pt_read_only_tables.depot_central_label + 0 },
  { 111, 4002, 9, (u8 *) &pt_read_only_tables.depot_capacity_label + 0 },
  { 111, 4011, 7, (u8 *) &pt_read_only_tables.depot_in_use_label + 0 },
  { 111, 4018, 82, (u8 *) &pt_read_only_tables.depot_stolen_format + 0 },
  { 112, 76, 2, (u8 *) &pt_read_only_tables.file_write_mode + 0 },
  { 112, 78, 4, (u8 *) &pt_read_only_tables.file_readwrite_mode + 0 },
  { 112, 718, 13, (u8 *) &pt_read_only_tables.restaurant_electricity_label + 0 },
  { 112, 731, 22, (u8 *) &pt_read_only_tables.restaurant_slot_income_label + 0 },
  { 112, 753, 7, (u8 *) &pt_read_only_tables.restaurant_invoice_total_label + 0 },
  { 112, 780, 14, (u8 *) &pt_read_only_tables.country_market_leader_label + 0 },
  { 112, 794, 11, (u8 *) &pt_read_only_tables.country_no_market_leader_label + 0 },
  { 112, 805, 16, (u8 *) &pt_read_only_tables.country_map_filename + 0 },
  { 112, 821, 18, (u8 *) &pt_read_only_tables.country_no_city_message + 0 },
  { 112, 839, 18, (u8 *) &pt_read_only_tables.country_insufficient_money_message + 0 },
  { 112, 943, 13, (u8 *) &pt_read_only_tables.town_initializing_caption + 0 },
  { 112, 956, 4, (u8 *) &pt_read_only_tables.town_initializing_ellipsis + 0 },
  { 112, 960, 11, (u8 *) &pt_read_only_tables.staff_market_filename + 0 },
  { 112, 995, 2, (u8 *) &pt_read_only_tables.attack_police_argument + 0 },
  { 112, 1389, 6, (u8 *) &pt_read_only_tables.competition_pizza_title + 0 },
  { 112, 1395, 12, (u8 *) &pt_read_only_tables.competition_title + 0 },
  { 112, 1553, 3, (u8 *) &pt_read_only_tables.competition_dollars_unit + 0 },
  { 112, 1556, 21, (u8 *) &pt_read_only_tables.competition_bet_prompt + 0 },
  { 112, 1617, 58, (u8 *) &pt_read_only_tables.competition_invitation_format + 0 },
  { 112, 1675, 23, (u8 *) &pt_read_only_tables.competition_stake_prompt + 0 },
  { 112, 1699, 7, (u8 *) &pt_read_only_tables.competition_speed_prompt + 0 },
  { 112, 1706, 14, (u8 *) &pt_read_only_tables.competition_jury_caption + 0 },
  { 112, 1720, 3, (u8 *) &pt_read_only_tables.competition_time_format + 0 },
  { 112, 1763, 6, (u8 *) &pt_read_only_tables.recipe_new_name + 0 },
  { 112, 1769, 16, (u8 *) &pt_read_only_tables.recipe_number_format + 0 },
  { 112, 1785, 16, (u8 *) &pt_read_only_tables.recipe_calories_format + 0 },
  { 112, 1801, 16, (u8 *) &pt_read_only_tables.recipe_fat_format + 0 },
  { 112, 1817, 16, (u8 *) &pt_read_only_tables.recipe_sweet_format + 0 },
  { 112, 1833, 16, (u8 *) &pt_read_only_tables.recipe_salty_format + 0 },
  { 112, 1849, 16, (u8 *) &pt_read_only_tables.recipe_hot_format + 0 },
  { 112, 1865, 15, (u8 *) &pt_read_only_tables.recipe_appearance_format + 0 },
  { 112, 1880, 15, (u8 *) &pt_read_only_tables.recipe_refinement_format + 0 },
  { 112, 1895, 15, (u8 *) &pt_read_only_tables.recipe_trend_format + 0 },
  { 112, 1910, 17, (u8 *) &pt_read_only_tables.recipe_imitation_format + 0 },
  { 112, 1927, 17, (u8 *) &pt_read_only_tables.recipe_normality_format + 0 },
  { 112, 1944, 13, (u8 *) &pt_read_only_tables.recipe_jury_filename + 0 },
  { 112, 1957, 34, (u8 *) &pt_read_only_tables.recipe_jury_compare_text + 0 },
  { 112, 1991, 26, (u8 *) &pt_read_only_tables.recipe_jury_evaluate_text + 0 },
  { 112, 2017, 20, (u8 *) &pt_read_only_tables.recipe_jury_competition_text + 0 },
  { 112, 2037, 6, (u8 *) &pt_read_only_tables.recipe_jury_total_text + 0 },
  { 112, 2179, 6, (u8 *) &pt_read_only_tables.recipe_view_info_text + 0 },
  { 112, 2185, 7, (u8 *) &pt_read_only_tables.recipe_view_recipe_text + 0 },
  { 112, 2192, 5, (u8 *) &pt_read_only_tables.recipe_view_here_text + 0 },
  { 112, 2197, 4, (u8 *) &pt_read_only_tables.recipe_view_all_text + 0 },
  { 112, 2201, 6, (u8 *) &pt_read_only_tables.recipe_view_costs_text + 0 },
  { 112, 2207, 6, (u8 *) &pt_read_only_tables.recipe_view_price_text + 0 },
  { 112, 2213, 6, (u8 *) &pt_read_only_tables.recipe_view_sales_text + 0 },
  { 112, 2219, 7, (u8 *) &pt_read_only_tables.recipe_view_profit_text + 0 },
  { 112, 2226, 11, (u8 *) &pt_read_only_tables.recipe_view_popularity_text + 0 },
  { 112, 2237, 2, (u8 *) &pt_read_only_tables.recipe_view_gram_text + 0 },
  { 112, 2239, 15, (u8 *) &pt_read_only_tables.recipe_view_total_weight_text + 0 },
  { 112, 2274, 18, (u8 *) &pt_read_only_tables.pizza_competition_title + 0 },
  { 112, 2292, 17, (u8 *) &pt_read_only_tables.pizza_competition_separator + 0 },
  { 112, 2309, 17, (u8 *) &pt_read_only_tables.pizza_competition_original_label + 0 },
  { 112, 2326, 7, (u8 *) &pt_read_only_tables.pizza_competition_level_label + 0 },
  { 112, 2333, 20, (u8 *) &pt_read_only_tables.pizza_competition_ingredient_label + 0 },
  { 112, 2366, 11, (u8 *) &pt_read_only_tables.ingredient_grammes_format + 0 },
  { 112, 2377, 12, (u8 *) &pt_read_only_tables.ingredient_calories_format + 0 },
  { 112, 2389, 14, (u8 *) &pt_read_only_tables.ingredient_intensity_format + 0 },
  { 112, 2403, 10, (u8 *) &pt_read_only_tables.ingredient_fat_format + 0 },
  { 112, 2413, 10, (u8 *) &pt_read_only_tables.ingredient_sweet_format + 0 },
  { 112, 2423, 10, (u8 *) &pt_read_only_tables.ingredient_salty_format + 0 },
  { 112, 2433, 10, (u8 *) &pt_read_only_tables.ingredient_hot_format + 0 },
  { 112, 2443, 10, (u8 *) &pt_read_only_tables.ingredient_trend_format + 0 },
  { 112, 2453, 5, (u8 *) &pt_read_only_tables.ingredient_trend_name_format + 0 },
  { 112, 2535, 16, (u8 *) &pt_read_only_tables.recipe_name_prompt + 0 },
  { 112, 2556, 10, (u8 *) &pt_read_only_tables.disk_menu_caption + 0 },
  { 112, 2580, 13, (u8 *) &pt_read_only_tables.character_selection_level_caption + 0 },
  { 112, 2593, 15, (u8 *) &pt_read_only_tables.highscores_filename + 0 },
  { 112, 2608, 3, (u8 *) &pt_read_only_tables.options_on_label + 0 },
  { 112, 2611, 4, (u8 *) &pt_read_only_tables.options_off_label + 0 },
  { 112, 2615, 6, (u8 *) &pt_read_only_tables.options_prefs_filename + 0 },
  { 112, 2638, 15, (u8 *) &pt_read_only_tables.mission_selection_caption + 0 },
  { 112, 2662, 13, (u8 *) &pt_read_only_tables.main_menu_background_filename + 0 },
  { 112, 2675, 11, (u8 *) &pt_read_only_tables.game_screen_background_filename + 0 },
  { 112, 2686, 5, (u8 *) &pt_read_only_tables.selector_save_label + 0 },
  { 112, 2691, 5, (u8 *) &pt_read_only_tables.selector_load_label + 0 },
  { 112, 2702, 5, (u8 *) &pt_read_only_tables.selector_game_directory + 0 },
  { 112, 2707, 16, (u8 *) &pt_read_only_tables.selector_no_games + 0 },
  { 112, 2723, 26, (u8 *) &pt_read_only_tables.selector_no_characters + 0 },
  { 112, 2749, 24, (u8 *) &pt_read_only_tables.selector_no_missions + 0 },
  { 112, 2773, 13, (u8 *) &pt_read_only_tables.selector_new_name + 0 },
  { 112, 2786, 6, (u8 *) &pt_read_only_tables.selector_filename_prefix + 0 },
  { 112, 2792, 6, (u8 *) &pt_read_only_tables.selector_error_name + 0 },
  { 112, 2818, 6, (u8 *) &pt_read_only_tables.player_stats_money_label + 0 },
  { 112, 2824, 4, (u8 *) &pt_read_only_tables.player_stats_age_label + 0 },
  { 112, 2828, 11, (u8 *) &pt_read_only_tables.player_stats_popularity_label + 0 },
  { 112, 2839, 7, (u8 *) &pt_read_only_tables.player_stats_health_label + 0 },
  { 112, 2846, 7, (u8 *) &pt_read_only_tables.player_stats_energy_label + 0 },
  { 112, 2853, 13, (u8 *) &pt_read_only_tables.player_stats_intelligence_label + 0 },
  { 112, 2866, 9, (u8 *) &pt_read_only_tables.player_stats_charisma_label + 0 },
  { 112, 2875, 10, (u8 *) &pt_read_only_tables.player_stats_dexterity_label + 0 },
  { 112, 2885, 12, (u8 *) &pt_read_only_tables.player_stats_personality_label + 0 },
  { 112, 2897, 8, (u8 *) &pt_read_only_tables.player_stats_cooking_label + 0 },
  { 112, 2905, 8, (u8 *) &pt_read_only_tables.player_stats_courage_label + 0 },
  { 112, 2925, 14, (u8 *) &pt_read_only_tables.attack_action_caption + 0 },
  { 112, 2939, 14, (u8 *) &pt_read_only_tables.attack_weapon_caption + 0 },
  { 112, 2956, 8, (u8 *) &pt_read_only_tables.mission_invalid_place + 0 },
  { 112, 2974, 93, (u8 *) &pt_read_only_tables.poi_info_formats + 0 },
  { 112, 3067, 9, (u8 *) &pt_read_only_tables.poi_realtor_text + 0 },
  { 112, 3112, 16, (u8 *) &pt_read_only_tables.creating_map_text + 0 },
  { 112, 3128, 16, (u8 *) &pt_read_only_tables.maestro_background_filename + 0 },
  { 112, 3155, 118, (u8 *) &pt_read_only_tables.high_score_award_text + 0 },
  { 112, 3273, 10, (u8 *) &pt_read_only_tables.high_score_name_format + 0 },
  { 112, 3283, 6, (u8 *) &pt_read_only_tables.high_score_name_prompt + 0 },
  { 112, 3289, 6, (u8 *) &pt_read_only_tables.personal_record_directory + 0 },
  { 112, 3295, 6, (u8 *) &pt_read_only_tables.personal_record_fallback + 0 },
  { 112, 3438, 13, (u8 *) &pt_read_only_tables.ending_background_filename + 0 },
  { 112, 3452, 18, (u8 *) &pt_read_only_tables.animated_quality_caption + 0 },
  { 112, 3470, 22, (u8 *) &pt_read_only_tables.animated_reputation_caption + 0 },
  { 112, 3492, 3, (u8 *) &pt_read_only_tables.animated_no_text + 0 },
  { 112, 3495, 7, (u8 *) &pt_read_only_tables.animated_orders_text + 0 },
  { 112, 3502, 7, (u8 *) &pt_read_only_tables.animated_output_caption + 0 },
  { 112, 3521, 7, (u8 *) &pt_read_only_tables.animated_demand_text + 0 },
  { 112, 3528, 9, (u8 *) &pt_read_only_tables.animated_capacity_text + 0 },
  { 112, 3537, 11, (u8 *) &pt_read_only_tables.animated_popularity_caption + 0 },
  { 112, 3548, 15, (u8 *) &pt_read_only_tables.animated_customers_caption + 0 },
  { 112, 3563, 8, (u8 *) &pt_read_only_tables.animated_takings_text + 0 },
  { 112, 3571, 9, (u8 *) &pt_read_only_tables.animated_expenses_text + 0 },
  { 112, 3863, 7, (u8 *) &pt_read_only_tables.bank_transfer_amount_text + 0 },
  { 112, 3870, 13, (u8 *) &pt_read_only_tables.bank_transfer_account_text + 0 },
  { 113, 267, 93, (u8 *) &pt_read_only_tables.townhall_annual_pizza_announcement + 0 },
  { 113, 360, 20, (u8 *) &pt_read_only_tables.townhall_annual_recipe_name_format + 0 },
  { 113, 438, 13, (u8 *) &pt_read_only_tables.terminal_new_number_prompt + 0 },
  { 113, 451, 19, (u8 *) &pt_read_only_tables.terminal_no_entries + 0 },
  { 113, 470, 31, (u8 *) &pt_read_only_tables.terminal_title + 0 },
  { 113, 501, 30, (u8 *) &pt_read_only_tables.terminal_loading_message + 0 },
  { 113, 533, 3, (u8 *) &pt_read_only_tables.mission_completion_phone_text + 0 },
  { 113, 991, 33, (u8 *) &pt_read_only_tables.mafia_failed_business_text + 0 },
  { 113, 1024, 91, (u8 *) &pt_read_only_tables.mafia_failed_business_text + 33 },
  { 113, 1464, 8, (u8 *) &pt_read_only_tables.invoice_title + 0 },
  { 113, 1727, 2, (u8 *) &pt_read_only_tables.list_percent_text + 0 },
  { 113, 1733, 2, (u8 *) &pt_read_only_tables.dollar_alignment_text + 0 },
  { 113, 1743, 2, (u8 *) &pt_read_only_tables.advert_dollar + 0 },
  { 113, 1761, 272, (u8 *) &pt_read_only_tables.info_box_formats + 0 },
  { 113, 2033, 9, (u8 *) &pt_read_only_tables.restaurant_name_format + 0 },
  { 113, 2047, 3, (u8 *) &pt_read_only_tables.ok_text + 0 },
  { 113, 2050, 4, (u8 *) &pt_read_only_tables.yes_text + 0 },
  { 113, 2054, 3, (u8 *) &pt_read_only_tables.no_text + 0 },
  { 113, 2057, 7, (u8 *) &pt_read_only_tables.staff_disaster_question_format + 0 },
  { 113, 2168, 4, (u8 *) &pt_read_only_tables.market_low + 0 },
  { 113, 2172, 12, (u8 *) &pt_read_only_tables.market_very_little + 0 },
  { 113, 2184, 7, (u8 *) &pt_read_only_tables.market_little + 0 },
  { 113, 2191, 8, (u8 *) &pt_read_only_tables.market_average + 0 },
  { 113, 2199, 14, (u8 *) &pt_read_only_tables.market_above_average + 0 },
  { 113, 2213, 5, (u8 *) &pt_read_only_tables.market_high + 0 },
  { 113, 2218, 10, (u8 *) &pt_read_only_tables.market_very_high + 0 },
  { 113, 2228, 15, (u8 *) &pt_read_only_tables.market_extremely_high + 0 },
  { 113, 2243, 13, (u8 *) &pt_read_only_tables.market_out_of_range + 0 },
  { 113, 2282, 23, (u8 *) &pt_read_only_tables.market_premises_label + 0 },
  { 113, 2305, 12, (u8 *) &pt_read_only_tables.market_rent_label + 0 },
  { 113, 2317, 5, (u8 *) &pt_read_only_tables.market_per_square_metre + 0 },
  { 113, 2322, 13, (u8 *) &pt_read_only_tables.market_price_label + 0 },
  { 113, 2335, 10, (u8 *) &pt_read_only_tables.market_tendency_label + 0 },
  { 113, 2345, 8, (u8 *) &pt_read_only_tables.market_falling_label + 0 },
  { 113, 2353, 7, (u8 *) &pt_read_only_tables.market_rising_label + 0 },
  { 113, 2360, 14, (u8 *) &pt_read_only_tables.market_average_size_label + 0 },
  { 113, 2374, 4, (u8 *) &pt_read_only_tables.market_square_metres + 0 },
  { 113, 2378, 7, (u8 *) &pt_read_only_tables.market_rooms_label + 0 },
  { 113, 2385, 9, (u8 *) &pt_read_only_tables.market_to_rent_label + 0 },
  { 113, 2394, 10, (u8 *) &pt_read_only_tables.market_for_sale_label + 0 },
  { 113, 2404, 15, (u8 *) &pt_read_only_tables.market_not_available_label + 0 },
  { 113, 2419, 6, (u8 *) &pt_read_only_tables.market_taste_title + 0 },
  { 113, 2425, 2, (u8 *) &pt_read_only_tables.market_rank_dash + 0 },
  { 113, 2427, 21, (u8 *) &pt_read_only_tables.market_chain_title + 0 },
  { 113, 2484, 11, (u8 *) &pt_read_only_tables.floor_zero_filename + 0 },
  { 113, 2515, 4, (u8 *) &pt_read_only_tables.furniture_inventory_num_text + 0 },
  { 113, 2519, 5, (u8 *) &pt_read_only_tables.furniture_inventory_name_text + 0 },
  { 113, 2530, 10, (u8 *) &pt_read_only_tables.furniture_equipment_title + 0 },
  { 113, 2540, 8, (u8 *) &pt_read_only_tables.furniture_in_use_text + 0 },
  { 113, 2548, 7, (u8 *) &pt_read_only_tables.furniture_value_text + 0 },
  { 113, 2555, 9, (u8 *) &pt_read_only_tables.furniture_quality_text + 0 },
  { 113, 2564, 10, (u8 *) &pt_read_only_tables.poi_capacity_text + 0 },
  { 113, 2574, 6, (u8 *) &pt_read_only_tables.furniture_seats_text + 0 },
  { 113, 2580, 8, (u8 *) &pt_read_only_tables.furniture_per_day_text + 0 },
  { 113, 2588, 12, (u8 *) &pt_read_only_tables.furniture_kitchenware_title + 0 },
  { 113, 2600, 7, (u8 *) &pt_read_only_tables.furniture_ovens_text + 0 },
  { 113, 2607, 17, (u8 *) &pt_read_only_tables.furniture_bake_time_text + 0 },
  { 113, 2624, 8, (u8 *) &pt_read_only_tables.furniture_minutes_text + 0 },
  { 113, 2632, 17, (u8 *) &pt_read_only_tables.furniture_baking_capacity_text + 0 },
  { 113, 2649, 7, (u8 *) &pt_read_only_tables.furniture_number_text + 0 },
  { 113, 2656, 15, (u8 *) &pt_read_only_tables.furniture_floor_title + 0 },
  { 113, 2714, 7, (u8 *) &pt_read_only_tables.furniture_extras_title + 0 },
  { 113, 2764, 14, (u8 *) &pt_read_only_tables.furniture_floor_patterns_filename + 0 },
  { 113, 2816, 7, (u8 *) &pt_read_only_tables.furniture_price_text + 0 },
  { 113, 2823, 19, (u8 *) &pt_read_only_tables.furniture_capacity_format + 0 },
  { 113, 2842, 21, (u8 *) &pt_read_only_tables.furniture_baking_time_format + 0 },
  { 113, 2863, 12, (u8 *) &pt_read_only_tables.furniture_quality_format + 0 },
  { 113, 2901, 10, (u8 *) &pt_read_only_tables.furniture_invoice_caption + 0 },
  { 113, 2911, 25, (u8 *) &pt_read_only_tables.furniture_purchase_thanks + 0 },
  { 113, 3173, 39, (u8 *) &pt_read_only_tables.placement_sell_format + 0 },
  { 113, 3268, 10, (u8 *) &pt_read_only_tables.furniture_suppliers_caption + 0 },
  { 113, 3278, 5, (u8 *) &pt_read_only_tables.customer_ratings_menu_title + 0 },
  { 113, 3283, 10, (u8 *) &pt_read_only_tables.customer_ratings_furniture_title + 0 },
  { 113, 3293, 13, (u8 *) &pt_read_only_tables.customer_ratings_title + 0 },
  { 113, 3306, 11, (u8 *) &pt_read_only_tables.furniture_menu_caption + 0 },
  { 113, 3317, 10, (u8 *) &pt_read_only_tables.furniture_equipment_caption + 0 },
  { 113, 3993, 16, (u8 *) &pt_read_only_tables.restaurant_media_prompt + 0 },
  { 113, 4012, 9, (u8 *) &pt_read_only_tables.intro_music_filename + 0 },
  { 113, 4057, 12, (u8 *) &pt_read_only_tables.recipe_list_title + 0 },
  { 113, 4077, 6, (u8 *) &pt_read_only_tables.recipe_list_new + 0 },
  { 113, 4095, 2, (u8 *) &pt_read_only_tables.recipe_number_unit + 0 },
  { 114, 1, 19, (u8 *) &pt_read_only_tables.recipe_load_prompt + 0 },
  { 114, 152, 18, (u8 *) &pt_read_only_tables.recipe_save_prompt + 0 },
  { 114, 170, 5, (u8 *) &pt_read_only_tables.recipe_save_tag + 0 },
  { 114, 331, 14, (u8 *) &pt_read_only_tables.pizza_menu_sales_title + 0 },
  { 114, 345, 5, (u8 *) &pt_read_only_tables.pizza_menu_title + 0 },
  { 114, 350, 6, (u8 *) &pt_read_only_tables.pizza_menu_sales_label + 0 },
  { 114, 356, 10, (u8 *) &pt_read_only_tables.pizza_menu_turnover_label + 0 },
  { 114, 366, 14, (u8 *) &pt_read_only_tables.pizza_menu_popularity_label + 0 },
  { 114, 380, 8, (u8 *) &pt_read_only_tables.pizza_menu_demand_label + 0 },
  { 114, 388, 7, (u8 *) &pt_read_only_tables.pizza_menu_info_sales_label + 0 },
  { 114, 516, 10, (u8 *) &pt_read_only_tables.cash_flow_caption + 0 },
  { 114, 539, 6, (u8 *) &pt_read_only_tables.profit_graph_weeks + 0 },
  { 114, 553, 14, (u8 *) &pt_read_only_tables.daily_profit_caption + 0 },
  { 114, 567, 5, (u8 *) &pt_read_only_tables.profit_graph_days + 0 },
  { 114, 580, 118, (u8 *) &pt_read_only_tables.balance_labels + 0 },
  { 114, 698, 7, (u8 *) &pt_read_only_tables.weekly_profit_text + 0 },
  { 114, 705, 110, (u8 *) &pt_read_only_tables.finance_stats_labels + 0 },
  { 114, 815, 17, (u8 *) &pt_read_only_tables.monthly_overview_caption + 0 },
  { 114, 832, 8, (u8 *) &pt_read_only_tables.monthly_result_caption + 0 },
  { 114, 840, 12, (u8 *) &pt_read_only_tables.monthly_restaurants_caption + 0 },
  { 114, 852, 17, (u8 *) &pt_read_only_tables.daily_settlement_text + 0 },
  { 114, 869, 11, (u8 *) &pt_read_only_tables.daily_day_before_text + 0 },
  { 114, 880, 10, (u8 *) &pt_read_only_tables.daily_yesterday_text + 0 },
  { 114, 890, 6, (u8 *) &pt_read_only_tables.daily_today_text + 0 },
  { 114, 896, 7, (u8 *) &pt_read_only_tables.daily_orders_text + 0 },
  { 114, 903, 12, (u8 *) &pt_read_only_tables.daily_pizzas_sold_text + 0 },
  { 114, 915, 18, (u8 *) &pt_read_only_tables.daily_delivery_capacity_text + 0 },
  { 114, 933, 13, (u8 *) &pt_read_only_tables.daily_demand_value_text + 0 },
  { 114, 946, 6, (u8 *) &pt_read_only_tables.weekly_sales_text + 0 },
  { 114, 952, 17, (u8 *) &pt_read_only_tables.daily_ingredients_used_text + 0 },
  { 114, 969, 12, (u8 *) &pt_read_only_tables.daily_expenditure_text + 0 },
  { 114, 981, 7, (u8 *) &pt_read_only_tables.weekly_week_text + 0 },
  { 114, 988, 8, (u8 *) &pt_read_only_tables.weekly_takings_text + 0 },
  { 114, 996, 6, (u8 *) &pt_read_only_tables.weekly_items_text + 0 },
  { 114, 1002, 6, (u8 *) &pt_read_only_tables.weekly_costs_text + 0 },
  { 114, 1008, 7, (u8 *) &pt_read_only_tables.weekly_total_text + 0 },
  { 114, 1015, 9, (u8 *) &pt_read_only_tables.weekly_expenses_text + 0 },
  { 114, 1024, 15, (u8 *) &pt_read_only_tables.weekly_ingredients_text + 0 },
  { 114, 1039, 7, (u8 *) &pt_read_only_tables.weekly_cooks_text + 0 },
  { 114, 1055, 8, (u8 *) &pt_read_only_tables.weekly_waiters_text + 0 },
  { 114, 1063, 10, (u8 *) &pt_read_only_tables.weekly_managers_text + 0 },
  { 114, 1073, 4, (u8 *) &pt_read_only_tables.restaurant_overview_day + 0 },
  { 114, 1077, 5, (u8 *) &pt_read_only_tables.restaurant_overview_week + 0 },
  { 114, 1082, 20, (u8 *) &pt_read_only_tables.restaurant_overview_caption + 0 },
  { 114, 1102, 12, (u8 *) &pt_read_only_tables.rest_sales_caption + 0 },
  { 114, 1114, 10, (u8 *) &pt_read_only_tables.rest_customers_caption + 0 },
  { 114, 1130, 16, (u8 *) &pt_read_only_tables.rest_weekly_caption + 0 },
  { 114, 1146, 33, (u8 *) &pt_read_only_tables.rest_weekly_columns + 0 },
  { 114, 1235, 8, (u8 *) &pt_read_only_tables.advert_output_label + 0 },
  { 114, 1243, 7, (u8 *) &pt_read_only_tables.advert_costs_label + 0 },
  { 114, 1296, 7, (u8 *) &pt_read_only_tables.advert_tv_title + 0 },
  { 114, 1303, 8, (u8 *) &pt_read_only_tables.advert_tv_director + 0 },
  { 114, 1311, 5, (u8 *) &pt_read_only_tables.advert_days + 0 },
  { 114, 1316, 15, (u8 *) &pt_read_only_tables.advert_misc_filename + 0 },
  { 114, 1420, 2, (u8 *) &pt_read_only_tables.end_turn_phone_text + 0 },
  { 114, 1477, 3, (u8 *) &pt_read_only_tables.periodic_extortion_text + 0 },
  { 114, 1560, 48, (u8 *) &pt_read_only_tables.cash_goal_thresholds + 0 },
  { 114, 1764, 21, (u8 *) &pt_read_only_tables.delayed_message_format + 0 },
  { 114, 2234, 116, (u8 *) &pt_read_only_tables.overdrawn_account_text + 0 },
  { 114, 2410, 16, (u8 *) &pt_read_only_tables.weekly_profit_caption + 0 },
  { 114, 2434, 16, (u8 *) &pt_read_only_tables.monthly_cost_caption + 0 },
  { 114, 2450, 11, (u8 *) &pt_read_only_tables.monthly_repayment_caption + 0 },
  { 114, 2461, 10, (u8 *) &pt_read_only_tables.monthly_interest_caption + 0 },
  { 114, 2471, 11, (u8 *) &pt_read_only_tables.monthly_insurance_caption + 0 },
  { 114, 2482, 15, (u8 *) &pt_read_only_tables.monthly_investment_interest_caption + 0 },
  { 114, 2497, 6, (u8 *) &pt_read_only_tables.monthly_rent_caption + 0 },
  { 114, 2503, 7, (u8 *) &pt_read_only_tables.monthly_total_caption + 0 },
  { 114, 2510, 2, (u8 *) &pt_read_only_tables.player_card_dot + 0 },
  { 114, 2536, 3, (u8 *) &pt_read_only_tables.sound_config_read_mode + 0 },
  { 114, 2539, 10, (u8 *) &pt_read_only_tables.sound_config_filename + 0 },
  { 114, 2549, 12, (u8 *) &pt_read_only_tables.sound_pcm_driver_filename + 0 },
  { 114, 2568, 11, (u8 *) &pt_read_only_tables.sound_vector_filename + 0 },
  { 114, 2580, 3, (u8 *) &pt_read_only_tables.timezone_env_name + 0 },
  { 114, 2948, 256, (u8 *) &pt_read_only_tables.watcom_character_classes + 0 },
  { 114, 3208, 8, (u8 *) &pt_read_only_tables.price_parser_positive_infinity + 0 },
  { 114, 3232, 8, (u8 *) &pt_read_only_tables.scale10_positive_max + 0 },
  { 114, 3240, 8, (u8 *) &pt_read_only_tables.scale10_negative_max + 0 },
  { 114, 3248, 26, (u8 *) &pt_read_only_tables.time_common_month_days + 0 },
  { 114, 3274, 26, (u8 *) &pt_read_only_tables.time_leap_month_days + 0 },
  { 110, 3468, 15, (u8 *) &pt_interface_strings.file_menu_font + 0 },
  { 110, 3483, 15, (u8 *) &pt_interface_strings.file_ruby_font + 0 },
  { 110, 3498, 19, (u8 *) &pt_interface_strings.file_font_desc_cstandar + 0 },
  { 110, 3517, 19, (u8 *) &pt_interface_strings.file_font_desc_piznumb + 0 },
  { 110, 3536, 11, (u8 *) &pt_interface_strings.file_topaz + 0 },
  { 110, 3547, 19, (u8 *) &pt_interface_strings.file_font_desc_astandar + 0 },
  { 110, 3566, 19, (u8 *) &pt_interface_strings.file_font_desc_supersma + 0 },
  { 110, 3585, 1, (u8 *) &pt_interface_strings.empty_unit + 0 },
  { 110, 3586, 4, (u8 *) &pt_interface_strings.hint_map + 0 },
  { 110, 3590, 10, (u8 *) &pt_interface_strings.hint_character + 0 },
  { 110, 3600, 13, (u8 *) &pt_interface_strings.hint_disk_options + 0 },
  { 110, 3613, 12, (u8 *) &pt_interface_strings.hint_advertising + 0 },
  { 110, 3625, 16, (u8 *) &pt_interface_strings.hint_market_research + 0 },
  { 110, 3641, 10, (u8 *) &pt_interface_strings.hint_newspaper + 0 },
  { 110, 3651, 13, (u8 *) &pt_interface_strings.hint_global_stats + 0 },
  { 110, 3664, 9, (u8 *) &pt_interface_strings.hint_terminal + 0 },
  { 110, 3673, 9, (u8 *) &pt_interface_strings.hint_end_turn + 0 },
  { 110, 3682, 11, (u8 *) &pt_interface_strings.hint_open_close + 0 },
  { 110, 3693, 6, (u8 *) &pt_interface_strings.hint_stock + 0 },
  { 110, 3699, 5, (u8 *) &pt_interface_strings.hint_menu + 0 },
  { 110, 3704, 11, (u8 *) &pt_interface_strings.hint_furnishing + 0 },
  { 110, 3715, 6, (u8 *) &pt_interface_strings.hint_staff + 0 },
  { 110, 3721, 12, (u8 *) &pt_interface_strings.hint_recipe_list + 0 },
  { 110, 3733, 17, (u8 *) &pt_interface_strings.hint_restaurant_stats + 0 },
  { 110, 3750, 15, (u8 *) &pt_interface_strings.hint_animated_stats + 0 },
  { 110, 3765, 11, (u8 *) &pt_interface_strings.hint_vegetables + 0 },
  { 110, 3776, 5, (u8 *) &pt_interface_strings.hint_meat + 0 },
  { 110, 3781, 10, (u8 *) &pt_interface_strings.hint_contracts + 0 },
  { 110, 3791, 6, (u8 *) &pt_interface_strings.hint_fruit + 0 },
  { 110, 3797, 7, (u8 *) &pt_interface_strings.hint_cheese + 0 },
  { 110, 3804, 5, (u8 *) &pt_interface_strings.hint_shop + 0 },
  { 110, 3809, 5, (u8 *) &pt_interface_strings.hint_back + 0 },
  { 110, 3814, 7, (u8 *) &pt_interface_strings.hint_reject + 0 },
  { 110, 3821, 6, (u8 *) &pt_interface_strings.hint_agree + 0 },
  { 110, 3827, 10, (u8 *) &pt_interface_strings.hint_furniture + 0 },
  { 110, 3837, 12, (u8 *) &pt_interface_strings.hint_kitchenware + 0 },
  { 110, 3849, 17, (u8 *) &pt_interface_strings.hint_customer_ratings + 0 },
  { 110, 3866, 7, (u8 *) &pt_interface_strings.hint_extras + 0 },
  { 110, 3873, 9, (u8 *) &pt_interface_strings.hint_flooring + 0 },
  { 110, 3882, 8, (u8 *) &pt_interface_strings.hint_weapons + 0 },
  { 110, 3890, 13, (u8 *) &pt_interface_strings.hint_single_order + 0 },
  { 110, 3903, 14, (u8 *) &pt_interface_strings.hint_make_contract + 0 },
  { 110, 3917, 10, (u8 *) &pt_interface_strings.hint_turn_page + 0 },
  { 110, 3927, 12, (u8 *) &pt_interface_strings.hint_pizza_sales + 0 },
  { 110, 3939, 13, (u8 *) &pt_interface_strings.hint_remove_pizza + 0 },
  { 110, 3952, 11, (u8 *) &pt_interface_strings.hint_view_pizza + 0 },
  { 110, 3963, 12, (u8 *) &pt_interface_strings.hint_ingredients + 0 },
  { 110, 3975, 11, (u8 *) &pt_interface_strings.hint_pizza_info + 0 },
  { 110, 3986, 5, (u8 *) &pt_interface_strings.hint_o_k + 0 },
  { 110, 3991, 10, (u8 *) &pt_interface_strings.hint_flip_page + 0 },
  { 110, 4001, 16, (u8 *) &pt_interface_strings.hint_cancel_contract + 0 },
  { 110, 4017, 6, (u8 *) &pt_interface_strings.hint_chefs + 0 },
  { 110, 4023, 8, (u8 *) &pt_interface_strings.hint_waiters + 0 },
  { 110, 4031, 9, (u8 *) &pt_interface_strings.hint_managers + 0 },
  { 110, 4040, 5, (u8 *) &pt_interface_strings.hint_fire + 0 },
  { 110, 4045, 6, (u8 *) &pt_interface_strings.hint_wages + 0 },
  { 110, 4051, 9, (u8 *) &pt_interface_strings.hint_training + 0 },
  { 110, 4060, 8, (u8 *) &pt_interface_strings.hint_recruit + 0 },
  { 110, 4068, 11, (u8 *) &pt_interface_strings.hint_production + 0 },
  { 110, 4079, 11, (u8 *) &pt_interface_strings.hint_reputation + 0 },
  { 110, 4090, 9, (u8 *) &pt_interface_strings.hint_capacity + 0 },
  { 111, 3, 14, (u8 *) &pt_interface_strings.hint_pizza_quality + 0 },
  { 111, 17, 11, (u8 *) &pt_interface_strings.hint_popularity + 0 },
  { 111, 28, 9, (u8 *) &pt_interface_strings.hint_turnover + 0 },
  { 111, 37, 5, (u8 *) &pt_interface_strings.hint_data + 0 },
  { 111, 42, 13, (u8 *) &pt_interface_strings.hint_applications + 0 },
  { 111, 55, 5, (u8 *) &pt_interface_strings.hint_hire + 0 },
  { 111, 60, 4, (u8 *) &pt_interface_strings.hint_buy + 0 },
  { 111, 64, 7, (u8 *) &pt_interface_strings.hint_dial_1 + 0 },
  { 111, 71, 7, (u8 *) &pt_interface_strings.hint_dial_2 + 0 },
  { 111, 78, 7, (u8 *) &pt_interface_strings.hint_dial_3 + 0 },
  { 111, 85, 7, (u8 *) &pt_interface_strings.hint_dial_4 + 0 },
  { 111, 92, 7, (u8 *) &pt_interface_strings.hint_dial_5 + 0 },
  { 111, 99, 7, (u8 *) &pt_interface_strings.hint_dial_6 + 0 },
  { 111, 106, 7, (u8 *) &pt_interface_strings.hint_dial_7 + 0 },
  { 111, 113, 7, (u8 *) &pt_interface_strings.hint_dial_8 + 0 },
  { 111, 120, 7, (u8 *) &pt_interface_strings.hint_dial_9 + 0 },
  { 111, 127, 17, (u8 *) &pt_interface_strings.hint_character_values + 0 },
  { 111, 144, 8, (u8 *) &pt_interface_strings.hint_company + 0 },
  { 111, 152, 17, (u8 *) &pt_interface_strings.hint_curriculum_vitae + 0 },
  { 111, 169, 9, (u8 *) &pt_interface_strings.hint_position + 0 },
  { 111, 178, 7, (u8 *) &pt_interface_strings.hint_awards + 0 },
  { 111, 185, 10, (u8 *) &pt_interface_strings.hint_target_cv + 0 },
  { 111, 195, 10, (u8 *) &pt_interface_strings.hint_challenge + 0 },
  { 111, 205, 9, (u8 *) &pt_interface_strings.hint_sabotage + 0 },
  { 111, 214, 9, (u8 *) &pt_interface_strings.hint_transfer + 0 },
  { 111, 223, 11, (u8 *) &pt_interface_strings.hint_bake_pizza + 0 },
  { 111, 234, 7, (u8 *) &pt_interface_strings.hint_export + 0 },
  { 111, 241, 11, (u8 *) &pt_interface_strings.hint_priorities + 0 },
  { 111, 252, 5, (u8 *) &pt_interface_strings.hint_sell + 0 },
  { 111, 257, 10, (u8 *) &pt_interface_strings.hint_next_item + 0 },
  { 111, 267, 11, (u8 *) &pt_interface_strings.hint_throw_away + 0 },
  { 111, 278, 14, (u8 *) &pt_interface_strings.hint_previous_item + 0 },
  { 111, 292, 15, (u8 *) &pt_interface_strings.hint_month_overview + 0 },
  { 111, 307, 12, (u8 *) &pt_interface_strings.hint_restaurants + 0 },
  { 111, 319, 10, (u8 *) &pt_interface_strings.hint_cash_flow + 0 },
  { 111, 329, 6, (u8 *) &pt_interface_strings.hint_loans + 0 },
  { 111, 335, 10, (u8 *) &pt_interface_strings.hint_insurance + 0 },
  { 111, 345, 9, (u8 *) &pt_interface_strings.hint_property + 0 },
  { 111, 354, 8, (u8 *) &pt_interface_strings.hint_balance + 0 },
  { 111, 362, 14, (u8 *) &pt_interface_strings.hint_daily_profits + 0 },
  { 111, 376, 14, (u8 *) &pt_interface_strings.hint_daily_balance + 0 },
  { 111, 390, 12, (u8 *) &pt_interface_strings.hint_week_review + 0 },
  { 111, 402, 13, (u8 *) &pt_interface_strings.hint_month_review + 0 },
  { 111, 415, 15, (u8 *) &pt_interface_strings.hint_week_s_takings + 0 },
  { 111, 430, 15, (u8 *) &pt_interface_strings.hint_customer_stats + 0 },
  { 111, 445, 12, (u8 *) &pt_interface_strings.hint_sales_curve + 0 },
  { 111, 457, 10, (u8 *) &pt_interface_strings.hint_area_info + 0 },
  { 111, 481, 8, (u8 *) &pt_interface_strings.hint_economy + 0 },
  { 111, 489, 15, (u8 *) &pt_interface_strings.hint_customer_taste + 0 },
  { 111, 504, 7, (u8 *) &pt_interface_strings.hint_trends + 0 },
  { 111, 511, 13, (u8 *) &pt_interface_strings.hint_pizza_charts + 0 },
  { 111, 524, 12, (u8 *) &pt_interface_strings.hint_competitors + 0 },
  { 111, 536, 13, (u8 *) &pt_interface_strings.hint_market_share + 0 },
  { 111, 549, 9, (u8 *) &pt_interface_strings.hint_continue + 0 },
  { 111, 558, 15, (u8 *) &pt_interface_strings.hint_move_elsewhere + 0 },
  { 111, 573, 8, (u8 *) &pt_interface_strings.hint_sell_it + 0 },
  { 111, 581, 16, (u8 *) &pt_interface_strings.hint_dial_new_number + 0 },
  { 111, 597, 9, (u8 *) &pt_interface_strings.hint_view_ads + 0 },
};
// clang-format on
_Static_assert(sizeof(initial_native_globals.restaurant_mask_rect) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.cursor_hide_depth) == 2, "default field width");
_Static_assert(sizeof(initial_native_globals.input_queue_capacity) == 4, "default field width");
_Static_assert(sizeof(initial_native_globals.smallrects) == 76, "default field width");
_Static_assert(sizeof(initial_native_globals.furniture_desc) == 7216, "default field width");
_Static_assert(sizeof(initial_native_globals.furniture_supplier_count) == 1, "default field width");
_Static_assert(sizeof(initial_native_globals.furniture_suppliers) == 1584, "default field width");
_Static_assert(sizeof(initial_native_globals.days_in_month) == 13, "default field width");
_Static_assert(sizeof(initial_native_globals.prefs) == 12, "default field width");
_Static_assert(sizeof(initial_native_globals.state_slot_007) == 1, "default field width");
_Static_assert(sizeof(initial_native_globals.ingredients) == 5472, "default field width");
_Static_assert(sizeof(initial_native_globals.age_taste_preferences) == 72, "default field width");
_Static_assert(sizeof(initial_native_globals.goal_ui_colors) == 4, "default field width");
_Static_assert(sizeof(initial_native_globals.depot_capacity_exhausted_text) == 30, "default field width");
_Static_assert(sizeof(initial_native_globals.translation_error_text) == 11, "default field width");
_Static_assert(sizeof(initial_native_globals.mouse_bitmap_data_1) == 121, "default field width");
_Static_assert(sizeof(initial_native_globals.pizza_level) == 1, "default field width");
_Static_assert(sizeof(initial_native_globals.map_marker_toggle) == 1, "default field width");
_Static_assert(sizeof(initial_native_globals.map_tabs_order) == 12, "default field width");
_Static_assert(sizeof(initial_native_globals.map_tab_step) == 2, "default field width");
_Static_assert(sizeof(initial_native_globals.map_tab_base) == 2, "default field width");
_Static_assert(sizeof(initial_native_globals.terrain_names) == 320, "default field width");
_Static_assert(sizeof(initial_native_globals.palette_in_callbackground) == 1, "default field width");
_Static_assert(sizeof(initial_native_globals.last_loaded_bob_typ) == 1, "default field width");
_Static_assert(sizeof(initial_native_globals.sound_config_flag) == 1, "default field width");
_Static_assert(sizeof(initial_native_globals.sound_var_3) == 1, "default field width");
_Static_assert(sizeof(initial_native_globals.current_music_index) == 1, "default field width");
_Static_assert(sizeof(initial_native_globals.competition_sound_id) == 1, "default field width");
_Static_assert(sizeof(initial_native_globals.pt_random_state) == 4, "default field width");
_Static_assert(sizeof(initial_native_globals.guest_default_open_mode) == 4, "default field width");
_Static_assert(sizeof(initial_native_globals.sound_timer_period) == 2, "default field width");
_Static_assert(sizeof(initial_native_globals.guest_io_mode_capacity) == 4, "default field width");
_Static_assert(sizeof(initial_native_globals.timezone_start_rule) == 36, "default field width");
_Static_assert(sizeof(initial_native_globals.timezone_end_rule) == 36, "default field width");
_Static_assert(sizeof(initial_native_globals.timezone_seconds) == 4, "default field width");
_Static_assert(sizeof(initial_native_globals.timezone_dst_seconds) == 4, "default field width");
_Static_assert(sizeof(initial_native_globals.timezone_has_dst) == 4, "default field width");
_Static_assert(sizeof(initial_native_globals.timezone_standard_name) == 31, "default field width");
_Static_assert(sizeof(initial_native_globals.timezone_dst_name) == 31, "default field width");
_Static_assert(sizeof(initial_native_globals.guest_heap_grow_enabled) == 4, "default field width");
_Static_assert(sizeof(initial_native_globals.recipe_allocation_failure_text) == 21, "default field width");
_Static_assert(sizeof(initial_native_globals.restaurant_not_found_text) == 20, "default field width");
_Static_assert(sizeof(initial_native_globals.staff_disaster_raise_factor) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.staff_course_confirm_prompt) == 20, "default field width");
_Static_assert(sizeof(initial_native_globals.staff_pool_full_error) == 14, "default field width");
_Static_assert(sizeof(initial_native_globals.staff_hire_none_error) == 27, "default field width");
_Static_assert(sizeof(initial_native_globals.staff_hire_refusal_error) == 43, "default field width");
_Static_assert(sizeof(initial_native_globals.staff_expenses_underpaid_factor) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.staff_expenses_overpaid_factor) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.staff_daily_underpaid_factor) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.staff_daily_overpaid_factor) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.staff_fire_selection_error) == 53, "default field width");
_Static_assert(sizeof(initial_native_globals.staff_fire_prompt) == 37, "default field width");
_Static_assert(sizeof(initial_native_globals.staff_wages_selection_error) == 63, "default field width");
_Static_assert(sizeof(initial_native_globals.staff_course_selection_error) == 65, "default field width");
_Static_assert(sizeof(initial_native_globals.character_holiday_question) == 37, "default field width");
_Static_assert(sizeof(initial_native_globals.depot_quality_rejection_text) == 82, "default field width");
_Static_assert(sizeof(initial_native_globals.depot_trade_price_divisor) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.depot_trade_rating_divisor) == 4, "default field width");
_Static_assert(sizeof(initial_native_globals.contract_price_divisor) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.contract_purchase_divisor) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.contract_empty_error) == 14, "default field width");
_Static_assert(sizeof(initial_native_globals.contract_cancel_prompt) == 101, "default field width");
_Static_assert(sizeof(initial_native_globals.shop_price_divisor) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.stock_menu_demand_divisor) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.supplier_price_addon_divisor) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.supplier_price_level5_factor) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.supplier_price_level4_factor) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.supplier_price_level2_factor) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.supplier_price_level1_factor) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.contract_penalty_new_factor) == 4, "default field width");
_Static_assert(sizeof(initial_native_globals.contract_discount_factor) == 4, "default field width");
_Static_assert(sizeof(initial_native_globals.contract_discount_divisor) == 4, "default field width");
_Static_assert(sizeof(initial_native_globals.contract_charge_divisor) == 4, "default field width");
_Static_assert(sizeof(initial_native_globals.contract_fake_sum_factor) == 4, "default field width");
_Static_assert(sizeof(initial_native_globals.contract_penalty_reduction_factor) == 4, "default field width");
_Static_assert(sizeof(initial_native_globals.double_720) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.customer_other_tile_factor) == 4, "default field width");
_Static_assert(sizeof(initial_native_globals.customer_residential_tile_factor) == 4, "default field width");
_Static_assert(sizeof(initial_native_globals.float_720) == 4, "default field width");
_Static_assert(sizeof(initial_native_globals.pizza_income_price_divisor) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.pizza_income_price_offset) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.pizza_income_base_divisor) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.pizza_income_base_offset) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.pizza_price_offset) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.pizza_price_divisor) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.pizza_price_bonus_scale) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.customer_distribution_factor) == 4, "default field width");
_Static_assert(sizeof(initial_native_globals.pizza_order_random_threshold) == 4, "default field width");
_Static_assert(sizeof(initial_native_globals.restaurant_supplies_shortage_text) == 50, "default field width");
_Static_assert(sizeof(initial_native_globals.restaurant_ovens_shortage_text) == 40, "default field width");
_Static_assert(sizeof(initial_native_globals.restaurant_cooks_shortage_text) == 40, "default field width");
_Static_assert(sizeof(initial_native_globals.restaurant_waiters_shortage_text) == 42, "default field width");
_Static_assert(sizeof(initial_native_globals.restaurant_supply_percentage_scale) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.restaurant_turnover_goal_threshold) == 4, "default field width");
_Static_assert(sizeof(initial_native_globals.restaurant_sales_currency_divisor) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.attack_police_message) == 15, "default field width");
_Static_assert(sizeof(initial_native_globals.guest_type_error_text) == 10, "default field width");
_Static_assert(sizeof(initial_native_globals.ai_restaurant_upgrade_money_factor) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.ai_restaurant_memory_error) == 40, "default field width");
_Static_assert(sizeof(initial_native_globals.ai_low_rating_money_threshold) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.ai_low_default_rating_money_threshold) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.legacy_scale_word_byte_divisor) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.legacy_scale_difference_byte_divisor) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.legacy_scale_word_word_divisor) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.legacy_scale_difference_word_divisor) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.competition_challenge_format) == 94, "default field width");
_Static_assert(sizeof(initial_native_globals.competition_ai_challenge_text) == 52, "default field width");
_Static_assert(sizeof(initial_native_globals.competition_decline_text) == 40, "default field width");
_Static_assert(sizeof(initial_native_globals.competition_draw_caption) == 11, "default field width");
_Static_assert(sizeof(initial_native_globals.competition_win_caption) == 5, "default field width");
_Static_assert(sizeof(initial_native_globals.competition_minimum_money) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.competition_maximum_bet) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.competition_ai_money_fraction) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.pizza_competition_ready_text) == 13, "default field width");
_Static_assert(sizeof(initial_native_globals.ingredients_full_text) == 38, "default field width");
_Static_assert(sizeof(initial_native_globals.recipe_cost_divisor) == 4, "default field width");
_Static_assert(sizeof(initial_native_globals.disk_quit_prompt) == 14, "default field width");
_Static_assert(sizeof(initial_native_globals.character_selection_money_limit) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.selector_overwrite_prompt) == 20, "default field width");
_Static_assert(sizeof(initial_native_globals.closed_restaurant_text) == 36, "default field width");
_Static_assert(sizeof(initial_native_globals.game_over_retry_prompt) == 11, "default field width");
_Static_assert(sizeof(initial_native_globals.personal_record_save_prompt) == 47, "default field width");
_Static_assert(sizeof(initial_native_globals.game_over_low_score_text) == 90, "default field width");
_Static_assert(sizeof(initial_native_globals.animated_day_minutes) == 4, "default field width");
_Static_assert(sizeof(initial_native_globals.animated_percent_scale) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.realtor_sell_factor) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.realtor_maximum_factor) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.realtor_discount_factor) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.realtor_increase_factor) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.bank_loan_cash_limit) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.bank_investment_minimum) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.bank_investment_cash_limit) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.bank_investment_fraction) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.mafia_cost_cash_scale) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.mafia_cost_scale_limit) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.land_symbol_filename) == 16, "default field width");
_Static_assert(sizeof(initial_native_globals.no_memory_continue_prompt) == 34, "default field width");
_Static_assert(sizeof(initial_native_globals.price_display_divisor) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.font_error_text) == 13, "default field width");
_Static_assert(sizeof(initial_native_globals.legacy_round_half) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.bob_second_already_linked_error) == 42, "default field width");
_Static_assert(sizeof(initial_native_globals.bob_shared_capacity_error) == 57, "default field width");
_Static_assert(sizeof(initial_native_globals.decimal_scale_base) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.market_percent_scale) == 4, "default field width");
_Static_assert(sizeof(initial_native_globals.furniture_selection_limit_text) == 26, "default field width");
_Static_assert(sizeof(initial_native_globals.furniture_no_selection_text) == 67, "default field width");
_Static_assert(sizeof(initial_native_globals.placement_maximum_ovens_text) == 25, "default field width");
_Static_assert(sizeof(initial_native_globals.placement_maximum_items_text) == 25, "default field width");
_Static_assert(sizeof(initial_native_globals.placement_closed_text) == 120, "default field width");
_Static_assert(sizeof(initial_native_globals.placement_invalid_text) == 28, "default field width");
_Static_assert(sizeof(initial_native_globals.furniture_no_supplier_text) == 28, "default field width");
_Static_assert(sizeof(initial_native_globals.furniture_van_warning) == 113, "default field width");
_Static_assert(sizeof(initial_native_globals.furniture_info_formats) == 150, "default field width");
_Static_assert(sizeof(initial_native_globals.restaurant_close_confirm_text) == 18, "default field width");
_Static_assert(sizeof(initial_native_globals.restaurant_need_ovens_text) == 57, "default field width");
_Static_assert(sizeof(initial_native_globals.restaurant_need_chefs_text) == 65, "default field width");
_Static_assert(sizeof(initial_native_globals.restaurant_need_waiters_text) == 59, "default field width");
_Static_assert(sizeof(initial_native_globals.restaurant_need_recipes_text) == 75, "default field width");
_Static_assert(sizeof(initial_native_globals.restaurant_closed_today_text) == 40, "default field width");
_Static_assert(sizeof(initial_native_globals.restaurant_open_confirm_text) == 17, "default field width");
_Static_assert(sizeof(initial_native_globals.restaurant_rearrange_text) == 64, "default field width");
_Static_assert(sizeof(initial_native_globals.restaurant_nothing_text) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.recipe_price_limit) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.recipe_price_scale) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.recipe_load_custom_error) == 34, "default field width");
_Static_assert(sizeof(initial_native_globals.recipe_duplicate_error) == 37, "default field width");
_Static_assert(sizeof(initial_native_globals.recipe_checksum_error) == 27, "default field width");
_Static_assert(sizeof(initial_native_globals.recipe_list_save_custom_error) == 34, "default field width");
_Static_assert(sizeof(initial_native_globals.recipe_list_recreate_prompt) == 17, "default field width");
_Static_assert(sizeof(initial_native_globals.recipe_list_create_first_error) == 37, "default field width");
_Static_assert(sizeof(initial_native_globals.recipe_list_already_menu_error) == 36, "default field width");
_Static_assert(sizeof(initial_native_globals.recipe_price_prompt) == 13, "default field width");
_Static_assert(sizeof(initial_native_globals.recipe_list_menu_full_error) == 48, "default field width");
_Static_assert(sizeof(initial_native_globals.pizza_menu_keep_one_error) == 45, "default field width");
_Static_assert(sizeof(initial_native_globals.pizza_menu_remove_selection_error) == 60, "default field width");
_Static_assert(sizeof(initial_native_globals.profit_graph_scale_base) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.legacy_graph_scale_base) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.cash_graph_scale_base) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.daily_profit_scale_base) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.advert_already_active) == 46, "default field width");
_Static_assert(sizeof(initial_native_globals.market_share_price_divisor) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.market_share_base_threshold) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.insufficient_cash_text) == 30, "default field width");
_Static_assert(sizeof(initial_native_globals.ledger_limit) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.restaurant_opening_text) == 40, "default field width");
_Static_assert(sizeof(initial_native_globals.restaurant_closing_text) == 40, "default field width");
_Static_assert(sizeof(initial_native_globals.new_restaurant_memory_error) == 42, "default field width");
_Static_assert(sizeof(initial_native_globals.supplier_price_scale) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.flooring_unlock_hint) == 281, "default field width");
_Static_assert(sizeof(initial_native_globals.weekly_cash_history_limit) == 8, "default field width");
_Static_assert(sizeof(initial_native_globals.end_turn_wait_text) == 15, "default field width");
_Static_assert(sizeof(initial_native_globals.sound_binary_read_mode) == 3, "default field width");
_Static_assert(sizeof(pt_read_only_tables.intro_text_groups) == 44, "default field width");
_Static_assert(sizeof(pt_read_only_tables.staff_training_courses) == 648, "default field width");
_Static_assert(sizeof(pt_read_only_tables.curriculum_scores) == 556, "default field width");
_Static_assert(sizeof(pt_read_only_tables.bitmap_to_show_for_goal) == 101, "default field width");
_Static_assert(sizeof(pt_read_only_tables.goal_bitmap_flags) == 36, "default field width");
_Static_assert(sizeof(pt_read_only_tables.default_recipe_names) == 280, "default field width");
_Static_assert(sizeof(pt_read_only_tables.main_menu_desc) == 54, "default field width");
_Static_assert(sizeof(pt_read_only_tables.map_tab_labels) == 30, "default field width");
_Static_assert(sizeof(pt_read_only_tables.season_names) == 35, "default field width");
_Static_assert(sizeof(pt_read_only_tables.market_zone_names) == 196, "default field width");
_Static_assert(sizeof(pt_read_only_tables.furniture_category_names) == 98, "default field width");
_Static_assert(sizeof(pt_read_only_tables.month_names) == 60, "default field width");
_Static_assert(sizeof(pt_read_only_tables.ordinal_suffixes) == 12, "default field width");
_Static_assert(sizeof(pt_read_only_tables.scale10_powers) == 80, "default field width");
_Static_assert(sizeof(pt_read_only_tables.staff_hire_role_labels) == 66, "default field width");
_Static_assert(sizeof(pt_read_only_tables.contract_justification_probabilities) == 8, "default field width");
_Static_assert(sizeof(pt_read_only_tables.contract_days_literal) == 5, "default field width");
_Static_assert(sizeof(pt_read_only_tables.manager_turnover_names) == 40, "default field width");
_Static_assert(sizeof(pt_read_only_tables.attack_health_left) == 4, "default field width");
_Static_assert(sizeof(pt_read_only_tables.attack_health_right) == 4, "default field width");
_Static_assert(sizeof(pt_read_only_tables.recipe_jury_x) == 10, "default field width");
_Static_assert(sizeof(pt_read_only_tables.recipe_chart_label_x) == 4, "default field width");
_Static_assert(sizeof(pt_read_only_tables.recipe_chart_x) == 4, "default field width");
_Static_assert(sizeof(pt_read_only_tables.ingredient_trend_names) == 50, "default field width");
_Static_assert(sizeof(pt_read_only_tables.selector_patterns) == 24, "default field width");
_Static_assert(sizeof(pt_read_only_tables.animated_quality_x) == 8, "default field width");
_Static_assert(sizeof(pt_read_only_tables.animated_quality_y) == 8, "default field width");
_Static_assert(sizeof(pt_read_only_tables.animated_capacity_colors) == 16, "default field width");
_Static_assert(sizeof(pt_read_only_tables.valid_bank_account_numbers) == 32, "default field width");
_Static_assert(sizeof(pt_read_only_tables.mission_reward_bases) == 20, "default field width");
_Static_assert(sizeof(pt_read_only_tables.mission_place_articles) == 32, "default field width");
_Static_assert(sizeof(pt_read_only_tables.mission_place_article_groups) == 8, "default field width");
_Static_assert(sizeof(pt_read_only_tables.mission_place_name_prefixes) == 8, "default field width");
_Static_assert(sizeof(pt_read_only_tables.paragraph_width_characters) == 4, "default field width");
_Static_assert(sizeof(pt_read_only_tables.furniture_extras_labels) == 88, "default field width");
_Static_assert(sizeof(pt_read_only_tables.recipe_load_tag) == 5, "default field width");
_Static_assert(sizeof(pt_read_only_tables.profit_graph_colors) == 8, "default field width");
_Static_assert(sizeof(pt_read_only_tables.profit_graph_thresholds) == 32, "default field width");
_Static_assert(sizeof(pt_read_only_tables.weekly_default_staff_name) == 13, "default field width");
_Static_assert(sizeof(pt_read_only_tables.num_ingredients_in_set) == 8, "default field width");
_Static_assert(sizeof(pt_read_only_tables.customer_age_names) == 96, "default field width");
_Static_assert(sizeof(pt_read_only_tables.customer_types) == 48, "default field width");
_Static_assert(sizeof(pt_read_only_tables.property_names) == 198, "default field width");
_Static_assert(sizeof(pt_read_only_tables.property_availability_names) == 72, "default field width");
_Static_assert(sizeof(pt_read_only_tables.ingredient_popup_x) == 4, "default field width");
_Static_assert(sizeof(pt_read_only_tables.g_stock_name_kind) == 64, "default field width");
_Static_assert(sizeof(pt_read_only_tables.town_names) == 120, "default field width");
_Static_assert(sizeof(pt_read_only_tables.staff_ui_file_animation) == 40, "default field width");
_Static_assert(sizeof(pt_read_only_tables.season_map_colors) == 48, "default field width");
_Static_assert(sizeof(pt_read_only_tables.g_button_config) == 306, "default field width");
_Static_assert(sizeof(pt_read_only_tables.button_icon_ids) == 306, "default field width");
_Static_assert(sizeof(pt_read_only_tables.button_x) == 36, "default field width");
_Static_assert(sizeof(pt_read_only_tables.button_y) == 36, "default field width");
_Static_assert(sizeof(pt_read_only_tables.invoice_month_names) == 120, "default field width");
_Static_assert(sizeof(pt_read_only_tables.pizza_menu_info_separator) == 16, "default field width");
_Static_assert(sizeof(pt_read_only_tables.customer_eating_times) == 6, "default field width");
_Static_assert(sizeof(pt_read_only_tables.empty_restaurant_floor) == 32, "default field width");
_Static_assert(sizeof(pt_read_only_tables.staff_ui_manager_reactions) == 42, "default field width");
_Static_assert(sizeof(pt_read_only_tables.staff_ui_manager_behaviors) == 84, "default field width");
_Static_assert(sizeof(pt_read_only_tables.staff_course_role_labels) == 36, "default field width");
_Static_assert(sizeof(pt_read_only_tables.character_mode_names) == 40, "default field width");
_Static_assert(sizeof(pt_read_only_tables.index_to_rank_value) == 28, "default field width");
_Static_assert(sizeof(pt_read_only_tables.contract_insolvency_text) == 85, "default field width");
_Static_assert(sizeof(pt_read_only_tables.manager_role_names) == 16, "default field width");
_Static_assert(sizeof(pt_read_only_tables.country_selector_x) == 20, "default field width");
_Static_assert(sizeof(pt_read_only_tables.country_selector_y) == 20, "default field width");
_Static_assert(sizeof(pt_read_only_tables.attack_walk_images) == 4, "default field width");
_Static_assert(sizeof(pt_read_only_tables.guest_seat_x) == 4, "default field width");
_Static_assert(sizeof(pt_read_only_tables.guest_seat_y) == 4, "default field width");
_Static_assert(sizeof(pt_read_only_tables.guest_body_x) == 4, "default field width");
_Static_assert(sizeof(pt_read_only_tables.guest_body_y) == 4, "default field width");
_Static_assert(sizeof(pt_read_only_tables.guest_pose_x) == 16, "default field width");
_Static_assert(sizeof(pt_read_only_tables.guest_pose_y) == 16, "default field width");
_Static_assert(sizeof(pt_read_only_tables.attack_collision_x) == 8, "default field width");
_Static_assert(sizeof(pt_read_only_tables.attack_collision_y) == 8, "default field width");
_Static_assert(sizeof(pt_read_only_tables.attack_head0_x) == 4, "default field width");
_Static_assert(sizeof(pt_read_only_tables.attack_head0_y) == 4, "default field width");
_Static_assert(sizeof(pt_read_only_tables.attack_head1_x) == 4, "default field width");
_Static_assert(sizeof(pt_read_only_tables.attack_head1_y) == 4, "default field width");
_Static_assert(sizeof(pt_read_only_tables.guest_carry_x) == 12, "default field width");
_Static_assert(sizeof(pt_read_only_tables.guest_carry_y) == 12, "default field width");
_Static_assert(sizeof(pt_read_only_tables.attack_held_weapon_x) == 32, "default field width");
_Static_assert(sizeof(pt_read_only_tables.attack_held_weapon_y) == 32, "default field width");
_Static_assert(sizeof(pt_read_only_tables.attack_knife_x) == 40, "default field width");
_Static_assert(sizeof(pt_read_only_tables.attack_spray_x) == 40, "default field width");
_Static_assert(sizeof(pt_read_only_tables.attack_knife_y) == 40, "default field width");
_Static_assert(sizeof(pt_read_only_tables.attack_spray_y) == 40, "default field width");
_Static_assert(sizeof(pt_read_only_tables.attack_projectile_x) == 4, "default field width");
_Static_assert(sizeof(pt_read_only_tables.attack_projectile_y) == 4, "default field width");
_Static_assert(sizeof(pt_read_only_tables.attack_explosion_x) == 10, "default field width");
_Static_assert(sizeof(pt_read_only_tables.attack_explosion_y) == 10, "default field width");
_Static_assert(sizeof(pt_read_only_tables.guest_idle_script) == 24, "default field width");
_Static_assert(sizeof(pt_read_only_tables.attack_weapon_reach) == 10, "default field width");
_Static_assert(sizeof(pt_read_only_tables.attack_step_x) == 4, "default field width");
_Static_assert(sizeof(pt_read_only_tables.attack_step_y) == 4, "default field width");
_Static_assert(sizeof(pt_read_only_tables.main_menu_strings) == 2010, "default field width");
_Static_assert(sizeof(pt_read_only_tables.selector_family_titles) == 90, "default field width");
_Static_assert(sizeof(pt_read_only_tables.owner_to_color) == 18, "default field width");
_Static_assert(sizeof(pt_read_only_tables.tabs_y_values) == 6, "default field width");
_Static_assert(sizeof(pt_read_only_tables.district_map_pattern) == 16, "default field width");
_Static_assert(sizeof(pt_read_only_tables.terrain_tile_colors) == 256, "default field width");
_Static_assert(sizeof(pt_read_only_tables.month_to_arstid) == 13, "default field width");
_Static_assert(sizeof(pt_read_only_tables.animated_production_x) == 4, "default field width");
_Static_assert(sizeof(pt_read_only_tables.animated_production_y) == 4, "default field width");
_Static_assert(sizeof(pt_read_only_tables.phone_face_five_x) == 25, "default field width");
_Static_assert(sizeof(pt_read_only_tables.phone_face_five_y) == 25, "default field width");
_Static_assert(sizeof(pt_read_only_tables.phone_face_y) == 25, "default field width");
_Static_assert(sizeof(pt_read_only_tables.phone_background_face_x) == 16, "default field width");
_Static_assert(sizeof(pt_read_only_tables.phone_background_face_y) == 16, "default field width");
_Static_assert(sizeof(pt_read_only_tables.terminal_category_names) == 130, "default field width");
_Static_assert(sizeof(pt_read_only_tables.terminal_dial_messages) == 144, "default field width");
_Static_assert(sizeof(pt_read_only_tables.terminal_waiting_message) == 26, "default field width");
_Static_assert(sizeof(pt_read_only_tables.insurance_claim_reason_by_type) == 5, "default field width");
_Static_assert(sizeof(pt_read_only_tables.townhall_competition_messages) == 6, "default field width");
_Static_assert(sizeof(pt_read_only_tables.mission_item_pronouns) == 32, "default field width");
_Static_assert(sizeof(pt_read_only_tables.mission_pronouns) == 12, "default field width");
_Static_assert(sizeof(pt_read_only_tables.mission_completion_goals) == 6, "default field width");
_Static_assert(sizeof(pt_read_only_tables.arms_dealer_phone_nr) == 400, "default field width");
_Static_assert(sizeof(pt_read_only_tables.weapon_store_quantities) == 6, "default field width");
_Static_assert(sizeof(pt_read_only_tables.info_location_names) == 200, "default field width");
_Static_assert(sizeof(pt_read_only_tables.market_bar_colors) == 10, "default field width");
_Static_assert(sizeof(pt_read_only_tables.guest_food_offset_x) == 4, "default field width");
_Static_assert(sizeof(pt_read_only_tables.guest_food_offset_y) == 4, "default field width");
_Static_assert(sizeof(pt_read_only_tables.guest_lower_offset_x) == 4, "default field width");
_Static_assert(sizeof(pt_read_only_tables.guest_lower_offset_y) == 4, "default field width");
_Static_assert(sizeof(pt_read_only_tables.guest_upper_offset_x) == 4, "default field width");
_Static_assert(sizeof(pt_read_only_tables.guest_upper_offset_y) == 4, "default field width");
_Static_assert(sizeof(pt_read_only_tables.restaurant_media_labels) == 384, "default field width");
_Static_assert(sizeof(pt_read_only_tables.finance_stats_categories) == 90, "default field width");
_Static_assert(sizeof(pt_read_only_tables.monthly_receipt_labels) == 64, "default field width");
_Static_assert(sizeof(pt_read_only_tables.monthly_expense_labels) == 180, "default field width");
_Static_assert(sizeof(pt_read_only_tables.insurance_category_labels) == 100, "default field width");
_Static_assert(sizeof(pt_read_only_tables.advert_tv_frames) == 198, "default field width");
_Static_assert(sizeof(pt_read_only_tables.newspaper_article_backgrounds) == 100, "default field width");
_Static_assert(sizeof(pt_read_only_tables.guest_dos_error_errno) == 20, "default field width");
_Static_assert(sizeof(pt_read_only_tables.market_property_title) == 14, "default field width");
_Static_assert(sizeof(pt_read_only_tables.graphics_palette_filename) == 14, "default field width");
_Static_assert(sizeof(pt_read_only_tables.intro_presents) == 9, "default field width");
_Static_assert(sizeof(pt_read_only_tables.intro_logo_filename) == 17, "default field width");
_Static_assert(sizeof(pt_read_only_tables.intro_cyber_filename) == 14, "default field width");
_Static_assert(sizeof(pt_read_only_tables.intro_version) == 5, "default field width");
_Static_assert(sizeof(pt_read_only_tables.intro_title1_filename) == 15, "default field width");
_Static_assert(sizeof(pt_read_only_tables.intro_title2_filename) == 15, "default field width");
_Static_assert(sizeof(pt_read_only_tables.startup_permap_filename) == 15, "default field width");
_Static_assert(sizeof(pt_read_only_tables.startup_icons_filename) == 14, "default field width");
_Static_assert(sizeof(pt_read_only_tables.startup_k0_filename) == 11, "default field width");
_Static_assert(sizeof(pt_read_only_tables.startup_med1_filename) == 13, "default field width");
_Static_assert(sizeof(pt_read_only_tables.main_sound_config_filename) == 10, "default field width");
_Static_assert(sizeof(pt_read_only_tables.main_prefs_filename) == 6, "default field width");
_Static_assert(sizeof(pt_read_only_tables.staff_disaster_theft_text) == 65, "default field width");
_Static_assert(sizeof(pt_read_only_tables.staff_disaster_car_format) == 44, "default field width");
_Static_assert(sizeof(pt_read_only_tables.staff_ui_payment) == 8, "default field width");
_Static_assert(sizeof(pt_read_only_tables.staff_ui_pizzas_day) == 11, "default field width");
_Static_assert(sizeof(pt_read_only_tables.staff_ui_cooking) == 8, "default field width");
_Static_assert(sizeof(pt_read_only_tables.staff_ui_serving) == 8, "default field width");
_Static_assert(sizeof(pt_read_only_tables.staff_ui_management) == 11, "default field width");
_Static_assert(sizeof(pt_read_only_tables.staff_ui_experience) == 11, "default field width");
_Static_assert(sizeof(pt_read_only_tables.staff_ui_intelligence) == 13, "default field width");
_Static_assert(sizeof(pt_read_only_tables.staff_ui_trustworthiness) == 16, "default field width");
_Static_assert(sizeof(pt_read_only_tables.staff_ui_health) == 7, "default field width");
_Static_assert(sizeof(pt_read_only_tables.staff_ui_motivation) == 11, "default field width");
_Static_assert(sizeof(pt_read_only_tables.staff_application_title) == 17, "default field width");
_Static_assert(sizeof(pt_read_only_tables.staff_course_title) == 8, "default field width");
_Static_assert(sizeof(pt_read_only_tables.staff_course_name_title) == 12, "default field width");
_Static_assert(sizeof(pt_read_only_tables.staff_course_scores_title) == 7, "default field width");
_Static_assert(sizeof(pt_read_only_tables.staff_course_costs_title) == 6, "default field width");
_Static_assert(sizeof(pt_read_only_tables.staff_course_days_title) == 5, "default field width");
_Static_assert(sizeof(pt_read_only_tables.staff_ui_no_manager) == 11, "default field width");
_Static_assert(sizeof(pt_read_only_tables.staff_ui_authority) == 10, "default field width");
_Static_assert(sizeof(pt_read_only_tables.staff_ui_reactions) == 10, "default field width");
_Static_assert(sizeof(pt_read_only_tables.staff_ui_behavior_towards_staff) == 23, "default field width");
_Static_assert(sizeof(pt_read_only_tables.staff_ui_shopping_if_needed) == 19, "default field width");
_Static_assert(sizeof(pt_read_only_tables.staff_ui_change_prices) == 14, "default field width");
_Static_assert(sizeof(pt_read_only_tables.staff_ui_overdraw_account) == 17, "default field width");
_Static_assert(sizeof(pt_read_only_tables.staff_ui_turnover_decrease) == 18, "default field width");
_Static_assert(sizeof(pt_read_only_tables.staff_ui_turnover_increase) == 18, "default field width");
_Static_assert(sizeof(pt_read_only_tables.staff_ui_supply_shortage) == 16, "default field width");
_Static_assert(sizeof(pt_read_only_tables.staff_ui_staff_shortage) == 15, "default field width");
_Static_assert(sizeof(pt_read_only_tables.staff_expenses_label) == 15, "default field width");
_Static_assert(sizeof(pt_read_only_tables.staff_ui_name) == 5, "default field width");
_Static_assert(sizeof(pt_read_only_tables.staff_ui_days_absent) == 12, "default field width");
_Static_assert(sizeof(pt_read_only_tables.staff_ui_capacity) == 4, "default field width");
_Static_assert(sizeof(pt_read_only_tables.staff_ui_salary) == 4, "default field width");
_Static_assert(sizeof(pt_read_only_tables.staff_ui_course) == 7, "default field width");
_Static_assert(sizeof(pt_read_only_tables.staff_ui_sick) == 5, "default field width");
_Static_assert(sizeof(pt_read_only_tables.staff_ui_pregnant) == 9, "default field width");
_Static_assert(sizeof(pt_read_only_tables.staff_ui_chefs) == 6, "default field width");
_Static_assert(sizeof(pt_read_only_tables.staff_ui_waiters) == 8, "default field width");
_Static_assert(sizeof(pt_read_only_tables.staff_ui_number) == 8, "default field width");
_Static_assert(sizeof(pt_read_only_tables.staff_ui_total_capacity) == 16, "default field width");
_Static_assert(sizeof(pt_read_only_tables.staff_ui_average_quality) == 17, "default field width");
_Static_assert(sizeof(pt_read_only_tables.staff_ui_time_per_pizza) == 16, "default field width");
_Static_assert(sizeof(pt_read_only_tables.staff_ui_work) == 6, "default field width");
_Static_assert(sizeof(pt_read_only_tables.staff_ui_total_costs) == 12, "default field width");
_Static_assert(sizeof(pt_read_only_tables.staff_wage_unit) == 2, "default field width");
_Static_assert(sizeof(pt_read_only_tables.staff_wage_prompt) == 15, "default field width");
_Static_assert(sizeof(pt_read_only_tables.mission_completion_format) == 85, "default field width");
_Static_assert(sizeof(pt_read_only_tables.goal_name_1000_pizzas) == 33, "default field width");
_Static_assert(sizeof(pt_read_only_tables.goal_name_10000_pizzas) == 30, "default field width");
_Static_assert(sizeof(pt_read_only_tables.goal_name_profit_25000) == 39, "default field width");
_Static_assert(sizeof(pt_read_only_tables.goal_name_profit_50000) == 39, "default field width");
_Static_assert(sizeof(pt_read_only_tables.goal_name_world) == 18, "default field width");
_Static_assert(sizeof(pt_read_only_tables.character_initial_labels) == 37, "default field width");
_Static_assert(sizeof(pt_read_only_tables.target_curriculum_heading) == 24, "default field width");
_Static_assert(sizeof(pt_read_only_tables.curriculum_mission_only) == 21, "default field width");
_Static_assert(sizeof(pt_read_only_tables.curriculum_heading) == 17, "default field width");
_Static_assert(sizeof(pt_read_only_tables.character_value_labels) == 128, "default field width");
_Static_assert(sizeof(pt_read_only_tables.character_company_heading) == 17, "default field width");
_Static_assert(sizeof(pt_read_only_tables.character_company_profit) == 14, "default field width");
_Static_assert(sizeof(pt_read_only_tables.character_awards_filename) == 14, "default field width");
_Static_assert(sizeof(pt_read_only_tables.character_awards_heading) == 7, "default field width");
_Static_assert(sizeof(pt_read_only_tables.character_holiday_days) == 5, "default field width");
_Static_assert(sizeof(pt_read_only_tables.character_holiday_prompt) == 14, "default field width");
_Static_assert(sizeof(pt_read_only_tables.depot_weapon_category_label) == 9, "default field width");
_Static_assert(sizeof(pt_read_only_tables.stock_menu_depot_title) == 6, "default field width");
_Static_assert(sizeof(pt_read_only_tables.depot_weapon_bag_label) == 4, "default field width");
_Static_assert(sizeof(pt_read_only_tables.depot_town_title_format) == 17, "default field width");
_Static_assert(sizeof(pt_read_only_tables.depot_columns_label) == 23, "default field width");
_Static_assert(sizeof(pt_read_only_tables.depot_sum_value_label) == 10, "default field width");
_Static_assert(sizeof(pt_read_only_tables.depot_export_title_format) == 14, "default field width");
_Static_assert(sizeof(pt_read_only_tables.depot_sale_label) == 5, "default field width");
_Static_assert(sizeof(pt_read_only_tables.depot_purchase_label) == 7, "default field width");
_Static_assert(sizeof(pt_read_only_tables.depot_purchase_title_format) == 14, "default field width");
_Static_assert(sizeof(pt_read_only_tables.shop_price_label) == 6, "default field width");
_Static_assert(sizeof(pt_read_only_tables.depot_supplier_column_label) == 6, "default field width");
_Static_assert(sizeof(pt_read_only_tables.shop_amount_label) == 7, "default field width");
_Static_assert(sizeof(pt_read_only_tables.depot_store_column_label) == 6, "default field width");
_Static_assert(sizeof(pt_read_only_tables.contract_sum_label) == 5, "default field width");
_Static_assert(sizeof(pt_read_only_tables.depot_priorities_title) == 20, "default field width");
_Static_assert(sizeof(pt_read_only_tables.contract_discount_label) == 10, "default field width");
_Static_assert(sizeof(pt_read_only_tables.contract_charge_label) == 8, "default field width");
_Static_assert(sizeof(pt_read_only_tables.contract_terms_penalty_label) == 9, "default field width");
_Static_assert(sizeof(pt_read_only_tables.contract_debug_portrait_prompt) == 5, "default field width");
_Static_assert(sizeof(pt_read_only_tables.contract_minimum_order_format) == 17, "default field width");
_Static_assert(sizeof(pt_read_only_tables.contract_delivery_amounts_title) == 18, "default field width");
_Static_assert(sizeof(pt_read_only_tables.stock_menu_kg_label) == 3, "default field width");
_Static_assert(sizeof(pt_read_only_tables.contract_with_title) == 14, "default field width");
_Static_assert(sizeof(pt_read_only_tables.shop_total_label) == 7, "default field width");
_Static_assert(sizeof(pt_read_only_tables.contract_penalty_label) == 16, "default field width");
_Static_assert(sizeof(pt_read_only_tables.contract_amount_label) == 8, "default field width");
_Static_assert(sizeof(pt_read_only_tables.contract_as_required_label) == 12, "default field width");
_Static_assert(sizeof(pt_read_only_tables.contract_as_agreed_label) == 17, "default field width");
_Static_assert(sizeof(pt_read_only_tables.contract_quality_label) == 9, "default field width");
_Static_assert(sizeof(pt_read_only_tables.contract_remaining_term_label) == 16, "default field width");
_Static_assert(sizeof(pt_read_only_tables.contract_term_prompt) == 10, "default field width");
_Static_assert(sizeof(pt_read_only_tables.contract_delivery_space_text) == 2, "default field width");
_Static_assert(sizeof(pt_read_only_tables.shop_unavailable_price_text) == 7, "default field width");
_Static_assert(sizeof(pt_read_only_tables.shop_stock_label) == 6, "default field width");
_Static_assert(sizeof(pt_read_only_tables.stock_menu_days_label) == 5, "default field width");
_Static_assert(sizeof(pt_read_only_tables.stock_menu_stocks_title) == 7, "default field width");
_Static_assert(sizeof(pt_read_only_tables.stock_menu_kilogram_label) == 9, "default field width");
_Static_assert(sizeof(pt_read_only_tables.stock_menu_value_label) == 6, "default field width");
_Static_assert(sizeof(pt_read_only_tables.stock_menu_quality_label) == 5, "default field width");
_Static_assert(sizeof(pt_read_only_tables.stock_menu_supply_label) == 4, "default field width");
_Static_assert(sizeof(pt_read_only_tables.stock_menu_need_label) == 4, "default field width");
_Static_assert(sizeof(pt_read_only_tables.stock_menu_use_label) == 4, "default field width");
_Static_assert(sizeof(pt_read_only_tables.depot_central_label) == 8, "default field width");
_Static_assert(sizeof(pt_read_only_tables.depot_capacity_label) == 9, "default field width");
_Static_assert(sizeof(pt_read_only_tables.depot_in_use_label) == 7, "default field width");
_Static_assert(sizeof(pt_read_only_tables.depot_stolen_format) == 82, "default field width");
_Static_assert(sizeof(pt_read_only_tables.file_write_mode) == 2, "default field width");
_Static_assert(sizeof(pt_read_only_tables.file_readwrite_mode) == 4, "default field width");
_Static_assert(sizeof(pt_read_only_tables.restaurant_electricity_label) == 13, "default field width");
_Static_assert(sizeof(pt_read_only_tables.restaurant_slot_income_label) == 22, "default field width");
_Static_assert(sizeof(pt_read_only_tables.restaurant_invoice_total_label) == 7, "default field width");
_Static_assert(sizeof(pt_read_only_tables.country_market_leader_label) == 14, "default field width");
_Static_assert(sizeof(pt_read_only_tables.country_no_market_leader_label) == 11, "default field width");
_Static_assert(sizeof(pt_read_only_tables.country_map_filename) == 16, "default field width");
_Static_assert(sizeof(pt_read_only_tables.country_no_city_message) == 18, "default field width");
_Static_assert(sizeof(pt_read_only_tables.country_insufficient_money_message) == 18, "default field width");
_Static_assert(sizeof(pt_read_only_tables.town_initializing_caption) == 13, "default field width");
_Static_assert(sizeof(pt_read_only_tables.town_initializing_ellipsis) == 4, "default field width");
_Static_assert(sizeof(pt_read_only_tables.staff_market_filename) == 11, "default field width");
_Static_assert(sizeof(pt_read_only_tables.attack_police_argument) == 2, "default field width");
_Static_assert(sizeof(pt_read_only_tables.competition_pizza_title) == 6, "default field width");
_Static_assert(sizeof(pt_read_only_tables.competition_title) == 12, "default field width");
_Static_assert(sizeof(pt_read_only_tables.competition_dollars_unit) == 3, "default field width");
_Static_assert(sizeof(pt_read_only_tables.competition_bet_prompt) == 21, "default field width");
_Static_assert(sizeof(pt_read_only_tables.competition_invitation_format) == 58, "default field width");
_Static_assert(sizeof(pt_read_only_tables.competition_stake_prompt) == 23, "default field width");
_Static_assert(sizeof(pt_read_only_tables.competition_speed_prompt) == 7, "default field width");
_Static_assert(sizeof(pt_read_only_tables.competition_jury_caption) == 14, "default field width");
_Static_assert(sizeof(pt_read_only_tables.competition_time_format) == 3, "default field width");
_Static_assert(sizeof(pt_read_only_tables.recipe_new_name) == 6, "default field width");
_Static_assert(sizeof(pt_read_only_tables.recipe_number_format) == 16, "default field width");
_Static_assert(sizeof(pt_read_only_tables.recipe_calories_format) == 16, "default field width");
_Static_assert(sizeof(pt_read_only_tables.recipe_fat_format) == 16, "default field width");
_Static_assert(sizeof(pt_read_only_tables.recipe_sweet_format) == 16, "default field width");
_Static_assert(sizeof(pt_read_only_tables.recipe_salty_format) == 16, "default field width");
_Static_assert(sizeof(pt_read_only_tables.recipe_hot_format) == 16, "default field width");
_Static_assert(sizeof(pt_read_only_tables.recipe_appearance_format) == 15, "default field width");
_Static_assert(sizeof(pt_read_only_tables.recipe_refinement_format) == 15, "default field width");
_Static_assert(sizeof(pt_read_only_tables.recipe_trend_format) == 15, "default field width");
_Static_assert(sizeof(pt_read_only_tables.recipe_imitation_format) == 17, "default field width");
_Static_assert(sizeof(pt_read_only_tables.recipe_normality_format) == 17, "default field width");
_Static_assert(sizeof(pt_read_only_tables.recipe_jury_filename) == 13, "default field width");
_Static_assert(sizeof(pt_read_only_tables.recipe_jury_compare_text) == 34, "default field width");
_Static_assert(sizeof(pt_read_only_tables.recipe_jury_evaluate_text) == 26, "default field width");
_Static_assert(sizeof(pt_read_only_tables.recipe_jury_competition_text) == 20, "default field width");
_Static_assert(sizeof(pt_read_only_tables.recipe_jury_total_text) == 6, "default field width");
_Static_assert(sizeof(pt_read_only_tables.recipe_view_info_text) == 6, "default field width");
_Static_assert(sizeof(pt_read_only_tables.recipe_view_recipe_text) == 7, "default field width");
_Static_assert(sizeof(pt_read_only_tables.recipe_view_here_text) == 5, "default field width");
_Static_assert(sizeof(pt_read_only_tables.recipe_view_all_text) == 4, "default field width");
_Static_assert(sizeof(pt_read_only_tables.recipe_view_costs_text) == 6, "default field width");
_Static_assert(sizeof(pt_read_only_tables.recipe_view_price_text) == 6, "default field width");
_Static_assert(sizeof(pt_read_only_tables.recipe_view_sales_text) == 6, "default field width");
_Static_assert(sizeof(pt_read_only_tables.recipe_view_profit_text) == 7, "default field width");
_Static_assert(sizeof(pt_read_only_tables.recipe_view_popularity_text) == 11, "default field width");
_Static_assert(sizeof(pt_read_only_tables.recipe_view_gram_text) == 2, "default field width");
_Static_assert(sizeof(pt_read_only_tables.recipe_view_total_weight_text) == 15, "default field width");
_Static_assert(sizeof(pt_read_only_tables.pizza_competition_title) == 18, "default field width");
_Static_assert(sizeof(pt_read_only_tables.pizza_competition_separator) == 17, "default field width");
_Static_assert(sizeof(pt_read_only_tables.pizza_competition_original_label) == 17, "default field width");
_Static_assert(sizeof(pt_read_only_tables.pizza_competition_level_label) == 7, "default field width");
_Static_assert(sizeof(pt_read_only_tables.pizza_competition_ingredient_label) == 20, "default field width");
_Static_assert(sizeof(pt_read_only_tables.ingredient_grammes_format) == 11, "default field width");
_Static_assert(sizeof(pt_read_only_tables.ingredient_calories_format) == 12, "default field width");
_Static_assert(sizeof(pt_read_only_tables.ingredient_intensity_format) == 14, "default field width");
_Static_assert(sizeof(pt_read_only_tables.ingredient_fat_format) == 10, "default field width");
_Static_assert(sizeof(pt_read_only_tables.ingredient_sweet_format) == 10, "default field width");
_Static_assert(sizeof(pt_read_only_tables.ingredient_salty_format) == 10, "default field width");
_Static_assert(sizeof(pt_read_only_tables.ingredient_hot_format) == 10, "default field width");
_Static_assert(sizeof(pt_read_only_tables.ingredient_trend_format) == 10, "default field width");
_Static_assert(sizeof(pt_read_only_tables.ingredient_trend_name_format) == 5, "default field width");
_Static_assert(sizeof(pt_read_only_tables.recipe_name_prompt) == 16, "default field width");
_Static_assert(sizeof(pt_read_only_tables.disk_menu_caption) == 10, "default field width");
_Static_assert(sizeof(pt_read_only_tables.character_selection_level_caption) == 13, "default field width");
_Static_assert(sizeof(pt_read_only_tables.highscores_filename) == 15, "default field width");
_Static_assert(sizeof(pt_read_only_tables.options_on_label) == 3, "default field width");
_Static_assert(sizeof(pt_read_only_tables.options_off_label) == 4, "default field width");
_Static_assert(sizeof(pt_read_only_tables.options_prefs_filename) == 6, "default field width");
_Static_assert(sizeof(pt_read_only_tables.mission_selection_caption) == 15, "default field width");
_Static_assert(sizeof(pt_read_only_tables.main_menu_background_filename) == 13, "default field width");
_Static_assert(sizeof(pt_read_only_tables.game_screen_background_filename) == 11, "default field width");
_Static_assert(sizeof(pt_read_only_tables.selector_save_label) == 5, "default field width");
_Static_assert(sizeof(pt_read_only_tables.selector_load_label) == 5, "default field width");
_Static_assert(sizeof(pt_read_only_tables.selector_game_directory) == 5, "default field width");
_Static_assert(sizeof(pt_read_only_tables.selector_no_games) == 16, "default field width");
_Static_assert(sizeof(pt_read_only_tables.selector_no_characters) == 26, "default field width");
_Static_assert(sizeof(pt_read_only_tables.selector_no_missions) == 24, "default field width");
_Static_assert(sizeof(pt_read_only_tables.selector_new_name) == 13, "default field width");
_Static_assert(sizeof(pt_read_only_tables.selector_filename_prefix) == 6, "default field width");
_Static_assert(sizeof(pt_read_only_tables.selector_error_name) == 6, "default field width");
_Static_assert(sizeof(pt_read_only_tables.player_stats_money_label) == 6, "default field width");
_Static_assert(sizeof(pt_read_only_tables.player_stats_age_label) == 4, "default field width");
_Static_assert(sizeof(pt_read_only_tables.player_stats_popularity_label) == 11, "default field width");
_Static_assert(sizeof(pt_read_only_tables.player_stats_health_label) == 7, "default field width");
_Static_assert(sizeof(pt_read_only_tables.player_stats_energy_label) == 7, "default field width");
_Static_assert(sizeof(pt_read_only_tables.player_stats_intelligence_label) == 13, "default field width");
_Static_assert(sizeof(pt_read_only_tables.player_stats_charisma_label) == 9, "default field width");
_Static_assert(sizeof(pt_read_only_tables.player_stats_dexterity_label) == 10, "default field width");
_Static_assert(sizeof(pt_read_only_tables.player_stats_personality_label) == 12, "default field width");
_Static_assert(sizeof(pt_read_only_tables.player_stats_cooking_label) == 8, "default field width");
_Static_assert(sizeof(pt_read_only_tables.player_stats_courage_label) == 8, "default field width");
_Static_assert(sizeof(pt_read_only_tables.attack_action_caption) == 14, "default field width");
_Static_assert(sizeof(pt_read_only_tables.attack_weapon_caption) == 14, "default field width");
_Static_assert(sizeof(pt_read_only_tables.mission_invalid_place) == 8, "default field width");
_Static_assert(sizeof(pt_read_only_tables.poi_info_formats) == 93, "default field width");
_Static_assert(sizeof(pt_read_only_tables.poi_realtor_text) == 9, "default field width");
_Static_assert(sizeof(pt_read_only_tables.creating_map_text) == 16, "default field width");
_Static_assert(sizeof(pt_read_only_tables.maestro_background_filename) == 16, "default field width");
_Static_assert(sizeof(pt_read_only_tables.high_score_award_text) == 118, "default field width");
_Static_assert(sizeof(pt_read_only_tables.high_score_name_format) == 10, "default field width");
_Static_assert(sizeof(pt_read_only_tables.high_score_name_prompt) == 6, "default field width");
_Static_assert(sizeof(pt_read_only_tables.personal_record_directory) == 6, "default field width");
_Static_assert(sizeof(pt_read_only_tables.personal_record_fallback) == 6, "default field width");
_Static_assert(sizeof(pt_read_only_tables.ending_background_filename) == 13, "default field width");
_Static_assert(sizeof(pt_read_only_tables.animated_quality_caption) == 18, "default field width");
_Static_assert(sizeof(pt_read_only_tables.animated_reputation_caption) == 22, "default field width");
_Static_assert(sizeof(pt_read_only_tables.animated_no_text) == 3, "default field width");
_Static_assert(sizeof(pt_read_only_tables.animated_orders_text) == 7, "default field width");
_Static_assert(sizeof(pt_read_only_tables.animated_output_caption) == 7, "default field width");
_Static_assert(sizeof(pt_read_only_tables.animated_demand_text) == 7, "default field width");
_Static_assert(sizeof(pt_read_only_tables.animated_capacity_text) == 9, "default field width");
_Static_assert(sizeof(pt_read_only_tables.animated_popularity_caption) == 11, "default field width");
_Static_assert(sizeof(pt_read_only_tables.animated_customers_caption) == 15, "default field width");
_Static_assert(sizeof(pt_read_only_tables.animated_takings_text) == 8, "default field width");
_Static_assert(sizeof(pt_read_only_tables.animated_expenses_text) == 9, "default field width");
_Static_assert(sizeof(pt_read_only_tables.bank_transfer_amount_text) == 7, "default field width");
_Static_assert(sizeof(pt_read_only_tables.bank_transfer_account_text) == 13, "default field width");
_Static_assert(sizeof(pt_read_only_tables.townhall_annual_pizza_announcement) == 93, "default field width");
_Static_assert(sizeof(pt_read_only_tables.townhall_annual_recipe_name_format) == 20, "default field width");
_Static_assert(sizeof(pt_read_only_tables.terminal_new_number_prompt) == 13, "default field width");
_Static_assert(sizeof(pt_read_only_tables.terminal_no_entries) == 19, "default field width");
_Static_assert(sizeof(pt_read_only_tables.terminal_title) == 31, "default field width");
_Static_assert(sizeof(pt_read_only_tables.terminal_loading_message) == 30, "default field width");
_Static_assert(sizeof(pt_read_only_tables.mission_completion_phone_text) == 3, "default field width");
_Static_assert(sizeof(pt_read_only_tables.mafia_failed_business_text) == 124, "default field width");
_Static_assert(sizeof(pt_read_only_tables.invoice_title) == 8, "default field width");
_Static_assert(sizeof(pt_read_only_tables.list_percent_text) == 2, "default field width");
_Static_assert(sizeof(pt_read_only_tables.dollar_alignment_text) == 2, "default field width");
_Static_assert(sizeof(pt_read_only_tables.advert_dollar) == 2, "default field width");
_Static_assert(sizeof(pt_read_only_tables.info_box_formats) == 272, "default field width");
_Static_assert(sizeof(pt_read_only_tables.restaurant_name_format) == 9, "default field width");
_Static_assert(sizeof(pt_read_only_tables.ok_text) == 3, "default field width");
_Static_assert(sizeof(pt_read_only_tables.yes_text) == 4, "default field width");
_Static_assert(sizeof(pt_read_only_tables.no_text) == 3, "default field width");
_Static_assert(sizeof(pt_read_only_tables.staff_disaster_question_format) == 7, "default field width");
_Static_assert(sizeof(pt_read_only_tables.market_low) == 4, "default field width");
_Static_assert(sizeof(pt_read_only_tables.market_very_little) == 12, "default field width");
_Static_assert(sizeof(pt_read_only_tables.market_little) == 7, "default field width");
_Static_assert(sizeof(pt_read_only_tables.market_average) == 8, "default field width");
_Static_assert(sizeof(pt_read_only_tables.market_above_average) == 14, "default field width");
_Static_assert(sizeof(pt_read_only_tables.market_high) == 5, "default field width");
_Static_assert(sizeof(pt_read_only_tables.market_very_high) == 10, "default field width");
_Static_assert(sizeof(pt_read_only_tables.market_extremely_high) == 15, "default field width");
_Static_assert(sizeof(pt_read_only_tables.market_out_of_range) == 13, "default field width");
_Static_assert(sizeof(pt_read_only_tables.market_premises_label) == 23, "default field width");
_Static_assert(sizeof(pt_read_only_tables.market_rent_label) == 12, "default field width");
_Static_assert(sizeof(pt_read_only_tables.market_per_square_metre) == 5, "default field width");
_Static_assert(sizeof(pt_read_only_tables.market_price_label) == 13, "default field width");
_Static_assert(sizeof(pt_read_only_tables.market_tendency_label) == 10, "default field width");
_Static_assert(sizeof(pt_read_only_tables.market_falling_label) == 8, "default field width");
_Static_assert(sizeof(pt_read_only_tables.market_rising_label) == 7, "default field width");
_Static_assert(sizeof(pt_read_only_tables.market_average_size_label) == 14, "default field width");
_Static_assert(sizeof(pt_read_only_tables.market_square_metres) == 4, "default field width");
_Static_assert(sizeof(pt_read_only_tables.market_rooms_label) == 7, "default field width");
_Static_assert(sizeof(pt_read_only_tables.market_to_rent_label) == 9, "default field width");
_Static_assert(sizeof(pt_read_only_tables.market_for_sale_label) == 10, "default field width");
_Static_assert(sizeof(pt_read_only_tables.market_not_available_label) == 15, "default field width");
_Static_assert(sizeof(pt_read_only_tables.market_taste_title) == 6, "default field width");
_Static_assert(sizeof(pt_read_only_tables.market_rank_dash) == 2, "default field width");
_Static_assert(sizeof(pt_read_only_tables.market_chain_title) == 21, "default field width");
_Static_assert(sizeof(pt_read_only_tables.floor_zero_filename) == 11, "default field width");
_Static_assert(sizeof(pt_read_only_tables.furniture_inventory_num_text) == 4, "default field width");
_Static_assert(sizeof(pt_read_only_tables.furniture_inventory_name_text) == 5, "default field width");
_Static_assert(sizeof(pt_read_only_tables.furniture_equipment_title) == 10, "default field width");
_Static_assert(sizeof(pt_read_only_tables.furniture_in_use_text) == 8, "default field width");
_Static_assert(sizeof(pt_read_only_tables.furniture_value_text) == 7, "default field width");
_Static_assert(sizeof(pt_read_only_tables.furniture_quality_text) == 9, "default field width");
_Static_assert(sizeof(pt_read_only_tables.poi_capacity_text) == 10, "default field width");
_Static_assert(sizeof(pt_read_only_tables.furniture_seats_text) == 6, "default field width");
_Static_assert(sizeof(pt_read_only_tables.furniture_per_day_text) == 8, "default field width");
_Static_assert(sizeof(pt_read_only_tables.furniture_kitchenware_title) == 12, "default field width");
_Static_assert(sizeof(pt_read_only_tables.furniture_ovens_text) == 7, "default field width");
_Static_assert(sizeof(pt_read_only_tables.furniture_bake_time_text) == 17, "default field width");
_Static_assert(sizeof(pt_read_only_tables.furniture_minutes_text) == 8, "default field width");
_Static_assert(sizeof(pt_read_only_tables.furniture_baking_capacity_text) == 17, "default field width");
_Static_assert(sizeof(pt_read_only_tables.furniture_number_text) == 7, "default field width");
_Static_assert(sizeof(pt_read_only_tables.furniture_floor_title) == 15, "default field width");
_Static_assert(sizeof(pt_read_only_tables.furniture_extras_title) == 7, "default field width");
_Static_assert(sizeof(pt_read_only_tables.furniture_floor_patterns_filename) == 14, "default field width");
_Static_assert(sizeof(pt_read_only_tables.furniture_price_text) == 7, "default field width");
_Static_assert(sizeof(pt_read_only_tables.furniture_capacity_format) == 19, "default field width");
_Static_assert(sizeof(pt_read_only_tables.furniture_baking_time_format) == 21, "default field width");
_Static_assert(sizeof(pt_read_only_tables.furniture_quality_format) == 12, "default field width");
_Static_assert(sizeof(pt_read_only_tables.furniture_invoice_caption) == 10, "default field width");
_Static_assert(sizeof(pt_read_only_tables.furniture_purchase_thanks) == 25, "default field width");
_Static_assert(sizeof(pt_read_only_tables.placement_sell_format) == 39, "default field width");
_Static_assert(sizeof(pt_read_only_tables.furniture_suppliers_caption) == 10, "default field width");
_Static_assert(sizeof(pt_read_only_tables.customer_ratings_menu_title) == 5, "default field width");
_Static_assert(sizeof(pt_read_only_tables.customer_ratings_furniture_title) == 10, "default field width");
_Static_assert(sizeof(pt_read_only_tables.customer_ratings_title) == 13, "default field width");
_Static_assert(sizeof(pt_read_only_tables.furniture_menu_caption) == 11, "default field width");
_Static_assert(sizeof(pt_read_only_tables.furniture_equipment_caption) == 10, "default field width");
_Static_assert(sizeof(pt_read_only_tables.restaurant_media_prompt) == 16, "default field width");
_Static_assert(sizeof(pt_read_only_tables.intro_music_filename) == 9, "default field width");
_Static_assert(sizeof(pt_read_only_tables.recipe_list_title) == 12, "default field width");
_Static_assert(sizeof(pt_read_only_tables.recipe_list_new) == 6, "default field width");
_Static_assert(sizeof(pt_read_only_tables.recipe_number_unit) == 2, "default field width");
_Static_assert(sizeof(pt_read_only_tables.recipe_load_prompt) == 19, "default field width");
_Static_assert(sizeof(pt_read_only_tables.recipe_save_prompt) == 18, "default field width");
_Static_assert(sizeof(pt_read_only_tables.recipe_save_tag) == 5, "default field width");
_Static_assert(sizeof(pt_read_only_tables.pizza_menu_sales_title) == 14, "default field width");
_Static_assert(sizeof(pt_read_only_tables.pizza_menu_title) == 5, "default field width");
_Static_assert(sizeof(pt_read_only_tables.pizza_menu_sales_label) == 6, "default field width");
_Static_assert(sizeof(pt_read_only_tables.pizza_menu_turnover_label) == 10, "default field width");
_Static_assert(sizeof(pt_read_only_tables.pizza_menu_popularity_label) == 14, "default field width");
_Static_assert(sizeof(pt_read_only_tables.pizza_menu_demand_label) == 8, "default field width");
_Static_assert(sizeof(pt_read_only_tables.pizza_menu_info_sales_label) == 7, "default field width");
_Static_assert(sizeof(pt_read_only_tables.cash_flow_caption) == 10, "default field width");
_Static_assert(sizeof(pt_read_only_tables.profit_graph_weeks) == 6, "default field width");
_Static_assert(sizeof(pt_read_only_tables.daily_profit_caption) == 14, "default field width");
_Static_assert(sizeof(pt_read_only_tables.profit_graph_days) == 5, "default field width");
_Static_assert(sizeof(pt_read_only_tables.balance_labels) == 118, "default field width");
_Static_assert(sizeof(pt_read_only_tables.weekly_profit_text) == 7, "default field width");
_Static_assert(sizeof(pt_read_only_tables.finance_stats_labels) == 110, "default field width");
_Static_assert(sizeof(pt_read_only_tables.monthly_overview_caption) == 17, "default field width");
_Static_assert(sizeof(pt_read_only_tables.monthly_result_caption) == 8, "default field width");
_Static_assert(sizeof(pt_read_only_tables.monthly_restaurants_caption) == 12, "default field width");
_Static_assert(sizeof(pt_read_only_tables.daily_settlement_text) == 17, "default field width");
_Static_assert(sizeof(pt_read_only_tables.daily_day_before_text) == 11, "default field width");
_Static_assert(sizeof(pt_read_only_tables.daily_yesterday_text) == 10, "default field width");
_Static_assert(sizeof(pt_read_only_tables.daily_today_text) == 6, "default field width");
_Static_assert(sizeof(pt_read_only_tables.daily_orders_text) == 7, "default field width");
_Static_assert(sizeof(pt_read_only_tables.daily_pizzas_sold_text) == 12, "default field width");
_Static_assert(sizeof(pt_read_only_tables.daily_delivery_capacity_text) == 18, "default field width");
_Static_assert(sizeof(pt_read_only_tables.daily_demand_value_text) == 13, "default field width");
_Static_assert(sizeof(pt_read_only_tables.weekly_sales_text) == 6, "default field width");
_Static_assert(sizeof(pt_read_only_tables.daily_ingredients_used_text) == 17, "default field width");
_Static_assert(sizeof(pt_read_only_tables.daily_expenditure_text) == 12, "default field width");
_Static_assert(sizeof(pt_read_only_tables.weekly_week_text) == 7, "default field width");
_Static_assert(sizeof(pt_read_only_tables.weekly_takings_text) == 8, "default field width");
_Static_assert(sizeof(pt_read_only_tables.weekly_items_text) == 6, "default field width");
_Static_assert(sizeof(pt_read_only_tables.weekly_costs_text) == 6, "default field width");
_Static_assert(sizeof(pt_read_only_tables.weekly_total_text) == 7, "default field width");
_Static_assert(sizeof(pt_read_only_tables.weekly_expenses_text) == 9, "default field width");
_Static_assert(sizeof(pt_read_only_tables.weekly_ingredients_text) == 15, "default field width");
_Static_assert(sizeof(pt_read_only_tables.weekly_cooks_text) == 7, "default field width");
_Static_assert(sizeof(pt_read_only_tables.weekly_waiters_text) == 8, "default field width");
_Static_assert(sizeof(pt_read_only_tables.weekly_managers_text) == 10, "default field width");
_Static_assert(sizeof(pt_read_only_tables.restaurant_overview_day) == 4, "default field width");
_Static_assert(sizeof(pt_read_only_tables.restaurant_overview_week) == 5, "default field width");
_Static_assert(sizeof(pt_read_only_tables.restaurant_overview_caption) == 20, "default field width");
_Static_assert(sizeof(pt_read_only_tables.rest_sales_caption) == 12, "default field width");
_Static_assert(sizeof(pt_read_only_tables.rest_customers_caption) == 10, "default field width");
_Static_assert(sizeof(pt_read_only_tables.rest_weekly_caption) == 16, "default field width");
_Static_assert(sizeof(pt_read_only_tables.rest_weekly_columns) == 33, "default field width");
_Static_assert(sizeof(pt_read_only_tables.advert_output_label) == 8, "default field width");
_Static_assert(sizeof(pt_read_only_tables.advert_costs_label) == 7, "default field width");
_Static_assert(sizeof(pt_read_only_tables.advert_tv_title) == 7, "default field width");
_Static_assert(sizeof(pt_read_only_tables.advert_tv_director) == 8, "default field width");
_Static_assert(sizeof(pt_read_only_tables.advert_days) == 5, "default field width");
_Static_assert(sizeof(pt_read_only_tables.advert_misc_filename) == 15, "default field width");
_Static_assert(sizeof(pt_read_only_tables.end_turn_phone_text) == 2, "default field width");
_Static_assert(sizeof(pt_read_only_tables.periodic_extortion_text) == 3, "default field width");
_Static_assert(sizeof(pt_read_only_tables.cash_goal_thresholds) == 48, "default field width");
_Static_assert(sizeof(pt_read_only_tables.delayed_message_format) == 21, "default field width");
_Static_assert(sizeof(pt_read_only_tables.overdrawn_account_text) == 116, "default field width");
_Static_assert(sizeof(pt_read_only_tables.weekly_profit_caption) == 16, "default field width");
_Static_assert(sizeof(pt_read_only_tables.monthly_cost_caption) == 16, "default field width");
_Static_assert(sizeof(pt_read_only_tables.monthly_repayment_caption) == 11, "default field width");
_Static_assert(sizeof(pt_read_only_tables.monthly_interest_caption) == 10, "default field width");
_Static_assert(sizeof(pt_read_only_tables.monthly_insurance_caption) == 11, "default field width");
_Static_assert(sizeof(pt_read_only_tables.monthly_investment_interest_caption) == 15, "default field width");
_Static_assert(sizeof(pt_read_only_tables.monthly_rent_caption) == 6, "default field width");
_Static_assert(sizeof(pt_read_only_tables.monthly_total_caption) == 7, "default field width");
_Static_assert(sizeof(pt_read_only_tables.player_card_dot) == 2, "default field width");
_Static_assert(sizeof(pt_read_only_tables.sound_config_read_mode) == 3, "default field width");
_Static_assert(sizeof(pt_read_only_tables.sound_config_filename) == 10, "default field width");
_Static_assert(sizeof(pt_read_only_tables.sound_pcm_driver_filename) == 12, "default field width");
_Static_assert(sizeof(pt_read_only_tables.sound_vector_filename) == 11, "default field width");
_Static_assert(sizeof(pt_read_only_tables.timezone_env_name) == 3, "default field width");
_Static_assert(sizeof(pt_read_only_tables.watcom_character_classes) == 256, "default field width");
_Static_assert(sizeof(pt_read_only_tables.price_parser_positive_infinity) == 8, "default field width");
_Static_assert(sizeof(pt_read_only_tables.scale10_positive_max) == 8, "default field width");
_Static_assert(sizeof(pt_read_only_tables.scale10_negative_max) == 8, "default field width");
_Static_assert(sizeof(pt_read_only_tables.time_common_month_days) == 26, "default field width");
_Static_assert(sizeof(pt_read_only_tables.time_leap_month_days) == 26, "default field width");

bool pt_native_defaults_load(const char *directory) {
  u8 *data = NULL;
  size_t size = 0;
  // clang-format off
  static const char *const names[] = {
  "pt.exe",
  "pt_original.exe"
};
  // clang-format on
  bool found = false;
  for (unsigned i = 0; i < 2 && !found; ++i) {
    if (!pt_game_data_read_file(directory, names[i], &data, &size))
      continue;
    u8 digest[32];
    char hash[65];
    static const char hex[] = "0123456789abcdef";
    pt_sha256(data, size, digest);
    for (unsigned j = 0; j < 32; ++j) {
      hash[j * 2] = hex[digest[j] >> 4];
      hash[j * 2 + 1] = hex[digest[j] & 15];
    }
    hash[64] = 0;
    found = size == 470411 && !strcmp(hash, "9cce82355b466fc0e47c51ae1de4ba850a666b5f12c3831ee19ba5c4c4ef6add");
    if (!found) {
      free(data);
      data = NULL;
    }
  }
  if (!found) {
    fputs("Game assets: verified English DOS V2.0 PT.EXE or PT_Original.EXE is required.\n", stderr);
    return false;
  }
  memset(&initial_native_globals, 0, sizeof(initial_native_globals));
  memset(&pt_read_only_tables, 0, sizeof(pt_read_only_tables));
  memset(&pt_interface_strings, 0, sizeof(pt_interface_strings));
  for (size_t i = 0; i < sizeof(default_slices) / sizeof(*default_slices); ++i) {
    const PTDefaultSlice *slice = &default_slices[i];
    size_t offset = (size_t)slice->page * 4096 + slice->offset;
    if (offset > size || slice->size > size - offset) {
      free(data);
      return false;
    }
    memcpy(slice->destination, data + offset, slice->size);
  }
  free(data);
  initial_native_globals.guest_heap_break = pt_public_heap_limit();
  initial_native_globals.guest_psp_selector = 0x24u;
  initial_native_globals.state_slot_012 = 1;
  initial_native_globals.guest_heap_growth_minimum = 0x8000u;
  initial_native_globals.sound_driver_interrupt = 0x80u;
  initial_native_globals.delay_polls_per_second = 0x12u;
  return true;
}
void pt_native_initial_data_apply(void) {
  pt_native_globals = initial_native_globals;
}
