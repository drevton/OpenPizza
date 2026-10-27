// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
#ifndef PT_STATE_TYPES_H
#define PT_STATE_TYPES_H
#include "layout.h"
#include "types.h"

// File stream state.

#include <stdbool.h>
// The anchor remains a native buffer pointer. Advancing changes only the
// position, so rewind/wrap arithmetic never constructs an invalid C pointer.

typedef struct PTBufferCursor {
  u8 *data;
  u32 position;
} PTBufferCursor;
struct GuestFileStream {
  PTBufferCursor cursor;
  u32 buffered_bytes;
  u8 *buffer;
  union {
    u32 status_word;
    struct {
      u8 flags;
      u8 buffer_flags;
      u8 unknown_e[2];
    };
  };
  u32 handle;
  u32 buffer_size;
  u8 inline_buffer;
  u8 temporary_index;
};
enum { PT_FILE_STREAM_COUNT = 20 };
extern GuestFileStream guest_file_streams[PT_FILE_STREAM_COUNT];
typedef struct PTGameReadBuffer {
  u8 *allocation;
  PTBufferCursor cursor;
  u32 capacity;
} PTGameReadBuffer;
GuestFileStream *pt_file_stream_reset(GuestFileStream *stream);
bool pt_file_stream_precedes(const GuestFileStream *stream, u32 first);
u8 *pt_file_buffer_data(const GuestFileStream *stream);
u32 pt_file_buffer_distance(const GuestFileStream *stream);
u8 pt_file_buffer_take(GuestFileStream *stream);
void pt_file_buffer_append(GuestFileStream *stream, u8 value);
void pt_file_stream_buffer_retired(const void *buffer, size_t size);
void pt_game_read_set_cursor(Game *owner, u8 *data, u32 capacity);
bool pt_game_read_available(const Game *owner, u32 size);
const u8 *pt_game_read_data(const Game *owner, u32 size);

// Placement state.

typedef struct PTPlacementContact {
  const Furniture *item;
  u8 index, type, amount;
} PTPlacementContact;
void pt_placement_contacts_reset(void);
void pt_placement_contacts_retire(const void *storage, size_t size);
PTPlacementContact *pt_placement_contact_state(i32);

// Linked list.

// Two real boundary nodes keep the native list independent of its wire layout.

struct LinkedElem {
  LinkedElem *next;
  LinkedElem *previous;
};
typedef LinkedElem PTForwardLink;
struct LinkedHead {
  LinkedElem begin;
  LinkedElem end;
};
void pt_list_init(LinkedHead *);

// Sorting accepts the named keys used by game records, never byte offsets.

typedef enum PTListKey { PT_LIST_RECIPE_ID, PT_LIST_SUPPLIER_QUALITY } PTListKey;

typedef enum PTListResultKind { PT_LIST_OBJECT, PT_LIST_KEY } PTListResultKind;
typedef struct PTListResult {
  PTListResultKind kind;
  const void *object;
  u8 value;
} PTListResult;
static inline PTListResult pt_list_object_result(const void *object) {
  return (PTListResult){PT_LIST_OBJECT, object, 0};
}
LinkedHead *LinkList_InsertFirst(LinkedHead *head, LinkedElem *element);
LinkedHead *LinkList_InsertLast(LinkedHead *head, LinkedElem *element);
PTForwardLink *LinkList_RemoveFirst(LinkedHead *head);
PTForwardLink *LinkList_Remove(LinkedElem *element);
PTListResult LinkList_ByteAt(const LinkedElem *element, PTListKey key);
void *LinkList_InsertAfter(LinkedHead *head, LinkedElem *element, PTForwardLink *previous);
PTListResult LinkList_InsertSorted(LinkedHead *head, LinkedElem *element, PTListKey key);

// Gameplay locals.

#include <stddef.h>

// src/recipe.c

typedef struct __attribute__((packed)) RecipeIngredientSelection {
  u8 selected[96];
  u32 rank;
} RecipeIngredientSelection;
_Static_assert(sizeof(RecipeIngredientSelection) == 100, "FindMajorRecipeIngredients native ingredient-selection size");
_Static_assert(offsetof(RecipeIngredientSelection, selected) == 0, "selected ingredient flags offset");
_Static_assert(offsetof(RecipeIngredientSelection, rank) == 96, "FindMajorRecipeIngredients.rank offset");

