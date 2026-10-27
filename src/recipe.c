// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
// Recipe.

#include "state_types.h"
#include "game.h"
#include "recipe.h"

u16 IsValidIngredient(u8 index) {
  u16 valid = 0;
  if (index < 96) {
    u32 available = num_ingredients_in_set[index / 24];
    valid = index % 24 < available;
  }
  return valid;
}

// Select four ingredients by weight and factor; ties retain the first one.
// Empty slots use ingredient zero. The original sentinel 255 overflowed the
// selected-ingredient flags when fewer than four scores were positive.

void FindMajorRecipeIngredients(Recipe *r) {
  // Entry command or enclosing weights preflight proves destination, bounded
  // sets and either the complete repair or four positive stock candidates.

  RecipeIngredientSelection storage;
  RecipeIngredientSelection *locals = &storage;

  memset(locals->selected, 0, sizeof(locals->selected));

  memset(r->major_ingredient, 0, sizeof(r->major_ingredient));

  locals->rank = 0;
  while (locals->rank < 4) {
    unsigned rank = locals->rank;
    u32 best_value = 0;
    for (unsigned set = 0; set < 4; ++set) {
      for (unsigned item = 0; item < num_ingredients_in_set[set]; ++item) {
        unsigned index = 24 * set + item;

        u16 valid = IsValidIngredient((u8)index);

        if (valid && !locals->selected[index]) {
          u32 value = ingredients[set].vegs[item].factor * (u32)(u16)r->weight_of_type[index];
          if (value > best_value) {
            r->major_ingredient[rank] = (u8)index;
            best_value = value;
          }
        }
      }
    }
    locals->selected[r->major_ingredient[rank]] = 1;
    locals->rank = rank + 1u;
  }
}

// Recipe statistics.

#include "objects.h"
#include "finance.h"

u32 ComputeIngredientCalories(const Ingredient *ingredient, u16 weight) {
  u32 calories = ingredient->byte1B;
  if (calories > 250)
    calories += 10u * (calories - 250u);
  return (u32)weight * calories / 100u;
}
u32 ComputeRecipeWeights(Recipe *recipe) {
  memset(recipe->weight_of_type, 0, sizeof(recipe->weight_of_type));
  u8 index = 0;
  while (index < recipe->num_entries) {
    const RecipeEntry *entry = pt_recipe_entry(recipe, index);
    u8 ingredient = entry->ingredient, variation = entry->variation >> 2;
    u8 amount = pt_ingredient_amount(ingredient, variation);
    u16 weight = (u16)recipe->weight_of_type[ingredient];
    weight = (u16)(weight + amount);
    recipe->weight_of_type[ingredient] = (i16)weight;
    ++index;
  }
  FindMajorRecipeIngredients(recipe);
  u32 result = ComputeRecipeStatistics(recipe);
  return result;
}
// FILD qword sees the DWORD total with a zero high DWORD. FSTP single then
// rounds that unsigned integer to binary32, nearest/even. Explicit integer
// rounding makes this independent of host floating-point control settings.

static u64 denominator(u32 total, u32 *bits) {
  unsigned shift = 0;
  u32 mantissa = total;
  while (mantissa > UINT32_C(0xffffff)) {
    mantissa >>= 1;
    ++shift;
  }
  if (shift) {
    u32 remainder = total & ((1u << shift) - 1u), half = 1u << (shift - 1u);
    if (remainder > half || (remainder == half && (mantissa & 1u)))
      ++mantissa;
  }
  u64 rounded = (u64)mantissa << shift;
  // Already exactly representable.
  *bits = pt_float_bits((float)rounded);
  return rounded;
}
// A signed-word numerator and integral binary32 denominator cannot round
// across an integer boundary at x87 53-bit precision. Integer division thus
// reproduces FDIV / __CHP / FISTP, including signed truncation. Masked zero
// division converts to the original integer-indefinite DWORD.

static u32 quotient(i16 numerator, u64 divisor) {
  return divisor ? (u32)((i64)numerator / (i64)divisor) : UINT32_C(0x80000000);
}
u32 ComputeRecipeStatistics(Recipe *recipe) {
  u32 totals[4] = {0};
  u32 spice_weight = 0, total_weight = 250, total_calories = 0;
  u8 index = 0;
  while (index < 96) {
    u16 weight = (u16)recipe->weight_of_type[index];
    if (weight) {
      const Ingredient *ingredient = &ingredients[index / 24].vegs[index % 24];
      const i16 properties[4] = {ingredient->fat, ingredient->sweet, ingredient->salty, ingredient->hot};
      for (unsigned i = 0; i < 4; ++i) {
        u32 product = (u32)(i32)properties[i] * (u32)(i32)(i16)weight * (ingredient->factor / 2u);
        u32 value = (u32)(i32)(i16)((u16)totals[i]);
        totals[i] = value + product;
      }
      if (index >= 80)
        spice_weight += weight;
      total_weight += (u32)(i32)(i16)weight;
      u32 calories = ComputeIngredientCalories(ingredient, weight);
      total_calories += (u16)calories;
    }
    ++index;
  }
  u32 float_bits;
  u64 divisor = denominator(total_weight, &float_bits);
  for (unsigned i = 0; i < 4; ++i) {
    u32 value = quotient((i16)((u16)totals[i]), divisor);
    u32 clamped = (u32)Clamp(pt_signed_dword(value), 0, 10);
    recipe->tastes.values[i] = (u8)clamped;
  }
  i32 spice = (i16)((u16)spice_weight) / 3;
  recipe->amount_of_spices = (u8)Clamp(spice, 0, 10);
  recipe->calories = (i16)((u16)total_calories);
  return total_calories;
}

// Recipe editor.

#include "messages.h"
#include "graphics.h"
#include "text.h"
#include "menu.h"
#include "world.h"
#include "calendar.h"
#include "restaurant.h"
#include "sound.h"
#include "input.h"
#include <stdlib.h>
#include <string.h>

const u8 *GetDefaultRecipeName(u8 recipe) {
  return pt_default_recipe_name(recipe);
}
Game *CreatePizza_Helper2(void) {
  game_ptr->minutes_left = 0;
  game_ptr->seconds_left = 0;
  game_ptr->show_time_left_instead_of_date = 1;
  return game_ptr;
}
u32 CreatePizza_Helper5(void) {

  game_ptr->show_time_left_instead_of_date = 0;
  u32 result = (u32)game_ptr->seconds_left + 60u * (u32)game_ptr->minutes_left;

  return result;
}
void CreatePizza_Helper4(u8 orientation) {
  u16 image, x, y;
  if (orientation & 1u) {
    (void)BitBlt(bmp26_ptr, 7, 12, current_bitmap, 19, 156, 38, 15);
    image = 193;
    x = 29;
    y = 147;
  } else {
    (void)BitBlt(bmp26_ptr, 17, 1, current_bitmap, 29, 145, 16, 35);
    image = 194;
    x = 19;
    y = 156;
  }
  DrawBob(&bob_ingredients, image, x, y);
}
u16 DrawEmptyPizzaBg(void) {
  LoadBobFile(&bob_ingredients);
  FillLetterBackground(4, 1, 6);
  SetupCanvasTempIglooLink();
  (void)BitBlt(current_bitmap, 12, 144, bmp26_ptr, 0, 0, 48, 36);
  CreatePizza_Helper4(0);
  return DrawMapHelper4(current_bitmap);
}
// The original wrapper falls directly into DrawEmptyPizzaBg.

u16 CreatePizza_Helper1(void) {
  return DrawEmptyPizzaBg();
}
static void competition_greeting(void) {
  const Character *character = recipe_rank_character;
  const u8 *name = character->first_name;
  const u8 *end = memchr(name, 0, sizeof(character->first_name));
  if (!end)
    abort();
  size_t n = (size_t)(end - name);
  memcpy(strbuf, "Right, ", 7);
  memcpy(strbuf + 7, name, n);
  memcpy(strbuf + 7 + n, "! Let's see what you can do!", 29);
  DisplayErrorMessage(0, strbuf);
  CreatePizza_Helper2();
}
PTActionResult CreatePizza_Helper3(u8 large) {
  PTActionResult result;
  if (large) {
    result = pt_action_value(RepositionFurnitureHelper2(current_bitmap));
  } else {
    result = RepositionFurnitureHelper1(&temp_hazel, temp_canvas, active_palette_pointer);
  }
  return result;
}

void IngredientsFull(void) {
  ShowMessage(16, ingredients_full_text);
}
// The editor reads a signed image WORD before the sprite setup helper's
// unsigned index clamp. Keep these two original address calculations distinct.

