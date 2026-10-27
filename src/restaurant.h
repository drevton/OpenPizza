// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
#ifndef PT_RESTAURANT_H
#define PT_RESTAURANT_H
#include "layout.h"
#include "state_types.h"
#include "graphics.h"

// Restaurant schedule.

// restaurant day

void RunRestaurantDay(Rest *restaurant);
void DummyProc(Rest *restaurant);

// restaurant week

void RunRestaurantWeek(Rest *restaurant);
u8 DummyProc2(Rest *restaurant);

// restaurant billing

u8 RunRestaurantMonthInner(Rest *restaurant);
u8 RunRestaurantMonth(Rest *restaurant);

// staff expenses

void ShowStaffExpenses(Rest *restaurant);

// Restaurant attack.

void HandleUserAttackRestaurant(Rest *restaurant);
u8 CrashFurniture(Furniture *furniture);
void CrashWholeRestaurant(Rest *restaurant);
i32 ChangeRestaurantByte2C(Rest *restaurant, i8 delta);
void HandleRestaurantAttackedStink(Rest *restaurant, u8 kind);
void HandleRestaurantAttackedWeapon(Rest *restaurant, u8 kind);

// Restaurant stats.

void ShowMonthReviewStats(Rest *rest);
void ShowWeeklyOverviewStats(Rest *rest);
void ShowCustomerStats(Rest *rest);
void ShowSalesCurve(Rest *rest);
void ShowSalesCurve_Helper(u16 image, i16 bottom, i32 height);
i32 GetDayMonthHistoryString(u8 index);
PTActionResult HandleRestaurantStatsMenu(Rest *rest);

// Restaurant.

#include <stdbool.h>

void RestRunDailyLoop(Rest *restaurant);
u8 RestDoManagerDuties(Rest *restaurant);
u16 pt_add_restaurant_stock(Rest *rest, unsigned index, u16 amount, u8 quality, u16 value);
unsigned pt_add_depot_stock(Depot *depot, unsigned index, u16 amount, u8 quality, u16 value);
void RestHandleManagerShopping(Rest *rest, const Staff *manager);
u16 GetPriceForIngredient(const Supplier *supplier, u8 item);
i32 MgrAdjustPizzaPrice(RestoRecipe *recipe, u8 skill, u8 policy);
void RestHandleTurnoverChange(Rest *rest, u8 skill);
void RunContractsDailyLoop(Contract *contract);
Rest *FindRestaurantById(u8 restaurant_id);
void UpdateCityDepotNeedValues(u8 player_index);
u16 DepotExport_RemoveOld(u8 player, u8 town, u8 set, u8 item, u16 amount);
void DepotExport_AddNew(u8 player, u8 town, u8 set, u8 item, u8 quality, u16 amount, u16 value);
u16 AddStockToRestaurantInventory(Rest *rest, u8 set, u8 item, u16 amount, u8 quality, u16 value);
void CopyFromDepotToRest(Rest *rest, u8 set, u8 item, u16 amount);
bool pt_select_van_item(u32 start, u8 direction, u32 *result);
u32 RepositionFurnitureHelper3(u32 start, u8 direction);
u32 RePositionFurniture(Rest *restaurant, u8 orientation, u8 item);
void RestLoadMusic(Rest *restaurant, u8 transition);
Rest *SelectNextRestaurant(Rest *restaurant);
bool IsPointInFurniture(const Furniture *furniture, i16 x, i16 y);
u8 FindFurnitureAtPoint(Rest *restaurant, i16 x, i16 y);
u8 FindGuestForFurniture(u8 index);
u16 FindRestaurantObject(Rest *restaurant);
u16 GetPriceOfFurniture(u16 type);
void ShowFurnitureInfo(Rest *restaurant, const Furniture *furniture);
void PrintOvenStatus(Rest *restaurant, const Furniture *oven);
void ReClearEmptyStaffSlots(Rest *restaurant);
u32 RestComputePizzaCapacity(Rest *restaurant);
u16 ReComputeAverageTimes(Rest *restaurant);
u8 ReComputeStaffStuff(Rest *restaurant);
u32 RoundUpAndDivideBy100(u32 value);
u32 DeductFromStock(Rest *restaurant, u8 set, u8 item, u16 requested);
void UpdateAllCostsForPlayer(u8 player);
void AddCostForPlayer(u8 player, u32 amount, u8 category);
void AddCostForRestaurant(Rest *restaurant, u32 amount, u8 category);
void AddCostForRestaurantInner(Rest *restaurant, u32 amount, u8 category);
u32 ComputeSlotMachineIncome(Rest *restaurant);
i32 *RestRunNewMonth(Rest *restaurant);
void UpdateMoneyFromMissions(u8 player, u32 money, u8 category);
u8 AddIncomeForRestaurantInner(Rest *restaurant, u32 money, u8 category);
float RestCheckIfEnoughSupplies(Rest *restaurant);
u32 RestUpdateSupplyStats(Rest *restaurant);
void RestComputeRecipeQualities(Rest *restaurant);
u16 IsValidIngredient2(u8 index);
i8 RecipeComputeTrend(Recipe *recipe);
u8 RestComputePizzaRating(const Rest *restaurant, const RestoRecipe *entry, u8 age);
u8 RestComputeNumOrders(Rest *restaurant);
u8 LemonComputeValue(const Staff *staff);
void StaffAddMotivation(Staff *staff, u32 delta);
u16 ComputeWantedStaffSalary(const Staff *staff);
void RestRunStaffDaily(Rest *restaurant);
void RestComputeAveragePizzaPrice(Rest *restaurant);
void RestCheckStockRotten(Rest *restaurant);
u16 ComputeStockDaysLeft(const RestaurantStock *stock);
void RestRunNewWeek(Rest *restaurant);
u8 ReIsValidStaffIndex(Rest *restaurant, u8 slot);
u32 ComputeRestaurantSalariesSum(Rest *restaurant);
void RestUpdateByte29(Rest *restaurant, i8 amount);
void RestCheckIncreaseRange(Rest *restaurant);
void RestCheckIncreaseRange2(Rest *restaurant);
u8 RestaurantProc4(Rest *restaurant);
i32 RestaurantScaledCloseness(const Rest *restaurant, u32 popularity, u32 distance);
void RestaurantProc2(Rest *restaurant);
u32 RestaurantProc3(Rest *restaurant);
void DoOpenRestaurant(Rest *restaurant);
void DoCloseRestaurant(Rest *restaurant);
void RestHelper5(Rest *restaurant);
void AddRestaurantRangeStuff(Rest *restaurant, u8 value, u8 duration);
void RestSetReputationEntry(Rest *restaurant, u8 field, u8 value, u8 show_stats);
void RestUpdateReputations(Rest *restaurant);
void RestComputeCostsPerPizza(Rest *restaurant);
u8 RestComputeBeautiness(Rest *restaurant, u8 age);
u32 PlayerSumRamtownFields(u8 player);
u32 ComputeRecipeCost(const Recipe *recipe, const Rest *restaurant);
void ResetFurnitureContacts(void);
void RecordFurnitureContact(const Furniture *furniture, u8 index, u8 amount);
u32 FurnitureContactAmount(
    const Rest *restaurant, const Igloo *preview, const BobHeader *header, const Furniture *furniture);