// Recipe buffers.

enum { PT_RECIPE_EDITOR_MAX_ENTRIES = 250 };
Recipe *pt_recipe_allocate_file(u32);
u32 pt_recipe_read_file_bytes(Recipe *, u32, u32 (*)(void *, u32));
u32 pt_recipe_file_checksum(const Recipe *, u32, u32 (*)(const void *, u32));
// Saving starts with a native allocation size; loading starts with file bytes.

u32 pt_recipe_saved_checksum(const Recipe *, u32, u32 (*)(const void *, u32));

// Action result.

#include <stdbool.h>
// Actions can return their completed object or a numeric value. An enclosing
// input loop can additionally retain its final event. Register encoding and

typedef enum PTActionResultKind { PT_ACTION_OBJECT, PT_ACTION_VALUE } PTActionResultKind;
typedef struct PTActionResult {
  PTActionResultKind kind;
  const void *object;
  u32 value;
  bool has_event;
  u16 event;
} PTActionResult;
static inline PTActionResult pt_action_object(const void *object) {
  return (PTActionResult){PT_ACTION_OBJECT, object, 0, false, 0};
}
static inline PTActionResult pt_action_value(u32 value) {
  return (PTActionResult){PT_ACTION_VALUE, 0, value, false, 0};
}
static inline PTActionResult pt_action_event(PTActionResult result, u16 event) {
  result.has_event = true;
  result.event = event;
  return result;
}

// Canvas result.

// Some original drawing routines return either an object, a scalar, or the
// final horizontal position alongside their object. Preserve that information

typedef enum PTCanvasResultKind {
  PT_CANVAS_RESULT_OBJECT,
  PT_CANVAS_RESULT_VALUE,
  PT_CANVAS_RESULT_POSITION
} PTCanvasResultKind;
typedef struct PTCanvasResult {
  PTCanvasResultKind kind;
  Canvas *canvas;
  u32 value;
} PTCanvasResult;
static inline PTCanvasResult pt_canvas_result_object(Canvas *canvas) {
  return (PTCanvasResult){PT_CANVAS_RESULT_OBJECT, canvas, 0};
}
static inline PTCanvasResult pt_canvas_result_value(u32 value) {
  return (PTCanvasResult){PT_CANVAS_RESULT_VALUE, 0, value};
}
static inline PTCanvasResult pt_canvas_result_position(Canvas *canvas, u16 x) {
  return (PTCanvasResult){PT_CANVAS_RESULT_POSITION, canvas, x};
}

// Game objects.

#include <stdlib.h>
Supplier *ResolveSupplier(u8 id);
const Recipe *ResolveRecipe(const Player *player, u8 id);
i32 GetRecipeId(const Player *player, const Recipe *recipe);
// Each player owns history and targets as ordinary native records. The
// four byte arrays are also the independently encoded personal-mission fields.

typedef struct PTMissionGoals {
  u8 kind[100];
  u8 day[100];
  u8 month[100];
  u8 year[100];
} PTMissionGoals;
typedef struct PTPlayerGoals {
  PTMissionGoals history;
  PTMissionGoals targets;
} PTPlayerGoals;