PTPizzaEditorResult CreatePizza(Rest *restaurant, const Recipe *standard, u8 mode, u8 recipe_id) {
  (void)restaurant;
  u16 repeat_item = UINT16_MAX, repeat_category = UINT16_MAX;
  u32 saved_rotation = 0;
  u16 refinement_total = 0;
  const u16 category_top = 24, pizza_top = 42;
  u32 appearance_penalty = 0;
  u8 details_page = 0;
  u16 vertical_balance[96], horizontal_balance[96];
  for (unsigned i = 0; i < 96; ++i) {
    vertical_balance[i] = 0;
    horizontal_balance[i] = 0;
  }
  Recipe *buffer = recipe_lookup_fallback;
  pt_recipe_reset(buffer);
  Recipe *recipe = recipe_lookup_fallback;
  recipe->id = recipe_id;
  PTPizzaEditorResult result = {PT_PIZZA_EDITOR_RECIPE, recipe, 0};
  if (mode != 2) {
    LoadMusic(9);
    (void)Do3x3ButtonStuff(9);
  }
  InitGraphicsMode();
  UnloadLinkedBob(&bob_restaurant);
  u16 loaded = LoadBobFile(&bob_ingredients);
  if (loaded) {
    CreatePizza_Helper1();
    if (mode == 2 || recipe_rank_flag) {
      (void)Do3x3ButtonStuff(17);
      competition_greeting();
    } else
      (void)Do3x3ButtonStuff(9);
    SetCurrentFont(some_font_ptr);
    if (mode != 2 && recipe_id < 14) {
      const u8 *name = GetDefaultRecipeName(recipe_id);
      (void)pt_strcpy(recipe->name, name);
    }
    u16 selected_preparation = 0;
    u16 selected_item = UINT16_MAX;
    u16 selected_category = UINT16_MAX;
    u32 rotation = 0;
    for (;;) {
      u16 event = WaitForMouseClick();
      mouse_pressed = event;
      if (!event)
        continue;
      if (event == PT_MENU_BUTTON_5 ||
          ((event == PT_MENU_BUTTON_4 || (event == PT_MENU_BUTTON_1 && recipe_rank_flag)) && !recipe->num_entries)) {
        game_ptr->show_time_left_instead_of_date = 0;
        AdvanceClock(2);
        result.recipe = NULL;
        break;
      }
      if (event == PT_MENU_BUTTON_1 && !recipe_rank_flag && mode != 2) {
        if (details_page) {
          details_page = 0;
          PrintRecipeInfoBox(recipe_lookup_fallback, 0);
          StartPizzaCompetition(recipe_lookup_fallback);
        }
        continue;
      }
      if (event == PT_MENU_BUTTON_2 || event == PT_MENU_BUTTON_3) {
        u8 details = (u8)(event - PT_MENU_BUTTON_1);
        if (details_page != details) {
          details_page = details;
          if (event == PT_MENU_BUTTON_3)
            recipe->imitation = 100;
          PrintPizzaSalesOrRecipe(recipe_lookup_fallback, (u8)(event == PT_MENU_BUTTON_3));
        }
        continue;
      }
      if (event == PT_MENU_BUTTON_1 && !recipe_rank_flag && mode == 2) {
        EvaluatePizzaRecipe(standard, recipe_lookup_fallback, 2);
        break;
      }
      if (event == PT_MENU_BUTTON_4 || (event == PT_MENU_BUTTON_1 && recipe_rank_flag)) {
        AdvanceClock(5);
        recipe->num_unique_ingredients = 0;
        for (unsigned i = 0; i < 96; ++i) {
          if (recipe->weight_of_type[i])
            recipe->num_unique_ingredients = (u8)((u32)recipe->num_unique_ingredients + 1u);
        }
        FindMajorRecipeIngredients(recipe);
        recipe->refinement = (u8)((100u * refinement_total) / (2u * recipe->num_entries));
        for (unsigned i = 0; i < 96; ++i) {
          for (unsigned axis = 0; axis < 2; ++axis) {
            i32 value = (i16)(axis ? vertical_balance[i] : horizontal_balance[i]), distance;
            distance = abs(value);
            if (distance > 60) {
              distance = abs(value);
              (appearance_penalty = appearance_penalty + (u32)(distance - 60));
            }
          }
        }
        i32 appearance = Clamp((u8)(100u - (appearance_penalty >> 3)), 0, 100);
        recipe->appearance = (u8)appearance;
        EvaluatePizzaRecipe(standard, recipe_lookup_fallback, mode);
        result.recipe = PlayerInsertRecipe(player_ptr, recipe_id);
        if (recipe_id >= 50) {
          SetCurrentFont(font_astandar);
          game_ptr->text_edit_max_width = 130;
          AskForPizzaPrice(recipe_name_prompt, result.recipe->name, 118);
          game_ptr->text_edit_max_width = 149;
          MarkGoalReached(46);
        }
        break;
      }
      // All five toolbar events have been handled above.

      if (mouse_pressed == 1) {
        if (details_page)
          goto reset_selection;
        selected_category = UINT16_MAX;
        if (mouse_y > (i16)category_top && mouse_y < (i16)pizza_top && mouse_x > 30) {
        select_category:
          if (mouse_x < 183)
            selected_category = 3;
          if (mouse_x < 142)
            selected_category = 2;
          if (mouse_x < 101)
            selected_category = 1;
          if (mouse_x < 60)
            selected_category = 0;
        }
        i16 kind = (i16)selected_category;
        if (kind == -1) {
          if (recipe->num_entries >= 250) {
            IngredientsFull();
            continue;
          }
          selected_item = repeat_item;
          selected_category = repeat_category;
          rotation = saved_rotation;
        } else {
          bool valid_category = kind >= 0 && kind < 4;
          if (!valid_category)
            abort();
          u8 count = num_ingredients_in_set[kind];
          if (count > PT_POPUP_MENU_CAPACITY)
            abort();
          const u8 *items[PT_POPUP_MENU_CAPACITY];
          for (unsigned i = 0; i < count; ++i)
            items[i] = ingredients[kind].vegs[i].name;
          ExtractMapPopupMenuItems(items, count);
          u8 selected = DisplayMapPopupMenu(ingredient_popup_x[kind], 30, count);
          if (selected == 255)
            continue;
          if (recipe->num_entries >= 250) {
            IngredientsFull();
            continue;
          }
          selected_item = (u16)(i16)(i8)selected;
          // Only the low 16 bits are used on repeat selection.

          repeat_item = (u16)(i16)(i8)selected;
          repeat_category = selected_category;
          selected_preparation = 0;
          rotation = 0;
          saved_rotation = 0;
        }
      }
      if (mouse_pressed == 2) {
        if (details_page)
          goto reset_selection;
        if (recipe->num_entries) {
          recipe->num_entries = (u8)((u32)recipe->num_entries - 1u);
          const RecipeEntry *entry = pt_recipe_entry(recipe, recipe->num_entries);
          u16 ingredient = entry->ingredient, variation = entry->variation, variant = variation >> 2;
          bool valid_undo = ingredient < 96 && variation < 12;
          if (!valid_undo)
            abort();
          selected_preparation = variant;
          rotation = variation % 4;
          selected_category = ingredient / 24;
          selected_item = ingredient % 24;
          repeat_item = ingredient % 24;
          repeat_category = selected_category;
          saved_rotation = rotation;
          const Ingredient *data = &ingredients[ingredient / 24].vegs[ingredient % 24];
          recipe->weight_of_type[ingredient] = (i16)((u16)(recipe->weight_of_type[ingredient] - data->amount[variant]));
          u16 image = data->sprite_images[variation];
          const BobHeader *header = pt_bob_signed_header(&bob_ingredients, image);
          // Stock undo subtracts (3-variant), although placement
          // adds (4-variant); it also retains total refinement.

          u32 dx = 116u - (entry->x + header->w / 2u), dy = 117u - (entry->y + header->h / 2u),
              factor = 3u - (u32)selected_preparation;
          horizontal_balance[ingredient] = (u16)(horizontal_balance[ingredient] - factor * dx);
          vertical_balance[ingredient] = (u16)(vertical_balance[ingredient] - factor * dy);
          Copy320x200Bitmap(current_bitmap, old_bitmap);
          SetCurrentBitmap(old_bitmap);
          StartPizzaCompetition(recipe_lookup_fallback);
          CommitBitmapAndSetPalette(0);
        }
        continue;
      }
      if (selected_item == UINT16_MAX) {
        SetupCanvasTempIglooLink();
        continue;
      }
      // Original bug: timer-driven repeats bypass both click-time
      // capacity checks. The labelled original repair adds this check
      // before creating another drag, using the existing full message.

      if (recipe->num_entries >= 250) {
        // Repair behavior is selected explicitly in native_profile.h.

        IngredientsFull();
        goto reset_selection;
      }
      SaveAndSetInputEventFilter(5);
    restart_drag:
      saved_rotation = rotation;
      HideMouseCursor_();
      force_map_input = 1;
      mouse_pressed = 0;
      u8 drag_retry = 0;
      SetupCanvasTempIglooLink();
      u16 kind = selected_category, item = selected_item, variant = selected_preparation;
      u16 ingredient = (u16)(24u * (u32)selected_category + (u32)selected_item),
          variation = (u16)(rotation + 4u * (u32)selected_preparation);
      bool valid_drag = kind < 4 && item < 24 && variation < 12;
      if (!valid_drag)
        abort();
      const Ingredient *data = &ingredients[kind].vegs[item];
      u16 image = data->sprite_images[variation];
      const BobHeader *header = pt_bob_signed_header(&bob_ingredients, image);
      PrintRecipeInfo2((u8)kind, (u8)item, (u8)variant);
      u8 large = header->w > 20 || header->h > 20;
      if (large) {
        Copy320x200Bitmap(current_bitmap, old_bitmap);
        DrawMapHelper4(current_bitmap);
      }
      SetupHazelWithBobSprite(&temp_hazel, large ? NULL : ingredient_background, &bob_ingredients, image);
      for (;;) {
        const Mouse *mouse = mouse_result;
        mouse_x = mouse->x;
        mouse_y = mouse->y;
        temp_igloo.draw_x = (i16)((u32)(i32)mouse_x - header->w / 2u);
        temp_igloo.draw_y = (i16)((u32)(i32)mouse_y - header->h / 2u);
        SortCanvasIgloo(temp_canvas);
        if (large) {
          RepositionFurnitureHelper2(old_bitmap);
          SetCurrentBitmap(old_bitmap);
        }
        Canvas_DrawAllIgloos(temp_canvas, active_palette_pointer);
        if (large)
          CommitBitmapAndSetPalette(0);
        event = WaitForMouseClick();
        mouse_pressed = event;
        if (!event)
          continue;
        if (event == 1) {
          force_map_input = 0;
          if (mouse_y < (i16)pizza_top) {
            CreatePizza_Helper3(large);
            (void)GraphicsRestoreCursor2();
            goto select_category;
          }
          if ((temp_igloo.draw_x < 58 && (i32)temp_igloo.draw_y + header->h > 145) || (mouse_x < 56 && mouse_y > 138)) {
            CreatePizza_Helper3(large);
            if ((i16)variant < 2) {
              selected_preparation = (u16)(variant + 1);
              if (!game_ptr->debug_mode) {
                if (mode != 2) {
                  SoundStuff2(0);
                }
                for (unsigned i = 1; i <= 18; ++i) {
                  CreatePizza_Helper4((u8)i);
                  Delay(2);
                }
                if (mode != 2) {
                  SoundStuff1();
                }
              }
            }
            goto restart_drag;
          }
          u32 dx = (u32)((i32)mouse_x - 117), dy = (u32)((i32)mouse_y - 116), distance = dx * dx + dy * dy;
          if (pt_signed_dword(distance) > 3721 || temp_igloo.draw_y < (i16)pizza_top)
            drag_retry = 1;
          if (drag_retry) {
            CreatePizza_Helper3(large);
            (void)GraphicsRestoreCursor2();
            if (drag_retry == 2)
              goto reset_selection;
            goto restart_drag;
          }
        } else if (event == 2) {
          ++rotation;
          if ((i16)rotation > 3)
            rotation = 0;
          CreatePizza_Helper3(large);
          goto restart_drag;
        }
        // Toolbar events keep dragging. Other nonzero events,
        // including timer value 10, reach the original drop path.

        if (pt_is_menu_button_event(mouse_pressed))
          continue;
        break;
      }
      bool valid_drop = ingredient < 96 && variant < 3 && recipe->num_entries < 250;
      if (!valid_drop)
        abort();
      RecipeEntry *entry = pt_recipe_mutable_entry(recipe, recipe->num_entries);
      entry->ingredient = (u8)ingredient;
      entry->variation = (u8)variation;
      entry->x = (u8)temp_igloo.draw_x;
      entry->y = (u8)temp_igloo.draw_y;
      recipe->weight_of_type[ingredient] = (i16)((u16)(recipe->weight_of_type[ingredient] + data->amount[variant]));
      u32 dx = 116u - ((u32)(i32)temp_igloo.draw_x + header->w / 2u),
          dy = 117u - ((u32)(i32)temp_igloo.draw_y + header->h / 2u), factor = 4u - (u32)selected_preparation;
      horizontal_balance[ingredient] = (u16)(horizontal_balance[ingredient] + factor * dx);
      vertical_balance[ingredient] = (u16)(vertical_balance[ingredient] + factor * dy);
      ComputeRecipeStatistics(recipe_lookup_fallback);
      PrintRecipeInfoBox(recipe_lookup_fallback, 0);
      refinement_total = (u16)(refinement_total + selected_preparation);
      temp_canvas->actors = &map_igloo_link;
      recipe->num_entries = (u8)((u32)recipe->num_entries + 1u);
      SaveAndSetInputEventFilter(3);
      (void)GraphicsRestoreCursor2();
      continue;
    reset_selection:
      selected_item = UINT16_MAX;
      selected_category = UINT16_MAX;
    }
  }
  SaveAndSetInputEventFilter(3);
  if (mode == 2) {
    CopyBitmap2to1();
    result.kind = PT_PIZZA_EDITOR_DURATION;
    result.seconds = CreatePizza_Helper5();
  } else {
    if (recipe_rank_flag)
      game_ptr->show_time_left_instead_of_date = 0;
    (void)Do3x3ButtonStuff(20);
    CopyBitmap2to1();
    AdvanceClock(3);
  }
  return result;
}

// Recipe info.

