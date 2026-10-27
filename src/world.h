// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
#ifndef PT_WORLD_H
#define PT_WORLD_H
#include "command.h"
#include "menu.h"
#include "state_types.h"
#include "game.h"
#include "graphics.h"
#include "sound.h"
#include "messages.h"
#include "types.h"
#include "layout.h"

// Map.

// map render

u32 CopyBitmapToBuffer(const Bitmap *source, u8 *buffer);
u32 CopyBufferToBitmap(const Bitmap *target, const u8 *buffer);
u32 CopyBmp6AToHere(const Bitmap *target);
u32 CopyFromBackgroundBitmap(const Bitmap *target, u16 x, u16 y, u16 right, u16 bottom);
u8 GetBezColor(u8 mode, u8 x, u8 y);
void DrawMapBegin(void);
void DrawMapEnd(void);
void DrawMapZoomLevel1(u8 mode);
void DrawMapZoomLevel2(u8 mode);
u32 HandleMapZoomLevel2_Helper1(const Bitmap *target, u8 x, u8 y, u8 first, u8 count);
u32 HandleMapZoomLevel2_Helper2(const Bitmap *target, u8 x, u8 y, u8 first, u8 count);
u32 DrawMapHelper1(const Bitmap *target, u8 x, u8 y, u8 first, u8 count);
u32 DrawMapHelper2(const Bitmap *target, u8 x, u8 y, u8 first, u8 count);
i16 GetMapTabX(u8 tab);
void DrawMapTab(i16 x, i16 y, u8 color, const u8 *text);
void DrawMapTabs(void);

// map ui

Game *InitGraphicsMode(void);
PTActionResult Do3x3ButtonStuff(u8 menu);

// map markers

u16 CheckPoiAttrEquals(const Poi *poi, u8 attribute, u8 value);
void DrawMapMarkers(void);

// map controls

u32 HandleClickOnMapTabs(u8 tab);
u32 HandleClickOnMapControls(void);
int GetClickedMapTab(void);

// map zoom

void WaitForRetrace_(void);
u8 HandleMapZoomLevel_Helper2(i16 x, i16 y, i16 width, i16 height);
u32 HandleMapZoomLevel_Helper(u32 x, u32 y);

// large map

void LoadBlitmapAndOffsets(u8 index);
void SoundStuff4(void);
void DrawMapRowZoomLevel3(const Bitmap *target, u8 x, u8 y, u16 destination_y, u32 source_y_offset, u16 height);
u32 DrawMapColumnZoomLevel3(const Bitmap *target, u8 x, u8 y, u8 destination_x, u32 source_x_offset, u16 width);

// cursor visibility

void HideMouseCursorInner(void);
void GraphicsRestoreCursor(void);
Game *HideMouseCursor_(void);
Game *GraphicsRestoreCursor2(void);

// Poi.

float HandleAgePub(float population, u8 age, u8 tile);
u32 UpdatePoiCustomerCount(Poi *poi);
u32 GetSizeOfPoi(const Poi *poi);
u8 *itoa_(i32 value);
u32 DrawFlyerOutput(const u8 *label, u32 value);
void *HandleMapZoomLevel3_Helper6(Poi *poi);
u8 HandleMapZoomLevel3_Helper5(u8 y, u8 x);
u8 HandleClickOnRestaurant(Rest *restaurant);
typedef struct PTCustomerCounts {
  u16 groups[6];
  u32 total, seed;
  u8 radius;
  Zone *zone;
} PTCustomerCounts;
enum {
  POI_SEPARATOR = 0,
  POI_SIZE = 17,
  POI_OFFER = 29,
  POI_GUESTS = 36,
  POI_CUSTOMERS = 44,
  POI_SIZE_PADDED = 65,
  POI_PRICE = 82,
  POI_RENT = 88
};

size_t pt_poi_integer_text(u8 text[12], u32 bits);
size_t pt_poi_size_text(u8 text[256], const Poi *poi, bool padded);
void pt_poi_count_customers(Poi *poi, PTCustomerCounts *plan, bool runtime);

// Terrain.

void DrawMapAnyZoom(u8 zoom_level, u8 map_x, u8 map_y);

// Town init.

i32 GetTownPriceLevelStuff(void);
void UpdateTownMarketShare(void);
u8 InitLastDialedNumbers(void);
u32 ComputePoiBaseSize(u32 base, u32 range);
void RandomizeAiRecipe(AiRecipe *recipe);
void InitAiRecipes(void);
void CreateDefaultAiRestaurants(void);
void InitializePoiOwner(Poi *poi);
Poi *AllocatePoi(Town *town);
void InitializePoi(Poi *poi, u8 x, u8 y);
u8 DrawCountryInfoBoxBg(i16 left, i16 top, i16 right, i16 bottom);
u32 DrawGameBitmap(i16 left, i16 top, i16 right, i16 bottom);
void InitializeTown(void);

// City event trends.

typedef struct EventTrend {
  u8 event, index, initial, target, mode;
} EventTrend;
static const EventTrend event_trends[] = {{1, 0, 0, 206, 2},
  {2, 6, 0, 156, 2},
  {3, 0, 0, 0, 0},
  {7, 6, 25, 20, 3},
  {26, 24, 226, 20, 3},
  {30, 13, 0, 156, 2},
  {31, 48, 0, 50, 1},
  {32, 58, 236, 30, 3},
  {33, 56, 0, 60, 2},
  {35, 59, 50, 30, 3},
  {36, 11, 236, 30, 3},
  {37, 10, 0, 40, 1},
  {38, 17, 231, 20, 3},
  {39, 3, 0, 70, 2},
  {40, 21, 0, 156, 2},
  {41, 37, 0, 65, 2},
  {42, 34, 0, 176, 2},
  {43, 39, 0, 196, 2},
  {44, 38, 0, 166, 2},
  {45, 27, 0, 50, 1},
  {46, 72, 240, 226, 1},
  {47, 83, 20, 40, 1},
  {48, 76, 0, 40, 1}};
static inline const EventTrend *trend_for(u8 event) {
  for (unsigned i = 0; i < sizeof(event_trends) / sizeof(*event_trends); ++i)
    if (event_trends[i].event == event)
      return &event_trends[i];
  return NULL;
}

// Town assets.

#include <stdbool.h>
#include <stddef.h>
size_t pt_town_map_asset_name(u8 town, bool bez, u8 text[16]);
size_t pt_town_city_asset_name(u8 town, u8 text[16]);
size_t pt_town_temp_asset_name(u8 town, u8 text[5]);

// Town teardown.

void FreeRecipe(const Recipe *recipe);
void FreeAllRecipes(void);
void FreePoiContractsRestaurants(void);
Town *FreeLotsOfStuff(void);
void FreeRestaurant(Rest *restaurant, u8 update_city_count);
Town *TownFreeSuppliersAndZones(void);

#endif