struct Contract {
  LinkedElem link;
  Supplier *supplier;
  i16 amount_of_each[24];
  i32 sum;
  u8 discount;
  u8 term_length;
  i16 charge;
  i32 penalty;
  u8 gap48[4];
  u8 amount_mode;
  u8 restaurant_id;
  u8 byte4E;
  u8 unknown_4F[1];
  u8 term_length_total;
  u8 unknown_51[1];
  u8 player_index;
  u8 field_53;
};
struct RestoRecipe {
  u8 sum_of_qualities;
  u8 byte1;
  i16 last_price_per_pizza;
  Recipe *recipe;
  i16 price_per_pizza;
  i16 costs_per_pizza;
  u8 orders;
  u8 sales;
  i16 sold_items;
};
struct Town {
  i32 field_0;
  i16 num_poi;
  i16 field_6;
  u8 num_zones;
  u8 num_supplier;
  u8 unknown_A[2];
  i32 field_C;
  union {
    struct {
      i32 field_10;
      u8 gap14[20];
    };
    u8 name[24];
  };
  u8 num_phone_nr_of_kind[6];
  u8 phone_number_flags[6][10];
  i32 phone_numbers[6][10];
  union {
    struct {
      u8 realtors[240];
      u8 bank_names[240];
      u8 insurance_company_names[240];
      u8 authorities_names[240];
      u8 others_names[240];
      u8 gap60A[240];
    };
    u8 phonebook_names[6][10][24];
  };
  RealtorDetails realtor_details[10];
  u8 loan_details[10][4];
  u8 insurance_details[10][4];
  u8 gap772[40];
  TownMafia town_mafia[2];
  u8 gap7A2[72];
  u16 base_stock_prices[4][24];
  i8 ingredient_trend[96];
  i8 stockarr90A[96];
  i16 field_96A;
  u8 gap96C[38];
  i16 word992;
  u8 gap994[52];
  LinkedHead poi;
  LinkedHead zones;
  LinkedHead suppliers;
};
struct Supplier {
  LinkedElem link;
  u8 stock_kind;
  u8 quality;
  u8 supplier_id;
  u8 base_price_addon;
  u8 name[26];
  i16 minimum_order;
  i32 bonus_levels;
  i32 total_bought_by_player[8];
  u8 gap4C[64];
  u8 last_term_length[8];
  u8 num_contracts_with_player[8];
  i16 last_amount[8];
  i32 current_account[8];
  u8 last_discount[8];
  u8 had_shortages[8];
  u8 unpaid_bill_level[8];
  u8 annoyed;
  u8 gapE5[2];
  u8 byteE7;
  u8 num_times_said[8];
  u8 gapF0;
  u8 bitmaps;
  u8 gapF2[5];
  u8 price_addon[24];
  i16 num_items_left[24];
};
struct Zone {
  LinkedElem link;
  u8 traffic_density;
  u8 zone_id;
  u8 name[28];
  u8 active_mafia;
  u8 gap27[15];
  u16 unknown_word;
  u8 byte38;
  u8 crime_statistics;
  u8 byte3A;
  u8 how_low_income;
  u8 byte3C;
  u8 unknown_3D[1];
  u8 byte3E;
  u8 byte3F;
  u8 gap40;
  u8 type_of_zone;
  i16 base_size;
  i16 rand_size;
  i16 base_price_level;
  i16 base_rent_level;
  u8 gap4A[4];
  u8 premises_availability;
  u8 unknown_4F[1];
  float population_distribution[6];
};
struct Poi {
  LinkedElem link;
  u8 xcoord;
  u8 ycoord;
  PropertyType property_type;
  u8 current_owner;
  u8 current_tenant;
  u8 for_sale_or_rent_flags;
  u8 map_value;
  u8 realtor_index;
  Rest *restaurant;
  u8 size_x;
  u8 size_y;
  u8 kitchen_width;
  u8 kitchen_height;
  u8 door_x_start;
  u8 left_door_y_end;
  u8 byte1A;
  u8 months_left_of_lease;
  i32 purchase_price;
  i32 rent_price;
  u8 field_24;
  u8 field_25;
  u8 field_26;
  u8 field_27;
  union {
    struct {
      i16 customer_count_in_area[6];
      i32 total_customer_count;
    };
    u16 weapon_quantity[8];
  };
};
typedef struct PTRecipeTastes {
  i16 values[4];
} PTRecipeTastes;
struct Recipe {
  LinkedElem linked;
  i16 rating[7];
  u8 cooking_skill_of_char;
  u8 id;
  u8 name[10];
  u8 field_22;
  u8 field_23;
  u8 field_24;
  u8 field_25;
  u8 field_26;
  u8 field_27;
  u8 field_28;
  u8 field_29;
  u8 field_2A;
  u8 field_2B;
  u8 field_2C;
  u8 field_2D;
  u8 field_2E;
  u8 field_2F;
  u8 field_30;
  u8 field_31;
  u8 field_32;
  u8 field_33;
  u8 field_34;
  u8 field_35;
  u8 field_36;
  u8 field_37;
  u8 field_38;
  u8 field_39;
  u8 field_3A;
  u8 field_3B;
  i16 word3C;
  u8 gap3E[3];
  u8 imitation;
  u8 unknown_42[2];
  u8 major_ingredient[4];
  union {
    struct {
      i16 fat, sweet, salty, hot;
    };
    PTRecipeTastes tastes;
  };
  u8 appearance;
  u8 refinement;
  u8 normality;
  u8 amount_of_spices;
  u8 age_type_rating[6];
  i16 calories;
  u8 field_5C;
  u8 field_5D;
  u8 field_5E;
  u8 field_5F;
  u8 field_60;
  u8 field_61;
  i16 weight_of_type[96];
  u8 num_unique_ingredients;
  u8 num_entries;
  size_t entry_capacity;
  RecipeEntry entries[];
};
struct Player {
  i16 field_0;
  u8 gap2[11];
  u8 city_index;
  u8 gapE;
  u8 num_weapons[8];
  i32 average_weapon_price[8];
  PlayerDialledNumbers city_part;
  u8 num_restaurants_in_city[10];
  Depot *depots[10];
  i32 weekly_profit_by_town[10];
  i32 profit_last_30_days[30];
  double money;
  Poi *selected_property;
  LinkedHead restaurants;
  LinkedHead recipes;
  LinkedHead contracts;
  u8 num_recipes;
  u8 num_restaurants;
  u8 num_contracts;
  u8 character_index;
  AiRecipe ai_recipes[10];
  u8 num_days_passed_in_town[10];
  u8 num_restaurants_open;
  u8 gap23E[13];
  i16 debt_penalty;
  u8 byte24D;
  u8 gap24E[3];
  Mission mission;
  union {
    struct {
      u8 name, random_name, gap287[18];
    };
    u8 name_text[20];
  };
  union {
    struct {
      i32 money_as_integer[2];
      u8 gap2A1[188];
      i32 new_money_as_integer;
    };
    PTCashHistoryEntry cash_history[50];
  };
  u8 gap361;
  u8 index_of_next_goal;
  u8 days_away;
  u8 gap364;
  i16 sales_per_day_monthly[30];
  i32 accum_sales_per_day_monthly[30];
  i32 expenditure_monthly[30];
  Peach peaches[4];
  u8 last_insurance_damage_type;
  u8 bankruptcy_reason_hint;
  i32 last_crashed_furniture_value;
};
struct Apple {
  u8 field_0;
  u8 field_1;
  u8 field_2;
  u8 field_3;
  u8 field_4;
  u8 field_5;
  u8 field_6;
  u8 field_7;
  u8 field_8;
  u8 field_9;
  u8 field_A;
  u8 field_B;
  u8 field_C;
  u8 clock_flag;
  Rest *restaurant;
  LinkedHead links;
  u8 arr1E;
  u8 town_index;
  u8 weekday;
  u8 is_town_inited[10];
  u8 minutesx10;
  u8 day;
  u8 month;
  i16 year;
  u8 field_30;
  u8 field_31;
  u8 is_in_kitchen;
  u8 message_ticks_left;
  u8 field_34;
  u8 byte35;
  u8 player_index;
  u8 difficulty_level;
  u8 byte38;
  u8 byte39;
  u8 num_days_with_proper_stats;
  u8 num_valid_daily_profits_entries;
  u8 gap3C;
  i32 town_sizes[10];
  u8 field_65;
  u8 view_state;
};
struct Rest {
  LinkedElem linked;
  Poi *property;
  u8 player_index;
  u8 unknown_D[1];
  u8 restaurant_range_squared;
  u8 restaurant_range;
  u8 num_stars;
  u8 delivcapa_per_day[7];
  u8 restaurant_id;
  u8 days_until_major_visit;
  u8 protection_value;
  i16 average_pizza_price;
  i16 word1D;
  i16 word1F;
  u8 popularity_with_customer[6];
  i16 sum_of_reputations;
  i16 byte29;
  u8 byte2B;
  u8 byte2C;
  u8 quality_of_things[4];
  union {
    struct {
      u8 reputation[8];
      u8 range_stuff_1[4];
      u8 range_stuff_2[4];
    };
    // The statistics animation includes both temporary-effect arrays.