#include "arithmetic.h"
#include "savegame.h"
#include "missions.h"
#include "reports.h"
#include <math.h>
#include <stdio.h>
u32 GetRecipeIndex(const Recipe *recipe) {
  const Player *player = player_ptr;
  const PTForwardLink *node = player->recipes.begin.next;
  u32 index = 0;
  while ((const void *)node != recipe && index < player->num_recipes) {
    node = node->next;
    ++index;
  }
  return index;
}
static void number(u8 *output, size_t capacity, const u8 *format, i32 value) {
  int size = snprintf((char *)output, capacity, (const char *)format, value);
  (void)size;
}
static void info_number(u8 *output, size_t capacity, const u8 *format, i32 value) {
  number(output, capacity, format, value);
  (void)DrawInfoBoxText(output);
}
void PrintRecipeInfo2(u8 kind, u8 item, u8 variant) {

  const Ingredient *ingredient = &ingredients[kind].vegs[item];
  u32 amount = ingredient->amount[variant];
  i16 trend = (i8)current_town->ingredient_trend[24u * kind + item];

  FillGenericInfoBox();
  (void)DrawInfoBoxText(ingredient->name);
  info_number(strbuf, sizeof(strbuf), ingredient_grammes_format, (i32)amount);
  u32 calories = ComputeIngredientCalories(ingredient, (u16)amount);
  info_number(strbuf, sizeof(strbuf), ingredient_calories_format, (u16)calories);
  info_number(strbuf, sizeof(strbuf), ingredient_intensity_format, ingredient->factor);
  info_number(strbuf, sizeof(strbuf), ingredient_fat_format, ingredient->fat);
  info_number(strbuf, sizeof(strbuf), ingredient_sweet_format, ingredient->sweet);
  info_number(strbuf, sizeof(strbuf), ingredient_salty_format, ingredient->salty);
  info_number(strbuf, sizeof(strbuf), ingredient_hot_format, ingredient->hot);
  const u8 *trend_name = ingredient_trend_names[(trend > -30) + (trend > -9) + (trend > 9) + (trend > 30)];
  info_number(strbuf, sizeof(strbuf), ingredient_trend_format, trend);
  int size =
      snprintf((char *)strbuf, sizeof(strbuf), (const char *)ingredient_trend_name_format, (const char *)trend_name);
  (void)size;
  (void)DrawInfoBoxText(strbuf);
  PrintCurrentDateAndTime();
}
void PrintRecipeInfoBox(Recipe *recipe, u32 unused) {
  (void)unused;
  u8 caption[32];
  FillGenericInfoBox();
  u32 count = player_ptr->num_recipes, index;
  index = GetRecipeIndex(recipe);
  int size =
      snprintf((char *)caption, sizeof(caption), (const char *)recipe_number_format, (int)(index + 1), (int)count);
  (void)size;
  (void)DrawInfoBoxText(caption);
  (void)DrawInfoBoxText(recipe->name);
  (void)ComputeRecipeStatistics(recipe);
  info_number(caption, sizeof(caption), recipe_calories_format, (u16)recipe->calories);
  info_number(caption, sizeof(caption), recipe_fat_format, recipe->fat);
  info_number(caption, sizeof(caption), recipe_sweet_format, recipe->sweet);
  info_number(caption, sizeof(caption), recipe_salty_format, recipe->salty);
  info_number(caption, sizeof(caption), recipe_hot_format, recipe->hot);
  info_number(caption, sizeof(caption), recipe_appearance_format, recipe->appearance);
  info_number(caption, sizeof(caption), recipe_refinement_format, recipe->refinement);
  i8 trend;
  trend = RecipeComputeTrend(recipe);
  info_number(caption, sizeof(caption), recipe_trend_format, trend);
  if ((i8)recipe->id < 14)
    info_number(caption, sizeof(caption), recipe_imitation_format, recipe->imitation);
  else
    info_number(caption, sizeof(caption), recipe_normality_format, recipe->normality);
  PrintCurrentDateAndTime();
}
void StartPizzaCompetition(const Recipe *recipe) {
  temp_canvas->actors = &temp_igloo_link;
  if (recipe) {
    (void)DrawEmptyPizzaBg();
    if (pizza_competition_delay) {
      (void)HideMouseCursor_();
    }
    for (unsigned i = 0; i < recipe->num_entries; ++i) {
      PizzaCompetitionSoundStuff();
      const RecipeEntry *entry = pt_recipe_entry(recipe, i);
      if (pizza_competition_delay) {
        Delay(pizza_competition_delay);
        FillGenericInfoBox();
        (void)DrawInfoBoxText(pizza_competition_title);
        (void)DrawInfoBoxText(pizza_competition_separator);
        (void)DrawInfoBoxText(pizza_competition_original_label);
        (void)DrawInfoBoxText(recipe->name);
        (void)DrawFlyerOutput(pizza_competition_level_label, pizza_level);
        (void)DrawInfoBoxText(pizza_competition_ingredient_label);
        (void)DrawInfoBoxText(ingredients[entry->ingredient / 24u].vegs[entry->ingredient % 24u].name);
      }
      u16 image = ingredients[entry->ingredient / 24u].vegs[entry->ingredient % 24u].sprite_images[entry->variation];
      DrawBob(&bob_ingredients, image, entry->x, entry->y);
    }
    if (pizza_competition_delay) {
      PizzaCompetitionSoundStuff();
      ShowMessage(15, pizza_competition_ready_text);
      Delay(10u * pizza_competition_delay);
      pizza_competition_delay = 0;
      (void)GraphicsRestoreCursor2();
    }
  }
}

static const char *competition_name(const u8 *name, size_t size) {
  (void)size;
  return (const char *)name;
}
typedef struct CompetitionLimit {
  double money;
  i32 limit;
} CompetitionLimit;
static CompetitionLimit competition_maximum(double money) {
  double value = competition_maximum_bet >= money ? money : competition_maximum_bet;
  double integral = TruncateX87Integral(value);
  i32 limit = isfinite(integral) && integral >= -2147483648. && integral < 2147483648. ? (i32)integral : INT32_MIN;
  return (CompetitionLimit){value, limit};
}
typedef struct CompetitionState {
  u32 stakes[8];
  u16 participating[8], scores[8], totals[8], durations[8];
  u32 speed, difficulty_base, winner, best_total, previous_participant;
  u8 row, saved_hierarchy, results_index, target, jury_recipe;
  u8 tied, portrait, participant_index, saved_player;
} CompetitionState;
void HandleCompetitionChallenge(const Rest *restaurant) {
  CompetitionState storage;
  CompetitionState *state = &storage;
  state->saved_hierarchy = game_ptr->in_menu_hierarchy;
  state->tied = 1;
  state->best_total = 0;
  state->winner = UINT32_MAX;
  state->previous_participant = UINT32_MAX;
  if (!(characters[current_player_index].money >= competition_minimum_money))
    goto done;
  game_ptr->in_menu_hierarchy = 0;
  (void)SaveAndSetInputEventFilter(1);
  InitGraphicsMode();
  memset(state->participating, 0, sizeof(state->participating));
  (void)FillLetterBackground(4, 4, 18);
  SetCurrentFont(font_ruby);
  temp_canvas->color = 8;
  (void)DrawText2(12, 203, 44, competition_pizza_title);
  (void)DrawText2(12, 203, 54, competition_title);
  CopyBitmap2to1();
  if (restaurant) {
    u8 target = restaurant->player_index;
    state->target = target;
    state->portrait = players[target].character_index;
    state->participating[current_player_index] = 1;
    state->participating[target] = 1;
    u8 *question;
    u8 portrait;
    if (target >= game_ptr->num_interactive_players) {
      if (players[target].gap364)
        goto declined;
      question = competition_ai_challenge_text;
      portrait = target;
    } else {
      const Character *a = &characters[target], *b = &characters[current_player_index];
      (void)snprintf((char *)strbuf,
          sizeof(strbuf),
          (const char *)competition_challenge_format,
          competition_name(a->first_name, sizeof(a->first_name)),
          competition_name(a->last_name, sizeof(a->last_name)),
          competition_name(b->first_name, sizeof(b->first_name)),
          competition_name(b->last_name, sizeof(b->last_name)));
      question = strbuf;
      portrait = state->portrait;
    }
    u8 yes = ShowMessage2(portrait, question);
    if (!yes)
      goto cleanup;
    unit_for_number_query = competition_dollars_unit;
    CompetitionLimit maximum = competition_maximum(characters[current_player_index].money);
    i32 bet = ShowAskForNumberQuery(competition_bet_prompt, 500, 500, 500, maximum.limit);
    state->stakes[target] = (u32)bet;
    state->stakes[current_player_index] = (u32)bet;
    if (target >= game_ptr->num_interactive_players) {
      if ((double)(u32)bet > players[target].money * competition_ai_money_fraction)
        goto declined;
    }
  } else {
    for (state->participant_index = 0; state->participant_index < 8; ++state->participant_index) {
      u8 i = state->participant_index;
      state->stakes[i] = 500;
      if (i >= game_ptr->num_interactive_players) {
        state->participating[i] = 1;
        continue;
      }
      if (players[i].days_away)
        continue;
      const Character *c = &characters[i];
      (void)snprintf((char *)strbuf,
          sizeof(strbuf),
          (const char *)competition_invitation_format,
          competition_name(c->first_name, sizeof(c->first_name)),
          competition_name(c->last_name, sizeof(c->last_name)));
      u8 yes = ShowMessage2(players[i].character_index, strbuf);
      if (!yes)
        continue;
      state->participating[i] = 1;
      unit_for_number_query = competition_dollars_unit;
      CompetitionLimit maximum = competition_maximum(c->money);
      i32 bet =
          ShowAskForNumberQuery(competition_stake_prompt, pt_signed_dword(state->stakes[i]), 500, 500, maximum.limit);
      state->stakes[i] = (u32)bet;
    }
  }
  u32 participating;
  participating = SumListOfUnsignedWords(state->participating, game_ptr->num_interactive_players);
  if (!participating)
    goto cleanup;
  unit_for_number_query = competition_no_unit;
  i32 speed;
  speed = ShowAskForNumberQuery(competition_speed_prompt, pizza_level, 1, 1, 5);
  pizza_level = (u8)speed;
  state->speed = pizza_level;
  pizza_competition_delay = (u8)(100u - 15u * pizza_level);
  state->difficulty_base = savegame_sect_0.field_70 < 40 ? savegame_sect_0.field_70 : 40;
  u32 random = RandomNumber(10);
  state->jury_recipe = (u8)(random + 20u + state->difficulty_base);
  state->difficulty_base = state->jury_recipe - 20u;
  SetCurrentFont(font_piznumb);
  (void)DrawText(40, 70, competition_jury_caption);
  Recipe *recipe;
  recipe = LoadRecipeFromCompetitionFile(state->jury_recipe);
  (void)DrawText(110, 70, recipe->name);
  if (prefs.field_2) {
    (void)LoadMusic(11);
    SoundStuff2(0);
    competition_sound_pending = 1;
  }
  StartPizzaCompetition(recipe);
  for (state->participant_index = 0; state->participant_index < 8; ++state->participant_index) {
    u8 i = state->participant_index;
    recipe_rank_character = &characters[i];
    if (!state->participating[i])
      continue;
    u16 score;
    if (i < game_ptr->num_interactive_players) {
      u32 duration = CreatePizza(NULL, recipe, 2, i).seconds;
      state->durations[i] = (u16)duration;
      score = curr_score;
    } else {
      random = RandomNumber(characters[i].dexterity / 2);
      state->durations[i] = (u16)(50u - random);
      i32 extra = MulDiv100(state->durations[i], (i32)(5u * state->difficulty_base));
      state->durations[i] = (u16)(state->durations[i] + (u32)extra);
      random = RandomNumber(14);
      i32 difficulty = (i32)state->jury_recipe - 10 + 5 * (i32)state->speed;
      i32 value = 100 - difficulty + characters[i].cooking / 5 + (i32)random, clamped;
      clamped = Clamp(value, 0, 99);
      score = (u16)clamped;
    }
    state->scores[i] = score;
    i32 bonus, combined;
    // Original reserves Clamp's max/min arguments across MulDiv100.

    bonus = MulDiv100(20 * pizza_level, score);

    combined = Clamp((i32)score + bonus - state->durations[i] / 3, 0, 200);
    state->totals[i] = (u16)combined;
    i16 previous = (i16)state->previous_participant;
    if (previous >= 0 && state->totals[i] != state->totals[(u16)previous])
      state->tied = 0;
    if ((i32)state->totals[i] > (i16)state->best_total) {
      state->best_total = state->totals[i];
      state->winner = i;
    }
    state->previous_participant = i;
    (void)FillLetterBackground(4, 4, 18);
    state->row = 0;
    for (state->results_index = 0; state->results_index <= state->participant_index; ++state->results_index) {
      u8 j = state->results_index;
      if (!state->participating[j])
        continue;
      temp_canvas->color = 0;
      SetCurrentFont(font_piznumb);
      SetTempCanvasPosition(35, (i16)(60 + 9 * state->row));
      DrawListElement(players[j].name_text);
      const u8 *duration;
      duration = SprintfTimeLeft(state->durations[j]);
      (void)snprintf((char *)strbuf, sizeof(strbuf), (const char *)competition_time_format, (const char *)duration);
      cur_list_elem_x = 110;
      DrawStringBufferAsListElement();
      cur_list_elem_x = 148;
      DrawPercentageNumberInList(state->scores[j]);
      cur_list_elem_x = 174;
      DrawNumberInsideListRight(state->totals[j]);
      ++state->row;
    }
    Delay(50);
  }
  FreeRecipe(recipe);
  SetCurrentFont(font_astandar);
  temp_canvas->color = 8;
  i16 winner = (i16)state->winner;
  const u8 *caption = competition_draw_caption;
  if (winner != -1 && !state->tied) {
    (void)DrawText2(0, 0, 150, players[(u16)winner].name_text);
    caption = competition_win_caption;
  }
  (void)DrawText2(0, 0, 165, caption);
  (void)WaitForMouseClick();
  if (!state->tied)
    for (state->participant_index = 0; state->participant_index < 8; ++state->participant_index) {
      u8 i = state->participant_index;
      if (!state->participating[i])
        continue;
      u32 bet = state->stakes[i];
      if (i == (u16)winner) {
        (void)ChangeSocialOrUnderworldRating(i, 0, 40);
        (void)TransferPlayerMoney(i, 5, 10, (float)bet);
        ++savegame_sect_0.num_pizza_competitions[i][ptr_apple->town_index];
        if (i < game_ptr->num_interactive_players && savegame_sect_0.field_70 < 40) {
          state->saved_player = current_player_index;
          ++savegame_sect_0.field_70;
          SetCurrentPlayer(i);
          if (savegame_sect_0.num_pizza_competitions[i][ptr_apple->town_index] == 1)
            MarkGoalReached(87);
          if (savegame_sect_0.num_pizza_competitions[state->participant_index][ptr_apple->town_index] == 10)
            MarkGoalReached(123);
          SetCurrentPlayer(state->saved_player);
          DisplayStuffAfterPayment();
        }
      } else {
        (void)TransferPlayerMoney(i, 10, 5, (float)bet);
        if (restaurant) {
          u8 target = state->target;
          random = RandomNumber(100 - characters[target].courage);
          players[target].gap364 = (u8)random;
        }
      }
    }
  goto cleanup;
declined:
  DisplayErrorMessage(state->target, competition_decline_text);
cleanup:
  game_ptr->in_menu_hierarchy = state->saved_hierarchy;
  SoundStuff1();
  competition_sound_pending = 0;
  state_slot_007 = 100;
  (void)SaveAndSetInputEventFilter(3);
done:
  return;
}