u8 FindPlacementContacts(Rest *restaurant, const BobHeader *header, u8 layer);
void DrawRestaurant_SetFlag4OnAll(Rest *restaurant);
void DrawRestaurant_ResetIgloos(void);
void SetupCanvasTempIglooLink(void);
void DrawAllGuests(void);
i16 DrawAllGuests_Helper(Igloo *igloo, Guest *guest);
u8 GuestTurnCounterclockwise(u8 direction);
void DrawRestaurant_Proc8(Rest *restaurant, u8 layer, u8 inverse);
void DrawBob(BobHost *bob, u16 image, i16 x, i16 y);
// Loaded room drawing is reconstructed; new daytime guest generation stops
// at its original callee.

void DrawRestaurant(Rest *restaurant, u8 redraw);
void LoadFloorBitmaps(u8 index);
u32 DrawFloorPiece(u8 tile, u8 variant, i8 x, i8 y);
u32 DrawRestaurant_DrawWallCell(u8 tile, i8 x, i8 y);
u32 DrawRestaurant_SingleRow(Rest *restaurant, i8 row);
void DrawRestaurant_SingleCol(Rest *restaurant, i8 column);
void RefreshOverflow(Rest *restaurant);
void FurnitureSetFlag4(Rest *restaurant, Furniture *furniture);
void SetGuestsAndFurnitureFlag4(Rest *restaurant, i16 left, i16 top, i16 right, i16 bottom);
void RestDrawOnScreen(Rest *restaurant);
i32 RestScroll(Rest *restaurant, u8 direction);
Poi *DrawRestaurant_Floor(Rest *restaurant);
void DrawRestaurant_TopWall(Rest *restaurant);
void DrawRestaurant_RightWall(Rest *restaurant);
void DrawRestaurant_LeftWall(Rest *restaurant);
void DrawRestaurant_BottomWall(Rest *restaurant);
void RestDrawKitchen_HorizWall(i8 y, i8 start, i8 end);
void RestDrawKitchen_Background(Rest *restaurant);
// Full kitchen background and oven sprites.

void RestDrawKitchen(Rest *restaurant);
// Retained room input loop; unported gameplay callbacks stop explicitly.

u8 EnterRestaurant(Rest *restaurant, u8 competitor);
u8 EnterCompetitorRestaurant(Rest *restaurant);

void pt_floor_filename(u8 index, u8 name[13]);
u8 pt_floor_row_tile(const Rest *r, unsigned base, i8 pattern);

// Restaurant scroll.

