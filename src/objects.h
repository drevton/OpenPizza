// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
#ifndef PT_OBJECTS_H
#define PT_OBJECTS_H
#include "render_types.h"
#include "state_types.h"
#include "memory.h"
#include "sound_types.h"

// World objects.

#include <stdbool.h>
#include <stdlib.h>
enum { PT_BOB_DISK_HEADER_BYTES = 7 };
struct IglooLink {
  u8 field_0;
  Igloo *first;
  Igloo *last;
};
struct Hazelnut {
  i16 flag;
  u8 *background;
  Igloo *actor;
};
// Sprite dimensions are also borrowed as a two-byte, possibly unaligned view.

typedef struct PTSpriteDimensions {
  u8 width, height;
} PTSpriteDimensions;
_Static_assert(
    sizeof(PTSpriteDimensions) == 2 && _Alignof(PTSpriteDimensions) == 1, "sprite dimensions are two ordinary bytes");
struct BobHeader {
  union {
    struct {
      u8 w, h;
    };
    PTSpriteDimensions dimensions;
  };
  u8 field_2;
  u8 *pixels;
};
_Static_assert(offsetof(BobHeader, dimensions) == 0, "sprite dimensions prefix");
struct LangFileDesc {
  u8 num_entries;
  u8 field_1;
  i16 size;
  u8 *buffer;
  u8 name[10];
};
struct Igloo {
  Igloo *next;
  Igloo *previous;
  i16 last_y;
  i16 last_x;
  i16 flag;
  i16 draw_y;
  i16 draw_x;
  i16 h;
  i16 w;
  u8 *pixels;
  Hazelnut *sprite;
  u8 unknown_1E[18];
  u8 movement_remaining;
  u8 tile_column;
  u8 tile_row;
  u8 orientation;
  u8 movement_pause;
  u8 actor_index;
  u8 shortened_turn;
  u8 is_unused;
};
struct BobHost {
  i16 num_entries;
  i32 size;
  u8 *data;
  BobHost *next;
  BobHeader *headers;
  u8 *payload;
  i32 dword16;
  u8 flags;
  u8 filename[26];
  u8 *header_source;
  size_t header_capacity;
  bool owns_headers;
};
extern IglooLink temp_igloo_link;
extern IglooLink map_igloo_link;
extern Hazelnut hazelnuts[150];
extern Hazelnut temp_hazel;
extern LangFileDesc langfile_artikel3;
extern LangFileDesc langfile_credits;
extern LangFileDesc langfile_ende;
extern LangFileDesc langfile_gtalk0;
extern LangFileDesc langfile_gtalk1;
extern LangFileDesc langfile_markt;
extern LangFileDesc langfile_action;
extern LangFileDesc langfile_werbung;
extern LangFileDesc langfile_levent;
extern LangFileDesc langfile_mafia;
extern LangFileDesc langfile_waffen;
extern LangFileDesc langfile_realtors;
extern LangFileDesc langfile_bank;
extern LangFileDesc langfile_insurance;
extern LangFileDesc langfile_townhall;
extern LangFileDesc langfile_police;
extern LangFileDesc langfile_lief;
extern LangFileDesc langfile_leben;
extern LangFileDesc langfile_pers;
extern LangFileDesc langfile_persf;
extern LangFileDesc langfile_staff_applications[5];
extern LangFileDesc langfile_double;
extern Igloo igloos[150];
extern Igloo map_igloo_tail;
extern Igloo map_igloo_head;
extern Igloo temp_igloo_1;
extern Igloo temp_igloo_2;
extern Igloo temp_igloo;
extern BobHost bob_restaurant;
extern BobHost bob_ingredients;
extern BobHost bob_kitchen;
extern BobHost bob_restaurant_open;
extern BobHost bob_stanbobs;
extern BobHost bob_typ;
extern BobHost bob_walk;
extern BobHost bob7;
extern BobHost bob11;
extern BobHost menu_bob;
extern BobHost bob_restaurant_stats;
extern BobHost bob9;
// Fail before an invalid index can form an out-of-bounds native lvalue.

_Noreturn void pt_resource_bounds_failure(const char *resource, i64 index, size_t count);
// Pool indices are bounded independently of the original saved stride.

static inline Igloo *pt_igloo_pool_at(unsigned index) {
  if (index >= sizeof(igloos) / sizeof(*igloos))
    pt_resource_bounds_failure("actor pool", index, sizeof(igloos) / sizeof(*igloos));
  return &igloos[index];
}
static inline Hazelnut *pt_hazelnut_pool_at(unsigned index) {
  if (index >= sizeof(hazelnuts) / sizeof(*hazelnuts))
    pt_resource_bounds_failure("sprite pool", index, sizeof(hazelnuts) / sizeof(*hazelnuts));
  return &hazelnuts[index];
}
struct MouseInitializer {
  i32 field_0;
  Jatoba *parent;
};
struct Mouse {
  i16 y;
  i16 x;
  Jatoba *parent;
  Canvas *canvas;
  union {
    i32 dwordC;
    u32 input_filter;
  };
};
struct Pig {
  i16 x;
  i16 y;
  u8 byte4;
  u8 byte5;
  u8 gap6;
  u8 byte7;
  i16 x_coord;
  i16 y_coord;
  u8 weapon_type;
  u8 byteD;
  u8 byteE;
  u8 byteF;
  union {
    struct {
      u8 direction_unk, gap11[7];
    };
    u8 path_directions[8];
  };
  union {
    struct {
      u8 field_18, gap19[6], field_1F;
    };
    u8 path_steps[8];
  };
  union {
    u8 byte20, path_depth;
  };
  u8 byte21;
  u8 byte22;
  u8 dead_unk;
  u8 personality_attrib;
  u8 byte25;
  i16 word26;
  u8 byte28;
  u8 byte29;
  u8 byte2A;
  u8 field_2B;
  u8 gap2C[2];
  i16 byte2E;
  i16 word30;
  i16 word32;
  i16 word34;
  Pig *partner;
  Character *character;
};
_Static_assert(sizeof(((Pig *)0)->path_directions) == 8 && sizeof(((Pig *)0)->path_steps) == 8,
    "eight owned walking-history entries");
