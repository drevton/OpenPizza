// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
// Supplier day.

#include "objects.h"
#include "game.h"
#include "trade.h"
#include <math.h>
#pragma STDC FP_CONTRACT OFF

void SupplierReduceBoughtByPlayer(void) {
  for (Supplier *supplier = (Supplier *)current_town->suppliers.begin.next; supplier->link.next;
      supplier = (Supplier *)supplier->link.next) {
    for (unsigned i = 0; i < game_ptr->num_interactive_players; ++i) {
      u32 amount = (u32)supplier->total_bought_by_player[i];
      u32 reduction = (u32)supplier->bonus_levels;
      supplier->total_bought_by_player[i] = pt_signed_dword(amount > reduction ? amount - reduction : 0);
    }
  }
}

void SupplierDecreaseItemsLeft(Supplier *supplier, u8 ingredient, u16 decrease) {
  u16 left = (u16)supplier->num_items_left[ingredient];
  if (left > decrease)
    supplier->num_items_left[ingredient] = (i16)((u16)(left - decrease));
}

void RefillSupplierItems(u8 stock_kind) {
  for (Supplier *supplier = (Supplier *)current_town->suppliers.begin.next; supplier->link.next;
      supplier = (Supplier *)supplier->link.next) {
    if (supplier->stock_kind != stock_kind)
      continue;
    u8 index = 0;
    while (index < pt_supplier_ingredient_count(supplier->stock_kind)) {
      unsigned i = index;
      if (supplier->price_addon[i] && (u16)supplier->num_items_left[i] < 10000) {
        u32 amount = RandomNumber(8000) + 4000;
        supplier->num_items_left[i] = (i16)((u16)((u16)supplier->num_items_left[i] + amount));
      }
      ++index;
    }
  }
}

void RunSuppliersDailyLoop(void) {
  if (++suppliers_daily_loop_index > 3)
    suppliers_daily_loop_index = 0;
  for (Supplier *supplier = (Supplier *)current_town->suppliers.begin.next; supplier->link.next;
      supplier = (Supplier *)supplier->link.next) {
    for (unsigned i = 0; i < 8; ++i) {
      if (supplier->had_shortages[i])
        supplier->had_shortages[i] = (u8)((u32)(supplier->had_shortages[i]) - (u32)1u);
      if (supplier->unpaid_bill_level[i])
        supplier->unpaid_bill_level[i] = (u8)((u32)(supplier->unpaid_bill_level[i]) - (u32)1u);
    }
    unsigned kind = supplier->stock_kind, count = pt_supplier_ingredient_count((u8)kind);
    for (unsigned i = 0; i < count; ++i) {
      u8 ingredient = (u8)(24u * kind + i);
      if (!supplier->price_addon[i])
        continue;
      int trend = pt_supplier_ingredient_trend(ingredient);
      SupplierDecreaseItemsLeft(supplier, (u8)i, (u16)(5 * (trend + 130)));
      if (kind != suppliers_daily_loop_index)
        continue;
      if (!(u16)supplier->num_items_left[i])
        supplier->num_items_left[i] = 1;
      volatile double ratio = supplier_price_scale / (u16)supplier->num_items_left[i];
      volatile double root = sqrt(ratio);
      volatile double base = root * supplier->base_price_addon;
      trend = pt_supplier_ingredient_trend(ingredient);
      volatile double influence = trend * 0.25;
      volatile double price = base + influence;
      float rounded = (float)price;
      if (price < 20.)
        rounded = 20.f;
      if (rounded > 250.f)
        rounded = 250.f;
      u32 integer = (u32)rounded;
      supplier->price_addon[i] = (u8)integer;
    }
  }
}

// Stock menu.

#include "arithmetic.h"
#include "calendar.h"
#include "graphics.h"
#include "sound.h"
#include "finance.h"
#include "player.h"
#include "files.h"
#include "recipe.h"
#include "restaurant.h"
#include "reports.h"
#include "world.h"
#include "text.h"
#include "input.h"
#include "menu.h"
#include "frame.h"
#include "runtime_state.h"
#include "character.h"
#include "missions.h"
#include "messages.h"
#include <fenv.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

u8 HandleStockMenu(Rest *restaurant) {
  u8 category, row, index;

  category = 0;
  row = 0;
  u32 total_kg = 0, total_value = 0;
  const Rest *rest = restaurant;
  while (row < 96) {
    u16 valid = IsValidIngredient(row);
    if (valid) {
      const RestaurantStock *stock = &rest->stock[row];
      u32 weight = (u16)stock->weight;

      total_kg += weight / 100u;
      // The original IMUL wraps to 32 bits before signed IDIV.

      total_value += (u32)(pt_signed_dword(weight * (u16)stock->value) / 100);
    }
    ++row;
  }
  did_terminal_stuff = 0;
  (void)InitGraphicsMode();
  FillLetterBackground(4, 3, 59);
  temp_canvas->color = 0;
  SetCurrentFont(font_astandar);
  SetSomeFieldInCanvas(0);
  (void)DrawText2(78, 152, 83, stock_menu_stocks_title);
  (void)DrawText2(78, 152, 95, stock_menu_depot_title);
  SetCurrentFont(font_piznumb);
  FillRect(254, 77, 119, 120, 138);
  temp_canvas->color = 12;
  (void)SetTempCanvasPosition(77, 127);
  (void)DrawTextInsideList(stock_menu_kilogram_label);
  (void)SetTempCanvasPosition(77, 138);
  (void)DrawTextInsideList(stock_menu_value_label);
  temp_canvas->color = 0;
  (void)SetTempCanvasPosition(138, 127);
  DrawNumberInsideListRight((i32)total_kg);
  cur_list_elem_x = 144;
  (void)DrawTextInsideList(stock_menu_kg_label);
  (void)SetTempCanvasPosition(138, 138);
  (void)DrawDollarsInsideList((i32)(total_value / 100u));
  (void)Do3x3ButtonStuff(28);
  u16 event = WaitForMouseClick();
  mouse_pressed = event;
  if (event >= PT_MENU_BUTTON_1 && event <= PT_MENU_BUTTON_5) {
    if (event == PT_MENU_BUTTON_5)
      goto done;
    category = (u8)(event - PT_MENU_BUTTON_1);
  }
redraw:

  rest = restaurant;
  did_terminal_stuff = 0;
  (void)InitGraphicsMode();
  LoadBlitmapAndOffsets(1);
  (void)Do3x3ButtonStuff(2);
  FillLetterBackground(4, 9, 5);
  (void)DrawBitmapStuff1((u8)(category + 11u), 34, 18);
  SetCurrentFont(font_piznumb);
  SetSomeFieldInCanvas(0);
  temp_canvas->color = 0;
  (void)snprintf((char *)strbuf, sizeof(strbuf), "supplies %s", g_stock_name_kind[category]);
  (void)DrawText2(60, 190, 30, strbuf);
  temp_canvas->color = 14;
  static const i16 columns[5] = {96, 120, 140, 158, 176};
  for (unsigned c = 0; c < 5; ++c) {
    (void)SetTempCanvasPosition(columns[c], 36);
    (void)RelativeLineTo(0, 140);
  }
  SetSomeFieldInCanvas(0);
  temp_canvas->color = 0;
  SetCurrentFont(font_supersma);
  (void)DrawText(102, 41, stock_menu_quality_label);
  (void)DrawText(125, 41, stock_menu_supply_label);
  (void)DrawText(143, 41, stock_menu_need_label);
  (void)DrawText(162, 41, stock_menu_use_label);
  (void)DrawText(178, 41, stock_menu_days_label);
  SetCurrentFont(some_font_ptr);
  row = 0;

  while (row < num_ingredients_in_set[category]) {
    index = (u8)(24u * category + row);
    (void)SetTempCanvasPosition(25, (i16)(6u * row + 50u));
    temp_canvas->color = 0;
    (void)DrawTextInsideList(ingredients[category].vegs[row].name);
    const RestaurantStock *stock = &rest->stock[index];
    if (stock->quality || stock->need) {
      if ((u16)stock->weight < 100)
        temp_canvas->color = 21;
      (void)SetTempCanvasPositionX(118);
      DrawNumberInsideListRight(stock->quality);
      (void)SetTempCanvasPositionX(139);
      DrawNumberInsideListRight((u16)stock->weight / 100u);
      if (stock->need) {
        (void)SetTempCanvasPositionX(158);
        DrawNumberInsideListRight(stock->need);
        (void)SetTempCanvasPositionX(176);
        float demand = 0;

        for (unsigned k = 0; k < rest->num_recipes; ++k) {
          const RestoRecipe *entry = &rest->recipes[k];

          const Recipe *recipe = entry->recipe;
          u32 product = (u16)recipe->weight_of_type[index] * entry->sales;

          demand = (float)((double)demand + product);
        }

        demand = (float)((double)demand / stock_menu_demand_divisor);

        if (demand > 0) {
          double rounded = TruncateX87Integral(1.0 + (double)demand);

          DrawNumberInsideListRight((i32)rounded);
        }
        (void)SetTempCanvasPositionX(192);
        if ((u16)stock->weight) {
          u16 days = ComputeStockDaysLeft(stock);
          DrawNumberInsideListRight(days);
        }
      }
    }
    ++row;
  }
wait:
  event = WaitForMouseClick();
  mouse_pressed = event;
  if (event == 0x0f09) {
    restaurant = ((Rest *)(SelectNextRestaurant(restaurant)));
    goto redraw;
  }
  if (!pt_is_menu_button_event(event))
    goto wait;
  switch (event - PT_MENU_BUTTON_1) {
  case 0:
    category = 0;
    break;
  case 1:
    category = 1;
    break;
  case 3:
    category = 2;
    break;
  case 4:
    category = 3;
    break;
  case 2:
    HandleContractsMenu(restaurant);
    AdvanceClock(1);
    break;
  case 5:
    SoundStuff3(15, 0);
    (void)HandleStockShopMenu(restaurant, category, 0);
    RestLoadMusic(restaurant, 1);
    AdvanceClock(2);
    (void)CheckPeriodicPhoneCalls();
    break;
  default:
    (void)CheckGoalStuff();
    goto done;
  }
  u8 pending = CheckGoalStuff();
  if (!pending || !has_many_players)
    goto redraw;

  return 255;
done:

  return 0;
}

i32 GetSupplierQuality(u8 packed_quality) {
  return packed_quality / 16u;
}
u32 DrawSupplierBitmap(u8 quality, u8 row) {
  i16 y = (i16)((u16)(32u * row + 40u));
  (void)DrawBitmapStuff1(quality, 26, y);
  (void)DrawBitmapStuff1(6, 58, y);
  return DrawBitmapStuff1(24, 108, y);
}
u16 DrawSupplierName(const u8 *name, i16 x, i16 y) {
  SetCurrentFont(font_piznumb);
  temp_canvas->color = 12;
  u16 result;
  result = DrawText2(x, (i16)((u16)x + 125u), (i16)((u16)y + 16u), name);
  return result;
}
void ClearDepotPurchases(void) {
  (void)memset(purchase_amount, 0, sizeof(purchase_amount));
  (void)memset(shop_purchase_price, 0, sizeof(shop_purchase_price));
  (void)memset(shop_purchase_secondary_price, 0, sizeof(shop_purchase_secondary_price));
  current_purchase_amount = 0;
  shop_secondary_purchase_amount = 0;
}
void DrawShopContents(const Supplier *supplier, const Rest *restaurant, i8 index, u8 shade) {
  const Supplier *supp = supplier;
  unsigned set = supp->stock_kind;
  if (index < 0 || index >= num_ingredients_in_set[set]) {
    return;
  }
  const Rest *rest = restaurant;
  const RestaurantStock *stock = &rest->stock[24u * set + (unsigned)index];
  u16 price = GetPriceForIngredient(supplier, (u8)index);

  i32 amount = (i32)((u16)stock->weight / 100u) + (i32)purchase_amount[(unsigned)index];

  i16 top = (i16)(6 * index + 39);
  FillRect((u8)shade, 20, (i16)top, 192, (i16)(top + 6));
  temp_canvas->color = 0;
  (void)SetTempCanvasPosition(22, (i16)(top + 5));
  (void)DrawTextInsideList(ingredients[set].vegs[(unsigned)index].name);
  (void)SetTempCanvasPositionX(109);
  if (price) {
    double value = TruncateX87Integral((float)price);

    DrawSupplierStockPrice((i32)value);
  } else {
    (void)SetTempCanvasPositionX(90);
    (void)DrawTextInsideList(shop_unavailable_price_text);
  }
  (void)SetTempCanvasPositionX(142);
  DrawNumberInsideListRight(purchase_amount[(unsigned)index]);
  (void)SetTempCanvasPositionX(168);
  DrawNumberInsideListRight((u16)amount);
  temp_canvas->color = 11;
  cur_list_elem_x = 190;
  if (stock->need)
    DrawNumberInsideListRight((u16)amount / stock->need);
  temp_canvas->color = 14;
  (void)SetTempCanvasPosition(120, top);
  (void)RelativeLineTo(0, 8);
  (void)SetTempCanvasPosition(142, top);
  (void)RelativeLineTo(0, 8);
}
void UpdateTotalPurchasePrice(u8 count) {
  SetCurrentFont(some_font_ptr);
  FillRect(162, 124, 175, 178, 182);
  temp_canvas->color = 20;
  current_purchase_amount = 0;
  shop_secondary_purchase_amount = 0;
  for (unsigned i = 0; i < count; ++i) {
    current_purchase_amount = (float)((double)shop_purchase_price[i] + current_purchase_amount);
    shop_secondary_purchase_amount = (float)((double)shop_purchase_secondary_price[i] + shop_secondary_purchase_amount);
  }
  (void)SetTempCanvasPosition(159, 179);
  DrawRightAlignedFloat(current_purchase_amount);
}
// Fifteen suppliers leave room for the NULL sentinel in sixteen slots.

