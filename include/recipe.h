// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
#ifndef PT_RECIPE_H
#define PT_RECIPE_H
#include "layout.h"
#include "state_types.h"
#include "types.h"
#include "arithmetic.h"

// Recipe list.

u8 HandleRecipeListMenu(Rest *restaurant);
void DrawDepotPriorityEntry(i16 x, i16 y, i16 width, const u8 *name, u8 selected);
void DrawRecipeList(const Player *player, u8 page, u8 selected, u8 custom_count);
u8 GetLastStandardRecipe(const Player *player);
u32 GenerateRecipeFileName(void);
i32 GuestStrcmp(const u8 *first, const u8 *second);
u32 GetSizeOfRecipe(const Recipe *recipe);
i32 SetPricePerPizza(RestoRecipe *entry, double price);
void RestClearPizzaQualitiesSum(Rest *restaurant, u8 index);
PTActionResult RestInitNewPizza(Rest *restaurant, Recipe *recipe, double price);

// Recipe display.

const RestoRecipe *RestFindRestoRecipe(const Recipe *recipe, const Rest *restaurant);
void DrawRightAlignedFloat(float value);
void DrawSupplierStockPrice(i32 cents);
u32 DrawPizzaPopularityChart_Helper1(u8 index, i16 x, i16 y, i16 width);
void DrawPizzaPopularityChart_Helper2(i16 y);
void DrawBarChart(const u8 *text, u8 rating, i16 x, i16 y);
void PrintPizzaSalesOrRecipe(Recipe *recipe, u8 info);

// Recipe editor.

// Competition edits return elapsed seconds; ordinary edits return a recipe.

typedef enum PTPizzaEditorResultKind { PT_PIZZA_EDITOR_RECIPE, PT_PIZZA_EDITOR_DURATION } PTPizzaEditorResultKind;
typedef struct PTPizzaEditorResult {
  PTPizzaEditorResultKind kind;
  Recipe *recipe;
  u32 seconds;
} PTPizzaEditorResult;
const u8 *GetDefaultRecipeName(u8 recipe);
u16 DrawEmptyPizzaBg(void);
u16 CreatePizza_Helper1(void);
Game *CreatePizza_Helper2(void);
u32 CreatePizza_Helper5(void);
PTActionResult CreatePizza_Helper3(u8 large);
void IngredientsFull(void);
void CreatePizza_Helper4(u8 orientation);
PTPizzaEditorResult CreatePizza(Rest *restaurant, const Recipe *standard, u8 mode, u8 recipe_id);

// Recipe info.

u32 GetRecipeIndex(const Recipe *recipe);
void PrintRecipeInfoBox(Recipe *recipe, u32 unused);
void PrintRecipeInfo2(u8 kind, u8 item, u8 variant);
void StartPizzaCompetition(const Recipe *recipe);
void HandleCompetitionChallenge(const Rest *restaurant);

// Price parser.

// The cave is independent of the other labelled repair binaries.

// Native binary64 result with the original decimal rounding algorithm.

double GuestStrtod(const u8 *text, const u8 **end_pointer);
u64 Scale10V(u64 value, i32 exponent);
u64 IntegerDoubleBits(u64 value, u32 exponent_bits);
void DecimalDigitsToDouble(const u8 *digits, u64 *output);
u32 *GuestSetERange(void);

// Pizza menu.

Canvas *DrawTextAndAdvanceLine(const u8 *text);
void DrawPizzaMenu(Rest *restaurant, u8 scroll, u8 mode, i8 selected);
u32 ShowPizzaInfoInBox(Rest *restaurant, const RestoRecipe *entry);
u32 HandlePizzaMenu(Rest *restaurant);
RestoRecipe *RestRemovePizza(Rest *restaurant, u8 index);
u8 RestUpdateFurnitureRating(Rest *restaurant);
PTCanvasResult DrawCustomerRatings(Rest *restaurant, u8 menu);

// Recipe statistics.

u32 ComputeIngredientCalories(const Ingredient *ingredient, u16 weight);
u32 ComputeRecipeStatistics(Recipe *recipe);
u32 ComputeRecipeWeights(Recipe *recipe);

// Recipe jury.

u32 ComparePizza_CompareIngredient(const RecipeEntry *a, const RecipeEntry *b);
u32 ComparePizzaRecipe(const Recipe *standard, const Recipe *recipe);
u8 Return1(const Recipe *unused_recipe);
u8 GetCharacterCookingSkill(const Recipe *unused_recipe, const Character *character);
void EvaluatePizzaRecipe_GraphicsHelper(i16 x, i16 y);
void EvaluateStandardPizza_Helper2(void);
void EvaluatePizzaRecipe(const Recipe *standard, Recipe *recipe, u8 mode);
Recipe *AllocateRecipe(u32 entries);
Recipe *PlayerInsertRecipe(Player *player, u8 recipe_id);

#endif