// Recipe display.

#include "player.h"
const RestoRecipe *RestFindRestoRecipe(const Recipe *recipe, const Rest *restaurant) {
  const RestoRecipe *result = NULL;
  const Rest *rest = restaurant;
  unsigned count = rest->num_recipes;
  for (unsigned i = 0; i < count; ++i) {
    const Recipe *entry = pt_menu_recipe(rest, i);
    if (entry == recipe) {
      result = pt_menu_recipe_row(rest, i);
      break;
    }
  }

  return result;
}
// Watcom RTL193 caps fixed decimal conversion at 16 significant digits and
// removes the sign when rounding yields zero. RTL184 supplies trailing zeroes.
// EFG_Format's internal field is 39 bytes; larger output becomes asterisks,
// which the caller's unchecked decimal scan cannot safely consume.

static int fixed_price_text(float value, char bytes[52]) {
  double number = (double)value;
  if (fabs(number) < 0.005)
    number = 0.0;
  if (fabs(number) < 1e14)
    return snprintf(bytes, 52, "%8.2lf", number);
  char scientific[32], digits[16], unpacked[48];
  int n = snprintf(scientific, sizeof(scientific), "%.15e", fabs(number));
  if (n < 0 || n >= (int)sizeof(scientific))
    return -1;
  const char *exponent = strchr(scientific, 'e');
  if (!exponent)
    return -1;
  unsigned count = 0;
  for (const char *p = scientific; p < exponent; ++p)
    if (*p != '.') {
      if (count >= 16)
        return -1;
      digits[count++] = *p;
    }
  if (count != 16)
    return -1;
  int units = atoi(exponent + 1) + 1;
  if (units < 1 || units > 39)
    return -1;
  unsigned at = 0;
  if (number < 0.0)
    unpacked[at++] = '-';
  for (int i = 0; i < units; ++i)
    unpacked[at++] = i < 16 ? digits[i] : '0';
  unpacked[at++] = '.';
  for (int i = units; i < units + 2; ++i)
    unpacked[at++] = i < 16 ? digits[i] : '0';
  if (at > 39)
    return -1;
  unpacked[at] = 0;
  memcpy(bytes, unpacked, at + 1u);
  return (int)at;
}
void DrawRightAlignedFloat(float value) {
  char bytes[52];
  Canvas *canvas = temp_canvas;
  canvas->position_x = cur_list_elem_x;
  canvas->position_y = cur_list_elem_y;
  // The stock decimal scan has no terminator check. Stop before sprintf for
  // non-finite values rather than reading outside its 52-byte stack buffer.

  int length;
  length = fixed_price_text(value, bytes);
  const char *dot = memchr(bytes, '.', (size_t)length);

  u16 prefix = (u16)(dot - bytes + 1), width;
  width = Canvas_TextWidth(canvas, pt_text_span((const u8 *)bytes, prefix));
  canvas->position_x = (i16)((u16)canvas->position_x - width);
  u16 count;
  count = (u16)pt_strlen((const u8 *)bytes);
  (void)Canvas_DrawText(canvas, pt_text_span((const u8 *)bytes, count));
}
void DrawSupplierStockPrice(i32 cents) {
  DrawRightAlignedFloat((float)((double)cents / price_display_divisor));
}
u32 DrawPizzaPopularityChart_Helper1(u8 index, i16 x, i16 y, i16 width) {
  const SpriteRect *r = &blitmap_offsets[index];
  i16 bottom = r->bottom;
  i16 height = (i16)((i32)bottom - r->top + 1);
  return BitBlt(&bmp_blitmap, r->left, r->top, current_bitmap, (u16)x, (u16)y, (u16)width, (u16)height);
}
void DrawPizzaPopularityChart_Helper2(i16 y) {
  cur_list_elem_y = y;
  temp_canvas->position_y = y;
}

void DrawBarChart(const u8 *caption, u8 rating, i16 x, i16 y) {
  if (rating > 100)
    rating = 100;
  u32 width = 72u * rating / 100u;

  u8 index = rating > 70 ? 20 : rating > 30 ? 19 : 18;
  LoadBlitmapAndOffsets(3);
  (void)DrawBitmapStuff1(17, x, y);
  (void)DrawPizzaPopularityChart_Helper1(index, (i16)((u16)x - 1u), y, (i16)(width + 6u));
  SetCurrentFont(font_supersma);
  temp_canvas->color = 0;
  (void)SetTempCanvasPosition((i16)((u16)x + 6u), (i16)((u16)y + 8u));
  (void)DrawTextInsideList(caption);
  (void)SetTempCanvasPositionX((i16)((u16)x + (rating == 100 ? 82u : 80u)));
  SetCurrentFont(some_font_ptr);
  DrawPizzaPopularityChart_Helper2((i16)((u16)temp_canvas->position_y - 1u));
  DrawNumberInsideListRight(rating);
}
static u16 cost(const Recipe *recipe, const Rest *rest) {
  u32 result;
  result = ComputeRecipeCost(recipe, rest);
  return (u16)result;
}

void PrintPizzaSalesOrRecipe(Recipe *recipe, u8 info) {

  u32 total_cost = 0;
  u32 weight = 0, price_sum = 0;
  if (recipe) {
    LoadBlitmapAndOffsets(3);
    (void)FillLetterBackground(4, 4, 18);
    SetSomeFieldInCanvas(0);
    SetCurrentFont(font_astandar);
    temp_canvas->color = 10;
    (void)SetTempCanvasPosition(33, 38);
    (void)DrawTextInsideList(info ? recipe_view_info_text : recipe_view_recipe_text);
    (void)SetTempCanvasPositionX(83);
    if (recipe->name[0])
      (void)DrawTextInsideList(recipe->name);
    if (info) {
      const RestoRecipe *found;
      found = RestFindRestoRecipe(recipe, curr_rest);
      (void)ComputeRecipeStatistics(recipe);
      RecipeComputeAgeRatings(recipe);
      temp_canvas->color = 27;
      (void)SetTempCanvasPosition(48, 52);
      (void)DefaultLineTo(174, 52);
      (void)SetTempCanvasPosition(94, 43);
      (void)DefaultLineTo(94, 106);
      (void)SetTempCanvasPosition(136, 43);
      (void)DefaultLineTo(136, 106);
      temp_canvas->color = 9;
      SetCurrentFont(font_piznumb);
      (void)DrawText(108, 49, recipe_view_here_text);
      (void)DrawText(141, 49, recipe_view_all_text);
      (void)DrawText(50, 63, recipe_view_costs_text);
      cur_list_elem_x = 123;
      u16 here_cost = cost(recipe, curr_rest);
      DrawSupplierStockPrice(pt_signed_dword(here_cost));
      (void)DrawText(50, 73, recipe_view_price_text);
      if (found) {
        const RestoRecipe *sale = found;
        cur_list_elem_x = 123;
        DrawSupplierStockPrice(pt_signed_dword((u16)sale->price_per_pizza));
        (void)SetTempCanvasPosition(133, 83);
        DrawNumberInsideListRight(sale->sales);
        (void)SetTempCanvasPosition(123, 93);
        DrawSupplierStockPrice(pt_signed_dword((u32)sale->sales * ((u32)(u16)sale->price_per_pizza - here_cost)));
      }
      u8 restaurant_count = 0;

      const PTForwardLink *node = player_ptr->restaurants.begin.next;
      // A terminating sentinel has next==0 and is not processed. Cycles
      // are outside the bounded supported traversal.

      for (;;) {
        if (!node->next)
          break;

        const Rest *rest = (const Rest *)node;
        found = RestFindRestoRecipe(recipe, rest);
        if (found) {
          total_cost = total_cost + cost(recipe, rest);
          price_sum += (u16)found->price_per_pizza;
          ++restaurant_count;
        }
        node = node->next;
      }
      if (restaurant_count) {
        u32 average = total_cost / restaurant_count;
        total_cost = average;
        price_sum /= restaurant_count;
        (void)SetTempCanvasPosition(165, 63);
        DrawSupplierStockPrice(pt_signed_dword(average));
        (void)SetTempCanvasPosition(165, 72);
        DrawSupplierStockPrice(pt_signed_dword(price_sum));
        (void)SetTempCanvasPosition(175, 83);
        DrawNumberInsideListRight((u16)recipe->rating[6]);
        (void)SetTempCanvasPosition(165, 93);
        DrawSupplierStockPrice(pt_signed_dword((u32)(u16)recipe->rating[6] * (price_sum - average)));
      }
      (void)DrawText(50, 83, recipe_view_sales_text);
      (void)DrawText(50, 93, recipe_view_profit_text);
      (void)DrawText(89, 118, recipe_view_popularity_text);

      for (unsigned j = 0; j < 6; ++j) {
        temp_canvas->color = 0;
        cur_list_elem_x = (i16)recipe_chart_label_x[j / 3u];
        cur_list_elem_y = (i16)(15u * (j % 3u) + 130u);
        DrawBarChart(customer_age_names[j], recipe->age_type_rating[j], (i16)recipe_chart_x[j / 3u], cur_list_elem_y);
      }
    } else {
      SetCurrentFont(some_font_ptr);
      temp_canvas->color = 11;
      u8 ingredient_count = 0;

      for (unsigned j = 0; j < 4; ++j) {
        for (unsigned k = 0; k < num_ingredients_in_set[j]; ++k) {
          u16 amount = (u16)recipe->weight_of_type[24u * j + k];
          if (amount) {
            weight += amount;
            if (ingredient_count < 18) {
              (void)SetTempCanvasPosition(70, (i16)(6u * ingredient_count + 58u));
              DrawNumberInsideListRight(amount);
              (void)SetTempCanvasPositionX(72);
              (void)DrawTextInsideList(recipe_view_gram_text);
              (void)SetTempCanvasPositionX(82);
              (void)DrawTextInsideList(pt_ingredient_record(24u * j + k)->name);
            }
            ++ingredient_count;
          }
        }
      }
      (void)SetTempCanvasPosition(70, 50);
      DrawNumberInsideListRight(pt_signed_dword(weight));
      cur_list_elem_x = 72;
      (void)DrawListElement(recipe_view_total_weight_text);
      {
        char out[32];
        int n = snprintf(out, sizeof(out), "%d ingredients used", ingredient_count);

        memcpy(strbuf, out, (size_t)n + 1);
      }
      (void)SetTempCanvasPosition(77, 170);
      DrawStringBufferAsListElement();
    }
    PrintRecipeInfoBox(recipe, 0);
  }
}