u16 HandleStockShopMenu(Rest *restaurant, u8 kind, u8 mode) {

  u8 count, first, rows, offset;
  Supplier *suppliers[16];

  count = 0;
  first = 0;
  PTForwardLink *node = current_town->suppliers.begin.next;

  for (;;) {
    if (!node->next)
      break;

    Supplier *supplier = (Supplier *)node;
    if (supplier->stock_kind == kind) {
      if (count >= 15) {
        return PT_MENU_BUTTON_3;
      }

      suppliers[count] = supplier;
      ++count;
    }
    node = supplier->link.next;
  }

  suppliers[count] = NULL;
redraw:
  (void)Do3x3ButtonStuff(27);
  rows = 0;
  FillLetterBackground(4, 9, 5);
  (void)DrawBitmapStuff1((u8)(kind + 11u), 34, 18);
  (void)snprintf((char *)strbuf, sizeof(strbuf), "%s suppliers", g_stock_name_kind[kind]);
  SetCurrentFont(font_piznumb);
  temp_canvas->color = 0;
  (void)DrawText2(60, 199, 30, strbuf);
  temp_canvas->color = 7;
  SetSomeFieldInCanvas(0);
  SetCurrentFont(font_piznumb);
  while (rows < 4) {
    Supplier *supplier = suppliers[first + rows];
    if (!supplier)
      break;

    const Supplier *supp = supplier;
    offset = (u8)(32u * rows);
    // The row argument is already pushed across the nested quality call.

    i32 quality;
    quality = GetSupplierQuality(supp->quality);

    (void)DrawSupplierBitmap((u8)(quality - 1), rows);
    (void)DrawSupplierName(supplier->name, 54, (i16)(offset + 39u));

    i32 total = supp->total_bought_by_player[current_player_index];
    if (total) {
      SetCurrentFont(some_font_ptr);
      (void)snprintf((char *)strbuf, sizeof(strbuf), "so far: $%d", total);
      (void)DrawText2(60, 185, (i16)(offset + 62u), strbuf);
    }
    ++rows;
  }
  SetCurrentFont(font_piznumb);
  (void)SetTempCanvasPosition(97, 177);
  (void)snprintf((char *)strbuf, sizeof(strbuf), "- %d -", first / 4u + 1u);
  temp_canvas->color = 0;
  (void)DrawTextInsideList(strbuf);
wait:;
  u16 event = WaitForMouseClick();
  mouse_pressed = event;
  if (pt_is_menu_button_event(event)) {
    if (event == PT_MENU_BUTTON_1) {
      if ((unsigned)first + 5u > count)
        goto wait;
      first = (u8)(first + 4u);
      goto redraw;
    }
    if (event == PT_MENU_BUTTON_2) {
      if (!first)
        goto wait;
      first = (u8)(first - 4u);
      goto redraw;
    }
    if (event == PT_MENU_BUTTON_3) {
      return event;
    }
    goto redraw;
  }
  bool hit;
  hit = IsMouseInside(12, 40, 203, (i16)(32u * rows + 40u));
  if (!hit)
    goto wait;

  unsigned index = first + (u8)((u8)mouse_result->y - 40u) / 32u;

  Supplier *supplier = suppliers[index];
  if (!mode) {
    HandleBuyStockToRest(supplier, restaurant);
  } else if (mode <= 2) {
    HandleBuyStockToDepot(current_player_index, (u8)(mode - 1u), supplier);
  }
  ClearDepotPurchases();
  goto redraw;
}
void HandleBuyStockToRest(Supplier *supplier, Rest *restaurant) {
  u8 kind, selected, clicked, repeated;

  kind = supplier->stock_kind;
  selected = 255;
  clicked = 255;
  ClearDepotPurchases();
  FillLetterBackground(4, 9, 5);
  SetCurrentFont(font_piznumb);
  temp_canvas->color = 0;
  (void)DrawText2(60, 190, 30, supplier->name);
  temp_canvas->color = 14;
  (void)SetTempCanvasPosition(120, 39);
  (void)RelativeLineTo(0, 134);
  (void)SetTempCanvasPosition(142, 39);
  (void)RelativeLineTo(0, 134);
  temp_canvas->color = 12;
  (void)SetTempCanvasPosition(100, 173);
  (void)DefaultLineTo(171, 173);
  SetCurrentFont(some_font_ptr);
  for (u8 i = 0; i < num_ingredients_in_set[kind]; ++i)
    DrawShopContents(supplier, restaurant, (i8)i, 162);
  (void)DrawBitmapStuff1((u8)(kind + 11u), 34, 18);
  (void)Do3x3ButtonStuff(6);
  SetCurrentFont(font_supersma);
  temp_canvas->color = 0;
  (void)snprintf((char *)strbuf, sizeof(strbuf), "quality: %d", supplier->quality);
  (void)DrawText(44, 180, strbuf);
  (void)DrawText(100, 180, shop_total_label);
  (void)DrawText(97, 38, shop_price_label);
  (void)DrawText(123, 38, shop_amount_label);
  (void)DrawText(150, 38, shop_stock_label);
  (void)DrawText(175, 38, stock_menu_days_label);
  SetSomeFieldInCanvas(0);
  SaveAndSetInputEventFilter(5);
  UpdateTotalPurchasePrice(num_ingredients_in_set[kind]);
wait:;
  u16 event = WaitForMouseClick();
  mouse_pressed = event;
  if (!event) {
    bool hit;
    hit = IsMouseInside(12, 14, 203, 189);
    if (!hit)
      goto wait;
    i32 i = GetSelectedListItem(38, 6);
    clicked = (u8)i;
    if (clicked != selected) {
      DrawShopContents(supplier, restaurant, (i8)selected, 162);
      DrawShopContents(supplier, restaurant, (i8)clicked, 28);
      selected = clicked;
    }
  }
  if (mouse_pressed == 1) {
    if ((i8)clicked < 0 || clicked >= num_ingredients_in_set[kind])
      goto wait;
    repeated = 0;
    unsigned i = clicked;
    u16 held;
    for (;;) {
      held = WantSkipGameIntro();
      if (!held || !supplier->num_items_left[i])
        break;
      if (i >= num_ingredients_in_set[kind] || (i8)clicked < 0)
        continue;
      u32 weight = (u16)restaurant->stock[24u * supplier->stock_kind + i].weight;

      // The original repeatedly tests the hardware button at the cap.
      // Yield at its taken JGE so a recorded release can end the hold.

      if ((i32)weight + 100 * (i32)purchase_amount[i] >= 65000) {
        pt_frame_poll(PT_FRAME_POLL_STOCK_LIMIT);
        continue;
      }

      purchase_amount[i] = (i16)((u16)purchase_amount[i] + 1u);
      supplier->num_items_left[i] = (i16)((u16)supplier->num_items_left[i] - 1u);
      DrawShopContents(supplier, restaurant, (i8)i, 14);
      float amount = (float)purchase_amount[i];

      u16 price;
      price = GetPriceForIngredient(supplier, (u8)i);

      shop_purchase_price[i] = (float)((double)price * amount / shop_price_divisor);
      UpdateTotalPurchasePrice(num_ingredients_in_set[kind]);
      if (!repeated) {
        Delay(8);
        repeated = 1;
      }
      Delay(2);
    }
  }
  if (mouse_pressed == 2) {
    repeated = 0;
    if ((i8)clicked < 0 || clicked >= num_ingredients_in_set[kind])
      goto wait;
    unsigned i = clicked;
    u16 held;
    for (;;) {
      held = MouseThingy();
      if (!held || !purchase_amount[i])
        break;
      if (i >= num_ingredients_in_set[kind] || (i8)clicked < 0)
        continue;

      purchase_amount[i] = (i16)((u16)purchase_amount[i] - 1u);
      supplier->num_items_left[i] = (i16)((u16)supplier->num_items_left[i] + 1u);
      DrawShopContents(supplier, restaurant, (i8)i, 14);
      i32 quantity = purchase_amount[i];
      u16 price;
      price = GetPriceForIngredient(supplier, (u8)i);
      u32 product = (u32)quantity * price;

      shop_purchase_price[i] = (float)((double)pt_signed_dword(product) / shop_price_divisor);
      UpdateTotalPurchasePrice(num_ingredients_in_set[kind]);
      if (!repeated) {
        Delay(8);
        repeated = 1;
      }
      Delay(2);
    }
  }
  if (!pt_is_menu_button_event(mouse_pressed))
    goto wait;
  if (mouse_pressed == PT_MENU_BUTTON_2) {
    DoContractThingy(supplier, purchase_amount, restaurant);
    goto done;
  }
  if (mouse_pressed == PT_MENU_BUTTON_3) {
    for (unsigned i = 0; i < num_ingredients_in_set[kind]; ++i)
      supplier->num_items_left[i] = (i16)((u16)supplier->num_items_left[i] + (u16)purchase_amount[i]);
    goto done;
  }
  if (mouse_pressed != PT_MENU_BUTTON_1)
    goto wait;
  u32 paid;
  paid = TransferPlayerMoney(current_player_index, 4, 5, current_purchase_amount);
  if (!(u16)paid)
    goto wait;

  unsigned player = current_player_index;
  u32 expense = (u32)ramtown.total_expense_per_player[player];

  u32 weekly_cost;
  double integral = TruncateX87Integral((double)expense + current_purchase_amount);

  ramtown.total_expense_per_player[player] = pt_signed_dword((u32)(u64)(i64)integral);
  for (u8 i = 0; i < num_ingredients_in_set[kind]; ++i) {
    u16 price;
    price = GetPriceForIngredient(supplier, i);
    u16 amount = (u16)purchase_amount[i];
    (void)AddStockToRestaurantInventory(restaurant, kind, i, amount, supplier->quality, price);
  }
  // Category 3 is pushed before the original ST0 truncation call.

  integral = TruncateX87Integral(current_purchase_amount);

  u32 cost = (u32)(u64)(i64)integral;
  AddCostForRestaurant(restaurant, cost, 3);

  unsigned set = supplier->stock_kind;

  u32 prior = (u16)restaurant->weekly_ingredient_type_cost[set];

  integral = TruncateX87Integral((double)prior + current_purchase_amount);
  weekly_cost = (u32)(i32)integral;
  restaurant->weekly_ingredient_type_cost[set] = (i16)((u16)weekly_cost);
  restaurant->last_supplier_of_stock[set] = supplier->supplier_id;

  player = current_player_index;

  prior = (u32)supplier->total_bought_by_player[player];

  integral = TruncateX87Integral((double)prior + current_purchase_amount);

  supplier->total_bought_by_player[player] = pt_signed_dword((u32)(u64)(i64)integral);
  DisplayStuffAfterPayment();
done:
  AdvanceClock(1);
  SaveAndSetInputEventFilter(3);
  ;
}