_Static_assert(offsetof(Pig, path_directions) == offsetof(Pig, direction_unk) &&
        offsetof(Pig, path_steps) == offsetof(Pig, field_18) && offsetof(Pig, path_depth) == offsetof(Pig, byte20),
    "walking-history field aliases");
extern Pig walking_guest, g_pigs[2];
extern MouseInitializer mouse_initializer;
Mouse *pt_mouse_create(void);

void pt_bob_headers_storage_reset(void);
int pt_bob_format_filename(BobHost *, const char *, unsigned);
BobHeader *pt_bob_decode_headers(BobHost *, u16);
const BobHeader *pt_resource_header_at(const BobHost *, i32);
const u8 *pt_resource_actor_availability(unsigned);
BobHeader *pt_bob_headers_set(BobHost *owner, const BobHeader *value);

static inline Depot *pt_player_depot(const Player *player, unsigned town) {
  if (town >= 10)
    pt_resource_bounds_failure("player depots", town, 10);
  return player->depots[town];
}

// Guest time.

typedef struct __attribute__((packed, may_alias)) PTTimeValue {
  u32 value;
} PTTimeValue;
typedef struct PTTimezoneClock {
  PTTimeValue second, minute, hour;
} PTTimezoneClock;
typedef struct PTTimezoneRuleValues {
  PTTimezoneClock clock;
  PTTimeValue number;
} PTTimezoneRuleValues;
_Static_assert(sizeof(PTTimeValue) == 4 && sizeof(PTTimezoneClock) == 12 && sizeof(PTTimezoneRuleValues) == 16,
    "timezone native word layouts");
_Static_assert(
    offsetof(PTTimezoneClock, hour) == 8 && offsetof(PTTimezoneRuleValues, number) == 12, "timezone word offsets");
u32 GuestTime(PTTimeValue *output);
u32 GuestGetCurrentTime(GuestTM *time);
u32 GuestMktime(GuestTM *time);
u32 GuestLeapYear(i32 year);
GuestTM *GuestBreakTime(u32 days, u32 seconds, i32 offset, GuestTM *time);
i32 GuestRuleYearDay(const GuestTM *rule, i32 year);
u32 GuestRuleOrder(const GuestTM *first, const GuestTM *second, i32 year);
u32 GuestTimeLess(const GuestTM *first, const GuestTM *second);
i32 GuestIsInDST(GuestTM *time);
const u8 *GuestGetEnv(const u8 *name);
i32 GuestCaseCompare(const u8 *first, const u8 *second, u32 count);
typedef struct PTTimezoneResult {
  const u8 *end;
  bool has_dst;
} PTTimezoneResult;
PTTimezoneResult GuestTzset(void);
const u8 *GuestParseTime(const u8 *text, PTTimeValue *output);
const u8 *GuestParseOffset(const u8 *text, u8 *name, PTTimeValue *offset);
const u8 *GuestParseRule(const u8 *text, GuestTM *rule);
PTTimezoneResult GuestParseTimezone(const u8 *text);

// Objects.

static inline const u8 *pt_season_name_identity(u8 season) {
  static const u8 *const names[] = {season_names.spring, season_names.summer, season_names.fall, season_names.winter};
  if (season >= sizeof(names) / sizeof(*names))
    pt_resource_bounds_failure("season", season, sizeof(names) / sizeof(*names));
  return names[season];
}

static inline u8 pt_character_player_index(const Character *character) {
  for (u8 index = 0; index < sizeof(characters) / sizeof(*characters); ++index)
    if (character == &characters[index])
      return index;
  pt_resource_bounds_failure("character ownership", -1, sizeof(characters) / sizeof(*characters));
}

// Each four-color window can extend from the first palette into its repeat.

const PTRGB *pt_map_animation_colors(u16 phase);
static inline u8 *pt_loading_buffer(void) {
  const Game *state = game_ptr;
  return state->read_buffer.allocation;
}
const PTPreferencesFileImage *pt_preferences_file_image(void);

// Signed catalogue/quality indices retain their original physical domains.
// Field-sized image and link views also preserve reads outside semantic rows.