// Recipe jury.

#include "character.h"
#include "files.h"
u32 ComparePizza_CompareIngredient(const RecipeEntry *a, const RecipeEntry *b) {
  const RecipeEntry *x = a, *y = b;
  u32 result = 100;
  if (x->ingredient == y->ingredient) {
    const Ingredient *ingredient = &ingredients[x->ingredient / 24].vegs[x->ingredient % 24];
    result = 0;
    if (ingredient->sprite_images[x->variation] != ingredient->sprite_images[y->variation])
      result = (x->variation >> 2) == (y->variation >> 2) ? 20u : 60u;
    u32 delta = (u32)abs((i32)x->x - y->x);
    result = (u16)result + (delta >> 1);
    delta = (u32)abs((i32)x->y - y->y);
    result = (u16)result + (delta >> 1);
  }
  return result;
}
u32 ComparePizzaRecipe(const Recipe *standard, const Recipe *address) {
  const Recipe *a = standard, *b = address;
  u32 total = 0;
  for (unsigned i = 0; i < a->num_entries; ++i) {
    u32 best = 100;
    for (unsigned j = 0; j < b->num_entries; ++j) {
      u32 score = ComparePizza_CompareIngredient(pt_recipe_entry(a, i), pt_recipe_entry(b, j));
      if ((u16)score < (u16)best) {
        best = score;
      }
    }
    total += best;
  }
  u32 result = 100;
  if (a->num_entries && b->num_entries) {
    unsigned count = a->num_entries < b->num_entries ? a->num_entries : b->num_entries;
    result = (u16)total / count;
    for (unsigned i = 0; i < 96; ++i) {
      if (a->weight_of_type[i] && !b->weight_of_type[i])
        result += 10u;
    }
    if ((u16)result > 100)
      result = 100;
  }
  return result;
}
u8 Return1(const Recipe *unused) {
  (void)unused;
  return 1;
}
u8 GetCharacterCookingSkill(const Recipe *unused, const Character *character) {
  (void)unused;
  return character->cooking;
}

void EvaluatePizzaRecipe_GraphicsHelper(i16 x, i16 y) {
  const i16 bottom = 106;
  if (y <= bottom) {
    FillRect(29, (u16)x, (u16)y, (u16)((i16)((u16)x + 36u)), (u16)(y + 35 <= bottom ? (i16)((u16)y + 35u) : bottom));
  }
  if (y + 35 <= bottom)
    FillRect(15, (u16)((i16)((u16)x + 15u)), (u16)((i16)((u16)y + 35u)), (u16)((i16)((u16)x + 19u)), (u16)bottom);
}
void EvaluateStandardPizza_Helper2(void) {
  Rect *rect = rect_1;
  rect->left = 0;
  rect->top = 0;
  rect->right = 319;
  rect->bottom = 106;
  FillRect1OfGraphicsRam();
  temp_canvas->bitmap_mask_flag = 1;
}
Recipe *AllocateRecipe(u32 entries) {
  u32 size = pt_recipe_allocation_bytes(entries);
  Recipe *result = pt_simulation_allocate(PT_SIM_RECIPE, size);
  if (!result)
    DisplayErrorMessage(0, recipe_allocation_failure_text);
  return result;
}
Recipe *PlayerInsertRecipe(Player *player, u8 recipe_id) {
  unsigned entries = recipe_lookup_fallback->num_entries;
  Recipe *result = AllocateRecipe(entries);
  if (result) {
    Recipe *created = result;
    (void)pt_recipe_copy(created, recipe_lookup_fallback, entries);
    Player *owner = player;
    (void)pt_insert_recipe_sorted(owner, created);
    memset(created->rating, 0, sizeof(created->rating));
    owner->num_recipes += 1;
    created->unknown_42[0] = current_player_index;
    if (recipe_id >= 50)
      memcpy(created->name, recipe_new_name, sizeof(recipe_new_name));
    last_recipe_ptr = result;
  }
  return result;
}
void EvaluatePizzaRecipe(const Recipe *standard, Recipe *address, u8 mode) {

  // Labelled original repair: initialize every score before animation.

  u16 scores[5] = {0};
  u16 total = 0;
  Recipe *recipe = address;
  u32 weight = RecipeGetWeight(recipe);
  recipe->imitation = 100;
  if (game_ptr->force_freestyle_jury)
    mode = 1;
  LoadBackground(recipe_jury_filename, old_bitmap);
  SetCurrentBitmap(old_bitmap);
  CommitBitmapAndSetPalette(12);
  EvaluateStandardPizza_Helper2();
  temp_canvas->color = 26;
  (void)SetTempCanvasPosition(30, 20);
  SetCurrentFont(font_piznumb);
  if (mode <= 2)
    (void)DrawTextInsideList(mode == 0 ? recipe_jury_compare_text
            : mode == 1                ? recipe_jury_evaluate_text
                                       : recipe_jury_competition_text);
  if (standard) {
    u32 difference = ComparePizzaRecipe(standard, address);
    recipe->imitation = (u8)(100u - (u8)difference);
    u8 remaining = recipe->imitation, divisor = 100;
    for (unsigned i = 0; i < 5; ++i) {
      u8 score = (u8)(20u * remaining / divisor);
      scores[i] = score;
      total = (u16)(total + score);
      remaining = (u8)(remaining - score);
      divisor = (u8)(divisor - 20);
    }
  }
  SetCurrentFont(font_astandar);
  temp_canvas->color = 0;
  SetSomeFieldInCanvas(0);
  ComputeRecipeStatistics(recipe);
  RecipeComputeAgeRatings(recipe);
  recipe->cooking_skill_of_char = GetCharacterCookingSkill(address, character_ptr);
  recipe->field_36 = Return1(address);
  if (mode == 1) {
    for (unsigned i = 0; i < 5; ++i) {
      i32 score = 0;
      u8 scaled_weight = (u8)(weight >> 7);
      switch (i) {
      case 0:
        score = recipe->salty + scaled_weight;
        if (recipe->num_unique_ingredients < 10)
          score -= (10 - recipe->num_unique_ingredients) >> 1;
        score -= recipe->fat >= 0 ? recipe->fat / 2 : -((-(i32)recipe->fat + 1) / 2);
        break;
      case 1:
        score = (recipe->appearance + recipe->refinement) / 10;
        if (recipe->num_entries < 28)
          score -= (28 - recipe->num_entries) >> 1;
        break;
      case 2:
        score = recipe->hot + (u8)((i8)recipe->amount_of_spices / 10);
        break;
      case 3: {
        u32 random = RandomNumber(1);
        score = (i16)((u16)(2u * (u16)recipe->sweet - random));
        break;
      }
      case 4:
        score = recipe->fat + scaled_weight;
        break;
      }
      score = (i16)((u16)score);
      if (score > 20)
        score = 20;
      if (score < 0)
        score = 0;
      i32 adjusted = MulDiv100(score * recipe->normality / 100, recipe->imitation);
      scores[i] = (u8)adjusted;
    }
  }
  for (unsigned i = 0; i < 5; ++i) {
    i16 x = (i16)recipe_jury_x[i];
    (void)BitBlt320Width(current_bitmap, (u16)x, 30, old_bitmap, 32, 30, 48, 80);
    i32 y = 110;
    // Scores are defined by the selected repaired release.

    i16 score = (i16)scores[i];
    while ((i16)((u16)y) > 80 - 2 * (i32)score) {
      WaitForRetrace();
      if ((i16)((u16)y) < 74)
        (void)BitBlt320Width(old_bitmap, 32, (u16)(y + 36), current_bitmap, (u16)x, (u16)(y + 36), 48, 1);
      EvaluatePizzaRecipe_GraphicsHelper(x, (i16)((u16)y));
      y = (u16)(y - 1);
    }
    (void)SetTempCanvasPosition((i16)((u16)x + 22u), (i16)((u16)y + 20u));
    DrawNumberInsideListRight(score);
    Delay(25);
  }
  FillSomeRectInGraphicsRam();
  // Original mode-one bug: computed jury scores never reached curr_score.
  // The labelled repair retains the hidden total and publishes its sum.

  if (mode == 1) {
    total = 0;
    for (unsigned i = 0; i < 5; ++i)
      total = (u16)(total + scores[i]);
    curr_score = (u8)total;
  }
  if (mode != 1) {
    FillRect(26, 110, 140, 200, 170);
    temp_canvas->color = 21;
    SetCurrentFont(font_ruby);
    DrawText2(110, 200, 151, recipe_jury_total_text);
    {
      char text[32];
      int n = snprintf(text, sizeof(text), "%d%%", (i16)total);
      if (n < 0 || n >= 32)
        abort();
      memcpy(strbuf, text, (size_t)n + 1);
    }
    DrawText2(110, 200, 166, strbuf);
    curr_score = (u8)total;
  }
  ReadMouseEvent();
  SetCurrentBitmap(old_bitmap);
  FillSomeRectWithColor(0);
  CommitBitmapAndSetPalette(18);
  PrintPizzaSalesOrRecipe(recipe, 0);
  CopyBitmap2to1();
  if (!mode && (i8)recipe->id < 14 && curr_score < 10) {
    {
      static const char expected[] = "You need a bit more practice baking the standard recipes. This recipe can be "
                                     "found on page %d of the great Pizza Tycoon cook book.";
      int n = snprintf((char *)strbuf, sizeof(strbuf), expected, ((i32)(i8)recipe->id + 9) / 2);
      if (n < 0 || (size_t)n >= sizeof(strbuf))
        abort();
    }
    ShowRecipeBookHint(strbuf);
  }
}

// Recipe list.

#include "trade.h"
#include "staff.h"
#include <fenv.h>