void DoContractThingy(Supplier *supplier, const i16 *amounts, Rest *restaurant) {
  u8 index;
  u32 portrait;

  current_purchase_amount = 0;
  index = 0;

  while (index < num_ingredients_in_set[supplier->stock_kind]) {
    u16 amount = (u16)amounts[index];
    u16 price;
    price = GetPriceForIngredient(supplier, index);
    u32 product = (u32)amount * price;

    double previous = current_purchase_amount;

    current_purchase_amount = (float)((double)pt_signed_dword(product) / contract_purchase_divisor + previous);
    ++index;
  }
  SetPhoneTextRect(14, 140, 196, 189, 0, 2);
  game_ptr->list_element_spacing_2 = 0;
  (void)LoadLangFile(&langfile_lief);
  portrait = supplier->bitmaps;
  if (game_ptr->debug_mode) {
    i32 chosen_portrait = ShowAskForNumberQuery(contract_debug_portrait_prompt, pt_signed_dword(portrait), 1, 1, 20);
    portrait = (u32)chosen_portrait;
  }
  (void)LoadPhoneBitmaps((u8)(supplier->stock_kind + 10u), (u8)portrait);
  (void)Do3x3ButtonStuff(3);
  DrawPhonecallStuff();

  if ((u16)supplier->minimum_order > current_purchase_amount) {
    // Keep the minimum as the fourth formatting argument while translating.

    const u8 *text;

    text = GetTranslatedString(1);
    (void)snprintf((char *)strbuf,
        sizeof(strbuf),
        (const char *)contract_minimum_order_format,
        (const char *)text,
        (int)(u16)supplier->minimum_order);

    (void)DrawStrbufPhoneMessage();
  } else {
    unsigned player = current_player_index;

    if ((u32)supplier->current_account[player] > current_purchase_amount) {
      (void)DrawPhoneMessageFromLangfile(9);
    } else {
      Contract *contract, *agreed;
      contract = pt_simulation_allocate(PT_SIM_CONTRACT, sizeof(Contract));
      if (!contract)
        goto done;
      agreed = HandleNegotiateContract(contract, supplier, restaurant, amounts);
      AdvanceClock(2);
      UnloadContractsBobFile();
      if (contract == agreed) {
        (void)LinkList_InsertLast(&player_ptr->contracts, &contract->link);
        Contract *c = contract;

        supplier->last_discount[player] = c->discount;
        if (c->discount >= 50) {
          MarkGoalReached(95);
        }
        c->term_length_total = c->term_length;
        supplier->last_amount[player] = (i16)((u16)c->sum);
        supplier->last_term_length[player] = c->term_length;
        c->byte4E = (u8)(((i32)ptr_apple->day + c->term_length - 1) % 30 + 1);
        supplier->num_contracts_with_player[player] = (u8)((u32)(supplier->num_contracts_with_player[player]) + 1u);

        player_ptr->num_contracts = (u8)((u32)(player_ptr->num_contracts) + (u32)1u);
        ++ramtown.num_contracts_per_player[player];
      } else {
        pt_simulation_release(contract, sizeof(*contract));
      }
      AdvanceClock(2);
    }
  }
  FreeLangFile(&langfile_lief);
done:;
}

// Returns the final text helper's unsigned WORD.

PTCanvasResult DrawContractTerms(const Contract *contract) {
  const Contract *c = contract;
  FillRect(240, 114, 25, 191, 87);
  SetCurrentFont(font_piznumb);
  SetSomeFieldInCanvas(0);
  temp_canvas->color = 15;
  (void)SetTempCanvasPosition(117, 35);
  (void)DrawTextInsideList(contract_sum_label);
  (void)SetTempCanvasPositionX(180);
  (void)DrawDollarsInsideList(c->sum);
  (void)SetTempCanvasPosition(117, 46);
  (void)DrawTextInsideList(contract_discount_label);
  (void)SetTempCanvasPositionX(180);
  DrawPercentageNumberInList(c->discount);
  (void)SetTempCanvasPosition(117, 57);
  (void)DrawTextInsideList(contract_charge_label);
  (void)SetTempCanvasPositionX(180);
  (void)DrawDollarsInsideList((u16)c->charge);
  (void)SetTempCanvasPosition(117, 68);
  (void)DrawTextInsideList(contract_terms_penalty_label);
  (void)SetTempCanvasPositionX(180);
  (void)DrawDollarsInsideList(c->penalty);
  (void)snprintf((char *)strbuf, sizeof(strbuf), "term: %d days", (i8)c->term_length);
  (void)SetTempCanvasPosition(117, 79);
  return DrawTextInsideList(strbuf);
}

// Eight probability bytes followed by four choices.

u8 AskHowToJustifyDemands(void) {
  u8 choices[4];
  u8 chance = 0;

  (void)DrawPhoneMessageFromLangfile(30);
  (void)GetSupplierContractPhoneAlternatives(choices, 8, 20);
  u8 selected = ShowPhonecallAlternatives(choices[0], choices[1], choices[2], choices[3], 0, 0);

  if (selected < 1 || selected > 4)
    goto done;
  u8 index = choices[selected - 1u];
  // Repaired cancellation fails justification without consuming RNG.

  chance = RandomChance(contract_justification_probabilities[index - 20u]);
  if (chance) {
    DoPhoneFaces(1);
  }
done:;
  return chance ? 1 : 0;
}

// Bargaining retains each demand count and initializes fee reduction.

Contract *HandleNegotiateContract(Contract *c, Supplier *s, Rest *restaurant, const i16 *amounts) {
  u8 said_counts[10], choices[4];
  u8 ask, fake, root, probability, abort_call, lower_fees, next;

  const Character *character = character_ptr;
  unsigned player = current_player_index;
  Contract *agreed = NULL;
  u8 answer = 0;

  ask = 255;
  fake = 0;
  u32 x = 2u * character->personality;

  probability = (u8)((x + character->intelligence + character->charisma) >> 2);

  abort_call = 0;
  lower_fees = 0;
  memset(said_counts, 0, sizeof(said_counts));
  game_ptr->did_choose_tv_thingy = 10;
  u32 divisor = (u32)s->bonus_levels * 4u;

  u8 bonus = (u8)((u32)s->total_bought_by_player[player] / divisor);
  root = bonus > 9 ? 4 : bonus > 3 ? 3 : 2;

  u8 discount = 0;
  float initial_amount = current_purchase_amount;
  if (s->num_contracts_with_player[player]) {
    x = (u16)s->minimum_order;

    double discount_value =
        (((double)initial_amount / x + bonus) * contract_discount_factor + s->last_discount[player]) /
        contract_discount_divisor;
    double discount_integral = TruncateX87Integral(discount_value);
    u32 adjusted_discount;
    adjusted_discount = (u32)pt_integral_i32(discount_integral);
    discount = (u16)adjusted_discount > 60 ? 60 : (u8)adjusted_discount;
  }
  u32 penalty, term;
  double integral = TruncateX87Integral((double)initial_amount *
      (s->num_contracts_with_player[player] ? contract_discount_divisor : contract_penalty_new_factor));

  penalty = (u32)(u64)pt_integral_i64(integral);
  x = s->last_term_length[player] >= 28 ? s->last_term_length[player] : 28;
  term = x;

  double charge_value = (double)s->annoyed + (double)initial_amount / contract_charge_divisor;
  double charge_integral = TruncateX87Integral(charge_value);
  u32 adjusted_charge;
  adjusted_charge = (u32)pt_integral_i32(charge_integral);
  u16 charge = bonus > 3 ? 0 : (u16)adjusted_charge;
  InitializeContract(c, s, restaurant, amounts, discount, pt_signed_dword(penalty), charge, (u8)term);
  x = (u16)s->last_amount[player];

  if (x > current_purchase_amount)
    next = 7;
  else if (bonus > 10)
    next = 5;
  else if (bonus > 5)
    next = 4;
  else {
    next = bonus ? 3 : 46;
    if (RandomChance(90 - character->personality) && RandomChance(90 - character->personality)) {
      current_purchase_amount = (float)((double)current_purchase_amount * contract_fake_sum_factor);
      fake = 1;
    }
  }
draw:
  // Stock pushes the supplier as an extra ignored drawing argument.

  (void)DrawContractTerms(c);
  SetCurrentFont(font_piznumb);
  if (next != 255) {
    answer = AskPhoneYesNoQuestionFromLangfile(next);
  }
  if (next == 5) {
    AnimatePhonePleased();
  }
  if (ask != 255) {
    answer = AskPhoneYesNoQuestionFromBuffer();
  }
  next = 255;
  ask = 255;
  if (abort_call) {
    AnimatePhoneDispleased();
    goto done;
  }
decide:
  if (!answer) {
    s->annoyed = (u8)(s->annoyed + 2u);
    goto done;
  }
  if (answer == 102) {
    agreed = c;
    if (s->annoyed)
      s->annoyed = (u8)((u32)s->annoyed - 1u);
    goto done;
  }
  if (!bonus) {
    next = 58;
    abort_call = 1;
    goto draw;
  }
  (void)GetSupplierContractPhoneAlternatives(choices, 9, 10);
alternatives:
  answer = ShowPhonecallAlternatives(choices[0], choices[1], choices[2], choices[3], 0, 0);
  if (answer == 101)
    goto alternatives;
  if (answer == 102 || answer == 0)
    goto decide;
  // Cancel and toolbar responses are handled before indexing the four rows.

  answer = (u8)(choices[answer - 1u] - 9u);
  i8 action = (i8)answer;

  if (action == 9)
    s->gapF0 = (u8)((u32)s->gapF0 + 1u);
  else
    s->num_times_said[action - 1] = (u8)((u32)(s->num_times_said[action - 1]) + 1u);

  ++said_counts[(u8)action];
  if (said_counts[(u8)action] == 2) {
    next = 31;
    s->annoyed = (u8)((u32)s->annoyed + 1u);
    goto draw;
  }
  if (said_counts[(u8)action] > 2) {
    next = 34;
    abort_call = 1;
    s->annoyed = (u8)(s->annoyed + 3u);
    goto draw;
  }
  switch (action) {
  case 1:
  case 2:
    if ((u32)s->bonus_levels * 2u > (u32)s->total_bought_by_player[player]) {
      next = 32;
      c->charge = (i16)((u16)c->charge * 2u);
      break;
    }
    if (2u * s->byteE7 > bonus) {
      next = 54;
      s->annoyed = (u8)((u32)s->annoyed + 1u);
      break;
    }
    if (s->byteE7 >= root || c->discount >= 50) {
      next = 54;
      break;
    }
    if (RandomChance((i32)probability - 20)) {
      next = 48;
      c->discount = (u8)(c->discount + 5u);
      DoPhoneFaces(0);
      break;
    }
    {
      u8 justified = AskHowToJustifyDemands();
      if (justified) {
        c->discount = (u8)(c->discount + 5u * (RandomChance(character->personality) + 1u));
        next = 33;
      } else {
        if (c->discount > 4)
          c->discount = (u8)(c->discount - 4u);
        next = 47;
      }
    }
    break;
  case 3:
    if (bonus < 2) {
      next = 49;
      break;
    }
    if (bonus < s->unpaid_bill_level[player]) {
      next = 50;
      break;
    }
    if (RandomChance(probability)) {
      if (RandomChance(30)) {
        u8 justified = AskHowToJustifyDemands();
        if (justified) {
          next = 48;
          lower_fees = 1;
        }
      }
    } else {
      lower_fees = 1;

      next = 54;
    }

    if (lower_fees) {
      integral = TruncateX87Integral((double)(u32)c->penalty * contract_penalty_reduction_factor);

      c->penalty = pt_signed_dword((u32)(u64)pt_integral_i64(integral));
    } else
      next = 51;
    break;
  case 4:
    if (!c->charge) {
      next = 52;
      s->annoyed = (u8)(s->annoyed + 5u);
      c->charge = 100;
      break;
    }
    if (RandomChance(probability)) {
      lower_fees = 1;

      next = 53;
    } else {
      u8 justified = AskHowToJustifyDemands();
      if (justified) {
        lower_fees = 1;

        next = 48;
      }
    }

    if (lower_fees)
      c->charge = (i16)((u16)c->charge >> 1);
    else
      next = 54;
    break;
  case 5:
    if (!fake) {
      ask = 40;
      abort_call = 1;
      break;
    }
    next = 44;
    c->sum = pt_signed_dword((u32)c->sum >> 1);
    break;
  case 6:
    if (s->had_shortages[player]) {
      next = 36;
      c->discount = (u8)(c->discount + 5u);
    } else
      next = 35;
    break;
  case 7:
    next = 39;
    c->term_length = (u8)(c->term_length + 14u);
    break;
  case 8:
    if (c->term_length == 7)
      next = 38;
    else if (c->term_length == 84)
      next = 57;
    else if (said_counts[8] >= 2)
      next = 31;
    else {
      next = 39;
      c->term_length = (u8)(c->term_length - 7u);
      DoPhoneFaces(1);
    }
    break;
  case 9:
    if (!RandomChance(30) || c->discount >= 50) {
      next = 55;
      abort_call = 1;
    } else {
      next = 56;
      c->discount = (u8)(c->discount + (bonus > 6 ? 8u : 5u));
    }
    break;
  default:
    break;
  }
  goto draw;
done:
  if (agreed == c) {
    (void)DrawPhoneMessageFromLangfile(45);
  };
  return agreed;
}
// . The five-byte days literal is copied inline by MOVS.

