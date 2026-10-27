// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton

#ifndef PT_SAVED_GLOBALS_H
#define PT_SAVED_GLOBALS_H
#include "globals.h"
#include "savegame_format.h"
#pragma pack(push, 1)
typedef struct PTNamedMemory {
  u8 null_identity;

  u8 saved_staff_hire_role_labels[2][3][11];

  u8 saved_contract_justification_probabilities[8];

  u8 saved_contract_days_literal[5];

  u8 saved_manager_turnover_names[2][20];

  i16 saved_attack_health_left[2];

  i16 saved_attack_health_right[2];

  u16 saved_recipe_jury_x[5];

  u16 saved_recipe_chart_label_x[2];

  u16 saved_recipe_chart_x[2];

  u8 saved_ingredient_trend_names[5][10];

  u8 saved_selector_patterns[3][8];

  i16 saved_animated_quality_x[4];

  i16 saved_animated_quality_y[4];

  u8 saved_animated_capacity_colors[16];

  u32 saved_valid_bank_account_numbers[8];

  u32 saved_mission_reward_bases[5];

  u8 saved_mission_place_articles[4][8];

  u8 saved_mission_place_article_groups[8];

  u8 saved_mission_place_name_prefixes[8];

  u16 saved_paragraph_width_characters[2];

  u8 saved_furniture_extras_labels[4][22];

  u8 saved_recipe_load_tag[5];

  u8 saved_profit_graph_colors[8];

  i32 saved_profit_graph_thresholds[8];

  u8 saved_weekly_default_staff_name[13];

  u8 saved_sound_vector_registers[50];

  u16 saved_sound_vector_previous_offset;

  u16 saved_sound_vector_previous_segment;

  u16 saved_mouse_buttons;

  u32 saved_input_event_filter;

  u16 saved_last_bios_key;

  i16 saved_command_line_arg;

  u8 saved_serialized_display_timing[34];

  u16 saved_in_bitblt;

  u16 saved_mouse_pos_x;

  u16 saved_mouse_pos_y;

  u16 saved_mouse_move_pending;

  u16 saved_pending_mouse_x;

  u16 saved_pending_mouse_y;

  u16 saved_cursor_offset_x;

  u16 saved_cursor_offset_y;

  u16 saved_cursor_is_visible;

  u16 saved_did_hide_mouse_cursor;

  u16 saved_cursor_hide_depth;

  u16 saved_halve_mouse_x;

  union {
    u8 saved_serialized_cursor_screen_buffer[4];
    u32 saved_serialized_cursor_screen_buffer_word;
  };

  u16 saved_mouse_binding_count;

  PTSavedMousePositionBinding saved_serialized_mouse_position_bindings[5];

  u8 saved_transparent_color;

  PTSavedBitmap saved_serialized_glyph_bitmap;

  u8 saved_glyph_pixels[900];

  u32 saved_input_queue_capacity;

  u32 saved_input_queue_head;

  u32 saved_input_queue_count;

  InputEvent saved_input_events[80];

  u16 saved_joystick_center_x;

  u16 saved_joystick_center_y;

  SpriteRect saved_smallrects[19];

  u8 saved_num_ingredients_in_set[8];

  u8 saved_customer_age_names[6][16];

  u8 saved_customer_types[8][6];

  u8 saved_property_names[11][18];

  u8 saved_property_availability_names[4][18];

  u8 saved_ingredient_popup_x[4];

  u8 saved_g_stock_name_kind[4][16];

  FurnitureDesc saved_furniture_desc[164];

  u8 saved_furniture_supplier_count;

  FurnitureSupplier saved_furniture_suppliers[36];

  i32 saved_exit_code;

  u8 saved_force_map_input;

  u8 saved_days_in_month[13];

  PTSavedLoadFontDesc saved_serialized_menu_font;

  PTSavedLoadFontDesc saved_serialized_ruby_font;

  PTSavedLoadFontDesc saved_serialized_font_desc_cstandar;

  PTSavedLoadFontDesc saved_serialized_font_desc_piznumb;

  PTSavedLoadFontDesc saved_serialized_font_desc_astandar;

  PTSavedLoadFontDesc saved_serialized_font_desc_supersma;

  union {
    u8 saved_serialized_loaded_font[4];
    u32 saved_serialized_loaded_font_word;
  };

  union {
    u8 saved_serialized_temp_canvas[4];
    u32 saved_serialized_temp_canvas_word;
  };

  union {
    u8 saved_serialized_bmp25_ptr[4];
    u32 saved_serialized_bmp25_ptr_word;
  };

  union {
    u8 saved_serialized_bmp24_ptr[4];
    u32 saved_serialized_bmp24_ptr_word;
  };

  union {
    u8 saved_serialized_bmp_character_face[4];
    u32 saved_serialized_bmp_character_face_word;
  };

  union {
    u8 saved_serialized_bmp22_ptr[4];
    u32 saved_serialized_bmp22_ptr_word;
  };

  union {
    u8 saved_serialized_bmp21_ptr[4];
    u32 saved_serialized_bmp21_ptr_word;
  };

  union {
    u8 saved_serialized_bmp20_ptr[4];
    u32 saved_serialized_bmp20_ptr_word;
  };

  union {
    u8 saved_serialized_bmp26_ptr[4];
    u32 saved_serialized_bmp26_ptr_word;
  };

  union {
    u8 saved_serialized_text_edit_bitmap[4];
    u32 saved_serialized_text_edit_bitmap_word;
  };

  union {
    u8 saved_serialized_temp_bitmap_84x6[4];
    u32 saved_serialized_temp_bitmap_84x6_word;
  };

  Rect saved_restaurant_mask_rect;

  union {
    u8 saved_serialized_rect_1[4];
    u32 saved_serialized_rect_1_word;
  };

  PTSavedBobHost saved_serialized_bob_restaurant;

  PTSavedBobHost saved_serialized_bob_ingredients;

  PTSavedBobHost saved_serialized_bob_kitchen;

  PTSavedBobHost saved_serialized_bob_restaurant_open;

  PTSavedBobHost saved_serialized_bob_stanbobs;

  PTSavedBobHost saved_serialized_bob_typ;

  PTSavedBobHost saved_serialized_bob_walk;

  PTSavedBobHost saved_serialized_bob7;

  PTSavedBobHost saved_serialized_bob11;

  PTSavedBobHost saved_serialized_menu_bob;

  PTSavedBobHost saved_serialized_bob_restaurant_stats;

  PTSavedBobHost saved_serialized_bob9;

  PTSavedLangFileDesc saved_serialized_langfile_artikel3;

  PTSavedLangFileDesc saved_serialized_langfile_credits;

  PTSavedLangFileDesc saved_serialized_langfile_ende;

  PTSavedLangFileDesc saved_serialized_langfile_gtalk0;

  PTSavedLangFileDesc saved_serialized_langfile_gtalk1;

  PTSavedLangFileDesc saved_serialized_langfile_markt;

  PTSavedLangFileDesc saved_serialized_langfile_action;

  PTSavedLangFileDesc saved_serialized_langfile_werbung;

  PTSavedLangFileDesc saved_serialized_langfile_levent;

  PTSavedLangFileDesc saved_serialized_langfile_mafia;

  PTSavedLangFileDesc saved_serialized_langfile_waffen;

  PTSavedLangFileDesc saved_serialized_langfile_realtors;

  PTSavedLangFileDesc saved_serialized_langfile_bank;

  PTSavedLangFileDesc saved_serialized_langfile_insurance;

  PTSavedLangFileDesc saved_serialized_langfile_townhall;

  PTSavedLangFileDesc saved_serialized_langfile_police;

  PTSavedLangFileDesc saved_serialized_langfile_lief;

  PTSavedLangFileDesc saved_serialized_langfile_leben;

  PTSavedLangFileDesc saved_serialized_langfile_pers;

  PTSavedLangFileDesc saved_serialized_langfile_persf;

  PTSavedLangFileDesc saved_serialized_langfile_staff_applications[5];

  PTSavedLangFileDesc saved_serialized_langfile_double;

  union {
    u8 saved_serialized_mouse_over_zone[4];
    u32 saved_serialized_mouse_over_zone_word;
  };

  union {
    u8 saved_serialized_last_mouse_over_district[4];
    u32 saved_serialized_last_mouse_over_district_word;
  };

  u8 saved_town_names[10][12];

  Prefs saved_prefs;

  u8 saved_staff_ui_file_animation[20][2];

  u8 saved_state_slot_007;

  u8 saved_disable_draw_door;

  u8 saved_terminal_manual_mode;

  u8 saved_state_slot_008;

  u8 saved_palette_fade_algo;

  u8 saved_special_debt_flag;

  u8 saved_disable_init_country_progress_msg;

  u8 saved_main_menu_canvas_flag;

  u8 saved_state_slot_009;

  u8 saved_game_exit_requested;

  u8 saved_has_many_players;

  union {
    u8 saved_serialized_unit_for_number_query[4];
    u32 saved_serialized_unit_for_number_query_word;
  };

  JatobaInitializer saved_jatoba_initializer;

  PTSavedMouseInitializer saved_serialized_mouse_initializer;

  u8 saved_paragraph_highlight_override;

  u8 saved_season_map_colors[4][12];

  u16 saved_map_animation_palette_index;

  u8 saved_g_button_config[34][9];

  u8 saved_button_icon_ids[34][9];

  union {
    u8 saved_serialized_button_texts[34][9][4];
    u32 saved_serialized_button_texts_word[34][9];
  };

  u16 saved_button_x[9][2];

  u16 saved_button_y[9][2];

  union {
    u8 saved_serialized_player_ptr[4];
    u32 saved_serialized_player_ptr_word;
  };

  union {
    u8 saved_serialized_character_ptr[4];
    u32 saved_serialized_character_ptr_word;
  };

  u8 saved_invoice_month_names[12][10];

  union {
    u8 saved_serialized_cur_town[4];
    u32 saved_serialized_cur_town_word;
  };

  u8 saved_pizza_menu_info_separator[16];

  u8 saved_customer_eating_times[6];

  u8 saved_empty_restaurant_floor[32];

  IngredientSet saved_ingredients[4];

  union {
    u8 saved_serialized_age_taste_preferences[72];
    PTAgeTastePreference saved_age_taste_preferences[6];
  };

  u8 saved_state_slot_010;

  IntroTextGroup saved_intro_text_groups[22];

  u8 saved_staff_ui_manager_reactions[3][14];

  u8 saved_staff_ui_manager_behaviors[7][12];

  StaffTrainingCourse saved_staff_training_courses[3][6];

  u8 saved_staff_course_role_labels[3][12];

  u8 saved_goal_ui_colors[4];

  u32 saved_curriculum_scores[139];

  u32 saved_loaded_awards_bitmap;

  u32 saved_character_curriculum_flag;

  i32 saved_curriculum_page;

  u8 saved_character_mode_names[2][20];

  u16 saved_index_to_rank_value[14];

  u8 saved_bitmap_to_show_for_goal[101];

  union {
    u8 saved_goal_bitmap_flags[12][3];
    PTGoalSceneStyle saved_goal_scene_styles[12];
  };

  u8 saved_contract_insolvency_text[85];

  u8 saved_depot_capacity_exhausted_text[30];

  union {
    u8 saved_serialized_legacy_checksum_data[4];
    u32 saved_serialized_legacy_checksum_data_word;
  };

  u8 saved_empty_translation[1];

  u8 saved_translation_error_text[11];

  u8 saved_mouse_bitmap_data_1[121];

  u8 saved_mouse_bitmap_data_2[121];

  u8 saved_manager_role_names[2][8];

  i16 saved_country_selector_x[10];

  i16 saved_country_selector_y[10];

  u8 saved_attack_walk_images[4];

  i8 saved_guest_seat_x[4];

  i8 saved_guest_seat_y[4];

  i8 saved_guest_body_x[4];

  i8 saved_guest_body_y[4];

  i8 saved_guest_pose_x[16];

  i8 saved_guest_pose_y[16];

  u8 saved_legacy_walk_animation_phase;

  i8 saved_attack_collision_x[8];

  i8 saved_attack_collision_y[8];

  i8 saved_attack_head0_x[4];

  i8 saved_attack_head0_y[4];

  i8 saved_attack_head1_x[4];

  i8 saved_attack_head1_y[4];

  i8 saved_guest_carry_x[12];

  i8 saved_guest_carry_y[12];

  i8 saved_attack_held_weapon_x[32];

  i8 saved_attack_held_weapon_y[32];

  i8 saved_attack_knife_x[40];

  i8 saved_attack_spray_x[40];

  i8 saved_attack_knife_y[40];

  i8 saved_attack_spray_y[40];

  i8 saved_attack_projectile_x[4];

  i8 saved_attack_projectile_y[4];

  i8 saved_attack_explosion_x[10];

  i8 saved_attack_explosion_y[10];

  u8 saved_g_num_active_pigs;

  u8 saved_rest_path_depth;

  u8 saved_guest_idle_script[24];

  i8 saved_attack_weapon_reach[10];

  i8 saved_attack_step_x[4];

  i8 saved_attack_step_y[4];

  u8 saved_pizza_competition_delay;

  u8 saved_pizza_level;

  u8 saved_default_recipe_names[14][20];

  MainMenuDesc saved_main_menu_desc[9];

  u8 saved_main_menu_strings[67][30];

  u8 saved_selector_family_titles[3][30];

  u32 saved_state_slot_023;

  union {
    u8 saved_serialized_last_customer_count_poi[4];
    u32 saved_serialized_last_customer_count_poi_word;
  };

  u8 saved_curr_map_mode;

  u8 saved_selected_poi_attr;

  u8 saved_selected_poi_player;

  u8 saved_map_marker_toggle;

  u32 saved_map_tabs_order[3];

  u8 saved_owner_to_color[18];

  u16 saved_map_tab_step;

  u16 saved_map_tab_base;

  i16 saved_tabs_y_values[3];

  u8 saved_map_tab_labels[3][10];

  u16 saved_district_map_pattern[8];

  u8 saved_terrain_tile_colors[256];

  u8 saved_season_names[35];

  u8 saved_month_to_arstid[13];

  u8 saved_terrain_names[16][20];

  u8 saved_map_marker_visible;

  u16 saved_map_animation_palette_ticks;

  u8 saved_animated_production_x[4];

  u8 saved_animated_production_y[4];

  u8 saved_palette_in_callbackground;

  u8 saved_last_loaded_bob_typ;

  u8 saved_phone_face_five_x[25];

  u8 saved_phone_face_five_y[25];

  u8 saved_phone_face_y[25];

  u8 saved_phone_background_face_x[16];

  u8 saved_phone_background_face_y[16];

  u8 saved_terminal_category_names[5][26];

  u8 saved_terminal_dial_messages[4][36];

  u8 saved_terminal_waiting_message[26];

  u8 saved_dial_number_position;

  u8 saved_insurance_claim_reason_by_type[5];

  u8 saved_townhall_competition_messages[6];

  u8 saved_mission_item_pronouns[32];

  u8 saved_mission_pronouns[3][4];

  u8 saved_mission_completion_goals[6];

  u8 saved_mission_date_buffer_index;

  u8 saved_mission_place_buffer_index;

  u8 saved_arms_dealer_phone_nr[10][4][10];

  u8 saved_weapon_store_quantities[6];

  u32 saved_last_input_tick;

  u32 saved_last_second_tick;

  union {
    u8 saved_serialized_secondary_filename[4];
    u32 saved_serialized_secondary_filename_word;
  };

  u8 saved_info_location_names[10][20];

  union {
    u8 saved_serialized_current_button_hint[4];
    u32 saved_serialized_current_button_hint_word;
  };

  u8 saved_input_timer_phase;

  u8 saved_market_zone_names[7][28];

  u8 saved_market_bar_colors[10];

  i8 saved_guest_food_offset_x[4];

  i8 saved_guest_food_offset_y[4];

  i8 saved_guest_lower_offset_x[4];

  i8 saved_guest_lower_offset_y[4];

  i8 saved_guest_upper_offset_x[4];

  i8 saved_guest_upper_offset_y[4];

  u8 saved_placement_contact_count;

  u8 saved_furniture_refresh_depth;

  u8 saved_furniture_category_names[7][14];

  u8 saved_restaurant_media_labels[2][8][24];

  u8 saved_competition_sound_pending;

  u8 saved_sound_config_flag;

  u8 saved_sound_var_3;

  union {
    u8 saved_serialized_sound_buffer[4];
    u32 saved_serialized_sound_buffer_word;
  };

  u8 saved_current_music_index;

  u8 saved_state_slot_011;

  u8 saved_competition_sound_id;

  u8 saved_selected_pizza_load_idx;

  u8 saved_finance_stats_categories[3][30];

  u8 saved_daily_balance_flag;

  u8 saved_monthly_receipt_labels[4][16];

  u8 saved_monthly_expense_labels[9][20];

  u8 saved_insurance_category_labels[5][20];

  u8 saved_advert_tv_frames[9][22];

  u8 saved_newspaper_article_backgrounds[100];

  u8 saved_had_holiday;

  u8 saved_next_event_to_do;

  u32 saved_suppliers_daily_loop_index;

  u8 saved_month_names[12][5];

  u8 saved_ordinal_suffixes[4][3];

  u32 saved_pt_random_state;

  union {
    u8 saved_serialized_guest_heap_regions[4];
    u32 saved_serialized_guest_heap_regions_word;
  };

  union {
    u8 saved_serialized_guest_heap_rover[4];
    u32 saved_serialized_guest_heap_rover_word;
  };

  u32 saved_guest_heap_largest_before_rover;

  union {
    u8 saved_serialized_sound_pcm_cursor[4];
    u32 saved_serialized_sound_pcm_cursor_word;
  };

  u32 saved_sound_pcm_remaining;

  u32 saved_sound_digital_initialized;

  u32 saved_guest_heap_break;

  u32 saved_guest_psp_selector;

  u32 saved_guest_stack_lower_bound;

  u32 saved_guest_console_cached_character;

  u8 saved_state_slot_012;

  u8 saved_state_slot_013;

  u8 saved_guest_dos_major;

  PTSavedFileStreamRecord saved_serialized_guest_file_streams[20];

  u32 saved_guest_default_open_mode;

  u8 saved_guest_dos_error_errno[20];

  u32 saved_sound_dos_block_count;

  u32 saved_sound_pcm_driver_loaded;

  u32 saved_sound_pcm_driver_ready;

  union {
    u8 saved_serialized_sound_pcm_driver_image[4];
    u32 saved_serialized_sound_pcm_driver_image_word;
  };

  u32 saved_sound_pcm_driver_external;

  union {
    u8 saved_serialized_sound_vector_image[4];
    u32 saved_serialized_sound_vector_image_word;
  };

  u8 saved_sound_timer_previous_vector[6];

  u32 saved_sound_timer_active;

  u16 saved_sound_timer_previous_period;

  u16 saved_sound_timer_period;

  union {
    u8 saved_serialized_sound_timer_callback[4];
    u32 saved_serialized_sound_timer_callback_word;
  };

  u32 saved_guest_permission_mask;

  u32 saved_guest_io_mode_capacity;

  u32 saved_guest_io_modes[20];

  union {
    u8 saved_serialized_guest_io_mode_table[4];
    u32 saved_serialized_guest_io_mode_table_word;
  };

  GuestTM saved_timezone_start_rule;

  GuestTM saved_timezone_end_rule;

  i32 saved_timezone_seconds;

  i32 saved_timezone_dst_seconds;

  i32 saved_timezone_has_dst;

  u8 saved_timezone_standard_name[31];

  u8 saved_timezone_dst_name[31];

  u32 saved_guest_heap_grow_enabled;

  u32 saved_guest_heap_growth_minimum;

  union {
    u8 saved_serialized_guest_environment[4];
    u32 saved_serialized_guest_environment_word;
  };

  u8 saved_market_property_title[14];

  u8 saved_graphics_palette_filename[14];

  u8 saved_intro_presents[9];

  u8 saved_intro_logo_filename[17];

  u8 saved_intro_cyber_filename[14];

  u8 saved_intro_version[5];

  u8 saved_intro_title1_filename[15];

  u8 saved_intro_title2_filename[15];

  u8 saved_startup_permap_filename[15];

  u8 saved_startup_icons_filename[14];

  u8 saved_startup_k0_filename[11];

  u8 saved_startup_med1_filename[13];

  u8 saved_main_sound_config_filename[10];

  u8 saved_main_prefs_filename[6];

  u8 saved_quickstart_restaurant_file_format[9];

  u8 saved_town_filename_format[4];

  u8 saved_city_filename_format[14];

  u8 saved_recipe_allocation_failure_text[21];

  u8 saved_standard_recipe_file_format[9];

  u8 saved_map_filename_format[14];

  u8 saved_bez_filename_format[14];

  u8 saved_restaurant_not_found_text[20];

  u8 saved_savegame_filename_format[7];

  u8 saved_savegame_load_filename_format[3];

  u8 saved_staff_disaster_theft_text[65];

  u8 saved_staff_disaster_car_format[44];

  double saved_staff_disaster_raise_factor;

  u8 saved_staff_ui_years_format[9];

  u8 saved_staff_ui_payment[8];

  u8 saved_staff_ui_pizzas_day[11];

  u8 saved_staff_ui_cooking[8];

  u8 saved_staff_ui_serving[8];

  u8 saved_staff_ui_management[11];

  u8 saved_staff_ui_experience[11];

  u8 saved_staff_ui_intelligence[13];

  u8 saved_staff_ui_trustworthiness[16];

  u8 saved_staff_ui_health[7];

  u8 saved_staff_ui_motivation[11];

  u8 saved_staff_application_title[17];

  u8 saved_staff_course_title[8];

  u8 saved_staff_course_name_title[12];

  u8 saved_staff_course_scores_title[7];

  u8 saved_staff_course_costs_title[6];

  u8 saved_staff_course_days_title[5];

  u8 saved_staff_course_confirm_prompt[20];

  u8 saved_staff_ui_no_manager[11];

  u8 saved_staff_ui_authority[10];

  u8 saved_staff_ui_reactions[10];

  u8 saved_staff_ui_behavior_towards_staff[23];

  u8 saved_staff_ui_shopping_if_needed[19];

  u8 saved_staff_ui_change_prices[14];

  u8 saved_staff_ui_overdraw_account[17];

  u8 saved_staff_ui_turnover_decrease[18];

  u8 saved_staff_ui_turnover_increase[18];

  u8 saved_staff_ui_supply_shortage[16];

  u8 saved_staff_ui_staff_shortage[15];

  u8 saved_staff_pool_full_error[14];

  u8 saved_staff_hire_none_error[27];

  u8 saved_staff_hire_full_format[33];

  u8 saved_staff_hire_refusal_error[43];

  u8 saved_staff_hire_prompt_format[52];

  u8 saved_staff_expenses_label[15];

  double saved_staff_expenses_underpaid_factor;

  double saved_staff_expenses_overpaid_factor;

  double saved_staff_daily_underpaid_factor;

  double saved_staff_daily_overpaid_factor;

  u8 saved_staff_ui_name[5];

  u8 saved_staff_ui_days_absent[12];

  u8 saved_staff_ui_capacity[4];

  u8 saved_staff_ui_salary[4];

  u8 saved_staff_ui_name_format[6];

  u8 saved_staff_ui_course[7];

  u8 saved_staff_ui_sick[5];

  u8 saved_staff_ui_pregnant[9];

  u8 saved_staff_ui_chefs[6];

  u8 saved_staff_ui_waiters[8];

  u8 saved_staff_ui_number[8];

  u8 saved_staff_ui_total_capacity[16];

  u8 saved_staff_ui_average_quality[17];

  u8 saved_staff_ui_time_per_pizza[16];

  u8 saved_staff_ui_work[6];

  u8 saved_staff_ui_total_costs[12];

  u8 saved_staff_fire_selection_error[53];

  u8 saved_staff_fire_away_format[45];

  u8 saved_staff_fire_prompt[37];

  u8 saved_staff_wages_selection_error[63];

  u8 saved_staff_wage_unit[2];

  u8 saved_staff_wage_prompt[15];

  u8 saved_staff_course_selection_error[65];

  u8 saved_staff_course_already_format[37];

  u8 saved_staff_course_away_format[19];

  u8 saved_goal_background_filename_format[13];

  u8 saved_mission_completion_format[85];

  u8 saved_character_filename_format[11];

  u8 saved_goal_name_1000_pizzas[33];

  u8 saved_goal_name_10000_pizzas[30];

  u8 saved_goal_name_profit_25000[39];

  u8 saved_goal_name_profit_50000[39];

  u8 saved_goal_name_world[18];

  u8 saved_curriculum_date_format[9];

  u8 saved_character_initial_labels[37];

  u8 saved_target_curriculum_heading[24];

  u8 saved_curriculum_mission_only[21];

  u8 saved_curriculum_heading[17];

  u8 saved_character_full_name_format[6];

  u8 saved_character_value_labels[128];

  u8 saved_character_company_heading[17];

  u8 saved_character_company_owner_format[13];

  u8 saved_character_company_profit[14];

  u8 saved_character_awards_filename[14];

  u8 saved_character_awards_heading[7];

  u8 saved_character_holiday_question[37];

  u8 saved_character_holiday_days[5];

  u8 saved_character_holiday_prompt[14];

  u8 saved_personal_character_image_format[10];

  u8 saved_personal_character_filename_format[7];

  u8 saved_personal_mission_filename_format[7];

  u8 saved_depot_weapon_category_label[9];

  u8 saved_stock_menu_depot_title[6];

  u8 saved_depot_weapon_bag_label[4];

  u8 saved_depot_town_title_format[17];

  u8 saved_depot_columns_label[23];

  u8 saved_depot_sum_value_label[10];

  u8 saved_depot_export_title_format[14];

  u8 saved_depot_sale_label[5];

  u8 saved_depot_purchase_label[7];

  u8 saved_depot_purchase_title_format[14];

  u8 saved_shop_price_label[6];

  u8 saved_depot_supplier_column_label[6];

  u8 saved_shop_amount_label[7];

  u8 saved_depot_store_column_label[6];

  u8 saved_contract_sum_label[5];

  u8 saved_depot_quality_rejection_text[82];

  double saved_depot_trade_price_divisor;

  float saved_depot_trade_rating_divisor;

  u8 saved_depot_priorities_title[20];

  u8 saved_contract_discount_label[10];

  u8 saved_contract_charge_label[8];

  u8 saved_contract_terms_penalty_label[9];

  u8 saved_contract_term_format[14];

  double saved_contract_price_divisor;

  u8 saved_contract_debug_portrait_prompt[5];

  u8 saved_contract_minimum_order_format[17];

  double saved_contract_purchase_divisor;

  u8 saved_contract_delivery_amounts_title[18];

  u8 saved_stock_menu_kg_label[3];

  u8 saved_contract_with_title[14];

  u8 saved_shop_total_label[7];

  u8 saved_contract_penalty_label[16];

  u8 saved_contract_amount_label[8];

  u8 saved_contract_as_required_label[12];

  u8 saved_contract_as_agreed_label[17];

  u8 saved_contract_quality_label[9];

  u8 saved_contract_remaining_term_label[16];

  u8 saved_contract_empty_error[14];

  u8 saved_contract_term_prompt[10];

  u8 saved_contract_cancel_prompt[101];

  u8 saved_contract_delivery_space_text[2];

  u8 saved_contract_cancel_format[31];

  u8 saved_shop_unavailable_price_text[7];

  u8 saved_shop_quality_format[12];

  u8 saved_shop_stock_label[6];

  u8 saved_stock_menu_days_label[5];

  double saved_shop_price_divisor;

  u8 saved_contract_expired_format[34];

  u8 saved_contract_shortage_format[28];

  u8 saved_contract_delivery_format[29];

  u8 saved_shop_suppliers_title_format[13];

  u8 saved_shop_supplier_total_format[12];

  u8 saved_shop_page_format[7];

  u8 saved_stock_menu_stocks_title[7];

  u8 saved_stock_menu_kilogram_label[9];

  u8 saved_stock_menu_value_label[6];

  u8 saved_stock_menu_supplies_format[12];

  u8 saved_stock_menu_quality_label[5];

  u8 saved_stock_menu_supply_label[4];

  u8 saved_stock_menu_need_label[4];

  u8 saved_stock_menu_use_label[4];

  double saved_stock_menu_demand_divisor;

  double saved_supplier_price_addon_divisor;

  double saved_supplier_price_level5_factor;

  double saved_supplier_price_level4_factor;

  double saved_supplier_price_level2_factor;

  double saved_supplier_price_level1_factor;

  u8 saved_depot_central_label[8];

  u8 saved_depot_capacity_label[9];

  u8 saved_depot_in_use_label[7];

  u8 saved_depot_stolen_format[82];

  float saved_contract_penalty_new_factor;

  float saved_contract_discount_factor;

  float saved_contract_discount_divisor;

  float saved_contract_charge_divisor;

  float saved_contract_fake_sum_factor;

  float saved_contract_penalty_reduction_factor;

  u8 saved_language_filename_format[5];

  u8 saved_file_write_mode[2];

  u8 saved_file_readwrite_mode[4];

  double saved_double_720;

  float saved_customer_other_tile_factor;

  float saved_customer_residential_tile_factor;

  float saved_float_720;

  double saved_pizza_income_price_divisor;

  double saved_pizza_income_price_offset;

  double saved_pizza_income_base_divisor;

  double saved_pizza_income_base_offset;

  double saved_pizza_price_offset;

  double saved_pizza_price_divisor;

  double saved_pizza_price_bonus_scale;

  float saved_customer_distribution_factor;

  float saved_pizza_order_random_threshold;

  u8 saved_restaurant_supplies_shortage_text[50];

  u8 saved_restaurant_ovens_shortage_text[40];

  u8 saved_restaurant_cooks_shortage_text[40];

  u8 saved_restaurant_waiters_shortage_text[42];

  double saved_restaurant_supply_percentage_scale;

  float saved_restaurant_turnover_goal_threshold;

  double saved_restaurant_sales_currency_divisor;

  u8 saved_restaurant_electricity_label[13];

  u8 saved_restaurant_slot_income_label[22];

  u8 saved_restaurant_invoice_total_label[7];

  u8 saved_country_market_leader_label[14];

  u8 saved_country_no_market_leader_label[11];

  u8 saved_country_map_filename[16];

  u8 saved_country_no_city_message[18];

  u8 saved_country_insufficient_money_message[18];

  u8 saved_country_flight_cost_format[22];

  u8 saved_country_transport_cost_format[25];

  u8 saved_country_missing_depot_format[39];

  u8 saved_town_initializing_caption[13];

  u8 saved_town_initializing_ellipsis[4];

  u8 saved_staff_market_filename[11];

  u8 saved_attack_police_message[15];

  u8 saved_attack_police_argument[2];

  u8 saved_guest_walk_bob_format[15];

  u8 saved_guest_type_error_text[10];

  u8 saved_attack_weapon_bob_format[17];

  u8 saved_attack_damage_format[11];

  u8 saved_attack_contamination_format[19];

  u8 saved_guest_portrait_filename_format[12];

  u8 saved_guest_eating_comment_format[19];

  double saved_ai_restaurant_upgrade_money_factor;

  u8 saved_ai_restaurant_memory_error[40];

  double saved_ai_low_rating_money_threshold;

  double saved_ai_low_default_rating_money_threshold;

  double saved_legacy_scale_word_byte_divisor;

  double saved_legacy_scale_difference_byte_divisor;

  double saved_legacy_scale_word_word_divisor;

  double saved_legacy_scale_difference_word_divisor;

  u8 saved_competition_pizza_title[6];

  u8 saved_competition_title[12];

  u8 saved_competition_challenge_format[94];

  u8 saved_competition_ai_challenge_text[52];

  u8 saved_competition_dollars_unit[3];

  u8 saved_competition_bet_prompt[21];

  u8 saved_competition_decline_text[40];

  u8 saved_competition_invitation_format[58];

  u8 saved_competition_stake_prompt[23];

  u8 saved_competition_no_unit[1];

  u8 saved_competition_speed_prompt[7];

  u8 saved_competition_jury_caption[14];

  u8 saved_competition_time_format[3];

  u8 saved_competition_draw_caption[11];

  u8 saved_competition_win_caption[5];

  double saved_competition_minimum_money;

  double saved_competition_maximum_bet;

  double saved_competition_ai_money_fraction;

  u8 saved_recipe_new_name[6];

  u8 saved_recipe_number_format[16];

  u8 saved_recipe_calories_format[16];

  u8 saved_recipe_fat_format[16];

  u8 saved_recipe_sweet_format[16];

  u8 saved_recipe_salty_format[16];

  u8 saved_recipe_hot_format[16];

  u8 saved_recipe_appearance_format[15];

  u8 saved_recipe_refinement_format[15];

  u8 saved_recipe_trend_format[15];

  u8 saved_recipe_imitation_format[17];

  u8 saved_recipe_normality_format[17];

  u8 saved_recipe_jury_filename[13];

  u8 saved_recipe_jury_compare_text[34];

  u8 saved_recipe_jury_evaluate_text[26];

  u8 saved_recipe_jury_competition_text[20];

  u8 saved_recipe_jury_total_text[6];

  u8 saved_recipe_jury_percent_format[5];

  u8 saved_recipe_jury_book_hint_format[131];

  u8 saved_recipe_view_info_text[6];

  u8 saved_recipe_view_recipe_text[7];

  u8 saved_recipe_view_here_text[5];

  u8 saved_recipe_view_all_text[4];

  u8 saved_recipe_view_costs_text[6];

  u8 saved_recipe_view_price_text[6];

  u8 saved_recipe_view_sales_text[6];

  u8 saved_recipe_view_profit_text[7];

  u8 saved_recipe_view_popularity_text[11];

  u8 saved_recipe_view_gram_text[2];

  u8 saved_recipe_view_total_weight_text[15];

  u8 saved_recipe_view_ingredient_count_format[20];

  u8 saved_pizza_competition_title[18];

  u8 saved_pizza_competition_separator[17];

  u8 saved_pizza_competition_original_label[17];

  u8 saved_pizza_competition_level_label[7];

  u8 saved_pizza_competition_ingredient_label[20];

  u8 saved_pizza_competition_ready_text[13];

  u8 saved_ingredient_grammes_format[11];

  u8 saved_ingredient_calories_format[12];

  u8 saved_ingredient_intensity_format[14];

  u8 saved_ingredient_fat_format[10];

  u8 saved_ingredient_sweet_format[10];

  u8 saved_ingredient_salty_format[10];

  u8 saved_ingredient_hot_format[10];

  u8 saved_ingredient_trend_format[10];

  u8 saved_ingredient_trend_name_format[5];

  u8 saved_ingredients_full_text[38];

  u8 saved_recipe_editor_greeting_format[38];

  u8 saved_recipe_name_prompt[16];

  float saved_recipe_cost_divisor;

  u8 saved_disk_menu_caption[10];

  u8 saved_disk_quit_prompt[14];

  u8 saved_character_selection_level_caption[13];

  u8 saved_highscores_filename[15];

  u8 saved_options_on_label[3];

  u8 saved_options_off_label[4];

  u8 saved_options_prefs_filename[6];

  double saved_character_selection_money_limit;

  u8 saved_mission_date_format[9];

  u8 saved_mission_selection_caption[15];

  u8 saved_mission_filename_format[9];

  u8 saved_main_menu_background_filename[13];

  u8 saved_game_screen_background_filename[11];

  u8 saved_selector_save_label[5];

  u8 saved_selector_load_label[5];

  u8 saved_selector_title_format[6];

  u8 saved_selector_game_directory[5];

  u8 saved_selector_no_games[16];

  u8 saved_selector_no_characters[26];

  u8 saved_selector_no_missions[24];

  u8 saved_selector_new_name[13];

  u8 saved_selector_filename_prefix[6];

  u8 saved_selector_error_name[6];

  u8 saved_selector_overwrite_prompt[20];

  u8 saved_player_stats_money_label[6];

  u8 saved_player_stats_age_label[4];

  u8 saved_player_stats_popularity_label[11];

  u8 saved_player_stats_health_label[7];

  u8 saved_player_stats_energy_label[7];

  u8 saved_player_stats_intelligence_label[13];

  u8 saved_player_stats_charisma_label[9];

  u8 saved_player_stats_dexterity_label[10];

  u8 saved_player_stats_personality_label[12];

  u8 saved_player_stats_cooking_label[8];

  u8 saved_player_stats_courage_label[8];

  u8 saved_attack_action_caption[14];

  u8 saved_attack_weapon_caption[14];

  u8 saved_mission_invalid_place[8];

  u8 saved_poi_info_formats[93];

  u8 saved_poi_realtor_text[9];

  u8 saved_closed_restaurant_text[36];

  u8 saved_creating_map_text[16];

  u8 saved_maestro_background_filename[16];

  u8 saved_game_over_retry_prompt[11];

  u8 saved_high_score_award_text[118];

  u8 saved_high_score_name_format[10];

  u8 saved_high_score_name_prompt[6];

  u8 saved_personal_record_directory[6];

  u8 saved_personal_record_fallback[6];

  u8 saved_personal_record_save_prompt[47];

  u8 saved_game_over_low_score_text[90];

  u8 saved_ending_background_filename[13];

  u8 saved_animated_quality_caption[18];

  u8 saved_animated_reputation_caption[22];

  u8 saved_animated_no_text[3];

  u8 saved_animated_orders_text[7];

  u8 saved_animated_output_caption[7];

  float saved_animated_day_minutes;

  double saved_animated_percent_scale;

  u8 saved_animated_demand_text[7];

  u8 saved_animated_capacity_text[9];

  u8 saved_animated_popularity_caption[11];

  u8 saved_animated_customers_caption[15];

  u8 saved_animated_takings_text[8];

  u8 saved_animated_expenses_text[9];

  double saved_realtor_sell_factor;

  double saved_realtor_maximum_factor;

  double saved_realtor_discount_factor;

  double saved_realtor_increase_factor;

  u8 saved_bank_transfer_amount_text[7];

  u8 saved_bank_transfer_account_text[13];

  double saved_bank_loan_cash_limit;

  double saved_bank_investment_minimum;

  double saved_bank_investment_cash_limit;

  double saved_bank_investment_fraction;

  u8 saved_townhall_pair_format[6];

  u8 saved_townhall_restaurant_name_format[4];

  u8 saved_townhall_annual_pizza_announcement[93];

  u8 saved_townhall_annual_recipe_name_format[20];

  u8 saved_townhall_character_name_format[7];

  u8 saved_townhall_name_format[3];

  u8 saved_townhall_congratulations_format[23];

  u8 saved_townhall_competition_account_format[6];

  u8 saved_townhall_mayor_date_format[12];

  u8 saved_townhall_mayor_account_format[6];

  u8 saved_terminal_new_number_prompt[13];

  u8 saved_terminal_no_entries[19];

  u8 saved_terminal_title[31];

  u8 saved_terminal_loading_message[30];

  u8 saved_empty_string[1];

  u8 saved_mission_completion_phone_text[3];

  u8 saved_mission_date_phrase_format[22];

  u8 saved_mission_place_format[18];

  u8 saved_mission_pickup_format[37];

  u8 saved_mission_contamination_format[72];

  u8 saved_mission_damage_format[55];

  u8 saved_mission_laundering_format[117];

  u8 saved_mission_delivery_format[31];

  u8 saved_mission_deal_format[100];

  u8 saved_mafia_failed_business_text[124];

  u8 saved_mafia_protection_format[67];

  u8 saved_mission_reward_format[39];

  u8 saved_mission_accept_format[47];

  u8 saved_mafia_existing_loan_format[9];

  u8 saved_mafia_loan_offer_format[40];

  u8 saved_mafia_tip_town_format[7];

  u8 saved_mafia_weapon_tip_format[44];

  u8 saved_mafia_dealer_tip_format[9];

  u8 saved_mafia_tip_price_format[19];

  double saved_mafia_cost_cash_scale;

  double saved_mafia_cost_scale_limit;

  u8 saved_weapon_store_piece_format[20];

  u8 saved_weapon_store_sell_format[17];

  u8 saved_weapon_store_buy_format[12];

  u8 saved_invoice_title[8];

  u8 saved_invoice_name_format[7];
  u8 saved_land_symbol_filename[16];

  u8 saved_background_filename_format[15];

  u8 saved_no_memory_continue_prompt[34];

  u8 saved_file_missing_retry_format[32];

  u8 saved_file_open_error_format[23];

  u8 saved_file_read_error_format[23];

  u8 saved_file_write_error_format[23];

  u8 saved_price_decimal_format[7];

  double saved_price_display_divisor;

  u8 saved_font_error_text[13];

  u8 saved_list_percent_text[2];

  u8 saved_dollar_alignment_text[2];

  u8 saved_dollar_text_format[4];

  u8 saved_integer_text_format[3];

  u8 saved_advert_dollar[2];

  double saved_legacy_round_half;

  u8 saved_info_box_formats[272];

  u8 saved_restaurant_name_format[9];

  u8 saved_money_display_format[5];

  u8 saved_ok_text[3];

  u8 saved_yes_text[4];

  u8 saved_no_text[3];

  u8 saved_staff_disaster_question_format[7];

  u8 saved_number_query_format[5];

  u8 saved_bob_second_already_linked_error[42];

  u8 saved_bob_shared_capacity_error[57];

  u8 saved_market_low[4];

  u8 saved_market_very_little[12];

  u8 saved_market_little[7];

  u8 saved_market_average[8];

  u8 saved_market_above_average[14];

  u8 saved_market_high[5];

  u8 saved_market_very_high[10];

  u8 saved_market_extremely_high[15];

  u8 saved_market_out_of_range[13];

  double saved_decimal_scale_base;

  float saved_market_percent_scale;

  u8 saved_market_premises_label[23];

  u8 saved_market_rent_label[12];

  u8 saved_market_per_square_metre[5];

  u8 saved_market_price_label[13];

  u8 saved_market_tendency_label[10];

  u8 saved_market_falling_label[8];

  u8 saved_market_rising_label[7];

  u8 saved_market_average_size_label[14];

  u8 saved_market_square_metres[4];

  u8 saved_market_rooms_label[7];

  u8 saved_market_to_rent_label[9];

  u8 saved_market_for_sale_label[10];

  u8 saved_market_not_available_label[15];

  u8 saved_market_taste_title[6];

  u8 saved_market_rank_dash[2];

  u8 saved_market_chain_title[21];

  u8 saved_market_share_format[20];

  u8 saved_market_turnover_format[16];

  u8 saved_floor_zero_filename[11];

  u8 saved_floor_filename_format[12];

  u8 saved_furniture_inventory_num_text[4];

  u8 saved_furniture_inventory_name_text[5];

  u8 saved_furniture_inventory_format[6];

  u8 saved_furniture_equipment_title[10];

  u8 saved_furniture_in_use_text[8];

  u8 saved_furniture_value_text[7];

  u8 saved_furniture_quality_text[9];

  u8 saved_poi_capacity_text[10];

  u8 saved_furniture_seats_text[6];

  u8 saved_furniture_per_day_text[8];

  u8 saved_furniture_kitchenware_title[12];

  u8 saved_furniture_ovens_text[7];

  u8 saved_furniture_bake_time_text[17];

  u8 saved_furniture_minutes_text[8];

  u8 saved_furniture_baking_capacity_text[17];

  u8 saved_furniture_number_text[7];

  u8 saved_furniture_floor_title[15];

  u8 saved_furniture_tile_count_format[30];

  u8 saved_furniture_area_format[13];

  u8 saved_furniture_extras_title[7];

  u8 saved_refresh_overflow_format[43];

  u8 saved_furniture_floor_patterns_filename[14];

  u8 saved_furniture_floor_price_format[20];

  u8 saved_furniture_floor_total_format[11];

  u8 saved_furniture_supplier_page_format[7];

  u8 saved_furniture_price_text[7];

  u8 saved_furniture_capacity_format[19];

  u8 saved_furniture_baking_time_format[21];

  u8 saved_furniture_quality_format[12];

  u8 saved_furniture_selection_limit_text[26];

  u8 saved_furniture_invoice_caption[10];

  u8 saved_furniture_purchase_thanks[25];

  u8 saved_furniture_no_selection_text[67];

  u8 saved_placement_maximum_ovens_text[25];

  u8 saved_placement_maximum_items_text[25];

  u8 saved_placement_closed_text[120];

  u8 saved_placement_sell_format[39];

  u8 saved_placement_invalid_text[28];

  u8 saved_furniture_no_supplier_text[28];

  u8 saved_furniture_suppliers_caption[10];

  u8 saved_customer_ratings_menu_title[5];

  u8 saved_customer_ratings_furniture_title[10];

  u8 saved_customer_ratings_title[13];

  u8 saved_furniture_menu_caption[11];

  u8 saved_furniture_equipment_caption[10];

  u8 saved_furniture_van_warning[113];

  u8 saved_furniture_info_formats[150];

  u8 saved_restaurant_close_confirm_text[18];

  u8 saved_restaurant_need_ovens_text[57];

  u8 saved_restaurant_need_chefs_text[65];

  u8 saved_restaurant_need_waiters_text[59];

  u8 saved_restaurant_need_recipes_text[75];

  u8 saved_restaurant_closed_today_text[40];

  u8 saved_restaurant_open_confirm_text[17];

  u8 saved_restaurant_rearrange_text[64];

  u8 saved_restaurant_nothing_text[8];

  u8 saved_restaurant_media_prompt[16];

  u8 saved_intro_music_filename[9];

  u8 saved_music_filename_format[10];

  double saved_recipe_price_limit;

  double saved_recipe_price_scale;

  u8 saved_recipe_list_title[12];

  u8 saved_recipe_list_page_format[8];

  u8 saved_recipe_list_new[6];

  u8 saved_recipe_file_format[12];

  u8 saved_recipe_number_unit[2];

  u8 saved_recipe_load_prompt[19];

  u8 saved_recipe_load_custom_error[34];

  u8 saved_recipe_duplicate_error[37];

  u8 saved_recipe_checksum_error[27];

  u8 saved_recipe_list_save_custom_error[34];

  u8 saved_recipe_save_prompt[18];

  u8 saved_recipe_save_tag[5];

  u8 saved_recipe_list_recreate_prompt[17];

  u8 saved_recipe_list_create_first_error[37];

  u8 saved_recipe_list_already_menu_error[36];

  u8 saved_recipe_price_prompt[13];

  u8 saved_recipe_list_menu_full_error[48];

  u8 saved_pizza_menu_page_format[5];

  u8 saved_pizza_menu_sales_title[14];

  u8 saved_pizza_menu_title[5];

  u8 saved_pizza_menu_sales_label[6];

  u8 saved_pizza_menu_turnover_label[10];

  u8 saved_pizza_menu_popularity_label[14];

  u8 saved_pizza_menu_demand_label[8];

  u8 saved_pizza_menu_info_sales_label[7];

  u8 saved_pizza_menu_keep_one_error[45];

  u8 saved_pizza_menu_remove_selection_error[60];

  double saved_profit_graph_scale_base;

  double saved_legacy_graph_scale_base;

  u8 saved_cash_flow_caption[10];

  u8 saved_profit_graph_units_format[13];

  u8 saved_profit_graph_weeks[6];

  double saved_cash_graph_scale_base;

  u8 saved_daily_profit_caption[14];

  u8 saved_profit_graph_days[5];

  double saved_daily_profit_scale_base;

  u8 saved_balance_labels[118];

  u8 saved_weekly_profit_text[7];

  u8 saved_finance_stats_labels[110];

  u8 saved_monthly_overview_caption[17];

  u8 saved_monthly_result_caption[8];

  u8 saved_monthly_restaurants_caption[12];

  u8 saved_daily_settlement_text[17];

  u8 saved_daily_day_before_text[11];

  u8 saved_daily_yesterday_text[10];

  u8 saved_daily_today_text[6];

  u8 saved_daily_orders_text[7];

  u8 saved_daily_pizzas_sold_text[12];

  u8 saved_daily_delivery_capacity_text[18];

  u8 saved_daily_demand_value_text[13];

  u8 saved_weekly_sales_text[6];

  u8 saved_daily_ingredients_used_text[17];

  u8 saved_daily_expenditure_text[12];

  u8 saved_weekly_week_text[7];

  u8 saved_weekly_takings_text[8];

  u8 saved_weekly_items_text[6];

  u8 saved_weekly_costs_text[6];

  u8 saved_weekly_total_text[7];

  u8 saved_weekly_expenses_text[9];

  u8 saved_weekly_ingredients_text[15];

  u8 saved_weekly_cooks_text[7];

  u8 saved_weekly_staff_name_format[6];

  u8 saved_weekly_empty_staff_format[3];

  u8 saved_weekly_waiters_text[8];

  u8 saved_weekly_managers_text[10];

  u8 saved_restaurant_overview_day[4];

  u8 saved_restaurant_overview_week[5];

  u8 saved_restaurant_overview_caption[20];

  u8 saved_rest_sales_caption[12];

  u8 saved_rest_customers_caption[10];

  u8 saved_rest_history_date_format[6];

  u8 saved_rest_weekly_caption[16];

  u8 saved_rest_weekly_columns[33];

  u8 saved_advert_substitution_format[7];

  u8 saved_advert_output_format[17];

  u8 saved_advert_flyer_format[31];

  u8 saved_advert_output_label[8];

  u8 saved_advert_costs_label[7];

  u8 saved_advert_poster_format[17];

  u8 saved_advert_newspaper_cost_format[14];

  u8 saved_advert_tv_format[15];

  u8 saved_advert_tv_title[7];

  u8 saved_advert_tv_director[8];

  u8 saved_advert_days[5];

  u8 saved_advert_misc_filename[15];

  u8 saved_advert_already_active[46];

  u8 saved_end_turn_phone_text[2];

  double saved_market_share_price_divisor;

  double saved_market_share_base_threshold;

  u8 saved_insufficient_cash_text[30];

  double saved_ledger_limit;

  u8 saved_periodic_extortion_text[3];

  u8 saved_restaurant_opening_text[40];

  u8 saved_restaurant_closing_text[40];

  double saved_cash_goal_thresholds[6];

  u8 saved_new_restaurant_memory_error[42];

  u8 saved_delayed_message_format[21];

  double saved_supplier_price_scale;

  u8 saved_flooring_unlock_hint[281];

  u8 saved_supplier_debts_paid_format[37];

  u8 saved_supplier_debts_due_format[107];

  u8 saved_overdrawn_account_text[116];

  u8 saved_van_furniture_value_format[60];

  u8 saved_weekly_profit_caption[16];

  double saved_weekly_cash_history_limit;

  u8 saved_monthly_cost_caption[16];

  u8 saved_monthly_repayment_caption[11];

  u8 saved_monthly_interest_caption[10];

  u8 saved_monthly_insurance_caption[11];

  u8 saved_monthly_investment_interest_caption[15];

  u8 saved_monthly_rent_caption[6];

  u8 saved_monthly_total_caption[7];

  u8 saved_player_card_dot[2];

  u8 saved_end_turn_wait_text[15];

  u8 saved_sound_driver_signature[6];

  u8 saved_sound_config_read_mode[3];

  u8 saved_sound_config_filename[10];

  u8 saved_sound_pcm_driver_filename[12];
  u8 saved_sound_binary_read_mode[3];

  u8 saved_sound_vector_filename[11];

  u8 saved_timezone_env_name[3];

  u8 saved_watcom_character_classes[256];

  u64 saved_price_parser_positive_infinity;

  u64 saved_scale10_positive_infinity;

  u64 saved_scale10_negative_infinity;

  u64 saved_scale10_positive_max;

  u64 saved_scale10_negative_max;

  i16 saved_time_common_month_days[13];

  i16 saved_time_leap_month_days[13];

  Scale10Power saved_scale10_powers[8];

  float saved_shop_purchase_price[96];

  i16 saved_purchase_amount[96];
  union {
    u8 saved_loaded_map_file[120][160];
    u8 saved_map_tiles[120 * 160];
  };

  Staff saved_g_staff[200];

  union {
    u8 saved_serialized_mission_receipts[128];
    PTMissionReceiptLedger saved_mission_receipts[8];
  };

  GlobalBalance saved_global_balance[8];

  union {
    u8 saved_serialized_previous_month_expenses[320];
    PTMissionExpenseLedger saved_previous_month_expenses[8];
  };

  u8 saved_staff_is_type[256];

  union {
    u8 saved_serialized_previous_month_receipts[128];
    PTMissionReceiptLedger saved_previous_month_receipts[8];
  };

  union {
    u8 saved_serialized_player_loans[640];
    Loan saved_player_loans[8][4];
  };

  u8 saved_staff_hates_player[256];

  union {
    u8 saved_serialized_player_investments[256];
    Investment saved_player_investments[8][4];
  };

  union {
    u8 saved_serialized_mission_expenses[320];
    PTMissionExpenseLedger saved_mission_expenses[8];
  };

  SpriteRect saved_blitmap_offsets[30];
  union {
    struct {
      u8 saved_mission_year[8][100];
      u8 saved_mission_target_year[8][100];
    };
    u8 saved_mission_year_columns[16][100];
  };
  union {
    struct {
      u8 saved_mission_day[8][100];
      u8 saved_mission_target_day[8][100];
    };
    u8 saved_mission_day_columns[16][100];
  };
  union {
    struct {
      u8 saved_mission_month[8][100];
      u8 saved_mission_target_month[8][100];
    };
    u8 saved_mission_month_columns[16][100];
  };

  u8 saved_message_slots_free[16];
  union {
    struct {
      u8 saved_mission_goal[8][100];
      u8 saved_mission_target_goal[8][100];
    };
    u8 saved_mission_goal_columns[16][100];
  };

  u8 saved_weapon_base_price[40];

  u8 saved_delayed_msg_contents[16][256];

  Banana saved_bananas[16];
  union {
    struct {
      u8 saved_map_animation_palette_colors[4][3];
      u8 saved_map_animation_palette_repeat[4][3];
    };
    u8 saved_map_animation_colors[8][3];
  };

  u8 saved_array_size_8[8];

  u8 saved_extra_town_data[8][24][16];

  PTSavedIgloo saved_serialized_igloos[150];

  PTSavedIgloo saved_serialized_map_igloo_tail;

  PTSavedIgloo saved_serialized_map_igloo_head;

  i16 saved_animated_stats_reputation[16];

  i16 saved_animated_stats_quality[4];

  PTSavedIgloo saved_serialized_temp_igloo_1;

  PTSavedIgloo saved_serialized_temp_igloo_2;

  Guest saved_guests[40];

  union {
    u8 saved_serialized_largest_map_column_bitmaps[2][4];
    u32 saved_serialized_largest_map_column_bitmaps_word[2];
  };

  union {
    u8 saved_serialized_map_row_bitmaps[2][4];
    u32 saved_serialized_map_row_bitmaps_word[2];
  };

  union {
    u8 saved_serialized_largest_map_row_bitmaps[2][4];
    u32 saved_serialized_largest_map_row_bitmaps_word[2];
  };

  union {
    u8 saved_serialized_map_column_bitmaps[2][4];
    u32 saved_serialized_map_column_bitmaps_word[2];
  };

  PTSavedIgloo saved_serialized_temp_igloo;

  u8 saved_strbuf[256];

  u8 saved_palette_e[71][768];

  PTSavedPlayer saved_serialized_players[8];

  PTSavedPalettePointer saved_serialized_pal_pointer;

  Character saved_characters[8];

  union {
    u8 saved_serialized_display_bitmaps[2][4];
    u32 saved_serialized_display_bitmaps_word[2];
  };

  union {
    u8 saved_serialized_popup_menu_items[24][4];
    u32 saved_serialized_popup_menu_items_word[24];
  };

  u32 saved_value_of_crashed_furniture_1;

  u8 saved_rest_floormap[46][46];

  u32 saved_attack_damage;

  u8 saved_loaded_bez_file[15][20];

  float saved_current_purchase_amount;

  PTSavedTown saved_serialized_durian;

  u32 saved_active_input_class;

  PTSavedHazelnut saved_serialized_hazelnuts[150];

  union {
    u8 saved_serialized_cur_loading_filehandle[4];
    u32 saved_serialized_cur_loading_filehandle_word;
  };

  u32 saved_num_written_bytes;

  union {
    u8 saved_serialized_ingredient_background[4];
    u32 saved_serialized_ingredient_background_word;
  };

  u32 saved_num_read_bytes;

  union {
    u8 saved_serialized_cur_writing_filehandle[4];
    u32 saved_serialized_cur_writing_filehandle_word;
  };

  union {
    u8 saved_serialized_buffer_backup[4];
    u32 saved_serialized_buffer_backup_word;
  };

  union {
    u8 saved_serialized_last_recipe_ptr[4];
    u32 saved_serialized_last_recipe_ptr_word;
  };

  union {
    u8 saved_serialized_restaurant_with_guests[4];
    u32 saved_serialized_restaurant_with_guests_word;
  };

  union {
    u8 saved_serialized_restaurant_hit_test_owner[4];
    u32 saved_serialized_restaurant_hit_test_owner_word;
  };

  PTSavedGame saved_serialized_game;

  PTSavedJuice saved_serialized_some_juice;

  union {
    u8 saved_serialized_bmp16_ptr[4];
    u32 saved_serialized_bmp16_ptr_word;
  };

  union {
    u8 saved_serialized_last_input_event[4];
    u32 saved_serialized_last_input_event_word;
  };

  union {
    u8 saved_serialized_font_cstandar[4];
    u32 saved_serialized_font_cstandar_word;
  };

  union {
    u8 saved_serialized_exit_optional_font[4];
    u32 saved_serialized_exit_optional_font_word;
  };

  union {
    u8 saved_serialized_canvas1_ptr[4];
    u32 saved_serialized_canvas1_ptr_word;
  };

  union {
    u8 saved_serialized_font_astandar[4];
    u32 saved_serialized_font_astandar_word;
  };

  union {
    u8 saved_serialized_recipe_lookup_fallback[4];
    u32 saved_serialized_recipe_lookup_fallback_word;
  };

  union {
    u8 saved_serialized_some_font_ptr[4];
    u32 saved_serialized_some_font_ptr_word;
  };

  union {
    u8 saved_serialized_font_supersma[4];
    u32 saved_serialized_font_supersma_word;
  };

  union {
    u8 saved_serialized_font_ruby[4];
    u32 saved_serialized_font_ruby_word;
  };

  union {
    u8 saved_serialized_font_piznumb[4];
    u32 saved_serialized_font_piznumb_word;
  };

  union {
    u8 saved_serialized_buffer_2[4];
    u32 saved_serialized_buffer_2_word;
  };

  union {
    u8 saved_serialized_current_bitmap[4];
    u32 saved_serialized_current_bitmap_word;
  };

  union {
    u8 saved_serialized_old_bitmap[4];
    u32 saved_serialized_old_bitmap_word;
  };

  union {
    u8 saved_serialized_active_palette_pointer[4];
    u32 saved_serialized_active_palette_pointer_word;
  };

  union {
    u8 saved_serialized_ptr_apple[4];
    u32 saved_serialized_ptr_apple_word;
  };

  union {
    u8 saved_serialized_curr_rest[4];
    u32 saved_serialized_curr_rest_word;
  };

  union {
    u8 saved_serialized_game_ptr[4];
    u32 saved_serialized_game_ptr_word;
  };

  union {
    u8 saved_serialized_mouse_result[4];
    u32 saved_serialized_mouse_result_word;
  };

  union {
    u8 saved_serialized_jatoba_1[4];
    u32 saved_serialized_jatoba_1_word;
  };

  union {
    u8 saved_serialized_canvas_1[4];
    u32 saved_serialized_canvas_1_word;
  };

  u8 saved_yaffa_value[50];

  u8 saved_yaffa_counter[50];

  PTSavedPig saved_serialized_walking_guest;

  PTSavedBitmap saved_serialized_bmp6;

  PTSavedBitmap saved_serialized_bmp15;

  PTSavedBitmap saved_serialized_bmp5;

  PTSavedBitmap saved_serialized_bmp14;

  union {
    u8 saved_serialized_temp_palette_slot[4];
    u32 saved_serialized_temp_palette_slot_word;
  };

  PTSavedBitmap saved_serialized_bmp3;

  PTSavedBitmap saved_serialized_bmp11;

  PTSavedBitmap saved_serialized_bmp2;

  PTSavedBitmap saved_serialized_bmp1;

  PTSavedBitmap saved_serialized_bmp10;

  PTSavedBitmap saved_serialized_bmp12;

  u8 saved_did_run_town_day_loop_1[10];

  PTSavedHazelnut saved_serialized_temp_hazel;

  SavegameSection0 saved_savegame_sect_0;

  RamTown saved_ramtown;

  u16 saved_active_input_value;

  u16 saved_did_load_map_and_bez;

  i16 saved_cur_list_elem_x;

  i16 saved_cur_list_elem_y;

  i16 saved_mouse_x;

  i16 saved_mouse_y;

  u16 saved_mouse_pressed;

  PTSavedRest saved_serialized_temp_rest;

  u8 saved_papaya_array[3];

  PTSavedApple saved_serialized_room;

  PTSavedIglooLink saved_serialized_temp_igloo_link;

  PTSavedIglooLink saved_serialized_map_igloo_link;

  PTSavedBitmap saved_serialized_bmp16;

  PTSavedBitmap saved_serialized_bmp25;

  PTSavedBitmap saved_serialized_bmp24;

  PTSavedBitmap saved_serialized_bmp_background;

  PTSavedBitmap saved_serialized_bmp_awards;

  PTSavedBitmap saved_serialized_bmp17;

  PTSavedCanvas saved_serialized_canvas1;

  CharacterFileImage saved_loaded_character;

  Character saved_temp_character;

  PTSavedBitmap saved_serialized_bmp_blitmap;

  u8 saved_map_scroll_x;

  u8 saved_map_scroll_y;

  u8 saved_attack_contamination;

  u8 saved_best_score_player;

  u8 saved_state_slot_014;

  u8 saved_map_actor_count;

  u8 saved_state_slot_015;

  u8 saved_curr_score;

  u8 saved_state_slot_016;

  u8 saved_state_slot_017;

  u8 saved_guest_count;

  u8 saved_player_stats_value_x;

  u8 saved_draw_list_elem_style;

  u8 saved_did_terminal_stuff;

  u8 saved_attack_police_outcome;

  u8 saved_max_num_characters;

  u8 saved_current_player_index;

  u8 saved_staff_display_flag;

  float saved_shop_purchase_secondary_price[24];

  float saved_shop_secondary_purchase_amount;

  u8 saved_temp_cutoff[50];

  u8 saved_depot_export_country;

  u32 saved_legacy_checksum_value;

  u32 saved_legacy_checksum_size;

  union {
    u8 saved_serialized_cursor_bitmap[4];
    u32 saved_serialized_cursor_bitmap_word;
  };

  union {
    u8 saved_serialized_cursor_background_bitmap[4];
    u32 saved_serialized_cursor_background_bitmap_word;
  };

  union {
    u8 saved_serialized_canvas_3[4];
    u32 saved_serialized_canvas_3_word;
  };

  union {
    u8 saved_serialized_map_background_buffer[4];
    u32 saved_serialized_map_background_buffer_word;
  };

  union {
    u8 saved_serialized_cursor_target_bitmap[4];
    u32 saved_serialized_cursor_target_bitmap_word;
  };

  u32 saved_serialized_display_bias;

  union {
    u8 saved_serialized_transparent_mask_buffer[4];
    u32 saved_serialized_transparent_mask_buffer_word;
  };

  union {
    u8 saved_serialized_some_pal_p[4];
    u32 saved_serialized_some_pal_p_word;
  };

  u16 saved_startup_word0;

  u16 saved_startup_word1;

  u16 saved_startup_word2;

  PTSavedBitmap saved_serialized_mouse_bitmap_1;

  PTSavedBitmap saved_serialized_mouse_bitmap_2;

  union {
    u8 saved_serialized_interactive_attack_attacker[4];
    u32 saved_serialized_interactive_attack_attacker_word;
  };

  union {
    u8 saved_serialized_interactive_attack_victim[4];
    u32 saved_serialized_interactive_attack_victim_word;
  };

  PTSavedPig saved_serialized_g_pigs[2];

  u32 saved_rest_path_visits;

  i16 saved_legacy_collision_x[2];

  i16 saved_legacy_collision_y[2];

  i16 saved_guest_portrait_x;

  i16 saved_guest_portrait_y;

  u16 saved_state_slot_031;

  i16 saved_legacy_walk_x;

  i16 saved_legacy_walk_y;

  u8 saved_state_slot_018;

  u8 saved_rest_path_target_x;

  u8 saved_rest_path_target_y;

  u8 saved_legacy_walk_direction;

  u8 saved_state_slot_019;

  u8 saved_state_slot_020;

  union {
    u8 saved_serialized_recipe_rank_character[4];
    u32 saved_serialized_recipe_rank_character_word;
  };

  u8 saved_time_left_text[12];

  u8 saved_selected_filename[18];

  u8 saved_savegame_slot;

  u8 saved_map_marker_phase;

  u8 saved_animated_order_snapshot;

  u8 saved_animated_pizza_snapshot;

  u8 saved_animated_finished_snapshot;

  u8 saved_animated_order_max;

  u8 saved_animated_oven_baking_time[10];

  u8 saved_animated_oven_capacity[10];

  u8 saved_animated_oven_used[10];

  u8 saved_animated_finished_count;

  u8 saved_animated_order_count;

  u8 saved_animated_stats_needle;

  u8 saved_animated_oven_count;

  u8 saved_animated_stats_arrow_drawn;

  u8 saved_animated_pizza_count;

  u8 saved_animated_pizza_max;

  u8 saved_dialled_number_string[8];

  u8 saved_dial_number_buffer[8];

  u8 saved_recipe_rank_flag;

  u8 saved_mission_date_buffers[2][60];

  u8 saved_mission_place_buffers[2][80];

  u8 saved_mission_text_string[300];

  u8 saved_temp_string[80];

  u8 saved_retained_message_text[200];

  u8 saved_restaurant_name_text[37];

  u8 saved_canvas_menu_backup;

  u8 saved_placement_contact_types[8];

  u8 saved_placement_contact_indices[8];

  u8 saved_placement_contact_amounts[8];

  union {
    u8 saved_serialized_placement_contact_items[14][4];
    u32 saved_serialized_placement_contact_items_word[14];
  };

  u8 saved_furniture_quality_in_van[164];

  u8 saved_placement_contact_recorded;

  union {
    u8 saved_serialized_sound_var_2[4];
    u32 saved_serialized_sound_var_2_word;
  };

  union {
    u8 saved_serialized_sound_var_1[4];
    u32 saved_serialized_sound_var_1_word;
  };

  u8 saved_sound_flag;

  u16 saved_legacy_menu_sales[24];

  u16 saved_legacy_menu_orders[24];

  u8 saved_advert_newspaper_lines[5][50];

  u8 saved_advert_newspaper_buffer[42];

  u8 saved_newspaper_text_buffer2[400];

  u8 saved_newspaper_text_buffer[80];

  union {
    u8 saved_serialized_newspaper_article_buffer[4];
    u32 saved_serialized_newspaper_article_buffer_word;
  };

  u8 saved_newspaper_delay;

  u8 saved_newpaper_rand;

  u8 saved_newspaper_random_direction;

  u8 saved_newspaper_random_mode;

  u8 saved_newspaper_background;

  u8 saved_newspaper_ingredient;

  u8 saved_did_run_town_day_loop_2[10];

  u8 saved_did_run_town_day_loop_3[10];

  u8 saved_state_slot_021;

  u32 saved_sound_driver_interrupt;

  union {
    u8 saved_serialized_sound_pcm_repeat_start[4];
    u32 saved_serialized_sound_pcm_repeat_start_word;
  };

  union {
    u8 saved_serialized_sound_pcm_output[4];
    u32 saved_serialized_sound_pcm_output_word;
  };

  union {
    u8 saved_serialized_sound_pcm_repeat_end[4];
    u32 saved_serialized_sound_pcm_repeat_end_word;
  };

  u32 saved_sound_pcm_repeats;

  u32 saved_sound_pcm_auto_dma;

  u32 saved_sound_pcm_dma_half;

  union {
    u8 saved_serialized_sound_dma_record[4];
    u32 saved_serialized_sound_dma_record_word;
  };

  union {
    u8 saved_serialized_sound_pcm_buffers[4];
    u32 saved_serialized_sound_pcm_buffers_word;
  };

  u16 saved_sound_digital_configuration;

  u16 saved_sound_config_fallback_word;

  union {
    u8 saved_serialized_guest_free_file_nodes[4];
    u32 saved_serialized_guest_free_file_nodes_word;
  };

  u8 saved_state_slot_022;

  union {
    u8 saved_serialized_guest_active_file_nodes[4];
    u32 saved_serialized_guest_active_file_nodes_word;
  };

  u32 saved_delay_polls_per_second;

  union {
    u8 saved_serialized_sound_dos_blocks[256][4];
    u32 saved_serialized_sound_dos_blocks_word[256];
  };

  u16 saved_sound_dos_selectors[256];

  u32 saved_guest_dos_heap_reserve;

  u32 saved_guest_dos_errno;

  u32 saved_guest_errno;
  u8 saved_interface_file_menu_font[15];
  u8 saved_interface_file_ruby_font[15];
  u8 saved_interface_file_font_desc_cstandar[19];
  u8 saved_interface_file_font_desc_piznumb[19];
  u8 saved_interface_file_topaz[11];
  u8 saved_interface_file_font_desc_astandar[19];
  u8 saved_interface_file_font_desc_supersma[19];
  u8 saved_interface_empty_unit[1];
  u8 saved_interface_hint_map[4];
  u8 saved_interface_hint_character[10];
  u8 saved_interface_hint_disk_options[13];
  u8 saved_interface_hint_advertising[12];
  u8 saved_interface_hint_market_research[16];
  u8 saved_interface_hint_newspaper[10];
  u8 saved_interface_hint_global_stats[13];
  u8 saved_interface_hint_terminal[9];
  u8 saved_interface_hint_end_turn[9];
  u8 saved_interface_hint_open_close[11];
  u8 saved_interface_hint_stock[6];
  u8 saved_interface_hint_menu[5];
  u8 saved_interface_hint_furnishing[11];
  u8 saved_interface_hint_staff[6];
  u8 saved_interface_hint_recipe_list[12];
  u8 saved_interface_hint_restaurant_stats[17];
  u8 saved_interface_hint_animated_stats[15];
  u8 saved_interface_hint_vegetables[11];
  u8 saved_interface_hint_meat[5];
  u8 saved_interface_hint_contracts[10];
  u8 saved_interface_hint_fruit[6];
  u8 saved_interface_hint_cheese[7];
  u8 saved_interface_hint_shop[5];
  u8 saved_interface_hint_back[5];
  u8 saved_interface_hint_reject[7];
  u8 saved_interface_hint_agree[6];
  u8 saved_interface_hint_furniture[10];
  u8 saved_interface_hint_kitchenware[12];
  u8 saved_interface_hint_customer_ratings[17];
  u8 saved_interface_hint_extras[7];
  u8 saved_interface_hint_flooring[9];
  u8 saved_interface_hint_weapons[8];
  u8 saved_interface_hint_single_order[13];
  u8 saved_interface_hint_make_contract[14];
  u8 saved_interface_hint_turn_page[10];
  u8 saved_interface_hint_pizza_sales[12];
  u8 saved_interface_hint_remove_pizza[13];
  u8 saved_interface_hint_view_pizza[11];
  u8 saved_interface_hint_ingredients[12];
  u8 saved_interface_hint_pizza_info[11];
  u8 saved_interface_hint_o_k[5];
  u8 saved_interface_hint_flip_page[10];
  u8 saved_interface_hint_cancel_contract[16];
  u8 saved_interface_hint_chefs[6];
  u8 saved_interface_hint_waiters[8];
  u8 saved_interface_hint_managers[9];
  u8 saved_interface_hint_fire[5];
  u8 saved_interface_hint_wages[6];
  u8 saved_interface_hint_training[9];
  u8 saved_interface_hint_recruit[8];
  u8 saved_interface_hint_production[11];
  u8 saved_interface_hint_reputation[11];
  u8 saved_interface_hint_capacity[9];
  u8 saved_interface_hint_pizza_quality[14];
  u8 saved_interface_hint_popularity[11];
  u8 saved_interface_hint_turnover[9];
  u8 saved_interface_hint_data[5];
  u8 saved_interface_hint_applications[13];
  u8 saved_interface_hint_hire[5];
  u8 saved_interface_hint_buy[4];
  u8 saved_interface_hint_dial_1[7];
  u8 saved_interface_hint_dial_2[7];
  u8 saved_interface_hint_dial_3[7];
  u8 saved_interface_hint_dial_4[7];
  u8 saved_interface_hint_dial_5[7];
  u8 saved_interface_hint_dial_6[7];
  u8 saved_interface_hint_dial_7[7];
  u8 saved_interface_hint_dial_8[7];
  u8 saved_interface_hint_dial_9[7];
  u8 saved_interface_hint_character_values[17];
  u8 saved_interface_hint_company[8];
  u8 saved_interface_hint_curriculum_vitae[17];
  u8 saved_interface_hint_position[9];
  u8 saved_interface_hint_awards[7];
  u8 saved_interface_hint_target_cv[10];
  u8 saved_interface_hint_challenge[10];
  u8 saved_interface_hint_sabotage[9];
  u8 saved_interface_hint_transfer[9];
  u8 saved_interface_hint_bake_pizza[11];
  u8 saved_interface_hint_export[7];
  u8 saved_interface_hint_priorities[11];
  u8 saved_interface_hint_sell[5];
  u8 saved_interface_hint_next_item[10];
  u8 saved_interface_hint_throw_away[11];
  u8 saved_interface_hint_previous_item[14];
  u8 saved_interface_hint_month_overview[15];
  u8 saved_interface_hint_restaurants[12];
  u8 saved_interface_hint_cash_flow[10];
  u8 saved_interface_hint_loans[6];
  u8 saved_interface_hint_insurance[10];
  u8 saved_interface_hint_property[9];
  u8 saved_interface_hint_balance[8];
  u8 saved_interface_hint_daily_profits[14];
  u8 saved_interface_hint_daily_balance[14];
  u8 saved_interface_hint_week_review[12];
  u8 saved_interface_hint_month_review[13];
  u8 saved_interface_hint_week_s_takings[15];
  u8 saved_interface_hint_customer_stats[15];
  u8 saved_interface_hint_sales_curve[12];
  u8 saved_interface_hint_area_info[10];
  u8 saved_interface_hint_economy[8];
  u8 saved_interface_hint_customer_taste[15];
  u8 saved_interface_hint_trends[7];
  u8 saved_interface_hint_pizza_charts[13];
  u8 saved_interface_hint_competitors[12];
  u8 saved_interface_hint_market_share[13];
  u8 saved_interface_hint_continue[9];
  u8 saved_interface_hint_move_elsewhere[15];
  u8 saved_interface_hint_sell_it[8];
  u8 saved_interface_hint_dial_new_number[16];
  u8 saved_interface_hint_view_ads[9];
  u8 saved_initial_music_status[1];
} PTNamedMemory;
#pragma pack(pop)
enum {
  PT_SAVE_PAGE = 4096,
  PT_SAVE_GLOBAL_CAPACITY = (sizeof(PTNamedMemory) + PT_SAVE_PAGE - 1) / PT_SAVE_PAGE * PT_SAVE_PAGE,
  PT_MEMORY_SIZE = PT_SAVE_GLOBAL_CAPACITY + 4 * 1024 * 1024,
  PT_STACK_BASE = PT_MEMORY_SIZE - PT_SAVE_PAGE
};
typedef union PTGuestMemory {
  u8 bytes[PT_MEMORY_SIZE];
  PTNamedMemory globals;
} PTGuestMemory;
extern PTGuestMemory pt_memory;

enum { PT_FIELD_sound_binary_read_mode = (offsetof(PTNamedMemory, saved_sound_binary_read_mode)) };
_Static_assert(offsetof(PTNamedMemory, saved_sound_binary_read_mode) == PT_FIELD_sound_binary_read_mode,
    "sound_binary_read_mode address");
enum { PT_FIELD_land_symbol_filename = (offsetof(PTNamedMemory, saved_land_symbol_filename)) };
_Static_assert(offsetof(PTNamedMemory, saved_land_symbol_filename) == PT_FIELD_land_symbol_filename,
    "land_symbol_filename address");
_Static_assert(sizeof(PTGuestMemory) == PT_MEMORY_SIZE, "memory size");
enum { PT_FIELD_staff_hire_role_labels = (offsetof(PTNamedMemory, saved_staff_hire_role_labels)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_hire_role_labels) == PT_FIELD_staff_hire_role_labels,
    "staff_hire_role_labels address");
enum {
  PT_FIELD_contract_justification_probabilities = (offsetof(PTNamedMemory, saved_contract_justification_probabilities))
};
_Static_assert(offsetof(PTNamedMemory, saved_contract_justification_probabilities) ==
        PT_FIELD_contract_justification_probabilities,
    "contract_justification_probabilities address");
enum { PT_FIELD_contract_days_literal = (offsetof(PTNamedMemory, saved_contract_days_literal)) };
_Static_assert(offsetof(PTNamedMemory, saved_contract_days_literal) == PT_FIELD_contract_days_literal,
    "contract_days_literal address");
enum { PT_FIELD_manager_turnover_names = (offsetof(PTNamedMemory, saved_manager_turnover_names)) };
_Static_assert(offsetof(PTNamedMemory, saved_manager_turnover_names) == PT_FIELD_manager_turnover_names,
    "manager_turnover_names address");
enum { PT_FIELD_attack_health_left = (offsetof(PTNamedMemory, saved_attack_health_left)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_attack_health_left) == PT_FIELD_attack_health_left, "attack_health_left address");
enum { PT_FIELD_attack_health_right = (offsetof(PTNamedMemory, saved_attack_health_right)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_attack_health_right) == PT_FIELD_attack_health_right, "attack_health_right address");
enum { PT_FIELD_recipe_jury_x = (offsetof(PTNamedMemory, saved_recipe_jury_x)) };
_Static_assert(offsetof(PTNamedMemory, saved_recipe_jury_x) == PT_FIELD_recipe_jury_x, "recipe_jury_x address");
enum { PT_FIELD_recipe_chart_label_x = (offsetof(PTNamedMemory, saved_recipe_chart_label_x)) };
_Static_assert(offsetof(PTNamedMemory, saved_recipe_chart_label_x) == PT_FIELD_recipe_chart_label_x,
    "recipe_chart_label_x address");
enum { PT_FIELD_recipe_chart_x = (offsetof(PTNamedMemory, saved_recipe_chart_x)) };
_Static_assert(offsetof(PTNamedMemory, saved_recipe_chart_x) == PT_FIELD_recipe_chart_x, "recipe_chart_x address");
enum { PT_FIELD_ingredient_trend_names = (offsetof(PTNamedMemory, saved_ingredient_trend_names)) };
_Static_assert(offsetof(PTNamedMemory, saved_ingredient_trend_names) == PT_FIELD_ingredient_trend_names,
    "ingredient_trend_names address");
enum { PT_FIELD_selector_patterns = (offsetof(PTNamedMemory, saved_selector_patterns)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_selector_patterns) == PT_FIELD_selector_patterns, "selector_patterns address");
enum { PT_FIELD_animated_quality_x = (offsetof(PTNamedMemory, saved_animated_quality_x)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_animated_quality_x) == PT_FIELD_animated_quality_x, "animated_quality_x address");
enum { PT_FIELD_animated_quality_y = (offsetof(PTNamedMemory, saved_animated_quality_y)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_animated_quality_y) == PT_FIELD_animated_quality_y, "animated_quality_y address");
enum { PT_FIELD_animated_capacity_colors = (offsetof(PTNamedMemory, saved_animated_capacity_colors)) };
_Static_assert(offsetof(PTNamedMemory, saved_animated_capacity_colors) == PT_FIELD_animated_capacity_colors,
    "animated_capacity_colors address");
enum { PT_FIELD_valid_bank_account_numbers = (offsetof(PTNamedMemory, saved_valid_bank_account_numbers)) };
_Static_assert(offsetof(PTNamedMemory, saved_valid_bank_account_numbers) == PT_FIELD_valid_bank_account_numbers,
    "valid_bank_account_numbers address");
enum { PT_FIELD_mission_reward_bases = (offsetof(PTNamedMemory, saved_mission_reward_bases)) };
_Static_assert(offsetof(PTNamedMemory, saved_mission_reward_bases) == PT_FIELD_mission_reward_bases,
    "mission_reward_bases address");
enum { PT_FIELD_mission_place_articles = (offsetof(PTNamedMemory, saved_mission_place_articles)) };
_Static_assert(offsetof(PTNamedMemory, saved_mission_place_articles) == PT_FIELD_mission_place_articles,
    "mission_place_articles address");
enum { PT_FIELD_mission_place_article_groups = (offsetof(PTNamedMemory, saved_mission_place_article_groups)) };
_Static_assert(offsetof(PTNamedMemory, saved_mission_place_article_groups) == PT_FIELD_mission_place_article_groups,
    "mission_place_article_groups address");
enum { PT_FIELD_mission_place_name_prefixes = (offsetof(PTNamedMemory, saved_mission_place_name_prefixes)) };
_Static_assert(offsetof(PTNamedMemory, saved_mission_place_name_prefixes) == PT_FIELD_mission_place_name_prefixes,
    "mission_place_name_prefixes address");
enum { PT_FIELD_paragraph_width_characters = (offsetof(PTNamedMemory, saved_paragraph_width_characters)) };
_Static_assert(offsetof(PTNamedMemory, saved_paragraph_width_characters) == PT_FIELD_paragraph_width_characters,
    "paragraph_width_characters address");
enum { PT_FIELD_furniture_extras_labels = (offsetof(PTNamedMemory, saved_furniture_extras_labels)) };
_Static_assert(offsetof(PTNamedMemory, saved_furniture_extras_labels) == PT_FIELD_furniture_extras_labels,
    "furniture_extras_labels address");
enum { PT_FIELD_profit_graph_colors = (offsetof(PTNamedMemory, saved_profit_graph_colors)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_profit_graph_colors) == PT_FIELD_profit_graph_colors, "profit_graph_colors address");
enum { PT_FIELD_profit_graph_thresholds = (offsetof(PTNamedMemory, saved_profit_graph_thresholds)) };
_Static_assert(offsetof(PTNamedMemory, saved_profit_graph_thresholds) == PT_FIELD_profit_graph_thresholds,
    "profit_graph_thresholds address");
enum { PT_FIELD_weekly_default_staff_name = (offsetof(PTNamedMemory, saved_weekly_default_staff_name)) };
_Static_assert(offsetof(PTNamedMemory, saved_weekly_default_staff_name) == PT_FIELD_weekly_default_staff_name,
    "weekly_default_staff_name address");
enum { PT_FIELD_sound_vector_registers = (offsetof(PTNamedMemory, saved_sound_vector_registers)) };
_Static_assert(offsetof(PTNamedMemory, saved_sound_vector_registers) == PT_FIELD_sound_vector_registers,
    "sound_vector_registers address");
enum { PT_FIELD_sound_vector_previous_offset = (offsetof(PTNamedMemory, saved_sound_vector_previous_offset)) };
_Static_assert(offsetof(PTNamedMemory, saved_sound_vector_previous_offset) == PT_FIELD_sound_vector_previous_offset,
    "sound_vector_previous_offset address");
enum { PT_FIELD_sound_vector_previous_segment = (offsetof(PTNamedMemory, saved_sound_vector_previous_segment)) };
_Static_assert(offsetof(PTNamedMemory, saved_sound_vector_previous_segment) == PT_FIELD_sound_vector_previous_segment,
    "sound_vector_previous_segment address");
enum { PT_FIELD_mouse_buttons = (offsetof(PTNamedMemory, saved_mouse_buttons)) };
_Static_assert(offsetof(PTNamedMemory, saved_mouse_buttons) == PT_FIELD_mouse_buttons, "mouse_buttons address");
enum { PT_FIELD_input_event_filter = (offsetof(PTNamedMemory, saved_input_event_filter)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_input_event_filter) == PT_FIELD_input_event_filter, "input_event_filter address");
enum { PT_FIELD_last_bios_key = (offsetof(PTNamedMemory, saved_last_bios_key)) };
_Static_assert(offsetof(PTNamedMemory, saved_last_bios_key) == PT_FIELD_last_bios_key, "last_bios_key address");
enum { PT_FIELD_command_line_arg = (offsetof(PTNamedMemory, saved_command_line_arg)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_command_line_arg) == PT_FIELD_command_line_arg, "command_line_arg address");
enum { PT_FIELD_serialized_display_timing = (offsetof(PTNamedMemory, saved_serialized_display_timing)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_display_timing) == PT_FIELD_serialized_display_timing,
    "serialized_display_timing address");
enum { PT_FIELD_in_bitblt = (offsetof(PTNamedMemory, saved_in_bitblt)) };
_Static_assert(offsetof(PTNamedMemory, saved_in_bitblt) == PT_FIELD_in_bitblt, "in_bitblt address");
enum { PT_FIELD_mouse_pos_x = (offsetof(PTNamedMemory, saved_mouse_pos_x)) };
_Static_assert(offsetof(PTNamedMemory, saved_mouse_pos_x) == PT_FIELD_mouse_pos_x, "mouse_pos_x address");
enum { PT_FIELD_mouse_pos_y = (offsetof(PTNamedMemory, saved_mouse_pos_y)) };
_Static_assert(offsetof(PTNamedMemory, saved_mouse_pos_y) == PT_FIELD_mouse_pos_y, "mouse_pos_y address");
enum { PT_FIELD_mouse_move_pending = (offsetof(PTNamedMemory, saved_mouse_move_pending)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_mouse_move_pending) == PT_FIELD_mouse_move_pending, "mouse_move_pending address");
enum { PT_FIELD_pending_mouse_x = (offsetof(PTNamedMemory, saved_pending_mouse_x)) };
_Static_assert(offsetof(PTNamedMemory, saved_pending_mouse_x) == PT_FIELD_pending_mouse_x, "pending_mouse_x address");
enum { PT_FIELD_pending_mouse_y = (offsetof(PTNamedMemory, saved_pending_mouse_y)) };
_Static_assert(offsetof(PTNamedMemory, saved_pending_mouse_y) == PT_FIELD_pending_mouse_y, "pending_mouse_y address");
enum { PT_FIELD_cursor_offset_x = (offsetof(PTNamedMemory, saved_cursor_offset_x)) };
_Static_assert(offsetof(PTNamedMemory, saved_cursor_offset_x) == PT_FIELD_cursor_offset_x, "cursor_offset_x address");
enum { PT_FIELD_cursor_offset_y = (offsetof(PTNamedMemory, saved_cursor_offset_y)) };
_Static_assert(offsetof(PTNamedMemory, saved_cursor_offset_y) == PT_FIELD_cursor_offset_y, "cursor_offset_y address");
enum { PT_FIELD_cursor_is_visible = (offsetof(PTNamedMemory, saved_cursor_is_visible)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_cursor_is_visible) == PT_FIELD_cursor_is_visible, "cursor_is_visible address");
enum { PT_FIELD_did_hide_mouse_cursor = (offsetof(PTNamedMemory, saved_did_hide_mouse_cursor)) };
_Static_assert(offsetof(PTNamedMemory, saved_did_hide_mouse_cursor) == PT_FIELD_did_hide_mouse_cursor,
    "did_hide_mouse_cursor address");
enum { PT_FIELD_cursor_hide_depth = (offsetof(PTNamedMemory, saved_cursor_hide_depth)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_cursor_hide_depth) == PT_FIELD_cursor_hide_depth, "cursor_hide_depth address");
enum { PT_FIELD_halve_mouse_x = (offsetof(PTNamedMemory, saved_halve_mouse_x)) };
_Static_assert(offsetof(PTNamedMemory, saved_halve_mouse_x) == PT_FIELD_halve_mouse_x, "halve_mouse_x address");
enum { PT_FIELD_cursor_screen_buffer = (offsetof(PTNamedMemory, saved_serialized_cursor_screen_buffer)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_cursor_screen_buffer) == PT_FIELD_cursor_screen_buffer,
    "cursor_screen_buffer address");
enum { PT_FIELD_mouse_binding_count = (offsetof(PTNamedMemory, saved_mouse_binding_count)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_mouse_binding_count) == PT_FIELD_mouse_binding_count, "mouse_binding_count address");
enum { PT_FIELD_mouse_position_bindings = (offsetof(PTNamedMemory, saved_serialized_mouse_position_bindings)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_mouse_position_bindings) == PT_FIELD_mouse_position_bindings,
    "mouse_position_bindings address");
enum { PT_FIELD_transparent_color = (offsetof(PTNamedMemory, saved_transparent_color)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_transparent_color) == PT_FIELD_transparent_color, "transparent_color address");
enum { PT_FIELD_glyph_bitmap = (offsetof(PTNamedMemory, saved_serialized_glyph_bitmap)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_glyph_bitmap) == PT_FIELD_glyph_bitmap, "glyph_bitmap address");
enum { PT_FIELD_glyph_pixels = (offsetof(PTNamedMemory, saved_glyph_pixels)) };
_Static_assert(offsetof(PTNamedMemory, saved_glyph_pixels) == PT_FIELD_glyph_pixels, "glyph_pixels address");
enum { PT_FIELD_input_queue_capacity = (offsetof(PTNamedMemory, saved_input_queue_capacity)) };
_Static_assert(offsetof(PTNamedMemory, saved_input_queue_capacity) == PT_FIELD_input_queue_capacity,
    "input_queue_capacity address");
enum { PT_FIELD_input_queue_head = (offsetof(PTNamedMemory, saved_input_queue_head)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_input_queue_head) == PT_FIELD_input_queue_head, "input_queue_head address");
enum { PT_FIELD_input_queue_count = (offsetof(PTNamedMemory, saved_input_queue_count)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_input_queue_count) == PT_FIELD_input_queue_count, "input_queue_count address");
enum { PT_FIELD_input_events = (offsetof(PTNamedMemory, saved_input_events)) };
_Static_assert(offsetof(PTNamedMemory, saved_input_events) == PT_FIELD_input_events, "input_events address");
enum { PT_FIELD_smallrects = (offsetof(PTNamedMemory, saved_smallrects)) };
_Static_assert(offsetof(PTNamedMemory, saved_smallrects) == PT_FIELD_smallrects, "smallrects address");
enum { PT_FIELD_num_ingredients_in_set = (offsetof(PTNamedMemory, saved_num_ingredients_in_set)) };
_Static_assert(offsetof(PTNamedMemory, saved_num_ingredients_in_set) == PT_FIELD_num_ingredients_in_set,
    "num_ingredients_in_set address");
enum { PT_FIELD_customer_age_names = (offsetof(PTNamedMemory, saved_customer_age_names)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_customer_age_names) == PT_FIELD_customer_age_names, "customer_age_names address");
enum { PT_FIELD_customer_types = (offsetof(PTNamedMemory, saved_customer_types)) };
_Static_assert(offsetof(PTNamedMemory, saved_customer_types) == PT_FIELD_customer_types, "customer_types address");
enum { PT_FIELD_property_names = (offsetof(PTNamedMemory, saved_property_names)) };
_Static_assert(offsetof(PTNamedMemory, saved_property_names) == PT_FIELD_property_names, "property_names address");
enum { PT_FIELD_property_availability_names = (offsetof(PTNamedMemory, saved_property_availability_names)) };
_Static_assert(offsetof(PTNamedMemory, saved_property_availability_names) == PT_FIELD_property_availability_names,
    "property_availability_names address");
enum { PT_FIELD_ingredient_popup_x = (offsetof(PTNamedMemory, saved_ingredient_popup_x)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_ingredient_popup_x) == PT_FIELD_ingredient_popup_x, "ingredient_popup_x address");
enum { PT_FIELD_g_stock_name_kind = (offsetof(PTNamedMemory, saved_g_stock_name_kind)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_g_stock_name_kind) == PT_FIELD_g_stock_name_kind, "g_stock_name_kind address");
enum { PT_FIELD_furniture_desc = (offsetof(PTNamedMemory, saved_furniture_desc)) };
_Static_assert(offsetof(PTNamedMemory, saved_furniture_desc) == PT_FIELD_furniture_desc, "furniture_desc address");
enum { PT_FIELD_furniture_supplier_count = (offsetof(PTNamedMemory, saved_furniture_supplier_count)) };
_Static_assert(offsetof(PTNamedMemory, saved_furniture_supplier_count) == PT_FIELD_furniture_supplier_count,
    "furniture_supplier_count address");
enum { PT_FIELD_furniture_suppliers = (offsetof(PTNamedMemory, saved_furniture_suppliers)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_furniture_suppliers) == PT_FIELD_furniture_suppliers, "furniture_suppliers address");
enum { PT_FIELD_exit_code = (offsetof(PTNamedMemory, saved_exit_code)) };
_Static_assert(offsetof(PTNamedMemory, saved_exit_code) == PT_FIELD_exit_code, "exit_code address");
enum { PT_FIELD_force_map_input = (offsetof(PTNamedMemory, saved_force_map_input)) };
_Static_assert(offsetof(PTNamedMemory, saved_force_map_input) == PT_FIELD_force_map_input, "force_map_input address");
enum { PT_FIELD_days_in_month = (offsetof(PTNamedMemory, saved_days_in_month)) };
_Static_assert(offsetof(PTNamedMemory, saved_days_in_month) == PT_FIELD_days_in_month, "days_in_month address");
enum { PT_FIELD_menu_font = (offsetof(PTNamedMemory, saved_serialized_menu_font)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_menu_font) == PT_FIELD_menu_font, "menu_font address");
enum { PT_FIELD_ruby_font = (offsetof(PTNamedMemory, saved_serialized_ruby_font)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_ruby_font) == PT_FIELD_ruby_font, "ruby_font address");
enum { PT_FIELD_font_desc_cstandar = (offsetof(PTNamedMemory, saved_serialized_font_desc_cstandar)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_font_desc_cstandar) == PT_FIELD_font_desc_cstandar,
    "font_desc_cstandar address");
enum { PT_FIELD_font_desc_piznumb = (offsetof(PTNamedMemory, saved_serialized_font_desc_piznumb)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_font_desc_piznumb) == PT_FIELD_font_desc_piznumb,
    "font_desc_piznumb address");
enum { PT_FIELD_font_desc_astandar = (offsetof(PTNamedMemory, saved_serialized_font_desc_astandar)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_font_desc_astandar) == PT_FIELD_font_desc_astandar,
    "font_desc_astandar address");
enum { PT_FIELD_font_desc_supersma = (offsetof(PTNamedMemory, saved_serialized_font_desc_supersma)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_font_desc_supersma) == PT_FIELD_font_desc_supersma,
    "font_desc_supersma address");
enum { PT_FIELD_loaded_font = (offsetof(PTNamedMemory, saved_serialized_loaded_font)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_loaded_font) == PT_FIELD_loaded_font, "loaded_font address");
enum { PT_FIELD_temp_canvas = (offsetof(PTNamedMemory, saved_serialized_temp_canvas)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_temp_canvas) == PT_FIELD_temp_canvas, "temp_canvas address");
enum { PT_FIELD_bmp25_ptr = (offsetof(PTNamedMemory, saved_serialized_bmp25_ptr)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_bmp25_ptr) == PT_FIELD_bmp25_ptr, "bmp25_ptr address");
enum { PT_FIELD_bmp24_ptr = (offsetof(PTNamedMemory, saved_serialized_bmp24_ptr)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_bmp24_ptr) == PT_FIELD_bmp24_ptr, "bmp24_ptr address");
enum { PT_FIELD_bmp_character_face = (offsetof(PTNamedMemory, saved_serialized_bmp_character_face)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_bmp_character_face) == PT_FIELD_bmp_character_face,
    "bmp_character_face address");
enum { PT_FIELD_bmp22_ptr = (offsetof(PTNamedMemory, saved_serialized_bmp22_ptr)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_bmp22_ptr) == PT_FIELD_bmp22_ptr, "bmp22_ptr address");
enum { PT_FIELD_bmp21_ptr = (offsetof(PTNamedMemory, saved_serialized_bmp21_ptr)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_bmp21_ptr) == PT_FIELD_bmp21_ptr, "bmp21_ptr address");
enum { PT_FIELD_bmp20_ptr = (offsetof(PTNamedMemory, saved_serialized_bmp20_ptr)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_bmp20_ptr) == PT_FIELD_bmp20_ptr, "bmp20_ptr address");
enum { PT_FIELD_bmp26_ptr = (offsetof(PTNamedMemory, saved_serialized_bmp26_ptr)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_bmp26_ptr) == PT_FIELD_bmp26_ptr, "bmp26_ptr address");
enum { PT_FIELD_text_edit_bitmap = (offsetof(PTNamedMemory, saved_serialized_text_edit_bitmap)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_text_edit_bitmap) == PT_FIELD_text_edit_bitmap,
    "text_edit_bitmap address");
enum { PT_FIELD_temp_bitmap_84x6 = (offsetof(PTNamedMemory, saved_serialized_temp_bitmap_84x6)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_temp_bitmap_84x6) == PT_FIELD_temp_bitmap_84x6,
    "temp_bitmap_84x6 address");
enum { PT_FIELD_restaurant_mask_rect = (offsetof(PTNamedMemory, saved_restaurant_mask_rect)) };
_Static_assert(offsetof(PTNamedMemory, saved_restaurant_mask_rect) == PT_FIELD_restaurant_mask_rect,
    "restaurant_mask_rect address");
enum { PT_FIELD_rect_1 = (offsetof(PTNamedMemory, saved_serialized_rect_1)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_rect_1) == PT_FIELD_rect_1, "rect_1 address");
enum { PT_FIELD_bob_restaurant = (offsetof(PTNamedMemory, saved_serialized_bob_restaurant)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_serialized_bob_restaurant) == PT_FIELD_bob_restaurant, "bob_restaurant address");
enum { PT_FIELD_bob_ingredients = (offsetof(PTNamedMemory, saved_serialized_bob_ingredients)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_serialized_bob_ingredients) == PT_FIELD_bob_ingredients, "bob_ingredients address");
enum { PT_FIELD_bob_kitchen = (offsetof(PTNamedMemory, saved_serialized_bob_kitchen)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_bob_kitchen) == PT_FIELD_bob_kitchen, "bob_kitchen address");
enum { PT_FIELD_bob_restaurant_open = (offsetof(PTNamedMemory, saved_serialized_bob_restaurant_open)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_bob_restaurant_open) == PT_FIELD_bob_restaurant_open,
    "bob_restaurant_open address");
enum { PT_FIELD_bob_stanbobs = (offsetof(PTNamedMemory, saved_serialized_bob_stanbobs)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_bob_stanbobs) == PT_FIELD_bob_stanbobs, "bob_stanbobs address");
enum { PT_FIELD_bob_typ = (offsetof(PTNamedMemory, saved_serialized_bob_typ)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_bob_typ) == PT_FIELD_bob_typ, "bob_typ address");
enum { PT_FIELD_bob_walk = (offsetof(PTNamedMemory, saved_serialized_bob_walk)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_bob_walk) == PT_FIELD_bob_walk, "bob_walk address");
enum { PT_FIELD_bob7 = (offsetof(PTNamedMemory, saved_serialized_bob7)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_bob7) == PT_FIELD_bob7, "bob7 address");
enum { PT_FIELD_bob11 = (offsetof(PTNamedMemory, saved_serialized_bob11)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_bob11) == PT_FIELD_bob11, "bob11 address");
enum { PT_FIELD_menu_bob = (offsetof(PTNamedMemory, saved_serialized_menu_bob)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_menu_bob) == PT_FIELD_menu_bob, "menu_bob address");
enum { PT_FIELD_bob_restaurant_stats = (offsetof(PTNamedMemory, saved_serialized_bob_restaurant_stats)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_bob_restaurant_stats) == PT_FIELD_bob_restaurant_stats,
    "bob_restaurant_stats address");
enum { PT_FIELD_bob9 = (offsetof(PTNamedMemory, saved_serialized_bob9)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_bob9) == PT_FIELD_bob9, "bob9 address");
enum { PT_FIELD_langfile_artikel3 = (offsetof(PTNamedMemory, saved_serialized_langfile_artikel3)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_langfile_artikel3) == PT_FIELD_langfile_artikel3,
    "langfile_artikel3 address");
enum { PT_FIELD_langfile_credits = (offsetof(PTNamedMemory, saved_serialized_langfile_credits)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_langfile_credits) == PT_FIELD_langfile_credits,
    "langfile_credits address");
enum { PT_FIELD_langfile_ende = (offsetof(PTNamedMemory, saved_serialized_langfile_ende)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_serialized_langfile_ende) == PT_FIELD_langfile_ende, "langfile_ende address");
enum { PT_FIELD_langfile_gtalk0 = (offsetof(PTNamedMemory, saved_serialized_langfile_gtalk0)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_serialized_langfile_gtalk0) == PT_FIELD_langfile_gtalk0, "langfile_gtalk0 address");
enum { PT_FIELD_langfile_gtalk1 = (offsetof(PTNamedMemory, saved_serialized_langfile_gtalk1)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_serialized_langfile_gtalk1) == PT_FIELD_langfile_gtalk1, "langfile_gtalk1 address");
enum { PT_FIELD_langfile_markt = (offsetof(PTNamedMemory, saved_serialized_langfile_markt)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_serialized_langfile_markt) == PT_FIELD_langfile_markt, "langfile_markt address");
enum { PT_FIELD_langfile_action = (offsetof(PTNamedMemory, saved_serialized_langfile_action)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_serialized_langfile_action) == PT_FIELD_langfile_action, "langfile_action address");
enum { PT_FIELD_langfile_werbung = (offsetof(PTNamedMemory, saved_serialized_langfile_werbung)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_langfile_werbung) == PT_FIELD_langfile_werbung,
    "langfile_werbung address");
enum { PT_FIELD_langfile_levent = (offsetof(PTNamedMemory, saved_serialized_langfile_levent)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_serialized_langfile_levent) == PT_FIELD_langfile_levent, "langfile_levent address");
enum { PT_FIELD_langfile_mafia = (offsetof(PTNamedMemory, saved_serialized_langfile_mafia)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_serialized_langfile_mafia) == PT_FIELD_langfile_mafia, "langfile_mafia address");
enum { PT_FIELD_langfile_waffen = (offsetof(PTNamedMemory, saved_serialized_langfile_waffen)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_serialized_langfile_waffen) == PT_FIELD_langfile_waffen, "langfile_waffen address");
enum { PT_FIELD_langfile_realtors = (offsetof(PTNamedMemory, saved_serialized_langfile_realtors)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_langfile_realtors) == PT_FIELD_langfile_realtors,
    "langfile_realtors address");
enum { PT_FIELD_langfile_bank = (offsetof(PTNamedMemory, saved_serialized_langfile_bank)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_serialized_langfile_bank) == PT_FIELD_langfile_bank, "langfile_bank address");
enum { PT_FIELD_langfile_insurance = (offsetof(PTNamedMemory, saved_serialized_langfile_insurance)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_langfile_insurance) == PT_FIELD_langfile_insurance,
    "langfile_insurance address");
enum { PT_FIELD_langfile_townhall = (offsetof(PTNamedMemory, saved_serialized_langfile_townhall)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_langfile_townhall) == PT_FIELD_langfile_townhall,
    "langfile_townhall address");
enum { PT_FIELD_langfile_police = (offsetof(PTNamedMemory, saved_serialized_langfile_police)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_serialized_langfile_police) == PT_FIELD_langfile_police, "langfile_police address");
enum { PT_FIELD_langfile_lief = (offsetof(PTNamedMemory, saved_serialized_langfile_lief)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_serialized_langfile_lief) == PT_FIELD_langfile_lief, "langfile_lief address");
enum { PT_FIELD_langfile_leben = (offsetof(PTNamedMemory, saved_serialized_langfile_leben)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_serialized_langfile_leben) == PT_FIELD_langfile_leben, "langfile_leben address");
enum { PT_FIELD_langfile_pers = (offsetof(PTNamedMemory, saved_serialized_langfile_pers)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_serialized_langfile_pers) == PT_FIELD_langfile_pers, "langfile_pers address");
enum { PT_FIELD_langfile_persf = (offsetof(PTNamedMemory, saved_serialized_langfile_persf)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_serialized_langfile_persf) == PT_FIELD_langfile_persf, "langfile_persf address");
enum { PT_FIELD_langfile_staff_applications = (offsetof(PTNamedMemory, saved_serialized_langfile_staff_applications)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_serialized_langfile_staff_applications) == PT_FIELD_langfile_staff_applications,
    "langfile_staff_applications address");
enum { PT_FIELD_langfile_double = (offsetof(PTNamedMemory, saved_serialized_langfile_double)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_serialized_langfile_double) == PT_FIELD_langfile_double, "langfile_double address");
enum { PT_FIELD_mouse_over_zone = (offsetof(PTNamedMemory, saved_serialized_mouse_over_zone)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_serialized_mouse_over_zone) == PT_FIELD_mouse_over_zone, "mouse_over_zone address");
enum { PT_FIELD_last_mouse_over_district = (offsetof(PTNamedMemory, saved_serialized_last_mouse_over_district)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_last_mouse_over_district) == PT_FIELD_last_mouse_over_district,
    "last_mouse_over_district address");
enum { PT_FIELD_town_names = (offsetof(PTNamedMemory, saved_town_names)) };
_Static_assert(offsetof(PTNamedMemory, saved_town_names) == PT_FIELD_town_names, "town_names address");
enum { PT_FIELD_prefs = (offsetof(PTNamedMemory, saved_prefs)) };
_Static_assert(offsetof(PTNamedMemory, saved_prefs) == PT_FIELD_prefs, "prefs address");
enum { PT_FIELD_staff_ui_file_animation = (offsetof(PTNamedMemory, saved_staff_ui_file_animation)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_ui_file_animation) == PT_FIELD_staff_ui_file_animation,
    "staff_ui_file_animation address");
enum { PT_FIELD_state_slot_007 = (offsetof(PTNamedMemory, saved_state_slot_007)) };
_Static_assert(offsetof(PTNamedMemory, saved_state_slot_007) == PT_FIELD_state_slot_007, "state_slot_007 address");
enum { PT_FIELD_disable_draw_door = (offsetof(PTNamedMemory, saved_disable_draw_door)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_disable_draw_door) == PT_FIELD_disable_draw_door, "disable_draw_door address");
enum { PT_FIELD_terminal_manual_mode = (offsetof(PTNamedMemory, saved_terminal_manual_mode)) };
_Static_assert(offsetof(PTNamedMemory, saved_terminal_manual_mode) == PT_FIELD_terminal_manual_mode,
    "terminal_manual_mode address");
enum { PT_FIELD_state_slot_008 = (offsetof(PTNamedMemory, saved_state_slot_008)) };
_Static_assert(offsetof(PTNamedMemory, saved_state_slot_008) == PT_FIELD_state_slot_008, "state_slot_008 address");
enum { PT_FIELD_palette_fade_algo = (offsetof(PTNamedMemory, saved_palette_fade_algo)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_palette_fade_algo) == PT_FIELD_palette_fade_algo, "palette_fade_algo address");
enum { PT_FIELD_special_debt_flag = (offsetof(PTNamedMemory, saved_special_debt_flag)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_special_debt_flag) == PT_FIELD_special_debt_flag, "special_debt_flag address");
enum {
  PT_FIELD_disable_init_country_progress_msg = (offsetof(PTNamedMemory, saved_disable_init_country_progress_msg))
};
_Static_assert(
    offsetof(PTNamedMemory, saved_disable_init_country_progress_msg) == PT_FIELD_disable_init_country_progress_msg,
    "disable_init_country_progress_msg address");
enum { PT_FIELD_main_menu_canvas_flag = (offsetof(PTNamedMemory, saved_main_menu_canvas_flag)) };
_Static_assert(offsetof(PTNamedMemory, saved_main_menu_canvas_flag) == PT_FIELD_main_menu_canvas_flag,
    "main_menu_canvas_flag address");
enum { PT_FIELD_state_slot_009 = (offsetof(PTNamedMemory, saved_state_slot_009)) };
_Static_assert(offsetof(PTNamedMemory, saved_state_slot_009) == PT_FIELD_state_slot_009, "state_slot_009 address");
enum { PT_FIELD_game_exit_requested = (offsetof(PTNamedMemory, saved_game_exit_requested)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_game_exit_requested) == PT_FIELD_game_exit_requested, "game_exit_requested address");
enum { PT_FIELD_has_many_players = (offsetof(PTNamedMemory, saved_has_many_players)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_has_many_players) == PT_FIELD_has_many_players, "has_many_players address");
enum { PT_FIELD_unit_for_number_query = (offsetof(PTNamedMemory, saved_serialized_unit_for_number_query)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_unit_for_number_query) == PT_FIELD_unit_for_number_query,
    "unit_for_number_query address");
enum { PT_FIELD_jatoba_initializer = (offsetof(PTNamedMemory, saved_jatoba_initializer)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_jatoba_initializer) == PT_FIELD_jatoba_initializer, "jatoba_initializer address");
enum { PT_FIELD_mouse_initializer = (offsetof(PTNamedMemory, saved_serialized_mouse_initializer)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_mouse_initializer) == PT_FIELD_mouse_initializer,
    "mouse_initializer address");
enum { PT_FIELD_paragraph_highlight_override = (offsetof(PTNamedMemory, saved_paragraph_highlight_override)) };
_Static_assert(offsetof(PTNamedMemory, saved_paragraph_highlight_override) == PT_FIELD_paragraph_highlight_override,
    "paragraph_highlight_override address");
enum { PT_FIELD_season_map_colors = (offsetof(PTNamedMemory, saved_season_map_colors)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_season_map_colors) == PT_FIELD_season_map_colors, "season_map_colors address");
enum { PT_FIELD_map_animation_palette_index = (offsetof(PTNamedMemory, saved_map_animation_palette_index)) };
_Static_assert(offsetof(PTNamedMemory, saved_map_animation_palette_index) == PT_FIELD_map_animation_palette_index,
    "map_animation_palette_index address");
enum { PT_FIELD_g_button_config = (offsetof(PTNamedMemory, saved_g_button_config)) };
_Static_assert(offsetof(PTNamedMemory, saved_g_button_config) == PT_FIELD_g_button_config, "g_button_config address");
enum { PT_FIELD_button_icon_ids = (offsetof(PTNamedMemory, saved_button_icon_ids)) };
_Static_assert(offsetof(PTNamedMemory, saved_button_icon_ids) == PT_FIELD_button_icon_ids, "button_icon_ids address");
enum { PT_FIELD_button_texts = (offsetof(PTNamedMemory, saved_serialized_button_texts)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_button_texts) == PT_FIELD_button_texts, "button_texts address");
enum { PT_FIELD_button_x = (offsetof(PTNamedMemory, saved_button_x)) };
_Static_assert(offsetof(PTNamedMemory, saved_button_x) == PT_FIELD_button_x, "button_x address");
enum { PT_FIELD_button_y = (offsetof(PTNamedMemory, saved_button_y)) };
_Static_assert(offsetof(PTNamedMemory, saved_button_y) == PT_FIELD_button_y, "button_y address");
enum { PT_FIELD_player_ptr = (offsetof(PTNamedMemory, saved_serialized_player_ptr)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_player_ptr) == PT_FIELD_player_ptr, "player_ptr address");
enum { PT_FIELD_character_ptr = (offsetof(PTNamedMemory, saved_serialized_character_ptr)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_serialized_character_ptr) == PT_FIELD_character_ptr, "character_ptr address");
enum { PT_FIELD_invoice_month_names = (offsetof(PTNamedMemory, saved_invoice_month_names)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_invoice_month_names) == PT_FIELD_invoice_month_names, "invoice_month_names address");
enum { PT_FIELD_cur_town = (offsetof(PTNamedMemory, saved_serialized_cur_town)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_cur_town) == PT_FIELD_cur_town, "cur_town address");
enum { PT_FIELD_pizza_menu_info_separator = (offsetof(PTNamedMemory, saved_pizza_menu_info_separator)) };
_Static_assert(offsetof(PTNamedMemory, saved_pizza_menu_info_separator) == PT_FIELD_pizza_menu_info_separator,
    "pizza_menu_info_separator address");
enum { PT_FIELD_customer_eating_times = (offsetof(PTNamedMemory, saved_customer_eating_times)) };
_Static_assert(offsetof(PTNamedMemory, saved_customer_eating_times) == PT_FIELD_customer_eating_times,
    "customer_eating_times address");
enum { PT_FIELD_empty_restaurant_floor = (offsetof(PTNamedMemory, saved_empty_restaurant_floor)) };
_Static_assert(offsetof(PTNamedMemory, saved_empty_restaurant_floor) == PT_FIELD_empty_restaurant_floor,
    "empty_restaurant_floor address");
enum { PT_FIELD_ingredients = (offsetof(PTNamedMemory, saved_ingredients)) };
_Static_assert(offsetof(PTNamedMemory, saved_ingredients) == PT_FIELD_ingredients, "ingredients address");
enum { PT_FIELD_serialized_age_taste_preferences = (offsetof(PTNamedMemory, saved_age_taste_preferences)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_serialized_age_taste_preferences) == PT_FIELD_serialized_age_taste_preferences,
    "serialized_age_taste_preferences address");
enum { PT_FIELD_state_slot_010 = (offsetof(PTNamedMemory, saved_state_slot_010)) };
_Static_assert(offsetof(PTNamedMemory, saved_state_slot_010) == PT_FIELD_state_slot_010, "state_slot_010 address");
enum { PT_FIELD_intro_text_groups = (offsetof(PTNamedMemory, saved_intro_text_groups)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_intro_text_groups) == PT_FIELD_intro_text_groups, "intro_text_groups address");
enum { PT_FIELD_staff_ui_manager_reactions = (offsetof(PTNamedMemory, saved_staff_ui_manager_reactions)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_ui_manager_reactions) == PT_FIELD_staff_ui_manager_reactions,
    "staff_ui_manager_reactions address");
enum { PT_FIELD_staff_ui_manager_behaviors = (offsetof(PTNamedMemory, saved_staff_ui_manager_behaviors)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_ui_manager_behaviors) == PT_FIELD_staff_ui_manager_behaviors,
    "staff_ui_manager_behaviors address");
enum { PT_FIELD_staff_training_courses = (offsetof(PTNamedMemory, saved_staff_training_courses)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_training_courses) == PT_FIELD_staff_training_courses,
    "staff_training_courses address");
enum { PT_FIELD_staff_course_role_labels = (offsetof(PTNamedMemory, saved_staff_course_role_labels)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_course_role_labels) == PT_FIELD_staff_course_role_labels,
    "staff_course_role_labels address");
enum { PT_FIELD_goal_ui_colors = (offsetof(PTNamedMemory, saved_goal_ui_colors)) };
_Static_assert(offsetof(PTNamedMemory, saved_goal_ui_colors) == PT_FIELD_goal_ui_colors, "goal_ui_colors address");
enum { PT_FIELD_curriculum_scores = (offsetof(PTNamedMemory, saved_curriculum_scores)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_curriculum_scores) == PT_FIELD_curriculum_scores, "curriculum_scores address");
enum { PT_FIELD_loaded_awards_bitmap = (offsetof(PTNamedMemory, saved_loaded_awards_bitmap)) };
_Static_assert(offsetof(PTNamedMemory, saved_loaded_awards_bitmap) == PT_FIELD_loaded_awards_bitmap,
    "loaded_awards_bitmap address");
enum { PT_FIELD_character_curriculum_flag = (offsetof(PTNamedMemory, saved_character_curriculum_flag)) };
_Static_assert(offsetof(PTNamedMemory, saved_character_curriculum_flag) == PT_FIELD_character_curriculum_flag,
    "character_curriculum_flag address");
enum { PT_FIELD_curriculum_page = (offsetof(PTNamedMemory, saved_curriculum_page)) };
_Static_assert(offsetof(PTNamedMemory, saved_curriculum_page) == PT_FIELD_curriculum_page, "curriculum_page address");
enum { PT_FIELD_character_mode_names = (offsetof(PTNamedMemory, saved_character_mode_names)) };
_Static_assert(offsetof(PTNamedMemory, saved_character_mode_names) == PT_FIELD_character_mode_names,
    "character_mode_names address");
enum { PT_FIELD_index_to_rank_value = (offsetof(PTNamedMemory, saved_index_to_rank_value)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_index_to_rank_value) == PT_FIELD_index_to_rank_value, "index_to_rank_value address");
enum { PT_FIELD_bitmap_to_show_for_goal = (offsetof(PTNamedMemory, saved_bitmap_to_show_for_goal)) };
_Static_assert(offsetof(PTNamedMemory, saved_bitmap_to_show_for_goal) == PT_FIELD_bitmap_to_show_for_goal,
    "bitmap_to_show_for_goal address");
enum { PT_FIELD_goal_bitmap_flags = (offsetof(PTNamedMemory, saved_goal_bitmap_flags)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_goal_bitmap_flags) == PT_FIELD_goal_bitmap_flags, "goal_bitmap_flags address");
enum { PT_FIELD_contract_insolvency_text = (offsetof(PTNamedMemory, saved_contract_insolvency_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_contract_insolvency_text) == PT_FIELD_contract_insolvency_text,
    "contract_insolvency_text address");
enum { PT_FIELD_depot_capacity_exhausted_text = (offsetof(PTNamedMemory, saved_depot_capacity_exhausted_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_depot_capacity_exhausted_text) == PT_FIELD_depot_capacity_exhausted_text,
    "depot_capacity_exhausted_text address");
enum { PT_FIELD_empty_translation = (offsetof(PTNamedMemory, saved_empty_translation)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_empty_translation) == PT_FIELD_empty_translation, "empty_translation address");
enum { PT_FIELD_translation_error_text = (offsetof(PTNamedMemory, saved_translation_error_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_translation_error_text) == PT_FIELD_translation_error_text,
    "translation_error_text address");
enum { PT_FIELD_mouse_bitmap_data_1 = (offsetof(PTNamedMemory, saved_mouse_bitmap_data_1)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_mouse_bitmap_data_1) == PT_FIELD_mouse_bitmap_data_1, "mouse_bitmap_data_1 address");
enum { PT_FIELD_mouse_bitmap_data_2 = (offsetof(PTNamedMemory, saved_mouse_bitmap_data_2)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_mouse_bitmap_data_2) == PT_FIELD_mouse_bitmap_data_2, "mouse_bitmap_data_2 address");
enum { PT_FIELD_manager_role_names = (offsetof(PTNamedMemory, saved_manager_role_names)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_manager_role_names) == PT_FIELD_manager_role_names, "manager_role_names address");
enum { PT_FIELD_country_selector_x = (offsetof(PTNamedMemory, saved_country_selector_x)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_country_selector_x) == PT_FIELD_country_selector_x, "country_selector_x address");
enum { PT_FIELD_country_selector_y = (offsetof(PTNamedMemory, saved_country_selector_y)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_country_selector_y) == PT_FIELD_country_selector_y, "country_selector_y address");
enum { PT_FIELD_attack_walk_images = (offsetof(PTNamedMemory, saved_attack_walk_images)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_attack_walk_images) == PT_FIELD_attack_walk_images, "attack_walk_images address");
enum { PT_FIELD_guest_seat_x = (offsetof(PTNamedMemory, saved_guest_seat_x)) };
_Static_assert(offsetof(PTNamedMemory, saved_guest_seat_x) == PT_FIELD_guest_seat_x, "guest_seat_x address");
enum { PT_FIELD_guest_seat_y = (offsetof(PTNamedMemory, saved_guest_seat_y)) };
_Static_assert(offsetof(PTNamedMemory, saved_guest_seat_y) == PT_FIELD_guest_seat_y, "guest_seat_y address");
enum { PT_FIELD_guest_body_x = (offsetof(PTNamedMemory, saved_guest_body_x)) };
_Static_assert(offsetof(PTNamedMemory, saved_guest_body_x) == PT_FIELD_guest_body_x, "guest_body_x address");
enum { PT_FIELD_guest_body_y = (offsetof(PTNamedMemory, saved_guest_body_y)) };
_Static_assert(offsetof(PTNamedMemory, saved_guest_body_y) == PT_FIELD_guest_body_y, "guest_body_y address");
enum { PT_FIELD_guest_pose_x = (offsetof(PTNamedMemory, saved_guest_pose_x)) };
_Static_assert(offsetof(PTNamedMemory, saved_guest_pose_x) == PT_FIELD_guest_pose_x, "guest_pose_x address");
enum { PT_FIELD_guest_pose_y = (offsetof(PTNamedMemory, saved_guest_pose_y)) };
_Static_assert(offsetof(PTNamedMemory, saved_guest_pose_y) == PT_FIELD_guest_pose_y, "guest_pose_y address");
enum { PT_FIELD_attack_collision_x = (offsetof(PTNamedMemory, saved_attack_collision_x)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_attack_collision_x) == PT_FIELD_attack_collision_x, "attack_collision_x address");
enum { PT_FIELD_attack_collision_y = (offsetof(PTNamedMemory, saved_attack_collision_y)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_attack_collision_y) == PT_FIELD_attack_collision_y, "attack_collision_y address");
enum { PT_FIELD_attack_head0_x = (offsetof(PTNamedMemory, saved_attack_head0_x)) };
_Static_assert(offsetof(PTNamedMemory, saved_attack_head0_x) == PT_FIELD_attack_head0_x, "attack_head0_x address");
enum { PT_FIELD_attack_head0_y = (offsetof(PTNamedMemory, saved_attack_head0_y)) };
_Static_assert(offsetof(PTNamedMemory, saved_attack_head0_y) == PT_FIELD_attack_head0_y, "attack_head0_y address");
enum { PT_FIELD_attack_head1_x = (offsetof(PTNamedMemory, saved_attack_head1_x)) };
_Static_assert(offsetof(PTNamedMemory, saved_attack_head1_x) == PT_FIELD_attack_head1_x, "attack_head1_x address");
enum { PT_FIELD_attack_head1_y = (offsetof(PTNamedMemory, saved_attack_head1_y)) };
_Static_assert(offsetof(PTNamedMemory, saved_attack_head1_y) == PT_FIELD_attack_head1_y, "attack_head1_y address");
enum { PT_FIELD_guest_carry_x = (offsetof(PTNamedMemory, saved_guest_carry_x)) };
_Static_assert(offsetof(PTNamedMemory, saved_guest_carry_x) == PT_FIELD_guest_carry_x, "guest_carry_x address");
enum { PT_FIELD_guest_carry_y = (offsetof(PTNamedMemory, saved_guest_carry_y)) };
_Static_assert(offsetof(PTNamedMemory, saved_guest_carry_y) == PT_FIELD_guest_carry_y, "guest_carry_y address");
enum { PT_FIELD_attack_held_weapon_x = (offsetof(PTNamedMemory, saved_attack_held_weapon_x)) };
_Static_assert(offsetof(PTNamedMemory, saved_attack_held_weapon_x) == PT_FIELD_attack_held_weapon_x,
    "attack_held_weapon_x address");
enum { PT_FIELD_attack_held_weapon_y = (offsetof(PTNamedMemory, saved_attack_held_weapon_y)) };
_Static_assert(offsetof(PTNamedMemory, saved_attack_held_weapon_y) == PT_FIELD_attack_held_weapon_y,
    "attack_held_weapon_y address");
enum { PT_FIELD_attack_knife_x = (offsetof(PTNamedMemory, saved_attack_knife_x)) };
_Static_assert(offsetof(PTNamedMemory, saved_attack_knife_x) == PT_FIELD_attack_knife_x, "attack_knife_x address");
enum { PT_FIELD_attack_spray_x = (offsetof(PTNamedMemory, saved_attack_spray_x)) };
_Static_assert(offsetof(PTNamedMemory, saved_attack_spray_x) == PT_FIELD_attack_spray_x, "attack_spray_x address");
enum { PT_FIELD_attack_knife_y = (offsetof(PTNamedMemory, saved_attack_knife_y)) };
_Static_assert(offsetof(PTNamedMemory, saved_attack_knife_y) == PT_FIELD_attack_knife_y, "attack_knife_y address");
enum { PT_FIELD_attack_spray_y = (offsetof(PTNamedMemory, saved_attack_spray_y)) };
_Static_assert(offsetof(PTNamedMemory, saved_attack_spray_y) == PT_FIELD_attack_spray_y, "attack_spray_y address");
enum { PT_FIELD_attack_projectile_x = (offsetof(PTNamedMemory, saved_attack_projectile_x)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_attack_projectile_x) == PT_FIELD_attack_projectile_x, "attack_projectile_x address");
enum { PT_FIELD_attack_projectile_y = (offsetof(PTNamedMemory, saved_attack_projectile_y)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_attack_projectile_y) == PT_FIELD_attack_projectile_y, "attack_projectile_y address");
enum { PT_FIELD_attack_explosion_x = (offsetof(PTNamedMemory, saved_attack_explosion_x)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_attack_explosion_x) == PT_FIELD_attack_explosion_x, "attack_explosion_x address");
enum { PT_FIELD_attack_explosion_y = (offsetof(PTNamedMemory, saved_attack_explosion_y)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_attack_explosion_y) == PT_FIELD_attack_explosion_y, "attack_explosion_y address");
enum { PT_FIELD_g_num_active_pigs = (offsetof(PTNamedMemory, saved_g_num_active_pigs)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_g_num_active_pigs) == PT_FIELD_g_num_active_pigs, "g_num_active_pigs address");
enum { PT_FIELD_rest_path_depth = (offsetof(PTNamedMemory, saved_rest_path_depth)) };
_Static_assert(offsetof(PTNamedMemory, saved_rest_path_depth) == PT_FIELD_rest_path_depth, "rest_path_depth address");
enum { PT_FIELD_guest_idle_script = (offsetof(PTNamedMemory, saved_guest_idle_script)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_guest_idle_script) == PT_FIELD_guest_idle_script, "guest_idle_script address");
enum { PT_FIELD_attack_weapon_reach = (offsetof(PTNamedMemory, saved_attack_weapon_reach)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_attack_weapon_reach) == PT_FIELD_attack_weapon_reach, "attack_weapon_reach address");
enum { PT_FIELD_attack_step_x = (offsetof(PTNamedMemory, saved_attack_step_x)) };
_Static_assert(offsetof(PTNamedMemory, saved_attack_step_x) == PT_FIELD_attack_step_x, "attack_step_x address");
enum { PT_FIELD_attack_step_y = (offsetof(PTNamedMemory, saved_attack_step_y)) };
_Static_assert(offsetof(PTNamedMemory, saved_attack_step_y) == PT_FIELD_attack_step_y, "attack_step_y address");
enum { PT_FIELD_pizza_competition_delay = (offsetof(PTNamedMemory, saved_pizza_competition_delay)) };
_Static_assert(offsetof(PTNamedMemory, saved_pizza_competition_delay) == PT_FIELD_pizza_competition_delay,
    "pizza_competition_delay address");
enum { PT_FIELD_pizza_level = (offsetof(PTNamedMemory, saved_pizza_level)) };
_Static_assert(offsetof(PTNamedMemory, saved_pizza_level) == PT_FIELD_pizza_level, "pizza_level address");
enum { PT_FIELD_default_recipe_names = (offsetof(PTNamedMemory, saved_default_recipe_names)) };
_Static_assert(offsetof(PTNamedMemory, saved_default_recipe_names) == PT_FIELD_default_recipe_names,
    "default_recipe_names address");
enum { PT_FIELD_main_menu_desc = (offsetof(PTNamedMemory, saved_main_menu_desc)) };
_Static_assert(offsetof(PTNamedMemory, saved_main_menu_desc) == PT_FIELD_main_menu_desc, "main_menu_desc address");
enum { PT_FIELD_main_menu_strings = (offsetof(PTNamedMemory, saved_main_menu_strings)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_main_menu_strings) == PT_FIELD_main_menu_strings, "main_menu_strings address");
enum { PT_FIELD_selector_family_titles = (offsetof(PTNamedMemory, saved_selector_family_titles)) };
_Static_assert(offsetof(PTNamedMemory, saved_selector_family_titles) == PT_FIELD_selector_family_titles,
    "selector_family_titles address");
enum { PT_FIELD_state_slot_023 = (offsetof(PTNamedMemory, saved_state_slot_023)) };
_Static_assert(offsetof(PTNamedMemory, saved_state_slot_023) == PT_FIELD_state_slot_023, "state_slot_023 address");
enum { PT_FIELD_last_customer_count_poi = (offsetof(PTNamedMemory, saved_serialized_last_customer_count_poi)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_last_customer_count_poi) == PT_FIELD_last_customer_count_poi,
    "last_customer_count_poi address");
enum { PT_FIELD_curr_map_mode = (offsetof(PTNamedMemory, saved_curr_map_mode)) };
_Static_assert(offsetof(PTNamedMemory, saved_curr_map_mode) == PT_FIELD_curr_map_mode, "curr_map_mode address");
enum { PT_FIELD_selected_poi_attr = (offsetof(PTNamedMemory, saved_selected_poi_attr)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_selected_poi_attr) == PT_FIELD_selected_poi_attr, "selected_poi_attr address");
enum { PT_FIELD_selected_poi_player = (offsetof(PTNamedMemory, saved_selected_poi_player)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_selected_poi_player) == PT_FIELD_selected_poi_player, "selected_poi_player address");
enum { PT_FIELD_map_marker_toggle = (offsetof(PTNamedMemory, saved_map_marker_toggle)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_map_marker_toggle) == PT_FIELD_map_marker_toggle, "map_marker_toggle address");
enum { PT_FIELD_map_tabs_order = (offsetof(PTNamedMemory, saved_map_tabs_order)) };
_Static_assert(offsetof(PTNamedMemory, saved_map_tabs_order) == PT_FIELD_map_tabs_order, "map_tabs_order address");
enum { PT_FIELD_owner_to_color = (offsetof(PTNamedMemory, saved_owner_to_color)) };
_Static_assert(offsetof(PTNamedMemory, saved_owner_to_color) == PT_FIELD_owner_to_color, "owner_to_color address");
enum { PT_FIELD_map_tab_step = (offsetof(PTNamedMemory, saved_map_tab_step)) };
_Static_assert(offsetof(PTNamedMemory, saved_map_tab_step) == PT_FIELD_map_tab_step, "map_tab_step address");
enum { PT_FIELD_map_tab_base = (offsetof(PTNamedMemory, saved_map_tab_base)) };
_Static_assert(offsetof(PTNamedMemory, saved_map_tab_base) == PT_FIELD_map_tab_base, "map_tab_base address");
enum { PT_FIELD_tabs_y_values = (offsetof(PTNamedMemory, saved_tabs_y_values)) };
_Static_assert(offsetof(PTNamedMemory, saved_tabs_y_values) == PT_FIELD_tabs_y_values, "tabs_y_values address");
enum { PT_FIELD_map_tab_labels = (offsetof(PTNamedMemory, saved_map_tab_labels)) };
_Static_assert(offsetof(PTNamedMemory, saved_map_tab_labels) == PT_FIELD_map_tab_labels, "map_tab_labels address");
enum { PT_FIELD_district_map_pattern = (offsetof(PTNamedMemory, saved_district_map_pattern)) };
_Static_assert(offsetof(PTNamedMemory, saved_district_map_pattern) == PT_FIELD_district_map_pattern,
    "district_map_pattern address");
enum { PT_FIELD_terrain_tile_colors = (offsetof(PTNamedMemory, saved_terrain_tile_colors)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_terrain_tile_colors) == PT_FIELD_terrain_tile_colors, "terrain_tile_colors address");
enum { PT_FIELD_season_names = (offsetof(PTNamedMemory, saved_season_names)) };
_Static_assert(offsetof(PTNamedMemory, saved_season_names) == PT_FIELD_season_names, "season_names address");
enum { PT_FIELD_month_to_arstid = (offsetof(PTNamedMemory, saved_month_to_arstid)) };
_Static_assert(offsetof(PTNamedMemory, saved_month_to_arstid) == PT_FIELD_month_to_arstid, "month_to_arstid address");
enum { PT_FIELD_terrain_names = (offsetof(PTNamedMemory, saved_terrain_names)) };
_Static_assert(offsetof(PTNamedMemory, saved_terrain_names) == PT_FIELD_terrain_names, "terrain_names address");
enum { PT_FIELD_map_marker_visible = (offsetof(PTNamedMemory, saved_map_marker_visible)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_map_marker_visible) == PT_FIELD_map_marker_visible, "map_marker_visible address");
enum { PT_FIELD_map_animation_palette_ticks = (offsetof(PTNamedMemory, saved_map_animation_palette_ticks)) };
_Static_assert(offsetof(PTNamedMemory, saved_map_animation_palette_ticks) == PT_FIELD_map_animation_palette_ticks,
    "map_animation_palette_ticks address");
enum { PT_FIELD_animated_production_x = (offsetof(PTNamedMemory, saved_animated_production_x)) };
_Static_assert(offsetof(PTNamedMemory, saved_animated_production_x) == PT_FIELD_animated_production_x,
    "animated_production_x address");
enum { PT_FIELD_animated_production_y = (offsetof(PTNamedMemory, saved_animated_production_y)) };
_Static_assert(offsetof(PTNamedMemory, saved_animated_production_y) == PT_FIELD_animated_production_y,
    "animated_production_y address");
enum { PT_FIELD_palette_in_callbackground = (offsetof(PTNamedMemory, saved_palette_in_callbackground)) };
_Static_assert(offsetof(PTNamedMemory, saved_palette_in_callbackground) == PT_FIELD_palette_in_callbackground,
    "palette_in_callbackground address");
enum { PT_FIELD_last_loaded_bob_typ = (offsetof(PTNamedMemory, saved_last_loaded_bob_typ)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_last_loaded_bob_typ) == PT_FIELD_last_loaded_bob_typ, "last_loaded_bob_typ address");
enum { PT_FIELD_phone_face_five_x = (offsetof(PTNamedMemory, saved_phone_face_five_x)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_phone_face_five_x) == PT_FIELD_phone_face_five_x, "phone_face_five_x address");
enum { PT_FIELD_phone_face_five_y = (offsetof(PTNamedMemory, saved_phone_face_five_y)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_phone_face_five_y) == PT_FIELD_phone_face_five_y, "phone_face_five_y address");
enum { PT_FIELD_phone_face_y = (offsetof(PTNamedMemory, saved_phone_face_y)) };
_Static_assert(offsetof(PTNamedMemory, saved_phone_face_y) == PT_FIELD_phone_face_y, "phone_face_y address");
enum { PT_FIELD_phone_background_face_x = (offsetof(PTNamedMemory, saved_phone_background_face_x)) };
_Static_assert(offsetof(PTNamedMemory, saved_phone_background_face_x) == PT_FIELD_phone_background_face_x,
    "phone_background_face_x address");
enum { PT_FIELD_phone_background_face_y = (offsetof(PTNamedMemory, saved_phone_background_face_y)) };
_Static_assert(offsetof(PTNamedMemory, saved_phone_background_face_y) == PT_FIELD_phone_background_face_y,
    "phone_background_face_y address");
enum { PT_FIELD_terminal_category_names = (offsetof(PTNamedMemory, saved_terminal_category_names)) };
_Static_assert(offsetof(PTNamedMemory, saved_terminal_category_names) == PT_FIELD_terminal_category_names,
    "terminal_category_names address");
enum { PT_FIELD_terminal_dial_messages = (offsetof(PTNamedMemory, saved_terminal_dial_messages)) };
_Static_assert(offsetof(PTNamedMemory, saved_terminal_dial_messages) == PT_FIELD_terminal_dial_messages,
    "terminal_dial_messages address");
enum { PT_FIELD_terminal_waiting_message = (offsetof(PTNamedMemory, saved_terminal_waiting_message)) };
_Static_assert(offsetof(PTNamedMemory, saved_terminal_waiting_message) == PT_FIELD_terminal_waiting_message,
    "terminal_waiting_message address");
enum { PT_FIELD_dial_number_position = (offsetof(PTNamedMemory, saved_dial_number_position)) };
_Static_assert(offsetof(PTNamedMemory, saved_dial_number_position) == PT_FIELD_dial_number_position,
    "dial_number_position address");
enum { PT_FIELD_insurance_claim_reason_by_type = (offsetof(PTNamedMemory, saved_insurance_claim_reason_by_type)) };
_Static_assert(offsetof(PTNamedMemory, saved_insurance_claim_reason_by_type) == PT_FIELD_insurance_claim_reason_by_type,
    "insurance_claim_reason_by_type address");
enum { PT_FIELD_townhall_competition_messages = (offsetof(PTNamedMemory, saved_townhall_competition_messages)) };
_Static_assert(offsetof(PTNamedMemory, saved_townhall_competition_messages) == PT_FIELD_townhall_competition_messages,
    "townhall_competition_messages address");
enum { PT_FIELD_mission_item_pronouns = (offsetof(PTNamedMemory, saved_mission_item_pronouns)) };
_Static_assert(offsetof(PTNamedMemory, saved_mission_item_pronouns) == PT_FIELD_mission_item_pronouns,
    "mission_item_pronouns address");
enum { PT_FIELD_mission_pronouns = (offsetof(PTNamedMemory, saved_mission_pronouns)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_mission_pronouns) == PT_FIELD_mission_pronouns, "mission_pronouns address");
enum { PT_FIELD_mission_completion_goals = (offsetof(PTNamedMemory, saved_mission_completion_goals)) };
_Static_assert(offsetof(PTNamedMemory, saved_mission_completion_goals) == PT_FIELD_mission_completion_goals,
    "mission_completion_goals address");
enum { PT_FIELD_mission_date_buffer_index = (offsetof(PTNamedMemory, saved_mission_date_buffer_index)) };
_Static_assert(offsetof(PTNamedMemory, saved_mission_date_buffer_index) == PT_FIELD_mission_date_buffer_index,
    "mission_date_buffer_index address");
enum { PT_FIELD_mission_place_buffer_index = (offsetof(PTNamedMemory, saved_mission_place_buffer_index)) };
_Static_assert(offsetof(PTNamedMemory, saved_mission_place_buffer_index) == PT_FIELD_mission_place_buffer_index,
    "mission_place_buffer_index address");
enum { PT_FIELD_arms_dealer_phone_nr = (offsetof(PTNamedMemory, saved_arms_dealer_phone_nr)) };
_Static_assert(offsetof(PTNamedMemory, saved_arms_dealer_phone_nr) == PT_FIELD_arms_dealer_phone_nr,
    "arms_dealer_phone_nr address");
enum { PT_FIELD_last_input_tick = (offsetof(PTNamedMemory, saved_last_input_tick)) };
_Static_assert(offsetof(PTNamedMemory, saved_last_input_tick) == PT_FIELD_last_input_tick, "last_input_tick address");
enum { PT_FIELD_last_second_tick = (offsetof(PTNamedMemory, saved_last_second_tick)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_last_second_tick) == PT_FIELD_last_second_tick, "last_second_tick address");
enum { PT_FIELD_secondary_filename = (offsetof(PTNamedMemory, saved_serialized_secondary_filename)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_secondary_filename) == PT_FIELD_secondary_filename,
    "secondary_filename address");
enum { PT_FIELD_info_location_names = (offsetof(PTNamedMemory, saved_info_location_names)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_info_location_names) == PT_FIELD_info_location_names, "info_location_names address");
enum { PT_FIELD_current_button_hint = (offsetof(PTNamedMemory, saved_serialized_current_button_hint)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_current_button_hint) == PT_FIELD_current_button_hint,
    "current_button_hint address");
enum { PT_FIELD_input_timer_phase = (offsetof(PTNamedMemory, saved_input_timer_phase)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_input_timer_phase) == PT_FIELD_input_timer_phase, "input_timer_phase address");
enum { PT_FIELD_market_zone_names = (offsetof(PTNamedMemory, saved_market_zone_names)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_market_zone_names) == PT_FIELD_market_zone_names, "market_zone_names address");
enum { PT_FIELD_market_bar_colors = (offsetof(PTNamedMemory, saved_market_bar_colors)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_market_bar_colors) == PT_FIELD_market_bar_colors, "market_bar_colors address");
enum { PT_FIELD_guest_food_offset_x = (offsetof(PTNamedMemory, saved_guest_food_offset_x)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_guest_food_offset_x) == PT_FIELD_guest_food_offset_x, "guest_food_offset_x address");
enum { PT_FIELD_guest_food_offset_y = (offsetof(PTNamedMemory, saved_guest_food_offset_y)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_guest_food_offset_y) == PT_FIELD_guest_food_offset_y, "guest_food_offset_y address");
enum { PT_FIELD_guest_lower_offset_x = (offsetof(PTNamedMemory, saved_guest_lower_offset_x)) };
_Static_assert(offsetof(PTNamedMemory, saved_guest_lower_offset_x) == PT_FIELD_guest_lower_offset_x,
    "guest_lower_offset_x address");
enum { PT_FIELD_guest_lower_offset_y = (offsetof(PTNamedMemory, saved_guest_lower_offset_y)) };
_Static_assert(offsetof(PTNamedMemory, saved_guest_lower_offset_y) == PT_FIELD_guest_lower_offset_y,
    "guest_lower_offset_y address");
enum { PT_FIELD_guest_upper_offset_x = (offsetof(PTNamedMemory, saved_guest_upper_offset_x)) };
_Static_assert(offsetof(PTNamedMemory, saved_guest_upper_offset_x) == PT_FIELD_guest_upper_offset_x,
    "guest_upper_offset_x address");
enum { PT_FIELD_guest_upper_offset_y = (offsetof(PTNamedMemory, saved_guest_upper_offset_y)) };
_Static_assert(offsetof(PTNamedMemory, saved_guest_upper_offset_y) == PT_FIELD_guest_upper_offset_y,
    "guest_upper_offset_y address");
enum { PT_FIELD_placement_contact_count = (offsetof(PTNamedMemory, saved_placement_contact_count)) };
_Static_assert(offsetof(PTNamedMemory, saved_placement_contact_count) == PT_FIELD_placement_contact_count,
    "placement_contact_count address");
enum { PT_FIELD_furniture_refresh_depth = (offsetof(PTNamedMemory, saved_furniture_refresh_depth)) };
_Static_assert(offsetof(PTNamedMemory, saved_furniture_refresh_depth) == PT_FIELD_furniture_refresh_depth,
    "furniture_refresh_depth address");
enum { PT_FIELD_furniture_category_names = (offsetof(PTNamedMemory, saved_furniture_category_names)) };
_Static_assert(offsetof(PTNamedMemory, saved_furniture_category_names) == PT_FIELD_furniture_category_names,
    "furniture_category_names address");
enum { PT_FIELD_restaurant_media_labels = (offsetof(PTNamedMemory, saved_restaurant_media_labels)) };
_Static_assert(offsetof(PTNamedMemory, saved_restaurant_media_labels) == PT_FIELD_restaurant_media_labels,
    "restaurant_media_labels address");
enum { PT_FIELD_competition_sound_pending = (offsetof(PTNamedMemory, saved_competition_sound_pending)) };
_Static_assert(offsetof(PTNamedMemory, saved_competition_sound_pending) == PT_FIELD_competition_sound_pending,
    "competition_sound_pending address");
enum { PT_FIELD_sound_config_flag = (offsetof(PTNamedMemory, saved_sound_config_flag)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_sound_config_flag) == PT_FIELD_sound_config_flag, "sound_config_flag address");
enum { PT_FIELD_sound_var_3 = (offsetof(PTNamedMemory, saved_sound_var_3)) };
_Static_assert(offsetof(PTNamedMemory, saved_sound_var_3) == PT_FIELD_sound_var_3, "sound_var_3 address");
enum { PT_FIELD_sound_buffer = (offsetof(PTNamedMemory, saved_serialized_sound_buffer)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_sound_buffer) == PT_FIELD_sound_buffer, "sound_buffer address");
enum { PT_FIELD_current_music_index = (offsetof(PTNamedMemory, saved_current_music_index)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_current_music_index) == PT_FIELD_current_music_index, "current_music_index address");
enum { PT_FIELD_state_slot_011 = (offsetof(PTNamedMemory, saved_state_slot_011)) };
_Static_assert(offsetof(PTNamedMemory, saved_state_slot_011) == PT_FIELD_state_slot_011, "state_slot_011 address");
enum { PT_FIELD_competition_sound_id = (offsetof(PTNamedMemory, saved_competition_sound_id)) };
_Static_assert(offsetof(PTNamedMemory, saved_competition_sound_id) == PT_FIELD_competition_sound_id,
    "competition_sound_id address");
enum { PT_FIELD_selected_pizza_load_idx = (offsetof(PTNamedMemory, saved_selected_pizza_load_idx)) };
_Static_assert(offsetof(PTNamedMemory, saved_selected_pizza_load_idx) == PT_FIELD_selected_pizza_load_idx,
    "selected_pizza_load_idx address");
enum { PT_FIELD_finance_stats_categories = (offsetof(PTNamedMemory, saved_finance_stats_categories)) };
_Static_assert(offsetof(PTNamedMemory, saved_finance_stats_categories) == PT_FIELD_finance_stats_categories,
    "finance_stats_categories address");
enum { PT_FIELD_daily_balance_flag = (offsetof(PTNamedMemory, saved_daily_balance_flag)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_daily_balance_flag) == PT_FIELD_daily_balance_flag, "daily_balance_flag address");
enum { PT_FIELD_monthly_receipt_labels = (offsetof(PTNamedMemory, saved_monthly_receipt_labels)) };
_Static_assert(offsetof(PTNamedMemory, saved_monthly_receipt_labels) == PT_FIELD_monthly_receipt_labels,
    "monthly_receipt_labels address");
enum { PT_FIELD_monthly_expense_labels = (offsetof(PTNamedMemory, saved_monthly_expense_labels)) };
_Static_assert(offsetof(PTNamedMemory, saved_monthly_expense_labels) == PT_FIELD_monthly_expense_labels,
    "monthly_expense_labels address");
enum { PT_FIELD_insurance_category_labels = (offsetof(PTNamedMemory, saved_insurance_category_labels)) };
_Static_assert(offsetof(PTNamedMemory, saved_insurance_category_labels) == PT_FIELD_insurance_category_labels,
    "insurance_category_labels address");
enum { PT_FIELD_advert_tv_frames = (offsetof(PTNamedMemory, saved_advert_tv_frames)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_advert_tv_frames) == PT_FIELD_advert_tv_frames, "advert_tv_frames address");
enum { PT_FIELD_newspaper_article_backgrounds = (offsetof(PTNamedMemory, saved_newspaper_article_backgrounds)) };
_Static_assert(offsetof(PTNamedMemory, saved_newspaper_article_backgrounds) == PT_FIELD_newspaper_article_backgrounds,
    "newspaper_article_backgrounds address");
enum { PT_FIELD_had_holiday = (offsetof(PTNamedMemory, saved_had_holiday)) };
_Static_assert(offsetof(PTNamedMemory, saved_had_holiday) == PT_FIELD_had_holiday, "had_holiday address");
enum { PT_FIELD_next_event_to_do = (offsetof(PTNamedMemory, saved_next_event_to_do)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_next_event_to_do) == PT_FIELD_next_event_to_do, "next_event_to_do address");
enum { PT_FIELD_suppliers_daily_loop_index = (offsetof(PTNamedMemory, saved_suppliers_daily_loop_index)) };
_Static_assert(offsetof(PTNamedMemory, saved_suppliers_daily_loop_index) == PT_FIELD_suppliers_daily_loop_index,
    "suppliers_daily_loop_index address");
enum { PT_FIELD_month_names = (offsetof(PTNamedMemory, saved_month_names)) };
_Static_assert(offsetof(PTNamedMemory, saved_month_names) == PT_FIELD_month_names, "month_names address");
enum { PT_FIELD_ordinal_suffixes = (offsetof(PTNamedMemory, saved_ordinal_suffixes)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_ordinal_suffixes) == PT_FIELD_ordinal_suffixes, "ordinal_suffixes address");
enum { PT_FIELD_pt_random_state = (offsetof(PTNamedMemory, saved_pt_random_state)) };
_Static_assert(offsetof(PTNamedMemory, saved_pt_random_state) == PT_FIELD_pt_random_state, "pt_random_state address");
enum { PT_FIELD_guest_heap_regions = (offsetof(PTNamedMemory, saved_serialized_guest_heap_regions)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_guest_heap_regions) == PT_FIELD_guest_heap_regions,
    "guest_heap_regions address");
enum { PT_FIELD_guest_heap_rover = (offsetof(PTNamedMemory, saved_serialized_guest_heap_rover)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_guest_heap_rover) == PT_FIELD_guest_heap_rover,
    "guest_heap_rover address");
enum { PT_FIELD_guest_heap_largest_before_rover = (offsetof(PTNamedMemory, saved_guest_heap_largest_before_rover)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_guest_heap_largest_before_rover) == PT_FIELD_guest_heap_largest_before_rover,
    "guest_heap_largest_before_rover address");
enum { PT_FIELD_guest_heap_break = (offsetof(PTNamedMemory, saved_guest_heap_break)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_guest_heap_break) == PT_FIELD_guest_heap_break, "guest_heap_break address");
enum { PT_FIELD_guest_stack_lower_bound = (offsetof(PTNamedMemory, saved_guest_stack_lower_bound)) };
_Static_assert(offsetof(PTNamedMemory, saved_guest_stack_lower_bound) == PT_FIELD_guest_stack_lower_bound,
    "guest_stack_lower_bound address");
enum { PT_FIELD_guest_console_cached_character = (offsetof(PTNamedMemory, saved_guest_console_cached_character)) };
_Static_assert(offsetof(PTNamedMemory, saved_guest_console_cached_character) == PT_FIELD_guest_console_cached_character,
    "guest_console_cached_character address");
enum { PT_FIELD_state_slot_012 = (offsetof(PTNamedMemory, saved_state_slot_012)) };
_Static_assert(offsetof(PTNamedMemory, saved_state_slot_012) == PT_FIELD_state_slot_012, "state_slot_012 address");
enum { PT_FIELD_state_slot_013 = (offsetof(PTNamedMemory, saved_state_slot_013)) };
_Static_assert(offsetof(PTNamedMemory, saved_state_slot_013) == PT_FIELD_state_slot_013, "state_slot_013 address");
enum { PT_FIELD_guest_dos_major = (offsetof(PTNamedMemory, saved_guest_dos_major)) };
_Static_assert(offsetof(PTNamedMemory, saved_guest_dos_major) == PT_FIELD_guest_dos_major, "guest_dos_major address");
enum { PT_FIELD_guest_file_streams = (offsetof(PTNamedMemory, saved_serialized_guest_file_streams)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_guest_file_streams) == PT_FIELD_guest_file_streams,
    "guest_file_streams address");
enum { PT_FIELD_guest_default_open_mode = (offsetof(PTNamedMemory, saved_guest_default_open_mode)) };
_Static_assert(offsetof(PTNamedMemory, saved_guest_default_open_mode) == PT_FIELD_guest_default_open_mode,
    "guest_default_open_mode address");
enum { PT_FIELD_guest_dos_error_errno = (offsetof(PTNamedMemory, saved_guest_dos_error_errno)) };
_Static_assert(offsetof(PTNamedMemory, saved_guest_dos_error_errno) == PT_FIELD_guest_dos_error_errno,
    "guest_dos_error_errno address");
enum { PT_FIELD_guest_permission_mask = (offsetof(PTNamedMemory, saved_guest_permission_mask)) };
_Static_assert(offsetof(PTNamedMemory, saved_guest_permission_mask) == PT_FIELD_guest_permission_mask,
    "guest_permission_mask address");
enum { PT_FIELD_guest_io_mode_capacity = (offsetof(PTNamedMemory, saved_guest_io_mode_capacity)) };
_Static_assert(offsetof(PTNamedMemory, saved_guest_io_mode_capacity) == PT_FIELD_guest_io_mode_capacity,
    "guest_io_mode_capacity address");
enum { PT_FIELD_guest_io_modes = (offsetof(PTNamedMemory, saved_guest_io_modes)) };
_Static_assert(offsetof(PTNamedMemory, saved_guest_io_modes) == PT_FIELD_guest_io_modes, "guest_io_modes address");
enum { PT_FIELD_guest_io_mode_table = (offsetof(PTNamedMemory, saved_serialized_guest_io_mode_table)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_guest_io_mode_table) == PT_FIELD_guest_io_mode_table,
    "guest_io_mode_table address");
enum { PT_FIELD_timezone_start_rule = (offsetof(PTNamedMemory, saved_timezone_start_rule)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_timezone_start_rule) == PT_FIELD_timezone_start_rule, "timezone_start_rule address");
enum { PT_FIELD_timezone_end_rule = (offsetof(PTNamedMemory, saved_timezone_end_rule)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_timezone_end_rule) == PT_FIELD_timezone_end_rule, "timezone_end_rule address");
enum { PT_FIELD_timezone_seconds = (offsetof(PTNamedMemory, saved_timezone_seconds)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_timezone_seconds) == PT_FIELD_timezone_seconds, "timezone_seconds address");
enum { PT_FIELD_timezone_dst_seconds = (offsetof(PTNamedMemory, saved_timezone_dst_seconds)) };
_Static_assert(offsetof(PTNamedMemory, saved_timezone_dst_seconds) == PT_FIELD_timezone_dst_seconds,
    "timezone_dst_seconds address");
enum { PT_FIELD_timezone_has_dst = (offsetof(PTNamedMemory, saved_timezone_has_dst)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_timezone_has_dst) == PT_FIELD_timezone_has_dst, "timezone_has_dst address");
enum { PT_FIELD_timezone_standard_name = (offsetof(PTNamedMemory, saved_timezone_standard_name)) };
_Static_assert(offsetof(PTNamedMemory, saved_timezone_standard_name) == PT_FIELD_timezone_standard_name,
    "timezone_standard_name address");
enum { PT_FIELD_timezone_dst_name = (offsetof(PTNamedMemory, saved_timezone_dst_name)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_timezone_dst_name) == PT_FIELD_timezone_dst_name, "timezone_dst_name address");
enum { PT_FIELD_guest_heap_grow_enabled = (offsetof(PTNamedMemory, saved_guest_heap_grow_enabled)) };
_Static_assert(offsetof(PTNamedMemory, saved_guest_heap_grow_enabled) == PT_FIELD_guest_heap_grow_enabled,
    "guest_heap_grow_enabled address");
enum { PT_FIELD_guest_heap_growth_minimum = (offsetof(PTNamedMemory, saved_guest_heap_growth_minimum)) };
_Static_assert(offsetof(PTNamedMemory, saved_guest_heap_growth_minimum) == PT_FIELD_guest_heap_growth_minimum,
    "guest_heap_growth_minimum address");
enum { PT_FIELD_guest_environment = (offsetof(PTNamedMemory, saved_serialized_guest_environment)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_guest_environment) == PT_FIELD_guest_environment,
    "guest_environment address");
enum { PT_FIELD_market_property_title = (offsetof(PTNamedMemory, saved_market_property_title)) };
_Static_assert(offsetof(PTNamedMemory, saved_market_property_title) == PT_FIELD_market_property_title,
    "market_property_title address");
enum { PT_FIELD_graphics_palette_filename = (offsetof(PTNamedMemory, saved_graphics_palette_filename)) };
_Static_assert(offsetof(PTNamedMemory, saved_graphics_palette_filename) == PT_FIELD_graphics_palette_filename,
    "graphics_palette_filename address");
enum { PT_FIELD_intro_presents = (offsetof(PTNamedMemory, saved_intro_presents)) };
_Static_assert(offsetof(PTNamedMemory, saved_intro_presents) == PT_FIELD_intro_presents, "intro_presents address");
enum { PT_FIELD_intro_logo_filename = (offsetof(PTNamedMemory, saved_intro_logo_filename)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_intro_logo_filename) == PT_FIELD_intro_logo_filename, "intro_logo_filename address");
enum { PT_FIELD_intro_cyber_filename = (offsetof(PTNamedMemory, saved_intro_cyber_filename)) };
_Static_assert(offsetof(PTNamedMemory, saved_intro_cyber_filename) == PT_FIELD_intro_cyber_filename,
    "intro_cyber_filename address");
enum { PT_FIELD_intro_version = (offsetof(PTNamedMemory, saved_intro_version)) };
_Static_assert(offsetof(PTNamedMemory, saved_intro_version) == PT_FIELD_intro_version, "intro_version address");
enum { PT_FIELD_intro_title1_filename = (offsetof(PTNamedMemory, saved_intro_title1_filename)) };
_Static_assert(offsetof(PTNamedMemory, saved_intro_title1_filename) == PT_FIELD_intro_title1_filename,
    "intro_title1_filename address");
enum { PT_FIELD_intro_title2_filename = (offsetof(PTNamedMemory, saved_intro_title2_filename)) };
_Static_assert(offsetof(PTNamedMemory, saved_intro_title2_filename) == PT_FIELD_intro_title2_filename,
    "intro_title2_filename address");
enum { PT_FIELD_startup_permap_filename = (offsetof(PTNamedMemory, saved_startup_permap_filename)) };
_Static_assert(offsetof(PTNamedMemory, saved_startup_permap_filename) == PT_FIELD_startup_permap_filename,
    "startup_permap_filename address");
enum { PT_FIELD_startup_icons_filename = (offsetof(PTNamedMemory, saved_startup_icons_filename)) };
_Static_assert(offsetof(PTNamedMemory, saved_startup_icons_filename) == PT_FIELD_startup_icons_filename,
    "startup_icons_filename address");
enum { PT_FIELD_startup_k0_filename = (offsetof(PTNamedMemory, saved_startup_k0_filename)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_startup_k0_filename) == PT_FIELD_startup_k0_filename, "startup_k0_filename address");
enum { PT_FIELD_startup_med1_filename = (offsetof(PTNamedMemory, saved_startup_med1_filename)) };
_Static_assert(offsetof(PTNamedMemory, saved_startup_med1_filename) == PT_FIELD_startup_med1_filename,
    "startup_med1_filename address");
enum { PT_FIELD_main_sound_config_filename = (offsetof(PTNamedMemory, saved_main_sound_config_filename)) };
_Static_assert(offsetof(PTNamedMemory, saved_main_sound_config_filename) == PT_FIELD_main_sound_config_filename,
    "main_sound_config_filename address");
enum { PT_FIELD_main_prefs_filename = (offsetof(PTNamedMemory, saved_main_prefs_filename)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_main_prefs_filename) == PT_FIELD_main_prefs_filename, "main_prefs_filename address");
enum {
  PT_FIELD_quickstart_restaurant_file_format = (offsetof(PTNamedMemory, saved_quickstart_restaurant_file_format))
};
_Static_assert(
    offsetof(PTNamedMemory, saved_quickstart_restaurant_file_format) == PT_FIELD_quickstart_restaurant_file_format,
    "quickstart_restaurant_file_format address");
enum { PT_FIELD_town_filename_format = (offsetof(PTNamedMemory, saved_town_filename_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_town_filename_format) == PT_FIELD_town_filename_format,
    "town_filename_format address");
enum { PT_FIELD_city_filename_format = (offsetof(PTNamedMemory, saved_city_filename_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_city_filename_format) == PT_FIELD_city_filename_format,
    "city_filename_format address");
enum { PT_FIELD_recipe_allocation_failure_text = (offsetof(PTNamedMemory, saved_recipe_allocation_failure_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_recipe_allocation_failure_text) == PT_FIELD_recipe_allocation_failure_text,
    "recipe_allocation_failure_text address");
enum { PT_FIELD_standard_recipe_file_format = (offsetof(PTNamedMemory, saved_standard_recipe_file_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_standard_recipe_file_format) == PT_FIELD_standard_recipe_file_format,
    "standard_recipe_file_format address");
enum { PT_FIELD_map_filename_format = (offsetof(PTNamedMemory, saved_map_filename_format)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_map_filename_format) == PT_FIELD_map_filename_format, "map_filename_format address");
enum { PT_FIELD_bez_filename_format = (offsetof(PTNamedMemory, saved_bez_filename_format)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_bez_filename_format) == PT_FIELD_bez_filename_format, "bez_filename_format address");
enum { PT_FIELD_restaurant_not_found_text = (offsetof(PTNamedMemory, saved_restaurant_not_found_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_restaurant_not_found_text) == PT_FIELD_restaurant_not_found_text,
    "restaurant_not_found_text address");
enum { PT_FIELD_savegame_filename_format = (offsetof(PTNamedMemory, saved_savegame_filename_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_savegame_filename_format) == PT_FIELD_savegame_filename_format,
    "savegame_filename_format address");
enum { PT_FIELD_savegame_load_filename_format = (offsetof(PTNamedMemory, saved_savegame_load_filename_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_savegame_load_filename_format) == PT_FIELD_savegame_load_filename_format,
    "savegame_load_filename_format address");
enum { PT_FIELD_staff_disaster_theft_text = (offsetof(PTNamedMemory, saved_staff_disaster_theft_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_disaster_theft_text) == PT_FIELD_staff_disaster_theft_text,
    "staff_disaster_theft_text address");
enum { PT_FIELD_staff_disaster_car_format = (offsetof(PTNamedMemory, saved_staff_disaster_car_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_disaster_car_format) == PT_FIELD_staff_disaster_car_format,
    "staff_disaster_car_format address");
enum { PT_FIELD_staff_disaster_raise_factor = (offsetof(PTNamedMemory, saved_staff_disaster_raise_factor)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_disaster_raise_factor) == PT_FIELD_staff_disaster_raise_factor,
    "staff_disaster_raise_factor address");
enum { PT_FIELD_staff_ui_years_format = (offsetof(PTNamedMemory, saved_staff_ui_years_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_ui_years_format) == PT_FIELD_staff_ui_years_format,
    "staff_ui_years_format address");
enum { PT_FIELD_staff_ui_payment = (offsetof(PTNamedMemory, saved_staff_ui_payment)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_staff_ui_payment) == PT_FIELD_staff_ui_payment, "staff_ui_payment address");
enum { PT_FIELD_staff_ui_pizzas_day = (offsetof(PTNamedMemory, saved_staff_ui_pizzas_day)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_staff_ui_pizzas_day) == PT_FIELD_staff_ui_pizzas_day, "staff_ui_pizzas_day address");
enum { PT_FIELD_staff_ui_cooking = (offsetof(PTNamedMemory, saved_staff_ui_cooking)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_staff_ui_cooking) == PT_FIELD_staff_ui_cooking, "staff_ui_cooking address");
enum { PT_FIELD_staff_ui_serving = (offsetof(PTNamedMemory, saved_staff_ui_serving)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_staff_ui_serving) == PT_FIELD_staff_ui_serving, "staff_ui_serving address");
enum { PT_FIELD_staff_ui_management = (offsetof(PTNamedMemory, saved_staff_ui_management)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_staff_ui_management) == PT_FIELD_staff_ui_management, "staff_ui_management address");
enum { PT_FIELD_staff_ui_experience = (offsetof(PTNamedMemory, saved_staff_ui_experience)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_staff_ui_experience) == PT_FIELD_staff_ui_experience, "staff_ui_experience address");
enum { PT_FIELD_staff_ui_intelligence = (offsetof(PTNamedMemory, saved_staff_ui_intelligence)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_ui_intelligence) == PT_FIELD_staff_ui_intelligence,
    "staff_ui_intelligence address");
enum { PT_FIELD_staff_ui_trustworthiness = (offsetof(PTNamedMemory, saved_staff_ui_trustworthiness)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_ui_trustworthiness) == PT_FIELD_staff_ui_trustworthiness,
    "staff_ui_trustworthiness address");
enum { PT_FIELD_staff_ui_health = (offsetof(PTNamedMemory, saved_staff_ui_health)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_ui_health) == PT_FIELD_staff_ui_health, "staff_ui_health address");
enum { PT_FIELD_staff_ui_motivation = (offsetof(PTNamedMemory, saved_staff_ui_motivation)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_staff_ui_motivation) == PT_FIELD_staff_ui_motivation, "staff_ui_motivation address");
enum { PT_FIELD_staff_application_title = (offsetof(PTNamedMemory, saved_staff_application_title)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_application_title) == PT_FIELD_staff_application_title,
    "staff_application_title address");
enum { PT_FIELD_staff_course_title = (offsetof(PTNamedMemory, saved_staff_course_title)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_staff_course_title) == PT_FIELD_staff_course_title, "staff_course_title address");
enum { PT_FIELD_staff_course_name_title = (offsetof(PTNamedMemory, saved_staff_course_name_title)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_course_name_title) == PT_FIELD_staff_course_name_title,
    "staff_course_name_title address");
enum { PT_FIELD_staff_course_scores_title = (offsetof(PTNamedMemory, saved_staff_course_scores_title)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_course_scores_title) == PT_FIELD_staff_course_scores_title,
    "staff_course_scores_title address");
enum { PT_FIELD_staff_course_costs_title = (offsetof(PTNamedMemory, saved_staff_course_costs_title)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_course_costs_title) == PT_FIELD_staff_course_costs_title,
    "staff_course_costs_title address");
enum { PT_FIELD_staff_course_days_title = (offsetof(PTNamedMemory, saved_staff_course_days_title)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_course_days_title) == PT_FIELD_staff_course_days_title,
    "staff_course_days_title address");
enum { PT_FIELD_staff_course_confirm_prompt = (offsetof(PTNamedMemory, saved_staff_course_confirm_prompt)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_course_confirm_prompt) == PT_FIELD_staff_course_confirm_prompt,
    "staff_course_confirm_prompt address");
enum { PT_FIELD_staff_ui_no_manager = (offsetof(PTNamedMemory, saved_staff_ui_no_manager)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_staff_ui_no_manager) == PT_FIELD_staff_ui_no_manager, "staff_ui_no_manager address");
enum { PT_FIELD_staff_ui_authority = (offsetof(PTNamedMemory, saved_staff_ui_authority)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_staff_ui_authority) == PT_FIELD_staff_ui_authority, "staff_ui_authority address");
enum { PT_FIELD_staff_ui_reactions = (offsetof(PTNamedMemory, saved_staff_ui_reactions)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_staff_ui_reactions) == PT_FIELD_staff_ui_reactions, "staff_ui_reactions address");
enum { PT_FIELD_staff_ui_behavior_towards_staff = (offsetof(PTNamedMemory, saved_staff_ui_behavior_towards_staff)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_staff_ui_behavior_towards_staff) == PT_FIELD_staff_ui_behavior_towards_staff,
    "staff_ui_behavior_towards_staff address");
enum { PT_FIELD_staff_ui_shopping_if_needed = (offsetof(PTNamedMemory, saved_staff_ui_shopping_if_needed)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_ui_shopping_if_needed) == PT_FIELD_staff_ui_shopping_if_needed,
    "staff_ui_shopping_if_needed address");
enum { PT_FIELD_staff_ui_change_prices = (offsetof(PTNamedMemory, saved_staff_ui_change_prices)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_ui_change_prices) == PT_FIELD_staff_ui_change_prices,
    "staff_ui_change_prices address");
enum { PT_FIELD_staff_ui_overdraw_account = (offsetof(PTNamedMemory, saved_staff_ui_overdraw_account)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_ui_overdraw_account) == PT_FIELD_staff_ui_overdraw_account,
    "staff_ui_overdraw_account address");
enum { PT_FIELD_staff_ui_turnover_decrease = (offsetof(PTNamedMemory, saved_staff_ui_turnover_decrease)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_ui_turnover_decrease) == PT_FIELD_staff_ui_turnover_decrease,
    "staff_ui_turnover_decrease address");
enum { PT_FIELD_staff_ui_turnover_increase = (offsetof(PTNamedMemory, saved_staff_ui_turnover_increase)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_ui_turnover_increase) == PT_FIELD_staff_ui_turnover_increase,
    "staff_ui_turnover_increase address");
enum { PT_FIELD_staff_ui_supply_shortage = (offsetof(PTNamedMemory, saved_staff_ui_supply_shortage)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_ui_supply_shortage) == PT_FIELD_staff_ui_supply_shortage,
    "staff_ui_supply_shortage address");
enum { PT_FIELD_staff_ui_staff_shortage = (offsetof(PTNamedMemory, saved_staff_ui_staff_shortage)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_ui_staff_shortage) == PT_FIELD_staff_ui_staff_shortage,
    "staff_ui_staff_shortage address");
enum { PT_FIELD_staff_pool_full_error = (offsetof(PTNamedMemory, saved_staff_pool_full_error)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_pool_full_error) == PT_FIELD_staff_pool_full_error,
    "staff_pool_full_error address");
enum { PT_FIELD_staff_hire_none_error = (offsetof(PTNamedMemory, saved_staff_hire_none_error)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_hire_none_error) == PT_FIELD_staff_hire_none_error,
    "staff_hire_none_error address");
enum { PT_FIELD_staff_hire_full_format = (offsetof(PTNamedMemory, saved_staff_hire_full_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_hire_full_format) == PT_FIELD_staff_hire_full_format,
    "staff_hire_full_format address");
enum { PT_FIELD_staff_hire_refusal_error = (offsetof(PTNamedMemory, saved_staff_hire_refusal_error)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_hire_refusal_error) == PT_FIELD_staff_hire_refusal_error,
    "staff_hire_refusal_error address");
enum { PT_FIELD_staff_hire_prompt_format = (offsetof(PTNamedMemory, saved_staff_hire_prompt_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_hire_prompt_format) == PT_FIELD_staff_hire_prompt_format,
    "staff_hire_prompt_format address");
enum { PT_FIELD_staff_expenses_label = (offsetof(PTNamedMemory, saved_staff_expenses_label)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_expenses_label) == PT_FIELD_staff_expenses_label,
    "staff_expenses_label address");
enum { PT_FIELD_staff_expenses_underpaid_factor = (offsetof(PTNamedMemory, saved_staff_expenses_underpaid_factor)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_staff_expenses_underpaid_factor) == PT_FIELD_staff_expenses_underpaid_factor,
    "staff_expenses_underpaid_factor address");
enum { PT_FIELD_staff_expenses_overpaid_factor = (offsetof(PTNamedMemory, saved_staff_expenses_overpaid_factor)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_expenses_overpaid_factor) == PT_FIELD_staff_expenses_overpaid_factor,
    "staff_expenses_overpaid_factor address");
enum { PT_FIELD_staff_daily_underpaid_factor = (offsetof(PTNamedMemory, saved_staff_daily_underpaid_factor)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_daily_underpaid_factor) == PT_FIELD_staff_daily_underpaid_factor,
    "staff_daily_underpaid_factor address");
enum { PT_FIELD_staff_daily_overpaid_factor = (offsetof(PTNamedMemory, saved_staff_daily_overpaid_factor)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_daily_overpaid_factor) == PT_FIELD_staff_daily_overpaid_factor,
    "staff_daily_overpaid_factor address");
enum { PT_FIELD_staff_ui_name = (offsetof(PTNamedMemory, saved_staff_ui_name)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_ui_name) == PT_FIELD_staff_ui_name, "staff_ui_name address");
enum { PT_FIELD_staff_ui_days_absent = (offsetof(PTNamedMemory, saved_staff_ui_days_absent)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_ui_days_absent) == PT_FIELD_staff_ui_days_absent,
    "staff_ui_days_absent address");
enum { PT_FIELD_staff_ui_capacity = (offsetof(PTNamedMemory, saved_staff_ui_capacity)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_staff_ui_capacity) == PT_FIELD_staff_ui_capacity, "staff_ui_capacity address");
enum { PT_FIELD_staff_ui_salary = (offsetof(PTNamedMemory, saved_staff_ui_salary)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_ui_salary) == PT_FIELD_staff_ui_salary, "staff_ui_salary address");
enum { PT_FIELD_staff_ui_name_format = (offsetof(PTNamedMemory, saved_staff_ui_name_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_ui_name_format) == PT_FIELD_staff_ui_name_format,
    "staff_ui_name_format address");
enum { PT_FIELD_staff_ui_course = (offsetof(PTNamedMemory, saved_staff_ui_course)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_ui_course) == PT_FIELD_staff_ui_course, "staff_ui_course address");
enum { PT_FIELD_staff_ui_sick = (offsetof(PTNamedMemory, saved_staff_ui_sick)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_ui_sick) == PT_FIELD_staff_ui_sick, "staff_ui_sick address");
enum { PT_FIELD_staff_ui_pregnant = (offsetof(PTNamedMemory, saved_staff_ui_pregnant)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_staff_ui_pregnant) == PT_FIELD_staff_ui_pregnant, "staff_ui_pregnant address");
enum { PT_FIELD_staff_ui_chefs = (offsetof(PTNamedMemory, saved_staff_ui_chefs)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_ui_chefs) == PT_FIELD_staff_ui_chefs, "staff_ui_chefs address");
enum { PT_FIELD_staff_ui_waiters = (offsetof(PTNamedMemory, saved_staff_ui_waiters)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_staff_ui_waiters) == PT_FIELD_staff_ui_waiters, "staff_ui_waiters address");
enum { PT_FIELD_staff_ui_number = (offsetof(PTNamedMemory, saved_staff_ui_number)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_ui_number) == PT_FIELD_staff_ui_number, "staff_ui_number address");
enum { PT_FIELD_staff_ui_total_capacity = (offsetof(PTNamedMemory, saved_staff_ui_total_capacity)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_ui_total_capacity) == PT_FIELD_staff_ui_total_capacity,
    "staff_ui_total_capacity address");
enum { PT_FIELD_staff_ui_average_quality = (offsetof(PTNamedMemory, saved_staff_ui_average_quality)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_ui_average_quality) == PT_FIELD_staff_ui_average_quality,
    "staff_ui_average_quality address");
enum { PT_FIELD_staff_ui_time_per_pizza = (offsetof(PTNamedMemory, saved_staff_ui_time_per_pizza)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_ui_time_per_pizza) == PT_FIELD_staff_ui_time_per_pizza,
    "staff_ui_time_per_pizza address");
enum { PT_FIELD_staff_ui_work = (offsetof(PTNamedMemory, saved_staff_ui_work)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_ui_work) == PT_FIELD_staff_ui_work, "staff_ui_work address");
enum { PT_FIELD_staff_ui_total_costs = (offsetof(PTNamedMemory, saved_staff_ui_total_costs)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_ui_total_costs) == PT_FIELD_staff_ui_total_costs,
    "staff_ui_total_costs address");
enum { PT_FIELD_staff_fire_selection_error = (offsetof(PTNamedMemory, saved_staff_fire_selection_error)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_fire_selection_error) == PT_FIELD_staff_fire_selection_error,
    "staff_fire_selection_error address");
enum { PT_FIELD_staff_fire_away_format = (offsetof(PTNamedMemory, saved_staff_fire_away_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_fire_away_format) == PT_FIELD_staff_fire_away_format,
    "staff_fire_away_format address");
enum { PT_FIELD_staff_fire_prompt = (offsetof(PTNamedMemory, saved_staff_fire_prompt)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_staff_fire_prompt) == PT_FIELD_staff_fire_prompt, "staff_fire_prompt address");
enum { PT_FIELD_staff_wages_selection_error = (offsetof(PTNamedMemory, saved_staff_wages_selection_error)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_wages_selection_error) == PT_FIELD_staff_wages_selection_error,
    "staff_wages_selection_error address");
enum { PT_FIELD_staff_wage_unit = (offsetof(PTNamedMemory, saved_staff_wage_unit)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_wage_unit) == PT_FIELD_staff_wage_unit, "staff_wage_unit address");
enum { PT_FIELD_staff_wage_prompt = (offsetof(PTNamedMemory, saved_staff_wage_prompt)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_staff_wage_prompt) == PT_FIELD_staff_wage_prompt, "staff_wage_prompt address");
enum { PT_FIELD_staff_course_selection_error = (offsetof(PTNamedMemory, saved_staff_course_selection_error)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_course_selection_error) == PT_FIELD_staff_course_selection_error,
    "staff_course_selection_error address");
enum { PT_FIELD_staff_course_already_format = (offsetof(PTNamedMemory, saved_staff_course_already_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_course_already_format) == PT_FIELD_staff_course_already_format,
    "staff_course_already_format address");
enum { PT_FIELD_staff_course_away_format = (offsetof(PTNamedMemory, saved_staff_course_away_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_course_away_format) == PT_FIELD_staff_course_away_format,
    "staff_course_away_format address");
enum { PT_FIELD_goal_background_filename_format = (offsetof(PTNamedMemory, saved_goal_background_filename_format)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_goal_background_filename_format) == PT_FIELD_goal_background_filename_format,
    "goal_background_filename_format address");
enum { PT_FIELD_mission_completion_format = (offsetof(PTNamedMemory, saved_mission_completion_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_mission_completion_format) == PT_FIELD_mission_completion_format,
    "mission_completion_format address");
enum { PT_FIELD_character_filename_format = (offsetof(PTNamedMemory, saved_character_filename_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_character_filename_format) == PT_FIELD_character_filename_format,
    "character_filename_format address");
enum { PT_FIELD_goal_name_1000_pizzas = (offsetof(PTNamedMemory, saved_goal_name_1000_pizzas)) };
_Static_assert(offsetof(PTNamedMemory, saved_goal_name_1000_pizzas) == PT_FIELD_goal_name_1000_pizzas,
    "goal_name_1000_pizzas address");
enum { PT_FIELD_goal_name_10000_pizzas = (offsetof(PTNamedMemory, saved_goal_name_10000_pizzas)) };
_Static_assert(offsetof(PTNamedMemory, saved_goal_name_10000_pizzas) == PT_FIELD_goal_name_10000_pizzas,
    "goal_name_10000_pizzas address");
enum { PT_FIELD_goal_name_profit_25000 = (offsetof(PTNamedMemory, saved_goal_name_profit_25000)) };
_Static_assert(offsetof(PTNamedMemory, saved_goal_name_profit_25000) == PT_FIELD_goal_name_profit_25000,
    "goal_name_profit_25000 address");
enum { PT_FIELD_goal_name_profit_50000 = (offsetof(PTNamedMemory, saved_goal_name_profit_50000)) };
_Static_assert(offsetof(PTNamedMemory, saved_goal_name_profit_50000) == PT_FIELD_goal_name_profit_50000,
    "goal_name_profit_50000 address");
enum { PT_FIELD_goal_name_world = (offsetof(PTNamedMemory, saved_goal_name_world)) };
_Static_assert(offsetof(PTNamedMemory, saved_goal_name_world) == PT_FIELD_goal_name_world, "goal_name_world address");
enum { PT_FIELD_curriculum_date_format = (offsetof(PTNamedMemory, saved_curriculum_date_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_curriculum_date_format) == PT_FIELD_curriculum_date_format,
    "curriculum_date_format address");
enum { PT_FIELD_character_initial_labels = (offsetof(PTNamedMemory, saved_character_initial_labels)) };
_Static_assert(offsetof(PTNamedMemory, saved_character_initial_labels) == PT_FIELD_character_initial_labels,
    "character_initial_labels address");
enum { PT_FIELD_target_curriculum_heading = (offsetof(PTNamedMemory, saved_target_curriculum_heading)) };
_Static_assert(offsetof(PTNamedMemory, saved_target_curriculum_heading) == PT_FIELD_target_curriculum_heading,
    "target_curriculum_heading address");
enum { PT_FIELD_curriculum_mission_only = (offsetof(PTNamedMemory, saved_curriculum_mission_only)) };
_Static_assert(offsetof(PTNamedMemory, saved_curriculum_mission_only) == PT_FIELD_curriculum_mission_only,
    "curriculum_mission_only address");
enum { PT_FIELD_curriculum_heading = (offsetof(PTNamedMemory, saved_curriculum_heading)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_curriculum_heading) == PT_FIELD_curriculum_heading, "curriculum_heading address");
enum { PT_FIELD_character_full_name_format = (offsetof(PTNamedMemory, saved_character_full_name_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_character_full_name_format) == PT_FIELD_character_full_name_format,
    "character_full_name_format address");
enum { PT_FIELD_character_value_labels = (offsetof(PTNamedMemory, saved_character_value_labels)) };
_Static_assert(offsetof(PTNamedMemory, saved_character_value_labels) == PT_FIELD_character_value_labels,
    "character_value_labels address");
enum { PT_FIELD_character_company_heading = (offsetof(PTNamedMemory, saved_character_company_heading)) };
_Static_assert(offsetof(PTNamedMemory, saved_character_company_heading) == PT_FIELD_character_company_heading,
    "character_company_heading address");
enum { PT_FIELD_character_company_owner_format = (offsetof(PTNamedMemory, saved_character_company_owner_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_character_company_owner_format) == PT_FIELD_character_company_owner_format,
    "character_company_owner_format address");
enum { PT_FIELD_character_company_profit = (offsetof(PTNamedMemory, saved_character_company_profit)) };
_Static_assert(offsetof(PTNamedMemory, saved_character_company_profit) == PT_FIELD_character_company_profit,
    "character_company_profit address");
enum { PT_FIELD_character_awards_filename = (offsetof(PTNamedMemory, saved_character_awards_filename)) };
_Static_assert(offsetof(PTNamedMemory, saved_character_awards_filename) == PT_FIELD_character_awards_filename,
    "character_awards_filename address");
enum { PT_FIELD_character_awards_heading = (offsetof(PTNamedMemory, saved_character_awards_heading)) };
_Static_assert(offsetof(PTNamedMemory, saved_character_awards_heading) == PT_FIELD_character_awards_heading,
    "character_awards_heading address");
enum { PT_FIELD_character_holiday_question = (offsetof(PTNamedMemory, saved_character_holiday_question)) };
_Static_assert(offsetof(PTNamedMemory, saved_character_holiday_question) == PT_FIELD_character_holiday_question,
    "character_holiday_question address");
enum { PT_FIELD_character_holiday_days = (offsetof(PTNamedMemory, saved_character_holiday_days)) };
_Static_assert(offsetof(PTNamedMemory, saved_character_holiday_days) == PT_FIELD_character_holiday_days,
    "character_holiday_days address");
enum { PT_FIELD_character_holiday_prompt = (offsetof(PTNamedMemory, saved_character_holiday_prompt)) };
_Static_assert(offsetof(PTNamedMemory, saved_character_holiday_prompt) == PT_FIELD_character_holiday_prompt,
    "character_holiday_prompt address");
enum {
  PT_FIELD_personal_character_filename_format = (offsetof(PTNamedMemory, saved_personal_character_filename_format))
};
_Static_assert(
    offsetof(PTNamedMemory, saved_personal_character_filename_format) == PT_FIELD_personal_character_filename_format,
    "personal_character_filename_format address");
enum { PT_FIELD_personal_mission_filename_format = (offsetof(PTNamedMemory, saved_personal_mission_filename_format)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_personal_mission_filename_format) == PT_FIELD_personal_mission_filename_format,
    "personal_mission_filename_format address");
enum { PT_FIELD_depot_weapon_category_label = (offsetof(PTNamedMemory, saved_depot_weapon_category_label)) };
_Static_assert(offsetof(PTNamedMemory, saved_depot_weapon_category_label) == PT_FIELD_depot_weapon_category_label,
    "depot_weapon_category_label address");
enum { PT_FIELD_stock_menu_depot_title = (offsetof(PTNamedMemory, saved_stock_menu_depot_title)) };
_Static_assert(offsetof(PTNamedMemory, saved_stock_menu_depot_title) == PT_FIELD_stock_menu_depot_title,
    "stock_menu_depot_title address");
enum { PT_FIELD_depot_weapon_bag_label = (offsetof(PTNamedMemory, saved_depot_weapon_bag_label)) };
_Static_assert(offsetof(PTNamedMemory, saved_depot_weapon_bag_label) == PT_FIELD_depot_weapon_bag_label,
    "depot_weapon_bag_label address");
enum { PT_FIELD_depot_town_title_format = (offsetof(PTNamedMemory, saved_depot_town_title_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_depot_town_title_format) == PT_FIELD_depot_town_title_format,
    "depot_town_title_format address");
enum { PT_FIELD_depot_columns_label = (offsetof(PTNamedMemory, saved_depot_columns_label)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_depot_columns_label) == PT_FIELD_depot_columns_label, "depot_columns_label address");
enum { PT_FIELD_depot_sum_value_label = (offsetof(PTNamedMemory, saved_depot_sum_value_label)) };
_Static_assert(offsetof(PTNamedMemory, saved_depot_sum_value_label) == PT_FIELD_depot_sum_value_label,
    "depot_sum_value_label address");
enum { PT_FIELD_depot_export_title_format = (offsetof(PTNamedMemory, saved_depot_export_title_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_depot_export_title_format) == PT_FIELD_depot_export_title_format,
    "depot_export_title_format address");
enum { PT_FIELD_depot_sale_label = (offsetof(PTNamedMemory, saved_depot_sale_label)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_depot_sale_label) == PT_FIELD_depot_sale_label, "depot_sale_label address");
enum { PT_FIELD_depot_purchase_label = (offsetof(PTNamedMemory, saved_depot_purchase_label)) };
_Static_assert(offsetof(PTNamedMemory, saved_depot_purchase_label) == PT_FIELD_depot_purchase_label,
    "depot_purchase_label address");
enum { PT_FIELD_depot_purchase_title_format = (offsetof(PTNamedMemory, saved_depot_purchase_title_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_depot_purchase_title_format) == PT_FIELD_depot_purchase_title_format,
    "depot_purchase_title_format address");
enum { PT_FIELD_shop_price_label = (offsetof(PTNamedMemory, saved_shop_price_label)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_shop_price_label) == PT_FIELD_shop_price_label, "shop_price_label address");
enum { PT_FIELD_depot_supplier_column_label = (offsetof(PTNamedMemory, saved_depot_supplier_column_label)) };
_Static_assert(offsetof(PTNamedMemory, saved_depot_supplier_column_label) == PT_FIELD_depot_supplier_column_label,
    "depot_supplier_column_label address");
enum { PT_FIELD_shop_amount_label = (offsetof(PTNamedMemory, saved_shop_amount_label)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_shop_amount_label) == PT_FIELD_shop_amount_label, "shop_amount_label address");
enum { PT_FIELD_depot_store_column_label = (offsetof(PTNamedMemory, saved_depot_store_column_label)) };
_Static_assert(offsetof(PTNamedMemory, saved_depot_store_column_label) == PT_FIELD_depot_store_column_label,
    "depot_store_column_label address");
enum { PT_FIELD_contract_sum_label = (offsetof(PTNamedMemory, saved_contract_sum_label)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_contract_sum_label) == PT_FIELD_contract_sum_label, "contract_sum_label address");
enum { PT_FIELD_depot_quality_rejection_text = (offsetof(PTNamedMemory, saved_depot_quality_rejection_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_depot_quality_rejection_text) == PT_FIELD_depot_quality_rejection_text,
    "depot_quality_rejection_text address");
enum { PT_FIELD_depot_trade_price_divisor = (offsetof(PTNamedMemory, saved_depot_trade_price_divisor)) };
_Static_assert(offsetof(PTNamedMemory, saved_depot_trade_price_divisor) == PT_FIELD_depot_trade_price_divisor,
    "depot_trade_price_divisor address");
enum { PT_FIELD_depot_trade_rating_divisor = (offsetof(PTNamedMemory, saved_depot_trade_rating_divisor)) };
_Static_assert(offsetof(PTNamedMemory, saved_depot_trade_rating_divisor) == PT_FIELD_depot_trade_rating_divisor,
    "depot_trade_rating_divisor address");
enum { PT_FIELD_depot_priorities_title = (offsetof(PTNamedMemory, saved_depot_priorities_title)) };
_Static_assert(offsetof(PTNamedMemory, saved_depot_priorities_title) == PT_FIELD_depot_priorities_title,
    "depot_priorities_title address");
enum { PT_FIELD_contract_discount_label = (offsetof(PTNamedMemory, saved_contract_discount_label)) };
_Static_assert(offsetof(PTNamedMemory, saved_contract_discount_label) == PT_FIELD_contract_discount_label,
    "contract_discount_label address");
enum { PT_FIELD_contract_charge_label = (offsetof(PTNamedMemory, saved_contract_charge_label)) };
_Static_assert(offsetof(PTNamedMemory, saved_contract_charge_label) == PT_FIELD_contract_charge_label,
    "contract_charge_label address");
enum { PT_FIELD_contract_terms_penalty_label = (offsetof(PTNamedMemory, saved_contract_terms_penalty_label)) };
_Static_assert(offsetof(PTNamedMemory, saved_contract_terms_penalty_label) == PT_FIELD_contract_terms_penalty_label,
    "contract_terms_penalty_label address");
enum { PT_FIELD_contract_term_format = (offsetof(PTNamedMemory, saved_contract_term_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_contract_term_format) == PT_FIELD_contract_term_format,
    "contract_term_format address");
enum { PT_FIELD_contract_price_divisor = (offsetof(PTNamedMemory, saved_contract_price_divisor)) };
_Static_assert(offsetof(PTNamedMemory, saved_contract_price_divisor) == PT_FIELD_contract_price_divisor,
    "contract_price_divisor address");
enum { PT_FIELD_contract_debug_portrait_prompt = (offsetof(PTNamedMemory, saved_contract_debug_portrait_prompt)) };
_Static_assert(offsetof(PTNamedMemory, saved_contract_debug_portrait_prompt) == PT_FIELD_contract_debug_portrait_prompt,
    "contract_debug_portrait_prompt address");
enum { PT_FIELD_contract_minimum_order_format = (offsetof(PTNamedMemory, saved_contract_minimum_order_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_contract_minimum_order_format) == PT_FIELD_contract_minimum_order_format,
    "contract_minimum_order_format address");
enum { PT_FIELD_contract_purchase_divisor = (offsetof(PTNamedMemory, saved_contract_purchase_divisor)) };
_Static_assert(offsetof(PTNamedMemory, saved_contract_purchase_divisor) == PT_FIELD_contract_purchase_divisor,
    "contract_purchase_divisor address");
enum { PT_FIELD_contract_delivery_amounts_title = (offsetof(PTNamedMemory, saved_contract_delivery_amounts_title)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_contract_delivery_amounts_title) == PT_FIELD_contract_delivery_amounts_title,
    "contract_delivery_amounts_title address");
enum { PT_FIELD_stock_menu_kg_label = (offsetof(PTNamedMemory, saved_stock_menu_kg_label)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_stock_menu_kg_label) == PT_FIELD_stock_menu_kg_label, "stock_menu_kg_label address");
enum { PT_FIELD_contract_with_title = (offsetof(PTNamedMemory, saved_contract_with_title)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_contract_with_title) == PT_FIELD_contract_with_title, "contract_with_title address");
enum { PT_FIELD_shop_total_label = (offsetof(PTNamedMemory, saved_shop_total_label)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_shop_total_label) == PT_FIELD_shop_total_label, "shop_total_label address");
enum { PT_FIELD_contract_penalty_label = (offsetof(PTNamedMemory, saved_contract_penalty_label)) };
_Static_assert(offsetof(PTNamedMemory, saved_contract_penalty_label) == PT_FIELD_contract_penalty_label,
    "contract_penalty_label address");
enum { PT_FIELD_contract_amount_label = (offsetof(PTNamedMemory, saved_contract_amount_label)) };
_Static_assert(offsetof(PTNamedMemory, saved_contract_amount_label) == PT_FIELD_contract_amount_label,
    "contract_amount_label address");
enum { PT_FIELD_contract_as_required_label = (offsetof(PTNamedMemory, saved_contract_as_required_label)) };
_Static_assert(offsetof(PTNamedMemory, saved_contract_as_required_label) == PT_FIELD_contract_as_required_label,
    "contract_as_required_label address");
enum { PT_FIELD_contract_as_agreed_label = (offsetof(PTNamedMemory, saved_contract_as_agreed_label)) };
_Static_assert(offsetof(PTNamedMemory, saved_contract_as_agreed_label) == PT_FIELD_contract_as_agreed_label,
    "contract_as_agreed_label address");
enum { PT_FIELD_contract_quality_label = (offsetof(PTNamedMemory, saved_contract_quality_label)) };
_Static_assert(offsetof(PTNamedMemory, saved_contract_quality_label) == PT_FIELD_contract_quality_label,
    "contract_quality_label address");
enum { PT_FIELD_contract_remaining_term_label = (offsetof(PTNamedMemory, saved_contract_remaining_term_label)) };
_Static_assert(offsetof(PTNamedMemory, saved_contract_remaining_term_label) == PT_FIELD_contract_remaining_term_label,
    "contract_remaining_term_label address");
enum { PT_FIELD_contract_empty_error = (offsetof(PTNamedMemory, saved_contract_empty_error)) };
_Static_assert(offsetof(PTNamedMemory, saved_contract_empty_error) == PT_FIELD_contract_empty_error,
    "contract_empty_error address");
enum { PT_FIELD_contract_term_prompt = (offsetof(PTNamedMemory, saved_contract_term_prompt)) };
_Static_assert(offsetof(PTNamedMemory, saved_contract_term_prompt) == PT_FIELD_contract_term_prompt,
    "contract_term_prompt address");
enum { PT_FIELD_contract_cancel_prompt = (offsetof(PTNamedMemory, saved_contract_cancel_prompt)) };
_Static_assert(offsetof(PTNamedMemory, saved_contract_cancel_prompt) == PT_FIELD_contract_cancel_prompt,
    "contract_cancel_prompt address");
enum { PT_FIELD_contract_delivery_space_text = (offsetof(PTNamedMemory, saved_contract_delivery_space_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_contract_delivery_space_text) == PT_FIELD_contract_delivery_space_text,
    "contract_delivery_space_text address");
enum { PT_FIELD_contract_cancel_format = (offsetof(PTNamedMemory, saved_contract_cancel_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_contract_cancel_format) == PT_FIELD_contract_cancel_format,
    "contract_cancel_format address");
enum { PT_FIELD_shop_unavailable_price_text = (offsetof(PTNamedMemory, saved_shop_unavailable_price_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_shop_unavailable_price_text) == PT_FIELD_shop_unavailable_price_text,
    "shop_unavailable_price_text address");
enum { PT_FIELD_shop_quality_format = (offsetof(PTNamedMemory, saved_shop_quality_format)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_shop_quality_format) == PT_FIELD_shop_quality_format, "shop_quality_format address");
enum { PT_FIELD_shop_stock_label = (offsetof(PTNamedMemory, saved_shop_stock_label)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_shop_stock_label) == PT_FIELD_shop_stock_label, "shop_stock_label address");
enum { PT_FIELD_stock_menu_days_label = (offsetof(PTNamedMemory, saved_stock_menu_days_label)) };
_Static_assert(offsetof(PTNamedMemory, saved_stock_menu_days_label) == PT_FIELD_stock_menu_days_label,
    "stock_menu_days_label address");
enum { PT_FIELD_shop_price_divisor = (offsetof(PTNamedMemory, saved_shop_price_divisor)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_shop_price_divisor) == PT_FIELD_shop_price_divisor, "shop_price_divisor address");
enum { PT_FIELD_contract_expired_format = (offsetof(PTNamedMemory, saved_contract_expired_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_contract_expired_format) == PT_FIELD_contract_expired_format,
    "contract_expired_format address");
enum { PT_FIELD_contract_shortage_format = (offsetof(PTNamedMemory, saved_contract_shortage_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_contract_shortage_format) == PT_FIELD_contract_shortage_format,
    "contract_shortage_format address");
enum { PT_FIELD_contract_delivery_format = (offsetof(PTNamedMemory, saved_contract_delivery_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_contract_delivery_format) == PT_FIELD_contract_delivery_format,
    "contract_delivery_format address");
enum { PT_FIELD_shop_suppliers_title_format = (offsetof(PTNamedMemory, saved_shop_suppliers_title_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_shop_suppliers_title_format) == PT_FIELD_shop_suppliers_title_format,
    "shop_suppliers_title_format address");
enum { PT_FIELD_shop_supplier_total_format = (offsetof(PTNamedMemory, saved_shop_supplier_total_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_shop_supplier_total_format) == PT_FIELD_shop_supplier_total_format,
    "shop_supplier_total_format address");
enum { PT_FIELD_shop_page_format = (offsetof(PTNamedMemory, saved_shop_page_format)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_shop_page_format) == PT_FIELD_shop_page_format, "shop_page_format address");
enum { PT_FIELD_stock_menu_stocks_title = (offsetof(PTNamedMemory, saved_stock_menu_stocks_title)) };
_Static_assert(offsetof(PTNamedMemory, saved_stock_menu_stocks_title) == PT_FIELD_stock_menu_stocks_title,
    "stock_menu_stocks_title address");
enum { PT_FIELD_stock_menu_kilogram_label = (offsetof(PTNamedMemory, saved_stock_menu_kilogram_label)) };
_Static_assert(offsetof(PTNamedMemory, saved_stock_menu_kilogram_label) == PT_FIELD_stock_menu_kilogram_label,
    "stock_menu_kilogram_label address");
enum { PT_FIELD_stock_menu_value_label = (offsetof(PTNamedMemory, saved_stock_menu_value_label)) };
_Static_assert(offsetof(PTNamedMemory, saved_stock_menu_value_label) == PT_FIELD_stock_menu_value_label,
    "stock_menu_value_label address");
enum { PT_FIELD_stock_menu_supplies_format = (offsetof(PTNamedMemory, saved_stock_menu_supplies_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_stock_menu_supplies_format) == PT_FIELD_stock_menu_supplies_format,
    "stock_menu_supplies_format address");
enum { PT_FIELD_stock_menu_quality_label = (offsetof(PTNamedMemory, saved_stock_menu_quality_label)) };
_Static_assert(offsetof(PTNamedMemory, saved_stock_menu_quality_label) == PT_FIELD_stock_menu_quality_label,
    "stock_menu_quality_label address");
enum { PT_FIELD_stock_menu_supply_label = (offsetof(PTNamedMemory, saved_stock_menu_supply_label)) };
_Static_assert(offsetof(PTNamedMemory, saved_stock_menu_supply_label) == PT_FIELD_stock_menu_supply_label,
    "stock_menu_supply_label address");
enum { PT_FIELD_stock_menu_need_label = (offsetof(PTNamedMemory, saved_stock_menu_need_label)) };
_Static_assert(offsetof(PTNamedMemory, saved_stock_menu_need_label) == PT_FIELD_stock_menu_need_label,
    "stock_menu_need_label address");
enum { PT_FIELD_stock_menu_use_label = (offsetof(PTNamedMemory, saved_stock_menu_use_label)) };
_Static_assert(offsetof(PTNamedMemory, saved_stock_menu_use_label) == PT_FIELD_stock_menu_use_label,
    "stock_menu_use_label address");
enum { PT_FIELD_stock_menu_demand_divisor = (offsetof(PTNamedMemory, saved_stock_menu_demand_divisor)) };
_Static_assert(offsetof(PTNamedMemory, saved_stock_menu_demand_divisor) == PT_FIELD_stock_menu_demand_divisor,
    "stock_menu_demand_divisor address");
enum { PT_FIELD_supplier_price_addon_divisor = (offsetof(PTNamedMemory, saved_supplier_price_addon_divisor)) };
_Static_assert(offsetof(PTNamedMemory, saved_supplier_price_addon_divisor) == PT_FIELD_supplier_price_addon_divisor,
    "supplier_price_addon_divisor address");
enum { PT_FIELD_supplier_price_level5_factor = (offsetof(PTNamedMemory, saved_supplier_price_level5_factor)) };
_Static_assert(offsetof(PTNamedMemory, saved_supplier_price_level5_factor) == PT_FIELD_supplier_price_level5_factor,
    "supplier_price_level5_factor address");
enum { PT_FIELD_supplier_price_level4_factor = (offsetof(PTNamedMemory, saved_supplier_price_level4_factor)) };
_Static_assert(offsetof(PTNamedMemory, saved_supplier_price_level4_factor) == PT_FIELD_supplier_price_level4_factor,
    "supplier_price_level4_factor address");
enum { PT_FIELD_supplier_price_level2_factor = (offsetof(PTNamedMemory, saved_supplier_price_level2_factor)) };
_Static_assert(offsetof(PTNamedMemory, saved_supplier_price_level2_factor) == PT_FIELD_supplier_price_level2_factor,
    "supplier_price_level2_factor address");
enum { PT_FIELD_supplier_price_level1_factor = (offsetof(PTNamedMemory, saved_supplier_price_level1_factor)) };
_Static_assert(offsetof(PTNamedMemory, saved_supplier_price_level1_factor) == PT_FIELD_supplier_price_level1_factor,
    "supplier_price_level1_factor address");
enum { PT_FIELD_depot_central_label = (offsetof(PTNamedMemory, saved_depot_central_label)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_depot_central_label) == PT_FIELD_depot_central_label, "depot_central_label address");
enum { PT_FIELD_depot_capacity_label = (offsetof(PTNamedMemory, saved_depot_capacity_label)) };
_Static_assert(offsetof(PTNamedMemory, saved_depot_capacity_label) == PT_FIELD_depot_capacity_label,
    "depot_capacity_label address");
enum { PT_FIELD_depot_in_use_label = (offsetof(PTNamedMemory, saved_depot_in_use_label)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_depot_in_use_label) == PT_FIELD_depot_in_use_label, "depot_in_use_label address");
enum { PT_FIELD_depot_stolen_format = (offsetof(PTNamedMemory, saved_depot_stolen_format)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_depot_stolen_format) == PT_FIELD_depot_stolen_format, "depot_stolen_format address");
enum { PT_FIELD_contract_penalty_new_factor = (offsetof(PTNamedMemory, saved_contract_penalty_new_factor)) };
_Static_assert(offsetof(PTNamedMemory, saved_contract_penalty_new_factor) == PT_FIELD_contract_penalty_new_factor,
    "contract_penalty_new_factor address");
enum { PT_FIELD_contract_discount_factor = (offsetof(PTNamedMemory, saved_contract_discount_factor)) };
_Static_assert(offsetof(PTNamedMemory, saved_contract_discount_factor) == PT_FIELD_contract_discount_factor,
    "contract_discount_factor address");
enum { PT_FIELD_contract_discount_divisor = (offsetof(PTNamedMemory, saved_contract_discount_divisor)) };
_Static_assert(offsetof(PTNamedMemory, saved_contract_discount_divisor) == PT_FIELD_contract_discount_divisor,
    "contract_discount_divisor address");
enum { PT_FIELD_contract_charge_divisor = (offsetof(PTNamedMemory, saved_contract_charge_divisor)) };
_Static_assert(offsetof(PTNamedMemory, saved_contract_charge_divisor) == PT_FIELD_contract_charge_divisor,
    "contract_charge_divisor address");
enum { PT_FIELD_contract_fake_sum_factor = (offsetof(PTNamedMemory, saved_contract_fake_sum_factor)) };
_Static_assert(offsetof(PTNamedMemory, saved_contract_fake_sum_factor) == PT_FIELD_contract_fake_sum_factor,
    "contract_fake_sum_factor address");
enum {
  PT_FIELD_contract_penalty_reduction_factor = (offsetof(PTNamedMemory, saved_contract_penalty_reduction_factor))
};
_Static_assert(
    offsetof(PTNamedMemory, saved_contract_penalty_reduction_factor) == PT_FIELD_contract_penalty_reduction_factor,
    "contract_penalty_reduction_factor address");
enum { PT_FIELD_language_filename_format = (offsetof(PTNamedMemory, saved_language_filename_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_language_filename_format) == PT_FIELD_language_filename_format,
    "language_filename_format address");
enum { PT_FIELD_file_write_mode = (offsetof(PTNamedMemory, saved_file_write_mode)) };
_Static_assert(offsetof(PTNamedMemory, saved_file_write_mode) == PT_FIELD_file_write_mode, "file_write_mode address");
enum { PT_FIELD_file_readwrite_mode = (offsetof(PTNamedMemory, saved_file_readwrite_mode)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_file_readwrite_mode) == PT_FIELD_file_readwrite_mode, "file_readwrite_mode address");
enum { PT_FIELD_double_720 = (offsetof(PTNamedMemory, saved_double_720)) };
_Static_assert(offsetof(PTNamedMemory, saved_double_720) == PT_FIELD_double_720, "double_720 address");
enum { PT_FIELD_customer_other_tile_factor = (offsetof(PTNamedMemory, saved_customer_other_tile_factor)) };
_Static_assert(offsetof(PTNamedMemory, saved_customer_other_tile_factor) == PT_FIELD_customer_other_tile_factor,
    "customer_other_tile_factor address");
enum { PT_FIELD_customer_residential_tile_factor = (offsetof(PTNamedMemory, saved_customer_residential_tile_factor)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_customer_residential_tile_factor) == PT_FIELD_customer_residential_tile_factor,
    "customer_residential_tile_factor address");
enum { PT_FIELD_float_720 = (offsetof(PTNamedMemory, saved_float_720)) };
_Static_assert(offsetof(PTNamedMemory, saved_float_720) == PT_FIELD_float_720, "float_720 address");
enum { PT_FIELD_pizza_income_price_divisor = (offsetof(PTNamedMemory, saved_pizza_income_price_divisor)) };
_Static_assert(offsetof(PTNamedMemory, saved_pizza_income_price_divisor) == PT_FIELD_pizza_income_price_divisor,
    "pizza_income_price_divisor address");
enum { PT_FIELD_pizza_income_price_offset = (offsetof(PTNamedMemory, saved_pizza_income_price_offset)) };
_Static_assert(offsetof(PTNamedMemory, saved_pizza_income_price_offset) == PT_FIELD_pizza_income_price_offset,
    "pizza_income_price_offset address");
enum { PT_FIELD_pizza_income_base_divisor = (offsetof(PTNamedMemory, saved_pizza_income_base_divisor)) };
_Static_assert(offsetof(PTNamedMemory, saved_pizza_income_base_divisor) == PT_FIELD_pizza_income_base_divisor,
    "pizza_income_base_divisor address");
enum { PT_FIELD_pizza_income_base_offset = (offsetof(PTNamedMemory, saved_pizza_income_base_offset)) };
_Static_assert(offsetof(PTNamedMemory, saved_pizza_income_base_offset) == PT_FIELD_pizza_income_base_offset,
    "pizza_income_base_offset address");
enum { PT_FIELD_pizza_price_offset = (offsetof(PTNamedMemory, saved_pizza_price_offset)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_pizza_price_offset) == PT_FIELD_pizza_price_offset, "pizza_price_offset address");
enum { PT_FIELD_pizza_price_divisor = (offsetof(PTNamedMemory, saved_pizza_price_divisor)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_pizza_price_divisor) == PT_FIELD_pizza_price_divisor, "pizza_price_divisor address");
enum { PT_FIELD_pizza_price_bonus_scale = (offsetof(PTNamedMemory, saved_pizza_price_bonus_scale)) };
_Static_assert(offsetof(PTNamedMemory, saved_pizza_price_bonus_scale) == PT_FIELD_pizza_price_bonus_scale,
    "pizza_price_bonus_scale address");
enum { PT_FIELD_customer_distribution_factor = (offsetof(PTNamedMemory, saved_customer_distribution_factor)) };
_Static_assert(offsetof(PTNamedMemory, saved_customer_distribution_factor) == PT_FIELD_customer_distribution_factor,
    "customer_distribution_factor address");
enum { PT_FIELD_pizza_order_random_threshold = (offsetof(PTNamedMemory, saved_pizza_order_random_threshold)) };
_Static_assert(offsetof(PTNamedMemory, saved_pizza_order_random_threshold) == PT_FIELD_pizza_order_random_threshold,
    "pizza_order_random_threshold address");
enum {
  PT_FIELD_restaurant_supplies_shortage_text = (offsetof(PTNamedMemory, saved_restaurant_supplies_shortage_text))
};
_Static_assert(
    offsetof(PTNamedMemory, saved_restaurant_supplies_shortage_text) == PT_FIELD_restaurant_supplies_shortage_text,
    "restaurant_supplies_shortage_text address");
enum { PT_FIELD_restaurant_ovens_shortage_text = (offsetof(PTNamedMemory, saved_restaurant_ovens_shortage_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_restaurant_ovens_shortage_text) == PT_FIELD_restaurant_ovens_shortage_text,
    "restaurant_ovens_shortage_text address");
enum { PT_FIELD_restaurant_cooks_shortage_text = (offsetof(PTNamedMemory, saved_restaurant_cooks_shortage_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_restaurant_cooks_shortage_text) == PT_FIELD_restaurant_cooks_shortage_text,
    "restaurant_cooks_shortage_text address");
enum { PT_FIELD_restaurant_waiters_shortage_text = (offsetof(PTNamedMemory, saved_restaurant_waiters_shortage_text)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_restaurant_waiters_shortage_text) == PT_FIELD_restaurant_waiters_shortage_text,
    "restaurant_waiters_shortage_text address");
enum {
  PT_FIELD_restaurant_supply_percentage_scale = (offsetof(PTNamedMemory, saved_restaurant_supply_percentage_scale))
};
_Static_assert(
    offsetof(PTNamedMemory, saved_restaurant_supply_percentage_scale) == PT_FIELD_restaurant_supply_percentage_scale,
    "restaurant_supply_percentage_scale address");
enum {
  PT_FIELD_restaurant_turnover_goal_threshold = (offsetof(PTNamedMemory, saved_restaurant_turnover_goal_threshold))
};
_Static_assert(
    offsetof(PTNamedMemory, saved_restaurant_turnover_goal_threshold) == PT_FIELD_restaurant_turnover_goal_threshold,
    "restaurant_turnover_goal_threshold address");
enum {
  PT_FIELD_restaurant_sales_currency_divisor = (offsetof(PTNamedMemory, saved_restaurant_sales_currency_divisor))
};
_Static_assert(
    offsetof(PTNamedMemory, saved_restaurant_sales_currency_divisor) == PT_FIELD_restaurant_sales_currency_divisor,
    "restaurant_sales_currency_divisor address");
enum { PT_FIELD_restaurant_electricity_label = (offsetof(PTNamedMemory, saved_restaurant_electricity_label)) };
_Static_assert(offsetof(PTNamedMemory, saved_restaurant_electricity_label) == PT_FIELD_restaurant_electricity_label,
    "restaurant_electricity_label address");
enum { PT_FIELD_restaurant_slot_income_label = (offsetof(PTNamedMemory, saved_restaurant_slot_income_label)) };
_Static_assert(offsetof(PTNamedMemory, saved_restaurant_slot_income_label) == PT_FIELD_restaurant_slot_income_label,
    "restaurant_slot_income_label address");
enum { PT_FIELD_restaurant_invoice_total_label = (offsetof(PTNamedMemory, saved_restaurant_invoice_total_label)) };
_Static_assert(offsetof(PTNamedMemory, saved_restaurant_invoice_total_label) == PT_FIELD_restaurant_invoice_total_label,
    "restaurant_invoice_total_label address");
enum { PT_FIELD_country_market_leader_label = (offsetof(PTNamedMemory, saved_country_market_leader_label)) };
_Static_assert(offsetof(PTNamedMemory, saved_country_market_leader_label) == PT_FIELD_country_market_leader_label,
    "country_market_leader_label address");
enum { PT_FIELD_country_no_market_leader_label = (offsetof(PTNamedMemory, saved_country_no_market_leader_label)) };
_Static_assert(offsetof(PTNamedMemory, saved_country_no_market_leader_label) == PT_FIELD_country_no_market_leader_label,
    "country_no_market_leader_label address");
enum { PT_FIELD_country_map_filename = (offsetof(PTNamedMemory, saved_country_map_filename)) };
_Static_assert(offsetof(PTNamedMemory, saved_country_map_filename) == PT_FIELD_country_map_filename,
    "country_map_filename address");
enum { PT_FIELD_country_no_city_message = (offsetof(PTNamedMemory, saved_country_no_city_message)) };
_Static_assert(offsetof(PTNamedMemory, saved_country_no_city_message) == PT_FIELD_country_no_city_message,
    "country_no_city_message address");
enum {
  PT_FIELD_country_insufficient_money_message = (offsetof(PTNamedMemory, saved_country_insufficient_money_message))
};
_Static_assert(
    offsetof(PTNamedMemory, saved_country_insufficient_money_message) == PT_FIELD_country_insufficient_money_message,
    "country_insufficient_money_message address");
enum { PT_FIELD_country_flight_cost_format = (offsetof(PTNamedMemory, saved_country_flight_cost_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_country_flight_cost_format) == PT_FIELD_country_flight_cost_format,
    "country_flight_cost_format address");
enum { PT_FIELD_country_transport_cost_format = (offsetof(PTNamedMemory, saved_country_transport_cost_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_country_transport_cost_format) == PT_FIELD_country_transport_cost_format,
    "country_transport_cost_format address");
enum { PT_FIELD_country_missing_depot_format = (offsetof(PTNamedMemory, saved_country_missing_depot_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_country_missing_depot_format) == PT_FIELD_country_missing_depot_format,
    "country_missing_depot_format address");
enum { PT_FIELD_town_initializing_caption = (offsetof(PTNamedMemory, saved_town_initializing_caption)) };
_Static_assert(offsetof(PTNamedMemory, saved_town_initializing_caption) == PT_FIELD_town_initializing_caption,
    "town_initializing_caption address");
enum { PT_FIELD_town_initializing_ellipsis = (offsetof(PTNamedMemory, saved_town_initializing_ellipsis)) };
_Static_assert(offsetof(PTNamedMemory, saved_town_initializing_ellipsis) == PT_FIELD_town_initializing_ellipsis,
    "town_initializing_ellipsis address");
enum { PT_FIELD_staff_market_filename = (offsetof(PTNamedMemory, saved_staff_market_filename)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_market_filename) == PT_FIELD_staff_market_filename,
    "staff_market_filename address");
enum { PT_FIELD_attack_police_message = (offsetof(PTNamedMemory, saved_attack_police_message)) };
_Static_assert(offsetof(PTNamedMemory, saved_attack_police_message) == PT_FIELD_attack_police_message,
    "attack_police_message address");
enum { PT_FIELD_attack_police_argument = (offsetof(PTNamedMemory, saved_attack_police_argument)) };
_Static_assert(offsetof(PTNamedMemory, saved_attack_police_argument) == PT_FIELD_attack_police_argument,
    "attack_police_argument address");
enum { PT_FIELD_guest_walk_bob_format = (offsetof(PTNamedMemory, saved_guest_walk_bob_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_guest_walk_bob_format) == PT_FIELD_guest_walk_bob_format,
    "guest_walk_bob_format address");
enum { PT_FIELD_guest_type_error_text = (offsetof(PTNamedMemory, saved_guest_type_error_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_guest_type_error_text) == PT_FIELD_guest_type_error_text,
    "guest_type_error_text address");
enum { PT_FIELD_attack_weapon_bob_format = (offsetof(PTNamedMemory, saved_attack_weapon_bob_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_attack_weapon_bob_format) == PT_FIELD_attack_weapon_bob_format,
    "attack_weapon_bob_format address");
enum { PT_FIELD_attack_damage_format = (offsetof(PTNamedMemory, saved_attack_damage_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_attack_damage_format) == PT_FIELD_attack_damage_format,
    "attack_damage_format address");
enum { PT_FIELD_attack_contamination_format = (offsetof(PTNamedMemory, saved_attack_contamination_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_attack_contamination_format) == PT_FIELD_attack_contamination_format,
    "attack_contamination_format address");
enum { PT_FIELD_guest_portrait_filename_format = (offsetof(PTNamedMemory, saved_guest_portrait_filename_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_guest_portrait_filename_format) == PT_FIELD_guest_portrait_filename_format,
    "guest_portrait_filename_format address");
enum { PT_FIELD_guest_eating_comment_format = (offsetof(PTNamedMemory, saved_guest_eating_comment_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_guest_eating_comment_format) == PT_FIELD_guest_eating_comment_format,
    "guest_eating_comment_format address");
enum {
  PT_FIELD_ai_restaurant_upgrade_money_factor = (offsetof(PTNamedMemory, saved_ai_restaurant_upgrade_money_factor))
};
_Static_assert(
    offsetof(PTNamedMemory, saved_ai_restaurant_upgrade_money_factor) == PT_FIELD_ai_restaurant_upgrade_money_factor,
    "ai_restaurant_upgrade_money_factor address");
enum { PT_FIELD_ai_low_rating_money_threshold = (offsetof(PTNamedMemory, saved_ai_low_rating_money_threshold)) };
_Static_assert(offsetof(PTNamedMemory, saved_ai_low_rating_money_threshold) == PT_FIELD_ai_low_rating_money_threshold,
    "ai_low_rating_money_threshold address");
enum {
  PT_FIELD_ai_low_default_rating_money_threshold =
      (offsetof(PTNamedMemory, saved_ai_low_default_rating_money_threshold))
};
_Static_assert(offsetof(PTNamedMemory, saved_ai_low_default_rating_money_threshold) ==
        PT_FIELD_ai_low_default_rating_money_threshold,
    "ai_low_default_rating_money_threshold address");
enum { PT_FIELD_competition_pizza_title = (offsetof(PTNamedMemory, saved_competition_pizza_title)) };
_Static_assert(offsetof(PTNamedMemory, saved_competition_pizza_title) == PT_FIELD_competition_pizza_title,
    "competition_pizza_title address");
enum { PT_FIELD_competition_title = (offsetof(PTNamedMemory, saved_competition_title)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_competition_title) == PT_FIELD_competition_title, "competition_title address");
enum { PT_FIELD_competition_challenge_format = (offsetof(PTNamedMemory, saved_competition_challenge_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_competition_challenge_format) == PT_FIELD_competition_challenge_format,
    "competition_challenge_format address");
enum { PT_FIELD_competition_ai_challenge_text = (offsetof(PTNamedMemory, saved_competition_ai_challenge_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_competition_ai_challenge_text) == PT_FIELD_competition_ai_challenge_text,
    "competition_ai_challenge_text address");
enum { PT_FIELD_competition_dollars_unit = (offsetof(PTNamedMemory, saved_competition_dollars_unit)) };
_Static_assert(offsetof(PTNamedMemory, saved_competition_dollars_unit) == PT_FIELD_competition_dollars_unit,
    "competition_dollars_unit address");
enum { PT_FIELD_competition_bet_prompt = (offsetof(PTNamedMemory, saved_competition_bet_prompt)) };
_Static_assert(offsetof(PTNamedMemory, saved_competition_bet_prompt) == PT_FIELD_competition_bet_prompt,
    "competition_bet_prompt address");
enum { PT_FIELD_competition_decline_text = (offsetof(PTNamedMemory, saved_competition_decline_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_competition_decline_text) == PT_FIELD_competition_decline_text,
    "competition_decline_text address");
enum { PT_FIELD_competition_invitation_format = (offsetof(PTNamedMemory, saved_competition_invitation_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_competition_invitation_format) == PT_FIELD_competition_invitation_format,
    "competition_invitation_format address");
enum { PT_FIELD_competition_stake_prompt = (offsetof(PTNamedMemory, saved_competition_stake_prompt)) };
_Static_assert(offsetof(PTNamedMemory, saved_competition_stake_prompt) == PT_FIELD_competition_stake_prompt,
    "competition_stake_prompt address");
enum { PT_FIELD_competition_no_unit = (offsetof(PTNamedMemory, saved_competition_no_unit)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_competition_no_unit) == PT_FIELD_competition_no_unit, "competition_no_unit address");
enum { PT_FIELD_competition_speed_prompt = (offsetof(PTNamedMemory, saved_competition_speed_prompt)) };
_Static_assert(offsetof(PTNamedMemory, saved_competition_speed_prompt) == PT_FIELD_competition_speed_prompt,
    "competition_speed_prompt address");
enum { PT_FIELD_competition_jury_caption = (offsetof(PTNamedMemory, saved_competition_jury_caption)) };
_Static_assert(offsetof(PTNamedMemory, saved_competition_jury_caption) == PT_FIELD_competition_jury_caption,
    "competition_jury_caption address");
enum { PT_FIELD_competition_time_format = (offsetof(PTNamedMemory, saved_competition_time_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_competition_time_format) == PT_FIELD_competition_time_format,
    "competition_time_format address");
enum { PT_FIELD_competition_draw_caption = (offsetof(PTNamedMemory, saved_competition_draw_caption)) };
_Static_assert(offsetof(PTNamedMemory, saved_competition_draw_caption) == PT_FIELD_competition_draw_caption,
    "competition_draw_caption address");
enum { PT_FIELD_competition_win_caption = (offsetof(PTNamedMemory, saved_competition_win_caption)) };
_Static_assert(offsetof(PTNamedMemory, saved_competition_win_caption) == PT_FIELD_competition_win_caption,
    "competition_win_caption address");
enum { PT_FIELD_competition_minimum_money = (offsetof(PTNamedMemory, saved_competition_minimum_money)) };
_Static_assert(offsetof(PTNamedMemory, saved_competition_minimum_money) == PT_FIELD_competition_minimum_money,
    "competition_minimum_money address");
enum { PT_FIELD_competition_maximum_bet = (offsetof(PTNamedMemory, saved_competition_maximum_bet)) };
_Static_assert(offsetof(PTNamedMemory, saved_competition_maximum_bet) == PT_FIELD_competition_maximum_bet,
    "competition_maximum_bet address");
enum { PT_FIELD_competition_ai_money_fraction = (offsetof(PTNamedMemory, saved_competition_ai_money_fraction)) };
_Static_assert(offsetof(PTNamedMemory, saved_competition_ai_money_fraction) == PT_FIELD_competition_ai_money_fraction,
    "competition_ai_money_fraction address");
enum { PT_FIELD_recipe_new_name = (offsetof(PTNamedMemory, saved_recipe_new_name)) };
_Static_assert(offsetof(PTNamedMemory, saved_recipe_new_name) == PT_FIELD_recipe_new_name, "recipe_new_name address");
enum { PT_FIELD_recipe_number_format = (offsetof(PTNamedMemory, saved_recipe_number_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_recipe_number_format) == PT_FIELD_recipe_number_format,
    "recipe_number_format address");
enum { PT_FIELD_recipe_calories_format = (offsetof(PTNamedMemory, saved_recipe_calories_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_recipe_calories_format) == PT_FIELD_recipe_calories_format,
    "recipe_calories_format address");
enum { PT_FIELD_recipe_fat_format = (offsetof(PTNamedMemory, saved_recipe_fat_format)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_recipe_fat_format) == PT_FIELD_recipe_fat_format, "recipe_fat_format address");
enum { PT_FIELD_recipe_sweet_format = (offsetof(PTNamedMemory, saved_recipe_sweet_format)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_recipe_sweet_format) == PT_FIELD_recipe_sweet_format, "recipe_sweet_format address");
enum { PT_FIELD_recipe_salty_format = (offsetof(PTNamedMemory, saved_recipe_salty_format)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_recipe_salty_format) == PT_FIELD_recipe_salty_format, "recipe_salty_format address");
enum { PT_FIELD_recipe_hot_format = (offsetof(PTNamedMemory, saved_recipe_hot_format)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_recipe_hot_format) == PT_FIELD_recipe_hot_format, "recipe_hot_format address");
enum { PT_FIELD_recipe_appearance_format = (offsetof(PTNamedMemory, saved_recipe_appearance_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_recipe_appearance_format) == PT_FIELD_recipe_appearance_format,
    "recipe_appearance_format address");
enum { PT_FIELD_recipe_refinement_format = (offsetof(PTNamedMemory, saved_recipe_refinement_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_recipe_refinement_format) == PT_FIELD_recipe_refinement_format,
    "recipe_refinement_format address");
enum { PT_FIELD_recipe_trend_format = (offsetof(PTNamedMemory, saved_recipe_trend_format)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_recipe_trend_format) == PT_FIELD_recipe_trend_format, "recipe_trend_format address");
enum { PT_FIELD_recipe_imitation_format = (offsetof(PTNamedMemory, saved_recipe_imitation_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_recipe_imitation_format) == PT_FIELD_recipe_imitation_format,
    "recipe_imitation_format address");
enum { PT_FIELD_recipe_normality_format = (offsetof(PTNamedMemory, saved_recipe_normality_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_recipe_normality_format) == PT_FIELD_recipe_normality_format,
    "recipe_normality_format address");
enum { PT_FIELD_recipe_jury_filename = (offsetof(PTNamedMemory, saved_recipe_jury_filename)) };
_Static_assert(offsetof(PTNamedMemory, saved_recipe_jury_filename) == PT_FIELD_recipe_jury_filename,
    "recipe_jury_filename address");
enum { PT_FIELD_recipe_jury_compare_text = (offsetof(PTNamedMemory, saved_recipe_jury_compare_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_recipe_jury_compare_text) == PT_FIELD_recipe_jury_compare_text,
    "recipe_jury_compare_text address");
enum { PT_FIELD_recipe_jury_evaluate_text = (offsetof(PTNamedMemory, saved_recipe_jury_evaluate_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_recipe_jury_evaluate_text) == PT_FIELD_recipe_jury_evaluate_text,
    "recipe_jury_evaluate_text address");
enum { PT_FIELD_recipe_jury_competition_text = (offsetof(PTNamedMemory, saved_recipe_jury_competition_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_recipe_jury_competition_text) == PT_FIELD_recipe_jury_competition_text,
    "recipe_jury_competition_text address");
enum { PT_FIELD_recipe_jury_total_text = (offsetof(PTNamedMemory, saved_recipe_jury_total_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_recipe_jury_total_text) == PT_FIELD_recipe_jury_total_text,
    "recipe_jury_total_text address");
enum { PT_FIELD_recipe_jury_percent_format = (offsetof(PTNamedMemory, saved_recipe_jury_percent_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_recipe_jury_percent_format) == PT_FIELD_recipe_jury_percent_format,
    "recipe_jury_percent_format address");
enum { PT_FIELD_recipe_jury_book_hint_format = (offsetof(PTNamedMemory, saved_recipe_jury_book_hint_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_recipe_jury_book_hint_format) == PT_FIELD_recipe_jury_book_hint_format,
    "recipe_jury_book_hint_format address");
enum { PT_FIELD_recipe_view_info_text = (offsetof(PTNamedMemory, saved_recipe_view_info_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_recipe_view_info_text) == PT_FIELD_recipe_view_info_text,
    "recipe_view_info_text address");
enum { PT_FIELD_recipe_view_recipe_text = (offsetof(PTNamedMemory, saved_recipe_view_recipe_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_recipe_view_recipe_text) == PT_FIELD_recipe_view_recipe_text,
    "recipe_view_recipe_text address");
enum { PT_FIELD_recipe_view_here_text = (offsetof(PTNamedMemory, saved_recipe_view_here_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_recipe_view_here_text) == PT_FIELD_recipe_view_here_text,
    "recipe_view_here_text address");
enum { PT_FIELD_recipe_view_all_text = (offsetof(PTNamedMemory, saved_recipe_view_all_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_recipe_view_all_text) == PT_FIELD_recipe_view_all_text,
    "recipe_view_all_text address");
enum { PT_FIELD_recipe_view_costs_text = (offsetof(PTNamedMemory, saved_recipe_view_costs_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_recipe_view_costs_text) == PT_FIELD_recipe_view_costs_text,
    "recipe_view_costs_text address");
enum { PT_FIELD_recipe_view_price_text = (offsetof(PTNamedMemory, saved_recipe_view_price_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_recipe_view_price_text) == PT_FIELD_recipe_view_price_text,
    "recipe_view_price_text address");
enum { PT_FIELD_recipe_view_sales_text = (offsetof(PTNamedMemory, saved_recipe_view_sales_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_recipe_view_sales_text) == PT_FIELD_recipe_view_sales_text,
    "recipe_view_sales_text address");
enum { PT_FIELD_recipe_view_profit_text = (offsetof(PTNamedMemory, saved_recipe_view_profit_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_recipe_view_profit_text) == PT_FIELD_recipe_view_profit_text,
    "recipe_view_profit_text address");
enum { PT_FIELD_recipe_view_popularity_text = (offsetof(PTNamedMemory, saved_recipe_view_popularity_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_recipe_view_popularity_text) == PT_FIELD_recipe_view_popularity_text,
    "recipe_view_popularity_text address");
enum { PT_FIELD_recipe_view_gram_text = (offsetof(PTNamedMemory, saved_recipe_view_gram_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_recipe_view_gram_text) == PT_FIELD_recipe_view_gram_text,
    "recipe_view_gram_text address");
enum { PT_FIELD_recipe_view_total_weight_text = (offsetof(PTNamedMemory, saved_recipe_view_total_weight_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_recipe_view_total_weight_text) == PT_FIELD_recipe_view_total_weight_text,
    "recipe_view_total_weight_text address");
enum {
  PT_FIELD_recipe_view_ingredient_count_format = (offsetof(PTNamedMemory, saved_recipe_view_ingredient_count_format))
};
_Static_assert(
    offsetof(PTNamedMemory, saved_recipe_view_ingredient_count_format) == PT_FIELD_recipe_view_ingredient_count_format,
    "recipe_view_ingredient_count_format address");
enum { PT_FIELD_pizza_competition_title = (offsetof(PTNamedMemory, saved_pizza_competition_title)) };
_Static_assert(offsetof(PTNamedMemory, saved_pizza_competition_title) == PT_FIELD_pizza_competition_title,
    "pizza_competition_title address");
enum { PT_FIELD_pizza_competition_separator = (offsetof(PTNamedMemory, saved_pizza_competition_separator)) };
_Static_assert(offsetof(PTNamedMemory, saved_pizza_competition_separator) == PT_FIELD_pizza_competition_separator,
    "pizza_competition_separator address");
enum { PT_FIELD_pizza_competition_original_label = (offsetof(PTNamedMemory, saved_pizza_competition_original_label)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_pizza_competition_original_label) == PT_FIELD_pizza_competition_original_label,
    "pizza_competition_original_label address");
enum { PT_FIELD_pizza_competition_level_label = (offsetof(PTNamedMemory, saved_pizza_competition_level_label)) };
_Static_assert(offsetof(PTNamedMemory, saved_pizza_competition_level_label) == PT_FIELD_pizza_competition_level_label,
    "pizza_competition_level_label address");
enum {
  PT_FIELD_pizza_competition_ingredient_label = (offsetof(PTNamedMemory, saved_pizza_competition_ingredient_label))
};
_Static_assert(
    offsetof(PTNamedMemory, saved_pizza_competition_ingredient_label) == PT_FIELD_pizza_competition_ingredient_label,
    "pizza_competition_ingredient_label address");
enum { PT_FIELD_pizza_competition_ready_text = (offsetof(PTNamedMemory, saved_pizza_competition_ready_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_pizza_competition_ready_text) == PT_FIELD_pizza_competition_ready_text,
    "pizza_competition_ready_text address");
enum { PT_FIELD_ingredient_grammes_format = (offsetof(PTNamedMemory, saved_ingredient_grammes_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_ingredient_grammes_format) == PT_FIELD_ingredient_grammes_format,
    "ingredient_grammes_format address");
enum { PT_FIELD_ingredient_calories_format = (offsetof(PTNamedMemory, saved_ingredient_calories_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_ingredient_calories_format) == PT_FIELD_ingredient_calories_format,
    "ingredient_calories_format address");
enum { PT_FIELD_ingredient_intensity_format = (offsetof(PTNamedMemory, saved_ingredient_intensity_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_ingredient_intensity_format) == PT_FIELD_ingredient_intensity_format,
    "ingredient_intensity_format address");
enum { PT_FIELD_ingredient_fat_format = (offsetof(PTNamedMemory, saved_ingredient_fat_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_ingredient_fat_format) == PT_FIELD_ingredient_fat_format,
    "ingredient_fat_format address");
enum { PT_FIELD_ingredient_sweet_format = (offsetof(PTNamedMemory, saved_ingredient_sweet_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_ingredient_sweet_format) == PT_FIELD_ingredient_sweet_format,
    "ingredient_sweet_format address");
enum { PT_FIELD_ingredient_salty_format = (offsetof(PTNamedMemory, saved_ingredient_salty_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_ingredient_salty_format) == PT_FIELD_ingredient_salty_format,
    "ingredient_salty_format address");
enum { PT_FIELD_ingredient_hot_format = (offsetof(PTNamedMemory, saved_ingredient_hot_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_ingredient_hot_format) == PT_FIELD_ingredient_hot_format,
    "ingredient_hot_format address");
enum { PT_FIELD_ingredient_trend_format = (offsetof(PTNamedMemory, saved_ingredient_trend_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_ingredient_trend_format) == PT_FIELD_ingredient_trend_format,
    "ingredient_trend_format address");
enum { PT_FIELD_ingredient_trend_name_format = (offsetof(PTNamedMemory, saved_ingredient_trend_name_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_ingredient_trend_name_format) == PT_FIELD_ingredient_trend_name_format,
    "ingredient_trend_name_format address");
enum { PT_FIELD_ingredients_full_text = (offsetof(PTNamedMemory, saved_ingredients_full_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_ingredients_full_text) == PT_FIELD_ingredients_full_text,
    "ingredients_full_text address");
enum { PT_FIELD_recipe_editor_greeting_format = (offsetof(PTNamedMemory, saved_recipe_editor_greeting_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_recipe_editor_greeting_format) == PT_FIELD_recipe_editor_greeting_format,
    "recipe_editor_greeting_format address");
enum { PT_FIELD_recipe_name_prompt = (offsetof(PTNamedMemory, saved_recipe_name_prompt)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_recipe_name_prompt) == PT_FIELD_recipe_name_prompt, "recipe_name_prompt address");
enum { PT_FIELD_recipe_cost_divisor = (offsetof(PTNamedMemory, saved_recipe_cost_divisor)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_recipe_cost_divisor) == PT_FIELD_recipe_cost_divisor, "recipe_cost_divisor address");
enum { PT_FIELD_disk_menu_caption = (offsetof(PTNamedMemory, saved_disk_menu_caption)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_disk_menu_caption) == PT_FIELD_disk_menu_caption, "disk_menu_caption address");
enum { PT_FIELD_disk_quit_prompt = (offsetof(PTNamedMemory, saved_disk_quit_prompt)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_disk_quit_prompt) == PT_FIELD_disk_quit_prompt, "disk_quit_prompt address");
enum {
  PT_FIELD_character_selection_level_caption = (offsetof(PTNamedMemory, saved_character_selection_level_caption))
};
_Static_assert(
    offsetof(PTNamedMemory, saved_character_selection_level_caption) == PT_FIELD_character_selection_level_caption,
    "character_selection_level_caption address");
enum { PT_FIELD_highscores_filename = (offsetof(PTNamedMemory, saved_highscores_filename)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_highscores_filename) == PT_FIELD_highscores_filename, "highscores_filename address");
enum { PT_FIELD_options_on_label = (offsetof(PTNamedMemory, saved_options_on_label)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_options_on_label) == PT_FIELD_options_on_label, "options_on_label address");
enum { PT_FIELD_options_off_label = (offsetof(PTNamedMemory, saved_options_off_label)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_options_off_label) == PT_FIELD_options_off_label, "options_off_label address");
enum { PT_FIELD_options_prefs_filename = (offsetof(PTNamedMemory, saved_options_prefs_filename)) };
_Static_assert(offsetof(PTNamedMemory, saved_options_prefs_filename) == PT_FIELD_options_prefs_filename,
    "options_prefs_filename address");
enum { PT_FIELD_character_selection_money_limit = (offsetof(PTNamedMemory, saved_character_selection_money_limit)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_character_selection_money_limit) == PT_FIELD_character_selection_money_limit,
    "character_selection_money_limit address");
enum { PT_FIELD_mission_date_format = (offsetof(PTNamedMemory, saved_mission_date_format)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_mission_date_format) == PT_FIELD_mission_date_format, "mission_date_format address");
enum { PT_FIELD_mission_selection_caption = (offsetof(PTNamedMemory, saved_mission_selection_caption)) };
_Static_assert(offsetof(PTNamedMemory, saved_mission_selection_caption) == PT_FIELD_mission_selection_caption,
    "mission_selection_caption address");
enum { PT_FIELD_mission_filename_format = (offsetof(PTNamedMemory, saved_mission_filename_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_mission_filename_format) == PT_FIELD_mission_filename_format,
    "mission_filename_format address");
enum { PT_FIELD_main_menu_background_filename = (offsetof(PTNamedMemory, saved_main_menu_background_filename)) };
_Static_assert(offsetof(PTNamedMemory, saved_main_menu_background_filename) == PT_FIELD_main_menu_background_filename,
    "main_menu_background_filename address");
enum { PT_FIELD_game_screen_background_filename = (offsetof(PTNamedMemory, saved_game_screen_background_filename)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_game_screen_background_filename) == PT_FIELD_game_screen_background_filename,
    "game_screen_background_filename address");
enum { PT_FIELD_selector_save_label = (offsetof(PTNamedMemory, saved_selector_save_label)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_selector_save_label) == PT_FIELD_selector_save_label, "selector_save_label address");
enum { PT_FIELD_selector_load_label = (offsetof(PTNamedMemory, saved_selector_load_label)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_selector_load_label) == PT_FIELD_selector_load_label, "selector_load_label address");
enum { PT_FIELD_selector_title_format = (offsetof(PTNamedMemory, saved_selector_title_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_selector_title_format) == PT_FIELD_selector_title_format,
    "selector_title_format address");
enum { PT_FIELD_selector_game_directory = (offsetof(PTNamedMemory, saved_selector_game_directory)) };
_Static_assert(offsetof(PTNamedMemory, saved_selector_game_directory) == PT_FIELD_selector_game_directory,
    "selector_game_directory address");
enum { PT_FIELD_selector_no_games = (offsetof(PTNamedMemory, saved_selector_no_games)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_selector_no_games) == PT_FIELD_selector_no_games, "selector_no_games address");
enum { PT_FIELD_selector_no_characters = (offsetof(PTNamedMemory, saved_selector_no_characters)) };
_Static_assert(offsetof(PTNamedMemory, saved_selector_no_characters) == PT_FIELD_selector_no_characters,
    "selector_no_characters address");
enum { PT_FIELD_selector_no_missions = (offsetof(PTNamedMemory, saved_selector_no_missions)) };
_Static_assert(offsetof(PTNamedMemory, saved_selector_no_missions) == PT_FIELD_selector_no_missions,
    "selector_no_missions address");
enum { PT_FIELD_selector_new_name = (offsetof(PTNamedMemory, saved_selector_new_name)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_selector_new_name) == PT_FIELD_selector_new_name, "selector_new_name address");
enum { PT_FIELD_selector_filename_prefix = (offsetof(PTNamedMemory, saved_selector_filename_prefix)) };
_Static_assert(offsetof(PTNamedMemory, saved_selector_filename_prefix) == PT_FIELD_selector_filename_prefix,
    "selector_filename_prefix address");
enum { PT_FIELD_selector_error_name = (offsetof(PTNamedMemory, saved_selector_error_name)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_selector_error_name) == PT_FIELD_selector_error_name, "selector_error_name address");
enum { PT_FIELD_selector_overwrite_prompt = (offsetof(PTNamedMemory, saved_selector_overwrite_prompt)) };
_Static_assert(offsetof(PTNamedMemory, saved_selector_overwrite_prompt) == PT_FIELD_selector_overwrite_prompt,
    "selector_overwrite_prompt address");
enum { PT_FIELD_player_stats_money_label = (offsetof(PTNamedMemory, saved_player_stats_money_label)) };
_Static_assert(offsetof(PTNamedMemory, saved_player_stats_money_label) == PT_FIELD_player_stats_money_label,
    "player_stats_money_label address");
enum { PT_FIELD_player_stats_age_label = (offsetof(PTNamedMemory, saved_player_stats_age_label)) };
_Static_assert(offsetof(PTNamedMemory, saved_player_stats_age_label) == PT_FIELD_player_stats_age_label,
    "player_stats_age_label address");
enum { PT_FIELD_player_stats_popularity_label = (offsetof(PTNamedMemory, saved_player_stats_popularity_label)) };
_Static_assert(offsetof(PTNamedMemory, saved_player_stats_popularity_label) == PT_FIELD_player_stats_popularity_label,
    "player_stats_popularity_label address");
enum { PT_FIELD_player_stats_health_label = (offsetof(PTNamedMemory, saved_player_stats_health_label)) };
_Static_assert(offsetof(PTNamedMemory, saved_player_stats_health_label) == PT_FIELD_player_stats_health_label,
    "player_stats_health_label address");
enum { PT_FIELD_player_stats_energy_label = (offsetof(PTNamedMemory, saved_player_stats_energy_label)) };
_Static_assert(offsetof(PTNamedMemory, saved_player_stats_energy_label) == PT_FIELD_player_stats_energy_label,
    "player_stats_energy_label address");
enum { PT_FIELD_player_stats_intelligence_label = (offsetof(PTNamedMemory, saved_player_stats_intelligence_label)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_player_stats_intelligence_label) == PT_FIELD_player_stats_intelligence_label,
    "player_stats_intelligence_label address");
enum { PT_FIELD_player_stats_charisma_label = (offsetof(PTNamedMemory, saved_player_stats_charisma_label)) };
_Static_assert(offsetof(PTNamedMemory, saved_player_stats_charisma_label) == PT_FIELD_player_stats_charisma_label,
    "player_stats_charisma_label address");
enum { PT_FIELD_player_stats_dexterity_label = (offsetof(PTNamedMemory, saved_player_stats_dexterity_label)) };
_Static_assert(offsetof(PTNamedMemory, saved_player_stats_dexterity_label) == PT_FIELD_player_stats_dexterity_label,
    "player_stats_dexterity_label address");
enum { PT_FIELD_player_stats_personality_label = (offsetof(PTNamedMemory, saved_player_stats_personality_label)) };
_Static_assert(offsetof(PTNamedMemory, saved_player_stats_personality_label) == PT_FIELD_player_stats_personality_label,
    "player_stats_personality_label address");
enum { PT_FIELD_player_stats_cooking_label = (offsetof(PTNamedMemory, saved_player_stats_cooking_label)) };
_Static_assert(offsetof(PTNamedMemory, saved_player_stats_cooking_label) == PT_FIELD_player_stats_cooking_label,
    "player_stats_cooking_label address");
enum { PT_FIELD_player_stats_courage_label = (offsetof(PTNamedMemory, saved_player_stats_courage_label)) };
_Static_assert(offsetof(PTNamedMemory, saved_player_stats_courage_label) == PT_FIELD_player_stats_courage_label,
    "player_stats_courage_label address");
enum { PT_FIELD_attack_action_caption = (offsetof(PTNamedMemory, saved_attack_action_caption)) };
_Static_assert(offsetof(PTNamedMemory, saved_attack_action_caption) == PT_FIELD_attack_action_caption,
    "attack_action_caption address");
enum { PT_FIELD_attack_weapon_caption = (offsetof(PTNamedMemory, saved_attack_weapon_caption)) };
_Static_assert(offsetof(PTNamedMemory, saved_attack_weapon_caption) == PT_FIELD_attack_weapon_caption,
    "attack_weapon_caption address");
enum { PT_FIELD_mission_invalid_place = (offsetof(PTNamedMemory, saved_mission_invalid_place)) };
_Static_assert(offsetof(PTNamedMemory, saved_mission_invalid_place) == PT_FIELD_mission_invalid_place,
    "mission_invalid_place address");
enum { PT_FIELD_poi_info_formats = (offsetof(PTNamedMemory, saved_poi_info_formats)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_poi_info_formats) == PT_FIELD_poi_info_formats, "poi_info_formats address");
enum { PT_FIELD_poi_realtor_text = (offsetof(PTNamedMemory, saved_poi_realtor_text)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_poi_realtor_text) == PT_FIELD_poi_realtor_text, "poi_realtor_text address");
enum { PT_FIELD_closed_restaurant_text = (offsetof(PTNamedMemory, saved_closed_restaurant_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_closed_restaurant_text) == PT_FIELD_closed_restaurant_text,
    "closed_restaurant_text address");
enum { PT_FIELD_creating_map_text = (offsetof(PTNamedMemory, saved_creating_map_text)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_creating_map_text) == PT_FIELD_creating_map_text, "creating_map_text address");
enum { PT_FIELD_maestro_background_filename = (offsetof(PTNamedMemory, saved_maestro_background_filename)) };
_Static_assert(offsetof(PTNamedMemory, saved_maestro_background_filename) == PT_FIELD_maestro_background_filename,
    "maestro_background_filename address");
enum { PT_FIELD_game_over_retry_prompt = (offsetof(PTNamedMemory, saved_game_over_retry_prompt)) };
_Static_assert(offsetof(PTNamedMemory, saved_game_over_retry_prompt) == PT_FIELD_game_over_retry_prompt,
    "game_over_retry_prompt address");
enum { PT_FIELD_high_score_award_text = (offsetof(PTNamedMemory, saved_high_score_award_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_high_score_award_text) == PT_FIELD_high_score_award_text,
    "high_score_award_text address");
enum { PT_FIELD_high_score_name_format = (offsetof(PTNamedMemory, saved_high_score_name_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_high_score_name_format) == PT_FIELD_high_score_name_format,
    "high_score_name_format address");
enum { PT_FIELD_high_score_name_prompt = (offsetof(PTNamedMemory, saved_high_score_name_prompt)) };
_Static_assert(offsetof(PTNamedMemory, saved_high_score_name_prompt) == PT_FIELD_high_score_name_prompt,
    "high_score_name_prompt address");
enum { PT_FIELD_personal_record_directory = (offsetof(PTNamedMemory, saved_personal_record_directory)) };
_Static_assert(offsetof(PTNamedMemory, saved_personal_record_directory) == PT_FIELD_personal_record_directory,
    "personal_record_directory address");
enum { PT_FIELD_personal_record_fallback = (offsetof(PTNamedMemory, saved_personal_record_fallback)) };
_Static_assert(offsetof(PTNamedMemory, saved_personal_record_fallback) == PT_FIELD_personal_record_fallback,
    "personal_record_fallback address");
enum { PT_FIELD_personal_record_save_prompt = (offsetof(PTNamedMemory, saved_personal_record_save_prompt)) };
_Static_assert(offsetof(PTNamedMemory, saved_personal_record_save_prompt) == PT_FIELD_personal_record_save_prompt,
    "personal_record_save_prompt address");
enum { PT_FIELD_game_over_low_score_text = (offsetof(PTNamedMemory, saved_game_over_low_score_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_game_over_low_score_text) == PT_FIELD_game_over_low_score_text,
    "game_over_low_score_text address");
enum { PT_FIELD_ending_background_filename = (offsetof(PTNamedMemory, saved_ending_background_filename)) };
_Static_assert(offsetof(PTNamedMemory, saved_ending_background_filename) == PT_FIELD_ending_background_filename,
    "ending_background_filename address");
enum { PT_FIELD_animated_quality_caption = (offsetof(PTNamedMemory, saved_animated_quality_caption)) };
_Static_assert(offsetof(PTNamedMemory, saved_animated_quality_caption) == PT_FIELD_animated_quality_caption,
    "animated_quality_caption address");
enum { PT_FIELD_animated_reputation_caption = (offsetof(PTNamedMemory, saved_animated_reputation_caption)) };
_Static_assert(offsetof(PTNamedMemory, saved_animated_reputation_caption) == PT_FIELD_animated_reputation_caption,
    "animated_reputation_caption address");
enum { PT_FIELD_animated_no_text = (offsetof(PTNamedMemory, saved_animated_no_text)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_animated_no_text) == PT_FIELD_animated_no_text, "animated_no_text address");
enum { PT_FIELD_animated_orders_text = (offsetof(PTNamedMemory, saved_animated_orders_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_animated_orders_text) == PT_FIELD_animated_orders_text,
    "animated_orders_text address");
enum { PT_FIELD_animated_output_caption = (offsetof(PTNamedMemory, saved_animated_output_caption)) };
_Static_assert(offsetof(PTNamedMemory, saved_animated_output_caption) == PT_FIELD_animated_output_caption,
    "animated_output_caption address");
enum { PT_FIELD_animated_day_minutes = (offsetof(PTNamedMemory, saved_animated_day_minutes)) };
_Static_assert(offsetof(PTNamedMemory, saved_animated_day_minutes) == PT_FIELD_animated_day_minutes,
    "animated_day_minutes address");
enum { PT_FIELD_animated_percent_scale = (offsetof(PTNamedMemory, saved_animated_percent_scale)) };
_Static_assert(offsetof(PTNamedMemory, saved_animated_percent_scale) == PT_FIELD_animated_percent_scale,
    "animated_percent_scale address");
enum { PT_FIELD_animated_demand_text = (offsetof(PTNamedMemory, saved_animated_demand_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_animated_demand_text) == PT_FIELD_animated_demand_text,
    "animated_demand_text address");
enum { PT_FIELD_animated_capacity_text = (offsetof(PTNamedMemory, saved_animated_capacity_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_animated_capacity_text) == PT_FIELD_animated_capacity_text,
    "animated_capacity_text address");
enum { PT_FIELD_animated_popularity_caption = (offsetof(PTNamedMemory, saved_animated_popularity_caption)) };
_Static_assert(offsetof(PTNamedMemory, saved_animated_popularity_caption) == PT_FIELD_animated_popularity_caption,
    "animated_popularity_caption address");
enum { PT_FIELD_animated_customers_caption = (offsetof(PTNamedMemory, saved_animated_customers_caption)) };
_Static_assert(offsetof(PTNamedMemory, saved_animated_customers_caption) == PT_FIELD_animated_customers_caption,
    "animated_customers_caption address");
enum { PT_FIELD_animated_takings_text = (offsetof(PTNamedMemory, saved_animated_takings_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_animated_takings_text) == PT_FIELD_animated_takings_text,
    "animated_takings_text address");
enum { PT_FIELD_animated_expenses_text = (offsetof(PTNamedMemory, saved_animated_expenses_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_animated_expenses_text) == PT_FIELD_animated_expenses_text,
    "animated_expenses_text address");
enum { PT_FIELD_realtor_sell_factor = (offsetof(PTNamedMemory, saved_realtor_sell_factor)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_realtor_sell_factor) == PT_FIELD_realtor_sell_factor, "realtor_sell_factor address");
enum { PT_FIELD_realtor_maximum_factor = (offsetof(PTNamedMemory, saved_realtor_maximum_factor)) };
_Static_assert(offsetof(PTNamedMemory, saved_realtor_maximum_factor) == PT_FIELD_realtor_maximum_factor,
    "realtor_maximum_factor address");
enum { PT_FIELD_realtor_discount_factor = (offsetof(PTNamedMemory, saved_realtor_discount_factor)) };
_Static_assert(offsetof(PTNamedMemory, saved_realtor_discount_factor) == PT_FIELD_realtor_discount_factor,
    "realtor_discount_factor address");
enum { PT_FIELD_realtor_increase_factor = (offsetof(PTNamedMemory, saved_realtor_increase_factor)) };
_Static_assert(offsetof(PTNamedMemory, saved_realtor_increase_factor) == PT_FIELD_realtor_increase_factor,
    "realtor_increase_factor address");
enum { PT_FIELD_bank_transfer_amount_text = (offsetof(PTNamedMemory, saved_bank_transfer_amount_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_bank_transfer_amount_text) == PT_FIELD_bank_transfer_amount_text,
    "bank_transfer_amount_text address");
enum { PT_FIELD_bank_transfer_account_text = (offsetof(PTNamedMemory, saved_bank_transfer_account_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_bank_transfer_account_text) == PT_FIELD_bank_transfer_account_text,
    "bank_transfer_account_text address");
enum { PT_FIELD_bank_loan_cash_limit = (offsetof(PTNamedMemory, saved_bank_loan_cash_limit)) };
_Static_assert(offsetof(PTNamedMemory, saved_bank_loan_cash_limit) == PT_FIELD_bank_loan_cash_limit,
    "bank_loan_cash_limit address");
enum { PT_FIELD_bank_investment_minimum = (offsetof(PTNamedMemory, saved_bank_investment_minimum)) };
_Static_assert(offsetof(PTNamedMemory, saved_bank_investment_minimum) == PT_FIELD_bank_investment_minimum,
    "bank_investment_minimum address");
enum { PT_FIELD_bank_investment_cash_limit = (offsetof(PTNamedMemory, saved_bank_investment_cash_limit)) };
_Static_assert(offsetof(PTNamedMemory, saved_bank_investment_cash_limit) == PT_FIELD_bank_investment_cash_limit,
    "bank_investment_cash_limit address");
enum { PT_FIELD_bank_investment_fraction = (offsetof(PTNamedMemory, saved_bank_investment_fraction)) };
_Static_assert(offsetof(PTNamedMemory, saved_bank_investment_fraction) == PT_FIELD_bank_investment_fraction,
    "bank_investment_fraction address");
enum { PT_FIELD_townhall_pair_format = (offsetof(PTNamedMemory, saved_townhall_pair_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_townhall_pair_format) == PT_FIELD_townhall_pair_format,
    "townhall_pair_format address");
enum { PT_FIELD_townhall_restaurant_name_format = (offsetof(PTNamedMemory, saved_townhall_restaurant_name_format)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_townhall_restaurant_name_format) == PT_FIELD_townhall_restaurant_name_format,
    "townhall_restaurant_name_format address");
enum {
  PT_FIELD_townhall_annual_pizza_announcement = (offsetof(PTNamedMemory, saved_townhall_annual_pizza_announcement))
};
_Static_assert(
    offsetof(PTNamedMemory, saved_townhall_annual_pizza_announcement) == PT_FIELD_townhall_annual_pizza_announcement,
    "townhall_annual_pizza_announcement address");
enum {
  PT_FIELD_townhall_annual_recipe_name_format = (offsetof(PTNamedMemory, saved_townhall_annual_recipe_name_format))
};
_Static_assert(
    offsetof(PTNamedMemory, saved_townhall_annual_recipe_name_format) == PT_FIELD_townhall_annual_recipe_name_format,
    "townhall_annual_recipe_name_format address");
enum { PT_FIELD_townhall_character_name_format = (offsetof(PTNamedMemory, saved_townhall_character_name_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_townhall_character_name_format) == PT_FIELD_townhall_character_name_format,
    "townhall_character_name_format address");
enum { PT_FIELD_townhall_name_format = (offsetof(PTNamedMemory, saved_townhall_name_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_townhall_name_format) == PT_FIELD_townhall_name_format,
    "townhall_name_format address");
enum { PT_FIELD_townhall_congratulations_format = (offsetof(PTNamedMemory, saved_townhall_congratulations_format)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_townhall_congratulations_format) == PT_FIELD_townhall_congratulations_format,
    "townhall_congratulations_format address");
enum {
  PT_FIELD_townhall_competition_account_format = (offsetof(PTNamedMemory, saved_townhall_competition_account_format))
};
_Static_assert(
    offsetof(PTNamedMemory, saved_townhall_competition_account_format) == PT_FIELD_townhall_competition_account_format,
    "townhall_competition_account_format address");
enum { PT_FIELD_townhall_mayor_date_format = (offsetof(PTNamedMemory, saved_townhall_mayor_date_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_townhall_mayor_date_format) == PT_FIELD_townhall_mayor_date_format,
    "townhall_mayor_date_format address");
enum { PT_FIELD_townhall_mayor_account_format = (offsetof(PTNamedMemory, saved_townhall_mayor_account_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_townhall_mayor_account_format) == PT_FIELD_townhall_mayor_account_format,
    "townhall_mayor_account_format address");
enum { PT_FIELD_terminal_new_number_prompt = (offsetof(PTNamedMemory, saved_terminal_new_number_prompt)) };
_Static_assert(offsetof(PTNamedMemory, saved_terminal_new_number_prompt) == PT_FIELD_terminal_new_number_prompt,
    "terminal_new_number_prompt address");
enum { PT_FIELD_terminal_no_entries = (offsetof(PTNamedMemory, saved_terminal_no_entries)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_terminal_no_entries) == PT_FIELD_terminal_no_entries, "terminal_no_entries address");
enum { PT_FIELD_terminal_title = (offsetof(PTNamedMemory, saved_terminal_title)) };
_Static_assert(offsetof(PTNamedMemory, saved_terminal_title) == PT_FIELD_terminal_title, "terminal_title address");
enum { PT_FIELD_terminal_loading_message = (offsetof(PTNamedMemory, saved_terminal_loading_message)) };
_Static_assert(offsetof(PTNamedMemory, saved_terminal_loading_message) == PT_FIELD_terminal_loading_message,
    "terminal_loading_message address");
enum { PT_FIELD_empty_string = (offsetof(PTNamedMemory, saved_empty_string)) };
_Static_assert(offsetof(PTNamedMemory, saved_empty_string) == PT_FIELD_empty_string, "empty_string address");
enum { PT_FIELD_mission_completion_phone_text = (offsetof(PTNamedMemory, saved_mission_completion_phone_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_mission_completion_phone_text) == PT_FIELD_mission_completion_phone_text,
    "mission_completion_phone_text address");
enum { PT_FIELD_mission_date_phrase_format = (offsetof(PTNamedMemory, saved_mission_date_phrase_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_mission_date_phrase_format) == PT_FIELD_mission_date_phrase_format,
    "mission_date_phrase_format address");
enum { PT_FIELD_mission_place_format = (offsetof(PTNamedMemory, saved_mission_place_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_mission_place_format) == PT_FIELD_mission_place_format,
    "mission_place_format address");
enum { PT_FIELD_mission_pickup_format = (offsetof(PTNamedMemory, saved_mission_pickup_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_mission_pickup_format) == PT_FIELD_mission_pickup_format,
    "mission_pickup_format address");
enum { PT_FIELD_mission_contamination_format = (offsetof(PTNamedMemory, saved_mission_contamination_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_mission_contamination_format) == PT_FIELD_mission_contamination_format,
    "mission_contamination_format address");
enum { PT_FIELD_mission_damage_format = (offsetof(PTNamedMemory, saved_mission_damage_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_mission_damage_format) == PT_FIELD_mission_damage_format,
    "mission_damage_format address");
enum { PT_FIELD_mission_laundering_format = (offsetof(PTNamedMemory, saved_mission_laundering_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_mission_laundering_format) == PT_FIELD_mission_laundering_format,
    "mission_laundering_format address");
enum { PT_FIELD_mission_delivery_format = (offsetof(PTNamedMemory, saved_mission_delivery_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_mission_delivery_format) == PT_FIELD_mission_delivery_format,
    "mission_delivery_format address");
enum { PT_FIELD_mission_deal_format = (offsetof(PTNamedMemory, saved_mission_deal_format)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_mission_deal_format) == PT_FIELD_mission_deal_format, "mission_deal_format address");
enum { PT_FIELD_mafia_failed_business_text = (offsetof(PTNamedMemory, saved_mafia_failed_business_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_mafia_failed_business_text) == PT_FIELD_mafia_failed_business_text,
    "mafia_failed_business_text address");
enum { PT_FIELD_mafia_protection_format = (offsetof(PTNamedMemory, saved_mafia_protection_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_mafia_protection_format) == PT_FIELD_mafia_protection_format,
    "mafia_protection_format address");
enum { PT_FIELD_mission_reward_format = (offsetof(PTNamedMemory, saved_mission_reward_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_mission_reward_format) == PT_FIELD_mission_reward_format,
    "mission_reward_format address");
enum { PT_FIELD_mission_accept_format = (offsetof(PTNamedMemory, saved_mission_accept_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_mission_accept_format) == PT_FIELD_mission_accept_format,
    "mission_accept_format address");
enum { PT_FIELD_mafia_existing_loan_format = (offsetof(PTNamedMemory, saved_mafia_existing_loan_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_mafia_existing_loan_format) == PT_FIELD_mafia_existing_loan_format,
    "mafia_existing_loan_format address");
enum { PT_FIELD_mafia_loan_offer_format = (offsetof(PTNamedMemory, saved_mafia_loan_offer_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_mafia_loan_offer_format) == PT_FIELD_mafia_loan_offer_format,
    "mafia_loan_offer_format address");
enum { PT_FIELD_mafia_tip_town_format = (offsetof(PTNamedMemory, saved_mafia_tip_town_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_mafia_tip_town_format) == PT_FIELD_mafia_tip_town_format,
    "mafia_tip_town_format address");
enum { PT_FIELD_mafia_weapon_tip_format = (offsetof(PTNamedMemory, saved_mafia_weapon_tip_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_mafia_weapon_tip_format) == PT_FIELD_mafia_weapon_tip_format,
    "mafia_weapon_tip_format address");
enum { PT_FIELD_mafia_dealer_tip_format = (offsetof(PTNamedMemory, saved_mafia_dealer_tip_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_mafia_dealer_tip_format) == PT_FIELD_mafia_dealer_tip_format,
    "mafia_dealer_tip_format address");
enum { PT_FIELD_mafia_tip_price_format = (offsetof(PTNamedMemory, saved_mafia_tip_price_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_mafia_tip_price_format) == PT_FIELD_mafia_tip_price_format,
    "mafia_tip_price_format address");
enum { PT_FIELD_mafia_cost_cash_scale = (offsetof(PTNamedMemory, saved_mafia_cost_cash_scale)) };
_Static_assert(offsetof(PTNamedMemory, saved_mafia_cost_cash_scale) == PT_FIELD_mafia_cost_cash_scale,
    "mafia_cost_cash_scale address");
enum { PT_FIELD_mafia_cost_scale_limit = (offsetof(PTNamedMemory, saved_mafia_cost_scale_limit)) };
_Static_assert(offsetof(PTNamedMemory, saved_mafia_cost_scale_limit) == PT_FIELD_mafia_cost_scale_limit,
    "mafia_cost_scale_limit address");
enum { PT_FIELD_invoice_title = (offsetof(PTNamedMemory, saved_invoice_title)) };
_Static_assert(offsetof(PTNamedMemory, saved_invoice_title) == PT_FIELD_invoice_title, "invoice_title address");
enum { PT_FIELD_invoice_name_format = (offsetof(PTNamedMemory, saved_invoice_name_format)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_invoice_name_format) == PT_FIELD_invoice_name_format, "invoice_name_format address");
enum { PT_FIELD_background_filename_format = (offsetof(PTNamedMemory, saved_background_filename_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_background_filename_format) == PT_FIELD_background_filename_format,
    "background_filename_format address");
enum { PT_FIELD_price_decimal_format = (offsetof(PTNamedMemory, saved_price_decimal_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_price_decimal_format) == PT_FIELD_price_decimal_format,
    "price_decimal_format address");
enum { PT_FIELD_price_display_divisor = (offsetof(PTNamedMemory, saved_price_display_divisor)) };
_Static_assert(offsetof(PTNamedMemory, saved_price_display_divisor) == PT_FIELD_price_display_divisor,
    "price_display_divisor address");
enum { PT_FIELD_list_percent_text = (offsetof(PTNamedMemory, saved_list_percent_text)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_list_percent_text) == PT_FIELD_list_percent_text, "list_percent_text address");
enum { PT_FIELD_dollar_alignment_text = (offsetof(PTNamedMemory, saved_dollar_alignment_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_dollar_alignment_text) == PT_FIELD_dollar_alignment_text,
    "dollar_alignment_text address");
enum { PT_FIELD_dollar_text_format = (offsetof(PTNamedMemory, saved_dollar_text_format)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_dollar_text_format) == PT_FIELD_dollar_text_format, "dollar_text_format address");
enum { PT_FIELD_integer_text_format = (offsetof(PTNamedMemory, saved_integer_text_format)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_integer_text_format) == PT_FIELD_integer_text_format, "integer_text_format address");
enum { PT_FIELD_advert_dollar = (offsetof(PTNamedMemory, saved_advert_dollar)) };
_Static_assert(offsetof(PTNamedMemory, saved_advert_dollar) == PT_FIELD_advert_dollar, "advert_dollar address");
enum { PT_FIELD_info_box_formats = (offsetof(PTNamedMemory, saved_info_box_formats)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_info_box_formats) == PT_FIELD_info_box_formats, "info_box_formats address");
enum { PT_FIELD_restaurant_name_format = (offsetof(PTNamedMemory, saved_restaurant_name_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_restaurant_name_format) == PT_FIELD_restaurant_name_format,
    "restaurant_name_format address");
enum { PT_FIELD_money_display_format = (offsetof(PTNamedMemory, saved_money_display_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_money_display_format) == PT_FIELD_money_display_format,
    "money_display_format address");
enum { PT_FIELD_ok_text = (offsetof(PTNamedMemory, saved_ok_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_ok_text) == PT_FIELD_ok_text, "ok_text address");
enum { PT_FIELD_yes_text = (offsetof(PTNamedMemory, saved_yes_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_yes_text) == PT_FIELD_yes_text, "yes_text address");
enum { PT_FIELD_no_text = (offsetof(PTNamedMemory, saved_no_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_no_text) == PT_FIELD_no_text, "no_text address");
enum { PT_FIELD_staff_disaster_question_format = (offsetof(PTNamedMemory, saved_staff_disaster_question_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_disaster_question_format) == PT_FIELD_staff_disaster_question_format,
    "staff_disaster_question_format address");
enum { PT_FIELD_number_query_format = (offsetof(PTNamedMemory, saved_number_query_format)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_number_query_format) == PT_FIELD_number_query_format, "number_query_format address");
enum { PT_FIELD_market_low = (offsetof(PTNamedMemory, saved_market_low)) };
_Static_assert(offsetof(PTNamedMemory, saved_market_low) == PT_FIELD_market_low, "market_low address");
enum { PT_FIELD_market_very_little = (offsetof(PTNamedMemory, saved_market_very_little)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_market_very_little) == PT_FIELD_market_very_little, "market_very_little address");
enum { PT_FIELD_market_little = (offsetof(PTNamedMemory, saved_market_little)) };
_Static_assert(offsetof(PTNamedMemory, saved_market_little) == PT_FIELD_market_little, "market_little address");
enum { PT_FIELD_market_average = (offsetof(PTNamedMemory, saved_market_average)) };
_Static_assert(offsetof(PTNamedMemory, saved_market_average) == PT_FIELD_market_average, "market_average address");
enum { PT_FIELD_market_above_average = (offsetof(PTNamedMemory, saved_market_above_average)) };
_Static_assert(offsetof(PTNamedMemory, saved_market_above_average) == PT_FIELD_market_above_average,
    "market_above_average address");
enum { PT_FIELD_market_high = (offsetof(PTNamedMemory, saved_market_high)) };
_Static_assert(offsetof(PTNamedMemory, saved_market_high) == PT_FIELD_market_high, "market_high address");
enum { PT_FIELD_market_very_high = (offsetof(PTNamedMemory, saved_market_very_high)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_market_very_high) == PT_FIELD_market_very_high, "market_very_high address");
enum { PT_FIELD_market_extremely_high = (offsetof(PTNamedMemory, saved_market_extremely_high)) };
_Static_assert(offsetof(PTNamedMemory, saved_market_extremely_high) == PT_FIELD_market_extremely_high,
    "market_extremely_high address");
enum { PT_FIELD_market_out_of_range = (offsetof(PTNamedMemory, saved_market_out_of_range)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_market_out_of_range) == PT_FIELD_market_out_of_range, "market_out_of_range address");
enum { PT_FIELD_decimal_scale_base = (offsetof(PTNamedMemory, saved_decimal_scale_base)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_decimal_scale_base) == PT_FIELD_decimal_scale_base, "decimal_scale_base address");
enum { PT_FIELD_market_percent_scale = (offsetof(PTNamedMemory, saved_market_percent_scale)) };
_Static_assert(offsetof(PTNamedMemory, saved_market_percent_scale) == PT_FIELD_market_percent_scale,
    "market_percent_scale address");
enum { PT_FIELD_market_premises_label = (offsetof(PTNamedMemory, saved_market_premises_label)) };
_Static_assert(offsetof(PTNamedMemory, saved_market_premises_label) == PT_FIELD_market_premises_label,
    "market_premises_label address");
enum { PT_FIELD_market_rent_label = (offsetof(PTNamedMemory, saved_market_rent_label)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_market_rent_label) == PT_FIELD_market_rent_label, "market_rent_label address");
enum { PT_FIELD_market_per_square_metre = (offsetof(PTNamedMemory, saved_market_per_square_metre)) };
_Static_assert(offsetof(PTNamedMemory, saved_market_per_square_metre) == PT_FIELD_market_per_square_metre,
    "market_per_square_metre address");
enum { PT_FIELD_market_price_label = (offsetof(PTNamedMemory, saved_market_price_label)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_market_price_label) == PT_FIELD_market_price_label, "market_price_label address");
enum { PT_FIELD_market_tendency_label = (offsetof(PTNamedMemory, saved_market_tendency_label)) };
_Static_assert(offsetof(PTNamedMemory, saved_market_tendency_label) == PT_FIELD_market_tendency_label,
    "market_tendency_label address");
enum { PT_FIELD_market_falling_label = (offsetof(PTNamedMemory, saved_market_falling_label)) };
_Static_assert(offsetof(PTNamedMemory, saved_market_falling_label) == PT_FIELD_market_falling_label,
    "market_falling_label address");
enum { PT_FIELD_market_rising_label = (offsetof(PTNamedMemory, saved_market_rising_label)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_market_rising_label) == PT_FIELD_market_rising_label, "market_rising_label address");
enum { PT_FIELD_market_average_size_label = (offsetof(PTNamedMemory, saved_market_average_size_label)) };
_Static_assert(offsetof(PTNamedMemory, saved_market_average_size_label) == PT_FIELD_market_average_size_label,
    "market_average_size_label address");
enum { PT_FIELD_market_square_metres = (offsetof(PTNamedMemory, saved_market_square_metres)) };
_Static_assert(offsetof(PTNamedMemory, saved_market_square_metres) == PT_FIELD_market_square_metres,
    "market_square_metres address");
enum { PT_FIELD_market_rooms_label = (offsetof(PTNamedMemory, saved_market_rooms_label)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_market_rooms_label) == PT_FIELD_market_rooms_label, "market_rooms_label address");
enum { PT_FIELD_market_to_rent_label = (offsetof(PTNamedMemory, saved_market_to_rent_label)) };
_Static_assert(offsetof(PTNamedMemory, saved_market_to_rent_label) == PT_FIELD_market_to_rent_label,
    "market_to_rent_label address");
enum { PT_FIELD_market_for_sale_label = (offsetof(PTNamedMemory, saved_market_for_sale_label)) };
_Static_assert(offsetof(PTNamedMemory, saved_market_for_sale_label) == PT_FIELD_market_for_sale_label,
    "market_for_sale_label address");
enum { PT_FIELD_market_not_available_label = (offsetof(PTNamedMemory, saved_market_not_available_label)) };
_Static_assert(offsetof(PTNamedMemory, saved_market_not_available_label) == PT_FIELD_market_not_available_label,
    "market_not_available_label address");
enum { PT_FIELD_market_taste_title = (offsetof(PTNamedMemory, saved_market_taste_title)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_market_taste_title) == PT_FIELD_market_taste_title, "market_taste_title address");
enum { PT_FIELD_market_rank_dash = (offsetof(PTNamedMemory, saved_market_rank_dash)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_market_rank_dash) == PT_FIELD_market_rank_dash, "market_rank_dash address");
enum { PT_FIELD_market_chain_title = (offsetof(PTNamedMemory, saved_market_chain_title)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_market_chain_title) == PT_FIELD_market_chain_title, "market_chain_title address");
enum { PT_FIELD_market_share_format = (offsetof(PTNamedMemory, saved_market_share_format)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_market_share_format) == PT_FIELD_market_share_format, "market_share_format address");
enum { PT_FIELD_market_turnover_format = (offsetof(PTNamedMemory, saved_market_turnover_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_market_turnover_format) == PT_FIELD_market_turnover_format,
    "market_turnover_format address");
enum { PT_FIELD_floor_zero_filename = (offsetof(PTNamedMemory, saved_floor_zero_filename)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_floor_zero_filename) == PT_FIELD_floor_zero_filename, "floor_zero_filename address");
enum { PT_FIELD_floor_filename_format = (offsetof(PTNamedMemory, saved_floor_filename_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_floor_filename_format) == PT_FIELD_floor_filename_format,
    "floor_filename_format address");
enum { PT_FIELD_furniture_inventory_num_text = (offsetof(PTNamedMemory, saved_furniture_inventory_num_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_furniture_inventory_num_text) == PT_FIELD_furniture_inventory_num_text,
    "furniture_inventory_num_text address");
enum { PT_FIELD_furniture_inventory_name_text = (offsetof(PTNamedMemory, saved_furniture_inventory_name_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_furniture_inventory_name_text) == PT_FIELD_furniture_inventory_name_text,
    "furniture_inventory_name_text address");
enum { PT_FIELD_furniture_inventory_format = (offsetof(PTNamedMemory, saved_furniture_inventory_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_furniture_inventory_format) == PT_FIELD_furniture_inventory_format,
    "furniture_inventory_format address");
enum { PT_FIELD_furniture_equipment_title = (offsetof(PTNamedMemory, saved_furniture_equipment_title)) };
_Static_assert(offsetof(PTNamedMemory, saved_furniture_equipment_title) == PT_FIELD_furniture_equipment_title,
    "furniture_equipment_title address");
enum { PT_FIELD_furniture_in_use_text = (offsetof(PTNamedMemory, saved_furniture_in_use_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_furniture_in_use_text) == PT_FIELD_furniture_in_use_text,
    "furniture_in_use_text address");
enum { PT_FIELD_furniture_value_text = (offsetof(PTNamedMemory, saved_furniture_value_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_furniture_value_text) == PT_FIELD_furniture_value_text,
    "furniture_value_text address");
enum { PT_FIELD_furniture_quality_text = (offsetof(PTNamedMemory, saved_furniture_quality_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_furniture_quality_text) == PT_FIELD_furniture_quality_text,
    "furniture_quality_text address");
enum { PT_FIELD_poi_capacity_text = (offsetof(PTNamedMemory, saved_poi_capacity_text)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_poi_capacity_text) == PT_FIELD_poi_capacity_text, "poi_capacity_text address");
enum { PT_FIELD_furniture_seats_text = (offsetof(PTNamedMemory, saved_furniture_seats_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_furniture_seats_text) == PT_FIELD_furniture_seats_text,
    "furniture_seats_text address");
enum { PT_FIELD_furniture_per_day_text = (offsetof(PTNamedMemory, saved_furniture_per_day_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_furniture_per_day_text) == PT_FIELD_furniture_per_day_text,
    "furniture_per_day_text address");
enum { PT_FIELD_furniture_kitchenware_title = (offsetof(PTNamedMemory, saved_furniture_kitchenware_title)) };
_Static_assert(offsetof(PTNamedMemory, saved_furniture_kitchenware_title) == PT_FIELD_furniture_kitchenware_title,
    "furniture_kitchenware_title address");
enum { PT_FIELD_furniture_ovens_text = (offsetof(PTNamedMemory, saved_furniture_ovens_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_furniture_ovens_text) == PT_FIELD_furniture_ovens_text,
    "furniture_ovens_text address");
enum { PT_FIELD_furniture_bake_time_text = (offsetof(PTNamedMemory, saved_furniture_bake_time_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_furniture_bake_time_text) == PT_FIELD_furniture_bake_time_text,
    "furniture_bake_time_text address");
enum { PT_FIELD_furniture_minutes_text = (offsetof(PTNamedMemory, saved_furniture_minutes_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_furniture_minutes_text) == PT_FIELD_furniture_minutes_text,
    "furniture_minutes_text address");
enum { PT_FIELD_furniture_baking_capacity_text = (offsetof(PTNamedMemory, saved_furniture_baking_capacity_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_furniture_baking_capacity_text) == PT_FIELD_furniture_baking_capacity_text,
    "furniture_baking_capacity_text address");
enum { PT_FIELD_furniture_number_text = (offsetof(PTNamedMemory, saved_furniture_number_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_furniture_number_text) == PT_FIELD_furniture_number_text,
    "furniture_number_text address");
enum { PT_FIELD_furniture_floor_title = (offsetof(PTNamedMemory, saved_furniture_floor_title)) };
_Static_assert(offsetof(PTNamedMemory, saved_furniture_floor_title) == PT_FIELD_furniture_floor_title,
    "furniture_floor_title address");
enum { PT_FIELD_furniture_tile_count_format = (offsetof(PTNamedMemory, saved_furniture_tile_count_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_furniture_tile_count_format) == PT_FIELD_furniture_tile_count_format,
    "furniture_tile_count_format address");
enum { PT_FIELD_furniture_area_format = (offsetof(PTNamedMemory, saved_furniture_area_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_furniture_area_format) == PT_FIELD_furniture_area_format,
    "furniture_area_format address");
enum { PT_FIELD_furniture_extras_title = (offsetof(PTNamedMemory, saved_furniture_extras_title)) };
_Static_assert(offsetof(PTNamedMemory, saved_furniture_extras_title) == PT_FIELD_furniture_extras_title,
    "furniture_extras_title address");
enum {
  PT_FIELD_furniture_floor_patterns_filename = (offsetof(PTNamedMemory, saved_furniture_floor_patterns_filename))
};
_Static_assert(
    offsetof(PTNamedMemory, saved_furniture_floor_patterns_filename) == PT_FIELD_furniture_floor_patterns_filename,
    "furniture_floor_patterns_filename address");
enum { PT_FIELD_furniture_floor_price_format = (offsetof(PTNamedMemory, saved_furniture_floor_price_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_furniture_floor_price_format) == PT_FIELD_furniture_floor_price_format,
    "furniture_floor_price_format address");
enum { PT_FIELD_furniture_floor_total_format = (offsetof(PTNamedMemory, saved_furniture_floor_total_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_furniture_floor_total_format) == PT_FIELD_furniture_floor_total_format,
    "furniture_floor_total_format address");
enum { PT_FIELD_furniture_supplier_page_format = (offsetof(PTNamedMemory, saved_furniture_supplier_page_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_furniture_supplier_page_format) == PT_FIELD_furniture_supplier_page_format,
    "furniture_supplier_page_format address");
enum { PT_FIELD_furniture_price_text = (offsetof(PTNamedMemory, saved_furniture_price_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_furniture_price_text) == PT_FIELD_furniture_price_text,
    "furniture_price_text address");
enum { PT_FIELD_furniture_capacity_format = (offsetof(PTNamedMemory, saved_furniture_capacity_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_furniture_capacity_format) == PT_FIELD_furniture_capacity_format,
    "furniture_capacity_format address");
enum { PT_FIELD_furniture_baking_time_format = (offsetof(PTNamedMemory, saved_furniture_baking_time_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_furniture_baking_time_format) == PT_FIELD_furniture_baking_time_format,
    "furniture_baking_time_format address");
enum { PT_FIELD_furniture_quality_format = (offsetof(PTNamedMemory, saved_furniture_quality_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_furniture_quality_format) == PT_FIELD_furniture_quality_format,
    "furniture_quality_format address");
enum { PT_FIELD_furniture_selection_limit_text = (offsetof(PTNamedMemory, saved_furniture_selection_limit_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_furniture_selection_limit_text) == PT_FIELD_furniture_selection_limit_text,
    "furniture_selection_limit_text address");
enum { PT_FIELD_furniture_invoice_caption = (offsetof(PTNamedMemory, saved_furniture_invoice_caption)) };
_Static_assert(offsetof(PTNamedMemory, saved_furniture_invoice_caption) == PT_FIELD_furniture_invoice_caption,
    "furniture_invoice_caption address");
enum { PT_FIELD_furniture_purchase_thanks = (offsetof(PTNamedMemory, saved_furniture_purchase_thanks)) };
_Static_assert(offsetof(PTNamedMemory, saved_furniture_purchase_thanks) == PT_FIELD_furniture_purchase_thanks,
    "furniture_purchase_thanks address");
enum { PT_FIELD_furniture_no_selection_text = (offsetof(PTNamedMemory, saved_furniture_no_selection_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_furniture_no_selection_text) == PT_FIELD_furniture_no_selection_text,
    "furniture_no_selection_text address");
enum { PT_FIELD_placement_maximum_ovens_text = (offsetof(PTNamedMemory, saved_placement_maximum_ovens_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_placement_maximum_ovens_text) == PT_FIELD_placement_maximum_ovens_text,
    "placement_maximum_ovens_text address");
enum { PT_FIELD_placement_maximum_items_text = (offsetof(PTNamedMemory, saved_placement_maximum_items_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_placement_maximum_items_text) == PT_FIELD_placement_maximum_items_text,
    "placement_maximum_items_text address");
enum { PT_FIELD_placement_closed_text = (offsetof(PTNamedMemory, saved_placement_closed_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_placement_closed_text) == PT_FIELD_placement_closed_text,
    "placement_closed_text address");
enum { PT_FIELD_placement_sell_format = (offsetof(PTNamedMemory, saved_placement_sell_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_placement_sell_format) == PT_FIELD_placement_sell_format,
    "placement_sell_format address");
enum { PT_FIELD_placement_invalid_text = (offsetof(PTNamedMemory, saved_placement_invalid_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_placement_invalid_text) == PT_FIELD_placement_invalid_text,
    "placement_invalid_text address");
enum { PT_FIELD_furniture_no_supplier_text = (offsetof(PTNamedMemory, saved_furniture_no_supplier_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_furniture_no_supplier_text) == PT_FIELD_furniture_no_supplier_text,
    "furniture_no_supplier_text address");
enum { PT_FIELD_furniture_suppliers_caption = (offsetof(PTNamedMemory, saved_furniture_suppliers_caption)) };
_Static_assert(offsetof(PTNamedMemory, saved_furniture_suppliers_caption) == PT_FIELD_furniture_suppliers_caption,
    "furniture_suppliers_caption address");
enum { PT_FIELD_customer_ratings_menu_title = (offsetof(PTNamedMemory, saved_customer_ratings_menu_title)) };
_Static_assert(offsetof(PTNamedMemory, saved_customer_ratings_menu_title) == PT_FIELD_customer_ratings_menu_title,
    "customer_ratings_menu_title address");
enum { PT_FIELD_customer_ratings_furniture_title = (offsetof(PTNamedMemory, saved_customer_ratings_furniture_title)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_customer_ratings_furniture_title) == PT_FIELD_customer_ratings_furniture_title,
    "customer_ratings_furniture_title address");
enum { PT_FIELD_customer_ratings_title = (offsetof(PTNamedMemory, saved_customer_ratings_title)) };
_Static_assert(offsetof(PTNamedMemory, saved_customer_ratings_title) == PT_FIELD_customer_ratings_title,
    "customer_ratings_title address");
enum { PT_FIELD_furniture_menu_caption = (offsetof(PTNamedMemory, saved_furniture_menu_caption)) };
_Static_assert(offsetof(PTNamedMemory, saved_furniture_menu_caption) == PT_FIELD_furniture_menu_caption,
    "furniture_menu_caption address");
enum { PT_FIELD_furniture_equipment_caption = (offsetof(PTNamedMemory, saved_furniture_equipment_caption)) };
_Static_assert(offsetof(PTNamedMemory, saved_furniture_equipment_caption) == PT_FIELD_furniture_equipment_caption,
    "furniture_equipment_caption address");
enum { PT_FIELD_furniture_van_warning = (offsetof(PTNamedMemory, saved_furniture_van_warning)) };
_Static_assert(offsetof(PTNamedMemory, saved_furniture_van_warning) == PT_FIELD_furniture_van_warning,
    "furniture_van_warning address");
enum { PT_FIELD_furniture_info_formats = (offsetof(PTNamedMemory, saved_furniture_info_formats)) };
_Static_assert(offsetof(PTNamedMemory, saved_furniture_info_formats) == PT_FIELD_furniture_info_formats,
    "furniture_info_formats address");
enum { PT_FIELD_restaurant_close_confirm_text = (offsetof(PTNamedMemory, saved_restaurant_close_confirm_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_restaurant_close_confirm_text) == PT_FIELD_restaurant_close_confirm_text,
    "restaurant_close_confirm_text address");
enum { PT_FIELD_restaurant_need_ovens_text = (offsetof(PTNamedMemory, saved_restaurant_need_ovens_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_restaurant_need_ovens_text) == PT_FIELD_restaurant_need_ovens_text,
    "restaurant_need_ovens_text address");
enum { PT_FIELD_restaurant_need_chefs_text = (offsetof(PTNamedMemory, saved_restaurant_need_chefs_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_restaurant_need_chefs_text) == PT_FIELD_restaurant_need_chefs_text,
    "restaurant_need_chefs_text address");
enum { PT_FIELD_restaurant_need_waiters_text = (offsetof(PTNamedMemory, saved_restaurant_need_waiters_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_restaurant_need_waiters_text) == PT_FIELD_restaurant_need_waiters_text,
    "restaurant_need_waiters_text address");
enum { PT_FIELD_restaurant_need_recipes_text = (offsetof(PTNamedMemory, saved_restaurant_need_recipes_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_restaurant_need_recipes_text) == PT_FIELD_restaurant_need_recipes_text,
    "restaurant_need_recipes_text address");
enum { PT_FIELD_restaurant_closed_today_text = (offsetof(PTNamedMemory, saved_restaurant_closed_today_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_restaurant_closed_today_text) == PT_FIELD_restaurant_closed_today_text,
    "restaurant_closed_today_text address");
enum { PT_FIELD_restaurant_open_confirm_text = (offsetof(PTNamedMemory, saved_restaurant_open_confirm_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_restaurant_open_confirm_text) == PT_FIELD_restaurant_open_confirm_text,
    "restaurant_open_confirm_text address");
enum { PT_FIELD_restaurant_rearrange_text = (offsetof(PTNamedMemory, saved_restaurant_rearrange_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_restaurant_rearrange_text) == PT_FIELD_restaurant_rearrange_text,
    "restaurant_rearrange_text address");
enum { PT_FIELD_restaurant_nothing_text = (offsetof(PTNamedMemory, saved_restaurant_nothing_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_restaurant_nothing_text) == PT_FIELD_restaurant_nothing_text,
    "restaurant_nothing_text address");
enum { PT_FIELD_restaurant_media_prompt = (offsetof(PTNamedMemory, saved_restaurant_media_prompt)) };
_Static_assert(offsetof(PTNamedMemory, saved_restaurant_media_prompt) == PT_FIELD_restaurant_media_prompt,
    "restaurant_media_prompt address");
enum { PT_FIELD_recipe_price_limit = (offsetof(PTNamedMemory, saved_recipe_price_limit)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_recipe_price_limit) == PT_FIELD_recipe_price_limit, "recipe_price_limit address");
enum { PT_FIELD_recipe_price_scale = (offsetof(PTNamedMemory, saved_recipe_price_scale)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_recipe_price_scale) == PT_FIELD_recipe_price_scale, "recipe_price_scale address");
enum { PT_FIELD_recipe_list_title = (offsetof(PTNamedMemory, saved_recipe_list_title)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_recipe_list_title) == PT_FIELD_recipe_list_title, "recipe_list_title address");
enum { PT_FIELD_recipe_list_page_format = (offsetof(PTNamedMemory, saved_recipe_list_page_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_recipe_list_page_format) == PT_FIELD_recipe_list_page_format,
    "recipe_list_page_format address");
enum { PT_FIELD_recipe_list_new = (offsetof(PTNamedMemory, saved_recipe_list_new)) };
_Static_assert(offsetof(PTNamedMemory, saved_recipe_list_new) == PT_FIELD_recipe_list_new, "recipe_list_new address");
enum { PT_FIELD_recipe_file_format = (offsetof(PTNamedMemory, saved_recipe_file_format)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_recipe_file_format) == PT_FIELD_recipe_file_format, "recipe_file_format address");
enum { PT_FIELD_recipe_list_save_custom_error = (offsetof(PTNamedMemory, saved_recipe_list_save_custom_error)) };
_Static_assert(offsetof(PTNamedMemory, saved_recipe_list_save_custom_error) == PT_FIELD_recipe_list_save_custom_error,
    "recipe_list_save_custom_error address");
enum { PT_FIELD_recipe_list_recreate_prompt = (offsetof(PTNamedMemory, saved_recipe_list_recreate_prompt)) };
_Static_assert(offsetof(PTNamedMemory, saved_recipe_list_recreate_prompt) == PT_FIELD_recipe_list_recreate_prompt,
    "recipe_list_recreate_prompt address");
enum { PT_FIELD_recipe_list_create_first_error = (offsetof(PTNamedMemory, saved_recipe_list_create_first_error)) };
_Static_assert(offsetof(PTNamedMemory, saved_recipe_list_create_first_error) == PT_FIELD_recipe_list_create_first_error,
    "recipe_list_create_first_error address");
enum { PT_FIELD_recipe_list_already_menu_error = (offsetof(PTNamedMemory, saved_recipe_list_already_menu_error)) };
_Static_assert(offsetof(PTNamedMemory, saved_recipe_list_already_menu_error) == PT_FIELD_recipe_list_already_menu_error,
    "recipe_list_already_menu_error address");
enum { PT_FIELD_recipe_price_prompt = (offsetof(PTNamedMemory, saved_recipe_price_prompt)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_recipe_price_prompt) == PT_FIELD_recipe_price_prompt, "recipe_price_prompt address");
enum { PT_FIELD_recipe_list_menu_full_error = (offsetof(PTNamedMemory, saved_recipe_list_menu_full_error)) };
_Static_assert(offsetof(PTNamedMemory, saved_recipe_list_menu_full_error) == PT_FIELD_recipe_list_menu_full_error,
    "recipe_list_menu_full_error address");
enum { PT_FIELD_pizza_menu_page_format = (offsetof(PTNamedMemory, saved_pizza_menu_page_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_pizza_menu_page_format) == PT_FIELD_pizza_menu_page_format,
    "pizza_menu_page_format address");
enum { PT_FIELD_pizza_menu_sales_title = (offsetof(PTNamedMemory, saved_pizza_menu_sales_title)) };
_Static_assert(offsetof(PTNamedMemory, saved_pizza_menu_sales_title) == PT_FIELD_pizza_menu_sales_title,
    "pizza_menu_sales_title address");
enum { PT_FIELD_pizza_menu_title = (offsetof(PTNamedMemory, saved_pizza_menu_title)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_pizza_menu_title) == PT_FIELD_pizza_menu_title, "pizza_menu_title address");
enum { PT_FIELD_pizza_menu_sales_label = (offsetof(PTNamedMemory, saved_pizza_menu_sales_label)) };
_Static_assert(offsetof(PTNamedMemory, saved_pizza_menu_sales_label) == PT_FIELD_pizza_menu_sales_label,
    "pizza_menu_sales_label address");
enum { PT_FIELD_pizza_menu_turnover_label = (offsetof(PTNamedMemory, saved_pizza_menu_turnover_label)) };
_Static_assert(offsetof(PTNamedMemory, saved_pizza_menu_turnover_label) == PT_FIELD_pizza_menu_turnover_label,
    "pizza_menu_turnover_label address");
enum { PT_FIELD_pizza_menu_popularity_label = (offsetof(PTNamedMemory, saved_pizza_menu_popularity_label)) };
_Static_assert(offsetof(PTNamedMemory, saved_pizza_menu_popularity_label) == PT_FIELD_pizza_menu_popularity_label,
    "pizza_menu_popularity_label address");
enum { PT_FIELD_pizza_menu_demand_label = (offsetof(PTNamedMemory, saved_pizza_menu_demand_label)) };
_Static_assert(offsetof(PTNamedMemory, saved_pizza_menu_demand_label) == PT_FIELD_pizza_menu_demand_label,
    "pizza_menu_demand_label address");
enum { PT_FIELD_pizza_menu_info_sales_label = (offsetof(PTNamedMemory, saved_pizza_menu_info_sales_label)) };
_Static_assert(offsetof(PTNamedMemory, saved_pizza_menu_info_sales_label) == PT_FIELD_pizza_menu_info_sales_label,
    "pizza_menu_info_sales_label address");
enum { PT_FIELD_pizza_menu_keep_one_error = (offsetof(PTNamedMemory, saved_pizza_menu_keep_one_error)) };
_Static_assert(offsetof(PTNamedMemory, saved_pizza_menu_keep_one_error) == PT_FIELD_pizza_menu_keep_one_error,
    "pizza_menu_keep_one_error address");
enum {
  PT_FIELD_pizza_menu_remove_selection_error = (offsetof(PTNamedMemory, saved_pizza_menu_remove_selection_error))
};
_Static_assert(
    offsetof(PTNamedMemory, saved_pizza_menu_remove_selection_error) == PT_FIELD_pizza_menu_remove_selection_error,
    "pizza_menu_remove_selection_error address");
enum { PT_FIELD_profit_graph_scale_base = (offsetof(PTNamedMemory, saved_profit_graph_scale_base)) };
_Static_assert(offsetof(PTNamedMemory, saved_profit_graph_scale_base) == PT_FIELD_profit_graph_scale_base,
    "profit_graph_scale_base address");
enum { PT_FIELD_legacy_graph_scale_base = (offsetof(PTNamedMemory, saved_legacy_graph_scale_base)) };
_Static_assert(offsetof(PTNamedMemory, saved_legacy_graph_scale_base) == PT_FIELD_legacy_graph_scale_base,
    "legacy_graph_scale_base address");
enum { PT_FIELD_cash_flow_caption = (offsetof(PTNamedMemory, saved_cash_flow_caption)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_cash_flow_caption) == PT_FIELD_cash_flow_caption, "cash_flow_caption address");
enum { PT_FIELD_profit_graph_units_format = (offsetof(PTNamedMemory, saved_profit_graph_units_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_profit_graph_units_format) == PT_FIELD_profit_graph_units_format,
    "profit_graph_units_format address");
enum { PT_FIELD_profit_graph_weeks = (offsetof(PTNamedMemory, saved_profit_graph_weeks)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_profit_graph_weeks) == PT_FIELD_profit_graph_weeks, "profit_graph_weeks address");
enum { PT_FIELD_cash_graph_scale_base = (offsetof(PTNamedMemory, saved_cash_graph_scale_base)) };
_Static_assert(offsetof(PTNamedMemory, saved_cash_graph_scale_base) == PT_FIELD_cash_graph_scale_base,
    "cash_graph_scale_base address");
enum { PT_FIELD_daily_profit_caption = (offsetof(PTNamedMemory, saved_daily_profit_caption)) };
_Static_assert(offsetof(PTNamedMemory, saved_daily_profit_caption) == PT_FIELD_daily_profit_caption,
    "daily_profit_caption address");
enum { PT_FIELD_profit_graph_days = (offsetof(PTNamedMemory, saved_profit_graph_days)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_profit_graph_days) == PT_FIELD_profit_graph_days, "profit_graph_days address");
enum { PT_FIELD_daily_profit_scale_base = (offsetof(PTNamedMemory, saved_daily_profit_scale_base)) };
_Static_assert(offsetof(PTNamedMemory, saved_daily_profit_scale_base) == PT_FIELD_daily_profit_scale_base,
    "daily_profit_scale_base address");
enum { PT_FIELD_balance_labels = (offsetof(PTNamedMemory, saved_balance_labels)) };
_Static_assert(offsetof(PTNamedMemory, saved_balance_labels) == PT_FIELD_balance_labels, "balance_labels address");
enum { PT_FIELD_weekly_profit_text = (offsetof(PTNamedMemory, saved_weekly_profit_text)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_weekly_profit_text) == PT_FIELD_weekly_profit_text, "weekly_profit_text address");
enum { PT_FIELD_finance_stats_labels = (offsetof(PTNamedMemory, saved_finance_stats_labels)) };
_Static_assert(offsetof(PTNamedMemory, saved_finance_stats_labels) == PT_FIELD_finance_stats_labels,
    "finance_stats_labels address");
enum { PT_FIELD_monthly_overview_caption = (offsetof(PTNamedMemory, saved_monthly_overview_caption)) };
_Static_assert(offsetof(PTNamedMemory, saved_monthly_overview_caption) == PT_FIELD_monthly_overview_caption,
    "monthly_overview_caption address");
enum { PT_FIELD_monthly_result_caption = (offsetof(PTNamedMemory, saved_monthly_result_caption)) };
_Static_assert(offsetof(PTNamedMemory, saved_monthly_result_caption) == PT_FIELD_monthly_result_caption,
    "monthly_result_caption address");
enum { PT_FIELD_monthly_restaurants_caption = (offsetof(PTNamedMemory, saved_monthly_restaurants_caption)) };
_Static_assert(offsetof(PTNamedMemory, saved_monthly_restaurants_caption) == PT_FIELD_monthly_restaurants_caption,
    "monthly_restaurants_caption address");
enum { PT_FIELD_daily_settlement_text = (offsetof(PTNamedMemory, saved_daily_settlement_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_daily_settlement_text) == PT_FIELD_daily_settlement_text,
    "daily_settlement_text address");
enum { PT_FIELD_daily_day_before_text = (offsetof(PTNamedMemory, saved_daily_day_before_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_daily_day_before_text) == PT_FIELD_daily_day_before_text,
    "daily_day_before_text address");
enum { PT_FIELD_daily_yesterday_text = (offsetof(PTNamedMemory, saved_daily_yesterday_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_daily_yesterday_text) == PT_FIELD_daily_yesterday_text,
    "daily_yesterday_text address");
enum { PT_FIELD_daily_today_text = (offsetof(PTNamedMemory, saved_daily_today_text)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_daily_today_text) == PT_FIELD_daily_today_text, "daily_today_text address");
enum { PT_FIELD_daily_orders_text = (offsetof(PTNamedMemory, saved_daily_orders_text)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_daily_orders_text) == PT_FIELD_daily_orders_text, "daily_orders_text address");
enum { PT_FIELD_daily_pizzas_sold_text = (offsetof(PTNamedMemory, saved_daily_pizzas_sold_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_daily_pizzas_sold_text) == PT_FIELD_daily_pizzas_sold_text,
    "daily_pizzas_sold_text address");
enum { PT_FIELD_daily_delivery_capacity_text = (offsetof(PTNamedMemory, saved_daily_delivery_capacity_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_daily_delivery_capacity_text) == PT_FIELD_daily_delivery_capacity_text,
    "daily_delivery_capacity_text address");
enum { PT_FIELD_daily_demand_value_text = (offsetof(PTNamedMemory, saved_daily_demand_value_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_daily_demand_value_text) == PT_FIELD_daily_demand_value_text,
    "daily_demand_value_text address");
enum { PT_FIELD_weekly_sales_text = (offsetof(PTNamedMemory, saved_weekly_sales_text)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_weekly_sales_text) == PT_FIELD_weekly_sales_text, "weekly_sales_text address");
enum { PT_FIELD_daily_ingredients_used_text = (offsetof(PTNamedMemory, saved_daily_ingredients_used_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_daily_ingredients_used_text) == PT_FIELD_daily_ingredients_used_text,
    "daily_ingredients_used_text address");
enum { PT_FIELD_daily_expenditure_text = (offsetof(PTNamedMemory, saved_daily_expenditure_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_daily_expenditure_text) == PT_FIELD_daily_expenditure_text,
    "daily_expenditure_text address");
enum { PT_FIELD_weekly_week_text = (offsetof(PTNamedMemory, saved_weekly_week_text)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_weekly_week_text) == PT_FIELD_weekly_week_text, "weekly_week_text address");
enum { PT_FIELD_weekly_takings_text = (offsetof(PTNamedMemory, saved_weekly_takings_text)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_weekly_takings_text) == PT_FIELD_weekly_takings_text, "weekly_takings_text address");
enum { PT_FIELD_weekly_items_text = (offsetof(PTNamedMemory, saved_weekly_items_text)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_weekly_items_text) == PT_FIELD_weekly_items_text, "weekly_items_text address");
enum { PT_FIELD_weekly_costs_text = (offsetof(PTNamedMemory, saved_weekly_costs_text)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_weekly_costs_text) == PT_FIELD_weekly_costs_text, "weekly_costs_text address");
enum { PT_FIELD_weekly_total_text = (offsetof(PTNamedMemory, saved_weekly_total_text)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_weekly_total_text) == PT_FIELD_weekly_total_text, "weekly_total_text address");
enum { PT_FIELD_weekly_expenses_text = (offsetof(PTNamedMemory, saved_weekly_expenses_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_weekly_expenses_text) == PT_FIELD_weekly_expenses_text,
    "weekly_expenses_text address");
enum { PT_FIELD_weekly_ingredients_text = (offsetof(PTNamedMemory, saved_weekly_ingredients_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_weekly_ingredients_text) == PT_FIELD_weekly_ingredients_text,
    "weekly_ingredients_text address");
enum { PT_FIELD_weekly_cooks_text = (offsetof(PTNamedMemory, saved_weekly_cooks_text)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_weekly_cooks_text) == PT_FIELD_weekly_cooks_text, "weekly_cooks_text address");
enum { PT_FIELD_weekly_staff_name_format = (offsetof(PTNamedMemory, saved_weekly_staff_name_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_weekly_staff_name_format) == PT_FIELD_weekly_staff_name_format,
    "weekly_staff_name_format address");
enum { PT_FIELD_weekly_empty_staff_format = (offsetof(PTNamedMemory, saved_weekly_empty_staff_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_weekly_empty_staff_format) == PT_FIELD_weekly_empty_staff_format,
    "weekly_empty_staff_format address");
enum { PT_FIELD_weekly_waiters_text = (offsetof(PTNamedMemory, saved_weekly_waiters_text)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_weekly_waiters_text) == PT_FIELD_weekly_waiters_text, "weekly_waiters_text address");
enum { PT_FIELD_weekly_managers_text = (offsetof(PTNamedMemory, saved_weekly_managers_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_weekly_managers_text) == PT_FIELD_weekly_managers_text,
    "weekly_managers_text address");
enum { PT_FIELD_restaurant_overview_day = (offsetof(PTNamedMemory, saved_restaurant_overview_day)) };
_Static_assert(offsetof(PTNamedMemory, saved_restaurant_overview_day) == PT_FIELD_restaurant_overview_day,
    "restaurant_overview_day address");
enum { PT_FIELD_restaurant_overview_week = (offsetof(PTNamedMemory, saved_restaurant_overview_week)) };
_Static_assert(offsetof(PTNamedMemory, saved_restaurant_overview_week) == PT_FIELD_restaurant_overview_week,
    "restaurant_overview_week address");
enum { PT_FIELD_restaurant_overview_caption = (offsetof(PTNamedMemory, saved_restaurant_overview_caption)) };
_Static_assert(offsetof(PTNamedMemory, saved_restaurant_overview_caption) == PT_FIELD_restaurant_overview_caption,
    "restaurant_overview_caption address");
enum { PT_FIELD_rest_sales_caption = (offsetof(PTNamedMemory, saved_rest_sales_caption)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_rest_sales_caption) == PT_FIELD_rest_sales_caption, "rest_sales_caption address");
enum { PT_FIELD_rest_customers_caption = (offsetof(PTNamedMemory, saved_rest_customers_caption)) };
_Static_assert(offsetof(PTNamedMemory, saved_rest_customers_caption) == PT_FIELD_rest_customers_caption,
    "rest_customers_caption address");
enum { PT_FIELD_rest_history_date_format = (offsetof(PTNamedMemory, saved_rest_history_date_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_rest_history_date_format) == PT_FIELD_rest_history_date_format,
    "rest_history_date_format address");
enum { PT_FIELD_rest_weekly_caption = (offsetof(PTNamedMemory, saved_rest_weekly_caption)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_rest_weekly_caption) == PT_FIELD_rest_weekly_caption, "rest_weekly_caption address");
enum { PT_FIELD_rest_weekly_columns = (offsetof(PTNamedMemory, saved_rest_weekly_columns)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_rest_weekly_columns) == PT_FIELD_rest_weekly_columns, "rest_weekly_columns address");
enum { PT_FIELD_advert_substitution_format = (offsetof(PTNamedMemory, saved_advert_substitution_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_advert_substitution_format) == PT_FIELD_advert_substitution_format,
    "advert_substitution_format address");
enum { PT_FIELD_advert_output_format = (offsetof(PTNamedMemory, saved_advert_output_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_advert_output_format) == PT_FIELD_advert_output_format,
    "advert_output_format address");
enum { PT_FIELD_advert_flyer_format = (offsetof(PTNamedMemory, saved_advert_flyer_format)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_advert_flyer_format) == PT_FIELD_advert_flyer_format, "advert_flyer_format address");
enum { PT_FIELD_advert_output_label = (offsetof(PTNamedMemory, saved_advert_output_label)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_advert_output_label) == PT_FIELD_advert_output_label, "advert_output_label address");
enum { PT_FIELD_advert_costs_label = (offsetof(PTNamedMemory, saved_advert_costs_label)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_advert_costs_label) == PT_FIELD_advert_costs_label, "advert_costs_label address");
enum { PT_FIELD_advert_poster_format = (offsetof(PTNamedMemory, saved_advert_poster_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_advert_poster_format) == PT_FIELD_advert_poster_format,
    "advert_poster_format address");
enum { PT_FIELD_advert_newspaper_cost_format = (offsetof(PTNamedMemory, saved_advert_newspaper_cost_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_advert_newspaper_cost_format) == PT_FIELD_advert_newspaper_cost_format,
    "advert_newspaper_cost_format address");
enum { PT_FIELD_advert_tv_format = (offsetof(PTNamedMemory, saved_advert_tv_format)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_advert_tv_format) == PT_FIELD_advert_tv_format, "advert_tv_format address");
enum { PT_FIELD_advert_tv_title = (offsetof(PTNamedMemory, saved_advert_tv_title)) };
_Static_assert(offsetof(PTNamedMemory, saved_advert_tv_title) == PT_FIELD_advert_tv_title, "advert_tv_title address");
enum { PT_FIELD_advert_tv_director = (offsetof(PTNamedMemory, saved_advert_tv_director)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_advert_tv_director) == PT_FIELD_advert_tv_director, "advert_tv_director address");
enum { PT_FIELD_advert_days = (offsetof(PTNamedMemory, saved_advert_days)) };
_Static_assert(offsetof(PTNamedMemory, saved_advert_days) == PT_FIELD_advert_days, "advert_days address");
enum { PT_FIELD_advert_misc_filename = (offsetof(PTNamedMemory, saved_advert_misc_filename)) };
_Static_assert(offsetof(PTNamedMemory, saved_advert_misc_filename) == PT_FIELD_advert_misc_filename,
    "advert_misc_filename address");
enum { PT_FIELD_advert_already_active = (offsetof(PTNamedMemory, saved_advert_already_active)) };
_Static_assert(offsetof(PTNamedMemory, saved_advert_already_active) == PT_FIELD_advert_already_active,
    "advert_already_active address");
enum { PT_FIELD_end_turn_phone_text = (offsetof(PTNamedMemory, saved_end_turn_phone_text)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_end_turn_phone_text) == PT_FIELD_end_turn_phone_text, "end_turn_phone_text address");
enum { PT_FIELD_market_share_price_divisor = (offsetof(PTNamedMemory, saved_market_share_price_divisor)) };
_Static_assert(offsetof(PTNamedMemory, saved_market_share_price_divisor) == PT_FIELD_market_share_price_divisor,
    "market_share_price_divisor address");
enum { PT_FIELD_market_share_base_threshold = (offsetof(PTNamedMemory, saved_market_share_base_threshold)) };
_Static_assert(offsetof(PTNamedMemory, saved_market_share_base_threshold) == PT_FIELD_market_share_base_threshold,
    "market_share_base_threshold address");
enum { PT_FIELD_insufficient_cash_text = (offsetof(PTNamedMemory, saved_insufficient_cash_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_insufficient_cash_text) == PT_FIELD_insufficient_cash_text,
    "insufficient_cash_text address");
enum { PT_FIELD_ledger_limit = (offsetof(PTNamedMemory, saved_ledger_limit)) };
_Static_assert(offsetof(PTNamedMemory, saved_ledger_limit) == PT_FIELD_ledger_limit, "ledger_limit address");
enum { PT_FIELD_periodic_extortion_text = (offsetof(PTNamedMemory, saved_periodic_extortion_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_periodic_extortion_text) == PT_FIELD_periodic_extortion_text,
    "periodic_extortion_text address");
enum { PT_FIELD_restaurant_opening_text = (offsetof(PTNamedMemory, saved_restaurant_opening_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_restaurant_opening_text) == PT_FIELD_restaurant_opening_text,
    "restaurant_opening_text address");
enum { PT_FIELD_restaurant_closing_text = (offsetof(PTNamedMemory, saved_restaurant_closing_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_restaurant_closing_text) == PT_FIELD_restaurant_closing_text,
    "restaurant_closing_text address");
enum { PT_FIELD_cash_goal_thresholds = (offsetof(PTNamedMemory, saved_cash_goal_thresholds)) };
_Static_assert(offsetof(PTNamedMemory, saved_cash_goal_thresholds) == PT_FIELD_cash_goal_thresholds,
    "cash_goal_thresholds address");
enum { PT_FIELD_delayed_message_format = (offsetof(PTNamedMemory, saved_delayed_message_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_delayed_message_format) == PT_FIELD_delayed_message_format,
    "delayed_message_format address");
enum { PT_FIELD_supplier_price_scale = (offsetof(PTNamedMemory, saved_supplier_price_scale)) };
_Static_assert(offsetof(PTNamedMemory, saved_supplier_price_scale) == PT_FIELD_supplier_price_scale,
    "supplier_price_scale address");
enum { PT_FIELD_flooring_unlock_hint = (offsetof(PTNamedMemory, saved_flooring_unlock_hint)) };
_Static_assert(offsetof(PTNamedMemory, saved_flooring_unlock_hint) == PT_FIELD_flooring_unlock_hint,
    "flooring_unlock_hint address");
enum { PT_FIELD_supplier_debts_paid_format = (offsetof(PTNamedMemory, saved_supplier_debts_paid_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_supplier_debts_paid_format) == PT_FIELD_supplier_debts_paid_format,
    "supplier_debts_paid_format address");
enum { PT_FIELD_supplier_debts_due_format = (offsetof(PTNamedMemory, saved_supplier_debts_due_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_supplier_debts_due_format) == PT_FIELD_supplier_debts_due_format,
    "supplier_debts_due_format address");
enum { PT_FIELD_overdrawn_account_text = (offsetof(PTNamedMemory, saved_overdrawn_account_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_overdrawn_account_text) == PT_FIELD_overdrawn_account_text,
    "overdrawn_account_text address");
enum { PT_FIELD_van_furniture_value_format = (offsetof(PTNamedMemory, saved_van_furniture_value_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_van_furniture_value_format) == PT_FIELD_van_furniture_value_format,
    "van_furniture_value_format address");
enum { PT_FIELD_weekly_profit_caption = (offsetof(PTNamedMemory, saved_weekly_profit_caption)) };
_Static_assert(offsetof(PTNamedMemory, saved_weekly_profit_caption) == PT_FIELD_weekly_profit_caption,
    "weekly_profit_caption address");
enum { PT_FIELD_weekly_cash_history_limit = (offsetof(PTNamedMemory, saved_weekly_cash_history_limit)) };
_Static_assert(offsetof(PTNamedMemory, saved_weekly_cash_history_limit) == PT_FIELD_weekly_cash_history_limit,
    "weekly_cash_history_limit address");
enum { PT_FIELD_monthly_cost_caption = (offsetof(PTNamedMemory, saved_monthly_cost_caption)) };
_Static_assert(offsetof(PTNamedMemory, saved_monthly_cost_caption) == PT_FIELD_monthly_cost_caption,
    "monthly_cost_caption address");
enum { PT_FIELD_monthly_repayment_caption = (offsetof(PTNamedMemory, saved_monthly_repayment_caption)) };
_Static_assert(offsetof(PTNamedMemory, saved_monthly_repayment_caption) == PT_FIELD_monthly_repayment_caption,
    "monthly_repayment_caption address");
enum { PT_FIELD_monthly_interest_caption = (offsetof(PTNamedMemory, saved_monthly_interest_caption)) };
_Static_assert(offsetof(PTNamedMemory, saved_monthly_interest_caption) == PT_FIELD_monthly_interest_caption,
    "monthly_interest_caption address");
enum { PT_FIELD_monthly_insurance_caption = (offsetof(PTNamedMemory, saved_monthly_insurance_caption)) };
_Static_assert(offsetof(PTNamedMemory, saved_monthly_insurance_caption) == PT_FIELD_monthly_insurance_caption,
    "monthly_insurance_caption address");
enum {
  PT_FIELD_monthly_investment_interest_caption = (offsetof(PTNamedMemory, saved_monthly_investment_interest_caption))
};
_Static_assert(
    offsetof(PTNamedMemory, saved_monthly_investment_interest_caption) == PT_FIELD_monthly_investment_interest_caption,
    "monthly_investment_interest_caption address");
enum { PT_FIELD_monthly_rent_caption = (offsetof(PTNamedMemory, saved_monthly_rent_caption)) };
_Static_assert(offsetof(PTNamedMemory, saved_monthly_rent_caption) == PT_FIELD_monthly_rent_caption,
    "monthly_rent_caption address");
enum { PT_FIELD_monthly_total_caption = (offsetof(PTNamedMemory, saved_monthly_total_caption)) };
_Static_assert(offsetof(PTNamedMemory, saved_monthly_total_caption) == PT_FIELD_monthly_total_caption,
    "monthly_total_caption address");
enum { PT_FIELD_player_card_dot = (offsetof(PTNamedMemory, saved_player_card_dot)) };
_Static_assert(offsetof(PTNamedMemory, saved_player_card_dot) == PT_FIELD_player_card_dot, "player_card_dot address");
enum { PT_FIELD_end_turn_wait_text = (offsetof(PTNamedMemory, saved_end_turn_wait_text)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_end_turn_wait_text) == PT_FIELD_end_turn_wait_text, "end_turn_wait_text address");
enum { PT_FIELD_sound_driver_signature = (offsetof(PTNamedMemory, saved_sound_driver_signature)) };
_Static_assert(offsetof(PTNamedMemory, saved_sound_driver_signature) == PT_FIELD_sound_driver_signature,
    "sound_driver_signature address");
enum { PT_FIELD_timezone_env_name = (offsetof(PTNamedMemory, saved_timezone_env_name)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_timezone_env_name) == PT_FIELD_timezone_env_name, "timezone_env_name address");
enum { PT_FIELD_watcom_character_classes = (offsetof(PTNamedMemory, saved_watcom_character_classes)) };
_Static_assert(offsetof(PTNamedMemory, saved_watcom_character_classes) == PT_FIELD_watcom_character_classes,
    "watcom_character_classes address");
enum { PT_FIELD_price_parser_positive_infinity = (offsetof(PTNamedMemory, saved_price_parser_positive_infinity)) };
_Static_assert(offsetof(PTNamedMemory, saved_price_parser_positive_infinity) == PT_FIELD_price_parser_positive_infinity,
    "price_parser_positive_infinity address");
enum { PT_FIELD_scale10_positive_infinity = (offsetof(PTNamedMemory, saved_scale10_positive_infinity)) };
_Static_assert(offsetof(PTNamedMemory, saved_scale10_positive_infinity) == PT_FIELD_scale10_positive_infinity,
    "scale10_positive_infinity address");
enum { PT_FIELD_scale10_negative_infinity = (offsetof(PTNamedMemory, saved_scale10_negative_infinity)) };
_Static_assert(offsetof(PTNamedMemory, saved_scale10_negative_infinity) == PT_FIELD_scale10_negative_infinity,
    "scale10_negative_infinity address");
enum { PT_FIELD_scale10_positive_max = (offsetof(PTNamedMemory, saved_scale10_positive_max)) };
_Static_assert(offsetof(PTNamedMemory, saved_scale10_positive_max) == PT_FIELD_scale10_positive_max,
    "scale10_positive_max address");
enum { PT_FIELD_scale10_negative_max = (offsetof(PTNamedMemory, saved_scale10_negative_max)) };
_Static_assert(offsetof(PTNamedMemory, saved_scale10_negative_max) == PT_FIELD_scale10_negative_max,
    "scale10_negative_max address");
enum { PT_FIELD_time_common_month_days = (offsetof(PTNamedMemory, saved_time_common_month_days)) };
_Static_assert(offsetof(PTNamedMemory, saved_time_common_month_days) == PT_FIELD_time_common_month_days,
    "time_common_month_days address");
enum { PT_FIELD_time_leap_month_days = (offsetof(PTNamedMemory, saved_time_leap_month_days)) };
_Static_assert(offsetof(PTNamedMemory, saved_time_leap_month_days) == PT_FIELD_time_leap_month_days,
    "time_leap_month_days address");
enum { PT_FIELD_scale10_powers = (offsetof(PTNamedMemory, saved_scale10_powers)) };
_Static_assert(offsetof(PTNamedMemory, saved_scale10_powers) == PT_FIELD_scale10_powers, "scale10_powers address");
enum { PT_FIELD_shop_purchase_price = (offsetof(PTNamedMemory, saved_shop_purchase_price)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_shop_purchase_price) == PT_FIELD_shop_purchase_price, "shop_purchase_price address");
enum { PT_FIELD_purchase_amount = (offsetof(PTNamedMemory, saved_purchase_amount)) };
_Static_assert(offsetof(PTNamedMemory, saved_purchase_amount) == PT_FIELD_purchase_amount, "purchase_amount address");
enum { PT_FIELD_loaded_map_file = (offsetof(PTNamedMemory, saved_loaded_map_file)) };
_Static_assert(offsetof(PTNamedMemory, saved_loaded_map_file) == PT_FIELD_loaded_map_file, "loaded_map_file address");
enum { PT_FIELD_g_staff = (offsetof(PTNamedMemory, saved_g_staff)) };
_Static_assert(offsetof(PTNamedMemory, saved_g_staff) == PT_FIELD_g_staff, "g_staff address");
enum { PT_FIELD_serialized_mission_receipts = (offsetof(PTNamedMemory, saved_mission_receipts)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_mission_receipts) == PT_FIELD_serialized_mission_receipts,
    "serialized_mission_receipts address");
enum { PT_FIELD_global_balance = (offsetof(PTNamedMemory, saved_global_balance)) };
_Static_assert(offsetof(PTNamedMemory, saved_global_balance) == PT_FIELD_global_balance, "global_balance address");
enum { PT_FIELD_serialized_previous_month_expenses = (offsetof(PTNamedMemory, saved_previous_month_expenses)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_serialized_previous_month_expenses) == PT_FIELD_serialized_previous_month_expenses,
    "serialized_previous_month_expenses address");
enum { PT_FIELD_staff_is_type = (offsetof(PTNamedMemory, saved_staff_is_type)) };
_Static_assert(offsetof(PTNamedMemory, saved_staff_is_type) == PT_FIELD_staff_is_type, "staff_is_type address");
enum { PT_FIELD_serialized_previous_month_receipts = (offsetof(PTNamedMemory, saved_previous_month_receipts)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_serialized_previous_month_receipts) == PT_FIELD_serialized_previous_month_receipts,
    "serialized_previous_month_receipts address");
enum { PT_FIELD_serialized_player_loans = (offsetof(PTNamedMemory, saved_player_loans)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_player_loans) == PT_FIELD_serialized_player_loans,
    "serialized_player_loans address");
enum { PT_FIELD_staff_hates_player = (offsetof(PTNamedMemory, saved_staff_hates_player)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_staff_hates_player) == PT_FIELD_staff_hates_player, "staff_hates_player address");
enum { PT_FIELD_serialized_player_investments = (offsetof(PTNamedMemory, saved_player_investments)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_player_investments) == PT_FIELD_serialized_player_investments,
    "serialized_player_investments address");
enum { PT_FIELD_serialized_mission_expenses = (offsetof(PTNamedMemory, saved_mission_expenses)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_mission_expenses) == PT_FIELD_serialized_mission_expenses,
    "serialized_mission_expenses address");
enum { PT_FIELD_blitmap_offsets = (offsetof(PTNamedMemory, saved_blitmap_offsets)) };
_Static_assert(offsetof(PTNamedMemory, saved_blitmap_offsets) == PT_FIELD_blitmap_offsets, "blitmap_offsets address");
enum { PT_FIELD_mission_year = (offsetof(PTNamedMemory, saved_mission_year_columns)) };
_Static_assert(offsetof(PTNamedMemory, saved_mission_year) == PT_FIELD_mission_year, "mission_year address");
enum { PT_FIELD_mission_target_year = (offsetof(PTNamedMemory, saved_mission_target_year)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_mission_target_year) == PT_FIELD_mission_target_year, "mission_target_year address");
enum { PT_FIELD_mission_day = (offsetof(PTNamedMemory, saved_mission_day_columns)) };
_Static_assert(offsetof(PTNamedMemory, saved_mission_day) == PT_FIELD_mission_day, "mission_day address");
enum { PT_FIELD_mission_target_day = (offsetof(PTNamedMemory, saved_mission_target_day)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_mission_target_day) == PT_FIELD_mission_target_day, "mission_target_day address");
enum { PT_FIELD_mission_month = (offsetof(PTNamedMemory, saved_mission_month_columns)) };
_Static_assert(offsetof(PTNamedMemory, saved_mission_month) == PT_FIELD_mission_month, "mission_month address");
enum { PT_FIELD_mission_target_month = (offsetof(PTNamedMemory, saved_mission_target_month)) };
_Static_assert(offsetof(PTNamedMemory, saved_mission_target_month) == PT_FIELD_mission_target_month,
    "mission_target_month address");
enum { PT_FIELD_message_slots_free = (offsetof(PTNamedMemory, saved_message_slots_free)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_message_slots_free) == PT_FIELD_message_slots_free, "message_slots_free address");
enum { PT_FIELD_mission_goal = (offsetof(PTNamedMemory, saved_mission_goal_columns)) };
_Static_assert(offsetof(PTNamedMemory, saved_mission_goal) == PT_FIELD_mission_goal, "mission_goal address");
enum { PT_FIELD_mission_target_goal = (offsetof(PTNamedMemory, saved_mission_target_goal)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_mission_target_goal) == PT_FIELD_mission_target_goal, "mission_target_goal address");
enum { PT_FIELD_weapon_base_price = (offsetof(PTNamedMemory, saved_weapon_base_price)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_weapon_base_price) == PT_FIELD_weapon_base_price, "weapon_base_price address");
enum { PT_FIELD_delayed_msg_contents = (offsetof(PTNamedMemory, saved_delayed_msg_contents)) };
_Static_assert(offsetof(PTNamedMemory, saved_delayed_msg_contents) == PT_FIELD_delayed_msg_contents,
    "delayed_msg_contents address");
enum { PT_FIELD_bananas = (offsetof(PTNamedMemory, saved_bananas)) };
_Static_assert(offsetof(PTNamedMemory, saved_bananas) == PT_FIELD_bananas, "bananas address");
enum { PT_FIELD_map_animation_palette_colors = (offsetof(PTNamedMemory, saved_map_animation_colors)) };
_Static_assert(offsetof(PTNamedMemory, saved_map_animation_palette_colors) == PT_FIELD_map_animation_palette_colors,
    "map_animation_palette_colors address");
enum { PT_FIELD_map_animation_palette_repeat = (offsetof(PTNamedMemory, saved_map_animation_palette_repeat)) };
_Static_assert(offsetof(PTNamedMemory, saved_map_animation_palette_repeat) == PT_FIELD_map_animation_palette_repeat,
    "map_animation_palette_repeat address");
enum { PT_FIELD_array_size_8 = (offsetof(PTNamedMemory, saved_array_size_8)) };
_Static_assert(offsetof(PTNamedMemory, saved_array_size_8) == PT_FIELD_array_size_8, "array_size_8 address");
enum { PT_FIELD_extra_town_data = (offsetof(PTNamedMemory, saved_extra_town_data)) };
_Static_assert(offsetof(PTNamedMemory, saved_extra_town_data) == PT_FIELD_extra_town_data, "extra_town_data address");
enum { PT_FIELD_igloos = (offsetof(PTNamedMemory, saved_serialized_igloos)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_igloos) == PT_FIELD_igloos, "igloos address");
enum { PT_FIELD_map_igloo_tail = (offsetof(PTNamedMemory, saved_serialized_map_igloo_tail)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_serialized_map_igloo_tail) == PT_FIELD_map_igloo_tail, "map_igloo_tail address");
enum { PT_FIELD_map_igloo_head = (offsetof(PTNamedMemory, saved_serialized_map_igloo_head)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_serialized_map_igloo_head) == PT_FIELD_map_igloo_head, "map_igloo_head address");
enum { PT_FIELD_animated_stats_reputation = (offsetof(PTNamedMemory, saved_animated_stats_reputation)) };
_Static_assert(offsetof(PTNamedMemory, saved_animated_stats_reputation) == PT_FIELD_animated_stats_reputation,
    "animated_stats_reputation address");
enum { PT_FIELD_animated_stats_quality = (offsetof(PTNamedMemory, saved_animated_stats_quality)) };
_Static_assert(offsetof(PTNamedMemory, saved_animated_stats_quality) == PT_FIELD_animated_stats_quality,
    "animated_stats_quality address");
enum { PT_FIELD_temp_igloo_1 = (offsetof(PTNamedMemory, saved_serialized_temp_igloo_1)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_temp_igloo_1) == PT_FIELD_temp_igloo_1, "temp_igloo_1 address");
enum { PT_FIELD_temp_igloo_2 = (offsetof(PTNamedMemory, saved_serialized_temp_igloo_2)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_temp_igloo_2) == PT_FIELD_temp_igloo_2, "temp_igloo_2 address");
enum { PT_FIELD_guests = (offsetof(PTNamedMemory, saved_guests)) };
_Static_assert(offsetof(PTNamedMemory, saved_guests) == PT_FIELD_guests, "guests address");
enum { PT_FIELD_largest_map_column_bitmaps = (offsetof(PTNamedMemory, saved_serialized_largest_map_column_bitmaps)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_serialized_largest_map_column_bitmaps) == PT_FIELD_largest_map_column_bitmaps,
    "largest_map_column_bitmaps address");
enum { PT_FIELD_map_row_bitmaps = (offsetof(PTNamedMemory, saved_serialized_map_row_bitmaps)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_serialized_map_row_bitmaps) == PT_FIELD_map_row_bitmaps, "map_row_bitmaps address");
enum { PT_FIELD_largest_map_row_bitmaps = (offsetof(PTNamedMemory, saved_serialized_largest_map_row_bitmaps)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_largest_map_row_bitmaps) == PT_FIELD_largest_map_row_bitmaps,
    "largest_map_row_bitmaps address");
enum { PT_FIELD_map_column_bitmaps = (offsetof(PTNamedMemory, saved_serialized_map_column_bitmaps)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_map_column_bitmaps) == PT_FIELD_map_column_bitmaps,
    "map_column_bitmaps address");
enum { PT_FIELD_temp_igloo = (offsetof(PTNamedMemory, saved_serialized_temp_igloo)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_temp_igloo) == PT_FIELD_temp_igloo, "temp_igloo address");
enum { PT_FIELD_strbuf = (offsetof(PTNamedMemory, saved_strbuf)) };
_Static_assert(offsetof(PTNamedMemory, saved_strbuf) == PT_FIELD_strbuf, "strbuf address");
enum { PT_FIELD_palette_e = (offsetof(PTNamedMemory, saved_palette_e)) };
_Static_assert(offsetof(PTNamedMemory, saved_palette_e) == PT_FIELD_palette_e, "palette_e address");
enum { PT_FIELD_players = (offsetof(PTNamedMemory, saved_serialized_players)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_players) == PT_FIELD_players, "players address");
enum { PT_FIELD_pal_pointer = (offsetof(PTNamedMemory, saved_serialized_pal_pointer)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_pal_pointer) == PT_FIELD_pal_pointer, "pal_pointer address");
enum { PT_FIELD_characters = (offsetof(PTNamedMemory, saved_characters)) };
_Static_assert(offsetof(PTNamedMemory, saved_characters) == PT_FIELD_characters, "characters address");
enum { PT_FIELD_display_bitmaps = (offsetof(PTNamedMemory, saved_serialized_display_bitmaps)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_serialized_display_bitmaps) == PT_FIELD_display_bitmaps, "display_bitmaps address");
enum { PT_FIELD_popup_menu_items = (offsetof(PTNamedMemory, saved_serialized_popup_menu_items)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_popup_menu_items) == PT_FIELD_popup_menu_items,
    "popup_menu_items address");
enum { PT_FIELD_value_of_crashed_furniture_1 = (offsetof(PTNamedMemory, saved_value_of_crashed_furniture_1)) };
_Static_assert(offsetof(PTNamedMemory, saved_value_of_crashed_furniture_1) == PT_FIELD_value_of_crashed_furniture_1,
    "value_of_crashed_furniture_1 address");
enum { PT_FIELD_rest_floormap = (offsetof(PTNamedMemory, saved_rest_floormap)) };
_Static_assert(offsetof(PTNamedMemory, saved_rest_floormap) == PT_FIELD_rest_floormap, "rest_floormap address");
enum { PT_FIELD_attack_damage = (offsetof(PTNamedMemory, saved_attack_damage)) };
_Static_assert(offsetof(PTNamedMemory, saved_attack_damage) == PT_FIELD_attack_damage, "attack_damage address");
enum { PT_FIELD_loaded_bez_file = (offsetof(PTNamedMemory, saved_loaded_bez_file)) };
_Static_assert(offsetof(PTNamedMemory, saved_loaded_bez_file) == PT_FIELD_loaded_bez_file, "loaded_bez_file address");
enum { PT_FIELD_current_purchase_amount = (offsetof(PTNamedMemory, saved_current_purchase_amount)) };
_Static_assert(offsetof(PTNamedMemory, saved_current_purchase_amount) == PT_FIELD_current_purchase_amount,
    "current_purchase_amount address");
enum { PT_FIELD_durian = (offsetof(PTNamedMemory, saved_serialized_durian)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_durian) == PT_FIELD_durian, "durian address");
enum { PT_FIELD_active_input_class = (offsetof(PTNamedMemory, saved_active_input_class)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_active_input_class) == PT_FIELD_active_input_class, "active_input_class address");
enum { PT_FIELD_hazelnuts = (offsetof(PTNamedMemory, saved_serialized_hazelnuts)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_hazelnuts) == PT_FIELD_hazelnuts, "hazelnuts address");
enum { PT_FIELD_cur_loading_filehandle = (offsetof(PTNamedMemory, saved_serialized_cur_loading_filehandle)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_cur_loading_filehandle) == PT_FIELD_cur_loading_filehandle,
    "cur_loading_filehandle address");
enum { PT_FIELD_num_written_bytes = (offsetof(PTNamedMemory, saved_num_written_bytes)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_num_written_bytes) == PT_FIELD_num_written_bytes, "num_written_bytes address");
enum { PT_FIELD_ingredient_background = (offsetof(PTNamedMemory, saved_serialized_ingredient_background)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_ingredient_background) == PT_FIELD_ingredient_background,
    "ingredient_background address");
enum { PT_FIELD_num_read_bytes = (offsetof(PTNamedMemory, saved_num_read_bytes)) };
_Static_assert(offsetof(PTNamedMemory, saved_num_read_bytes) == PT_FIELD_num_read_bytes, "num_read_bytes address");
enum { PT_FIELD_cur_writing_filehandle = (offsetof(PTNamedMemory, saved_serialized_cur_writing_filehandle)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_cur_writing_filehandle) == PT_FIELD_cur_writing_filehandle,
    "cur_writing_filehandle address");
enum { PT_FIELD_buffer_backup = (offsetof(PTNamedMemory, saved_serialized_buffer_backup)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_serialized_buffer_backup) == PT_FIELD_buffer_backup, "buffer_backup address");
enum { PT_FIELD_last_recipe_ptr = (offsetof(PTNamedMemory, saved_serialized_last_recipe_ptr)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_serialized_last_recipe_ptr) == PT_FIELD_last_recipe_ptr, "last_recipe_ptr address");
enum { PT_FIELD_restaurant_with_guests = (offsetof(PTNamedMemory, saved_serialized_restaurant_with_guests)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_restaurant_with_guests) == PT_FIELD_restaurant_with_guests,
    "restaurant_with_guests address");
enum { PT_FIELD_restaurant_hit_test_owner = (offsetof(PTNamedMemory, saved_serialized_restaurant_hit_test_owner)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_serialized_restaurant_hit_test_owner) == PT_FIELD_restaurant_hit_test_owner,
    "restaurant_hit_test_owner address");
enum { PT_FIELD_game = (offsetof(PTNamedMemory, saved_serialized_game)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_game) == PT_FIELD_game, "game address");
enum { PT_FIELD_some_juice = (offsetof(PTNamedMemory, saved_serialized_some_juice)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_some_juice) == PT_FIELD_some_juice, "some_juice address");
enum { PT_FIELD_bmp16_ptr = (offsetof(PTNamedMemory, saved_serialized_bmp16_ptr)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_bmp16_ptr) == PT_FIELD_bmp16_ptr, "bmp16_ptr address");
enum { PT_FIELD_last_input_event = (offsetof(PTNamedMemory, saved_serialized_last_input_event)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_last_input_event) == PT_FIELD_last_input_event,
    "last_input_event address");
enum { PT_FIELD_font_cstandar = (offsetof(PTNamedMemory, saved_serialized_font_cstandar)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_serialized_font_cstandar) == PT_FIELD_font_cstandar, "font_cstandar address");
enum { PT_FIELD_exit_optional_font = (offsetof(PTNamedMemory, saved_serialized_exit_optional_font)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_exit_optional_font) == PT_FIELD_exit_optional_font,
    "exit_optional_font address");
enum { PT_FIELD_canvas1_ptr = (offsetof(PTNamedMemory, saved_serialized_canvas1_ptr)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_canvas1_ptr) == PT_FIELD_canvas1_ptr, "canvas1_ptr address");
enum { PT_FIELD_font_astandar = (offsetof(PTNamedMemory, saved_serialized_font_astandar)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_serialized_font_astandar) == PT_FIELD_font_astandar, "font_astandar address");
enum { PT_FIELD_recipe_lookup_fallback = (offsetof(PTNamedMemory, saved_serialized_recipe_lookup_fallback)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_recipe_lookup_fallback) == PT_FIELD_recipe_lookup_fallback,
    "recipe_lookup_fallback address");
enum { PT_FIELD_some_font_ptr = (offsetof(PTNamedMemory, saved_serialized_some_font_ptr)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_serialized_some_font_ptr) == PT_FIELD_some_font_ptr, "some_font_ptr address");
enum { PT_FIELD_font_supersma = (offsetof(PTNamedMemory, saved_serialized_font_supersma)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_serialized_font_supersma) == PT_FIELD_font_supersma, "font_supersma address");
enum { PT_FIELD_font_ruby = (offsetof(PTNamedMemory, saved_serialized_font_ruby)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_font_ruby) == PT_FIELD_font_ruby, "font_ruby address");
enum { PT_FIELD_font_piznumb = (offsetof(PTNamedMemory, saved_serialized_font_piznumb)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_font_piznumb) == PT_FIELD_font_piznumb, "font_piznumb address");
enum { PT_FIELD_buffer_2 = (offsetof(PTNamedMemory, saved_serialized_buffer_2)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_buffer_2) == PT_FIELD_buffer_2, "buffer_2 address");
enum { PT_FIELD_current_bitmap = (offsetof(PTNamedMemory, saved_serialized_current_bitmap)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_serialized_current_bitmap) == PT_FIELD_current_bitmap, "current_bitmap address");
enum { PT_FIELD_old_bitmap = (offsetof(PTNamedMemory, saved_serialized_old_bitmap)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_old_bitmap) == PT_FIELD_old_bitmap, "old_bitmap address");
enum { PT_FIELD_active_palette_pointer = (offsetof(PTNamedMemory, saved_serialized_active_palette_pointer)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_active_palette_pointer) == PT_FIELD_active_palette_pointer,
    "active_palette_pointer address");
enum { PT_FIELD_ptr_apple = (offsetof(PTNamedMemory, saved_serialized_ptr_apple)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_ptr_apple) == PT_FIELD_ptr_apple, "ptr_apple address");
enum { PT_FIELD_curr_rest = (offsetof(PTNamedMemory, saved_serialized_curr_rest)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_curr_rest) == PT_FIELD_curr_rest, "curr_rest address");
enum { PT_FIELD_game_ptr = (offsetof(PTNamedMemory, saved_serialized_game_ptr)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_game_ptr) == PT_FIELD_game_ptr, "game_ptr address");
enum { PT_FIELD_mouse_result = (offsetof(PTNamedMemory, saved_serialized_mouse_result)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_mouse_result) == PT_FIELD_mouse_result, "mouse_result address");
enum { PT_FIELD_jatoba_1 = (offsetof(PTNamedMemory, saved_serialized_jatoba_1)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_jatoba_1) == PT_FIELD_jatoba_1, "jatoba_1 address");
enum { PT_FIELD_canvas_1 = (offsetof(PTNamedMemory, saved_serialized_canvas_1)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_canvas_1) == PT_FIELD_canvas_1, "canvas_1 address");
enum { PT_FIELD_yaffa_value = (offsetof(PTNamedMemory, saved_yaffa_value)) };
_Static_assert(offsetof(PTNamedMemory, saved_yaffa_value) == PT_FIELD_yaffa_value, "yaffa_value address");
enum { PT_FIELD_yaffa_counter = (offsetof(PTNamedMemory, saved_yaffa_counter)) };
_Static_assert(offsetof(PTNamedMemory, saved_yaffa_counter) == PT_FIELD_yaffa_counter, "yaffa_counter address");
enum { PT_FIELD_walking_guest = (offsetof(PTNamedMemory, saved_serialized_walking_guest)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_serialized_walking_guest) == PT_FIELD_walking_guest, "walking_guest address");
enum { PT_FIELD_bmp6 = (offsetof(PTNamedMemory, saved_serialized_bmp6)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_bmp6) == PT_FIELD_bmp6, "bmp6 address");
enum { PT_FIELD_bmp15 = (offsetof(PTNamedMemory, saved_serialized_bmp15)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_bmp15) == PT_FIELD_bmp15, "bmp15 address");
enum { PT_FIELD_bmp5 = (offsetof(PTNamedMemory, saved_serialized_bmp5)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_bmp5) == PT_FIELD_bmp5, "bmp5 address");
enum { PT_FIELD_bmp14 = (offsetof(PTNamedMemory, saved_serialized_bmp14)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_bmp14) == PT_FIELD_bmp14, "bmp14 address");
enum { PT_FIELD_temp_palette_slot = (offsetof(PTNamedMemory, saved_serialized_temp_palette_slot)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_temp_palette_slot) == PT_FIELD_temp_palette_slot,
    "temp_palette_slot address");
enum { PT_FIELD_bmp3 = (offsetof(PTNamedMemory, saved_serialized_bmp3)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_bmp3) == PT_FIELD_bmp3, "bmp3 address");
enum { PT_FIELD_bmp11 = (offsetof(PTNamedMemory, saved_serialized_bmp11)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_bmp11) == PT_FIELD_bmp11, "bmp11 address");
enum { PT_FIELD_bmp2 = (offsetof(PTNamedMemory, saved_serialized_bmp2)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_bmp2) == PT_FIELD_bmp2, "bmp2 address");
enum { PT_FIELD_bmp1 = (offsetof(PTNamedMemory, saved_serialized_bmp1)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_bmp1) == PT_FIELD_bmp1, "bmp1 address");
enum { PT_FIELD_bmp10 = (offsetof(PTNamedMemory, saved_serialized_bmp10)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_bmp10) == PT_FIELD_bmp10, "bmp10 address");
enum { PT_FIELD_bmp12 = (offsetof(PTNamedMemory, saved_serialized_bmp12)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_bmp12) == PT_FIELD_bmp12, "bmp12 address");
enum { PT_FIELD_did_run_town_day_loop_1 = (offsetof(PTNamedMemory, saved_did_run_town_day_loop_1)) };
_Static_assert(offsetof(PTNamedMemory, saved_did_run_town_day_loop_1) == PT_FIELD_did_run_town_day_loop_1,
    "did_run_town_day_loop_1 address");
enum { PT_FIELD_temp_hazel = (offsetof(PTNamedMemory, saved_serialized_temp_hazel)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_temp_hazel) == PT_FIELD_temp_hazel, "temp_hazel address");
enum { PT_FIELD_savegame_sect_0 = (offsetof(PTNamedMemory, saved_savegame_sect_0)) };
_Static_assert(offsetof(PTNamedMemory, saved_savegame_sect_0) == PT_FIELD_savegame_sect_0, "savegame_sect_0 address");
enum { PT_FIELD_ramtown = (offsetof(PTNamedMemory, saved_ramtown)) };
_Static_assert(offsetof(PTNamedMemory, saved_ramtown) == PT_FIELD_ramtown, "ramtown address");
enum { PT_FIELD_active_input_value = (offsetof(PTNamedMemory, saved_active_input_value)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_active_input_value) == PT_FIELD_active_input_value, "active_input_value address");
enum { PT_FIELD_did_load_map_and_bez = (offsetof(PTNamedMemory, saved_did_load_map_and_bez)) };
_Static_assert(offsetof(PTNamedMemory, saved_did_load_map_and_bez) == PT_FIELD_did_load_map_and_bez,
    "did_load_map_and_bez address");
enum { PT_FIELD_cur_list_elem_x = (offsetof(PTNamedMemory, saved_cur_list_elem_x)) };
_Static_assert(offsetof(PTNamedMemory, saved_cur_list_elem_x) == PT_FIELD_cur_list_elem_x, "cur_list_elem_x address");
enum { PT_FIELD_cur_list_elem_y = (offsetof(PTNamedMemory, saved_cur_list_elem_y)) };
_Static_assert(offsetof(PTNamedMemory, saved_cur_list_elem_y) == PT_FIELD_cur_list_elem_y, "cur_list_elem_y address");
enum { PT_FIELD_mouse_x = (offsetof(PTNamedMemory, saved_mouse_x)) };
_Static_assert(offsetof(PTNamedMemory, saved_mouse_x) == PT_FIELD_mouse_x, "mouse_x address");
enum { PT_FIELD_mouse_y = (offsetof(PTNamedMemory, saved_mouse_y)) };
_Static_assert(offsetof(PTNamedMemory, saved_mouse_y) == PT_FIELD_mouse_y, "mouse_y address");
enum { PT_FIELD_mouse_pressed = (offsetof(PTNamedMemory, saved_mouse_pressed)) };
_Static_assert(offsetof(PTNamedMemory, saved_mouse_pressed) == PT_FIELD_mouse_pressed, "mouse_pressed address");
enum { PT_FIELD_temp_rest = (offsetof(PTNamedMemory, saved_serialized_temp_rest)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_temp_rest) == PT_FIELD_temp_rest, "temp_rest address");
enum { PT_FIELD_papaya_array = (offsetof(PTNamedMemory, saved_papaya_array)) };
_Static_assert(offsetof(PTNamedMemory, saved_papaya_array) == PT_FIELD_papaya_array, "papaya_array address");
enum { PT_FIELD_room = (offsetof(PTNamedMemory, saved_serialized_room)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_room) == PT_FIELD_room, "room address");
enum { PT_FIELD_temp_igloo_link = (offsetof(PTNamedMemory, saved_serialized_temp_igloo_link)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_serialized_temp_igloo_link) == PT_FIELD_temp_igloo_link, "temp_igloo_link address");
enum { PT_FIELD_map_igloo_link = (offsetof(PTNamedMemory, saved_serialized_map_igloo_link)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_serialized_map_igloo_link) == PT_FIELD_map_igloo_link, "map_igloo_link address");
enum { PT_FIELD_bmp16 = (offsetof(PTNamedMemory, saved_serialized_bmp16)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_bmp16) == PT_FIELD_bmp16, "bmp16 address");
enum { PT_FIELD_bmp25 = (offsetof(PTNamedMemory, saved_serialized_bmp25)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_bmp25) == PT_FIELD_bmp25, "bmp25 address");
enum { PT_FIELD_bmp24 = (offsetof(PTNamedMemory, saved_serialized_bmp24)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_bmp24) == PT_FIELD_bmp24, "bmp24 address");
enum { PT_FIELD_bmp_background = (offsetof(PTNamedMemory, saved_serialized_bmp_background)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_serialized_bmp_background) == PT_FIELD_bmp_background, "bmp_background address");
enum { PT_FIELD_bmp_awards = (offsetof(PTNamedMemory, saved_serialized_bmp_awards)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_bmp_awards) == PT_FIELD_bmp_awards, "bmp_awards address");
enum { PT_FIELD_bmp17 = (offsetof(PTNamedMemory, saved_serialized_bmp17)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_bmp17) == PT_FIELD_bmp17, "bmp17 address");
enum { PT_FIELD_canvas1 = (offsetof(PTNamedMemory, saved_serialized_canvas1)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_canvas1) == PT_FIELD_canvas1, "canvas1 address");
enum { PT_FIELD_loaded_character = (offsetof(PTNamedMemory, saved_loaded_character)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_loaded_character) == PT_FIELD_loaded_character, "loaded_character address");
enum { PT_FIELD_temp_character = (offsetof(PTNamedMemory, saved_temp_character)) };
_Static_assert(offsetof(PTNamedMemory, saved_temp_character) == PT_FIELD_temp_character, "temp_character address");
enum { PT_FIELD_bmp_blitmap = (offsetof(PTNamedMemory, saved_serialized_bmp_blitmap)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_bmp_blitmap) == PT_FIELD_bmp_blitmap, "bmp_blitmap address");
enum { PT_FIELD_map_scroll_x = (offsetof(PTNamedMemory, saved_map_scroll_x)) };
_Static_assert(offsetof(PTNamedMemory, saved_map_scroll_x) == PT_FIELD_map_scroll_x, "map_scroll_x address");
enum { PT_FIELD_map_scroll_y = (offsetof(PTNamedMemory, saved_map_scroll_y)) };
_Static_assert(offsetof(PTNamedMemory, saved_map_scroll_y) == PT_FIELD_map_scroll_y, "map_scroll_y address");
enum { PT_FIELD_attack_contamination = (offsetof(PTNamedMemory, saved_attack_contamination)) };
_Static_assert(offsetof(PTNamedMemory, saved_attack_contamination) == PT_FIELD_attack_contamination,
    "attack_contamination address");
enum { PT_FIELD_best_score_player = (offsetof(PTNamedMemory, saved_best_score_player)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_best_score_player) == PT_FIELD_best_score_player, "best_score_player address");
enum { PT_FIELD_state_slot_014 = (offsetof(PTNamedMemory, saved_state_slot_014)) };
_Static_assert(offsetof(PTNamedMemory, saved_state_slot_014) == PT_FIELD_state_slot_014, "state_slot_014 address");
enum { PT_FIELD_map_actor_count = (offsetof(PTNamedMemory, saved_map_actor_count)) };
_Static_assert(offsetof(PTNamedMemory, saved_map_actor_count) == PT_FIELD_map_actor_count, "map_actor_count address");
enum { PT_FIELD_state_slot_015 = (offsetof(PTNamedMemory, saved_state_slot_015)) };
_Static_assert(offsetof(PTNamedMemory, saved_state_slot_015) == PT_FIELD_state_slot_015, "state_slot_015 address");
enum { PT_FIELD_curr_score = (offsetof(PTNamedMemory, saved_curr_score)) };
_Static_assert(offsetof(PTNamedMemory, saved_curr_score) == PT_FIELD_curr_score, "curr_score address");
enum { PT_FIELD_state_slot_016 = (offsetof(PTNamedMemory, saved_state_slot_016)) };
_Static_assert(offsetof(PTNamedMemory, saved_state_slot_016) == PT_FIELD_state_slot_016, "state_slot_016 address");
enum { PT_FIELD_state_slot_017 = (offsetof(PTNamedMemory, saved_state_slot_017)) };
_Static_assert(offsetof(PTNamedMemory, saved_state_slot_017) == PT_FIELD_state_slot_017, "state_slot_017 address");
enum { PT_FIELD_guest_count = (offsetof(PTNamedMemory, saved_guest_count)) };
_Static_assert(offsetof(PTNamedMemory, saved_guest_count) == PT_FIELD_guest_count, "guest_count address");
enum { PT_FIELD_player_stats_value_x = (offsetof(PTNamedMemory, saved_player_stats_value_x)) };
_Static_assert(offsetof(PTNamedMemory, saved_player_stats_value_x) == PT_FIELD_player_stats_value_x,
    "player_stats_value_x address");
enum { PT_FIELD_draw_list_elem_style = (offsetof(PTNamedMemory, saved_draw_list_elem_style)) };
_Static_assert(offsetof(PTNamedMemory, saved_draw_list_elem_style) == PT_FIELD_draw_list_elem_style,
    "draw_list_elem_style address");
enum { PT_FIELD_did_terminal_stuff = (offsetof(PTNamedMemory, saved_did_terminal_stuff)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_did_terminal_stuff) == PT_FIELD_did_terminal_stuff, "did_terminal_stuff address");
enum { PT_FIELD_attack_police_outcome = (offsetof(PTNamedMemory, saved_attack_police_outcome)) };
_Static_assert(offsetof(PTNamedMemory, saved_attack_police_outcome) == PT_FIELD_attack_police_outcome,
    "attack_police_outcome address");
enum { PT_FIELD_max_num_characters = (offsetof(PTNamedMemory, saved_max_num_characters)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_max_num_characters) == PT_FIELD_max_num_characters, "max_num_characters address");
enum { PT_FIELD_current_player_index = (offsetof(PTNamedMemory, saved_current_player_index)) };
_Static_assert(offsetof(PTNamedMemory, saved_current_player_index) == PT_FIELD_current_player_index,
    "current_player_index address");
enum { PT_FIELD_staff_display_flag = (offsetof(PTNamedMemory, saved_staff_display_flag)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_staff_display_flag) == PT_FIELD_staff_display_flag, "staff_display_flag address");
enum { PT_FIELD_shop_purchase_secondary_price = (offsetof(PTNamedMemory, saved_shop_purchase_secondary_price)) };
_Static_assert(offsetof(PTNamedMemory, saved_shop_purchase_secondary_price) == PT_FIELD_shop_purchase_secondary_price,
    "shop_purchase_secondary_price address");
enum { PT_FIELD_shop_secondary_purchase_amount = (offsetof(PTNamedMemory, saved_shop_secondary_purchase_amount)) };
_Static_assert(offsetof(PTNamedMemory, saved_shop_secondary_purchase_amount) == PT_FIELD_shop_secondary_purchase_amount,
    "shop_secondary_purchase_amount address");
enum { PT_FIELD_temp_cutoff = (offsetof(PTNamedMemory, saved_temp_cutoff)) };
_Static_assert(offsetof(PTNamedMemory, saved_temp_cutoff) == PT_FIELD_temp_cutoff, "temp_cutoff address");
enum { PT_FIELD_depot_export_country = (offsetof(PTNamedMemory, saved_depot_export_country)) };
_Static_assert(offsetof(PTNamedMemory, saved_depot_export_country) == PT_FIELD_depot_export_country,
    "depot_export_country address");
enum { PT_FIELD_cursor_bitmap = (offsetof(PTNamedMemory, saved_serialized_cursor_bitmap)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_serialized_cursor_bitmap) == PT_FIELD_cursor_bitmap, "cursor_bitmap address");
enum { PT_FIELD_cursor_background_bitmap = (offsetof(PTNamedMemory, saved_serialized_cursor_background_bitmap)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_cursor_background_bitmap) == PT_FIELD_cursor_background_bitmap,
    "cursor_background_bitmap address");
enum { PT_FIELD_canvas_3 = (offsetof(PTNamedMemory, saved_serialized_canvas_3)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_canvas_3) == PT_FIELD_canvas_3, "canvas_3 address");
enum { PT_FIELD_map_background_buffer = (offsetof(PTNamedMemory, saved_serialized_map_background_buffer)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_map_background_buffer) == PT_FIELD_map_background_buffer,
    "map_background_buffer address");
enum { PT_FIELD_cursor_target_bitmap = (offsetof(PTNamedMemory, saved_serialized_cursor_target_bitmap)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_cursor_target_bitmap) == PT_FIELD_cursor_target_bitmap,
    "cursor_target_bitmap address");
enum { PT_FIELD_serialized_display_bias = (offsetof(PTNamedMemory, saved_serialized_display_bias)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_display_bias) == PT_FIELD_serialized_display_bias,
    "serialized_display_bias address");
enum { PT_FIELD_transparent_mask_buffer = (offsetof(PTNamedMemory, saved_serialized_transparent_mask_buffer)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_transparent_mask_buffer) == PT_FIELD_transparent_mask_buffer,
    "transparent_mask_buffer address");
enum { PT_FIELD_some_pal_p = (offsetof(PTNamedMemory, saved_serialized_some_pal_p)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_some_pal_p) == PT_FIELD_some_pal_p, "some_pal_p address");
enum { PT_FIELD_startup_word0 = (offsetof(PTNamedMemory, saved_startup_word0)) };
_Static_assert(offsetof(PTNamedMemory, saved_startup_word0) == PT_FIELD_startup_word0, "startup_word0 address");
enum { PT_FIELD_startup_word1 = (offsetof(PTNamedMemory, saved_startup_word1)) };
_Static_assert(offsetof(PTNamedMemory, saved_startup_word1) == PT_FIELD_startup_word1, "startup_word1 address");
enum { PT_FIELD_startup_word2 = (offsetof(PTNamedMemory, saved_startup_word2)) };
_Static_assert(offsetof(PTNamedMemory, saved_startup_word2) == PT_FIELD_startup_word2, "startup_word2 address");
enum { PT_FIELD_mouse_bitmap_1 = (offsetof(PTNamedMemory, saved_serialized_mouse_bitmap_1)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_serialized_mouse_bitmap_1) == PT_FIELD_mouse_bitmap_1, "mouse_bitmap_1 address");
enum { PT_FIELD_mouse_bitmap_2 = (offsetof(PTNamedMemory, saved_serialized_mouse_bitmap_2)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_serialized_mouse_bitmap_2) == PT_FIELD_mouse_bitmap_2, "mouse_bitmap_2 address");
enum { PT_FIELD_interactive_attack_attacker = (offsetof(PTNamedMemory, saved_serialized_interactive_attack_attacker)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_serialized_interactive_attack_attacker) == PT_FIELD_interactive_attack_attacker,
    "interactive_attack_attacker address");
enum { PT_FIELD_interactive_attack_victim = (offsetof(PTNamedMemory, saved_serialized_interactive_attack_victim)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_serialized_interactive_attack_victim) == PT_FIELD_interactive_attack_victim,
    "interactive_attack_victim address");
enum { PT_FIELD_g_pigs = (offsetof(PTNamedMemory, saved_serialized_g_pigs)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_g_pigs) == PT_FIELD_g_pigs, "g_pigs address");
enum { PT_FIELD_rest_path_visits = (offsetof(PTNamedMemory, saved_rest_path_visits)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_rest_path_visits) == PT_FIELD_rest_path_visits, "rest_path_visits address");
enum { PT_FIELD_guest_portrait_x = (offsetof(PTNamedMemory, saved_guest_portrait_x)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_guest_portrait_x) == PT_FIELD_guest_portrait_x, "guest_portrait_x address");
enum { PT_FIELD_guest_portrait_y = (offsetof(PTNamedMemory, saved_guest_portrait_y)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_guest_portrait_y) == PT_FIELD_guest_portrait_y, "guest_portrait_y address");
enum { PT_FIELD_state_slot_031 = (offsetof(PTNamedMemory, saved_state_slot_031)) };
_Static_assert(offsetof(PTNamedMemory, saved_state_slot_031) == PT_FIELD_state_slot_031, "state_slot_031 address");
enum { PT_FIELD_state_slot_018 = (offsetof(PTNamedMemory, saved_state_slot_018)) };
_Static_assert(offsetof(PTNamedMemory, saved_state_slot_018) == PT_FIELD_state_slot_018, "state_slot_018 address");
enum { PT_FIELD_rest_path_target_x = (offsetof(PTNamedMemory, saved_rest_path_target_x)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_rest_path_target_x) == PT_FIELD_rest_path_target_x, "rest_path_target_x address");
enum { PT_FIELD_rest_path_target_y = (offsetof(PTNamedMemory, saved_rest_path_target_y)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_rest_path_target_y) == PT_FIELD_rest_path_target_y, "rest_path_target_y address");
enum { PT_FIELD_state_slot_019 = (offsetof(PTNamedMemory, saved_state_slot_019)) };
_Static_assert(offsetof(PTNamedMemory, saved_state_slot_019) == PT_FIELD_state_slot_019, "state_slot_019 address");
enum { PT_FIELD_state_slot_020 = (offsetof(PTNamedMemory, saved_state_slot_020)) };
_Static_assert(offsetof(PTNamedMemory, saved_state_slot_020) == PT_FIELD_state_slot_020, "state_slot_020 address");
enum { PT_FIELD_recipe_rank_character = (offsetof(PTNamedMemory, saved_serialized_recipe_rank_character)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_recipe_rank_character) == PT_FIELD_recipe_rank_character,
    "recipe_rank_character address");
enum { PT_FIELD_time_left_text = (offsetof(PTNamedMemory, saved_time_left_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_time_left_text) == PT_FIELD_time_left_text, "time_left_text address");
enum { PT_FIELD_selected_filename = (offsetof(PTNamedMemory, saved_selected_filename)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_selected_filename) == PT_FIELD_selected_filename, "selected_filename address");
enum { PT_FIELD_savegame_slot = (offsetof(PTNamedMemory, saved_savegame_slot)) };
_Static_assert(offsetof(PTNamedMemory, saved_savegame_slot) == PT_FIELD_savegame_slot, "savegame_slot address");
enum { PT_FIELD_map_marker_phase = (offsetof(PTNamedMemory, saved_map_marker_phase)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_map_marker_phase) == PT_FIELD_map_marker_phase, "map_marker_phase address");
enum { PT_FIELD_animated_order_snapshot = (offsetof(PTNamedMemory, saved_animated_order_snapshot)) };
_Static_assert(offsetof(PTNamedMemory, saved_animated_order_snapshot) == PT_FIELD_animated_order_snapshot,
    "animated_order_snapshot address");
enum { PT_FIELD_animated_pizza_snapshot = (offsetof(PTNamedMemory, saved_animated_pizza_snapshot)) };
_Static_assert(offsetof(PTNamedMemory, saved_animated_pizza_snapshot) == PT_FIELD_animated_pizza_snapshot,
    "animated_pizza_snapshot address");
enum { PT_FIELD_animated_finished_snapshot = (offsetof(PTNamedMemory, saved_animated_finished_snapshot)) };
_Static_assert(offsetof(PTNamedMemory, saved_animated_finished_snapshot) == PT_FIELD_animated_finished_snapshot,
    "animated_finished_snapshot address");
enum { PT_FIELD_animated_order_max = (offsetof(PTNamedMemory, saved_animated_order_max)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_animated_order_max) == PT_FIELD_animated_order_max, "animated_order_max address");
enum { PT_FIELD_animated_oven_baking_time = (offsetof(PTNamedMemory, saved_animated_oven_baking_time)) };
_Static_assert(offsetof(PTNamedMemory, saved_animated_oven_baking_time) == PT_FIELD_animated_oven_baking_time,
    "animated_oven_baking_time address");
enum { PT_FIELD_animated_oven_capacity = (offsetof(PTNamedMemory, saved_animated_oven_capacity)) };
_Static_assert(offsetof(PTNamedMemory, saved_animated_oven_capacity) == PT_FIELD_animated_oven_capacity,
    "animated_oven_capacity address");
enum { PT_FIELD_animated_oven_used = (offsetof(PTNamedMemory, saved_animated_oven_used)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_animated_oven_used) == PT_FIELD_animated_oven_used, "animated_oven_used address");
enum { PT_FIELD_animated_finished_count = (offsetof(PTNamedMemory, saved_animated_finished_count)) };
_Static_assert(offsetof(PTNamedMemory, saved_animated_finished_count) == PT_FIELD_animated_finished_count,
    "animated_finished_count address");
enum { PT_FIELD_animated_order_count = (offsetof(PTNamedMemory, saved_animated_order_count)) };
_Static_assert(offsetof(PTNamedMemory, saved_animated_order_count) == PT_FIELD_animated_order_count,
    "animated_order_count address");
enum { PT_FIELD_animated_stats_needle = (offsetof(PTNamedMemory, saved_animated_stats_needle)) };
_Static_assert(offsetof(PTNamedMemory, saved_animated_stats_needle) == PT_FIELD_animated_stats_needle,
    "animated_stats_needle address");
enum { PT_FIELD_animated_oven_count = (offsetof(PTNamedMemory, saved_animated_oven_count)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_animated_oven_count) == PT_FIELD_animated_oven_count, "animated_oven_count address");
enum { PT_FIELD_animated_stats_arrow_drawn = (offsetof(PTNamedMemory, saved_animated_stats_arrow_drawn)) };
_Static_assert(offsetof(PTNamedMemory, saved_animated_stats_arrow_drawn) == PT_FIELD_animated_stats_arrow_drawn,
    "animated_stats_arrow_drawn address");
enum { PT_FIELD_animated_pizza_count = (offsetof(PTNamedMemory, saved_animated_pizza_count)) };
_Static_assert(offsetof(PTNamedMemory, saved_animated_pizza_count) == PT_FIELD_animated_pizza_count,
    "animated_pizza_count address");
enum { PT_FIELD_animated_pizza_max = (offsetof(PTNamedMemory, saved_animated_pizza_max)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_animated_pizza_max) == PT_FIELD_animated_pizza_max, "animated_pizza_max address");
enum { PT_FIELD_dialled_number_string = (offsetof(PTNamedMemory, saved_dialled_number_string)) };
_Static_assert(offsetof(PTNamedMemory, saved_dialled_number_string) == PT_FIELD_dialled_number_string,
    "dialled_number_string address");
enum { PT_FIELD_dial_number_buffer = (offsetof(PTNamedMemory, saved_dial_number_buffer)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_dial_number_buffer) == PT_FIELD_dial_number_buffer, "dial_number_buffer address");
enum { PT_FIELD_recipe_rank_flag = (offsetof(PTNamedMemory, saved_recipe_rank_flag)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_recipe_rank_flag) == PT_FIELD_recipe_rank_flag, "recipe_rank_flag address");
enum { PT_FIELD_mission_date_buffers = (offsetof(PTNamedMemory, saved_mission_date_buffers)) };
_Static_assert(offsetof(PTNamedMemory, saved_mission_date_buffers) == PT_FIELD_mission_date_buffers,
    "mission_date_buffers address");
enum { PT_FIELD_mission_place_buffers = (offsetof(PTNamedMemory, saved_mission_place_buffers)) };
_Static_assert(offsetof(PTNamedMemory, saved_mission_place_buffers) == PT_FIELD_mission_place_buffers,
    "mission_place_buffers address");
enum { PT_FIELD_mission_text_string = (offsetof(PTNamedMemory, saved_mission_text_string)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_mission_text_string) == PT_FIELD_mission_text_string, "mission_text_string address");
enum { PT_FIELD_temp_string = (offsetof(PTNamedMemory, saved_temp_string)) };
_Static_assert(offsetof(PTNamedMemory, saved_temp_string) == PT_FIELD_temp_string, "temp_string address");
enum { PT_FIELD_retained_message_text = (offsetof(PTNamedMemory, saved_retained_message_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_retained_message_text) == PT_FIELD_retained_message_text,
    "retained_message_text address");
enum { PT_FIELD_restaurant_name_text = (offsetof(PTNamedMemory, saved_restaurant_name_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_restaurant_name_text) == PT_FIELD_restaurant_name_text,
    "restaurant_name_text address");
enum { PT_FIELD_menu_hierarchy_backup = (offsetof(PTNamedMemory, saved_restaurant_name_text) + 36) };
_Static_assert(offsetof(PTNamedMemory, saved_restaurant_name_text) + 36 == PT_FIELD_menu_hierarchy_backup,
    "menu_hierarchy_backup address");
enum { PT_FIELD_canvas_menu_backup = (offsetof(PTNamedMemory, saved_canvas_menu_backup)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_canvas_menu_backup) == PT_FIELD_canvas_menu_backup, "canvas_menu_backup address");
enum { PT_FIELD_placement_contact_types = (offsetof(PTNamedMemory, saved_placement_contact_types)) };
_Static_assert(offsetof(PTNamedMemory, saved_placement_contact_types) == PT_FIELD_placement_contact_types,
    "placement_contact_types address");
enum { PT_FIELD_placement_contact_indices = (offsetof(PTNamedMemory, saved_placement_contact_indices)) };
_Static_assert(offsetof(PTNamedMemory, saved_placement_contact_indices) == PT_FIELD_placement_contact_indices,
    "placement_contact_indices address");
enum { PT_FIELD_placement_contact_amounts = (offsetof(PTNamedMemory, saved_placement_contact_amounts)) };
_Static_assert(offsetof(PTNamedMemory, saved_placement_contact_amounts) == PT_FIELD_placement_contact_amounts,
    "placement_contact_amounts address");
enum { PT_FIELD_placement_contact_items = (offsetof(PTNamedMemory, saved_serialized_placement_contact_items)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_placement_contact_items) == PT_FIELD_placement_contact_items,
    "placement_contact_items address");
enum { PT_FIELD_furniture_quality_in_van = (offsetof(PTNamedMemory, saved_furniture_quality_in_van)) };
_Static_assert(offsetof(PTNamedMemory, saved_furniture_quality_in_van) == PT_FIELD_furniture_quality_in_van,
    "furniture_quality_in_van address");
enum { PT_FIELD_placement_contact_recorded = (offsetof(PTNamedMemory, saved_placement_contact_recorded)) };
_Static_assert(offsetof(PTNamedMemory, saved_placement_contact_recorded) == PT_FIELD_placement_contact_recorded,
    "placement_contact_recorded address");
enum { PT_FIELD_sound_var_2 = (offsetof(PTNamedMemory, saved_serialized_sound_var_2)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_sound_var_2) == PT_FIELD_sound_var_2, "sound_var_2 address");
enum { PT_FIELD_sound_var_1 = (offsetof(PTNamedMemory, saved_serialized_sound_var_1)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_sound_var_1) == PT_FIELD_sound_var_1, "sound_var_1 address");
enum { PT_FIELD_sound_flag = (offsetof(PTNamedMemory, saved_sound_flag)) };
_Static_assert(offsetof(PTNamedMemory, saved_sound_flag) == PT_FIELD_sound_flag, "sound_flag address");
enum { PT_FIELD_legacy_menu_sales = (offsetof(PTNamedMemory, saved_legacy_menu_sales)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_legacy_menu_sales) == PT_FIELD_legacy_menu_sales, "legacy_menu_sales address");
enum { PT_FIELD_legacy_menu_orders = (offsetof(PTNamedMemory, saved_legacy_menu_orders)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_legacy_menu_orders) == PT_FIELD_legacy_menu_orders, "legacy_menu_orders address");
enum { PT_FIELD_advert_newspaper_lines = (offsetof(PTNamedMemory, saved_advert_newspaper_lines)) };
_Static_assert(offsetof(PTNamedMemory, saved_advert_newspaper_lines) == PT_FIELD_advert_newspaper_lines,
    "advert_newspaper_lines address");
enum { PT_FIELD_advert_newspaper_buffer = (offsetof(PTNamedMemory, saved_advert_newspaper_buffer)) };
_Static_assert(offsetof(PTNamedMemory, saved_advert_newspaper_buffer) == PT_FIELD_advert_newspaper_buffer,
    "advert_newspaper_buffer address");
enum { PT_FIELD_newspaper_text_buffer2 = (offsetof(PTNamedMemory, saved_newspaper_text_buffer2)) };
_Static_assert(offsetof(PTNamedMemory, saved_newspaper_text_buffer2) == PT_FIELD_newspaper_text_buffer2,
    "newspaper_text_buffer2 address");
enum { PT_FIELD_newspaper_text_buffer = (offsetof(PTNamedMemory, saved_newspaper_text_buffer)) };
_Static_assert(offsetof(PTNamedMemory, saved_newspaper_text_buffer) == PT_FIELD_newspaper_text_buffer,
    "newspaper_text_buffer address");
enum { PT_FIELD_newspaper_article_buffer = (offsetof(PTNamedMemory, saved_serialized_newspaper_article_buffer)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_newspaper_article_buffer) == PT_FIELD_newspaper_article_buffer,
    "newspaper_article_buffer address");
enum { PT_FIELD_newspaper_delay = (offsetof(PTNamedMemory, saved_newspaper_delay)) };
_Static_assert(offsetof(PTNamedMemory, saved_newspaper_delay) == PT_FIELD_newspaper_delay, "newspaper_delay address");
enum { PT_FIELD_newpaper_rand = (offsetof(PTNamedMemory, saved_newpaper_rand)) };
_Static_assert(offsetof(PTNamedMemory, saved_newpaper_rand) == PT_FIELD_newpaper_rand, "newpaper_rand address");
enum { PT_FIELD_newspaper_random_direction = (offsetof(PTNamedMemory, saved_newspaper_random_direction)) };
_Static_assert(offsetof(PTNamedMemory, saved_newspaper_random_direction) == PT_FIELD_newspaper_random_direction,
    "newspaper_random_direction address");
enum { PT_FIELD_newspaper_random_mode = (offsetof(PTNamedMemory, saved_newspaper_random_mode)) };
_Static_assert(offsetof(PTNamedMemory, saved_newspaper_random_mode) == PT_FIELD_newspaper_random_mode,
    "newspaper_random_mode address");
enum { PT_FIELD_newspaper_background = (offsetof(PTNamedMemory, saved_newspaper_background)) };
_Static_assert(offsetof(PTNamedMemory, saved_newspaper_background) == PT_FIELD_newspaper_background,
    "newspaper_background address");
enum { PT_FIELD_newspaper_ingredient = (offsetof(PTNamedMemory, saved_newspaper_ingredient)) };
_Static_assert(offsetof(PTNamedMemory, saved_newspaper_ingredient) == PT_FIELD_newspaper_ingredient,
    "newspaper_ingredient address");
enum { PT_FIELD_did_run_town_day_loop_2 = (offsetof(PTNamedMemory, saved_did_run_town_day_loop_2)) };
_Static_assert(offsetof(PTNamedMemory, saved_did_run_town_day_loop_2) == PT_FIELD_did_run_town_day_loop_2,
    "did_run_town_day_loop_2 address");
enum { PT_FIELD_did_run_town_day_loop_3 = (offsetof(PTNamedMemory, saved_did_run_town_day_loop_3)) };
_Static_assert(offsetof(PTNamedMemory, saved_did_run_town_day_loop_3) == PT_FIELD_did_run_town_day_loop_3,
    "did_run_town_day_loop_3 address");
enum { PT_FIELD_state_slot_021 = (offsetof(PTNamedMemory, saved_state_slot_021)) };
_Static_assert(offsetof(PTNamedMemory, saved_state_slot_021) == PT_FIELD_state_slot_021, "state_slot_021 address");
enum { PT_FIELD_sound_driver_interrupt = (offsetof(PTNamedMemory, saved_sound_driver_interrupt)) };
_Static_assert(offsetof(PTNamedMemory, saved_sound_driver_interrupt) == PT_FIELD_sound_driver_interrupt,
    "sound_driver_interrupt address");
enum { PT_FIELD_guest_free_file_nodes = (offsetof(PTNamedMemory, saved_serialized_guest_free_file_nodes)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_guest_free_file_nodes) == PT_FIELD_guest_free_file_nodes,
    "guest_free_file_nodes address");
enum { PT_FIELD_state_slot_022 = (offsetof(PTNamedMemory, saved_state_slot_022)) };
_Static_assert(offsetof(PTNamedMemory, saved_state_slot_022) == PT_FIELD_state_slot_022, "state_slot_022 address");
enum { PT_FIELD_guest_active_file_nodes = (offsetof(PTNamedMemory, saved_serialized_guest_active_file_nodes)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_guest_active_file_nodes) == PT_FIELD_guest_active_file_nodes,
    "guest_active_file_nodes address");
enum { PT_FIELD_delay_polls_per_second = (offsetof(PTNamedMemory, saved_delay_polls_per_second)) };
_Static_assert(offsetof(PTNamedMemory, saved_delay_polls_per_second) == PT_FIELD_delay_polls_per_second,
    "delay_polls_per_second address");
enum { PT_FIELD_guest_dos_errno = (offsetof(PTNamedMemory, saved_guest_dos_errno)) };
_Static_assert(offsetof(PTNamedMemory, saved_guest_dos_errno) == PT_FIELD_guest_dos_errno, "guest_dos_errno address");
enum { PT_FIELD_guest_errno = (offsetof(PTNamedMemory, saved_guest_errno)) };
_Static_assert(offsetof(PTNamedMemory, saved_guest_errno) == PT_FIELD_guest_errno, "guest_errno address");
enum { PT_FIELD_recipe_number_unit = (offsetof(PTNamedMemory, saved_recipe_number_unit)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_recipe_number_unit) == PT_FIELD_recipe_number_unit, "recipe_number_unit address");
enum { PT_FIELD_recipe_load_prompt = (offsetof(PTNamedMemory, saved_recipe_load_prompt)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_recipe_load_prompt) == PT_FIELD_recipe_load_prompt, "recipe_load_prompt address");
enum { PT_FIELD_recipe_save_prompt = (offsetof(PTNamedMemory, saved_recipe_save_prompt)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_recipe_save_prompt) == PT_FIELD_recipe_save_prompt, "recipe_save_prompt address");
enum { PT_FIELD_recipe_load_tag = (offsetof(PTNamedMemory, saved_recipe_load_tag)) };
_Static_assert(offsetof(PTNamedMemory, saved_recipe_load_tag) == PT_FIELD_recipe_load_tag, "recipe_load_tag address");
enum { PT_FIELD_recipe_save_tag = (offsetof(PTNamedMemory, saved_recipe_save_tag)) };
_Static_assert(offsetof(PTNamedMemory, saved_recipe_save_tag) == PT_FIELD_recipe_save_tag, "recipe_save_tag address");
enum { PT_FIELD_recipe_load_custom_error = (offsetof(PTNamedMemory, saved_recipe_load_custom_error)) };
_Static_assert(offsetof(PTNamedMemory, saved_recipe_load_custom_error) == PT_FIELD_recipe_load_custom_error,
    "recipe_load_custom_error address");
enum { PT_FIELD_recipe_duplicate_error = (offsetof(PTNamedMemory, saved_recipe_duplicate_error)) };
_Static_assert(offsetof(PTNamedMemory, saved_recipe_duplicate_error) == PT_FIELD_recipe_duplicate_error,
    "recipe_duplicate_error address");
enum { PT_FIELD_recipe_checksum_error = (offsetof(PTNamedMemory, saved_recipe_checksum_error)) };
_Static_assert(offsetof(PTNamedMemory, saved_recipe_checksum_error) == PT_FIELD_recipe_checksum_error,
    "recipe_checksum_error address");
enum { PT_FIELD_weapon_store_quantities = (offsetof(PTNamedMemory, saved_weapon_store_quantities)) };
_Static_assert(offsetof(PTNamedMemory, saved_weapon_store_quantities) == PT_FIELD_weapon_store_quantities,
    "weapon_store_quantities address");
enum { PT_FIELD_weapon_store_piece_format = (offsetof(PTNamedMemory, saved_weapon_store_piece_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_weapon_store_piece_format) == PT_FIELD_weapon_store_piece_format,
    "weapon_store_piece_format address");
enum { PT_FIELD_weapon_store_sell_format = (offsetof(PTNamedMemory, saved_weapon_store_sell_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_weapon_store_sell_format) == PT_FIELD_weapon_store_sell_format,
    "weapon_store_sell_format address");
enum { PT_FIELD_weapon_store_buy_format = (offsetof(PTNamedMemory, saved_weapon_store_buy_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_weapon_store_buy_format) == PT_FIELD_weapon_store_buy_format,
    "weapon_store_buy_format address");
enum { PT_FIELD_intro_music_filename = (offsetof(PTNamedMemory, saved_intro_music_filename)) };
_Static_assert(offsetof(PTNamedMemory, saved_intro_music_filename) == PT_FIELD_intro_music_filename,
    "intro_music_filename address");
enum { PT_FIELD_music_filename_format = (offsetof(PTNamedMemory, saved_music_filename_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_music_filename_format) == PT_FIELD_music_filename_format,
    "music_filename_format address");
enum { PT_FIELD_sound_pcm_cursor = (offsetof(PTNamedMemory, saved_serialized_sound_pcm_cursor)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_sound_pcm_cursor) == PT_FIELD_sound_pcm_cursor,
    "sound_pcm_cursor address");
enum { PT_FIELD_sound_pcm_remaining = (offsetof(PTNamedMemory, saved_sound_pcm_remaining)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_sound_pcm_remaining) == PT_FIELD_sound_pcm_remaining, "sound_pcm_remaining address");
enum { PT_FIELD_sound_pcm_repeat_start = (offsetof(PTNamedMemory, saved_serialized_sound_pcm_repeat_start)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_sound_pcm_repeat_start) == PT_FIELD_sound_pcm_repeat_start,
    "sound_pcm_repeat_start address");
enum { PT_FIELD_sound_pcm_repeat_end = (offsetof(PTNamedMemory, saved_serialized_sound_pcm_repeat_end)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_sound_pcm_repeat_end) == PT_FIELD_sound_pcm_repeat_end,
    "sound_pcm_repeat_end address");
enum { PT_FIELD_sound_pcm_repeats = (offsetof(PTNamedMemory, saved_sound_pcm_repeats)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_sound_pcm_repeats) == PT_FIELD_sound_pcm_repeats, "sound_pcm_repeats address");
enum { PT_FIELD_sound_dma_record = (offsetof(PTNamedMemory, saved_serialized_sound_dma_record)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_sound_dma_record) == PT_FIELD_sound_dma_record,
    "sound_dma_record address");
enum { PT_FIELD_sound_digital_configuration = (offsetof(PTNamedMemory, saved_sound_digital_configuration)) };
_Static_assert(offsetof(PTNamedMemory, saved_sound_digital_configuration) == PT_FIELD_sound_digital_configuration,
    "sound_digital_configuration address");
enum { PT_FIELD_sound_pcm_output = (offsetof(PTNamedMemory, saved_serialized_sound_pcm_output)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_sound_pcm_output) == PT_FIELD_sound_pcm_output,
    "sound_pcm_output address");
enum { PT_FIELD_sound_pcm_buffers = (offsetof(PTNamedMemory, saved_serialized_sound_pcm_buffers)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_sound_pcm_buffers) == PT_FIELD_sound_pcm_buffers,
    "sound_pcm_buffers address");
enum { PT_FIELD_sound_pcm_auto_dma = (offsetof(PTNamedMemory, saved_sound_pcm_auto_dma)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_sound_pcm_auto_dma) == PT_FIELD_sound_pcm_auto_dma, "sound_pcm_auto_dma address");
enum { PT_FIELD_sound_pcm_dma_half = (offsetof(PTNamedMemory, saved_sound_pcm_dma_half)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_sound_pcm_dma_half) == PT_FIELD_sound_pcm_dma_half, "sound_pcm_dma_half address");
enum { PT_FIELD_sound_dos_block_count = (offsetof(PTNamedMemory, saved_sound_dos_block_count)) };
_Static_assert(offsetof(PTNamedMemory, saved_sound_dos_block_count) == PT_FIELD_sound_dos_block_count,
    "sound_dos_block_count address");
enum { PT_FIELD_sound_dos_blocks = (offsetof(PTNamedMemory, saved_serialized_sound_dos_blocks)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_sound_dos_blocks) == PT_FIELD_sound_dos_blocks,
    "sound_dos_blocks address");
enum { PT_FIELD_sound_dos_selectors = (offsetof(PTNamedMemory, saved_sound_dos_selectors)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_sound_dos_selectors) == PT_FIELD_sound_dos_selectors, "sound_dos_selectors address");
enum { PT_FIELD_sound_pcm_driver_loaded = (offsetof(PTNamedMemory, saved_sound_pcm_driver_loaded)) };
_Static_assert(offsetof(PTNamedMemory, saved_sound_pcm_driver_loaded) == PT_FIELD_sound_pcm_driver_loaded,
    "sound_pcm_driver_loaded address");
enum { PT_FIELD_sound_pcm_driver_ready = (offsetof(PTNamedMemory, saved_sound_pcm_driver_ready)) };
_Static_assert(offsetof(PTNamedMemory, saved_sound_pcm_driver_ready) == PT_FIELD_sound_pcm_driver_ready,
    "sound_pcm_driver_ready address");
enum { PT_FIELD_sound_pcm_driver_image = (offsetof(PTNamedMemory, saved_serialized_sound_pcm_driver_image)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_sound_pcm_driver_image) == PT_FIELD_sound_pcm_driver_image,
    "sound_pcm_driver_image address");
enum { PT_FIELD_sound_pcm_driver_external = (offsetof(PTNamedMemory, saved_sound_pcm_driver_external)) };
_Static_assert(offsetof(PTNamedMemory, saved_sound_pcm_driver_external) == PT_FIELD_sound_pcm_driver_external,
    "sound_pcm_driver_external address");
enum { PT_FIELD_sound_vector_image = (offsetof(PTNamedMemory, saved_serialized_sound_vector_image)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_sound_vector_image) == PT_FIELD_sound_vector_image,
    "sound_vector_image address");
enum { PT_FIELD_sound_vector_filename = (offsetof(PTNamedMemory, saved_sound_vector_filename)) };
_Static_assert(offsetof(PTNamedMemory, saved_sound_vector_filename) == PT_FIELD_sound_vector_filename,
    "sound_vector_filename address");
enum { PT_FIELD_sound_digital_initialized = (offsetof(PTNamedMemory, saved_sound_digital_initialized)) };
_Static_assert(offsetof(PTNamedMemory, saved_sound_digital_initialized) == PT_FIELD_sound_digital_initialized,
    "sound_digital_initialized address");
enum { PT_FIELD_sound_timer_previous_vector = (offsetof(PTNamedMemory, saved_sound_timer_previous_vector)) };
_Static_assert(offsetof(PTNamedMemory, saved_sound_timer_previous_vector) == PT_FIELD_sound_timer_previous_vector,
    "sound_timer_previous_vector address");
enum { PT_FIELD_sound_timer_active = (offsetof(PTNamedMemory, saved_sound_timer_active)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_sound_timer_active) == PT_FIELD_sound_timer_active, "sound_timer_active address");
enum { PT_FIELD_sound_timer_previous_period = (offsetof(PTNamedMemory, saved_sound_timer_previous_period)) };
_Static_assert(offsetof(PTNamedMemory, saved_sound_timer_previous_period) == PT_FIELD_sound_timer_previous_period,
    "sound_timer_previous_period address");
enum { PT_FIELD_sound_timer_period = (offsetof(PTNamedMemory, saved_sound_timer_period)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_sound_timer_period) == PT_FIELD_sound_timer_period, "sound_timer_period address");
enum { PT_FIELD_sound_timer_callback = (offsetof(PTNamedMemory, saved_serialized_sound_timer_callback)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_sound_timer_callback) == PT_FIELD_sound_timer_callback,
    "sound_timer_callback address");
enum { PT_FIELD_sound_config_fallback_word = (offsetof(PTNamedMemory, saved_sound_config_fallback_word)) };
_Static_assert(offsetof(PTNamedMemory, saved_sound_config_fallback_word) == PT_FIELD_sound_config_fallback_word,
    "sound_config_fallback_word address");
enum { PT_FIELD_sound_config_read_mode = (offsetof(PTNamedMemory, saved_sound_config_read_mode)) };
_Static_assert(offsetof(PTNamedMemory, saved_sound_config_read_mode) == PT_FIELD_sound_config_read_mode,
    "sound_config_read_mode address");
enum { PT_FIELD_sound_config_filename = (offsetof(PTNamedMemory, saved_sound_config_filename)) };
_Static_assert(offsetof(PTNamedMemory, saved_sound_config_filename) == PT_FIELD_sound_config_filename,
    "sound_config_filename address");
enum { PT_FIELD_sound_pcm_driver_filename = (offsetof(PTNamedMemory, saved_sound_pcm_driver_filename)) };
_Static_assert(offsetof(PTNamedMemory, saved_sound_pcm_driver_filename) == PT_FIELD_sound_pcm_driver_filename,
    "sound_pcm_driver_filename address");
enum { PT_FIELD_refresh_overflow_format = (offsetof(PTNamedMemory, saved_refresh_overflow_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_refresh_overflow_format) == PT_FIELD_refresh_overflow_format,
    "refresh_overflow_format address");
enum { PT_FIELD_file_open_error_format = (offsetof(PTNamedMemory, saved_file_open_error_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_file_open_error_format) == PT_FIELD_file_open_error_format,
    "file_open_error_format address");
enum { PT_FIELD_file_write_error_format = (offsetof(PTNamedMemory, saved_file_write_error_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_file_write_error_format) == PT_FIELD_file_write_error_format,
    "file_write_error_format address");
enum { PT_FIELD_file_read_error_format = (offsetof(PTNamedMemory, saved_file_read_error_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_file_read_error_format) == PT_FIELD_file_read_error_format,
    "file_read_error_format address");
enum { PT_FIELD_file_missing_retry_format = (offsetof(PTNamedMemory, saved_file_missing_retry_format)) };
_Static_assert(offsetof(PTNamedMemory, saved_file_missing_retry_format) == PT_FIELD_file_missing_retry_format,
    "file_missing_retry_format address");
enum { PT_FIELD_no_memory_continue_prompt = (offsetof(PTNamedMemory, saved_no_memory_continue_prompt)) };
_Static_assert(offsetof(PTNamedMemory, saved_no_memory_continue_prompt) == PT_FIELD_no_memory_continue_prompt,
    "no_memory_continue_prompt address");
enum { PT_FIELD_ai_restaurant_memory_error = (offsetof(PTNamedMemory, saved_ai_restaurant_memory_error)) };
_Static_assert(offsetof(PTNamedMemory, saved_ai_restaurant_memory_error) == PT_FIELD_ai_restaurant_memory_error,
    "ai_restaurant_memory_error address");
enum { PT_FIELD_new_restaurant_memory_error = (offsetof(PTNamedMemory, saved_new_restaurant_memory_error)) };
_Static_assert(offsetof(PTNamedMemory, saved_new_restaurant_memory_error) == PT_FIELD_new_restaurant_memory_error,
    "new_restaurant_memory_error address");
enum { PT_FIELD_bob_second_already_linked_error = (offsetof(PTNamedMemory, saved_bob_second_already_linked_error)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_bob_second_already_linked_error) == PT_FIELD_bob_second_already_linked_error,
    "bob_second_already_linked_error address");
enum { PT_FIELD_bob_shared_capacity_error = (offsetof(PTNamedMemory, saved_bob_shared_capacity_error)) };
_Static_assert(offsetof(PTNamedMemory, saved_bob_shared_capacity_error) == PT_FIELD_bob_shared_capacity_error,
    "bob_shared_capacity_error address");
enum { PT_FIELD_font_error_text = (offsetof(PTNamedMemory, saved_font_error_text)) };
_Static_assert(offsetof(PTNamedMemory, saved_font_error_text) == PT_FIELD_font_error_text, "font_error_text address");
enum { PT_FIELD_joystick_center_x = (offsetof(PTNamedMemory, saved_joystick_center_x)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_joystick_center_x) == PT_FIELD_joystick_center_x, "joystick_center_x address");
enum { PT_FIELD_joystick_center_y = (offsetof(PTNamedMemory, saved_joystick_center_y)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_joystick_center_y) == PT_FIELD_joystick_center_y, "joystick_center_y address");
enum { PT_FIELD_guest_psp_selector = (offsetof(PTNamedMemory, saved_guest_psp_selector)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_guest_psp_selector) == PT_FIELD_guest_psp_selector, "guest_psp_selector address");
enum { PT_FIELD_guest_dos_heap_reserve = (offsetof(PTNamedMemory, saved_guest_dos_heap_reserve)) };
_Static_assert(offsetof(PTNamedMemory, saved_guest_dos_heap_reserve) == PT_FIELD_guest_dos_heap_reserve,
    "guest_dos_heap_reserve address");
enum { PT_FIELD_legacy_checksum_data = (offsetof(PTNamedMemory, saved_serialized_legacy_checksum_data)) };
_Static_assert(offsetof(PTNamedMemory, saved_serialized_legacy_checksum_data) == PT_FIELD_legacy_checksum_data,
    "legacy_checksum_data address");
enum { PT_FIELD_legacy_checksum_value = (offsetof(PTNamedMemory, saved_legacy_checksum_value)) };
_Static_assert(offsetof(PTNamedMemory, saved_legacy_checksum_value) == PT_FIELD_legacy_checksum_value,
    "legacy_checksum_value address");
enum { PT_FIELD_legacy_checksum_size = (offsetof(PTNamedMemory, saved_legacy_checksum_size)) };
_Static_assert(offsetof(PTNamedMemory, saved_legacy_checksum_size) == PT_FIELD_legacy_checksum_size,
    "legacy_checksum_size address");
enum { PT_FIELD_personal_character_image_format = (offsetof(PTNamedMemory, saved_personal_character_image_format)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_personal_character_image_format) == PT_FIELD_personal_character_image_format,
    "personal_character_image_format address");
enum { PT_FIELD_legacy_round_half = (offsetof(PTNamedMemory, saved_legacy_round_half)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_legacy_round_half) == PT_FIELD_legacy_round_half, "legacy_round_half address");
enum { PT_FIELD_legacy_scale_word_byte_divisor = (offsetof(PTNamedMemory, saved_legacy_scale_word_byte_divisor)) };
_Static_assert(offsetof(PTNamedMemory, saved_legacy_scale_word_byte_divisor) == PT_FIELD_legacy_scale_word_byte_divisor,
    "legacy_scale_word_byte_divisor address");
enum {
  PT_FIELD_legacy_scale_difference_byte_divisor = (offsetof(PTNamedMemory, saved_legacy_scale_difference_byte_divisor))
};
_Static_assert(offsetof(PTNamedMemory, saved_legacy_scale_difference_byte_divisor) ==
        PT_FIELD_legacy_scale_difference_byte_divisor,
    "legacy_scale_difference_byte_divisor address");
enum { PT_FIELD_legacy_scale_word_word_divisor = (offsetof(PTNamedMemory, saved_legacy_scale_word_word_divisor)) };
_Static_assert(offsetof(PTNamedMemory, saved_legacy_scale_word_word_divisor) == PT_FIELD_legacy_scale_word_word_divisor,
    "legacy_scale_word_word_divisor address");
enum {
  PT_FIELD_legacy_scale_difference_word_divisor = (offsetof(PTNamedMemory, saved_legacy_scale_difference_word_divisor))
};
_Static_assert(offsetof(PTNamedMemory, saved_legacy_scale_difference_word_divisor) ==
        PT_FIELD_legacy_scale_difference_word_divisor,
    "legacy_scale_difference_word_divisor address");
enum { PT_FIELD_legacy_collision_x = (offsetof(PTNamedMemory, saved_legacy_collision_x)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_legacy_collision_x) == PT_FIELD_legacy_collision_x, "legacy_collision_x address");
enum { PT_FIELD_legacy_collision_y = (offsetof(PTNamedMemory, saved_legacy_collision_y)) };
_Static_assert(
    offsetof(PTNamedMemory, saved_legacy_collision_y) == PT_FIELD_legacy_collision_y, "legacy_collision_y address");
enum { PT_FIELD_legacy_walk_x = (offsetof(PTNamedMemory, saved_legacy_walk_x)) };
_Static_assert(offsetof(PTNamedMemory, saved_legacy_walk_x) == PT_FIELD_legacy_walk_x, "legacy_walk_x address");
enum { PT_FIELD_legacy_walk_y = (offsetof(PTNamedMemory, saved_legacy_walk_y)) };
_Static_assert(offsetof(PTNamedMemory, saved_legacy_walk_y) == PT_FIELD_legacy_walk_y, "legacy_walk_y address");
enum { PT_FIELD_legacy_walk_direction = (offsetof(PTNamedMemory, saved_legacy_walk_direction)) };
_Static_assert(offsetof(PTNamedMemory, saved_legacy_walk_direction) == PT_FIELD_legacy_walk_direction,
    "legacy_walk_direction address");
enum { PT_FIELD_legacy_walk_animation_phase = (offsetof(PTNamedMemory, saved_legacy_walk_animation_phase)) };
_Static_assert(offsetof(PTNamedMemory, saved_legacy_walk_animation_phase) == PT_FIELD_legacy_walk_animation_phase,
    "legacy_walk_animation_phase address");
#define PT_NAMED_GLOBAL_RANGES                                                                                         \
  {PT_FIELD_sound_binary_read_mode, sizeof(sound_binary_read_mode)},                                                   \
      {PT_FIELD_land_symbol_filename, sizeof(land_symbol_filename)},                                                   \
      {PT_FIELD_staff_hire_role_labels, sizeof(staff_hire_role_labels)},                                               \
      {PT_FIELD_contract_justification_probabilities, sizeof(contract_justification_probabilities)},                   \
      {PT_FIELD_contract_days_literal, sizeof(contract_days_literal)},                                                 \
      {PT_FIELD_manager_turnover_names, sizeof(manager_turnover_names)},                                               \
      {PT_FIELD_attack_health_left, sizeof(attack_health_left)},                                                       \
      {PT_FIELD_attack_health_right, sizeof(attack_health_right)}, {PT_FIELD_recipe_jury_x, sizeof(recipe_jury_x)},    \
      {PT_FIELD_recipe_chart_label_x, sizeof(recipe_chart_label_x)},                                                   \
      {PT_FIELD_recipe_chart_x, sizeof(recipe_chart_x)},                                                               \
      {PT_FIELD_ingredient_trend_names, sizeof(ingredient_trend_names)},                                               \
      {PT_FIELD_selector_patterns, sizeof(selector_patterns)},                                                         \
      {PT_FIELD_animated_quality_x, sizeof(animated_quality_x)},                                                       \
      {PT_FIELD_animated_quality_y, sizeof(animated_quality_y)},                                                       \
      {PT_FIELD_animated_capacity_colors, sizeof(animated_capacity_colors)},                                           \
      {PT_FIELD_valid_bank_account_numbers, sizeof(valid_bank_account_numbers)},                                       \
      {PT_FIELD_mission_reward_bases, sizeof(mission_reward_bases)},                                                   \
      {PT_FIELD_mission_place_articles, sizeof(mission_place_articles)},                                               \
      {PT_FIELD_mission_place_article_groups, sizeof(mission_place_article_groups)},                                   \
      {PT_FIELD_mission_place_name_prefixes, sizeof(mission_place_name_prefixes)},                                     \
      {PT_FIELD_paragraph_width_characters, sizeof(paragraph_width_characters)},                                       \
      {PT_FIELD_furniture_extras_labels, sizeof(furniture_extras_labels)},                                             \
      {PT_FIELD_profit_graph_colors, sizeof(profit_graph_colors)},                                                     \
      {PT_FIELD_profit_graph_thresholds, sizeof(profit_graph_thresholds)},                                             \
      {PT_FIELD_weekly_default_staff_name, sizeof(weekly_default_staff_name)},                                         \
      {PT_FIELD_sound_vector_registers, sizeof(((PTNamedMemory *)0)->saved_sound_vector_registers)},                   \
      {PT_FIELD_sound_vector_previous_offset, sizeof(((PTNamedMemory *)0)->saved_sound_vector_previous_offset)},       \
      {PT_FIELD_sound_vector_previous_segment, sizeof(((PTNamedMemory *)0)->saved_sound_vector_previous_segment)},     \
      {PT_FIELD_mouse_buttons, sizeof(mouse_buttons)}, {PT_FIELD_input_event_filter, sizeof(input_event_filter)},      \
      {PT_FIELD_last_bios_key, sizeof(last_bios_key)}, {PT_FIELD_command_line_arg, sizeof(command_line_arg)},          \
      {PT_FIELD_serialized_display_timing, sizeof(((PTNamedMemory *)0)->saved_serialized_display_timing)},             \
      {PT_FIELD_in_bitblt, sizeof(in_bitblt)}, {PT_FIELD_mouse_pos_x, sizeof(mouse_pos_x)},                            \
      {PT_FIELD_mouse_pos_y, sizeof(mouse_pos_y)}, {PT_FIELD_mouse_move_pending, sizeof(mouse_move_pending)},          \
      {PT_FIELD_pending_mouse_x, sizeof(pending_mouse_x)}, {PT_FIELD_pending_mouse_y, sizeof(pending_mouse_y)},        \
      {PT_FIELD_cursor_offset_x, sizeof(cursor_offset_x)}, {PT_FIELD_cursor_offset_y, sizeof(cursor_offset_y)},        \
      {PT_FIELD_cursor_is_visible, sizeof(cursor_is_visible)},                                                         \
      {PT_FIELD_did_hide_mouse_cursor, sizeof(did_hide_mouse_cursor)},                                                 \
      {PT_FIELD_cursor_hide_depth, sizeof(cursor_hide_depth)}, {PT_FIELD_halve_mouse_x, sizeof(halve_mouse_x)},        \
      {PT_FIELD_cursor_screen_buffer, sizeof(((PTNamedMemory *)0)->saved_serialized_cursor_screen_buffer)},            \
      {PT_FIELD_mouse_binding_count, sizeof(mouse_binding_count)},                                                     \
      {PT_FIELD_mouse_position_bindings, sizeof(((PTNamedMemory *)0)->saved_serialized_mouse_position_bindings)},      \
      {PT_FIELD_transparent_color, sizeof(transparent_color)},                                                         \
      {PT_FIELD_glyph_bitmap, sizeof(((PTNamedMemory *)0)->saved_serialized_glyph_bitmap)},                            \
      {PT_FIELD_glyph_pixels, sizeof(glyph_pixels)}, {PT_FIELD_input_queue_capacity, sizeof(input_queue_capacity)},    \
      {PT_FIELD_input_queue_head, sizeof(input_queue_head)}, {PT_FIELD_input_queue_count, sizeof(input_queue_count)},  \
      {PT_FIELD_input_events, sizeof(input_events)}, {PT_FIELD_smallrects, sizeof(smallrects)},                        \
      {PT_FIELD_num_ingredients_in_set, sizeof(num_ingredients_in_set)},                                               \
      {PT_FIELD_customer_age_names, sizeof(customer_age_names)}, {PT_FIELD_customer_types, sizeof(customer_types)},    \
      {PT_FIELD_property_names, sizeof(property_names)},                                                               \
      {PT_FIELD_property_availability_names, sizeof(property_availability_names)},                                     \
      {PT_FIELD_ingredient_popup_x, sizeof(ingredient_popup_x)},                                                       \
      {PT_FIELD_g_stock_name_kind, sizeof(g_stock_name_kind)}, {PT_FIELD_furniture_desc, sizeof(furniture_desc)},      \
      {PT_FIELD_furniture_supplier_count, sizeof(furniture_supplier_count)},                                           \
      {PT_FIELD_furniture_suppliers, sizeof(furniture_suppliers)}, {PT_FIELD_exit_code, sizeof(exit_code)},            \
      {PT_FIELD_force_map_input, sizeof(force_map_input)}, {PT_FIELD_days_in_month, sizeof(days_in_month)},            \
      {PT_FIELD_menu_font, sizeof(((PTNamedMemory *)0)->saved_serialized_menu_font)},                                  \
      {PT_FIELD_ruby_font, sizeof(((PTNamedMemory *)0)->saved_serialized_ruby_font)},                                  \
      {PT_FIELD_font_desc_cstandar, sizeof(((PTNamedMemory *)0)->saved_serialized_font_desc_cstandar)},                \
      {PT_FIELD_font_desc_piznumb, sizeof(((PTNamedMemory *)0)->saved_serialized_font_desc_piznumb)},                  \
      {PT_FIELD_font_desc_astandar, sizeof(((PTNamedMemory *)0)->saved_serialized_font_desc_astandar)},                \
      {PT_FIELD_font_desc_supersma, sizeof(((PTNamedMemory *)0)->saved_serialized_font_desc_supersma)},                \
      {PT_FIELD_loaded_font, sizeof(((PTNamedMemory *)0)->saved_serialized_loaded_font)},                              \
      {PT_FIELD_temp_canvas, sizeof(((PTNamedMemory *)0)->saved_serialized_temp_canvas)},                              \
      {PT_FIELD_bmp25_ptr, sizeof(((PTNamedMemory *)0)->saved_serialized_bmp25_ptr)},                                  \
      {PT_FIELD_bmp24_ptr, sizeof(((PTNamedMemory *)0)->saved_serialized_bmp24_ptr)},                                  \
      {PT_FIELD_bmp_character_face, sizeof(((PTNamedMemory *)0)->saved_serialized_bmp_character_face)},                \
      {PT_FIELD_bmp22_ptr, sizeof(((PTNamedMemory *)0)->saved_serialized_bmp22_ptr)},                                  \
      {PT_FIELD_bmp21_ptr, sizeof(((PTNamedMemory *)0)->saved_serialized_bmp21_ptr)},                                  \
      {PT_FIELD_bmp20_ptr, sizeof(((PTNamedMemory *)0)->saved_serialized_bmp20_ptr)},                                  \
      {PT_FIELD_bmp26_ptr, sizeof(((PTNamedMemory *)0)->saved_serialized_bmp26_ptr)},                                  \
      {PT_FIELD_text_edit_bitmap, sizeof(((PTNamedMemory *)0)->saved_serialized_text_edit_bitmap)},                    \
      {PT_FIELD_temp_bitmap_84x6, sizeof(((PTNamedMemory *)0)->saved_serialized_temp_bitmap_84x6)},                    \
      {PT_FIELD_restaurant_mask_rect, sizeof(restaurant_mask_rect)},                                                   \
      {PT_FIELD_rect_1, sizeof(((PTNamedMemory *)0)->saved_serialized_rect_1)},                                        \
      {PT_FIELD_bob_restaurant, sizeof(((PTNamedMemory *)0)->saved_serialized_bob_restaurant)},                        \
      {PT_FIELD_bob_ingredients, sizeof(((PTNamedMemory *)0)->saved_serialized_bob_ingredients)},                      \
      {PT_FIELD_bob_kitchen, sizeof(((PTNamedMemory *)0)->saved_serialized_bob_kitchen)},                              \
      {PT_FIELD_bob_restaurant_open, sizeof(((PTNamedMemory *)0)->saved_serialized_bob_restaurant_open)},              \
      {PT_FIELD_bob_stanbobs, sizeof(((PTNamedMemory *)0)->saved_serialized_bob_stanbobs)},                            \
      {PT_FIELD_bob_typ, sizeof(((PTNamedMemory *)0)->saved_serialized_bob_typ)},                                      \
      {PT_FIELD_bob_walk, sizeof(((PTNamedMemory *)0)->saved_serialized_bob_walk)},                                    \
      {PT_FIELD_bob7, sizeof(((PTNamedMemory *)0)->saved_serialized_bob7)},                                            \
      {PT_FIELD_bob11, sizeof(((PTNamedMemory *)0)->saved_serialized_bob11)},                                          \
      {PT_FIELD_menu_bob, sizeof(((PTNamedMemory *)0)->saved_serialized_menu_bob)},                                    \
      {PT_FIELD_bob_restaurant_stats, sizeof(((PTNamedMemory *)0)->saved_serialized_bob_restaurant_stats)},            \
      {PT_FIELD_bob9, sizeof(((PTNamedMemory *)0)->saved_serialized_bob9)},                                            \
      {PT_FIELD_langfile_artikel3, sizeof(((PTNamedMemory *)0)->saved_serialized_langfile_artikel3)},                  \
      {PT_FIELD_langfile_credits, sizeof(((PTNamedMemory *)0)->saved_serialized_langfile_credits)},                    \
      {PT_FIELD_langfile_ende, sizeof(((PTNamedMemory *)0)->saved_serialized_langfile_ende)},                          \
      {PT_FIELD_langfile_gtalk0, sizeof(((PTNamedMemory *)0)->saved_serialized_langfile_gtalk0)},                      \
      {PT_FIELD_langfile_gtalk1, sizeof(((PTNamedMemory *)0)->saved_serialized_langfile_gtalk1)},                      \
      {PT_FIELD_langfile_markt, sizeof(((PTNamedMemory *)0)->saved_serialized_langfile_markt)},                        \
      {PT_FIELD_langfile_action, sizeof(((PTNamedMemory *)0)->saved_serialized_langfile_action)},                      \
      {PT_FIELD_langfile_werbung, sizeof(((PTNamedMemory *)0)->saved_serialized_langfile_werbung)},                    \
      {PT_FIELD_langfile_levent, sizeof(((PTNamedMemory *)0)->saved_serialized_langfile_levent)},                      \
      {PT_FIELD_langfile_mafia, sizeof(((PTNamedMemory *)0)->saved_serialized_langfile_mafia)},                        \
      {PT_FIELD_langfile_waffen, sizeof(((PTNamedMemory *)0)->saved_serialized_langfile_waffen)},                      \
      {PT_FIELD_langfile_realtors, sizeof(((PTNamedMemory *)0)->saved_serialized_langfile_realtors)},                  \
      {PT_FIELD_langfile_bank, sizeof(((PTNamedMemory *)0)->saved_serialized_langfile_bank)},                          \
      {PT_FIELD_langfile_insurance, sizeof(((PTNamedMemory *)0)->saved_serialized_langfile_insurance)},                \
      {PT_FIELD_langfile_townhall, sizeof(((PTNamedMemory *)0)->saved_serialized_langfile_townhall)},                  \
      {PT_FIELD_langfile_police, sizeof(((PTNamedMemory *)0)->saved_serialized_langfile_police)},                      \
      {PT_FIELD_langfile_lief, sizeof(((PTNamedMemory *)0)->saved_serialized_langfile_lief)},                          \
      {PT_FIELD_langfile_leben, sizeof(((PTNamedMemory *)0)->saved_serialized_langfile_leben)},                        \
      {PT_FIELD_langfile_pers, sizeof(((PTNamedMemory *)0)->saved_serialized_langfile_pers)},                          \
      {PT_FIELD_langfile_persf, sizeof(((PTNamedMemory *)0)->saved_serialized_langfile_persf)},                        \
      {PT_FIELD_langfile_staff_applications,                                                                           \
        sizeof(((PTNamedMemory *)0)->saved_serialized_langfile_staff_applications)},                                   \
      {PT_FIELD_langfile_double, sizeof(((PTNamedMemory *)0)->saved_serialized_langfile_double)},                      \
      {PT_FIELD_mouse_over_zone, sizeof(((PTNamedMemory *)0)->saved_serialized_mouse_over_zone)},                      \
      {PT_FIELD_last_mouse_over_district, sizeof(((PTNamedMemory *)0)->saved_serialized_last_mouse_over_district)},    \
      {PT_FIELD_town_names, sizeof(town_names)}, {PT_FIELD_prefs, sizeof(prefs)},                                      \
      {PT_FIELD_staff_ui_file_animation, sizeof(staff_ui_file_animation)},                                             \
      {PT_FIELD_state_slot_007, sizeof(state_slot_007)}, {PT_FIELD_disable_draw_door, sizeof(disable_draw_door)},      \
      {PT_FIELD_terminal_manual_mode, sizeof(terminal_manual_mode)},                                                   \
      {PT_FIELD_state_slot_008, sizeof(state_slot_008)}, {PT_FIELD_palette_fade_algo, sizeof(palette_fade_algo)},      \
      {PT_FIELD_special_debt_flag, sizeof(special_debt_flag)},                                                         \
      {PT_FIELD_disable_init_country_progress_msg, sizeof(disable_init_country_progress_msg)},                         \
      {PT_FIELD_main_menu_canvas_flag, sizeof(main_menu_canvas_flag)},                                                 \
      {PT_FIELD_state_slot_009, sizeof(state_slot_009)}, {PT_FIELD_game_exit_requested, sizeof(game_exit_requested)},  \
      {PT_FIELD_has_many_players, sizeof(has_many_players)},                                                           \
      {PT_FIELD_unit_for_number_query, sizeof(((PTNamedMemory *)0)->saved_serialized_unit_for_number_query)},          \
      {PT_FIELD_jatoba_initializer, sizeof(jatoba_initializer)},                                                       \
      {PT_FIELD_mouse_initializer, sizeof(((PTNamedMemory *)0)->saved_serialized_mouse_initializer)},                  \
      {PT_FIELD_paragraph_highlight_override, sizeof(paragraph_highlight_override)},                                   \
      {PT_FIELD_season_map_colors, sizeof(season_map_colors)},                                                         \
      {PT_FIELD_map_animation_palette_index, sizeof(map_animation_palette_index)},                                     \
      {PT_FIELD_g_button_config, sizeof(g_button_config)}, {PT_FIELD_button_icon_ids, sizeof(button_icon_ids)},        \
      {PT_FIELD_button_texts, sizeof(((PTNamedMemory *)0)->saved_serialized_button_texts)},                            \
      {PT_FIELD_button_x, sizeof(button_x)}, {PT_FIELD_button_y, sizeof(button_y)},                                    \
      {PT_FIELD_player_ptr, sizeof(((PTNamedMemory *)0)->saved_serialized_player_ptr)},                                \
      {PT_FIELD_character_ptr, sizeof(((PTNamedMemory *)0)->saved_serialized_character_ptr)},                          \
      {PT_FIELD_invoice_month_names, sizeof(invoice_month_names)},                                                     \
      {PT_FIELD_cur_town, sizeof(((PTNamedMemory *)0)->saved_serialized_cur_town)},                                    \
      {PT_FIELD_pizza_menu_info_separator, sizeof(pizza_menu_info_separator)},                                         \
      {PT_FIELD_customer_eating_times, sizeof(customer_eating_times)},                                                 \
      {PT_FIELD_empty_restaurant_floor, sizeof(empty_restaurant_floor)}, {PT_FIELD_ingredients, sizeof(ingredients)},  \
      {PT_FIELD_serialized_age_taste_preferences,                                                                      \
        sizeof(((PTNamedMemory *)0)->saved_serialized_age_taste_preferences)},                                         \
      {PT_FIELD_state_slot_010, sizeof(state_slot_010)}, {PT_FIELD_intro_text_groups, sizeof(intro_text_groups)},      \
      {PT_FIELD_staff_ui_manager_reactions, sizeof(staff_ui_manager_reactions)},                                       \
      {PT_FIELD_staff_ui_manager_behaviors, sizeof(staff_ui_manager_behaviors)},                                       \
      {PT_FIELD_staff_training_courses, sizeof(staff_training_courses)},                                               \
      {PT_FIELD_staff_course_role_labels, sizeof(staff_course_role_labels)},                                           \
      {PT_FIELD_goal_ui_colors, sizeof(goal_ui_colors)}, {PT_FIELD_curriculum_scores, sizeof(curriculum_scores)},      \
      {PT_FIELD_loaded_awards_bitmap, sizeof(loaded_awards_bitmap)},                                                   \
      {PT_FIELD_character_curriculum_flag, sizeof(character_curriculum_flag)},                                         \
      {PT_FIELD_curriculum_page, sizeof(curriculum_page)},                                                             \
      {PT_FIELD_character_mode_names, sizeof(character_mode_names)},                                                   \
      {PT_FIELD_index_to_rank_value, sizeof(index_to_rank_value)},                                                     \
      {PT_FIELD_bitmap_to_show_for_goal, sizeof(bitmap_to_show_for_goal)},                                             \
      {PT_FIELD_goal_bitmap_flags, sizeof(goal_bitmap_flags)},                                                         \
      {PT_FIELD_contract_insolvency_text, sizeof(contract_insolvency_text)},                                           \
      {PT_FIELD_depot_capacity_exhausted_text, sizeof(depot_capacity_exhausted_text)},                                 \
      {PT_FIELD_empty_translation, sizeof(empty_translation)},                                                         \
      {PT_FIELD_translation_error_text, sizeof(translation_error_text)},                                               \
      {PT_FIELD_mouse_bitmap_data_1, sizeof(mouse_bitmap_data_1)},                                                     \
      {PT_FIELD_mouse_bitmap_data_2, sizeof(mouse_bitmap_data_2)},                                                     \
      {PT_FIELD_manager_role_names, sizeof(manager_role_names)},                                                       \
      {PT_FIELD_country_selector_x, sizeof(country_selector_x)},                                                       \
      {PT_FIELD_country_selector_y, sizeof(country_selector_y)},                                                       \
      {PT_FIELD_attack_walk_images, sizeof(attack_walk_images)}, {PT_FIELD_guest_seat_x, sizeof(guest_seat_x)},        \
      {PT_FIELD_guest_seat_y, sizeof(guest_seat_y)}, {PT_FIELD_guest_body_x, sizeof(guest_body_x)},                    \
      {PT_FIELD_guest_body_y, sizeof(guest_body_y)}, {PT_FIELD_guest_pose_x, sizeof(guest_pose_x)},                    \
      {PT_FIELD_guest_pose_y, sizeof(guest_pose_y)}, {PT_FIELD_attack_collision_x, sizeof(attack_collision_x)},        \
      {PT_FIELD_attack_collision_y, sizeof(attack_collision_y)}, {PT_FIELD_attack_head0_x, sizeof(attack_head0_x)},    \
      {PT_FIELD_attack_head0_y, sizeof(attack_head0_y)}, {PT_FIELD_attack_head1_x, sizeof(attack_head1_x)},            \
      {PT_FIELD_attack_head1_y, sizeof(attack_head1_y)}, {PT_FIELD_guest_carry_x, sizeof(guest_carry_x)},              \
      {PT_FIELD_guest_carry_y, sizeof(guest_carry_y)}, {PT_FIELD_attack_held_weapon_x, sizeof(attack_held_weapon_x)},  \
      {PT_FIELD_attack_held_weapon_y, sizeof(attack_held_weapon_y)},                                                   \
      {PT_FIELD_attack_knife_x, sizeof(attack_knife_x)}, {PT_FIELD_attack_spray_x, sizeof(attack_spray_x)},            \
      {PT_FIELD_attack_knife_y, sizeof(attack_knife_y)}, {PT_FIELD_attack_spray_y, sizeof(attack_spray_y)},            \
      {PT_FIELD_attack_projectile_x, sizeof(attack_projectile_x)},                                                     \
      {PT_FIELD_attack_projectile_y, sizeof(attack_projectile_y)},                                                     \
      {PT_FIELD_attack_explosion_x, sizeof(attack_explosion_x)},                                                       \
      {PT_FIELD_attack_explosion_y, sizeof(attack_explosion_y)},                                                       \
      {PT_FIELD_g_num_active_pigs, sizeof(g_num_active_pigs)}, {PT_FIELD_rest_path_depth, sizeof(rest_path_depth)},    \
      {PT_FIELD_guest_idle_script, sizeof(guest_idle_script)},                                                         \
      {PT_FIELD_attack_weapon_reach, sizeof(attack_weapon_reach)}, {PT_FIELD_attack_step_x, sizeof(attack_step_x)},    \
      {PT_FIELD_attack_step_y, sizeof(attack_step_y)},                                                                 \
      {PT_FIELD_pizza_competition_delay, sizeof(pizza_competition_delay)},                                             \
      {PT_FIELD_pizza_level, sizeof(pizza_level)}, {PT_FIELD_default_recipe_names, sizeof(default_recipe_names)},      \
      {PT_FIELD_main_menu_desc, sizeof(main_menu_desc)}, {PT_FIELD_main_menu_strings, sizeof(main_menu_strings)},      \
      {PT_FIELD_selector_family_titles, sizeof(selector_family_titles)},                                               \
      {PT_FIELD_state_slot_023, sizeof(state_slot_023)},                                                               \
      {PT_FIELD_last_customer_count_poi, sizeof(((PTNamedMemory *)0)->saved_serialized_last_customer_count_poi)},      \
      {PT_FIELD_curr_map_mode, sizeof(curr_map_mode)}, {PT_FIELD_selected_poi_attr, sizeof(selected_poi_attr)},        \
      {PT_FIELD_selected_poi_player, sizeof(selected_poi_player)},                                                     \
      {PT_FIELD_map_marker_toggle, sizeof(map_marker_toggle)}, {PT_FIELD_map_tabs_order, sizeof(map_tabs_order)},      \
      {PT_FIELD_owner_to_color, sizeof(owner_to_color)}, {PT_FIELD_map_tab_step, sizeof(map_tab_step)},                \
      {PT_FIELD_map_tab_base, sizeof(map_tab_base)}, {PT_FIELD_tabs_y_values, sizeof(tabs_y_values)},                  \
      {PT_FIELD_map_tab_labels, sizeof(map_tab_labels)},                                                               \
      {PT_FIELD_district_map_pattern, sizeof(district_map_pattern)},                                                   \
      {PT_FIELD_terrain_tile_colors, sizeof(terrain_tile_colors)},                                                     \
      {PT_FIELD_season_names, sizeof(((PTNamedMemory *)0)->saved_season_names)},                                       \
      {PT_FIELD_month_to_arstid, sizeof(month_to_arstid)}, {PT_FIELD_terrain_names, sizeof(terrain_names)},            \
      {PT_FIELD_map_marker_visible, sizeof(map_marker_visible)},                                                       \
      {PT_FIELD_map_animation_palette_ticks, sizeof(map_animation_palette_ticks)},                                     \
      {PT_FIELD_animated_production_x, sizeof(animated_production_x)},                                                 \
      {PT_FIELD_animated_production_y, sizeof(animated_production_y)},                                                 \
      {PT_FIELD_palette_in_callbackground, sizeof(palette_in_callbackground)},                                         \
      {PT_FIELD_last_loaded_bob_typ, sizeof(last_loaded_bob_typ)},                                                     \
      {PT_FIELD_phone_face_five_x, sizeof(phone_face_five_x)},                                                         \
      {PT_FIELD_phone_face_five_y, sizeof(phone_face_five_y)}, {PT_FIELD_phone_face_y, sizeof(phone_face_y)},          \
      {PT_FIELD_phone_background_face_x, sizeof(phone_background_face_x)},                                             \
      {PT_FIELD_phone_background_face_y, sizeof(phone_background_face_y)},                                             \
      {PT_FIELD_terminal_category_names, sizeof(terminal_category_names)},                                             \
      {PT_FIELD_terminal_dial_messages, sizeof(terminal_dial_messages)},                                               \
      {PT_FIELD_terminal_waiting_message, sizeof(terminal_waiting_message)},                                           \
      {PT_FIELD_dial_number_position, sizeof(dial_number_position)},                                                   \
      {PT_FIELD_insurance_claim_reason_by_type, sizeof(insurance_claim_reason_by_type)},                               \
      {PT_FIELD_townhall_competition_messages, sizeof(townhall_competition_messages)},                                 \
      {PT_FIELD_mission_item_pronouns, sizeof(mission_item_pronouns)},                                                 \
      {PT_FIELD_mission_pronouns, sizeof(mission_pronouns)},                                                           \
      {PT_FIELD_mission_completion_goals, sizeof(mission_completion_goals)},                                           \
      {PT_FIELD_mission_date_buffer_index, sizeof(mission_date_buffer_index)},                                         \
      {PT_FIELD_mission_place_buffer_index, sizeof(mission_place_buffer_index)},                                       \
      {PT_FIELD_arms_dealer_phone_nr, sizeof(arms_dealer_phone_nr)},                                                   \
      {PT_FIELD_last_input_tick, sizeof(last_input_tick)}, {PT_FIELD_last_second_tick, sizeof(last_second_tick)},      \
      {PT_FIELD_secondary_filename, sizeof(((PTNamedMemory *)0)->saved_serialized_secondary_filename)},                \
      {PT_FIELD_info_location_names, sizeof(info_location_names)},                                                     \
      {PT_FIELD_current_button_hint, sizeof(((PTNamedMemory *)0)->saved_serialized_current_button_hint)},              \
      {PT_FIELD_input_timer_phase, sizeof(input_timer_phase)},                                                         \
      {PT_FIELD_market_zone_names, sizeof(market_zone_names)},                                                         \
      {PT_FIELD_market_bar_colors, sizeof(market_bar_colors)},                                                         \
      {PT_FIELD_guest_food_offset_x, sizeof(guest_food_offset_x)},                                                     \
      {PT_FIELD_guest_food_offset_y, sizeof(guest_food_offset_y)},                                                     \
      {PT_FIELD_guest_lower_offset_x, sizeof(guest_lower_offset_x)},                                                   \
      {PT_FIELD_guest_lower_offset_y, sizeof(guest_lower_offset_y)},                                                   \
      {PT_FIELD_guest_upper_offset_x, sizeof(guest_upper_offset_x)},                                                   \
      {PT_FIELD_guest_upper_offset_y, sizeof(guest_upper_offset_y)},                                                   \
      {PT_FIELD_placement_contact_count, sizeof(placement_contact_count)},                                             \
      {PT_FIELD_furniture_refresh_depth, sizeof(furniture_refresh_depth)},                                             \
      {PT_FIELD_furniture_category_names, sizeof(furniture_category_names)},                                           \
      {PT_FIELD_restaurant_media_labels, sizeof(restaurant_media_labels)},                                             \
      {PT_FIELD_competition_sound_pending, sizeof(competition_sound_pending)},                                         \
      {PT_FIELD_sound_config_flag, sizeof(sound_config_flag)}, {PT_FIELD_sound_var_3, sizeof(sound_var_3)},            \
      {PT_FIELD_sound_buffer, sizeof(((PTNamedMemory *)0)->saved_serialized_sound_buffer)},                            \
      {PT_FIELD_current_music_index, sizeof(current_music_index)}, {PT_FIELD_state_slot_011, sizeof(state_slot_011)},  \
      {PT_FIELD_competition_sound_id, sizeof(competition_sound_id)},                                                   \
      {PT_FIELD_selected_pizza_load_idx, sizeof(selected_pizza_load_idx)},                                             \
      {PT_FIELD_finance_stats_categories, sizeof(finance_stats_categories)},                                           \
      {PT_FIELD_daily_balance_flag, sizeof(daily_balance_flag)},                                                       \
      {PT_FIELD_monthly_receipt_labels, sizeof(monthly_receipt_labels)},                                               \
      {PT_FIELD_monthly_expense_labels, sizeof(monthly_expense_labels)},                                               \
      {PT_FIELD_insurance_category_labels, sizeof(insurance_category_labels)},                                         \
      {PT_FIELD_advert_tv_frames, sizeof(advert_tv_frames)},                                                           \
      {PT_FIELD_newspaper_article_backgrounds, sizeof(newspaper_article_backgrounds)},                                 \
      {PT_FIELD_had_holiday, sizeof(had_holiday)}, {PT_FIELD_next_event_to_do, sizeof(next_event_to_do)},              \
      {PT_FIELD_suppliers_daily_loop_index, sizeof(suppliers_daily_loop_index)},                                       \
      {PT_FIELD_month_names, sizeof(month_names)}, {PT_FIELD_ordinal_suffixes, sizeof(ordinal_suffixes)},              \
      {PT_FIELD_pt_random_state, sizeof(pt_random_state)},                                                             \
      {PT_FIELD_guest_heap_regions, sizeof(((PTNamedMemory *)0)->saved_serialized_guest_heap_regions)},                \
      {PT_FIELD_guest_heap_rover, sizeof(((PTNamedMemory *)0)->saved_serialized_guest_heap_rover)},                    \
      {PT_FIELD_guest_heap_largest_before_rover, sizeof(guest_heap_largest_before_rover)},                             \
      {PT_FIELD_guest_heap_break, sizeof(guest_heap_break)},                                                           \
      {PT_FIELD_guest_stack_lower_bound, sizeof(((PTNamedMemory *)0)->saved_guest_stack_lower_bound)},                 \
      {PT_FIELD_guest_console_cached_character, sizeof(guest_console_cached_character)},                               \
      {PT_FIELD_state_slot_012, sizeof(state_slot_012)}, {PT_FIELD_state_slot_013, sizeof(state_slot_013)},            \
      {PT_FIELD_guest_dos_major, sizeof(guest_dos_major)},                                                             \
      {PT_FIELD_guest_file_streams, sizeof(((PTNamedMemory *)0)->saved_serialized_guest_file_streams)},                \
      {PT_FIELD_guest_default_open_mode, sizeof(guest_default_open_mode)},                                             \
      {PT_FIELD_guest_dos_error_errno, sizeof(guest_dos_error_errno)},                                                 \
      {PT_FIELD_guest_permission_mask, sizeof(guest_permission_mask)},                                                 \
      {PT_FIELD_guest_io_mode_capacity, sizeof(guest_io_mode_capacity)},                                               \
      {PT_FIELD_guest_io_modes, sizeof(guest_io_modes)},                                                               \
      {PT_FIELD_guest_io_mode_table, sizeof(((PTNamedMemory *)0)->saved_serialized_guest_io_mode_table)},              \
      {PT_FIELD_timezone_start_rule, sizeof(timezone_start_rule)},                                                     \
      {PT_FIELD_timezone_end_rule, sizeof(timezone_end_rule)}, {PT_FIELD_timezone_seconds, sizeof(timezone_seconds)},  \
      {PT_FIELD_timezone_dst_seconds, sizeof(timezone_dst_seconds)},                                                   \
      {PT_FIELD_timezone_has_dst, sizeof(timezone_has_dst)},                                                           \
      {PT_FIELD_timezone_standard_name, sizeof(timezone_standard_name)},                                               \
      {PT_FIELD_timezone_dst_name, sizeof(timezone_dst_name)},                                                         \
      {PT_FIELD_guest_heap_grow_enabled, sizeof(guest_heap_grow_enabled)},                                             \
      {PT_FIELD_guest_heap_growth_minimum, sizeof(guest_heap_growth_minimum)},                                         \
      {PT_FIELD_guest_environment, sizeof(((PTNamedMemory *)0)->saved_serialized_guest_environment)},                  \
      {PT_FIELD_market_property_title, sizeof(market_property_title)},                                                 \
      {PT_FIELD_graphics_palette_filename, sizeof(graphics_palette_filename)},                                         \
      {PT_FIELD_intro_presents, sizeof(intro_presents)}, {PT_FIELD_intro_logo_filename, sizeof(intro_logo_filename)},  \
      {PT_FIELD_intro_cyber_filename, sizeof(intro_cyber_filename)}, {PT_FIELD_intro_version, sizeof(intro_version)},  \
      {PT_FIELD_intro_title1_filename, sizeof(intro_title1_filename)},                                                 \
      {PT_FIELD_intro_title2_filename, sizeof(intro_title2_filename)},                                                 \
      {PT_FIELD_startup_permap_filename, sizeof(startup_permap_filename)},                                             \
      {PT_FIELD_startup_icons_filename, sizeof(startup_icons_filename)},                                               \
      {PT_FIELD_startup_k0_filename, sizeof(startup_k0_filename)},                                                     \
      {PT_FIELD_startup_med1_filename, sizeof(startup_med1_filename)},                                                 \
      {PT_FIELD_main_sound_config_filename, sizeof(main_sound_config_filename)},                                       \
      {PT_FIELD_main_prefs_filename, sizeof(main_prefs_filename)},                                                     \
      {PT_FIELD_quickstart_restaurant_file_format,                                                                     \
        sizeof(((PTNamedMemory *)0)->saved_quickstart_restaurant_file_format)},                                        \
      {PT_FIELD_town_filename_format, sizeof(((PTNamedMemory *)0)->saved_town_filename_format)},                       \
      {PT_FIELD_city_filename_format, sizeof(((PTNamedMemory *)0)->saved_city_filename_format)},                       \
      {PT_FIELD_recipe_allocation_failure_text, sizeof(recipe_allocation_failure_text)},                               \
      {PT_FIELD_standard_recipe_file_format, sizeof(((PTNamedMemory *)0)->saved_standard_recipe_file_format)},         \
      {PT_FIELD_map_filename_format, sizeof(((PTNamedMemory *)0)->saved_map_filename_format)},                         \
      {PT_FIELD_bez_filename_format, sizeof(((PTNamedMemory *)0)->saved_bez_filename_format)},                         \
      {PT_FIELD_restaurant_not_found_text, sizeof(restaurant_not_found_text)},                                         \
      {PT_FIELD_savegame_filename_format, sizeof(((PTNamedMemory *)0)->saved_savegame_filename_format)},               \
      {PT_FIELD_savegame_load_filename_format, sizeof(((PTNamedMemory *)0)->saved_savegame_load_filename_format)},     \
      {PT_FIELD_staff_disaster_theft_text, sizeof(staff_disaster_theft_text)},                                         \
      {PT_FIELD_staff_disaster_car_format, sizeof(staff_disaster_car_format)},                                         \
      {PT_FIELD_staff_disaster_raise_factor, sizeof(staff_disaster_raise_factor)},                                     \
      {PT_FIELD_staff_ui_years_format, sizeof(((PTNamedMemory *)0)->saved_staff_ui_years_format)},                     \
      {PT_FIELD_staff_ui_payment, sizeof(staff_ui_payment)},                                                           \
      {PT_FIELD_staff_ui_pizzas_day, sizeof(staff_ui_pizzas_day)},                                                     \
      {PT_FIELD_staff_ui_cooking, sizeof(staff_ui_cooking)}, {PT_FIELD_staff_ui_serving, sizeof(staff_ui_serving)},    \
      {PT_FIELD_staff_ui_management, sizeof(staff_ui_management)},                                                     \
      {PT_FIELD_staff_ui_experience, sizeof(staff_ui_experience)},                                                     \
      {PT_FIELD_staff_ui_intelligence, sizeof(staff_ui_intelligence)},                                                 \
      {PT_FIELD_staff_ui_trustworthiness, sizeof(staff_ui_trustworthiness)},                                           \
      {PT_FIELD_staff_ui_health, sizeof(staff_ui_health)},                                                             \
      {PT_FIELD_staff_ui_motivation, sizeof(staff_ui_motivation)},                                                     \
      {PT_FIELD_staff_application_title, sizeof(staff_application_title)},                                             \
      {PT_FIELD_staff_course_title, sizeof(staff_course_title)},                                                       \
      {PT_FIELD_staff_course_name_title, sizeof(staff_course_name_title)},                                             \
      {PT_FIELD_staff_course_scores_title, sizeof(staff_course_scores_title)},                                         \
      {PT_FIELD_staff_course_costs_title, sizeof(staff_course_costs_title)},                                           \
      {PT_FIELD_staff_course_days_title, sizeof(staff_course_days_title)},                                             \
      {PT_FIELD_staff_course_confirm_prompt, sizeof(staff_course_confirm_prompt)},                                     \
      {PT_FIELD_staff_ui_no_manager, sizeof(staff_ui_no_manager)},                                                     \
      {PT_FIELD_staff_ui_authority, sizeof(staff_ui_authority)},                                                       \
      {PT_FIELD_staff_ui_reactions, sizeof(staff_ui_reactions)},                                                       \
      {PT_FIELD_staff_ui_behavior_towards_staff, sizeof(staff_ui_behavior_towards_staff)},                             \
      {PT_FIELD_staff_ui_shopping_if_needed, sizeof(staff_ui_shopping_if_needed)},                                     \
      {PT_FIELD_staff_ui_change_prices, sizeof(staff_ui_change_prices)},                                               \
      {PT_FIELD_staff_ui_overdraw_account, sizeof(staff_ui_overdraw_account)},                                         \
      {PT_FIELD_staff_ui_turnover_decrease, sizeof(staff_ui_turnover_decrease)},                                       \
      {PT_FIELD_staff_ui_turnover_increase, sizeof(staff_ui_turnover_increase)},                                       \
      {PT_FIELD_staff_ui_supply_shortage, sizeof(staff_ui_supply_shortage)},                                           \
      {PT_FIELD_staff_ui_staff_shortage, sizeof(staff_ui_staff_shortage)},                                             \
      {PT_FIELD_staff_pool_full_error, sizeof(staff_pool_full_error)},                                                 \
      {PT_FIELD_staff_hire_none_error, sizeof(staff_hire_none_error)},                                                 \
      {PT_FIELD_staff_hire_full_format, sizeof(((PTNamedMemory *)0)->saved_staff_hire_full_format)},                   \
      {PT_FIELD_staff_hire_refusal_error, sizeof(staff_hire_refusal_error)},                                           \
      {PT_FIELD_staff_hire_prompt_format, sizeof(((PTNamedMemory *)0)->saved_staff_hire_prompt_format)},               \
      {PT_FIELD_staff_expenses_label, sizeof(staff_expenses_label)},                                                   \
      {PT_FIELD_staff_expenses_underpaid_factor, sizeof(staff_expenses_underpaid_factor)},                             \
      {PT_FIELD_staff_expenses_overpaid_factor, sizeof(staff_expenses_overpaid_factor)},                               \
      {PT_FIELD_staff_daily_underpaid_factor, sizeof(staff_daily_underpaid_factor)},                                   \
      {PT_FIELD_staff_daily_overpaid_factor, sizeof(staff_daily_overpaid_factor)},                                     \
      {PT_FIELD_staff_ui_name, sizeof(staff_ui_name)}, {PT_FIELD_staff_ui_days_absent, sizeof(staff_ui_days_absent)},  \
      {PT_FIELD_staff_ui_capacity, sizeof(staff_ui_capacity)}, {PT_FIELD_staff_ui_salary, sizeof(staff_ui_salary)},    \
      {PT_FIELD_staff_ui_name_format, sizeof(((PTNamedMemory *)0)->saved_staff_ui_name_format)},                       \
      {PT_FIELD_staff_ui_course, sizeof(staff_ui_course)}, {PT_FIELD_staff_ui_sick, sizeof(staff_ui_sick)},            \
      {PT_FIELD_staff_ui_pregnant, sizeof(staff_ui_pregnant)}, {PT_FIELD_staff_ui_chefs, sizeof(staff_ui_chefs)},      \
      {PT_FIELD_staff_ui_waiters, sizeof(staff_ui_waiters)}, {PT_FIELD_staff_ui_number, sizeof(staff_ui_number)},      \
      {PT_FIELD_staff_ui_total_capacity, sizeof(staff_ui_total_capacity)},                                             \
      {PT_FIELD_staff_ui_average_quality, sizeof(staff_ui_average_quality)},                                           \
      {PT_FIELD_staff_ui_time_per_pizza, sizeof(staff_ui_time_per_pizza)},                                             \
      {PT_FIELD_staff_ui_work, sizeof(staff_ui_work)}, {PT_FIELD_staff_ui_total_costs, sizeof(staff_ui_total_costs)},  \
      {PT_FIELD_staff_fire_selection_error, sizeof(staff_fire_selection_error)},                                       \
      {PT_FIELD_staff_fire_away_format, sizeof(((PTNamedMemory *)0)->saved_staff_fire_away_format)},                   \
      {PT_FIELD_staff_fire_prompt, sizeof(staff_fire_prompt)},                                                         \
      {PT_FIELD_staff_wages_selection_error, sizeof(staff_wages_selection_error)},                                     \
      {PT_FIELD_staff_wage_unit, sizeof(staff_wage_unit)}, {PT_FIELD_staff_wage_prompt, sizeof(staff_wage_prompt)},    \
      {PT_FIELD_staff_course_selection_error, sizeof(staff_course_selection_error)},                                   \
      {PT_FIELD_staff_course_already_format, sizeof(((PTNamedMemory *)0)->saved_staff_course_already_format)},         \
      {PT_FIELD_staff_course_away_format, sizeof(((PTNamedMemory *)0)->saved_staff_course_away_format)},               \
      {PT_FIELD_goal_background_filename_format, sizeof(((PTNamedMemory *)0)->saved_goal_background_filename_format)}, \
      {PT_FIELD_mission_completion_format, sizeof(mission_completion_format)},                                         \
      {PT_FIELD_character_filename_format, sizeof(((PTNamedMemory *)0)->saved_character_filename_format)},             \
      {PT_FIELD_goal_name_1000_pizzas, sizeof(goal_name_1000_pizzas)},                                                 \
      {PT_FIELD_goal_name_10000_pizzas, sizeof(goal_name_10000_pizzas)},                                               \
      {PT_FIELD_goal_name_profit_25000, sizeof(goal_name_profit_25000)},                                               \
      {PT_FIELD_goal_name_profit_50000, sizeof(goal_name_profit_50000)},                                               \
      {PT_FIELD_goal_name_world, sizeof(goal_name_world)},                                                             \
      {PT_FIELD_curriculum_date_format, sizeof(((PTNamedMemory *)0)->saved_curriculum_date_format)},                   \
      {PT_FIELD_character_initial_labels, sizeof(((PTNamedMemory *)0)->saved_character_initial_labels)},               \
      {PT_FIELD_target_curriculum_heading, sizeof(target_curriculum_heading)},                                         \
      {PT_FIELD_curriculum_mission_only, sizeof(curriculum_mission_only)},                                             \
      {PT_FIELD_curriculum_heading, sizeof(curriculum_heading)},                                                       \
      {PT_FIELD_character_full_name_format, sizeof(((PTNamedMemory *)0)->saved_character_full_name_format)},           \
      {PT_FIELD_character_value_labels, sizeof(((PTNamedMemory *)0)->saved_character_value_labels)},                   \
      {PT_FIELD_character_company_heading, sizeof(character_company_heading)},                                         \
      {PT_FIELD_character_company_owner_format, sizeof(((PTNamedMemory *)0)->saved_character_company_owner_format)},   \
      {PT_FIELD_character_company_profit, sizeof(character_company_profit)},                                           \
      {PT_FIELD_character_awards_filename, sizeof(character_awards_filename)},                                         \
      {PT_FIELD_character_awards_heading, sizeof(character_awards_heading)},                                           \
      {PT_FIELD_character_holiday_question, sizeof(character_holiday_question)},                                       \
      {PT_FIELD_character_holiday_days, sizeof(character_holiday_days)},                                               \
      {PT_FIELD_character_holiday_prompt, sizeof(character_holiday_prompt)},                                           \
      {PT_FIELD_personal_character_filename_format,                                                                    \
        sizeof(((PTNamedMemory *)0)->saved_personal_character_filename_format)},                                       \
      {PT_FIELD_personal_mission_filename_format,                                                                      \
        sizeof(((PTNamedMemory *)0)->saved_personal_mission_filename_format)},                                         \
      {PT_FIELD_depot_weapon_category_label, sizeof(depot_weapon_category_label)},                                     \
      {PT_FIELD_stock_menu_depot_title, sizeof(stock_menu_depot_title)},                                               \
      {PT_FIELD_depot_weapon_bag_label, sizeof(depot_weapon_bag_label)},                                               \
      {PT_FIELD_depot_town_title_format, sizeof(depot_town_title_format)},                                             \
      {PT_FIELD_depot_columns_label, sizeof(depot_columns_label)},                                                     \
      {PT_FIELD_depot_sum_value_label, sizeof(depot_sum_value_label)},                                                 \
      {PT_FIELD_depot_export_title_format, sizeof(depot_export_title_format)},                                         \
      {PT_FIELD_depot_sale_label, sizeof(depot_sale_label)},                                                           \
      {PT_FIELD_depot_purchase_label, sizeof(depot_purchase_label)},                                                   \
      {PT_FIELD_depot_purchase_title_format, sizeof(depot_purchase_title_format)},                                     \
      {PT_FIELD_shop_price_label, sizeof(shop_price_label)},                                                           \
      {PT_FIELD_depot_supplier_column_label, sizeof(depot_supplier_column_label)},                                     \
      {PT_FIELD_shop_amount_label, sizeof(shop_amount_label)},                                                         \
      {PT_FIELD_depot_store_column_label, sizeof(depot_store_column_label)},                                           \
      {PT_FIELD_contract_sum_label, sizeof(contract_sum_label)},                                                       \
      {PT_FIELD_depot_quality_rejection_text, sizeof(depot_quality_rejection_text)},                                   \
      {PT_FIELD_depot_trade_price_divisor, sizeof(depot_trade_price_divisor)},                                         \
      {PT_FIELD_depot_trade_rating_divisor, sizeof(depot_trade_rating_divisor)},                                       \
      {PT_FIELD_depot_priorities_title, sizeof(depot_priorities_title)},                                               \
      {PT_FIELD_contract_discount_label, sizeof(contract_discount_label)},                                             \
      {PT_FIELD_contract_charge_label, sizeof(contract_charge_label)},                                                 \
      {PT_FIELD_contract_terms_penalty_label, sizeof(contract_terms_penalty_label)},                                   \
      {PT_FIELD_contract_term_format, sizeof(((PTNamedMemory *)0)->saved_contract_term_format)},                       \
      {PT_FIELD_contract_price_divisor, sizeof(contract_price_divisor)},                                               \
      {PT_FIELD_contract_debug_portrait_prompt, sizeof(contract_debug_portrait_prompt)},                               \
      {PT_FIELD_contract_minimum_order_format, sizeof(contract_minimum_order_format)},                                 \
      {PT_FIELD_contract_purchase_divisor, sizeof(contract_purchase_divisor)},                                         \
      {PT_FIELD_contract_delivery_amounts_title, sizeof(contract_delivery_amounts_title)},                             \
      {PT_FIELD_stock_menu_kg_label, sizeof(stock_menu_kg_label)},                                                     \
      {PT_FIELD_contract_with_title, sizeof(contract_with_title)},                                                     \
      {PT_FIELD_shop_total_label, sizeof(shop_total_label)},                                                           \
      {PT_FIELD_contract_penalty_label, sizeof(contract_penalty_label)},                                               \
      {PT_FIELD_contract_amount_label, sizeof(contract_amount_label)},                                                 \
      {PT_FIELD_contract_as_required_label, sizeof(contract_as_required_label)},                                       \
      {PT_FIELD_contract_as_agreed_label, sizeof(contract_as_agreed_label)},                                           \
      {PT_FIELD_contract_quality_label, sizeof(contract_quality_label)},                                               \
      {PT_FIELD_contract_remaining_term_label, sizeof(contract_remaining_term_label)},                                 \
      {PT_FIELD_contract_empty_error, sizeof(contract_empty_error)},                                                   \
      {PT_FIELD_contract_term_prompt, sizeof(contract_term_prompt)},                                                   \
      {PT_FIELD_contract_cancel_prompt, sizeof(contract_cancel_prompt)},                                               \
      {PT_FIELD_contract_delivery_space_text, sizeof(contract_delivery_space_text)},                                   \
      {PT_FIELD_contract_cancel_format, sizeof(((PTNamedMemory *)0)->saved_contract_cancel_format)},                   \
      {PT_FIELD_shop_unavailable_price_text, sizeof(shop_unavailable_price_text)},                                     \
      {PT_FIELD_shop_quality_format, sizeof(((PTNamedMemory *)0)->saved_shop_quality_format)},                         \
      {PT_FIELD_shop_stock_label, sizeof(shop_stock_label)},                                                           \
      {PT_FIELD_stock_menu_days_label, sizeof(stock_menu_days_label)},                                                 \
      {PT_FIELD_shop_price_divisor, sizeof(shop_price_divisor)},                                                       \
      {PT_FIELD_contract_expired_format, sizeof(((PTNamedMemory *)0)->saved_contract_expired_format)},                 \
      {PT_FIELD_contract_shortage_format, sizeof(((PTNamedMemory *)0)->saved_contract_shortage_format)},               \
      {PT_FIELD_contract_delivery_format, sizeof(((PTNamedMemory *)0)->saved_contract_delivery_format)},               \
      {PT_FIELD_shop_suppliers_title_format, sizeof(((PTNamedMemory *)0)->saved_shop_suppliers_title_format)},         \
      {PT_FIELD_shop_supplier_total_format, sizeof(((PTNamedMemory *)0)->saved_shop_supplier_total_format)},           \
      {PT_FIELD_shop_page_format, sizeof(((PTNamedMemory *)0)->saved_shop_page_format)},                               \
      {PT_FIELD_stock_menu_stocks_title, sizeof(stock_menu_stocks_title)},                                             \
      {PT_FIELD_stock_menu_kilogram_label, sizeof(stock_menu_kilogram_label)},                                         \
      {PT_FIELD_stock_menu_value_label, sizeof(stock_menu_value_label)},                                               \
      {PT_FIELD_stock_menu_supplies_format, sizeof(((PTNamedMemory *)0)->saved_stock_menu_supplies_format)},           \
      {PT_FIELD_stock_menu_quality_label, sizeof(stock_menu_quality_label)},                                           \
      {PT_FIELD_stock_menu_supply_label, sizeof(stock_menu_supply_label)},                                             \
      {PT_FIELD_stock_menu_need_label, sizeof(stock_menu_need_label)},                                                 \
      {PT_FIELD_stock_menu_use_label, sizeof(stock_menu_use_label)},                                                   \
      {PT_FIELD_stock_menu_demand_divisor, sizeof(stock_menu_demand_divisor)},                                         \
      {PT_FIELD_supplier_price_addon_divisor, sizeof(supplier_price_addon_divisor)},                                   \
      {PT_FIELD_supplier_price_level5_factor, sizeof(supplier_price_level5_factor)},                                   \
      {PT_FIELD_supplier_price_level4_factor, sizeof(supplier_price_level4_factor)},                                   \
      {PT_FIELD_supplier_price_level2_factor, sizeof(supplier_price_level2_factor)},                                   \
      {PT_FIELD_supplier_price_level1_factor, sizeof(supplier_price_level1_factor)},                                   \
      {PT_FIELD_depot_central_label, sizeof(depot_central_label)},                                                     \
      {PT_FIELD_depot_capacity_label, sizeof(depot_capacity_label)},                                                   \
      {PT_FIELD_depot_in_use_label, sizeof(depot_in_use_label)},                                                       \
      {PT_FIELD_depot_stolen_format, sizeof(depot_stolen_format)},                                                     \
      {PT_FIELD_contract_penalty_new_factor, sizeof(contract_penalty_new_factor)},                                     \
      {PT_FIELD_contract_discount_factor, sizeof(contract_discount_factor)},                                           \
      {PT_FIELD_contract_discount_divisor, sizeof(contract_discount_divisor)},                                         \
      {PT_FIELD_contract_charge_divisor, sizeof(contract_charge_divisor)},                                             \
      {PT_FIELD_contract_fake_sum_factor, sizeof(contract_fake_sum_factor)},                                           \
      {PT_FIELD_contract_penalty_reduction_factor, sizeof(contract_penalty_reduction_factor)},                         \
      {PT_FIELD_language_filename_format, sizeof(((PTNamedMemory *)0)->saved_language_filename_format)},               \
      {PT_FIELD_file_write_mode, sizeof(file_write_mode)},                                                             \
      {PT_FIELD_file_readwrite_mode, sizeof(file_readwrite_mode)}, {PT_FIELD_double_720, sizeof(double_720)},          \
      {PT_FIELD_customer_other_tile_factor, sizeof(customer_other_tile_factor)},                                       \
      {PT_FIELD_customer_residential_tile_factor, sizeof(customer_residential_tile_factor)},                           \
      {PT_FIELD_float_720, sizeof(float_720)},                                                                         \
      {PT_FIELD_pizza_income_price_divisor, sizeof(pizza_income_price_divisor)},                                       \
      {PT_FIELD_pizza_income_price_offset, sizeof(pizza_income_price_offset)},                                         \
      {PT_FIELD_pizza_income_base_divisor, sizeof(pizza_income_base_divisor)},                                         \
      {PT_FIELD_pizza_income_base_offset, sizeof(pizza_income_base_offset)},                                           \
      {PT_FIELD_pizza_price_offset, sizeof(pizza_price_offset)},                                                       \
      {PT_FIELD_pizza_price_divisor, sizeof(pizza_price_divisor)},                                                     \
      {PT_FIELD_pizza_price_bonus_scale, sizeof(pizza_price_bonus_scale)},                                             \
      {PT_FIELD_customer_distribution_factor, sizeof(customer_distribution_factor)},                                   \
      {PT_FIELD_pizza_order_random_threshold, sizeof(pizza_order_random_threshold)},                                   \
      {PT_FIELD_restaurant_supplies_shortage_text, sizeof(restaurant_supplies_shortage_text)},                         \
      {PT_FIELD_restaurant_ovens_shortage_text, sizeof(restaurant_ovens_shortage_text)},                               \
      {PT_FIELD_restaurant_cooks_shortage_text, sizeof(restaurant_cooks_shortage_text)},                               \
      {PT_FIELD_restaurant_waiters_shortage_text, sizeof(restaurant_waiters_shortage_text)},                           \
      {PT_FIELD_restaurant_supply_percentage_scale, sizeof(restaurant_supply_percentage_scale)},                       \
      {PT_FIELD_restaurant_turnover_goal_threshold, sizeof(restaurant_turnover_goal_threshold)},                       \
      {PT_FIELD_restaurant_sales_currency_divisor, sizeof(restaurant_sales_currency_divisor)},                         \
      {PT_FIELD_restaurant_electricity_label, sizeof(restaurant_electricity_label)},                                   \
      {PT_FIELD_restaurant_slot_income_label, sizeof(restaurant_slot_income_label)},                                   \
      {PT_FIELD_restaurant_invoice_total_label, sizeof(restaurant_invoice_total_label)},                               \
      {PT_FIELD_country_market_leader_label, sizeof(country_market_leader_label)},                                     \
      {PT_FIELD_country_no_market_leader_label, sizeof(country_no_market_leader_label)},                               \
      {PT_FIELD_country_map_filename, sizeof(country_map_filename)},                                                   \
      {PT_FIELD_country_no_city_message, sizeof(country_no_city_message)},                                             \
      {PT_FIELD_country_insufficient_money_message, sizeof(country_insufficient_money_message)},                       \
      {PT_FIELD_country_flight_cost_format, sizeof(((PTNamedMemory *)0)->saved_country_flight_cost_format)},           \
      {PT_FIELD_country_transport_cost_format, sizeof(((PTNamedMemory *)0)->saved_country_transport_cost_format)},     \
      {PT_FIELD_country_missing_depot_format, sizeof(((PTNamedMemory *)0)->saved_country_missing_depot_format)},       \
      {PT_FIELD_town_initializing_caption, sizeof(town_initializing_caption)},                                         \
      {PT_FIELD_town_initializing_ellipsis, sizeof(town_initializing_ellipsis)},                                       \
      {PT_FIELD_staff_market_filename, sizeof(staff_market_filename)},                                                 \
      {PT_FIELD_attack_police_message, sizeof(attack_police_message)},                                                 \
      {PT_FIELD_attack_police_argument, sizeof(attack_police_argument)},                                               \
      {PT_FIELD_guest_walk_bob_format, sizeof(((PTNamedMemory *)0)->saved_guest_walk_bob_format)},                     \
      {PT_FIELD_guest_type_error_text, sizeof(guest_type_error_text)},                                                 \
      {PT_FIELD_attack_weapon_bob_format, sizeof(((PTNamedMemory *)0)->saved_attack_weapon_bob_format)},               \
      {PT_FIELD_attack_damage_format, sizeof(((PTNamedMemory *)0)->saved_attack_damage_format)},                       \
      {PT_FIELD_attack_contamination_format, sizeof(((PTNamedMemory *)0)->saved_attack_contamination_format)},         \
      {PT_FIELD_guest_portrait_filename_format, sizeof(((PTNamedMemory *)0)->saved_guest_portrait_filename_format)},   \
      {PT_FIELD_guest_eating_comment_format, sizeof(((PTNamedMemory *)0)->saved_guest_eating_comment_format)},         \
      {PT_FIELD_ai_restaurant_upgrade_money_factor, sizeof(ai_restaurant_upgrade_money_factor)},                       \
      {PT_FIELD_ai_low_rating_money_threshold, sizeof(ai_low_rating_money_threshold)},                                 \
      {PT_FIELD_ai_low_default_rating_money_threshold, sizeof(ai_low_default_rating_money_threshold)},                 \
      {PT_FIELD_competition_pizza_title, sizeof(competition_pizza_title)},                                             \
      {PT_FIELD_competition_title, sizeof(competition_title)},                                                         \
      {PT_FIELD_competition_challenge_format, sizeof(competition_challenge_format)},                                   \
      {PT_FIELD_competition_ai_challenge_text, sizeof(competition_ai_challenge_text)},                                 \
      {PT_FIELD_competition_dollars_unit, sizeof(competition_dollars_unit)},                                           \
      {PT_FIELD_competition_bet_prompt, sizeof(competition_bet_prompt)},                                               \
      {PT_FIELD_competition_decline_text, sizeof(competition_decline_text)},                                           \
      {PT_FIELD_competition_invitation_format, sizeof(competition_invitation_format)},                                 \
      {PT_FIELD_competition_stake_prompt, sizeof(competition_stake_prompt)},                                           \
      {PT_FIELD_competition_no_unit, sizeof(competition_no_unit)},                                                     \
      {PT_FIELD_competition_speed_prompt, sizeof(competition_speed_prompt)},                                           \
      {PT_FIELD_competition_jury_caption, sizeof(competition_jury_caption)},                                           \
      {PT_FIELD_competition_time_format, sizeof(competition_time_format)},                                             \
      {PT_FIELD_competition_draw_caption, sizeof(competition_draw_caption)},                                           \
      {PT_FIELD_competition_win_caption, sizeof(competition_win_caption)},                                             \
      {PT_FIELD_competition_minimum_money, sizeof(competition_minimum_money)},                                         \
      {PT_FIELD_competition_maximum_bet, sizeof(competition_maximum_bet)},                                             \
      {PT_FIELD_competition_ai_money_fraction, sizeof(competition_ai_money_fraction)},                                 \
      {PT_FIELD_recipe_new_name, sizeof(recipe_new_name)},                                                             \
      {PT_FIELD_recipe_number_format, sizeof(recipe_number_format)},                                                   \
      {PT_FIELD_recipe_calories_format, sizeof(recipe_calories_format)},                                               \
      {PT_FIELD_recipe_fat_format, sizeof(recipe_fat_format)},                                                         \
      {PT_FIELD_recipe_sweet_format, sizeof(recipe_sweet_format)},                                                     \
      {PT_FIELD_recipe_salty_format, sizeof(recipe_salty_format)},                                                     \
      {PT_FIELD_recipe_hot_format, sizeof(recipe_hot_format)},                                                         \
      {PT_FIELD_recipe_appearance_format, sizeof(recipe_appearance_format)},                                           \
      {PT_FIELD_recipe_refinement_format, sizeof(recipe_refinement_format)},                                           \
      {PT_FIELD_recipe_trend_format, sizeof(recipe_trend_format)},                                                     \
      {PT_FIELD_recipe_imitation_format, sizeof(recipe_imitation_format)},                                             \
      {PT_FIELD_recipe_normality_format, sizeof(recipe_normality_format)},                                             \
      {PT_FIELD_recipe_jury_filename, sizeof(recipe_jury_filename)},                                                   \
      {PT_FIELD_recipe_jury_compare_text, sizeof(recipe_jury_compare_text)},                                           \
      {PT_FIELD_recipe_jury_evaluate_text, sizeof(recipe_jury_evaluate_text)},                                         \
      {PT_FIELD_recipe_jury_competition_text, sizeof(recipe_jury_competition_text)},                                   \
      {PT_FIELD_recipe_jury_total_text, sizeof(recipe_jury_total_text)},                                               \
      {PT_FIELD_recipe_jury_percent_format, sizeof(((PTNamedMemory *)0)->saved_recipe_jury_percent_format)},           \
      {PT_FIELD_recipe_jury_book_hint_format, sizeof(((PTNamedMemory *)0)->saved_recipe_jury_book_hint_format)},       \
      {PT_FIELD_recipe_view_info_text, sizeof(recipe_view_info_text)},                                                 \
      {PT_FIELD_recipe_view_recipe_text, sizeof(recipe_view_recipe_text)},                                             \
      {PT_FIELD_recipe_view_here_text, sizeof(recipe_view_here_text)},                                                 \
      {PT_FIELD_recipe_view_all_text, sizeof(recipe_view_all_text)},                                                   \
      {PT_FIELD_recipe_view_costs_text, sizeof(recipe_view_costs_text)},                                               \
      {PT_FIELD_recipe_view_price_text, sizeof(recipe_view_price_text)},                                               \
      {PT_FIELD_recipe_view_sales_text, sizeof(recipe_view_sales_text)},                                               \
      {PT_FIELD_recipe_view_profit_text, sizeof(recipe_view_profit_text)},                                             \
      {PT_FIELD_recipe_view_popularity_text, sizeof(recipe_view_popularity_text)},                                     \
      {PT_FIELD_recipe_view_gram_text, sizeof(recipe_view_gram_text)},                                                 \
      {PT_FIELD_recipe_view_total_weight_text, sizeof(recipe_view_total_weight_text)},                                 \
      {PT_FIELD_recipe_view_ingredient_count_format,                                                                   \
        sizeof(((PTNamedMemory *)0)->saved_recipe_view_ingredient_count_format)},                                      \
      {PT_FIELD_pizza_competition_title, sizeof(pizza_competition_title)},                                             \
      {PT_FIELD_pizza_competition_separator, sizeof(pizza_competition_separator)},                                     \
      {PT_FIELD_pizza_competition_original_label, sizeof(pizza_competition_original_label)},                           \
      {PT_FIELD_pizza_competition_level_label, sizeof(pizza_competition_level_label)},                                 \
      {PT_FIELD_pizza_competition_ingredient_label, sizeof(pizza_competition_ingredient_label)},                       \
      {PT_FIELD_pizza_competition_ready_text, sizeof(pizza_competition_ready_text)},                                   \
      {PT_FIELD_ingredient_grammes_format, sizeof(ingredient_grammes_format)},                                         \
      {PT_FIELD_ingredient_calories_format, sizeof(ingredient_calories_format)},                                       \
      {PT_FIELD_ingredient_intensity_format, sizeof(ingredient_intensity_format)},                                     \
      {PT_FIELD_ingredient_fat_format, sizeof(ingredient_fat_format)},                                                 \
      {PT_FIELD_ingredient_sweet_format, sizeof(ingredient_sweet_format)},                                             \
      {PT_FIELD_ingredient_salty_format, sizeof(ingredient_salty_format)},                                             \
      {PT_FIELD_ingredient_hot_format, sizeof(ingredient_hot_format)},                                                 \
      {PT_FIELD_ingredient_trend_format, sizeof(ingredient_trend_format)},                                             \
      {PT_FIELD_ingredient_trend_name_format, sizeof(ingredient_trend_name_format)},                                   \
      {PT_FIELD_ingredients_full_text, sizeof(ingredients_full_text)},                                                 \
      {PT_FIELD_recipe_editor_greeting_format, sizeof(((PTNamedMemory *)0)->saved_recipe_editor_greeting_format)},     \
      {PT_FIELD_recipe_name_prompt, sizeof(recipe_name_prompt)},                                                       \
      {PT_FIELD_recipe_cost_divisor, sizeof(recipe_cost_divisor)},                                                     \
      {PT_FIELD_disk_menu_caption, sizeof(disk_menu_caption)}, {PT_FIELD_disk_quit_prompt, sizeof(disk_quit_prompt)},  \
      {PT_FIELD_character_selection_level_caption, sizeof(character_selection_level_caption)},                         \
      {PT_FIELD_highscores_filename, sizeof(highscores_filename)},                                                     \
      {PT_FIELD_options_on_label, sizeof(options_on_label)}, {PT_FIELD_options_off_label, sizeof(options_off_label)},  \
      {PT_FIELD_options_prefs_filename, sizeof(options_prefs_filename)},                                               \
      {PT_FIELD_character_selection_money_limit, sizeof(character_selection_money_limit)},                             \
      {PT_FIELD_mission_date_format, sizeof(((PTNamedMemory *)0)->saved_mission_date_format)},                         \
      {PT_FIELD_mission_selection_caption, sizeof(mission_selection_caption)},                                         \
      {PT_FIELD_mission_filename_format, sizeof(((PTNamedMemory *)0)->saved_mission_filename_format)},                 \
      {PT_FIELD_main_menu_background_filename, sizeof(main_menu_background_filename)},                                 \
      {PT_FIELD_game_screen_background_filename, sizeof(game_screen_background_filename)},                             \
      {PT_FIELD_selector_save_label, sizeof(selector_save_label)},                                                     \
      {PT_FIELD_selector_load_label, sizeof(selector_load_label)},                                                     \
      {PT_FIELD_selector_title_format, sizeof(((PTNamedMemory *)0)->saved_selector_title_format)},                     \
      {PT_FIELD_selector_game_directory, sizeof(selector_game_directory)},                                             \
      {PT_FIELD_selector_no_games, sizeof(selector_no_games)},                                                         \
      {PT_FIELD_selector_no_characters, sizeof(selector_no_characters)},                                               \
      {PT_FIELD_selector_no_missions, sizeof(selector_no_missions)},                                                   \
      {PT_FIELD_selector_new_name, sizeof(selector_new_name)},                                                         \
      {PT_FIELD_selector_filename_prefix, sizeof(selector_filename_prefix)},                                           \
      {PT_FIELD_selector_error_name, sizeof(selector_error_name)},                                                     \
      {PT_FIELD_selector_overwrite_prompt, sizeof(selector_overwrite_prompt)},                                         \
      {PT_FIELD_player_stats_money_label, sizeof(player_stats_money_label)},                                           \
      {PT_FIELD_player_stats_age_label, sizeof(player_stats_age_label)},                                               \
      {PT_FIELD_player_stats_popularity_label, sizeof(player_stats_popularity_label)},                                 \
      {PT_FIELD_player_stats_health_label, sizeof(player_stats_health_label)},                                         \
      {PT_FIELD_player_stats_energy_label, sizeof(player_stats_energy_label)},                                         \
      {PT_FIELD_player_stats_intelligence_label, sizeof(player_stats_intelligence_label)},                             \
      {PT_FIELD_player_stats_charisma_label, sizeof(player_stats_charisma_label)},                                     \
      {PT_FIELD_player_stats_dexterity_label, sizeof(player_stats_dexterity_label)},                                   \
      {PT_FIELD_player_stats_personality_label, sizeof(player_stats_personality_label)},                               \
      {PT_FIELD_player_stats_cooking_label, sizeof(player_stats_cooking_label)},                                       \
      {PT_FIELD_player_stats_courage_label, sizeof(player_stats_courage_label)},                                       \
      {PT_FIELD_attack_action_caption, sizeof(attack_action_caption)},                                                 \
      {PT_FIELD_attack_weapon_caption, sizeof(attack_weapon_caption)},                                                 \
      {PT_FIELD_mission_invalid_place, sizeof(mission_invalid_place)},                                                 \
      {PT_FIELD_poi_info_formats, sizeof(poi_info_formats)}, {PT_FIELD_poi_realtor_text, sizeof(poi_realtor_text)},    \
      {PT_FIELD_closed_restaurant_text, sizeof(closed_restaurant_text)},                                               \
      {PT_FIELD_creating_map_text, sizeof(creating_map_text)},                                                         \
      {PT_FIELD_maestro_background_filename, sizeof(maestro_background_filename)},                                     \
      {PT_FIELD_game_over_retry_prompt, sizeof(game_over_retry_prompt)},                                               \
      {PT_FIELD_high_score_award_text, sizeof(high_score_award_text)},                                                 \
      {PT_FIELD_high_score_name_format, sizeof(high_score_name_format)},                                               \
      {PT_FIELD_high_score_name_prompt, sizeof(high_score_name_prompt)},                                               \
      {PT_FIELD_personal_record_directory, sizeof(personal_record_directory)},                                         \
      {PT_FIELD_personal_record_fallback, sizeof(personal_record_fallback)},                                           \
      {PT_FIELD_personal_record_save_prompt, sizeof(personal_record_save_prompt)},                                     \
      {PT_FIELD_game_over_low_score_text, sizeof(game_over_low_score_text)},                                           \
      {PT_FIELD_ending_background_filename, sizeof(ending_background_filename)},                                       \
      {PT_FIELD_animated_quality_caption, sizeof(animated_quality_caption)},                                           \
      {PT_FIELD_animated_reputation_caption, sizeof(animated_reputation_caption)},                                     \
      {PT_FIELD_animated_no_text, sizeof(animated_no_text)},                                                           \
      {PT_FIELD_animated_orders_text, sizeof(animated_orders_text)},                                                   \
      {PT_FIELD_animated_output_caption, sizeof(animated_output_caption)},                                             \
      {PT_FIELD_animated_day_minutes, sizeof(animated_day_minutes)},                                                   \
      {PT_FIELD_animated_percent_scale, sizeof(animated_percent_scale)},                                               \
      {PT_FIELD_animated_demand_text, sizeof(animated_demand_text)},                                                   \
      {PT_FIELD_animated_capacity_text, sizeof(animated_capacity_text)},                                               \
      {PT_FIELD_animated_popularity_caption, sizeof(animated_popularity_caption)},                                     \
      {PT_FIELD_animated_customers_caption, sizeof(animated_customers_caption)},                                       \
      {PT_FIELD_animated_takings_text, sizeof(animated_takings_text)},                                                 \
      {PT_FIELD_animated_expenses_text, sizeof(animated_expenses_text)},                                               \
      {PT_FIELD_realtor_sell_factor, sizeof(realtor_sell_factor)},                                                     \
      {PT_FIELD_realtor_maximum_factor, sizeof(realtor_maximum_factor)},                                               \
      {PT_FIELD_realtor_discount_factor, sizeof(realtor_discount_factor)},                                             \
      {PT_FIELD_realtor_increase_factor, sizeof(realtor_increase_factor)},                                             \
      {PT_FIELD_bank_transfer_amount_text, sizeof(bank_transfer_amount_text)},                                         \
      {PT_FIELD_bank_transfer_account_text, sizeof(bank_transfer_account_text)},                                       \
      {PT_FIELD_bank_loan_cash_limit, sizeof(bank_loan_cash_limit)},                                                   \
      {PT_FIELD_bank_investment_minimum, sizeof(bank_investment_minimum)},                                             \
      {PT_FIELD_bank_investment_cash_limit, sizeof(bank_investment_cash_limit)},                                       \
      {PT_FIELD_bank_investment_fraction, sizeof(bank_investment_fraction)},                                           \
      {PT_FIELD_townhall_pair_format, sizeof(((PTNamedMemory *)0)->saved_townhall_pair_format)},                       \
      {PT_FIELD_townhall_restaurant_name_format, sizeof(((PTNamedMemory *)0)->saved_townhall_restaurant_name_format)}, \
      {PT_FIELD_townhall_annual_pizza_announcement, sizeof(townhall_annual_pizza_announcement)},                       \
      {PT_FIELD_townhall_annual_recipe_name_format, sizeof(townhall_annual_recipe_name_format)},                       \
      {PT_FIELD_townhall_character_name_format, sizeof(((PTNamedMemory *)0)->saved_townhall_character_name_format)},   \
      {PT_FIELD_townhall_name_format, sizeof(((PTNamedMemory *)0)->saved_townhall_name_format)},                       \
      {PT_FIELD_townhall_congratulations_format, sizeof(((PTNamedMemory *)0)->saved_townhall_congratulations_format)}, \
      {PT_FIELD_townhall_competition_account_format,                                                                   \
        sizeof(((PTNamedMemory *)0)->saved_townhall_competition_account_format)},                                      \
      {PT_FIELD_townhall_mayor_date_format, sizeof(((PTNamedMemory *)0)->saved_townhall_mayor_date_format)},           \
      {PT_FIELD_townhall_mayor_account_format, sizeof(((PTNamedMemory *)0)->saved_townhall_mayor_account_format)},     \
      {PT_FIELD_terminal_new_number_prompt, sizeof(terminal_new_number_prompt)},                                       \
      {PT_FIELD_terminal_no_entries, sizeof(terminal_no_entries)}, {PT_FIELD_terminal_title, sizeof(terminal_title)},  \
      {PT_FIELD_terminal_loading_message, sizeof(terminal_loading_message)},                                           \
      {PT_FIELD_empty_string, sizeof(empty_string)},                                                                   \
      {PT_FIELD_mission_completion_phone_text, sizeof(mission_completion_phone_text)},                                 \
      {PT_FIELD_mission_date_phrase_format, sizeof(((PTNamedMemory *)0)->saved_mission_date_phrase_format)},           \
      {PT_FIELD_mission_place_format, sizeof(((PTNamedMemory *)0)->saved_mission_place_format)},                       \
      {PT_FIELD_mission_pickup_format, sizeof(((PTNamedMemory *)0)->saved_mission_pickup_format)},                     \
      {PT_FIELD_mission_contamination_format, sizeof(((PTNamedMemory *)0)->saved_mission_contamination_format)},       \
      {PT_FIELD_mission_damage_format, sizeof(((PTNamedMemory *)0)->saved_mission_damage_format)},                     \
      {PT_FIELD_mission_laundering_format, sizeof(((PTNamedMemory *)0)->saved_mission_laundering_format)},             \
      {PT_FIELD_mission_delivery_format, sizeof(((PTNamedMemory *)0)->saved_mission_delivery_format)},                 \
      {PT_FIELD_mission_deal_format, sizeof(((PTNamedMemory *)0)->saved_mission_deal_format)},                         \
      {PT_FIELD_mafia_failed_business_text, sizeof(mafia_failed_business_text)},                                       \
      {PT_FIELD_mafia_protection_format, sizeof(((PTNamedMemory *)0)->saved_mafia_protection_format)},                 \
      {PT_FIELD_mission_reward_format, sizeof(((PTNamedMemory *)0)->saved_mission_reward_format)},                     \
      {PT_FIELD_mission_accept_format, sizeof(((PTNamedMemory *)0)->saved_mission_accept_format)},                     \
      {PT_FIELD_mafia_existing_loan_format, sizeof(((PTNamedMemory *)0)->saved_mafia_existing_loan_format)},           \
      {PT_FIELD_mafia_loan_offer_format, sizeof(((PTNamedMemory *)0)->saved_mafia_loan_offer_format)},                 \
      {PT_FIELD_mafia_tip_town_format, sizeof(((PTNamedMemory *)0)->saved_mafia_tip_town_format)},                     \
      {PT_FIELD_mafia_weapon_tip_format, sizeof(((PTNamedMemory *)0)->saved_mafia_weapon_tip_format)},                 \
      {PT_FIELD_mafia_dealer_tip_format, sizeof(((PTNamedMemory *)0)->saved_mafia_dealer_tip_format)},                 \
      {PT_FIELD_mafia_tip_price_format, sizeof(((PTNamedMemory *)0)->saved_mafia_tip_price_format)},                   \
      {PT_FIELD_mafia_cost_cash_scale, sizeof(mafia_cost_cash_scale)},                                                 \
      {PT_FIELD_mafia_cost_scale_limit, sizeof(mafia_cost_scale_limit)},                                               \
      {PT_FIELD_invoice_title, sizeof(invoice_title)},                                                                 \
      {PT_FIELD_invoice_name_format, sizeof(((PTNamedMemory *)0)->saved_invoice_name_format)},                         \
      {PT_FIELD_background_filename_format, sizeof(((PTNamedMemory *)0)->saved_background_filename_format)},           \
      {PT_FIELD_price_decimal_format, sizeof(((PTNamedMemory *)0)->saved_price_decimal_format)},                       \
      {PT_FIELD_price_display_divisor, sizeof(price_display_divisor)},                                                 \
      {PT_FIELD_list_percent_text, sizeof(list_percent_text)},                                                         \
      {PT_FIELD_dollar_alignment_text, sizeof(dollar_alignment_text)},                                                 \
      {PT_FIELD_dollar_text_format, sizeof(((PTNamedMemory *)0)->saved_dollar_text_format)},                           \
      {PT_FIELD_integer_text_format, sizeof(((PTNamedMemory *)0)->saved_integer_text_format)},                         \
      {PT_FIELD_advert_dollar, sizeof(advert_dollar)},                                                                 \
      {PT_FIELD_info_box_formats, sizeof(((PTNamedMemory *)0)->saved_info_box_formats)},                               \
      {PT_FIELD_restaurant_name_format, sizeof(restaurant_name_format)},                                               \
      {PT_FIELD_money_display_format, sizeof(((PTNamedMemory *)0)->saved_money_display_format)},                       \
      {PT_FIELD_ok_text, sizeof(ok_text)}, {PT_FIELD_yes_text, sizeof(yes_text)}, {PT_FIELD_no_text, sizeof(no_text)}, \
      {PT_FIELD_staff_disaster_question_format, sizeof(staff_disaster_question_format)},                               \
      {PT_FIELD_number_query_format, sizeof(((PTNamedMemory *)0)->saved_number_query_format)},                         \
      {PT_FIELD_market_low, sizeof(market_low)}, {PT_FIELD_market_very_little, sizeof(market_very_little)},            \
      {PT_FIELD_market_little, sizeof(market_little)}, {PT_FIELD_market_average, sizeof(market_average)},              \
      {PT_FIELD_market_above_average, sizeof(market_above_average)}, {PT_FIELD_market_high, sizeof(market_high)},      \
      {PT_FIELD_market_very_high, sizeof(market_very_high)},                                                           \
      {PT_FIELD_market_extremely_high, sizeof(market_extremely_high)},                                                 \
      {PT_FIELD_market_out_of_range, sizeof(market_out_of_range)},                                                     \
      {PT_FIELD_decimal_scale_base, sizeof(decimal_scale_base)},                                                       \
      {PT_FIELD_market_percent_scale, sizeof(market_percent_scale)},                                                   \
      {PT_FIELD_market_premises_label, sizeof(market_premises_label)},                                                 \
      {PT_FIELD_market_rent_label, sizeof(market_rent_label)},                                                         \
      {PT_FIELD_market_per_square_metre, sizeof(market_per_square_metre)},                                             \
      {PT_FIELD_market_price_label, sizeof(market_price_label)},                                                       \
      {PT_FIELD_market_tendency_label, sizeof(market_tendency_label)},                                                 \
      {PT_FIELD_market_falling_label, sizeof(market_falling_label)},                                                   \
      {PT_FIELD_market_rising_label, sizeof(market_rising_label)},                                                     \
      {PT_FIELD_market_average_size_label, sizeof(market_average_size_label)},                                         \
      {PT_FIELD_market_square_metres, sizeof(market_square_metres)},                                                   \
      {PT_FIELD_market_rooms_label, sizeof(market_rooms_label)},                                                       \
      {PT_FIELD_market_to_rent_label, sizeof(market_to_rent_label)},                                                   \
      {PT_FIELD_market_for_sale_label, sizeof(market_for_sale_label)},                                                 \
      {PT_FIELD_market_not_available_label, sizeof(market_not_available_label)},                                       \
      {PT_FIELD_market_taste_title, sizeof(market_taste_title)},                                                       \
      {PT_FIELD_market_rank_dash, sizeof(market_rank_dash)},                                                           \
      {PT_FIELD_market_chain_title, sizeof(market_chain_title)},                                                       \
      {PT_FIELD_market_share_format, sizeof(((PTNamedMemory *)0)->saved_market_share_format)},                         \
      {PT_FIELD_market_turnover_format, sizeof(((PTNamedMemory *)0)->saved_market_turnover_format)},                   \
      {PT_FIELD_floor_zero_filename, sizeof(floor_zero_filename)},                                                     \
      {PT_FIELD_floor_filename_format, sizeof(((PTNamedMemory *)0)->saved_floor_filename_format)},                     \
      {PT_FIELD_furniture_inventory_num_text, sizeof(furniture_inventory_num_text)},                                   \
      {PT_FIELD_furniture_inventory_name_text, sizeof(furniture_inventory_name_text)},                                 \
      {PT_FIELD_furniture_inventory_format, sizeof(((PTNamedMemory *)0)->saved_furniture_inventory_format)},           \
      {PT_FIELD_furniture_equipment_title, sizeof(furniture_equipment_title)},                                         \
      {PT_FIELD_furniture_in_use_text, sizeof(furniture_in_use_text)},                                                 \
      {PT_FIELD_furniture_value_text, sizeof(furniture_value_text)},                                                   \
      {PT_FIELD_furniture_quality_text, sizeof(furniture_quality_text)},                                               \
      {PT_FIELD_poi_capacity_text, sizeof(poi_capacity_text)},                                                         \
      {PT_FIELD_furniture_seats_text, sizeof(furniture_seats_text)},                                                   \
      {PT_FIELD_furniture_per_day_text, sizeof(furniture_per_day_text)},                                               \
      {PT_FIELD_furniture_kitchenware_title, sizeof(furniture_kitchenware_title)},                                     \
      {PT_FIELD_furniture_ovens_text, sizeof(furniture_ovens_text)},                                                   \
      {PT_FIELD_furniture_bake_time_text, sizeof(furniture_bake_time_text)},                                           \
      {PT_FIELD_furniture_minutes_text, sizeof(furniture_minutes_text)},                                               \
      {PT_FIELD_furniture_baking_capacity_text, sizeof(furniture_baking_capacity_text)},                               \
      {PT_FIELD_furniture_number_text, sizeof(furniture_number_text)},                                                 \
      {PT_FIELD_furniture_floor_title, sizeof(furniture_floor_title)},                                                 \
      {PT_FIELD_furniture_tile_count_format, sizeof(((PTNamedMemory *)0)->saved_furniture_tile_count_format)},         \
      {PT_FIELD_furniture_area_format, sizeof(((PTNamedMemory *)0)->saved_furniture_area_format)},                     \
      {PT_FIELD_furniture_extras_title, sizeof(furniture_extras_title)},                                               \
      {PT_FIELD_furniture_floor_patterns_filename, sizeof(furniture_floor_patterns_filename)},                         \
      {PT_FIELD_furniture_floor_price_format, sizeof(((PTNamedMemory *)0)->saved_furniture_floor_price_format)},       \
      {PT_FIELD_furniture_floor_total_format, sizeof(((PTNamedMemory *)0)->saved_furniture_floor_total_format)},       \
      {PT_FIELD_furniture_supplier_page_format, sizeof(((PTNamedMemory *)0)->saved_furniture_supplier_page_format)},   \
      {PT_FIELD_furniture_price_text, sizeof(furniture_price_text)},                                                   \
      {PT_FIELD_furniture_capacity_format, sizeof(furniture_capacity_format)},                                         \
      {PT_FIELD_furniture_baking_time_format, sizeof(furniture_baking_time_format)},                                   \
      {PT_FIELD_furniture_quality_format, sizeof(furniture_quality_format)},                                           \
      {PT_FIELD_furniture_selection_limit_text, sizeof(furniture_selection_limit_text)},                               \
      {PT_FIELD_furniture_invoice_caption, sizeof(furniture_invoice_caption)},                                         \
      {PT_FIELD_furniture_purchase_thanks, sizeof(furniture_purchase_thanks)},                                         \
      {PT_FIELD_furniture_no_selection_text, sizeof(furniture_no_selection_text)},                                     \
      {PT_FIELD_placement_maximum_ovens_text, sizeof(placement_maximum_ovens_text)},                                   \
      {PT_FIELD_placement_maximum_items_text, sizeof(placement_maximum_items_text)},                                   \
      {PT_FIELD_placement_closed_text, sizeof(placement_closed_text)},                                                 \
      {PT_FIELD_placement_sell_format, sizeof(placement_sell_format)},                                                 \
      {PT_FIELD_placement_invalid_text, sizeof(placement_invalid_text)},                                               \
      {PT_FIELD_furniture_no_supplier_text, sizeof(furniture_no_supplier_text)},                                       \
      {PT_FIELD_furniture_suppliers_caption, sizeof(furniture_suppliers_caption)},                                     \
      {PT_FIELD_customer_ratings_menu_title, sizeof(customer_ratings_menu_title)},                                     \
      {PT_FIELD_customer_ratings_furniture_title, sizeof(customer_ratings_furniture_title)},                           \
      {PT_FIELD_customer_ratings_title, sizeof(customer_ratings_title)},                                               \
      {PT_FIELD_furniture_menu_caption, sizeof(furniture_menu_caption)},                                               \
      {PT_FIELD_furniture_equipment_caption, sizeof(furniture_equipment_caption)},                                     \
      {PT_FIELD_furniture_van_warning, sizeof(furniture_van_warning)},                                                 \
      {PT_FIELD_furniture_info_formats, sizeof(furniture_info_formats)},                                               \
      {PT_FIELD_restaurant_close_confirm_text, sizeof(restaurant_close_confirm_text)},                                 \
      {PT_FIELD_restaurant_need_ovens_text, sizeof(restaurant_need_ovens_text)},                                       \
      {PT_FIELD_restaurant_need_chefs_text, sizeof(restaurant_need_chefs_text)},                                       \
      {PT_FIELD_restaurant_need_waiters_text, sizeof(restaurant_need_waiters_text)},                                   \
      {PT_FIELD_restaurant_need_recipes_text, sizeof(restaurant_need_recipes_text)},                                   \
      {PT_FIELD_restaurant_closed_today_text, sizeof(restaurant_closed_today_text)},                                   \
      {PT_FIELD_restaurant_open_confirm_text, sizeof(restaurant_open_confirm_text)},                                   \
      {PT_FIELD_restaurant_rearrange_text, sizeof(restaurant_rearrange_text)},                                         \
      {PT_FIELD_restaurant_nothing_text, sizeof(restaurant_nothing_text)},                                             \
      {PT_FIELD_restaurant_media_prompt, sizeof(restaurant_media_prompt)},                                             \
      {PT_FIELD_recipe_price_limit, sizeof(recipe_price_limit)},                                                       \
      {PT_FIELD_recipe_price_scale, sizeof(recipe_price_scale)},                                                       \
      {PT_FIELD_recipe_list_title, sizeof(recipe_list_title)},                                                         \
      {PT_FIELD_recipe_list_page_format, sizeof(((PTNamedMemory *)0)->saved_recipe_list_page_format)},                 \
      {PT_FIELD_recipe_list_new, sizeof(recipe_list_new)},                                                             \
      {PT_FIELD_recipe_file_format, sizeof(((PTNamedMemory *)0)->saved_recipe_file_format)},                           \
      {PT_FIELD_recipe_list_save_custom_error, sizeof(recipe_list_save_custom_error)},                                 \
      {PT_FIELD_recipe_list_recreate_prompt, sizeof(recipe_list_recreate_prompt)},                                     \
      {PT_FIELD_recipe_list_create_first_error, sizeof(recipe_list_create_first_error)},                               \
      {PT_FIELD_recipe_list_already_menu_error, sizeof(recipe_list_already_menu_error)},                               \
      {PT_FIELD_recipe_price_prompt, sizeof(recipe_price_prompt)},                                                     \
      {PT_FIELD_recipe_list_menu_full_error, sizeof(recipe_list_menu_full_error)},                                     \
      {PT_FIELD_pizza_menu_page_format, sizeof(((PTNamedMemory *)0)->saved_pizza_menu_page_format)},                   \
      {PT_FIELD_pizza_menu_sales_title, sizeof(pizza_menu_sales_title)},                                               \
      {PT_FIELD_pizza_menu_title, sizeof(pizza_menu_title)},                                                           \
      {PT_FIELD_pizza_menu_sales_label, sizeof(pizza_menu_sales_label)},                                               \
      {PT_FIELD_pizza_menu_turnover_label, sizeof(pizza_menu_turnover_label)},                                         \
      {PT_FIELD_pizza_menu_popularity_label, sizeof(pizza_menu_popularity_label)},                                     \
      {PT_FIELD_pizza_menu_demand_label, sizeof(pizza_menu_demand_label)},                                             \
      {PT_FIELD_pizza_menu_info_sales_label, sizeof(pizza_menu_info_sales_label)},                                     \
      {PT_FIELD_pizza_menu_keep_one_error, sizeof(pizza_menu_keep_one_error)},                                         \
      {PT_FIELD_pizza_menu_remove_selection_error, sizeof(pizza_menu_remove_selection_error)},                         \
      {PT_FIELD_profit_graph_scale_base, sizeof(profit_graph_scale_base)},                                             \
      {PT_FIELD_legacy_graph_scale_base, sizeof(legacy_graph_scale_base)},                                             \
      {PT_FIELD_cash_flow_caption, sizeof(cash_flow_caption)},                                                         \
      {PT_FIELD_profit_graph_units_format, sizeof(((PTNamedMemory *)0)->saved_profit_graph_units_format)},             \
      {PT_FIELD_profit_graph_weeks, sizeof(profit_graph_weeks)},                                                       \
      {PT_FIELD_cash_graph_scale_base, sizeof(cash_graph_scale_base)},                                                 \
      {PT_FIELD_daily_profit_caption, sizeof(daily_profit_caption)},                                                   \
      {PT_FIELD_profit_graph_days, sizeof(profit_graph_days)},                                                         \
      {PT_FIELD_daily_profit_scale_base, sizeof(daily_profit_scale_base)},                                             \
      {PT_FIELD_balance_labels, sizeof(((PTNamedMemory *)0)->saved_balance_labels)},                                   \
      {PT_FIELD_weekly_profit_text, sizeof(weekly_profit_text)},                                                       \
      {PT_FIELD_finance_stats_labels, sizeof(((PTNamedMemory *)0)->saved_finance_stats_labels)},                       \
      {PT_FIELD_monthly_overview_caption, sizeof(monthly_overview_caption)},                                           \
      {PT_FIELD_monthly_result_caption, sizeof(monthly_result_caption)},                                               \
      {PT_FIELD_monthly_restaurants_caption, sizeof(monthly_restaurants_caption)},                                     \
      {PT_FIELD_daily_settlement_text, sizeof(daily_settlement_text)},                                                 \
      {PT_FIELD_daily_day_before_text, sizeof(daily_day_before_text)},                                                 \
      {PT_FIELD_daily_yesterday_text, sizeof(daily_yesterday_text)},                                                   \
      {PT_FIELD_daily_today_text, sizeof(daily_today_text)}, {PT_FIELD_daily_orders_text, sizeof(daily_orders_text)},  \
      {PT_FIELD_daily_pizzas_sold_text, sizeof(daily_pizzas_sold_text)},                                               \
      {PT_FIELD_daily_delivery_capacity_text, sizeof(daily_delivery_capacity_text)},                                   \
      {PT_FIELD_daily_demand_value_text, sizeof(daily_demand_value_text)},                                             \
      {PT_FIELD_weekly_sales_text, sizeof(weekly_sales_text)},                                                         \
      {PT_FIELD_daily_ingredients_used_text, sizeof(daily_ingredients_used_text)},                                     \
      {PT_FIELD_daily_expenditure_text, sizeof(daily_expenditure_text)},                                               \
      {PT_FIELD_weekly_week_text, sizeof(weekly_week_text)},                                                           \
      {PT_FIELD_weekly_takings_text, sizeof(weekly_takings_text)},                                                     \
      {PT_FIELD_weekly_items_text, sizeof(weekly_items_text)},                                                         \
      {PT_FIELD_weekly_costs_text, sizeof(weekly_costs_text)},                                                         \
      {PT_FIELD_weekly_total_text, sizeof(weekly_total_text)},                                                         \
      {PT_FIELD_weekly_expenses_text, sizeof(weekly_expenses_text)},                                                   \
      {PT_FIELD_weekly_ingredients_text, sizeof(weekly_ingredients_text)},                                             \
      {PT_FIELD_weekly_cooks_text, sizeof(weekly_cooks_text)},                                                         \
      {PT_FIELD_weekly_staff_name_format, sizeof(((PTNamedMemory *)0)->saved_weekly_staff_name_format)},               \
      {PT_FIELD_weekly_empty_staff_format, sizeof(((PTNamedMemory *)0)->saved_weekly_empty_staff_format)},             \
      {PT_FIELD_weekly_waiters_text, sizeof(weekly_waiters_text)},                                                     \
      {PT_FIELD_weekly_managers_text, sizeof(weekly_managers_text)},                                                   \
      {PT_FIELD_restaurant_overview_day, sizeof(restaurant_overview_day)},                                             \
      {PT_FIELD_restaurant_overview_week, sizeof(restaurant_overview_week)},                                           \
      {PT_FIELD_restaurant_overview_caption, sizeof(restaurant_overview_caption)},                                     \
      {PT_FIELD_rest_sales_caption, sizeof(rest_sales_caption)},                                                       \
      {PT_FIELD_rest_customers_caption, sizeof(rest_customers_caption)},                                               \
      {PT_FIELD_rest_history_date_format, sizeof(((PTNamedMemory *)0)->saved_rest_history_date_format)},               \
      {PT_FIELD_rest_weekly_caption, sizeof(rest_weekly_caption)},                                                     \
      {PT_FIELD_rest_weekly_columns, sizeof(rest_weekly_columns)},                                                     \
      {PT_FIELD_advert_substitution_format, sizeof(((PTNamedMemory *)0)->saved_advert_substitution_format)},           \
      {PT_FIELD_advert_output_format, sizeof(((PTNamedMemory *)0)->saved_advert_output_format)},                       \
      {PT_FIELD_advert_flyer_format, sizeof(((PTNamedMemory *)0)->saved_advert_flyer_format)},                         \
      {PT_FIELD_advert_output_label, sizeof(advert_output_label)},                                                     \
      {PT_FIELD_advert_costs_label, sizeof(advert_costs_label)},                                                       \
      {PT_FIELD_advert_poster_format, sizeof(((PTNamedMemory *)0)->saved_advert_poster_format)},                       \
      {PT_FIELD_advert_newspaper_cost_format, sizeof(((PTNamedMemory *)0)->saved_advert_newspaper_cost_format)},       \
      {PT_FIELD_advert_tv_format, sizeof(((PTNamedMemory *)0)->saved_advert_tv_format)},                               \
      {PT_FIELD_advert_tv_title, sizeof(advert_tv_title)}, {PT_FIELD_advert_tv_director, sizeof(advert_tv_director)},  \
      {PT_FIELD_advert_days, sizeof(advert_days)}, {PT_FIELD_advert_misc_filename, sizeof(advert_misc_filename)},      \
      {PT_FIELD_advert_already_active, sizeof(advert_already_active)},                                                 \
      {PT_FIELD_end_turn_phone_text, sizeof(end_turn_phone_text)},                                                     \
      {PT_FIELD_market_share_price_divisor, sizeof(market_share_price_divisor)},                                       \
      {PT_FIELD_market_share_base_threshold, sizeof(market_share_base_threshold)},                                     \
      {PT_FIELD_insufficient_cash_text, sizeof(insufficient_cash_text)},                                               \
      {PT_FIELD_ledger_limit, sizeof(ledger_limit)},                                                                   \
      {PT_FIELD_periodic_extortion_text, sizeof(periodic_extortion_text)},                                             \
      {PT_FIELD_restaurant_opening_text, sizeof(restaurant_opening_text)},                                             \
      {PT_FIELD_restaurant_closing_text, sizeof(restaurant_closing_text)},                                             \
      {PT_FIELD_cash_goal_thresholds, sizeof(cash_goal_thresholds)},                                                   \
      {PT_FIELD_delayed_message_format, sizeof(delayed_message_format)},                                               \
      {PT_FIELD_supplier_price_scale, sizeof(supplier_price_scale)},                                                   \
      {PT_FIELD_flooring_unlock_hint, sizeof(flooring_unlock_hint)},                                                   \
      {PT_FIELD_supplier_debts_paid_format, sizeof(((PTNamedMemory *)0)->saved_supplier_debts_paid_format)},           \
      {PT_FIELD_supplier_debts_due_format, sizeof(((PTNamedMemory *)0)->saved_supplier_debts_due_format)},             \
      {PT_FIELD_overdrawn_account_text, sizeof(overdrawn_account_text)},                                               \
      {PT_FIELD_van_furniture_value_format, sizeof(((PTNamedMemory *)0)->saved_van_furniture_value_format)},           \
      {PT_FIELD_weekly_profit_caption, sizeof(weekly_profit_caption)},                                                 \
      {PT_FIELD_weekly_cash_history_limit, sizeof(weekly_cash_history_limit)},                                         \
      {PT_FIELD_monthly_cost_caption, sizeof(monthly_cost_caption)},                                                   \
      {PT_FIELD_monthly_repayment_caption, sizeof(monthly_repayment_caption)},                                         \
      {PT_FIELD_monthly_interest_caption, sizeof(monthly_interest_caption)},                                           \
      {PT_FIELD_monthly_insurance_caption, sizeof(monthly_insurance_caption)},                                         \
      {PT_FIELD_monthly_investment_interest_caption, sizeof(monthly_investment_interest_caption)},                     \
      {PT_FIELD_monthly_rent_caption, sizeof(monthly_rent_caption)},                                                   \
      {PT_FIELD_monthly_total_caption, sizeof(monthly_total_caption)},                                                 \
      {PT_FIELD_player_card_dot, sizeof(player_card_dot)}, {PT_FIELD_end_turn_wait_text, sizeof(end_turn_wait_text)},  \
      {PT_FIELD_sound_driver_signature, sizeof(((PTNamedMemory *)0)->saved_sound_driver_signature)},                   \
      {PT_FIELD_timezone_env_name, sizeof(timezone_env_name)},                                                         \
      {PT_FIELD_watcom_character_classes, sizeof(watcom_character_classes)},                                           \
      {PT_FIELD_price_parser_positive_infinity, sizeof(price_parser_positive_infinity)},                               \
      {PT_FIELD_scale10_positive_infinity, sizeof(((PTNamedMemory *)0)->saved_scale10_positive_infinity)},             \
      {PT_FIELD_scale10_negative_infinity, sizeof(((PTNamedMemory *)0)->saved_scale10_negative_infinity)},             \
      {PT_FIELD_scale10_positive_max, sizeof(scale10_positive_max)},                                                   \
      {PT_FIELD_scale10_negative_max, sizeof(scale10_negative_max)},                                                   \
      {PT_FIELD_time_common_month_days, sizeof(time_common_month_days)},                                               \
      {PT_FIELD_time_leap_month_days, sizeof(time_leap_month_days)},                                                   \
      {PT_FIELD_scale10_powers, sizeof(scale10_powers)}, {PT_FIELD_shop_purchase_price, sizeof(shop_purchase_price)},  \
      {PT_FIELD_purchase_amount, sizeof(purchase_amount)}, {PT_FIELD_loaded_map_file, sizeof(loaded_map_file)},        \
      {PT_FIELD_g_staff, sizeof(g_staff)},                                                                             \
      {PT_FIELD_serialized_mission_receipts, sizeof(((PTNamedMemory *)0)->saved_serialized_mission_receipts)},         \
      {PT_FIELD_global_balance, sizeof(global_balance)},                                                               \
      {PT_FIELD_serialized_previous_month_expenses,                                                                    \
        sizeof(((PTNamedMemory *)0)->saved_serialized_previous_month_expenses)},                                       \
      {PT_FIELD_staff_is_type, sizeof(staff_is_type)},                                                                 \
      {PT_FIELD_serialized_previous_month_receipts,                                                                    \
        sizeof(((PTNamedMemory *)0)->saved_serialized_previous_month_receipts)},                                       \
      {PT_FIELD_serialized_player_loans, sizeof(((PTNamedMemory *)0)->saved_serialized_player_loans)},                 \
      {PT_FIELD_staff_hates_player, sizeof(staff_hates_player)},                                                       \
      {PT_FIELD_serialized_player_investments, sizeof(((PTNamedMemory *)0)->saved_serialized_player_investments)},     \
      {PT_FIELD_serialized_mission_expenses, sizeof(((PTNamedMemory *)0)->saved_serialized_mission_expenses)},         \
      {PT_FIELD_blitmap_offsets, sizeof(blitmap_offsets)},                                                             \
      {PT_FIELD_mission_year, sizeof(((PTNamedMemory *)0)->saved_mission_year)},                                       \
      {PT_FIELD_mission_target_year, sizeof(((PTNamedMemory *)0)->saved_mission_target_year)},                         \
      {PT_FIELD_mission_day, sizeof(((PTNamedMemory *)0)->saved_mission_day)},                                         \
      {PT_FIELD_mission_target_day, sizeof(((PTNamedMemory *)0)->saved_mission_target_day)},                           \
      {PT_FIELD_mission_month, sizeof(((PTNamedMemory *)0)->saved_mission_month)},                                     \
      {PT_FIELD_mission_target_month, sizeof(((PTNamedMemory *)0)->saved_mission_target_month)},                       \
      {PT_FIELD_message_slots_free, sizeof(message_slots_free)},                                                       \
      {PT_FIELD_mission_goal, sizeof(((PTNamedMemory *)0)->saved_mission_goal)},                                       \
      {PT_FIELD_mission_target_goal, sizeof(((PTNamedMemory *)0)->saved_mission_target_goal)},                         \
      {PT_FIELD_weapon_base_price, sizeof(weapon_base_price)},                                                         \
      {PT_FIELD_delayed_msg_contents, sizeof(delayed_msg_contents)}, {PT_FIELD_bananas, sizeof(bananas)},              \
      {PT_FIELD_map_animation_palette_colors, sizeof(map_animation_palette_colors)},                                   \
      {PT_FIELD_map_animation_palette_repeat, sizeof(map_animation_palette_repeat)},                                   \
      {PT_FIELD_array_size_8, sizeof(array_size_8)}, {PT_FIELD_extra_town_data, sizeof(extra_town_data)},              \
      {PT_FIELD_igloos, sizeof(((PTNamedMemory *)0)->saved_serialized_igloos)},                                        \
      {PT_FIELD_map_igloo_tail, sizeof(((PTNamedMemory *)0)->saved_serialized_map_igloo_tail)},                        \
      {PT_FIELD_map_igloo_head, sizeof(((PTNamedMemory *)0)->saved_serialized_map_igloo_head)},                        \
      {PT_FIELD_animated_stats_reputation, sizeof(animated_stats_reputation)},                                         \
      {PT_FIELD_animated_stats_quality, sizeof(animated_stats_quality)},                                               \
      {PT_FIELD_temp_igloo_1, sizeof(((PTNamedMemory *)0)->saved_serialized_temp_igloo_1)},                            \
      {PT_FIELD_temp_igloo_2, sizeof(((PTNamedMemory *)0)->saved_serialized_temp_igloo_2)},                            \
      {PT_FIELD_guests, sizeof(guests)},                                                                               \
      {PT_FIELD_largest_map_column_bitmaps,                                                                            \
        sizeof(((PTNamedMemory *)0)->saved_serialized_largest_map_column_bitmaps)},                                    \
      {PT_FIELD_map_row_bitmaps, sizeof(((PTNamedMemory *)0)->saved_serialized_map_row_bitmaps)},                      \
      {PT_FIELD_largest_map_row_bitmaps, sizeof(((PTNamedMemory *)0)->saved_serialized_largest_map_row_bitmaps)},      \
      {PT_FIELD_map_column_bitmaps, sizeof(((PTNamedMemory *)0)->saved_serialized_map_column_bitmaps)},                \
      {PT_FIELD_temp_igloo, sizeof(((PTNamedMemory *)0)->saved_serialized_temp_igloo)},                                \
      {PT_FIELD_strbuf, sizeof(strbuf)}, {PT_FIELD_palette_e, sizeof(palette_e)},                                      \
      {PT_FIELD_players, sizeof(((PTNamedMemory *)0)->saved_serialized_players)},                                      \
      {PT_FIELD_pal_pointer, sizeof(((PTNamedMemory *)0)->saved_serialized_pal_pointer)},                              \
      {PT_FIELD_characters, sizeof(characters)},                                                                       \
      {PT_FIELD_display_bitmaps, sizeof(((PTNamedMemory *)0)->saved_serialized_display_bitmaps)},                      \
      {PT_FIELD_popup_menu_items, sizeof(((PTNamedMemory *)0)->saved_serialized_popup_menu_items)},                    \
      {PT_FIELD_value_of_crashed_furniture_1, sizeof(value_of_crashed_furniture_1)},                                   \
      {PT_FIELD_rest_floormap, sizeof(rest_floormap)}, {PT_FIELD_attack_damage, sizeof(attack_damage)},                \
      {PT_FIELD_loaded_bez_file, sizeof(loaded_bez_file)},                                                             \
      {PT_FIELD_current_purchase_amount, sizeof(current_purchase_amount)},                                             \
      {PT_FIELD_durian, sizeof(((PTNamedMemory *)0)->saved_serialized_durian)},                                        \
      {PT_FIELD_active_input_class, sizeof(active_input_class)},                                                       \
      {PT_FIELD_hazelnuts, sizeof(((PTNamedMemory *)0)->saved_serialized_hazelnuts)},                                  \
      {PT_FIELD_cur_loading_filehandle, sizeof(((PTNamedMemory *)0)->saved_serialized_cur_loading_filehandle)},        \
      {PT_FIELD_num_written_bytes, sizeof(num_written_bytes)},                                                         \
      {PT_FIELD_ingredient_background, sizeof(((PTNamedMemory *)0)->saved_serialized_ingredient_background)},          \
      {PT_FIELD_num_read_bytes, sizeof(num_read_bytes)},                                                               \
      {PT_FIELD_cur_writing_filehandle, sizeof(((PTNamedMemory *)0)->saved_serialized_cur_writing_filehandle)},        \
      {PT_FIELD_buffer_backup, sizeof(((PTNamedMemory *)0)->saved_serialized_buffer_backup)},                          \
      {PT_FIELD_last_recipe_ptr, sizeof(((PTNamedMemory *)0)->saved_serialized_last_recipe_ptr)},                      \
      {PT_FIELD_restaurant_with_guests, sizeof(((PTNamedMemory *)0)->saved_serialized_restaurant_with_guests)},        \
      {PT_FIELD_restaurant_hit_test_owner, sizeof(((PTNamedMemory *)0)->saved_serialized_restaurant_hit_test_owner)},  \
      {PT_FIELD_game, sizeof(((PTNamedMemory *)0)->saved_serialized_game)},                                            \
      {PT_FIELD_some_juice, sizeof(((PTNamedMemory *)0)->saved_serialized_some_juice)},                                \
      {PT_FIELD_bmp16_ptr, sizeof(((PTNamedMemory *)0)->saved_serialized_bmp16_ptr)},                                  \
      {PT_FIELD_last_input_event, sizeof(((PTNamedMemory *)0)->saved_serialized_last_input_event)},                    \
      {PT_FIELD_font_cstandar, sizeof(((PTNamedMemory *)0)->saved_serialized_font_cstandar)},                          \
      {PT_FIELD_exit_optional_font, sizeof(((PTNamedMemory *)0)->saved_serialized_exit_optional_font)},                \
      {PT_FIELD_canvas1_ptr, sizeof(((PTNamedMemory *)0)->saved_serialized_canvas1_ptr)},                              \
      {PT_FIELD_font_astandar, sizeof(((PTNamedMemory *)0)->saved_serialized_font_astandar)},                          \
      {PT_FIELD_recipe_lookup_fallback, sizeof(((PTNamedMemory *)0)->saved_serialized_recipe_lookup_fallback)},        \
      {PT_FIELD_some_font_ptr, sizeof(((PTNamedMemory *)0)->saved_serialized_some_font_ptr)},                          \
      {PT_FIELD_font_supersma, sizeof(((PTNamedMemory *)0)->saved_serialized_font_supersma)},                          \
      {PT_FIELD_font_ruby, sizeof(((PTNamedMemory *)0)->saved_serialized_font_ruby)},                                  \
      {PT_FIELD_font_piznumb, sizeof(((PTNamedMemory *)0)->saved_serialized_font_piznumb)},                            \
      {PT_FIELD_buffer_2, sizeof(((PTNamedMemory *)0)->saved_serialized_buffer_2)},                                    \
      {PT_FIELD_current_bitmap, sizeof(((PTNamedMemory *)0)->saved_serialized_current_bitmap)},                        \
      {PT_FIELD_old_bitmap, sizeof(((PTNamedMemory *)0)->saved_serialized_old_bitmap)},                                \
      {PT_FIELD_active_palette_pointer, sizeof(((PTNamedMemory *)0)->saved_serialized_active_palette_pointer)},        \
      {PT_FIELD_ptr_apple, sizeof(((PTNamedMemory *)0)->saved_serialized_ptr_apple)},                                  \
      {PT_FIELD_curr_rest, sizeof(((PTNamedMemory *)0)->saved_serialized_curr_rest)},                                  \
      {PT_FIELD_game_ptr, sizeof(((PTNamedMemory *)0)->saved_serialized_game_ptr)},                                    \
      {PT_FIELD_mouse_result, sizeof(((PTNamedMemory *)0)->saved_serialized_mouse_result)},                            \
      {PT_FIELD_jatoba_1, sizeof(((PTNamedMemory *)0)->saved_serialized_jatoba_1)},                                    \
      {PT_FIELD_canvas_1, sizeof(((PTNamedMemory *)0)->saved_serialized_canvas_1)},                                    \
      {PT_FIELD_yaffa_value, sizeof(yaffa_value)}, {PT_FIELD_yaffa_counter, sizeof(yaffa_counter)},                    \
      {PT_FIELD_walking_guest, sizeof(((PTNamedMemory *)0)->saved_serialized_walking_guest)},                          \
      {PT_FIELD_bmp6, sizeof(((PTNamedMemory *)0)->saved_serialized_bmp6)},                                            \
      {PT_FIELD_bmp15, sizeof(((PTNamedMemory *)0)->saved_serialized_bmp15)},                                          \
      {PT_FIELD_bmp5, sizeof(((PTNamedMemory *)0)->saved_serialized_bmp5)},                                            \
      {PT_FIELD_bmp14, sizeof(((PTNamedMemory *)0)->saved_serialized_bmp14)},                                          \
      {PT_FIELD_temp_palette_slot, sizeof(((PTNamedMemory *)0)->saved_serialized_temp_palette_slot)},                  \
      {PT_FIELD_bmp3, sizeof(((PTNamedMemory *)0)->saved_serialized_bmp3)},                                            \
      {PT_FIELD_bmp11, sizeof(((PTNamedMemory *)0)->saved_serialized_bmp11)},                                          \
      {PT_FIELD_bmp2, sizeof(((PTNamedMemory *)0)->saved_serialized_bmp2)},                                            \
      {PT_FIELD_bmp1, sizeof(((PTNamedMemory *)0)->saved_serialized_bmp1)},                                            \
      {PT_FIELD_bmp10, sizeof(((PTNamedMemory *)0)->saved_serialized_bmp10)},                                          \
      {PT_FIELD_bmp12, sizeof(((PTNamedMemory *)0)->saved_serialized_bmp12)},                                          \
      {PT_FIELD_did_run_town_day_loop_1, sizeof(did_run_town_day_loop_1)},                                             \
      {PT_FIELD_temp_hazel, sizeof(((PTNamedMemory *)0)->saved_serialized_temp_hazel)},                                \
      {PT_FIELD_savegame_sect_0, sizeof(savegame_sect_0)}, {PT_FIELD_ramtown, sizeof(ramtown)},                        \
      {PT_FIELD_active_input_value, sizeof(active_input_value)},                                                       \
      {PT_FIELD_did_load_map_and_bez, sizeof(did_load_map_and_bez)},                                                   \
      {PT_FIELD_cur_list_elem_x, sizeof(cur_list_elem_x)}, {PT_FIELD_cur_list_elem_y, sizeof(cur_list_elem_y)},        \
      {PT_FIELD_mouse_x, sizeof(mouse_x)}, {PT_FIELD_mouse_y, sizeof(mouse_y)},                                        \
      {PT_FIELD_mouse_pressed, sizeof(mouse_pressed)},                                                                 \
      {PT_FIELD_temp_rest, sizeof(((PTNamedMemory *)0)->saved_serialized_temp_rest)},                                  \
      {PT_FIELD_papaya_array, sizeof(papaya_array)},                                                                   \
      {PT_FIELD_room, sizeof(((PTNamedMemory *)0)->saved_serialized_room)},                                            \
      {PT_FIELD_temp_igloo_link, sizeof(((PTNamedMemory *)0)->saved_serialized_temp_igloo_link)},                      \
      {PT_FIELD_map_igloo_link, sizeof(((PTNamedMemory *)0)->saved_serialized_map_igloo_link)},                        \
      {PT_FIELD_bmp16, sizeof(((PTNamedMemory *)0)->saved_serialized_bmp16)},                                          \
      {PT_FIELD_bmp25, sizeof(((PTNamedMemory *)0)->saved_serialized_bmp25)},                                          \
      {PT_FIELD_bmp24, sizeof(((PTNamedMemory *)0)->saved_serialized_bmp24)},                                          \
      {PT_FIELD_bmp_background, sizeof(((PTNamedMemory *)0)->saved_serialized_bmp_background)},                        \
      {PT_FIELD_bmp_awards, sizeof(((PTNamedMemory *)0)->saved_serialized_bmp_awards)},                                \
      {PT_FIELD_bmp17, sizeof(((PTNamedMemory *)0)->saved_serialized_bmp17)},                                          \
      {PT_FIELD_canvas1, sizeof(((PTNamedMemory *)0)->saved_serialized_canvas1)},                                      \
      {PT_FIELD_loaded_character, sizeof(loaded_character)}, {PT_FIELD_temp_character, sizeof(temp_character)},        \
      {PT_FIELD_bmp_blitmap, sizeof(((PTNamedMemory *)0)->saved_serialized_bmp_blitmap)},                              \
      {PT_FIELD_map_scroll_x, sizeof(map_scroll_x)}, {PT_FIELD_map_scroll_y, sizeof(map_scroll_y)},                    \
      {PT_FIELD_attack_contamination, sizeof(attack_contamination)},                                                   \
      {PT_FIELD_best_score_player, sizeof(best_score_player)}, {PT_FIELD_state_slot_014, sizeof(state_slot_014)},      \
      {PT_FIELD_map_actor_count, sizeof(map_actor_count)}, {PT_FIELD_state_slot_015, sizeof(state_slot_015)},          \
      {PT_FIELD_curr_score, sizeof(curr_score)}, {PT_FIELD_state_slot_016, sizeof(state_slot_016)},                    \
      {PT_FIELD_state_slot_017, sizeof(state_slot_017)}, {PT_FIELD_guest_count, sizeof(guest_count)},                  \
      {PT_FIELD_player_stats_value_x, sizeof(player_stats_value_x)},                                                   \
      {PT_FIELD_draw_list_elem_style, sizeof(draw_list_elem_style)},                                                   \
      {PT_FIELD_did_terminal_stuff, sizeof(did_terminal_stuff)},                                                       \
      {PT_FIELD_attack_police_outcome, sizeof(attack_police_outcome)},                                                 \
      {PT_FIELD_max_num_characters, sizeof(max_num_characters)},                                                       \
      {PT_FIELD_current_player_index, sizeof(current_player_index)},                                                   \
      {PT_FIELD_staff_display_flag, sizeof(staff_display_flag)},                                                       \
      {PT_FIELD_shop_purchase_secondary_price, sizeof(shop_purchase_secondary_price)},                                 \
      {PT_FIELD_shop_secondary_purchase_amount, sizeof(shop_secondary_purchase_amount)},                               \
      {PT_FIELD_temp_cutoff, sizeof(temp_cutoff)}, {PT_FIELD_depot_export_country, sizeof(depot_export_country)},      \
      {PT_FIELD_cursor_bitmap, sizeof(((PTNamedMemory *)0)->saved_serialized_cursor_bitmap)},                          \
      {PT_FIELD_cursor_background_bitmap, sizeof(((PTNamedMemory *)0)->saved_serialized_cursor_background_bitmap)},    \
      {PT_FIELD_canvas_3, sizeof(((PTNamedMemory *)0)->saved_serialized_canvas_3)},                                    \
      {PT_FIELD_map_background_buffer, sizeof(((PTNamedMemory *)0)->saved_serialized_map_background_buffer)},          \
      {PT_FIELD_cursor_target_bitmap, sizeof(((PTNamedMemory *)0)->saved_serialized_cursor_target_bitmap)},            \
      {PT_FIELD_serialized_display_bias, sizeof(((PTNamedMemory *)0)->saved_serialized_display_bias)},                 \
      {PT_FIELD_transparent_mask_buffer, sizeof(((PTNamedMemory *)0)->saved_serialized_transparent_mask_buffer)},      \
      {PT_FIELD_some_pal_p, sizeof(((PTNamedMemory *)0)->saved_serialized_some_pal_p)},                                \
      {PT_FIELD_startup_word0, sizeof(startup_word0)}, {PT_FIELD_startup_word1, sizeof(startup_word1)},                \
      {PT_FIELD_startup_word2, sizeof(startup_word2)},                                                                 \
      {PT_FIELD_mouse_bitmap_1, sizeof(((PTNamedMemory *)0)->saved_serialized_mouse_bitmap_1)},                        \
      {PT_FIELD_mouse_bitmap_2, sizeof(((PTNamedMemory *)0)->saved_serialized_mouse_bitmap_2)},                        \
      {PT_FIELD_interactive_attack_attacker,                                                                           \
        sizeof(((PTNamedMemory *)0)->saved_serialized_interactive_attack_attacker)},                                   \
      {PT_FIELD_interactive_attack_victim, sizeof(((PTNamedMemory *)0)->saved_serialized_interactive_attack_victim)},  \
      {PT_FIELD_g_pigs, sizeof(((PTNamedMemory *)0)->saved_serialized_g_pigs)},                                        \
      {PT_FIELD_rest_path_visits, sizeof(rest_path_visits)}, {PT_FIELD_guest_portrait_x, sizeof(guest_portrait_x)},    \
      {PT_FIELD_guest_portrait_y, sizeof(guest_portrait_y)}, {PT_FIELD_state_slot_031, sizeof(state_slot_031)},        \
      {PT_FIELD_state_slot_018, sizeof(state_slot_018)}, {PT_FIELD_rest_path_target_x, sizeof(rest_path_target_x)},    \
      {PT_FIELD_rest_path_target_y, sizeof(rest_path_target_y)}, {PT_FIELD_state_slot_019, sizeof(state_slot_019)},    \
      {PT_FIELD_state_slot_020, sizeof(state_slot_020)},                                                               \
      {PT_FIELD_recipe_rank_character, sizeof(((PTNamedMemory *)0)->saved_serialized_recipe_rank_character)},          \
      {PT_FIELD_time_left_text, sizeof(time_left_text)}, {PT_FIELD_selected_filename, sizeof(selected_filename)},      \
      {PT_FIELD_savegame_slot, sizeof(savegame_slot)}, {PT_FIELD_map_marker_phase, sizeof(map_marker_phase)},          \
      {PT_FIELD_animated_order_snapshot, sizeof(animated_order_snapshot)},                                             \
      {PT_FIELD_animated_pizza_snapshot, sizeof(animated_pizza_snapshot)},                                             \
      {PT_FIELD_animated_finished_snapshot, sizeof(animated_finished_snapshot)},                                       \
      {PT_FIELD_animated_order_max, sizeof(animated_order_max)},                                                       \
      {PT_FIELD_animated_oven_baking_time, sizeof(animated_oven_baking_time)},                                         \
      {PT_FIELD_animated_oven_capacity, sizeof(animated_oven_capacity)},                                               \
      {PT_FIELD_animated_oven_used, sizeof(animated_oven_used)},                                                       \
      {PT_FIELD_animated_finished_count, sizeof(animated_finished_count)},                                             \
      {PT_FIELD_animated_order_count, sizeof(animated_order_count)},                                                   \
      {PT_FIELD_animated_stats_needle, sizeof(animated_stats_needle)},                                                 \
      {PT_FIELD_animated_oven_count, sizeof(animated_oven_count)},                                                     \
      {PT_FIELD_animated_stats_arrow_drawn, sizeof(animated_stats_arrow_drawn)},                                       \
      {PT_FIELD_animated_pizza_count, sizeof(animated_pizza_count)},                                                   \
      {PT_FIELD_animated_pizza_max, sizeof(animated_pizza_max)},                                                       \
      {PT_FIELD_dialled_number_string, sizeof(dialled_number_string)},                                                 \
      {PT_FIELD_dial_number_buffer, sizeof(dial_number_buffer)},                                                       \
      {PT_FIELD_recipe_rank_flag, sizeof(recipe_rank_flag)},                                                           \
      {PT_FIELD_mission_date_buffers, sizeof(mission_date_buffers)},                                                   \
      {PT_FIELD_mission_place_buffers, sizeof(mission_place_buffers)},                                                 \
      {PT_FIELD_mission_text_string, sizeof(mission_text_string)}, {PT_FIELD_temp_string, sizeof(temp_string)},        \
      {PT_FIELD_retained_message_text, sizeof(retained_message_text)},                                                 \
      {PT_FIELD_restaurant_name_text, sizeof(restaurant_name_text)},                                                   \
      {PT_FIELD_menu_hierarchy_backup, sizeof(menu_hierarchy_backup)},                                                 \
      {PT_FIELD_canvas_menu_backup, sizeof(canvas_menu_backup)},                                                       \
      {PT_FIELD_placement_contact_types, sizeof(((PTNamedMemory *)0)->saved_placement_contact_types)},                 \
      {PT_FIELD_placement_contact_indices, sizeof(((PTNamedMemory *)0)->saved_placement_contact_indices)},             \
      {PT_FIELD_placement_contact_amounts, sizeof(((PTNamedMemory *)0)->saved_placement_contact_amounts)},             \
      {PT_FIELD_placement_contact_items, sizeof(((PTNamedMemory *)0)->saved_serialized_placement_contact_items)},      \
      {PT_FIELD_furniture_quality_in_van, sizeof(furniture_quality_in_van)},                                           \
      {PT_FIELD_placement_contact_recorded, sizeof(placement_contact_recorded)},                                       \
      {PT_FIELD_sound_var_2, sizeof(((PTNamedMemory *)0)->saved_serialized_sound_var_2)},                              \
      {PT_FIELD_sound_var_1, sizeof(((PTNamedMemory *)0)->saved_serialized_sound_var_1)},                              \
      {PT_FIELD_sound_flag, sizeof(sound_flag)}, {PT_FIELD_legacy_menu_sales, sizeof(legacy_menu_sales)},              \
      {PT_FIELD_legacy_menu_orders, sizeof(legacy_menu_orders)},                                                       \
      {PT_FIELD_advert_newspaper_lines, sizeof(advert_newspaper_lines)},                                               \
      {PT_FIELD_advert_newspaper_buffer, sizeof(advert_newspaper_buffer)},                                             \
      {PT_FIELD_newspaper_text_buffer2, sizeof(newspaper_text_buffer2)},                                               \
      {PT_FIELD_newspaper_text_buffer, sizeof(newspaper_text_buffer)},                                                 \
      {PT_FIELD_newspaper_article_buffer, sizeof(((PTNamedMemory *)0)->saved_serialized_newspaper_article_buffer)},    \
      {PT_FIELD_newspaper_delay, sizeof(newspaper_delay)}, {PT_FIELD_newpaper_rand, sizeof(newpaper_rand)},            \
      {PT_FIELD_newspaper_random_direction, sizeof(newspaper_random_direction)},                                       \
      {PT_FIELD_newspaper_random_mode, sizeof(newspaper_random_mode)},                                                 \
      {PT_FIELD_newspaper_background, sizeof(newspaper_background)},                                                   \
      {PT_FIELD_newspaper_ingredient, sizeof(newspaper_ingredient)},                                                   \
      {PT_FIELD_did_run_town_day_loop_2, sizeof(did_run_town_day_loop_2)},                                             \
      {PT_FIELD_did_run_town_day_loop_3, sizeof(did_run_town_day_loop_3)},                                             \
      {PT_FIELD_state_slot_021, sizeof(state_slot_021)},                                                               \
      {PT_FIELD_sound_driver_interrupt, sizeof(sound_driver_interrupt)},                                               \
      {PT_FIELD_guest_free_file_nodes, sizeof(((PTNamedMemory *)0)->saved_serialized_guest_free_file_nodes)},          \
      {PT_FIELD_state_slot_022, sizeof(state_slot_022)},                                                               \
      {PT_FIELD_guest_active_file_nodes, sizeof(((PTNamedMemory *)0)->saved_serialized_guest_active_file_nodes)},      \
      {PT_FIELD_delay_polls_per_second, sizeof(delay_polls_per_second)},                                               \
      {PT_FIELD_guest_dos_errno, sizeof(guest_dos_errno)}, {PT_FIELD_guest_errno, sizeof(guest_errno)},                \
      {PT_FIELD_recipe_number_unit, sizeof(recipe_number_unit)},                                                       \
      {PT_FIELD_recipe_load_prompt, sizeof(recipe_load_prompt)},                                                       \
      {PT_FIELD_recipe_save_prompt, sizeof(recipe_save_prompt)}, {PT_FIELD_recipe_load_tag, sizeof(recipe_load_tag)},  \
      {PT_FIELD_recipe_save_tag, sizeof(recipe_save_tag)},                                                             \
      {PT_FIELD_recipe_load_custom_error, sizeof(recipe_load_custom_error)},                                           \
      {PT_FIELD_recipe_duplicate_error, sizeof(recipe_duplicate_error)},                                               \
      {PT_FIELD_recipe_checksum_error, sizeof(recipe_checksum_error)},                                                 \
      {PT_FIELD_weapon_store_quantities, sizeof(weapon_store_quantities)},                                             \
      {PT_FIELD_weapon_store_piece_format, sizeof(((PTNamedMemory *)0)->saved_weapon_store_piece_format)},             \
      {PT_FIELD_weapon_store_sell_format, sizeof(((PTNamedMemory *)0)->saved_weapon_store_sell_format)},               \
      {PT_FIELD_weapon_store_buy_format, sizeof(((PTNamedMemory *)0)->saved_weapon_store_buy_format)},                 \
      {PT_FIELD_intro_music_filename, sizeof(intro_music_filename)},                                                   \
      {PT_FIELD_music_filename_format, sizeof(((PTNamedMemory *)0)->saved_music_filename_format)},                     \
      {PT_FIELD_sound_pcm_cursor, sizeof(((PTNamedMemory *)0)->saved_serialized_sound_pcm_cursor)},                    \
      {PT_FIELD_sound_pcm_remaining, sizeof(sound_pcm_remaining)},                                                     \
      {PT_FIELD_sound_pcm_repeat_start, sizeof(((PTNamedMemory *)0)->saved_serialized_sound_pcm_repeat_start)},        \
      {PT_FIELD_sound_pcm_repeat_end, sizeof(((PTNamedMemory *)0)->saved_serialized_sound_pcm_repeat_end)},            \
      {PT_FIELD_sound_pcm_repeats, sizeof(sound_pcm_repeats)},                                                         \
      {PT_FIELD_sound_dma_record, sizeof(((PTNamedMemory *)0)->saved_serialized_sound_dma_record)},                    \
      {PT_FIELD_sound_digital_configuration, sizeof(sound_digital_configuration)},                                     \
      {PT_FIELD_sound_pcm_output, sizeof(((PTNamedMemory *)0)->saved_serialized_sound_pcm_output)},                    \
      {PT_FIELD_sound_pcm_buffers, sizeof(((PTNamedMemory *)0)->saved_serialized_sound_pcm_buffers)},                  \
      {PT_FIELD_sound_pcm_auto_dma, sizeof(sound_pcm_auto_dma)},                                                       \
      {PT_FIELD_sound_pcm_dma_half, sizeof(sound_pcm_dma_half)},                                                       \
      {PT_FIELD_sound_dos_block_count, sizeof(sound_dos_block_count)},                                                 \
      {PT_FIELD_sound_dos_blocks, sizeof(((PTNamedMemory *)0)->saved_serialized_sound_dos_blocks)},                    \
      {PT_FIELD_sound_dos_selectors, sizeof(sound_dos_selectors)},                                                     \
      {PT_FIELD_sound_pcm_driver_loaded, sizeof(sound_pcm_driver_loaded)},                                             \
      {PT_FIELD_sound_pcm_driver_ready, sizeof(sound_pcm_driver_ready)},                                               \
      {PT_FIELD_sound_pcm_driver_image, sizeof(((PTNamedMemory *)0)->saved_serialized_sound_pcm_driver_image)},        \
      {PT_FIELD_sound_pcm_driver_external, sizeof(sound_pcm_driver_external)},                                         \
      {PT_FIELD_sound_vector_image, sizeof(((PTNamedMemory *)0)->saved_serialized_sound_vector_image)},                \
      {PT_FIELD_sound_vector_filename, sizeof(sound_vector_filename)},                                                 \
      {PT_FIELD_sound_digital_initialized, sizeof(sound_digital_initialized)},                                         \
      {PT_FIELD_sound_timer_previous_vector, sizeof(((PTNamedMemory *)0)->saved_sound_timer_previous_vector)},         \
      {PT_FIELD_sound_timer_active, sizeof(sound_timer_active)},                                                       \
      {PT_FIELD_sound_timer_previous_period, sizeof(sound_timer_previous_period)},                                     \
      {PT_FIELD_sound_timer_period, sizeof(sound_timer_period)},                                                       \
      {PT_FIELD_sound_timer_callback, sizeof(((PTNamedMemory *)0)->saved_serialized_sound_timer_callback)},            \
      {PT_FIELD_sound_config_fallback_word, sizeof(sound_config_fallback_word)},                                       \
      {PT_FIELD_sound_config_read_mode, sizeof(sound_config_read_mode)},                                               \
      {PT_FIELD_sound_config_filename, sizeof(sound_config_filename)},                                                 \
      {PT_FIELD_sound_pcm_driver_filename, sizeof(sound_pcm_driver_filename)},                                         \
      {PT_FIELD_refresh_overflow_format, sizeof(((PTNamedMemory *)0)->saved_refresh_overflow_format)},                 \
      {PT_FIELD_file_open_error_format, sizeof(((PTNamedMemory *)0)->saved_file_open_error_format)},                   \
      {PT_FIELD_file_write_error_format, sizeof(((PTNamedMemory *)0)->saved_file_write_error_format)},                 \
      {PT_FIELD_file_read_error_format, sizeof(((PTNamedMemory *)0)->saved_file_read_error_format)},                   \
      {PT_FIELD_file_missing_retry_format, sizeof(((PTNamedMemory *)0)->saved_file_missing_retry_format)},             \
      {PT_FIELD_no_memory_continue_prompt, sizeof(no_memory_continue_prompt)},                                         \
      {PT_FIELD_ai_restaurant_memory_error, sizeof(ai_restaurant_memory_error)},                                       \
      {PT_FIELD_new_restaurant_memory_error, sizeof(new_restaurant_memory_error)},                                     \
      {PT_FIELD_bob_second_already_linked_error, sizeof(bob_second_already_linked_error)},                             \
      {PT_FIELD_bob_shared_capacity_error, sizeof(bob_shared_capacity_error)},                                         \
      {PT_FIELD_font_error_text, sizeof(font_error_text)}, {PT_FIELD_joystick_center_x, sizeof(joystick_center_x)},    \
      {PT_FIELD_joystick_center_y, sizeof(joystick_center_y)},                                                         \
      {PT_FIELD_guest_psp_selector, sizeof(guest_psp_selector)},                                                       \
      {PT_FIELD_guest_dos_heap_reserve, sizeof(guest_dos_heap_reserve)},                                               \
      {PT_FIELD_legacy_checksum_data, sizeof(((PTNamedMemory *)0)->saved_serialized_legacy_checksum_data)},            \
      {PT_FIELD_legacy_checksum_value, sizeof(((PTNamedMemory *)0)->saved_legacy_checksum_value)},                     \
      {PT_FIELD_legacy_checksum_size, sizeof(((PTNamedMemory *)0)->saved_legacy_checksum_size)},                       \
      {PT_FIELD_personal_character_image_format, sizeof(((PTNamedMemory *)0)->saved_personal_character_image_format)}, \
      {PT_FIELD_legacy_round_half, sizeof(legacy_round_half)},                                                         \
      {PT_FIELD_legacy_scale_word_byte_divisor, sizeof(legacy_scale_word_byte_divisor)},                               \
      {PT_FIELD_legacy_scale_difference_byte_divisor, sizeof(legacy_scale_difference_byte_divisor)},                   \
      {PT_FIELD_legacy_scale_word_word_divisor, sizeof(legacy_scale_word_word_divisor)},                               \
      {PT_FIELD_legacy_scale_difference_word_divisor, sizeof(legacy_scale_difference_word_divisor)},                   \
      {PT_FIELD_legacy_collision_x, sizeof(legacy_collision_x)},                                                       \
      {PT_FIELD_legacy_collision_y, sizeof(legacy_collision_y)}, {PT_FIELD_legacy_walk_x, sizeof(legacy_walk_x)},      \
      {PT_FIELD_legacy_walk_y, sizeof(legacy_walk_y)},                                                                 \
      {PT_FIELD_legacy_walk_direction, sizeof(legacy_walk_direction)},                                                 \
      {PT_FIELD_legacy_walk_animation_phase, sizeof(legacy_walk_animation_phase)},
#endif