u32 GetSizeOfRecipe(const Recipe *recipe) {
  return pt_recipe_allocation_bytes(recipe->num_entries);
}
u8 GetLastStandardRecipe(const Player *player) {
  u8 candidate = 50;
  bool changed;
  do {
    changed = false;
    for (const Recipe *node = pt_player_first_recipe(player); node; node = pt_recipe_next(node)) {
      if ((i8)node->id == candidate) {
        changed = true;
        ++candidate;
      }
    }
  } while (changed);

  return candidate;
}
// Unsigned byte ordering with the original normalized -1/0/1 result.

i32 GuestStrcmp(const u8 *first, const u8 *second) {
  i32 result = 0;
  if (first != second) {
    while (*first && *first == *second) {
      ++first;
      ++second;
    }
    if (*first != *second)
      result = *first < *second ? -1 : 1;
  }
  return result;
}
u32 GenerateRecipeFileName(void) {
  u32 result;
  {
    int n = snprintf((char *)strbuf, sizeof(strbuf), "game\\r%d.pc", selected_pizza_load_idx);
    result = (u32)n;
  }
  return result;
}
i32 SetPricePerPizza(RestoRecipe *entry, double price) {
  if (price < 1.0)
    price = 1.0;
  if (price > recipe_price_limit)
    price = 50.0;
  volatile double product = price * recipe_price_scale;
  double integral = TruncateX87Integral(product);
  i32 result = integral >= INT32_MIN && integral <= INT32_MAX ? (i32)integral : INT32_MIN;

  entry->price_per_pizza = (i16)((u16)result);

  return result;
}
void RestClearPizzaQualitiesSum(Rest *restaurant, u8 index) {
  pt_menu_recipe_mutable(restaurant, index)->sum_of_qualities = 20;
}
PTActionResult RestInitNewPizza(Rest *rest, Recipe *recipe, double price) {
  u8 owner;
  RestoRecipe *record = &rest->recipes[rest->num_recipes];
  owner = rest->player_index;
  RecipeComputeAgeRatings(recipe);
  *record = (RestoRecipe){0};
  record->recipe = recipe;
  (void)SetPricePerPizza(record, price);
  record->last_price_per_pizza = record->price_per_pizza;
  record->sum_of_qualities = rest->pizza_quality;
  RestClearPizzaQualitiesSum(rest, rest->num_recipes);
  rest->num_recipes += 1;
  if ((i8)recipe->id < 14)
    rest->num_standard_recipes = (u8)((u32)rest->num_standard_recipes + (u32)1u);
  PTActionResult result = pt_action_object(game_ptr);
  if (owner < game_ptr->num_interactive_players) {
    for (unsigned kind = 0; kind < 4; ++kind) {
      for (unsigned item = 0; item < num_ingredients_in_set[kind]; ++item) {
        unsigned index = 24u * kind + item;
        if (recipe->weight_of_type[index] && !rest->stock[index].need)
          rest->stock[index].need = 1;
      }
      result = pt_action_value(kind);
    }
  }

  return result;
}

void DrawDepotPriorityEntry(i16 x, i16 y, i16 width, const u8 *name, u8 selected) {
  i16 remaining = width, left = x;
  if (width > 88) {
    (void)BitBlt(&bmp17, 0, 101, current_bitmap, (u16)(i16)(x), (u16)(i16)(y), 88, 7);
    left = (i16)((u16)x + 88u);
    remaining = (i16)((u16)width - 88u);
  }
  (void)BitBlt(&bmp17, 0, 101, current_bitmap, (u16)(i16)(left), (u16)(i16)(y), (u16)(i16)(remaining), 7);
  temp_canvas->color = (u8)(selected ? 10 : 15);
  (void)SetTempCanvasPosition((i16)((u16)x - 1u), (i16)((u16)y + 7u));
  (void)DefaultLineTo((u16)x - 1u, (u16)y - 1u);
  (void)RelativeLineTo((u16)width, 0);
  temp_canvas->color = (u8)(selected ? 15 : 10);
  (void)SetTempCanvasPosition(x, (i16)((u16)y + 7u));
  (void)RelativeLineTo((u16)width - 1u, 0);
  temp_canvas->color = 0;
  (void)SetTempCanvasPosition((i16)((u16)x + 2u), (i16)((u16)y + 5u));
  SetCurrentFont(some_font_ptr);
  (void)DrawTextInsideList(name);
}
static u8 count_restaurants(const Player *player, const Recipe *recipe, u8 standard_id, bool standard) {
  u8 count = 0;

  for (const Rest *rest = pt_player_first_restaurant(player); rest; rest = pt_restaurant_next(rest)) {
    for (unsigned i = 0; i < rest->num_recipes; ++i) {
      const Recipe *entry = pt_menu_recipe(rest, i);

      if (standard ? entry->id == standard_id : entry == recipe) {
        ++count;
        break;
      }
    }
  }
  return count;
}

void DrawRecipeList(const Player *player, u8 page, u8 selected, u8 custom_count) {
  u8 restaurant_count;
  FillLetterBackground(4, 4, 18);
  SetCurrentFont(font_astandar);
  temp_canvas->color = 6;
  SetSomeFieldInCanvas(0);
  (void)DrawText(68, 40, recipe_list_title);
  SetCurrentFont(some_font_ptr);
  (void)SetTempCanvasPosition(146, 40);
  {
    int n = snprintf((char *)strbuf, sizeof(strbuf), "page %d", page + 1u);
    (void)n;
  }
  DrawStringBufferAsListElement();
  temp_canvas->color = 0;

  Recipe *first = pt_player_first_recipe(player);

  if (!page) {
    for (u8 row = 0; row < 14; ++row) {
      restaurant_count = 0;
      restaurant_count = count_restaurants(player, NULL, row, true);

      const u8 *name;
      name = GetDefaultRecipeName(row);
      DrawDepotPriorityEntry(53, (i16)(9u * row + 48u), 100, name, selected == row);
      for (Recipe *node = first; node; node = pt_recipe_next(node))
        if (node->id == row) {
          name = itoa_(restaurant_count);
          DrawDepotPriorityEntry(158, (i16)(9u * row + 48u), 12, name, 0);
        }
    }
  } else {
    u8 row = 0;
    for (Recipe *node = first; node; node = pt_recipe_next(node)) {
      const Recipe *recipe = node;
      i32 id = (i8)recipe->id;
      if (id <= 14 * ((i32)page - 1) + 49 || id >= 14 * (i32)page + 50)
        continue;
      restaurant_count = 0;
      restaurant_count = count_restaurants(player, node, 0, false);

      i16 y = (i16)((u16)(9u * row + 48u));
      DrawDepotPriorityEntry(53, y, 100, node->name, (i32)selected + 36 == id);
      const u8 *name;
      name = itoa_(restaurant_count);
      DrawDepotPriorityEntry(158, y, 13, name, 0);
      ++row;
    }
    if (row < 14)
      DrawDepotPriorityEntry(53, (i16)(9u * row + 48u), 100, recipe_list_new, (i32)selected - 14 == custom_count);
  }
}
// Recipe selection, editing and restaurant-menu actions.

typedef struct RecipeListMenuState {
  u8 expected_tag[5], file_tag[5];
  u32 stored_checksum, file_size, checksum;
  Recipe *replaced_recipe;
  u8 custom_count, details, info, page, selected, id, rows;
} RecipeListMenuState;