void DrawContractDetails(const Contract *contract, u8 page) {
  u8 kind, i;
  const Contract *c = contract;
  const Supplier *supplier = c->supplier;

  kind = supplier->stock_kind;

  FillLetterBackground(4, 5, 17);
  if (page) {
    FillRect(172, 45, 143, 92, 179);
    FillRect(172, 93, 145, 137, 161);
    FillRect(172, 138, 150, 148, 158);
    temp_canvas->color = 0;
    SetCurrentFont(font_piznumb);
    (void)DrawText(62, 35, contract_delivery_amounts_title);
    SetCurrentFont(some_font_ptr);
    (void)SetTempCanvasPosition(42, 58);
    i = 0;
    while (i < num_ingredients_in_set[kind]) {
      u16 quantity = (u16)c->amount_of_each[i];
      if (quantity) {
        (void)SetTempCanvasPositionX(42);
        (void)DrawTextInsideList(ingredients[kind].vegs[i].name);
        (void)SetTempCanvasPositionX(134);
        DrawNumberInsideListRight(quantity);
        (void)SetTempCanvasPositionX(136);
        (void)DrawTextInsideList(stock_menu_kg_label);
        cur_list_elem_y = (i16)((u16)cur_list_elem_y + 6u);
      }
      ++i;
    }
  } else {
    SetCurrentFont(font_piznumb);
    temp_canvas->color = 12;
    (void)SetTempCanvasPosition(111, 72);
    (void)DefaultLineTo(144, 72);
    temp_canvas->color = 0;
    (void)DrawText(62, 35, contract_with_title);
    (void)DrawText2(36, 160, 44, supplier->name);
    (void)DrawText(40, 58, shop_total_label);
    (void)SetTempCanvasPositionX(134);
    (void)DrawDollarsInsideList(c->sum);
    (void)DrawText(40, 69, contract_discount_label);
    (void)SetTempCanvasPositionX(93);
    DrawPercentageNumberInList(c->discount);
    u32 discount = ((u32)c->sum * c->discount) / 100u;
    (void)SetTempCanvasPositionX(134);
    (void)DrawDollarsInsideList(pt_signed_dword(0u - discount));
    (void)SetTempCanvasPosition(134, 82);
    (void)DrawDollarsInsideList(pt_signed_dword((u32)c->sum - discount));
    (void)DrawText(40, 90, contract_charge_label);
    (void)SetTempCanvasPositionX(96);
    (void)DrawDollarsInsideList((u16)c->charge);
    (void)DrawText(40, 101, contract_penalty_label);
    (void)SetTempCanvasPositionX(140);
    (void)DrawDollarsInsideList(c->penalty);
    (void)DrawText(40, 112, contract_amount_label);
    (void)SetTempCanvasPositionX(82);
    (void)DrawTextInsideList(c->amount_mode ? contract_as_required_label : contract_as_agreed_label);
    (void)DrawText(40, 123, contract_quality_label);
    (void)SetTempCanvasPositionX(123);
    DrawNumberInsideListRight((i8)supplier->quality);
    (void)DrawText(40, 134, contract_remaining_term_label);
    (void)SetTempCanvasPositionX(123);
    DrawNumberInsideListRight(c->term_length);
    (void)SetTempCanvasPosition(126, cur_list_elem_y);
    u32 count = pt_strlen(contract_days_literal);
    count -= c->term_length == 1;
    (void)Canvas_DrawText(temp_canvas, pt_text_span(contract_days_literal, (u16)count));
  }

  ;
}
// Signed term byte, low IMUL dword, unsigned DIV.

void ContractCancel(Contract *contract) {
  const Contract *c = contract;

  u32 penalty = ((u32)c->penalty * (u32)(i32)(i8)c->term_length) / c->term_length_total;

  float amount = (float)penalty;

  u32 paid;
  paid = TransferPlayerMoney(current_player_index, 10, 5, amount);
  const u8 *suffix = contract_delivery_space_text;
  if (!(u16)paid) {
    suffix = contract_insolvency_text;
    (void)TransferPlayerMoney(current_player_index, 10, 8, amount);

    Supplier *supplier = c->supplier;
    supplier->current_account[current_player_index] =
        pt_signed_dword((u32)supplier->current_account[current_player_index] + penalty);
  }
  const Supplier *supplier = c->supplier;
  (void)snprintf(
      (char *)strbuf, sizeof(strbuf), "contract with %s cancelled. %s", supplier->name, (const char *)suffix);
  ShowMessage(15, strbuf);
  AddCostForRestaurant(curr_rest, penalty, 0);
  (void)ContractFree(contract);
}
// The labelled original repair keeps one of the thirty pointer slots for
// the terminator and returns before storing a thirtieth matching contract.

void HandleContractsMenu(Rest *restaurant) {
  u8 count, index, page;
  Contract *contracts[30];

  page = 0;
rebuild:
  count = 0;
  PTForwardLink *node = player_ptr->contracts.begin.next;

  for (;;) {
    if (!node->next)
      break;

    Contract *c = (Contract *)node;
    const Rest *found;
    found = FindRestaurantById(c->restaurant_id);
    if (found == restaurant) {
      if (count >= 29)
        goto done;

      contracts[count] = c;
      ++count;
    }
    node = c->link.next;
  }

  contracts[count] = NULL;
  if (!count) {
    DisplayErrorMessage(0, contract_empty_error);
    goto done;
  }
  (void)Do3x3ButtonStuff(11);
  index = 0;
select:;
  Contract *contract = contracts[index];
draw:
  DrawContractDetails(contract, page);
wait:;
  u16 event = WaitForMouseClick();
  mouse_pressed = event;
  if (event == 1) {
    contract->amount_mode = (u8)((u32)contract->amount_mode ^ 1u);
  }
  if (mouse_pressed == 2) {
    i32 term = ShowAskForNumberQuery(contract_term_prompt, (i8)contract->term_length, 1, 0, 100);

    contract->term_length = (u8)term;
  }
  switch (mouse_pressed) {
  case PT_MENU_BUTTON_1:
    if ((unsigned)index + 1u >= count)
      goto wait;
    ++index;
    goto select;
  case PT_MENU_BUTTON_2:
    ++page;
    if (page > 1)
      page = 0;
    goto draw;
  case PT_MENU_BUTTON_3:
    if (!index)
      goto wait;
    --index;
    goto select;
  case PT_MENU_BUTTON_4: {
    u8 yes = ShowMessage2(0, contract_cancel_prompt);
    if (yes == 1) {
      ContractCancel(contract);
      if (player_ptr->num_contracts)
        goto rebuild;
    }
    goto done;
  }
  case PT_MENU_BUTTON_5:
    goto done;
  default:
    goto select;
  }
done:;
}

void DepotDragStockBg(u8 kind) {
  LoadBlitmapAndOffsets(1);
  FillLetterBackground(4, 9, 5);
  (void)DrawBitmapStuff1((u8)(kind + 11u), 34, 18);
  SetCurrentFont(some_font_ptr);
  temp_canvas->color = 0;
  for (unsigned i = 0; i < num_ingredients_in_set[kind]; ++i) {
    (void)SetTempCanvasPosition(20, (i16)(6 * i + 42));
    (void)DrawTextInsideList(ingredients[kind].vegs[i].name);
  }
}
void DrawDepotContents(const Depot *depot, u8 kind, u8 town) {
  u32 total = 0;
  DepotDragStockBg(kind);
  temp_canvas->color = 0;
  SetCurrentFont(font_piznumb);
  (void)snprintf((char *)strbuf, sizeof(strbuf), (const char *)depot_town_title_format, town_names[town]);
  (void)DrawText2(60, 190, 26, strbuf);
  SetCurrentFont(some_font_ptr);
  (void)DrawText(81, 34, depot_columns_label);
  SetCurrentFont(font_supersma);

  for (unsigned i = 0; i < num_ingredients_in_set[kind]; ++i) {
    const DepotEntry *entry = &depot->stock[24u * kind + i];
    temp_canvas->color = 0;
    if ((u16)entry->need > (u16)entry->stock)
      temp_canvas->color = 21;
    (void)SetTempCanvasPosition(100, (i16)(6 * i + 42));
    DrawNumberInsideListRight(entry->quality);
    (void)SetTempCanvasPositionX(124);
    DrawNumberInsideListRight((u16)entry->stock);
    (void)SetTempCanvasPositionX(142);
    DrawNumberInsideListRight((u16)entry->need);
    (void)SetTempCanvasPositionX(158);
    DrawSupplierStockPrice((u16)entry->value);
    (void)SetTempCanvasPositionX(184);
    DrawSupplierStockPrice((u16)entry->last_purchase_value);
    total += (u32)(u16)entry->stock * (u16)entry->value;
  }
  SetCurrentFont(some_font_ptr);
  (void)DrawText(60, 179, depot_sum_value_label);
  (void)SetTempCanvasPosition(158, 179);
  DrawSupplierStockPrice(pt_signed_dword(total));
  SetCurrentFont(font_supersma);
}
u8 GetDepotExportCountry(void) {
  u8 country;
  country = CountrySelector(2, 0);
  (void)InitGraphicsMode();
  ClearDepotPurchases();

  return country;
}