typedef struct __attribute__((packed, may_alias)) PTFurnitureCategoryName {
  u8 text[14];
} PTFurnitureCategoryName;
_Static_assert(sizeof(PTFurnitureCategoryName) == 14, "furniture category name stride");
static inline const u8 *pt_furniture_category_name(u8 category) {
  if (category >= sizeof(furniture_category_names) / sizeof(*furniture_category_names))
    pt_resource_bounds_failure(
        "furniture category", category, sizeof(furniture_category_names) / sizeof(*furniture_category_names));
  return furniture_category_names[category];
}
static inline i32 pt_furniture_record_number(const Rest *rest, const Furniture *item, bool kitchen) {
  const Furniture *first = kitchen ? rest->ovens : rest->furniture;
  size_t count =
      kitchen ? sizeof(rest->ovens) / sizeof(*rest->ovens) : sizeof(rest->furniture) / sizeof(*rest->furniture);
  for (size_t index = 0; index < count; ++index)
    if (item == &first[index])
      return (i32)index;
  pt_resource_bounds_failure("furniture ownership", -1, count);
}
static inline const u8 *pt_recipe_age_rating_field(const Recipe *recipe, u8 age) {
  if (age >= sizeof(recipe->age_type_rating))
    pt_resource_bounds_failure("recipe age group", age, sizeof(recipe->age_type_rating));
  return &recipe->age_type_rating[age];
}
// Identity views preserve partial records and defer reads to named fields.

static inline const FurnitureDesc *pt_furniture_descriptor_identity(u8 type) {
  if (type >= sizeof(furniture_desc) / sizeof(*furniture_desc))
    pt_resource_bounds_failure("furniture catalogue", type, sizeof(furniture_desc) / sizeof(*furniture_desc));
  return &furniture_desc[type];
}
static inline const Guest *pt_guest_identity(u8 index) {
  if (index >= sizeof(guests) / sizeof(*guests))
    pt_resource_bounds_failure("customer", index, sizeof(guests) / sizeof(*guests));
  return &guests[index];
}
static inline FurnitureDesc *pt_furniture_descriptor(u32 type) {
  if (type >= sizeof(furniture_desc) / sizeof(*furniture_desc))
    pt_resource_bounds_failure("furniture catalogue", type, sizeof(furniture_desc) / sizeof(*furniture_desc));
  return &furniture_desc[type];
}
u16 pt_furniture_image(const FurnitureDesc *descriptor, i16 orientation);
bool pt_furniture_van_amount(i16 row, u8 *amount);
const u8 *pt_furniture_van_quality(i16 index);
static inline u8 *pt_rest_furniture_link(Rest *rest, u8 index, bool kitchen) {
  if (index >=
      (kitchen ? sizeof(rest->ovens) / sizeof(*rest->ovens) : sizeof(rest->furniture) / sizeof(*rest->furniture)))
    pt_resource_bounds_failure("restaurant furniture",
        index,
        kitchen ? sizeof(rest->ovens) / sizeof(*rest->ovens) : sizeof(rest->furniture) / sizeof(*rest->furniture));
  Furniture *item = kitchen ? &rest->ovens[index] : &rest->furniture[index];
  return &item->linked_furniture;
}
_Static_assert(offsetof(BobHeader, w) == 0 && offsetof(BobHeader, h) == 1, "sprite dimension offsets");
// The resource owns eight categories of twenty-four fixed-width names.

static inline const u8 *pt_extra_town_name(u8 kind, u8 owner) {
  unsigned row = 24u * kind + owner;
  if (row >= 8u * 24u)
    pt_resource_bounds_failure("extra town name", row, 8u * 24u);
  // Original owner indices may cross into the next owned category.

  return extra_town_data[row / 24u][row % 24u];
}

// Native records and table indices have explicit bounds.

static inline const u8 *pt_month_name_identity(u32 index) {
  if (index >= sizeof(month_names) / sizeof(*month_names))
    pt_resource_bounds_failure("month name", index, sizeof(month_names) / sizeof(*month_names));
  return month_names[index];
}
static inline const u8 *pt_ordinal_suffix_identity(u8 index) {
  if (index >= sizeof(ordinal_suffixes) / sizeof(*ordinal_suffixes))
    pt_resource_bounds_failure("ordinal suffix", index, sizeof(ordinal_suffixes) / sizeof(*ordinal_suffixes));
  return ordinal_suffixes[index];
}

// Six serialized age groups share four signed flavor preferences and four
// byte fields. The remaining byte meanings are still unidentified.

_Static_assert(sizeof(PTAgeTastePreference) == 12, "age taste row stride");
_Static_assert(offsetof(PTAgeTastePreference, fat) == 0, "age taste fat offset");
_Static_assert(offsetof(PTAgeTastePreference, sweet) == 2, "age taste sweet offset");
_Static_assert(offsetof(PTAgeTastePreference, salty) == 4, "age taste salty offset");
_Static_assert(offsetof(PTAgeTastePreference, hot) == 6, "age taste hot offset");
_Static_assert(offsetof(PTAgeTastePreference, unknown_8) == 8, "age taste byte 8 offset");
_Static_assert(offsetof(PTAgeTastePreference, unknown_9) == 9, "age taste byte 9 offset");
_Static_assert(offsetof(PTAgeTastePreference, unknown_a) == 10, "age taste byte a offset");
_Static_assert(offsetof(PTAgeTastePreference, unknown_b) == 11, "age taste byte b offset");
PTAgeTastePreference *pt_age_taste(unsigned age);