    u8 reputation_and_effects[16];
  };
  u8 gap41[4];
  i16 customer_count[6];
  u8 furniture_rating[6];
  u8 menu_rating[6];
  i16 total_num_seats;
  u8 gap5F[4];
  i16 oven_capacity;
  u8 gap65[2];
  i16 demand_of_kind[5];
  i16 capacity_of_kind[5];
  u8 demand_capacity_ratio[5];
  union {
    struct {
      u8 furniture_quality;
      u8 unknown_81[1];
      u8 average_cooking_rating;
      u8 oven_quality;
      u8 average_waiter_rating;
    };
    u8 quality_block[5];
  };
  u8 original_map_value;
  i16 attack_var_1;
  u8 local_share;
  u8 restaurant_closed_for_days;
  u8 gap8A[2];
  u8 byte8C;
  u8 target_coordinate_x;
  u8 target_coordinate_y;
  u8 byte8F;
  u8 gap90;
  i16 word91;
  u8 gap93[4];
  u8 restaurant_opened;
  u8 use_real_demand;
  u8 byte99;
  u8 gap9A;
  u8 pizza_quality;
  u8 attack_var;
  u8 some_counter;
  u8 attacked_by_whom;
  u8 ai_preferred_age_group;
  u8 default_rating;
  u8 template_rest;
  u8 some_furniture_counter;
  u8 gapA3[4];
  i16 orders_per_day[7];
  i16 pizza_count_per_day[7];
  i16 demand_value_per_day[7];
  i16 sales_per_day[7];
  i32 expenditure_per_day[7];
  i16 expenses_per_day[7];
  u8 gap109[2];
  i32 curr_month_expenses[12];
  i32 curr_month_receipts[6];
  i32 last_month_expenses[12];
  i32 last_month_receipts[6];
  i16 guests_per_day[7];
  u8 gap1A9[2];
  float average_chef_time;
  float average_oven_time;
  float average_waiter_time;
  float average_eating_time;
  float average_stay_time;
  u8 num_chefs;
  u8 num_waiters;
  u8 staff_indexes[13];
  u8 gap1CE[13];
  union {
    struct {
      u8 mgr_shopping, mgr_change_prices, mgr_overdraw_acct;
    };
    u8 manager_authority[3];
  };
  union {
    struct {
      u8 mgr_turnover_decrease, mgr_turnover_increase, mgr_supply_shortage, mgr_staff_shortage;
    };
    u8 manager_reactions[4];
  };
  u8 mgr_staff_behavior;
  u8 manager_flag_1;
  u8 manager_had_to_help;
  u8 gap1E5[6];
  u8 waiting_time;
  u8 num_recipes;
  u8 num_standard_recipes;
  u8 num_ovens;
  u8 gap1EF[5];
  i32 dword1F4;
  RestoRecipe recipes[24];
  u8 gap378[7];
  u8 average_purchase_quality;
  u8 gap380;
  u8 num_furniture;
  u8 gap382[6];
  u8 byte388;
  u8 gap389[6];
  u8 byte38F;
  u8 gap390[5];
  u8 scrollpos_x;
  u8 scrollpos_y;
  u8 kitchen_scroll_x;
  u8 kitchen_scroll_y;
  union {
    struct {
      union {
        struct {
          Floor floor;
          u8 base_beautiness[6];
        };
        FloorPurchase floor_purchase;
      };
      u8 field_3A6[19];
    };
    RestaurantFloorState floor_state;
  };
  u8 field_3B9;
  u8 is_in_kitchen;
  u8 jukebox_selection;
  u8 tv_selection;
  RestaurantStock stock[96];
  Furniture furniture[100];
  Furniture ovens[12];
  i16 weekly_ingredient_type_cost[4];
  u8 last_supplier_of_stock[4];
  u8 field_9F9;
};