u8 HandleDepotPriorities(Player *player) {
redraw:;
  unsigned count = 1;
  FillLetterBackground(4, 9, 5);
  SetCurrentFont(font_piznumb);
  temp_canvas->color = 0;
  (void)DrawText2(12, 203, 29, depot_priorities_title);
  SetCurrentFont(some_font_ptr);
  PTForwardLink *node = player->restaurants.begin.next;
  while (1) {
    if (!node->next || count > 12)
      break;
    (void)SetTempCanvasPosition(40, (i16)(12 * count + 29));
    DrawNumberInsideListRight((i16)count);
    const u8 *name = GetRestaurantName((const Rest *)node);

    DrawDepotPriorityEntry(49, (i16)(12 * count + 24), 120, name, 0);
    node = node->next;
    ++count;
  }
  for (;;) {
    u16 event = WaitForMouseClick();
    mouse_pressed = event;
    if (event != 1) {
      return (u8)event;
    }
    bool hit;
    hit = IsMouseInside(49, 36, 148, (i16)(12 * (count - 1) + 35));
    if (!hit)
      continue;
    i32 selected = GetSelectedListItem(36, 12);
    u8 index = (u8)selected;

    node = player->restaurants.begin.next;
    for (unsigned i = 0; i < index; ++i)
      node = node->next;
    const u8 *name = GetRestaurantName((const Rest *)node);

    DrawDepotPriorityEntry(49, (i16)(12 * (index + 1) + 24), 120, name, 1);
    (void)WaitForMouseLeave();
    (void)LinkList_Remove(&((Rest *)node)->linked);
    (void)LinkList_InsertFirst(&player->restaurants, &((Rest *)node)->linked);
    goto redraw;
  }
}
u8 HandleCentralDepot(u8 player) {
  u8 stock_kind, remote_depot;
  stock_kind = 0;

  u8 town = ptr_apple->town_index;
  Depot *depot = pt_optional_city_depot(player, town);
  if (!depot)
    goto done;
summary:;

  ClearDepotPurchases();
  (void)InitGraphicsMode();
  FillLetterBackground(4, 3, 15);
  temp_canvas->color = 0;
  SetCurrentFont(font_astandar);
  SetSomeFieldInCanvas(0);
  (void)DrawText2(78, 152, 83, depot_central_label);
  (void)DrawText2(78, 152, 95, shop_stock_label);
  SetCurrentFont(font_piznumb);
  temp_canvas->color = 0;
  (void)SetTempCanvasPosition(72, 127);
  (void)DrawTextInsideList(depot_capacity_label);
  (void)SetTempCanvasPosition(72, 138);
  (void)DrawTextInsideList(depot_in_use_label);
  (void)SetTempCanvasPosition(141, 127);
  DrawNumberInsideListRight(depot->total_size);
  (void)SetTempCanvasPositionX(143);
  (void)DrawTextInsideList(stock_menu_kg_label);
  (void)SetTempCanvasPosition(141, 138);
  DrawNumberInsideListRight(depot->in_use);
  (void)SetTempCanvasPositionX(143);
  (void)DrawTextInsideList(stock_menu_kg_label);
  (void)Do3x3ButtonStuff(5);
  if (depot->what_is_stolen) {
    (void)snprintf((char *)strbuf,
        sizeof(strbuf),
        (const char *)depot_stolen_format,
        g_stock_name_kind[depot->what_is_stolen - 1]);
    DisplayErrorMessage(0, strbuf);
    depot->what_is_stolen = 0;
  }
  u16 event = WaitForMouseClick();
  mouse_pressed = event;
  if (event >= PT_MENU_BUTTON_1 && event <= PT_MENU_BUTTON_4)
    stock_kind = (u8)(event - PT_MENU_BUTTON_1);
  else if (event == PT_MENU_BUTTON_5) {
    HandleDepotWeapons(player_ptr->selected_property);
    goto summary;
  } else if (event == PT_MENU_BUTTON_6)
    goto done;
load:
  LoadBlitmapAndOffsets(1);
  (void)InitGraphicsMode();
menu:
  (void)Do3x3ButtonStuff(town == ptr_apple->town_index ? 22 : 28);
  remote_depot = (u8)(town != ptr_apple->town_index);
redraw:
  AdvanceClock(1);
  DrawDepotContents(depot, stock_kind, town);
wait:
  event = WaitForMouseClick();
dispatch:
  mouse_pressed = event;
  if (pt_is_menu_button_event(event))
    switch (event) {
    case PT_MENU_BUTTON_1:
    case PT_MENU_BUTTON_2:
      stock_kind = (u8)(event - PT_MENU_BUTTON_1);
      goto redraw;
    case PT_MENU_BUTTON_3:
      if (remote_depot) {
        stock_kind = 2;
        goto redraw;
      }
      {
        u8 country = GetDepotExportCountry();
        depot_export_country = (i8)country;
        if ((i8)country >= 0 && country != ptr_apple->town_index)
          HandleDepotExport((i8)country, stock_kind);
      }
      goto load;
    case PT_MENU_BUTTON_4:
      stock_kind = remote_depot ? 3 : 2;
      goto redraw;
    case PT_MENU_BUTTON_5:
      if (remote_depot)
        goto leave;
      stock_kind = 3;
      goto redraw;
    case PT_MENU_BUTTON_6:

      depot_export_country = -1;
      event = HandleDepotPriorities(pt_player(player));
      goto dispatch;
    case PT_MENU_BUTTON_7:
    case PT_MENU_BUTTON_8:
      (void)HandleStockShopMenu(NULL, stock_kind, (u8)(event - PT_MENU_BUTTON_6));
      (void)Do3x3ButtonStuff(22);
      goto redraw;
    case PT_MENU_BUTTON_9:
      goto leave;
    }
  if (event == 3849) {
    for (unsigned i = (unsigned)town + 1; i < 10; ++i)
      if ((depot = pt_optional_city_depot(player, (u8)i))) {
        town = (u8)i;
        goto menu;
      }
    for (unsigned i = 0; i < 10; ++i)
      if ((depot = pt_optional_city_depot(player, (u8)i))) {
        town = (u8)i;
        goto menu;
      }
  }
  goto wait;
leave:
  (void)Copy320x200Bitmap(current_bitmap, old_bitmap);
done:

  return 1;
}

// Depots reuse the POI's final sixteen bytes as eight weapon WORDs. This
// includes the two WORDs normally interpreted as total_customer_count.

_Static_assert(sizeof(((Poi *)0)->weapon_quantity) == 8 * sizeof(u16), "depot weapon view extent");

void HandleDepotWeapons_DrawElem(Poi *poi, i8 row, u8 selected) {
  if (row >= 0 && row < 8) {
    FillRect((u8)(selected ? 15 : 13), 48, (i16)(9 * row + 75), 165, (i16)(9 * row + 83));
    temp_canvas->color = 0;
    (void)SetTempCanvasPosition(50, (i16)(9 * row + 83));
    u8 *name;
    name = GetTranslatedString((u8)(row + 3));
    DrawListElement(name);
    cur_list_elem_x = 135;
    DrawNumberInsideListRight(player_ptr->num_weapons[(u8)row]);
    cur_list_elem_x = 160;
    DrawNumberInsideListRight(*pt_depot_weapon_quantity(poi, (u8)row));
  }
}
void HandleDepotWeapons(Poi *poi) {
  draw_list_elem_style = 0;
  (void)LoadLangFile(&langfile_waffen);
  FillLetterBackground(4, 14, 61);
  (void)Do3x3ButtonStuff(17);
  FillRect(13, 45, 56, 170, 150);
  HandleDepotWeapons_DrawShade(45, 47, 126);
  HandleDepotWeapons_DrawShade(45, 150, 126);
  temp_canvas->color = 0;
  (void)SetTempCanvasPosition(50, 68);
  DrawListElement(depot_weapon_category_label);
  cur_list_elem_x = 145;
  DrawListElement(stock_menu_depot_title);
  cur_list_elem_x = 120;
  DrawListElement(depot_weapon_bag_label);
  for (i8 i = 0; i < 8; ++i)
    HandleDepotWeapons_DrawElem(poi, i, 0);
  SaveAndSetInputEventFilter(5);
  i8 previous = -1, selected = -1;
  for (;;) {
    if (selected != previous)
      HandleDepotWeapons_DrawElem(poi, previous, 0);
    HandleDepotWeapons_DrawElem(poi, selected, 1);
    (void)WaitForMouseClick();
    previous = selected;
    i32 row = GetSelectedListItem(75, 9);
    selected = (i8)((u8)row);
    if (pt_is_menu_button_event(mouse_pressed))
      break;
    if (selected < 0 || selected >= 8)
      continue;
    Player *player = player_ptr;
    u16 *slot = pt_depot_weapon_quantity(poi, (u8)selected);
    if (mouse_pressed == 2 && player->num_weapons[(u8)selected]) {
      player->num_weapons[(u8)selected] = (u8)((u32)(player->num_weapons[(u8)selected]) - 1u);
      ++(*slot);
    }
    if (mouse_pressed == 1 && (*slot) && player->num_weapons[(u8)selected] != 255) {
      player->num_weapons[(u8)selected] = (u8)((u32)(player->num_weapons[(u8)selected]) + 1u);
      --(*slot);
    }
  }
  SaveAndSetInputEventFilter(3);
  FreeLangFile(&langfile_waffen);
}

u8 *CutoffString(const u8 *source, u8 maxlen) {
  (void)pt_strcpy(temp_cutoff, source);
  temp_cutoff[maxlen] = 0;
  return temp_cutoff;
}
void NotifyWarehouseFull(void) {
  ShowMessage(16, depot_capacity_exhausted_text);
  Delay(10);
}
void DrawDepotPurchaseCaptions(const Depot *depot, const Supplier *supplier, u8 mode) {
  if (mode < 2 && !memchr(current_town->name, 0, sizeof(current_town->name)))
    pt_resource_bounds_failure("unterminated town name", sizeof(current_town->name), sizeof(current_town->name));
  (void)depot;

  const Supplier *s = supplier;

  DepotDragStockBg(s->stock_kind);
  SetCurrentFont(font_piznumb);
  temp_canvas->color = 11;
  if (mode < 2) {
    (void)snprintf((char *)strbuf,
        sizeof(strbuf),
        (const char *)depot_purchase_title_format,
        mode ? depot_sale_label : depot_purchase_label,
        current_town->name);
  } else {
    (void)snprintf(
        (char *)strbuf, sizeof(strbuf), (const char *)depot_export_title_format, town_names[(u8)depot_export_country]);
  }
  (void)DrawText2(60, 190, 26, strbuf);
  SetCurrentFont(font_supersma);
  (void)SetTempCanvasPosition(88, 34);
  draw_list_elem_style = 0;
  if (mode < 2)
    DrawListElement(shop_price_label);
  cur_list_elem_x = 112;
  const u8 *label = depot_supplier_column_label;
  if (mode >= 2) {
    label = CutoffString(town_names[ptr_apple->town_index], 5);
  }
  DrawListElement(label);
  cur_list_elem_x = 142;
  DrawListElement(shop_amount_label);
  cur_list_elem_x = 170;
  label = depot_store_column_label;
  if (mode >= 2)
    label = CutoffString(town_names[(u8)depot_export_country], 5);
  DrawListElement(label);
  SetCurrentFont(some_font_ptr);
  temp_canvas->color = 0;
  (void)DrawText(90, 179, contract_sum_label);
  for (u8 i = 0; i < num_ingredients_in_set[s->stock_kind]; ++i) {
    (void)SetTempCanvasPosition(103, (i16)(6u * i + 42u));
    if (mode < 2) {
      i32 price = GetPriceForIngredient(supplier, i);
      if (mode)
        price = 80 * price / 100;
      DrawSupplierStockPrice(price);
    }
  };
}
void DrawDepotPurchaseStuff(const Supplier *supplier, const Depot *depot, i8 index, u8 shade, u8 selling) {
  const Supplier *s = supplier;

  SetCurrentFont(font_supersma);
  if (index >= 0 && index < num_ingredients_in_set[s->stock_kind] && shade >= 10) {
    unsigned i = (u8)index;
    FillRect((u8)shade, 116, (i16)(6 * index + 37), 194, (i16)(6 * index + 42));
    temp_canvas->color = 0;
    (void)SetTempCanvasPosition(139, (i16)(6 * index + 43));
    DrawNumberInsideListRight((u16)s->num_items_left[i]);
    (void)SetTempCanvasPositionX(166);
    DrawNumberInsideListRight(purchase_amount[i]);
    (void)SetTempCanvasPositionX(193);
    DrawNumberInsideListRight(
        (selling ? -1 : 1) * purchase_amount[i] + (u16)depot->stock[24u * s->stock_kind + i].stock);
  };
}