u8 HandleRecipeListMenu(Rest *restaurant) {
  RecipeListMenuState state;
  Recipe *found = NULL;
  state.details = 0;
  state.custom_count = 0;
  state.page = 0;
  state.info = 0;
recount:

  for (Recipe *node = pt_player_first_recipe(player_ptr); node; node = pt_recipe_next(node))
    if ((i8)node->id > 49)
      ++state.custom_count;
  state.id = (u8)(state.custom_count + 50u);
  found = 0;
toolbar:
  Do3x3ButtonStuff(20);
redraw:
  state.replaced_recipe = 0;
  u8 advance = RandomChance(20);
  if (advance)
    AdvanceClock(1);
  state.custom_count = 0;

  for (Recipe *node = pt_player_first_recipe(player_ptr); node; node = pt_recipe_next(node)) {
    i32 recipe_id = (i8)node->id;
    if (recipe_id > 49)
      ++state.custom_count;
    if (!found && recipe_id == state.id)
      found = node;
  }
  if (found)
    state.id = found->id;
  state.selected = state.id < 14 ? state.id : (u8)(state.id - 36u);
  u8 total = (u8)(state.custom_count + 15u);
  state.rows = 14u * ((unsigned)state.page + 1u) <= total ? 14 : total % 14u;
  if (found)
    PrintRecipeInfoBox(found, player_ptr->num_recipes);
  else
    PrintGenericInfoBox();
  if (!state.details)
    DrawRecipeList(player_ptr, state.page, state.selected, state.custom_count);
  else if (state.details == 1)
    PrintPizzaSalesOrRecipe(found, state.info);
  else if (state.details == 2)
    StartPizzaCompetition(found);
wait:;
  u16 event = WaitForMouseClick();
  mouse_pressed = event;
  if (event == 0x0f09) {
    restaurant = SelectNextRestaurant(restaurant);
    goto toolbar;
  }
  if ((event == 0x2e63 || event == 0x2e43) && found)
    ComputeRecipeWeights(found);
  if (event == 0x3b00) {
    unit_for_number_query = recipe_number_unit;
    selected_pizza_load_idx = (u8)ShowAskForNumberQuery(recipe_load_prompt, selected_pizza_load_idx, 1, 0, 100);
    memcpy(state.expected_tag, recipe_load_tag, 5);
    (void)GenerateRecipeFileName();
    u32 opened;
    opened = OpenFileAsCurrent(strbuf);
    if (!(u16)opened)
      goto wait;

    ReadBytesFromCurrentFile(state.file_tag, 4);
    state.file_tag[4] = 0;
    i32 compared = GuestStrcmp(state.file_tag, state.expected_tag);
    if (compared) {
      // Original leaves the input stream open on a bad PREZ tag.

      DisplayErrorMessage(0, recipe_load_custom_error);
      goto wait;
    }
    ReadBytesFromCurrentFile(&state.stored_checksum, 4);
    ReadBytesFromCurrentFile(&state.file_size, 4);

    u32 size = state.file_size;
    Recipe *loaded;
    loaded = pt_recipe_allocate_file(size);
    if (loaded) {
      pt_recipe_read_file_bytes(loaded, size, ReadBytesFromCurrentFile);
      u32 checksum = pt_recipe_file_checksum(loaded, size, GetSumOfBytes);
      // The DOS loader trusts raw file lengths. Stop verification before
      // malformed records can cause reads outside their allocation.

      for (Recipe *node = pt_player_first_recipe(player_ptr); node; node = pt_recipe_next(node)) {
        compared = GuestStrcmp(loaded->name, node->name);
        if (!compared) {
          DisplayErrorMessage(0, recipe_duplicate_error);
          FreeRecipe(loaded);
          goto close_recipe;
        }
      }
      ComputeRecipeWeights(loaded);
      loaded->id = GetLastStandardRecipe(player_ptr);
      LinkList_InsertSorted(&player_ptr->recipes, &loaded->linked, PT_LIST_RECIPE_ID);
      player_ptr->num_recipes += 1;
      if (checksum != state.stored_checksum)
        DisplayErrorMessage(0, recipe_checksum_error);
    }
  close_recipe:
    CloseCurrentFile();
    goto recount;
  }
  if (event == 0x3c00 && found) {
    if ((i8)found->id < 14) {
      DisplayErrorMessage(0, recipe_list_save_custom_error);
      goto wait;
    }
    u32 size, checksum;
    size = GetSizeOfRecipe(found);
    checksum = pt_recipe_saved_checksum(found, size, GetSumOfBytes);
    state.checksum = checksum;
    unit_for_number_query = recipe_number_unit;
    selected_pizza_load_idx = (u8)ShowAskForNumberQuery(recipe_save_prompt, selected_pizza_load_idx, 1, 0, 100);
    (void)GenerateRecipeFileName();
    u32 opened;
    opened = OpenFileForWritingAsCurrent(strbuf);
    if ((u16)opened) {
      WriteBytesToCurrentFile(recipe_save_tag, 4);

      WriteBytesToCurrentFile(&state.checksum, 4);

      SaveRecipeRecord(found);
      CloseCurrentWritingFile();
    }
    event = mouse_pressed;
  }
  if (event == 1) {
    if (state.details)
      goto wait;
    bool inside;
    inside = IsMouseInside(50, 48, 150, (i16)((u16)(9u * state.rows + 48u)));
    if (!inside)
      goto wait;
    found = 0;
    i32 row = GetSelectedListItem(48, 9);
    state.selected = (u8)(14u * state.page + (u8)row);
    state.id = state.selected < 14 ? state.selected : (u8)(state.selected + 36u);
    goto redraw;
  }
  if (!pt_is_menu_button_event(event))
    goto wait;
  switch (event) {
  case PT_MENU_BUTTON_1:
    if (!found)
      goto wait;
    state.details = 2;
    goto redraw;
  case PT_MENU_BUTTON_2:
  case PT_MENU_BUTTON_3:
    if (!found)
      goto wait;
    state.details = 1;
    state.info = (u8)(event - 241u);
    goto redraw;
  case PT_MENU_BUTTON_4:
    if (!state.details) {
      if (14u * state.page <= state.custom_count)
        ++state.page;
      goto redraw;
    }
    {
      Recipe *next = (Recipe *)found->linked.next;

      if (!next->linked.next)
        goto wait;

      found = next;
    }
    state.id = found->id;
    goto redraw;
  case PT_MENU_BUTTON_5:
    state.details = 0;
    if (found)
      state.id = found->id;
    goto redraw;
  case PT_MENU_BUTTON_6: {
    u8 *error = NULL;
    if (!found)
      error = recipe_list_create_first_error;
    else {
      const Rest *rest = restaurant;

      for (unsigned i = 0; i < rest->num_recipes; ++i)
        if (rest->recipes[i].recipe == found) {
          error = recipe_list_already_menu_error;
          break;
        }
      if (!error && rest->num_recipes >= 24)
        error = recipe_list_menu_full_error;
    }
    if (!error) {
      AskForPizzaPrice(recipe_price_prompt, strbuf, 5);
      double price = GuestStrtod(strbuf, NULL);
      RestInitNewPizza(restaurant, found, price);
      goto redraw;
    }
    CopyBitmap2to1();
    DisplayErrorMessage(0, error);
    goto redraw;
  }
  case PT_MENU_BUTTON_7:
    if (!state.details) {
      if (state.page)
        --state.page;
      goto redraw;
    }
    if (found == (Recipe *)(player_ptr->recipes).begin.next)
      goto wait;
    found = (Recipe *)found->linked.previous;

    state.id = found->id;
    goto redraw;
  case PT_MENU_BUTTON_8: {
    if (found) {
      CopyBitmap2to1();
      u8 confirmed = ShowMessage2(0, recipe_list_recreate_prompt);
      if (!confirmed)
        goto redraw;
      state.replaced_recipe = found;
      LinkList_Remove(&found->linked);
      player_ptr->num_recipes -= 1;
    }
    PTPizzaEditorResult created;
    if (state.id < 50) {
      Recipe *standard;
      standard = AllocateRecipe(200);
      LoadRecipeFromFile(state.id, standard);
      created = CreatePizza(restaurant, standard, 0, state.id);
      FreeRecipe(standard);
    } else {
      created = CreatePizza(restaurant, NULL, 1, state.id);
      RestLoadMusic(restaurant, 1);
    }
    Recipe *old = state.replaced_recipe;
    if (old) {
      if (created.recipe) {
        u32 size = GetSizeOfRecipe(old);
        found = last_recipe_ptr;

        for (Rest *node = pt_player_first_restaurant(player_ptr); node; node = pt_restaurant_next(node)) {
          Rest *rest = node;

          for (unsigned i = 0; i < rest->num_recipes; ++i)
            if (rest->recipes[i].recipe == old)
              rest->recipes[i].recipe = found;
        }
        pt_simulation_release(old, size);
      } else {
        LinkList_InsertSorted(&player_ptr->recipes, &old->linked, PT_LIST_RECIPE_ID);
        player_ptr->num_recipes += 1;
      }
    }
    Do3x3ButtonStuff(20);
    CheckGoalStuff();
    if (!has_many_players)
      goto toolbar;

    return 255;
  }
  case PT_MENU_BUTTON_9: {
    return CheckGoalStuff();
  }
  }

  return 255;
}

// Pizza menu.

#include "frame.h"

RestoRecipe *RestRemovePizza(Rest *restaurant, u8 index) {
  Rest *rest = restaurant;
  if (!rest || rest->num_recipes > sizeof(rest->recipes) / sizeof(rest->recipes[0]) || index >= rest->num_recipes)
    pt_resource_bounds_failure("restaurant menu removal", index, rest ? rest->num_recipes : 0);
  const Recipe *recipe = pt_menu_recipe(rest, index);
  if ((i8)recipe->id < 14)
    rest->num_standard_recipes = (u8)((u32)rest->num_standard_recipes - (u32)1u);
  // Original off-by-one repair: shift the remaining entries only.
  // The inactive final entry stays intact, including its native pointer.

  RestoRecipe *result;
  result = memmove(&rest->recipes[index],
      &rest->recipes[(unsigned)index + 1u],
      (rest->num_recipes - index - 1u) * sizeof(rest->recipes[0]));

  rest->num_recipes -= 1;

  return result;
}
u8 RestUpdateFurnitureRating(Rest *restaurant) {
  Rest *rest = restaurant;
  u8 value = 0;
  for (unsigned age = 0; age < 6; ++age) {
    value = RestComputeBeautiness(rest, (u8)age);
    rest->furniture_rating[age] = value;
  }

  return value;
}
Canvas *DrawTextAndAdvanceLine(const u8 *text_address) {
  Canvas *canvas = temp_canvas;
  u16 count, width;
  count = (u16)pt_strlen(text_address);
  (void)Canvas_DrawText(canvas, pt_text_span(text_address, count));
  count = (u16)pt_strlen(text_address);
  width = Canvas_TextWidth(canvas, pt_text_span(text_address, count));
  canvas->position_x = (i16)((u16)canvas->position_x - width);
  canvas->position_y = (i16)((u16)canvas->position_y + (u16)canvas->font_height + 1u);

  return temp_canvas;
}

PTCanvasResult DrawCustomerRatings(Rest *restaurant, u8 menu) {
  LoadBlitmapAndOffsets(2);
  SelectPalette(7);
  temp_canvas->color = 12;
  SetCurrentFont(font_ruby);
  (void)DrawText2(12, 203, 30, (menu ? customer_ratings_menu_title : customer_ratings_furniture_title));
  (void)DrawText2(12, 203, 42, customer_ratings_title);
  (void)RestUpdateFurnitureRating(restaurant);
  for (unsigned age = 0; age < 6; ++age)
    (void)DrawBitmapStuff1((u8)age, (i16)(26u * age + 31u), 153);
  SetCurrentFont(some_font_ptr);
  PTCanvasResult result = pt_canvas_result_value(0);
  const Rest *rest = restaurant;
  for (unsigned age = 0; age < 6; ++age) {
    unsigned value = menu ? rest->menu_rating[age] : rest->furniture_rating[age];

    result = pt_canvas_result_value(value);
    if (!value)
      continue;
    i16 x = (i16)(26u * age + 35u);
    int bottom = 139 - (int)(value / 2u);
    if (bottom <= 66)
      bottom = 66;
    u8 y = 139;
    while (y > bottom) {
      (void)DrawBitmapStuff1(29, x, y);
      --y;
    }
    (void)SetTempCanvasPosition(x + 15, (i16)(y - 1));
    DrawNumberInsideListRight((i32)value);
    result = pt_canvas_result_object(temp_canvas);
  }

  return result;
}
void DrawPizzaMenu(Rest *restaurant, u8 scroll, u8 mode, i8 selected) {
  u8 half = scroll / 2u, page = scroll % 2u;

  u8 background_color = mode || half ? 9 : 2, palette = mode || half ? 5 : 19;
  (void)FillLetterBackground(4, background_color, palette);
  if (mode == 2) {
    (void)DrawCustomerRatings(restaurant, 1);

    return;
  }
  FillRect(162, 100, 167, 120, 174);
  SetSomeFieldInCanvas(0);
  SetCurrentFont(font_piznumb);
  temp_canvas->color = 0;
  (void)snprintf((char *)strbuf, sizeof(strbuf), "-%d-", page + 1);
  (void)DrawText(100, 173, strbuf);
  SetCurrentFont(font_ruby);
  temp_canvas->color = 8;
  (void)SetTempCanvasPosition(51, 35);
  if (mode)
    (void)DrawText(33, 33, pizza_menu_sales_title);
  else
    (void)DrawTextAndAdvanceLine(pizza_menu_title);
  SetCurrentFont(some_font_ptr);
  temp_canvas->color = 6;
  const Rest *rest = restaurant;
  if (mode) {
    SetCurrentFont(font_supersma);
    (void)SetTempCanvasPosition(155, 172);
    DrawNumberInsideListRight((u16)rest->pizza_count_per_day[6]);
    (void)SetTempCanvasPosition(182, 172);

    DrawRightAlignedFloat((float)(u16)rest->sales_per_day[6]);
    (void)DrawText(140, 40, pizza_menu_sales_label);
    (void)SetTempCanvasPosition(168, 35);
    DrawListElement(pizza_menu_turnover_label);
  }

  for (unsigned row = 0; row < 14; ++row) {
    i16 y = (i16)(8u * row + 48u);
    if (half) {
      SetCurrentFont(font_supersma);
      (void)SetTempCanvasPosition(30, y);
      DrawNumberInsideListRight((i32)row);
      for (unsigned age = 0; age < 6; ++age) {
        (void)SetTempCanvasPositionX((i16)(16u * age + 50u));
        DrawNumberInsideListRight(RestComputePizzaRating(restaurant, pt_menu_recipe_row(restaurant, row), (u8)age));
      }
    } else {
      unsigned index = 14u * page + row;
      if (index >= rest->num_recipes)
        break;
      const RestoRecipe *entry = pt_menu_recipe_row(restaurant, index);
      if (!mode && selected == (int)row)
        FillRect(14, 38, (i16)(8u * row + 42u), 144, (i16)(8u * row + 49u));
      (void)SetTempCanvasPosition(49, y);
      if (mode)
        (void)SetTempCanvasPositionX(30);
      DrawNumberInsideListRight((i32)index + 1);
      (void)SetTempCanvasPositionX(mode ? 37 : 54);

      (void)DrawTextInsideList(entry->recipe->name);
      if (mode) {
        (void)SetTempCanvasPositionX(128);
        DrawSupplierStockPrice((u16)entry->price_per_pizza);
        (void)SetTempCanvasPositionX(155);
        DrawNumberInsideListRight(entry->sales);
        (void)SetTempCanvasPositionX(182);
        DrawSupplierStockPrice((u16)entry->price_per_pizza * entry->sales);
      } else {
        (void)SetTempCanvasPositionX(168);
        DrawSupplierStockPrice((u16)entry->price_per_pizza);
      }
    }
  }
}
u32 ShowPizzaInfoInBox(Rest *restaurant, const RestoRecipe *row) {
  const RestoRecipe *entry = row;

  FillGenericInfoBox();
  (void)DrawInfoBoxText(pizza_menu_popularity_label);
  (void)DrawInfoBoxText(entry->recipe->name);
  for (unsigned age = 0; age < 6; ++age) {
    u8 value = RestComputePizzaRating(restaurant, row, (u8)age);
    (void)DrawFlyerOutput(customer_age_names[age], value);
  }
  (void)DrawInfoBoxText(pizza_menu_info_separator);
  (void)DrawFlyerOutput(pizza_menu_demand_label, entry->orders);
  return DrawFlyerOutput(pizza_menu_info_sales_label, entry->sales);
}
static i16 rows_bottom(Rest *restaurant, u8 scroll) {
  i32 rows = (i32)restaurant->num_recipes - 14 * (i32)scroll;
  if (rows >= 14)
    rows = 14;
  return (i16)((u16)(8 * rows + 42));
}
u32 HandlePizzaMenu(Rest *restaurant) {
  u8 scroll, mode, selected, price_row;

  scroll = 0;
  mode = 0;
toolbar:

  did_terminal_stuff = 0;
  Do3x3ButtonStuff(7);
redraw:
  selected = 255;
draw:
  DrawPizzaMenu(restaurant, scroll, mode, (i8)selected);
wait:;
  u16 event;
  event = WaitForMouseClick();
  mouse_pressed = event;
  if (event == 0x0f09) {
    restaurant = SelectNextRestaurant(restaurant);
    goto toolbar;
  }
  if (did_terminal_stuff) {
    restaurant = ptr_apple->restaurant;
    goto toolbar;
  }
  if (event == 1) {
    if (mode)
      goto wait;
    bool hit;
    hit = IsMouseInside(148, 42, 180, rows_bottom(restaurant, scroll));
    if (hit) {
      i32 row;
      row = GetSelectedListItem(42, 8);
      price_row = (u8)row;
      FillRect(15, 148, (i16)(8u * price_row + 42u), 180, (i16)(8u * price_row + 49u));
      temp_canvas->color = 6;
      (void)SetTempCanvasPosition(155, (i16)(8u * price_row + 48u));
      temp_canvas->background_color = 15;
      HandleTextEditing(strbuf, 5);
      double value;
      value = GuestStrtod(strbuf, NULL);

      (void)SetPricePerPizza(pt_menu_recipe_mutable(restaurant, 14u * scroll + price_row), value);
      goto draw;
    }
    hit = IsMouseInside(40, 42, 129, rows_bottom(restaurant, scroll));
    if (!hit)
      goto wait;
    i32 row;
    row = GetSelectedListItem(42, 8);
    selected = (u8)row;
    (void)ShowPizzaInfoInBox(restaurant, pt_menu_recipe_row(restaurant, (u32)(14 * (i32)scroll + (i8)selected)));
    goto draw;
  }
  if (!pt_is_menu_button_event(event))
    goto wait;
  switch (event) {
  case PT_MENU_BUTTON_1:
    if (scroll < 1)
      ++scroll;
    goto redraw;
  case PT_MENU_BUTTON_2:
    mode = 0;
    goto redraw;
  case PT_MENU_BUTTON_3:
    if (scroll)
      --scroll;
    goto redraw;
  case PT_MENU_BUTTON_4:
    mode = 1;
    goto redraw;
  case PT_MENU_BUTTON_5:
    mode = 2;
    goto redraw;
  case PT_MENU_BUTTON_6:
    if ((i8)selected < 0) {
      DisplayErrorMessage(0, pizza_menu_remove_selection_error);
      did_terminal_stuff = 0;
      goto redraw;
    }
    if (restaurant->num_recipes <= 1) {
      ShowMessage(16, pizza_menu_keep_one_error);
      goto redraw;
    }
    (void)RestRemovePizza(restaurant, (u8)(14u * scroll + selected));
    goto redraw;
  case PT_MENU_BUTTON_7:

    return 6;
  default:
    goto redraw;
  }
}