// Native records and table indices have explicit bounds.

static inline PTCashHistoryEntry *pt_player_cash_history_entry(u8 player, u8 index) {
  if (player >= sizeof(players) / sizeof(*players))
    pt_resource_bounds_failure("player", player, sizeof(players) / sizeof(*players));
  if (index >= sizeof(players[0].cash_history) / sizeof(*players[0].cash_history))
    pt_resource_bounds_failure(
        "cash history", index, sizeof(players[0].cash_history) / sizeof(*players[0].cash_history));
  return &players[player].cash_history[index];
}

// Signed direction offsets belong to the native eight-entry tables.

// Month lookups are bounded by the native calendar table.

u8 pt_calendar_month_days(i8 month);

_Static_assert(sizeof(PTMissionReceiptLedger) == 16, "mission receipt player stride");
_Static_assert(sizeof(PTMissionExpenseLedger) == 40, "mission expense player stride");
static inline PTMissionReceiptLedger *pt_mission_receipts(u8 player) {
  if (player >= sizeof(mission_receipts) / sizeof(*mission_receipts))
    pt_resource_bounds_failure("player", player, sizeof(mission_receipts) / sizeof(*mission_receipts));
  return &mission_receipts[player];
}
static inline PTMissionExpenseLedger *pt_mission_expenses(u8 player) {
  if (player >= sizeof(mission_expenses) / sizeof(*mission_expenses))
    pt_resource_bounds_failure("player", player, sizeof(mission_expenses) / sizeof(*mission_expenses));
  return &mission_expenses[player];
}
// Current-month summary tables use the same four receipt and ten expense
// categories as the cumulative mission ledgers, with independent storage.

const PTMissionReceiptLedger *pt_monthly_receipts(u8 player);
const PTMissionExpenseLedger *pt_monthly_expenses(u8 player);

// Decode an optional depot without reading it. Empty-ingredient and zero-add
// paths preserve the original lazy access even for unread opaque encodings.

static inline Depot *pt_optional_city_depot(u8 player, u8 town) {
  return pt_player_depot(&players[player], town);
}

// Character and player records are ordinary bounded native arrays.

static inline Character *pt_character_record_slot(u8 slot) {
  if (slot >= sizeof(characters) / sizeof(*characters))
    pt_resource_bounds_failure("character", slot, sizeof(characters) / sizeof(*characters));
  return &characters[slot];
}
static inline u8 *pt_character_record_index(u8 slot) {
  if (slot >= sizeof(players) / sizeof(*players))
    pt_resource_bounds_failure("player", slot, sizeof(players) / sizeof(*players));
  return &players[slot].character_index;
}
// A native player owns its complete name buffer.

_Static_assert(offsetof(Rest, linked) == 0, "restaurant linked prefix");

// Native records and table indices have explicit bounds.

_Static_assert(offsetof(Poi, size_y) == offsetof(Poi, size_x) + 1, "property dimension offsets");

// The original calorie reader owns one byte, including wrapped serialized identities.

static inline PTListResult pt_insert_recipe_sorted(Player *player, Recipe *recipe) {
  return LinkList_InsertSorted(&player->recipes, &recipe->linked, PT_LIST_RECIPE_ID);
}
const RecipeEntry *pt_recipe_entry(const Recipe *recipe, unsigned index);

// Sixty history values share a 61-byte append-and-shift buffer.

static inline u8 *pt_economy_history_entry(u8 index) {
  if (index >= sizeof(ramtown.economy_history_samples))
    pt_resource_bounds_failure("economy history", index, sizeof(ramtown.economy_history_samples));
  return &ramtown.economy_history_samples[index];
}
// The final sample is the append scratch byte used before shifting.

static inline u8 *pt_economy_history_window(u8 index, size_t count) {
  if (index > sizeof(ramtown.economy_history_samples) || count > sizeof(ramtown.economy_history_samples) - index)
    pt_resource_bounds_failure("economy history window", index, sizeof(ramtown.economy_history_samples));
  return &ramtown.economy_history_samples[index];
}
static inline i8 pt_economy_history_value(u8 index) {
  const u8 *field = pt_economy_history_entry(index);
  return (i8)((*field));
}
// The end node belongs to a native head. Recover that head through its
// named member offset when advancing past the last record.

_Static_assert(offsetof(Zone, link) == 0, "district linked prefix");
static inline PTForwardLink *pt_list_wrapped_first(const PTForwardLink *end) {
  const LinkedHead *head = (const LinkedHead *)((const u8 *)end - offsetof(LinkedHead, end));
  return head->begin.next;
}
static inline u8 *pt_delayed_message_text(u8 slot) {
  if (slot >= sizeof(delayed_msg_contents) / sizeof(*delayed_msg_contents))
    pt_resource_bounds_failure("delayed message", slot, sizeof(delayed_msg_contents) / sizeof(*delayed_msg_contents));
  return delayed_msg_contents[slot];
}

// Recipe variation selects one of the ingredient's three defined amounts.

u8 pt_ingredient_amount(u8 ingredient, u8 variation);
typedef PTRecipeTastes PTRecipeTasteView;