_Static_assert(sizeof(((Rest *)0)->reputation_and_effects) == 16, "complete restaurant animation input");
_Static_assert(offsetof(Rest, reputation_and_effects) == offsetof(Rest, reputation) &&
        offsetof(Rest, range_stuff_1) == offsetof(Rest, reputation) + 8 &&
        offsetof(Rest, range_stuff_2) == offsetof(Rest, reputation) + 12,
    "restaurant animation includes reputation and temporary effects");
static inline u32 pt_recipe_allocation_bytes(u32 entries) {
  if (entries > UINT8_MAX)
    abort();
  return (u32)sizeof(Recipe) + (entries + 1u) * (u32)sizeof(RecipeEntry);
}
extern Player players[8];
extern Town durian;
extern Rest temp_rest;
extern Apple room;
// Optional save boundary observes actual stores; reads use native fields.

typedef enum PTSimulationKind {
  PT_SIM_CONTRACT,
  PT_SIM_RESTORECIPE,
  PT_SIM_TOWN,
  PT_SIM_SUPPLIER,
  PT_SIM_ZONE,
  PT_SIM_POI,
  PT_SIM_RECIPE,
  PT_SIM_PLAYER,
  PT_SIM_APPLE,
  PT_SIM_REST,
  PT_SIM_MOUSE,
  PT_SIM_KIND_COUNT
} PTSimulationKind;
// The game owns native allocations. The save boundary assigns file identities
// and retires their metadata when the game releases the object.