void HandleBuyStockToDepot(u8 player, u8 mode, Supplier *supplier) {
  u8 previous, repeated;

  Supplier *s = supplier;
  u8 kind = s->stock_kind, selling = mode % 2, exporting = mode >> 1;

  previous = 255;

  u8 city = exporting ? (u8)depot_export_country : ptr_apple->town_index;

  Depot *depot = pt_optional_city_depot(player, city);
  double factor = selling ? 0.8 : 1.0;

  SaveAndSetInputEventFilter(5);
  ClearDepotPurchases();
  (void)Do3x3ButtonStuff(10);
  DrawDepotPurchaseCaptions(depot, supplier, mode);
  for (i8 i = 0; i < 24; ++i)
    DrawDepotPurchaseStuff(supplier, depot, i, 162, selling);
  i8 selected = -1;
  u16 event, held, delta;
  i32 row;
wait:
  event = WaitForMouseClick();
  mouse_pressed = event;
  repeated = 0;
repeat:
  UpdateTotalPurchasePrice(num_ingredients_in_set[kind]);
  row = GetSelectedListItem(38, 6);
  selected = (i8)((u8)row);
  if ((u8)selected != previous)
    DrawDepotPurchaseStuff(supplier, depot, (i8)previous, 162, selling);
  DrawDepotPurchaseStuff(supplier, depot, selected, (u8)(14u - (u8)mouse_pressed), selling);
  if (mouse_pressed == 1) {
    held = WantSkipGameIntro();
    if (held) {
      row = GetSelectedListItem(38, 6);
      selected = (i8)((u8)row);
      if (selected < 0 || selected >= num_ingredients_in_set[kind])
        goto wait;
      unsigned i = (u8)selected;
      const DepotEntry *entry = &depot->stock[24u * kind + i];
      if (selling && entry->stock) {
        u8 stock_quality;
        i32 quality;
        quality = GetSupplierQuality(entry->quality);
        stock_quality = (u8)quality;
        quality = GetSupplierQuality(s->quality);
        if ((u8)quality > stock_quality) {
          ShowMessage(16, depot_quality_rejection_text);
          Delay(10);
          goto wait;
        }
      }

      held = MouseThingy();
      delta = (u16)(24u * held + 1u);
      i32 change = selling ? delta : -(i32)delta;

      purchase_amount[i] = (i16)((u16)purchase_amount[i] + (u32)change);
      if (purchase_amount[i] < 0) {
        delta = (u16)(delta + (u16)purchase_amount[i]);
        purchase_amount[i] = 0;
      }
      if (selling) {
        u32 stock = (u16)entry->stock;

        if (purchase_amount[i] > (i32)stock) {
          u32 excess = (u32)purchase_amount[i] - stock;

          delta = (u16)(delta - excess);
          purchase_amount[i] = (i16)((u16)stock);
        }
      }
      s->num_items_left[i] = (i16)((u16)s->num_items_left[i] + delta);
    }
  }
  if (mouse_pressed == 2) {
    held = MouseThingy();
    if (held) {
      row = GetSelectedListItem(38, 6);
      selected = (i8)((u8)row);
      if (selected < 0 || selected >= num_ingredients_in_set[kind])
        goto wait;
      unsigned i = (u8)selected;
      if ((!purchase_amount[i] && selling) || ((u16)s->num_items_left[i] < 500 && !exporting))
        goto prices;
      held = WantSkipGameIntro();
      delta = (u16)(24u * held + 1u);
      // Original branch spins without yielding at insufficient export
      // stock. It cannot produce a verified command completion.

      i32 change = selling ? -(i32)delta : delta;

      purchase_amount[i] = (i16)((u16)purchase_amount[i] + (u32)change);
      if (purchase_amount[i] < 0) {
        delta = (u16)(delta + (u16)purchase_amount[i]);
        purchase_amount[i] = 0;
      }
      if (!selling) {
        u32 sum = SumListOfWords(purchase_amount, num_ingredients_in_set[kind]);
        if ((u32)((u32)depot->in_use + sum) > (u32)depot->total_size) {
          NotifyWarehouseFull();
          purchase_amount[i] = (i16)((u16)purchase_amount[i] - delta);
          goto wait;
        }
      }

      if (delta > (u16)s->num_items_left[i])
        purchase_amount[i] = (i16)((u16)purchase_amount[i] - (selling ? delta : -(i32)delta));
      else
        s->num_items_left[i] = (i16)((u16)s->num_items_left[i] - delta);
    }
  }
prices:
  if (selected >= 0 && selected < num_ingredients_in_set[kind]) {
    unsigned i = (u8)selected;

    u16 price = GetPriceForIngredient(supplier, (u8)i);

    shop_purchase_price[i] =
        (float)((double)price * (float)purchase_amount[i] * (float)factor / depot_trade_price_divisor);

    u32 value = (u16)depot->stock[24u * kind + i].value;

    shop_purchase_secondary_price[i] = (float)((double)value * purchase_amount[i] / depot_trade_price_divisor);
  }
  previous = (u8)selected;
  if (mouse_pressed == 1) {
    held = WantSkipGameIntro();
    if (held)
      goto held_repeat;
  }
  if (mouse_pressed == 2) {
    held = MouseThingy();
    if (held)
      goto held_repeat;
  }
  if (!pt_is_menu_button_event(mouse_pressed))
    goto wait;
  if (mouse_pressed == PT_MENU_BUTTON_1) {
    if (exporting)
      goto done;
    if (selling) {
      if (current_purchase_amount < shop_secondary_purchase_amount) {
        TransferPlayerMoney(player, 5, 4, current_purchase_amount);
        TransferPlayerMoney(player, 10, 4, (float)((double)shop_secondary_purchase_amount - current_purchase_amount));
      } else {
        TransferPlayerMoney(player, 5, 4, shop_secondary_purchase_amount);
        TransferPlayerMoney(player, 5, 10, (float)((double)current_purchase_amount - shop_secondary_purchase_amount));
      }
    } else {
      if (!(u16)TransferPlayerMoney(player, 4, 5, current_purchase_amount))
        goto wait;

      u32 rating;
      double integral;
      integral = TruncateX87Integral((double)current_purchase_amount / depot_trade_rating_divisor);
      rating = (u32)(i32)integral;
      (void)ChangeCurrentPlayerSocialRating((i16)((u16)rating));
      u32 expense = (u32)ramtown.total_expense_per_player[player];

      integral = TruncateX87Integral((double)expense + current_purchase_amount);

      ramtown.total_expense_per_player[player] = pt_signed_dword((u32)(u64)(i64)integral);

      integral = TruncateX87Integral(current_purchase_amount);

      u32 cost = (u32)(u64)(i64)integral;
      AddCostForPlayer(player, cost, 3);
      u32 bought = (u32)s->total_bought_by_player[player];

      integral = TruncateX87Integral((double)bought + current_purchase_amount);

      s->total_bought_by_player[player] = pt_signed_dword((u32)(u64)(i64)integral);
    }
    for (u8 i = 0; i < num_ingredients_in_set[kind]; ++i)
      if (purchase_amount[i]) {
        if (selling)
          (void)DepotExport_RemoveOld(player, ptr_apple->town_index, kind, i, (u16)purchase_amount[i]);
        else {
          u16 price = GetPriceForIngredient(supplier, i);
          DepotExport_AddNew(player, ptr_apple->town_index, kind, i, s->quality, (u16)purchase_amount[i], price);
        }
      }
  } else if (mouse_pressed == PT_MENU_BUTTON_2) {
    // Original cancel adds the selected quantities even in sale mode.

    for (u8 i = 0; i < num_ingredients_in_set[kind]; ++i)
      s->num_items_left[i] = (i16)((u16)s->num_items_left[i] + (u16)purchase_amount[i]);
  } else
    goto wait;
  goto done;
held_repeat:
  Delay(2);
  if (!repeated) {
    repeated = 1;
    Delay(8);
  }
  goto repeat;
done:
  AdvanceClock(2);
  SaveAndSetInputEventFilter(3);
  ;
}
void HandleDepotExport(i8 country, u8 kind) {
  Supplier original, fake;
  u32 total;

  Depot *depot = pt_optional_city_depot(current_player_index, ptr_apple->town_index);

  total = 0;
  depot_export_country = country;

  // Separately labelled original-binary repair: remove stale supplier fields.

  fake = (Supplier){0};
  fake.stock_kind = kind;
  // The repaired release initializes every supplier field before pricing.

  for (u8 i = 0; i < num_ingredients_in_set[kind]; ++i)
    fake.num_items_left[i] = depot->stock[24u * kind + i].stock;
  original = fake;
  HandleBuyStockToDepot(current_player_index, 2, &fake);
  for (u8 i = 0; i < num_ingredients_in_set[kind]; ++i) {
    u16 amount = (u16)((u16)original.num_items_left[i] - (u16)fake.num_items_left[i]);
    total += amount;

    (void)DepotExport_RemoveOld(current_player_index, ptr_apple->town_index, kind, i, amount);
    const DepotEntry *entry = &depot->stock[24u * kind + i];
    DepotExport_AddNew(current_player_index, (u8)country, kind, i, entry->quality, amount, (u16)entry->value);
  }

  savegame_sect_0.transatlantic_score[current_player_index] += total;
  (void)ChangeCurrentPlayerSocialRating((i16)((u16)(total / 1000u)));
  ;
}

// Stock contract.

// Each signed product is accumulated with a binary32 store, as in the game.

void InitializeContract(Contract *contract, const Supplier *supplier, const Rest *restaurant, const i16 *amounts,
    u8 discount, i32 penalty, u16 charge, u8 term) {
  u8 index = 0;
  current_purchase_amount = 0;
  for (; index < 24; ++index) {
    u16 weight = (u16)amounts[index];
    contract->amount_of_each[index] = (i16)weight;
    u16 price = GetPriceForIngredient(supplier, index);
    i32 product = pt_signed_dword((u32)weight * price);
    double previous = current_purchase_amount;
    current_purchase_amount = (float)((double)product / contract_price_divisor + previous);
  }
  i64 integral = (i64)TruncateX87Integral(current_purchase_amount);
  contract->sum = pt_signed_dword((u32)(u64)integral);
  contract->discount = discount;
  contract->penalty = penalty;
  contract->charge = (i16)charge;
  contract->term_length = term;
  contract->restaurant_id = restaurant->restaurant_id;
  contract->supplier = (Supplier *)supplier;
  contract->player_index = current_player_index;
}

// Choose four distinct entries without changing the original RNG call order.

u32 GetSupplierContractPhoneAlternatives(u8 *output, u8 count, u8 base) {
  u8 selected[50] = {0}, index = 0;
  u32 result = 0;
  if (count < 4 || (unsigned)base + count > sizeof(selected))
    abort();
  for (; index < 4; ++index) {
    do {
      result = RandomNumber(count);
      result = (u8)(base + (u8)result);
    } while (selected[result]);
    output[index] = (u8)result;
    selected[result] = 1;
  }
  return result;
}

// Contracts daily.

#include "state_types.h"

// Retained execution proves actual children as they are reached. Whole direct
// commands remain unsupported until their future finance/message proof exists.

static u16 add(Rest *rest, u8 set, u8 item, u16 amount, u8 quality, u16 value) {
  return AddStockToRestaurantInventory(rest, set, item, amount, quality, value);
}
static void format(const Supplier *supplier, const char *pattern, const char *suffix) {
  suffix ? snprintf((char *)strbuf, sizeof(strbuf), pattern, supplier->name, suffix)
         : snprintf((char *)strbuf, sizeof(strbuf), pattern, supplier->name);
}
// Signed quality controls shortage chance; unsigned quality controls delivered stock.

void RunContractsDailyLoop(Contract *contract) {
  Rest *rest;

  rest = FindRestaurantById(contract->restaurant_id);

  Supplier *supplier = contract->supplier;
  u8 kind = supplier->stock_kind;

  if (!contract->term_length)
    format(supplier, "The contract with %s has expired!", NULL);
  else {
    u8 shortage;
    int signed_quality = (i8)supplier->quality;
    int shifted = signed_quality >= 0 ? signed_quality / 4 : -((-signed_quality + 3) / 4);
    shortage = RandomChance(25 - shifted);
    if (shortage) {
      supplier->had_shortages[rest->player_index] = 30;
      format(supplier, "%s has not delivered today!", NULL);
    } else {
      u32 extra = 0;
      bool did_extra = false;

      for (u8 item = 0; item < num_ingredients_in_set[supplier->stock_kind]; ++item) {
        u16 amount = (u16)contract->amount_of_each[item];

        if (amount) {
          unsigned index = (u8)(24u * kind + item);
          i32 wanted = 2u * rest->stock[index].need;

          wanted -= (u16)rest->stock[index].weight / 100;

          // The percent argument is already pushed during pricing.

          u16 value = GetPriceForIngredient(contract->supplier, item);
          i32 discounted;
          discounted = MulDiv100(value, 100 - (int)contract->discount);
          u16 received = add(rest, supplier->stock_kind, item, amount, supplier->quality, (u16)discounted);

          i32 remaining = wanted - received;

          if (contract->amount_mode && remaining > 0) {
            value = GetPriceForIngredient(contract->supplier, item);
            u16 count = add(rest, supplier->stock_kind, item, (u16)remaining, supplier->quality, value);

            if (count) {
              value = GetPriceForIngredient(contract->supplier, item);
              extra += (u32)(pt_signed_dword((u32)count * value) / 100);

              did_extra = true;
            }
          }
        }
      }
      supplier->total_bought_by_player[current_player_index] =
          pt_signed_dword((u32)supplier->total_bought_by_player[current_player_index] + (u32)contract->sum);
      i32 discounted;
      discounted = MulDiv100(contract->sum, 100 - (int)contract->discount);
      u32 cost = extra + (u32)discounted;
      if (did_extra)
        cost += (u16)contract->charge;

      float amount = (float)pt_signed_dword(cost);

      for (unsigned transfer = 0; transfer < 2; ++transfer) {
        u32 paid;
        paid = TransferPlayerMoney(current_player_index, transfer ? 8 : 4, transfer ? 5 : 8, amount);
        if (transfer) {
          if ((u16)paid) {
            ramtown.total_expense_per_player[current_player_index] =
                pt_signed_dword((u32)ramtown.total_expense_per_player[current_player_index] + cost);
            AddCostForRestaurant(rest, cost, 3);
            rest->weekly_ingredient_type_cost[supplier->stock_kind] =
                (i16)((u16)((u16)rest->weekly_ingredient_type_cost[supplier->stock_kind] + cost));
            format(supplier, "delivery from %s arrived. %s", " ");
          } else {
            supplier->current_account[current_player_index] =
                pt_signed_dword((u32)supplier->current_account[current_player_index] + cost);
            player_ptr->bankruptcy_reason_hint = 4;

            format(supplier,
                "delivery from %s arrived. %s",
                "Because of insolvency the amount will be credited to the supplier's current account.");
          }
        }
      }
    }
  }
  u8 minutes = (u8)(4u * supplier->stock_kind + 54u);
  ShowDelayedMessage(rest->player_index, rest->restaurant_id, minutes, strbuf);
}