static inline RecipeEntry *pt_recipe_mutable_entry(Recipe *recipe, unsigned index) {
  return (RecipeEntry *)(void *)pt_recipe_entry(recipe, index);
}
// Default-name identities retain the full original BYTE index domain.

static inline const u8 *pt_default_recipe_name(u8 index) {
  if (index >= sizeof(default_recipe_names) / sizeof(*default_recipe_names))
    pt_resource_bounds_failure(
        "default recipe name", index, sizeof(default_recipe_names) / sizeof(*default_recipe_names));
  return default_recipe_names[index];
}
// Weights retain the original unsigned WORD interpretation.

u16 pt_recipe_weight(const Recipe *recipe, unsigned ingredient);

// A row identity is distinct from the recipe pointer stored in that row.

static inline const RestoRecipe *pt_menu_recipe_row(const Rest *rest, unsigned index) {
  if (index >= sizeof(rest->recipes) / sizeof(*rest->recipes) + 1u)
    pt_resource_bounds_failure("menu row", index, sizeof(rest->recipes) / sizeof(*rest->recipes) + 1u);
  return &rest->recipes[index];
}
// The last node is a four-byte marker. Inspect its link before constructing
// a full typed record; an empty list never reads a Recipe/Rest at its marker.

static inline Recipe *pt_recipe_node(const PTForwardLink *node) {
  return node->next ? (Recipe *)(void *)node : NULL;
}
static inline Recipe *pt_player_first_recipe(const Player *player) {
  return pt_recipe_node(player->recipes.begin.next);
}
static inline Recipe *pt_recipe_next(const Recipe *recipe) {
  return pt_recipe_node(recipe->linked.next);
}
static inline Rest *pt_restaurant_node(const PTForwardLink *node) {
  return node->next ? (Rest *)(void *)node : NULL;
}
static inline Rest *pt_player_first_restaurant(const Player *player) {
  return pt_restaurant_node(player->restaurants.begin.next);
}
static inline Rest *pt_restaurant_next(const Rest *rest) {
  return pt_restaurant_node(rest->linked.next);
}
static inline PTRGB *pt_palette_color_at(const Palette *palette, i16 index) {
  if ((u64)(index) >= (256))
    pt_resource_bounds_failure("palette color", index, 256);
  return &palette->colors[index];
}
// The legacy palette query keeps the full wrapping DWORD index.

static inline const PTRGB *pt_palette_bank(i32 index) {
  if ((u64)(index) >= sizeof(palette_e) / sizeof(*palette_e))
    pt_resource_bounds_failure("palette bank", index, sizeof(palette_e) / sizeof(*palette_e));
  return palette_e[index].colors;
}
static inline const u8 *pt_map_tab_label(u8 row) {
  if (row >= sizeof(map_tab_labels) / sizeof(*map_tab_labels))
    pt_resource_bounds_failure("map tab", row, sizeof(map_tab_labels) / sizeof(*map_tab_labels));
  return map_tab_labels[row];
}

static inline Staff *pt_staff_record(u8 index) {
  if (index >= sizeof(g_staff) / sizeof(g_staff[0]))
    pt_resource_bounds_failure("staff record", index, sizeof(g_staff) / sizeof(g_staff[0]));
  return &g_staff[index];
}
_Static_assert(
    sizeof(((Staff *)0)->traits) == sizeof(((Character *)0)->staff_traits), "staff and character trait extents");

static inline const Furniture *pt_rest_furniture_identity(const Rest *rest, u8 index, bool kitchen) {
  if (index >=
      (kitchen ? sizeof(rest->ovens) / sizeof(*rest->ovens) : sizeof(rest->furniture) / sizeof(*rest->furniture)))
    pt_resource_bounds_failure("restaurant furniture",
        index,
        kitchen ? sizeof(rest->ovens) / sizeof(*rest->ovens) : sizeof(rest->furniture) / sizeof(*rest->furniture));
  return kitchen ? &rest->ovens[index] : &rest->furniture[index];
}
const BobHeader *pt_bob_signed_header(const BobHost *bob, u16 image);
// Stock searches all 256 BYTE indices, including physical storage after
// the declared 150 actors. Keep this one-byte extent at the layout boundary.

const u8 *pt_map_igloo_availability(unsigned index);

Guest *pt_guest_at(unsigned index);

_Static_assert(offsetof(Guest, flags_low) == offsetof(Furniture, flags_low) &&
        offsetof(Guest, linked_furniture) == offsetof(Furniture, linked_furniture),
    "guest furniture prefix");
static inline u8 *pt_pig_path_direction(Pig *pig) {
  if (pig->path_depth < sizeof(pig->path_directions))
    return &pig->path_directions[pig->path_depth];
  // The original ninth direction overwrites the first step count.

  if (pig->path_depth == sizeof(pig->path_directions))
    return &pig->path_steps[0];
  pt_resource_bounds_failure("walking direction history", pig->path_depth, sizeof(pig->path_directions) + 1);
}
static inline const u8 *pt_pig_previous_direction(const Pig *pig) {
  if (!pig->path_depth)
    return &pig->byteF;
  if (pig->path_depth <= sizeof(pig->path_directions))
    return &pig->path_directions[pig->path_depth - 1u];
  pt_resource_bounds_failure("previous walking direction", pig->path_depth, sizeof(pig->path_directions) + 1);
}
// Callers validate byte-sized path coordinates before accessing the floor.