typedef struct PTSimulationStorage {
  bool (*bind)(PTSimulationKind, size_t, void *);
  void (*release)(const void *, size_t);
} PTSimulationStorage;
void pt_simulation_storage(const PTSimulationStorage *);
void *pt_simulation_allocate(PTSimulationKind, size_t);
void pt_simulation_release(const void *, size_t);
Recipe *pt_recipe_copy(Recipe *, const Recipe *, u32 entries);
Recipe *pt_recipe_reset(Recipe *);

typedef struct PTPaletteSlot PTPaletteSlot;
// Native state owns its pointers; exact saved bytes are a boundary concern.

struct Game {
  u8 field_0;
  u8 field_1;
  u8 field_2;
  u8 field_3;
  u8 field_4;
  u8 field_5;
  u8 field_6;
  u8 unknown_7[1];
  i32 dword8;
  u8 gapC;
  u8 is_mission_mode;
  u8 in_menu_hierarchy;
  u8 disable_staff_disasters;
  u8 gap10[24];
  u8 num_interactive_players;
  u8 mask_of_players;
  u8 gap2A[4];
  u8 byte2E;
  u8 paragraph_first_indent_height;
  u8 paragraph_indent_y_threshold;
  u8 list_element_spacing;
  u8 paragraph_background_enabled;
  u8 paragraph_first_indent_height_backup;
  u8 paragraph_indent_y_threshold_backup;
  u8 paragraph_background_enabled_backup;
  u8 gap36[24];
  PTPaletteSlot *palette_slots[2];
  u8 gap56[20];
  Bitmap *background;
  u8 gap6E[19];
  u8 field_81;
  u8 last_loaded_rest_bg;
  u8 mouse_cursor_visible;
  u8 current_palette;
  u8 info_box_state;
  u8 byte86;
  u8 byte87;
  u8 gap88;
  u8 allow_save_game;
  u8 did_set_bitmap;
  u8 current_button_menu;
  u8 some_button_menu;
  u8 list_element_spacing_2;
  u8 list_element_spacing_backup;
  u8 did_choose_tv_thingy;
  i16 word90;
  i16 word92;
  u8 byte94;
  u8 legacy_box_text_color;
  u8 legacy_box_top_left_color;
  u8 legacy_box_bottom_right_color;
  u16 legacy_box_width;
  u16 legacy_box_height;
  u16 legacy_box_text_offset_x;
  u8 last_loaded_blitmap_index;
  u8 last_loaded_background;
  u8 gapA0[2];
  u8 sound_enabled;
  u8 gapA3[3];
  u8 canvas_color_backup;
  u8 canvas_byteE_backup;
  u8 canvas_byte10_backup;
  u8 unknown_A9[1];
  u8 byteAA;
  u8 byteAB;
  u8 current_bitmap_index;
  u8 old_bitmap_index;
  u8 swapped_when_setting_bitmap;
  u8 gapAF[2];
  u8 debug_mode;
  u8 byteB2;
  u8 force_freestyle_jury;
  u8 byteB4;
  i16 canvas_position_x_backup;
  i16 canvas_position_y_backup;
  u8 flagB9;
  u8 show_time_left_instead_of_date;
  u32 legacy_timer_value;
  i32 seconds_left;
  i32 minutes_left;
  u8 gapC7;
  u8 map_zoom_level;
  u8 gapC9[11];
  u8 field_D4;
  u8 gapD5[7];
  u8 field_DC;
  const Font *font_backup;
  Player *player_table;
  Character *character_table;
  u8 graphics_reserved[8];
  u8 *scratch_first, *scratch_second;
  i32 dwordF9;
  Rect phone_text_rect;
  u8 phone_text_rect_flag;
  u8 phone_text_rect_color;
  u8 byte106;
  u8 byte107;
  u16 legacy_word109;
  u16 legacy_word10B;
  union {
    struct {
      u8 field_10C;
      u8 unknown_10E;
    };
    u16 legacy_word10D;
  };
  u8 text_edit_max_width;
  u8 byte10F;
  u8 skip_missing_file_dialog;
  u8 byte111;
  u8 byte112;
  u8 paragraph_render_mode;
  u8 byte114;
  u8 show_disk_read_errors;
  u8 is_loading_from_buffer;
  PTGameReadBuffer read_buffer;
  u32 loading_buffer_size;
  i32 dword124;
  LangFileDesc *language;
  LangFileDesc *language_storage;
};
extern Game game;
extern Town *current_town;
Game *pt_game_initialize(Game *);