// Depot inventory.

// The target chooses the depot; the current player chooses restaurants.
// Only valid ingredients are written, including zero for an empty list.

void UpdateCityDepotNeedValues(u8 target) {
  Depot *depot = pt_optional_city_depot(target, ptr_apple->town_index);
  if (depot)
    for (unsigned set = 0; set < 4; ++set) {
      for (unsigned item = 0; item < 24; ++item) {
        unsigned index = 24u * set + item;
        u16 valid = IsValidIngredient((u8)index);
        if (valid) {
          u32 total = 0;
          const PTForwardLink *node = player_ptr->restaurants.begin.next;
          while (node->next) {
            const Rest *rest = (Rest *)node;
            if (rest->restaurant_opened)
              total += rest->stock[index].need;
            node = rest->linked.next;
          }
          depot->stock[index].need = (i16)((u16)total);
        }
      }
    }
}
u16 pt_add_restaurant_stock(Rest *rest, unsigned index, u16 amount, u8 quality, u16 value) {
  if (amount) {
    RestaurantStock *stock = &rest->stock[index];
    u32 old = (u16)stock->weight, added = 100u * amount;
    if (old + added > 65535u)
      added = 65535u - old;
    u32 weighted_quality = old * stock->quality + added * quality;
    u32 weighted_value = old * (u16)stock->value + added * value;
    u16 total = (u16)(old + added);
    stock->weight = (i16)total;
    if (total) {
      stock->quality = (u8)(weighted_quality / total);
      stock->value = (i16)((u16)(weighted_value / total));
    }
    rest->average_purchase_quality =
        rest->average_purchase_quality ? (u8)((rest->average_purchase_quality + quality) >> 1) : quality;
  }
  // The stock return is the original request, including when weight capped.
  // CopyFromDepotToRest consequently does not return excess depot goods.

  return amount;
}

u16 AddStockToRestaurantInventory(Rest *rest, u8 set, u8 item, u16 amount, u8 quality, u16 value) {
  u16 result = pt_add_restaurant_stock(rest, 24u * set + item, amount, quality, value);
  return result;
}
u16 DepotExport_RemoveOld(u8 player, u8 town, u8 set, u8 item, u16 amount) {
  Depot *depot = pt_optional_city_depot(player, town);
  u16 result = 0;
  if (depot) {
    DepotEntry *stock = &depot->stock[24u * set + item];
    result = (u16)stock->stock < amount ? (u16)stock->stock : amount;
    stock->stock = (i16)((u16)((u16)stock->stock - result));
    depot->in_use = pt_signed_dword((u32)depot->in_use - result);
  }
  return result;
}
unsigned pt_add_depot_stock(Depot *depot, unsigned index, u16 amount, u8 quality, u16 value) {
  if (!amount)
    return 0;
  u16 added = amount;
  if ((u32)depot->in_use + amount > (u32)depot->total_size)
    added = (u16)((u16)depot->total_size - (u16)depot->in_use);
  DepotEntry *stock = &depot->stock[index];
  u16 old = (u16)stock->stock;
  if (old) {
    // Separately labelled original repair changes only zero-denominator
    // word wrap. Every other stock case preserves the pristine arithmetic.

    if (!(u16)(old + added))
      --added;
    u32 weighted_quality = (u32)old * stock->quality + (u32)added * quality;
    u32 weighted_value = (u32)old * (u16)stock->value + (u32)added * value;
    u16 total = (u16)(old + added);
    if (!total)
      // Stock DIV faults after the word stock wraps.
      return 1;
    stock->stock = (i16)total;
    stock->quality = (u8)(weighted_quality / total);
    stock->value = (i16)((u16)(weighted_value / total));
    stock->last_purchase_value = (i16)value;
  } else {
    stock->stock = (i16)added;
    stock->quality = quality;
    stock->value = stock->last_purchase_value = (i16)value;
  }
  depot->in_use = pt_signed_dword((u32)depot->in_use + added);
  return 0;
}

void DepotExport_AddNew(u8 player, u8 town, u8 set, u8 item, u8 quality, u16 amount, u16 value) {
  Depot *depot = pt_optional_city_depot(player, town);
  if (depot && amount) {
    unsigned status = pt_add_depot_stock(depot, 24u * set + item, amount, quality, value);
    if (status)
      // Original zero-denominator fault outside the repaired profile.
      abort();
  }
}
void CopyFromDepotToRest(Rest *rest, u8 set, u8 item, u16 amount) {
  Depot *depot = pt_optional_city_depot(rest->player_index, ptr_apple->town_index);
  if (depot) {
    const DepotEntry *entry = &depot->stock[24u * set + item];
    u16 removed = DepotExport_RemoveOld(rest->player_index, ptr_apple->town_index, set, item, amount);
    if (removed) {
      u16 used = AddStockToRestaurantInventory(rest, set, item, removed, entry->quality, (u16)entry->value);
      u16 remaining = (u16)(removed - used);
      if (remaining)
        DepotExport_AddNew(
            rest->player_index, ptr_apple->town_index, set, item, entry->quality, remaining, (u16)entry->value);
    }
  }
}

// Manager prices.

#pragma STDC FP_CONTRACT OFF
static void price_steps(const Supplier *supplier, u8 item, float *price_out, float *addon_out) {
  volatile float price = (float)current_town->base_stock_prices[supplier->stock_kind][item];
  volatile double addon_ratio = (double)supplier->price_addon[item] / supplier_price_addon_divisor;
  volatile float addon = (float)addon_ratio;
  double factor = 1;
  switch (ptr_apple->difficulty_level) {
  case 1:
    factor = supplier_price_level1_factor;
    break;
  case 2:
    factor = supplier_price_level2_factor;
    break;
  case 4:
    factor = supplier_price_level4_factor;
    break;
  case 5:
    factor = supplier_price_level5_factor;
    break;
  default:
    break;
  }
  if (factor != 1) {
    volatile double scaled = (double)price * factor;
    price = (float)scaled;
  }
  *price_out = price;
  *addon_out = addon;
}

u16 GetPriceForIngredient(const Supplier *supplier, u8 item) {
  float price, addon;
  price_steps(supplier, item, &price, &addon);

  volatile double product = (double)price * addon;

  i32 result = (i32)TruncateX87Integral(product);

  return (u16)result;
}
i32 MgrAdjustPizzaPrice(RestoRecipe *recipe, u8 skill, u8 policy) {
  i32 difference = 0;
  if (recipe->orders == recipe->sales || skill <= 70) {
    u32 count = skill < 30 ? recipe->sales : recipe->orders;
    u32 previous = (u32)recipe->byte1 * (u16)recipe->last_price_per_pizza;
    u32 current = count * (u16)recipe->price_per_pizza;
    if (skill > 60) {
      previous -= (u32)recipe->byte1 * (u16)recipe->costs_per_pizza;
      current -= count * (u16)recipe->costs_per_pizza;
    }
    difference = pt_signed_dword(current - previous);
    if (policy) {
      if (difference < -5000)
        recipe->price_per_pizza = (i16)((u16)((u16)recipe->price_per_pizza - 40u));
      if (difference > 2000)
        recipe->price_per_pizza = (i16)((u16)((u16)recipe->price_per_pizza + 30u));
    }
  }

  return difference;
}
void RestHandleTurnoverChange(Rest *rest, u8 skill) {
  if (rest->delivcapa_per_day[6] < 80 && rest->mgr_change_prices) {
    for (unsigned i = 0; i < rest->num_recipes; ++i) {
      rest->recipes[i].price_per_pizza = (i16)((u16)((u16)rest->recipes[i].price_per_pizza + 100u));
    }
  }

  for (unsigned i = 0; i < rest->num_recipes; ++i) {
    MgrAdjustPizzaPrice(&rest->recipes[i], skill, rest->mgr_change_prices);
  }
}

// Manager shopping.

#pragma STDC FP_CONTRACT OFF
static Supplier *live_resolve_supplier(PTManagerShopping *c, unsigned set) {
  Supplier *selected = ResolveSupplier(c->rest.last_supplier_of_stock[set]);
  return selected;
}

static u32 live_random_number(PTManagerShopping *c, u32 range) {
  (void)c;

  u32 result = RandomNumber(range);

  return result;
}

static u8 manager_shopping_live_chance(PTManagerShopping *c, u8 probability) {
  (void)c;

  u8 result = RandomChance(probability);

  return result;
}

static u32 live_percent(PTManagerShopping *c, u32 value, int percent) {
  (void)c;

  u32 result = (u32)MulDiv100(pt_signed_dword(value), percent);

  return result;
}

static bool live_pay(PTManagerShopping *c, u32 amount) {
  float single = (float)amount;

  bool paid = (u16)TransferPlayerMoney(c->rest.player_index, 4, 5, single) != 0;

  return paid;
}

static void manager_shopping_live_message(PTManagerShopping *c, const u8 *text, size_t n, unsigned set, bool manager) {
  memcpy(strbuf, text, n + 1u);

  if (manager)
    ShowManagerMessage(c->rest.player_index, c->rest.restaurant_id, c->manager.face, (u8)(set + 54), 50, strbuf);
  else
    ShowDelayedMessage(c->rest.player_index, 100, (u8)(set + 60), strbuf);
}

static void live_begin(PTManagerShopping *c) {
  special_debt_flag = c->rest.mgr_overdraw_acct;
}

static u16 live_days_left(PTManagerShopping *c, unsigned index) {
  u16 days = ComputeStockDaysLeft(&c->restaurant->stock[index]);
  return days;
}

static u32 live_price(PTManagerShopping *c, const Supplier *s, unsigned item) {
  (void)c;
  return GetPriceForIngredient(s, (u8)item);
}

static void live_purchase(PTManagerShopping *c, Supplier *supplier, u32 amount, u32 value) {
  AddCostForRestaurant(c->restaurant, amount, 3);

  Supplier *out = supplier;
  out->total_bought_by_player[c->rest.player_index] =
      pt_signed_dword((u32)out->total_bought_by_player[c->rest.player_index] + value);
  c->rest = *c->restaurant;
}

static void live_weekly_cost(PTManagerShopping *c, unsigned set) {
  (c->restaurant)->weekly_ingredient_type_cost[set] = c->rest.weekly_ingredient_type_cost[set];
}

static void live_publish_name(PTManagerShopping *c) {
  GetRestaurantName(c->restaurant);
}

static void live_inventory(PTManagerShopping *c, unsigned set, unsigned item, u16 quantity, u8 quality, u16 new_value) {
  AddStockToRestaurantInventory(c->restaurant, (u8)set, (u8)item, quantity, quality, new_value);
}

static void live_refresh(PTManagerShopping *c) {
  c->rest = *c->restaurant;
}

static void manager_shopping_live_finished(PTManagerShopping *c) {
  (void)c;
  special_debt_flag = 0;
}
static const PTManagerShoppingServices shopping_live_services = {
  .resolve_supplier = live_resolve_supplier,
  .random_number = live_random_number,
  .chance = manager_shopping_live_chance,
  .percent = live_percent,
  .pay = live_pay,
  .message = manager_shopping_live_message,
  .days_left = live_days_left,
  .price = live_price,
  .purchase = live_purchase,
  .weekly_cost = live_weekly_cost,
  .publish_name = live_publish_name,
  .inventory = live_inventory,
  .refresh = live_refresh,
  .begin = live_begin,
  .finished = manager_shopping_live_finished,
};