u8 pt_rest_floor_cell(u8 row, u8 column);
// Manager settings are adjacent independent BYTE fields in the serialized record.

static inline u8 *pt_rest_manager_toggle(Rest *rest, u8 row) {
  if (row >= sizeof(rest->manager_authority))
    pt_resource_bounds_failure("manager authority", row, sizeof(rest->manager_authority));
  return &rest->manager_authority[row];
}
static inline u8 *pt_rest_manager_reaction(Rest *rest, u8 row) {
  if (row >= sizeof(rest->manager_reactions))
    pt_resource_bounds_failure("manager reaction", row, sizeof(rest->manager_reactions));
  return &rest->manager_reactions[row];
}
// Six phonebook categories each own ten entries.

static inline u8 *pt_terminal_dial_digit(u8 position) {
  if (position >= sizeof(dial_number_buffer))
    pt_resource_bounds_failure("dial digit", position, sizeof(dial_number_buffer));
  return &dial_number_buffer[position];
}
static inline u8 *pt_player_phone_history(Player *player, u8 kind, u8 slot) {
  if (kind >= (6))
    pt_resource_bounds_failure("phonebook category", kind, 6);
  if (slot >= (10))
    pt_resource_bounds_failure("phone history", slot, 10);
  return &player->city_part.last_dialled_number[kind][slot];
}
static inline u32 pt_town_phone_number(const Town *town, u8 kind, u8 index) {
  if (kind >= (6))
    pt_resource_bounds_failure("phonebook category", kind, 6);
  if (index >= (10))
    pt_resource_bounds_failure("phone number", index, 10);
  return (u32)town->phone_numbers[kind][index];
}
static inline const u8 *pt_town_phone_connection(u8 kind, u8 index) {
  if (kind >= (6))
    pt_resource_bounds_failure("phonebook category", kind, 6);
  if (index >= (10))
    pt_resource_bounds_failure("phone connection", index, 10);
  return &ramtown.number_has_no_connection[kind][index];
}
static inline u8 *pt_pig_path_steps(Pig *pig) {
  if (pig->path_depth < sizeof(pig->path_steps))
    return &pig->path_steps[pig->path_depth];
  // Stock clears the depth when the ninth step counter is initialized.
  // Preserve this alias without indexing beyond the eight owned counters.

  if (pig->path_depth == sizeof(pig->path_steps))
    return &pig->path_depth;
  pt_resource_bounds_failure("walking step history", pig->path_depth, sizeof(pig->path_steps) + 1);
}

// The original town byte addresses 256 DWORD slots starting at the weekly
// profits, all inside Player storage. The normal ten-town array is retained;
// this view also defines the original physical spill into later fields.

// Phonebook captions are stored in six tables of ten 24-byte names.

static inline const u8 *pt_terminal_caption(const Town *town, u8 kind, u8 index) {
  if (kind >= (6))
    pt_resource_bounds_failure("phonebook category", kind, 6);
  if (index >= (10))
    pt_resource_bounds_failure("phonebook entry", index, 10);
  return town->phonebook_names[kind][index];
}
// Each manager role has one complete eight-byte label.

typedef struct __attribute__((packed, may_alias)) PTManagerRoleText {
  u8 text[8];
} PTManagerRoleText;
_Static_assert(sizeof(manager_role_names[0]) == 8, "manager role label stride");
_Static_assert(sizeof(PTManagerRoleText) == sizeof(manager_role_names[0]), "manager role text window");
const PTManagerRoleText *pt_manager_role_text(u8 role);

// Newspaper substitutions can cross a physical 50-byte slot. These named
// windows retain whole-arena byte provenance for the original copies.

static inline u8 *pt_advert_line_window(u8 slot) {
  if (slot >= sizeof(advert_newspaper_lines) / sizeof(*advert_newspaper_lines))
    pt_resource_bounds_failure(
        "newspaper line", slot, sizeof(advert_newspaper_lines) / sizeof(*advert_newspaper_lines));
  return advert_newspaper_lines[slot];
}
static inline const Campaign *pt_campaign_object(u8 player, i8 category) {
  if (player >= (8))
    pt_resource_bounds_failure("player", player, 8);
  if ((u64)(category) >= (4))
    pt_resource_bounds_failure("advertising category", category, 4);
  return &ramtown.campaigns[player][category];
}
u8 pt_furniture_beauty(const FurnitureDesc *descriptor, u8 age);
u8 pt_interactive_player_count(void);

u16 pt_town_stock_price(unsigned set, unsigned ingredient);
static inline RestoRecipe *pt_menu_recipe_mutable(Rest *rest, unsigned index) {
  if (index >= sizeof(rest->recipes) / sizeof(*rest->recipes))
    pt_resource_bounds_failure("menu row", index, sizeof(rest->recipes) / sizeof(*rest->recipes));
  return &rest->recipes[index];
}

// Native records and table indices have explicit bounds.