// Live C object pointers. Image codecs observe assignments and restore these
// owners once when loading; ordinary reads never decode an address word.

extern Rect *rect_1;
extern Zone *mouse_over_zone;
extern Zone *last_mouse_over_district;
extern const u8 *unit_for_number_query;
extern const u8 *button_texts[34][9];
extern Player *player_ptr;
extern Character *character_ptr;
extern Poi *last_customer_count_poi;
extern const u8 *current_button_hint;
extern const u8 *popup_menu_items[24];
extern GuestFileStream *cur_loading_filehandle;
extern u8 *ingredient_background;
extern GuestFileStream *cur_writing_filehandle;
extern u8 *buffer_backup;
extern Recipe *last_recipe_ptr;
extern Rest *restaurant_with_guests;
extern Rest *restaurant_hit_test_owner;
extern InputEvent *last_input_event;
extern Recipe *recipe_lookup_fallback;
extern u8 *buffer_2;
extern Apple *ptr_apple;
extern Rest *curr_rest;
extern Game *game_ptr;
extern Mouse *mouse_result;
extern Jatoba *jatoba_1;
extern Character *interactive_attack_attacker;
extern Character *interactive_attack_victim;
extern Character *recipe_rank_character;
extern u8 *newspaper_article_buffer;

// Process tables.

#include <stdbool.h>
#include <stddef.h>
typedef struct __attribute__((packed, may_alias)) PTFileModeSlot {
  u32 mode;
} PTFileModeSlot;
_Static_assert(sizeof(PTFileModeSlot) == 4, "file mode stride");
extern PTFileModeSlot *guest_io_mode_table;
void pt_process_tables_reset(void);
PTFileModeSlot *pt_file_mode(u32 handle);
void pt_file_mode_set(u32 handle, u32 mode);
void pt_file_mode_or(u32 handle, u32 bits);
void pt_process_environment_import(const u8 *const *entries, size_t count);
bool pt_process_environment_present(void);
const u8 *pt_environment_string(u32 slot);

// Data views.

Loan *pt_player_loan(u8 player, u8 index);
Investment *pt_player_investment(u8 player, u8 index);
u8 pt_map_tile_read(u32 column, u32 row);
u8 pt_map_tile_read_signed(i32 column, i32 row);
u8 pt_supplier_ingredient_count(u8 kind);
i8 pt_supplier_ingredient_trend(u8 ingredient);

// Town day.

void RunCityNewDay(void);
u8 DoYaffaThing(u8 event);
u8 DoSomeMoreYaffaThing(void);
void DoSomeYaffaStuff(void);
void RecipeRankingsNoOp(void);
void ComputeRecipeRankings(void);
void SetIngredientTrends(u8 index, u8 initial_trend, u8 target_trend, u8 mode);
void DoMongoStuff(void);
void RunCityNewWeek(void);
void RunCityNewMonth(void);
void RunTownIngredientDailyLoop(u8 ingredient);
u32 DecreasePlayerFoodQualities(void);
void InitPapayaArrays(void);
void RunTownWeeklyLoop(void);
void RunTownDailyLoop(void);
void RunTownMonthlyLoop(void);
u16 GetPoiPropType(const Poi *poi);

#endif