bool pt_rest_scroll_move(Rest *, u8);
PTBlitRect pt_rest_scroll_shift(u8);
bool pt_rest_scroll_bounds(const Rest *, u8, i16[4]);

// Restaurant simulation.

typedef struct PTStaffDay PTStaffDay;
typedef struct PTStaffDayServices {
  u32 (*random_number)(PTStaffDay *, u32);
  u8 (*chance)(PTStaffDay *, i32);
  void (*motivate)(PTStaffDay *, unsigned, int);
  void (*training_goal)(PTStaffDay *, unsigned);
  void (*training_message)(PTStaffDay *, Staff *, unsigned, const u8 *, size_t);
  void (*money_loss)(PTStaffDay *, u16);
  u16 (*salary)(PTStaffDay *, const Staff *, unsigned);
  unsigned (*cash)(PTStaffDay *, double *);
  void (*staff_changed)(PTStaffDay *, Staff *, unsigned);
  void (*finished)(PTStaffDay *);
} PTStaffDayServices;
struct PTStaffDay {
  Rest *restaurant;
  Rest view;
  Staff staff[200];
  Apple apple;
  u32 rng;
  unsigned status;
  const PTStaffDayServices *services;
  void *service_context;
};
void pt_staff_day_run(PTStaffDay *day);

typedef struct PTManagerDuties PTManagerDuties;
typedef struct PTManagerDutiesServices {
  u8 (*chance)(PTManagerDuties *, unsigned);
  void (*motivate)(PTManagerDuties *, unsigned, int);
  void (*message)(PTManagerDuties *, const u8 *, size_t);
  unsigned (*restaurant_name)(PTManagerDuties *, u8[37], size_t *);
  const u8 *(*role_text)(PTManagerDuties *, unsigned);
  void (*shopping)(PTManagerDuties *, unsigned);
  void (*turnover)(PTManagerDuties *, u8);
  bool (*valid_slot)(PTManagerDuties *, unsigned);
  void (*staff_changed)(PTManagerDuties *, Staff *, unsigned);
  void (*finished)(PTManagerDuties *);
} PTManagerDutiesServices;
struct PTManagerDuties {
  Rest *restaurant;
  Rest rest;
  Staff staff[200];
  u32 rng;
  unsigned status;
  const PTManagerDutiesServices *services;
  void *service_context;
};
void pt_manager_duties_run(PTManagerDuties *day);

typedef struct PTManagerShopping PTManagerShopping;
typedef struct PTManagerShoppingServices {
  Supplier *(*resolve_supplier)(PTManagerShopping *, unsigned);
  u32 (*random_number)(PTManagerShopping *, u32);
  u8 (*chance)(PTManagerShopping *, u8);
  u32 (*percent)(PTManagerShopping *, u32, int);
  bool (*pay)(PTManagerShopping *, u32);
  void (*message)(PTManagerShopping *, const u8 *, size_t, unsigned, bool);
  u16 (*days_left)(PTManagerShopping *, unsigned);
  u32 (*price)(PTManagerShopping *, const Supplier *, unsigned);
  void (*purchase)(PTManagerShopping *, Supplier *, u32, u32);
  void (*weekly_cost)(PTManagerShopping *, unsigned);
  void (*publish_name)(PTManagerShopping *);
  void (*inventory)(PTManagerShopping *, unsigned, unsigned, u16, u8, u16);
  void (*refresh)(PTManagerShopping *);
  void (*begin)(PTManagerShopping *);
  void (*finished)(PTManagerShopping *);
} PTManagerShoppingServices;
struct PTManagerShopping {
  Rest *restaurant;
  Rest rest;
  Staff manager;
  Supplier *suppliers[1024];
  unsigned count, status;
  u32 rng;
  const PTManagerShoppingServices *services;
  void *service_context;
};
void pt_manager_shopping_run(PTManagerShopping *shopping);

// Restaurant media.

void DrawGuestEatingFrame_Helper3(i16 left, i16 top, i16 right, i16 bottom);
u8 ChooseItemFromTvOrJukebox(u8 kind, u8 selected);
u8 ChooseFromJukebox(Rest *rest);
u8 ChooseFromTV(Rest *rest);

// Furniture info.

enum PTFurnitureInfoFormat {
  OVEN_CAPACITY,
  OVEN_RATE,
  OVEN_MINUTES,
  OVEN_PRICE,
  OVEN_LINK,
  ITEM_NUMBER,
  FURNITURE_PRICE,
  TABLE_CAPACITY,
  TABLE_CHAIRS,
  CHAIR_LINK,
  ITEM_QUALITY
};
const u8 *pt_furniture_info_format(enum PTFurnitureInfoFormat format);
u16 pt_furniture_info_number_text(u8 text[256], enum PTFurnitureInfoFormat format, u32 bits);

// Restaurant overview.

Canvas *ShowRestaurantsOverviewEntry(const Rest *restaurant, i32 y);
u16 ShowRestaurantsOverview(void);

#endif