static inline u8 *pt_rest_staff_slot(Rest *rest, u8 slot) {
  if (slot >= sizeof(rest->staff_indexes))
    pt_resource_bounds_failure("restaurant staff slot", slot, sizeof(rest->staff_indexes));
  return &rest->staff_indexes[slot];
}
Staff *pt_rest_staff_member(const Rest *rest, u32 slot);
static inline Furniture *pt_rest_furniture_view(Rest *rest, u8 index, bool kitchen) {
  if (index >=
      (kitchen ? sizeof(rest->ovens) / sizeof(*rest->ovens) : sizeof(rest->furniture) / sizeof(*rest->furniture)))
    pt_resource_bounds_failure("restaurant furniture",
        index,
        kitchen ? sizeof(rest->ovens) / sizeof(*rest->ovens) : sizeof(rest->furniture) / sizeof(*rest->furniture));
  return kitchen ? &rest->ovens[index] : &rest->furniture[index];
}
// A wrapped byte cursor can carry into the next player's goal record.
// Preserve that original scan explicitly without crossing a native array.

static inline PTPlayerGoals *pt_player_goals(u8 player) {
  if (player >= 8)
    pt_resource_bounds_failure("goal player", player, 8);
  return &player_goals[player];
}
static inline PTMissionGoals *pt_goal_history_at(u8 player, u8 slot) {
  if (player >= 8)
    pt_resource_bounds_failure("goal player", player, 8);
  unsigned row = player + slot / 100u;
  return row < 8 ? &player_goals[row].history : &player_goals[row - 8u].targets;
}
static inline PTMissionGoals *pt_goal_target_at(u8 player, u8 slot) {
  if (player >= 8)
    pt_resource_bounds_failure("goal player", player, 8);
  unsigned row = player + slot / 100u;
  if (row >= 8)
    pt_resource_bounds_failure("goal target row", row + 8u, 16);
  return &player_goals[row].targets;
}
static inline u8 pt_goal_bitmap(u8 goal) {
  if (goal >= sizeof(bitmap_to_show_for_goal))
    pt_resource_bounds_failure("goal bitmap", goal, sizeof(bitmap_to_show_for_goal));
  return bitmap_to_show_for_goal[goal];
}
_Static_assert(sizeof(PTGoalSceneStyle) == sizeof(goal_bitmap_flags[0]), "goal scene stride");
const PTGoalSceneStyle *pt_goal_scene_style(u8 bitmap);
static inline u8 pt_goal_last_target(u8 player) {
  const PTMissionGoals *targets = &pt_player_goals(player)->targets;
  u8 slot = 0;
  while (slot < sizeof(targets->year) && targets->year[slot])
    ++slot;
  return (u8)(slot - 1u);
}
u8 *pt_next_language_text(u8 *text);
u8 *pt_character_property(Character *character, u16 index);
static inline u32 pt_goal_points(u8 kind) {
  // Original sentinel/ending aliases, expressed as their actual fields.

  if (!kind)
    return (u32)goal_ui_colors[0] | (u32)goal_ui_colors[1] << 8 | (u32)goal_ui_colors[2] << 16 |
        (u32)goal_ui_colors[3] << 24;
  if (kind == 140)
    return loaded_awards_bitmap;
  if (kind > sizeof(curriculum_scores) / sizeof(curriculum_scores[0]))
    pt_resource_bounds_failure("goal score", kind, 140);
  return curriculum_scores[kind - 1u];
}
static inline u8 *pt_pending_goal(u8 player) {
  if (player >= sizeof(players) / sizeof(*players))
    pt_resource_bounds_failure("player", player, sizeof(players) / sizeof(*players));
  return &players[player].index_of_next_goal;
}
typedef struct __attribute__((packed, may_alias)) PTBobFileHeader {
  u16 num_entries;
  u32 opaque_value;
} PTBobFileHeader;
_Static_assert(sizeof(PTBobFileHeader) == 6, "sprite file prefix size");
_Static_assert(offsetof(PTBobFileHeader, opaque_value) == 2, "sprite file opaque DWORD");
static inline PTBobFileHeader *pt_bob_file_header(const BobHost *bob) {
  return (PTBobFileHeader *)(void *)bob->data;
}

// Prefix views retain the original access checks without requiring whole objects.

// Supplier list sentinels expose only the next link; command entry proves payloads.

// Depot quantities physically overlay the six customer words and final DWORD.

static inline u16 *pt_depot_weapon_quantity(Poi *poi, u8 weapon) {
  if (weapon >= 8)
    pt_resource_bounds_failure("depot weapon quantities", weapon, 8);
  return &poi->weapon_quantity[weapon];
}

// Resource descriptor identities defer validation to their first consumer.

// Decode object identities without reading them; consuming diagnostic roots
// retain their original ordered ownership checks.

// Queue slots and field-sized links retain the stock physical domains. The
// may_alias DWORD view forces each POI link load to follow preceding byte stores.

static inline Peach *pt_player_phone_entry(u8 slot) {
  if (slot >= (4))
    pt_resource_bounds_failure("pending phone call", slot, 4);
  return &player_ptr->peaches[slot];
}
static inline Poi *pt_property_node(const PTForwardLink *node) {
  return node->next ? (Poi *)(void *)node : NULL;
}
static inline Poi *pt_town_first_property(void) {
  return pt_property_node(current_town->poi.begin.next);
}
static inline Poi *pt_property_next(const Poi *poi) {
  return pt_property_node(poi->link.next);
}
// Physical ingredient rows stay contiguous across their four named sets.