static Supplier *supplier(PTManagerShopping *c, unsigned set) {
  Supplier *selected = NULL;
  selected = c->services->resolve_supplier(c, set);
  if (!selected)
    for (unsigned i = 0; i < c->count; ++i)
      if (c->suppliers[i]->stock_kind == set) {
        selected = c->suppliers[i];
        break;
      }
  if (!selected)
    c->status = 1;
  return selected;
}
void pt_manager_shopping_run(PTManagerShopping *c) {
  u16 quantity[96] = {0};
  u32 prices[24] = {0};
  c->services->begin(c);
  for (unsigned set = 0; set < 4 && !c->status; ++set) {
    if (num_ingredients_in_set[set] > 24) {
      c->status = 1;
      break;
    }
    Supplier *s = supplier(c, set);
    if (c->status)
      break;
    u8 quality = s->quality;
    u32 value = 0;
    unsigned orders = 0;

    for (unsigned item = 0; item < num_ingredients_in_set[set]; ++item) {
      unsigned index = 24u * set + item;
      const RestaurantStock *stock = &c->rest.stock[index];
      u16 days = c->services->days_left(c, index);
      if (days < 3 && c->rest.mgr_supply_shortage) {
        quantity[index] = (u16)(2u * stock->need);

        if (quantity[index]) {
          prices[item] = c->services->price(c, s, item);
          if (c->status)
            break;
          u32 total = prices[item] * quantity[index] / 100u;
          value += total;
          ++orders;
        }
      }
    }
    if (c->status)
      break;

    if (!value)
      continue;
    if (c->rest.mgr_supply_shortage == 2 && c->rest.mgr_shopping) {
      unsigned discount =
          c->services->chance(c, c->manager.experience) ? c->services->random_number(c, c->manager.motivation >> 1) : 0;
      u32 amount = c->services->percent(c, value, 100 - (int)discount);
      bool paid = c->services->pay(c, amount);
      if (c->status)
        break;
      if (paid) {
        c->services->purchase(c, s, amount, value);
        if (c->status)
          break;
        c->rest.weekly_ingredient_type_cost[set] = (i16)((u16)((u16)c->rest.weekly_ingredient_type_cost[set] + amount));
        c->services->weekly_cost(c, set);
        u8 rest_name[37], text[256];
        size_t length;
        c->status = pt_restaurant_name_text(&c->rest, rest_name, &length);
        if (c->status)
          break;
        if (!memchr(s->name, 0, sizeof(s->name)) || !memchr(g_stock_name_kind[set], 0, 16)) {
          c->status = 1;
          break;
        }
        c->services->publish_name(c);
        int n = snprintf((char *)text,
            sizeof(text),
            "Your manager goes shopping for %s and buys %s at %s for a total of $%d at %d%% discount.",
            rest_name,
            g_stock_name_kind[set],
            s->name,
            pt_signed_dword(value),
            (int)discount);
        if (n < 0 || n >= 256) {
          c->status = 1;
          break;
        }
        c->services->message(c, text, (size_t)n, set, false);
        if (c->status)
          break;
        for (unsigned item = 0; item < num_ingredients_in_set[set]; ++item) {
          u16 new_value = (u16)c->services->percent(c, prices[item], 100 - (int)discount);
          c->services->inventory(c, set, item, quantity[24u * set + item], quality, new_value);
        }
        c->services->refresh(c);
      }
    }
    if (c->rest.mgr_supply_shortage == 1 && orders) {
      if (!memchr(g_stock_name_kind[set], 0, 16)) {
        c->status = 1;
        break;
      }
      u8 text[256];
      int n = snprintf((char *)text,
          sizeof(text),
          orders == 1 ? "Boss! We urgently need %d order of %s for restaurant %d."
                      : "Boss! We urgently need %d orders of %s for restaurant %d.",
          (int)orders,
          g_stock_name_kind[set],
          c->rest.restaurant_id + 1);
      if (n < 0 || n >= 256) {
        c->status = 1;
        break;
      }
      c->services->message(c, text, (size_t)n, set, true);
    }
  }
  c->services->finished(c);
}
void RestHandleManagerShopping(Rest *rest, const Staff *manager) {
  if (!rest->staff_indexes[12])
    return;
  PTManagerShopping c = {.restaurant = rest,
    .rest = *rest,
    .manager = *manager,
    .rng = pt_random_state,
    .services = &shopping_live_services};

  for (PTForwardLink *node = current_town->suppliers.begin.next; node->next; node = node->next) {
    if (c.count == sizeof(c.suppliers) / sizeof(c.suppliers[0]))
      abort();
    c.suppliers[c.count++] = (Supplier *)node;
  }

  pt_manager_shopping_run(&c);
  if (c.status)
    abort();
}

// Manager duties.

static u8 manager_duties_live_chance(PTManagerDuties *c, unsigned probability) {
  (void)c;

  u8 result = RandomChance((i32)probability);

  return result;
}

static void live_motivate(PTManagerDuties *c, unsigned index, int delta) {
  (void)c;
  StaffAddMotivation(&g_staff[index], (u32)delta);
}

static void manager_duties_live_message(PTManagerDuties *c, const u8 *text, size_t n) {
  const Staff *manager = &c->staff[c->rest.staff_indexes[12]];

  memcpy(strbuf, text, n + 1);

  ShowManagerMessage(c->rest.player_index, c->rest.restaurant_id, manager->face, 54, manager->prop_6, strbuf);
}

static unsigned live_restaurant_name(PTManagerDuties *c, u8 name[37], size_t *length) {
  unsigned status = pt_restaurant_name_text(&c->rest, name, length);
  if (status)
    return status;
  GetRestaurantName(c->restaurant);
  return 0;
}

static const u8 *live_role_text(PTManagerDuties *c, unsigned role) {
  const PTManagerRoleText *label = pt_manager_role_text((u8)role);
  if (!memchr(label->text, 0, sizeof(label->text))) {
    c->status = 1;
    return NULL;
  }
  return label->text;
}

static void live_shopping(PTManagerDuties *c, unsigned manager_index) {
  RestHandleManagerShopping(c->restaurant, &g_staff[manager_index]);

  c->rest = *c->restaurant;
}

static void live_turnover(PTManagerDuties *c, u8 skill) {
  RestHandleTurnoverChange(c->restaurant, skill);
}

static bool live_valid_slot(PTManagerDuties *c, unsigned slot) {
  return ReIsValidStaffIndex(c->restaurant, (u8)slot);
}

static void live_staff_changed(PTManagerDuties *c, Staff *staff, unsigned index) {
  (void)c;
  g_staff[index] = *staff;
}

static void manager_duties_live_finished(PTManagerDuties *c) {
  (void)c;
}

static const PTManagerDutiesServices duties_live_services = {
  .chance = manager_duties_live_chance,
  .motivate = live_motivate,
  .message = manager_duties_live_message,
  .restaurant_name = live_restaurant_name,
  .role_text = live_role_text,
  .shopping = live_shopping,
  .turnover = live_turnover,
  .valid_slot = live_valid_slot,
  .staff_changed = live_staff_changed,
  .finished = manager_duties_live_finished,
};

static void motivate(PTManagerDuties *c, unsigned index, int delta) {
  Staff *staff = &c->staff[index];
  int value = (i16)((u16)(staff->motivation + (u32)delta));
  staff->motivation = (u8)(value < 0 ? 0 : value > 100 ? 100 : value);
  c->services->motivate(c, index, delta);
}
static void text_notice(PTManagerDuties *c, const char *format, unsigned role, const char *turnover) {
  u8 text[256], name[37];
  size_t length;
  int n;
  if (role) {
    const u8 *role_text = c->services->role_text(c, role);
    if (c->status)
      return;
    c->status = c->services->restaurant_name(c, name, &length);
    if (c->status)
      return;
    n = snprintf((char *)text, sizeof(text), format, name, (const char *)role_text);
  } else
    n = snprintf((char *)text, sizeof(text), format, c->rest.restaurant_id + 1, turnover);
  if (n < 0 || n >= 256) {
    c->status = 1;
    return;
  }
  c->services->message(c, text, (size_t)n);
}
void pt_manager_duties_run(PTManagerDuties *c) {

  unsigned manager_index = c->rest.staff_indexes[12];
  if (!manager_index)
    return;
  if (manager_index >= 200) {
    c->status = 1;
    return;
  }
  Staff *manager = &c->staff[manager_index];
  if (manager->absent_days)
    return;
  if (c->rest.manager_had_to_help) {
    text_notice(c,
        "Boss! Yesterday in restaurant %s I had to help out our %s with their work!",
        c->rest.manager_had_to_help,
        NULL);
    return;
  }
  if (c->rest.manager_flag_1 && c->rest.mgr_staff_shortage == 1) {
    text_notice(c, "Boss! In restaurant %s we haven't got enough %s!", c->rest.manager_flag_1, NULL);
    return;
  }
  if (!c->services->chance(c, manager->motivation))
    return;
  c->services->shopping(c, manager_index);
  if (c->status)
    return;
  u8 skill = manager->management;
  bool profit = skill > 60;
  if (c->services->chance(c, skill))
    return;
  if (c->rest.mgr_turnover_decrease == 2 || c->rest.mgr_turnover_increase == 2) {
    c->services->turnover(c, skill);
    if (c->status)
      return;
  }
  int difference = (u16)c->rest.sales_per_day[6] - (u16)c->rest.sales_per_day[5];
  if (profit)
    difference -= ((u16)c->rest.expenses_per_day[6] - (u16)c->rest.expenses_per_day[5]);
  const char *label = (const char *)manager_turnover_names[profit];
  if (!memchr(label, 0, 20)) {
    c->status = 1;
    return;
  }
  if (c->rest.mgr_turnover_decrease == 1 && difference > 500) {
    text_notice(c, "Boss! In restaurant no. %d the %s is going up.", 0, label);
    return;
  }
  if (c->rest.mgr_turnover_increase == 1 && difference < 200) {
    text_notice(c, "Boss! In restaurant no. %d the %s is going down.", 0, label);
    return;
  }
  for (unsigned slot = 0; slot < 12; ++slot) {
    bool valid = c->services->valid_slot(c, slot);
    if (!valid)
      continue;
    unsigned index = c->rest.staff_indexes[slot];
    if (index >= 200) {
      c->status = 1;
      return;
    }
    Staff *staff = &c->staff[index];
    switch (c->rest.mgr_staff_behavior) {
    case 0:
      // fall through
      motivate(c, index, -3);
    case 1:
      if (staff->trustworthiness < (unsigned)staff->experience + 10)
        staff->trustworthiness = (u8)(staff->trustworthiness + 2);
      motivate(c, index, -1);
      break;
    case 2:
      if (c->services->chance(c, manager->prop_7) && staff->health < (unsigned)staff->motivation + 10)
        ++staff->health;
      if (staff->trustworthiness < staff->experience)
        staff->trustworthiness = (u8)(staff->trustworthiness + c->services->chance(c, manager->prop_7));
      break;
    case 3:
      if (staff->motivation < 80 && c->services->chance(c, manager->prop_5))
        motivate(c, index, 1);
      break;
    case 4:
      if (manager->prop_7 > 50 && staff->motivation < (unsigned)staff->health + 10)
        motivate(c, index, 1);
      if (staff->prop_11 < (unsigned)staff->intelligence + 15)
        ++staff->prop_11;
      break;
    case 6:
      if (staff->motivation > 8)
        motivate(c, index, -1);
      if (staff->prop_11 > (unsigned)staff->trustworthiness + 8)
        // fall through
        --staff->prop_11;
    case 5:
      if (staff->trustworthiness > (unsigned)staff->motivation + 10)
        --staff->trustworthiness;
      break;
    default:
      break;
    }
    c->services->staff_changed(c, staff, index);
  }
  c->services->finished(c);
}
u8 RestDoManagerDuties(Rest *restaurant) {
  PTManagerDuties c = {
    .restaurant = restaurant, .rest = *restaurant, .rng = pt_random_state, .services = &duties_live_services};

  memcpy(c.staff, g_staff, sizeof(c.staff));

  pt_manager_duties_run(&c);
  if (c.status)
    abort();

  return 0;
}