// Price parser.

#include "render_types.h"
#include "memory.h"

#define SIGN64 UINT64_C(0x8000000000000000)
#define INF64 UINT64_C(0x7ff0000000000000)
#define FRAC64 UINT64_C(0x000fffffffffffff)

// Watcom's integer normalization rounds a discarded half upward. Its carry accumulator
// initially contains low|high, as in the stock code.

u64 IntegerDoubleBits(u64 value, u32 exponent_bits) {
  u32 low = (u32)value, high = (u32)(value >> 32), discarded = low | high;
  if (!discarded)
    return 0;
  if (!(high & UINT32_C(0xfff00000))) {
    do {
      exponent_bits -= UINT32_C(0x100000);
      value <<= 1;
      high = (u32)(value >> 32);
    } while (!(high & UINT32_C(0xfff00000)));
  } else if (high & UINT32_C(0xffe00000)) {
    do {
      exponent_bits += UINT32_C(0x100000);
      discarded = (discarded >> 1) | ((u32)(value & 1u) << 31);
      value >>= 1;
      high = (u32)(value >> 32);
    } while (high & UINT32_C(0xffe00000));
    if (discarded & UINT32_C(0x80000000))
      ++value;
  }
  // Original bug: rounding can carry into bit 53; masking it without
  // raising the exponent halves the result. Keep the repaired carry.

  if (value & (UINT64_C(1) << 53))
    exponent_bits += UINT32_C(0x100000);
  return (value & FRAC64) | ((u64)exponent_bits << 32);
}
void DecimalDigitsToDouble(const u8 *digits, u64 *output) {
  u64 value = 0;
  for (const u8 *p = digits; *p; ++p)
    value = value * 10u + (*p & 15u);

  *output = IntegerDoubleBits(value, UINT32_C(0x43300000));
}

// A 53-bit x87 operation keeps the extended exponent range until its FSTP.
// Binary64 host arithmetic can round subnormal results in one step instead.
// Preserve the two original rounding steps with bounded unsigned integers.

typedef struct Binary53 {
  u64 mantissa;
  int exponent;
} Binary53;
static Binary53 unpack(u64 bits) {
  Binary53 a = {bits & FRAC64, (int)((bits >> 52) & 2047) - 1023};
  if ((bits & INF64) == 0) {
    a.exponent = -1022;
    while (a.mantissa && a.mantissa < (UINT64_C(1) << 52)) {
      a.mantissa <<= 1;
      --a.exponent;
    }
  } else
    a.mantissa |= UINT64_C(1) << 52;
  return a;
}
static u64 rounded_shift(u64 value, unsigned shift) {
  if (!shift)
    return value;
  if (shift > 63)
    return 0;
  u64 result = value >> shift, remainder = value & ((UINT64_C(1) << shift) - 1), half = UINT64_C(1) << (shift - 1);
  return result + (remainder > half || (remainder == half && (result & 1u)));
}
static u64 store_binary64(Binary53 a, u64 sign) {
  if (a.mantissa == (UINT64_C(1) << 53)) {
    a.mantissa >>= 1;
    ++a.exponent;
  }
  if (a.exponent > 1023)
    return sign | INF64;
  if (a.exponent < -1022)
    return sign | rounded_shift(a.mantissa, (unsigned)(-1022 - a.exponent));
  return sign | ((u64)(a.exponent + 1023) << 52) | (a.mantissa & FRAC64);
}
static u64 arithmetic53(u64 left, u64 right, bool divide) {
  u64 sign = (left ^ right) & SIGN64;
  if (!(left & ~SIGN64))
    return sign;
  Binary53 a = unpack(left), b = unpack(right), out;
  if (divide) {
    unsigned shift = a.mantissa < b.mantissa ? 53 : 52;
    out.exponent = a.exponent - b.exponent - (shift == 53);
    __uint128_t numerator = (__uint128_t)a.mantissa << shift;
    out.mantissa = (u64)(numerator / b.mantissa);
    u64 remainder = (u64)(numerator % b.mantissa);
    out.mantissa += remainder * 2 > b.mantissa || (remainder * 2 == b.mantissa && (out.mantissa & 1u));
  } else {
    __uint128_t product = (__uint128_t)a.mantissa * b.mantissa;
    unsigned shift = product >= ((__uint128_t)1 << 105) ? 53 : 52;
    out.exponent = a.exponent + b.exponent + (shift == 53);
    out.mantissa = (u64)(product >> shift);
    u64 remainder = (u64)(product & (((__uint128_t)1 << shift) - 1)), half = UINT64_C(1) << (shift - 1);
    out.mantissa += remainder > half || (remainder == half && (out.mantissa & 1u));
  }
  return store_binary64(out, sign);
}
// Preserve the original 53-bit intermediate rounding and binary64 stores.

u64 Scale10V(u64 value, i32 exponent) {
  if (exponent < -325 || exponent > 308)
    abort();
  if ((value & ~SIGN64) >= INF64)
    value = (value & ~SIGN64) > INF64 || !(value & SIGN64) ? scale10_positive_max : scale10_negative_max;
  unsigned magnitude = exponent < 0 ? 0u - (u32)exponent : (u32)exponent;
  if (magnitude > 308) {
    value = arithmetic53(value, scale10_powers[0].factor_bits, exponent < 0);
    magnitude -= 216;
  }
  u64 factor = UINT64_C(0x3ff0000000000000);
  for (unsigned i = 0;;) {
    u16 power = (u16)scale10_powers[i].exponent;
    if (magnitude >= power) {
      factor = arithmetic53(factor, scale10_powers[i].factor_bits, false);
      magnitude -= power;
    }
    if (!magnitude)
      break;
    if (power != 1)
      ++i;
  }
  value = arithmetic53(value, factor, exponent < 0);
  return value;
}

u32 *GuestSetERange(void) {
  return GuestSetErrno(14);
}

// Significant digits and flags are native locals; the end pointer is native too.

double GuestStrtod(const u8 *text, const u8 **end_pointer) {
  const u8 *p = text, *end = text;
  u8 digits[18];
  while (*p == 32 || (*p >= 9 && *p <= 13))
    ++p;
  u8 flags;
  flags = 0;
  if (*p == '+')
    ++p;
  else if (*p == '-') {
    ++p;
    flags = 1;
  }
  u8 significant = '0', c;
  u32 count = 0, fractional = 0;
  for (;;) {
    c = *p++;
    if (c == '.') {
      if (flags & 8u)
        break;
      flags |= 8u;
      continue;
    }
    if (c < '0' || c > '9')
      break;
    if (flags & 8u)
      ++fractional;
    significant |= c;
    if (significant != '0') {
      if (count < 17)
        digits[count] = c;
      ++count;
    }
    flags |= 4u;
  }
  i32 exponent = 0;
  if (flags & 4u) {
    if (c == 'e' || c == 'E' || c == 'd' || c == 'D') {
      const u8 *exponent_start = p - 1;
      if (*p == '+' || *p == '-') {
        if (*p == '-')
          flags |= 2u;
        ++p;
      }
      flags &= (u8)~4u;
      while (*p >= '0' && *p <= '9') {
        if (exponent < 1000)
          exponent = exponent * 10 + *p - '0';
        ++p;
        flags |= 4u;
      }
      if (flags & 2u)
        exponent = -exponent;
      if (!(flags & 4u))
        p = exponent_start;
    } else
      --p;
    end = p;
  }
  exponent -= (i32)fractional;
  if (count > 17) {
    exponent += (i32)(count - 17);
    count = 17;
  }
  while (count && digits[count - 1] == '0') {
    ++exponent;
    --count;
  }
  u64 bits = 0;
  if (count) {
    digits[count] = 0;
    DecimalDigitsToDouble(digits, &bits);
    if (exponent > 308) {
      GuestSetERange();
      bits = price_parser_positive_infinity | ((flags & 1u) ? SIGN64 : 0);
    } else if (exponent < -325) {
      GuestSetERange();
      bits = 0;
    } else {
      if (exponent)
        bits = Scale10V(bits, exponent);
      if (flags & 1u)
        bits ^= SIGN64;
    }
  }
  if (end_pointer)
    *end_pointer = end;
  return pt_double_from_bits(bits);
}