const Ingredient *pt_ingredient_record(u32 index);
// Optional owned recipe results and UI arguments use a native null pointer.

u8 pt_player_restaurant_count(u32 index);

// Partial object consumers retain their field-sized original ownership.

_Static_assert(offsetof(Poi, ycoord) == offsetof(Poi, xcoord) + 1u, "property coordinate pair");

// Current, target and notice tables each own 96 ingredient trend values.

static inline u8 *pt_town_current_trend(u8 index) {
  if (index >= sizeof(current_town->ingredient_trend))
    pt_resource_bounds_failure("ingredient trend", index, sizeof(current_town->ingredient_trend));
  return (u8 *)&current_town->ingredient_trend[index];
}
static inline u8 *pt_town_target_trend(u8 index) {
  if (index >= sizeof(current_town->stockarr90A))
    pt_resource_bounds_failure("ingredient trend", index, sizeof(current_town->stockarr90A));
  return (u8 *)&current_town->stockarr90A[index];
}
static inline u8 *pt_town_trend_notice(u8 index) {
  if (index >= sizeof(ramtown.stockarr56))
    pt_resource_bounds_failure("ingredient trend", index, sizeof(ramtown.stockarr56));
  return (u8 *)&ramtown.stockarr56[index];
}
u8 pt_customer_population_factor(u8 group, u8 age);
const u8 *pt_player_announcement_name(u8 player, size_t capacity);
bool pt_music_is_active(void);

// Signed byte pattern counters can select the two fields preceding piece.
// Name those original reads explicitly instead of indexing outside the array.

static inline u8 pt_floor_pattern_piece(const Floor *floor, int index) {
  if (index == -2)
    return floor->price;
  if (index == -1)
    return floor->floor_bg;
  if ((unsigned)index >= 4u)
    abort();
  return floor->piece[(unsigned)index / 2u][(unsigned)index % 2u];
}

// Object relationships return native pointers, with lazy payload ownership.

static inline const Recipe *pt_menu_recipe(const Rest *restaurant, unsigned index) {
  return pt_menu_recipe_row(restaurant, index)->recipe;
}
const u8 *pt_town_name(u8 index);
const u8 *pt_finance_category_name(u8 index);
const u8 *pt_insurance_category_name(u8 index);
const u8 *pt_terminal_category_name(u8 index);

// Sprite motion.

// validation. These helpers never create a second game-state implementation.

typedef struct ScrollSpawn {
  u8 first_column, last_column, first_row, last_row;
  i16 zone_column, zone_row;
  bool reset;
} ScrollSpawn;

typedef struct IglooTurn {
  u8 column, row, spawn_column, spawn_row, direction, shortening;
  bool wrap, random, changes;
} IglooTurn;

bool pt_sprite_tile_direction(u8 tile, u8 *direction);
u8 pt_sprite_actor_tile(u8 column, u8 row);
u8 pt_sprite_map_igloo_unused(unsigned index);
bool pt_sprite_shift_igloo(Igloo *igloo, u8 direction, u8 distance);
ScrollSpawn pt_sprite_scroll_spawn(u8 direction, u8 distance);
bool pt_sprite_blocks_movement(const Igloo *a, const Igloo *b);
IglooTurn pt_sprite_advance_tile(const Igloo *igloo);
bool pt_sprite_junction_tile(u8 tile);
bool pt_sprite_choose_turn(u8 tile, bool branch, u8 *direction);
void pt_sprite_finish_turn_plan(IglooTurn *turn, u8 old_direction);

// Restaurant flow.

// Native state calculations use host services for messages, goals and money.
// A copied state can run the same calculations with different services.

typedef struct PTStockDecayEffects {
  u32 (*draw)(void *context);
  unsigned (*notice)(void *context, const Rest *rest, const u8 *text, size_t length);
  unsigned (*charge)(void *context, float amount);
} PTStockDecayEffects;
typedef struct PTStockDecayState {
  Rest *destination;
  Rest view;
  const PTStockDecayEffects *effects;
  void *context;
  unsigned status;
} PTStockDecayState;
typedef struct PTSupplyEffects {
  unsigned (*message)(void *context, const Rest *rest, u8 *text);
  unsigned (*goal)(void *context, u8 kind);
  unsigned (*charge)(void *context, float amount);
} PTSupplyEffects;
typedef struct PTSupplyState {
  Rest *destination;
  Rest view;
  const PTSupplyEffects *effects;
  void *context;
  unsigned status;
} PTSupplyState;
void pt_run_stock_decay(PTStockDecayState *state);
float pt_run_restaurant_supplies(PTSupplyState *state);
void pt_restaurant_sales_totals(const Rest *rest, float *demand, float *sales, float *expenses, u16 *orders, u16 *sold);

// Paragraph policy.

static inline u32 pt_paragraph_left_edge(const Game *state, u32 first, u32 subsequent, u16 y, u16 offset) {
  return state->paragraph_indent_y_threshold
      ? (state->paragraph_indent_y_threshold <= (i16)y ? subsequent : first)
      : (state->paragraph_first_indent_height <= (i16)offset ? subsequent : first);
}

#endif
