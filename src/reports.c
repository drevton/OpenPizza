// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
// Invoice.

#include "world.h"
#include "objects.h"
#include "reports.h"
#include "messages.h"
#include "text.h"
#include "player.h"
#include <stdio.h>
u16 ShowInvoice_Helper(i16 x, i16 y, u8 character, u8 count) {

  memset(strbuf, character, count);

  Canvas_SetPosition(temp_canvas, x, y);

  u16 result = Canvas_DrawText(temp_canvas, pt_text_span(strbuf, count));

  return result;
}
u16 ShowInvoice(Rest *rest, const u8 *caption) {

  static const u16 rects[3][5] = {{13, 36, 20, 179, 180}, {15, 37, 21, 178, 179}, {14, 41, 25, 174, 175}};

  for (unsigned i = 0; i < 3; ++i) {

    FillRect(rects[i][0], rects[i][1], rects[i][2], rects[i][3], rects[i][4]);
  }

  SetCurrentFont(font_astandar);

  SetSomeFieldInCanvas(0);

  SetFontColor(8);

  DrawText(80, 38, invoice_title);

  const u8 *name;
  if (rest) {

    name = GetRestaurantName(rest);

  } else
    name = player_ptr->name_text;

  snprintf((char *)strbuf, sizeof(strbuf), "for %s", (const char *)name);

  SetCurrentFont(some_font_ptr);

  DrawText2(12, 203, 50, strbuf);

  ShowInvoice_Helper(54, 56, 64, 23);

  u16 result = DrawText2(12, 203, 66, caption);

  cur_list_elem_y = 77;
  return result;
}
Canvas *DrawDollarsInsideList(i32 amount) {
  u16 alignment = Canvas_TextWidth(temp_canvas, pt_text_span(dollar_alignment_text, 2));
  i16 x = cur_list_elem_x;
  u8 bytes[32];
  (void)snprintf((char *)bytes, sizeof(bytes), "$%d", amount);
  u16 length = (u16)pt_strlen(bytes);
  u16 width = Canvas_TextWidth(temp_canvas, pt_text_span(bytes, length));
  SetTempCanvasPosition((i16)((u16)x + alignment - width), cur_list_elem_y);
  Canvas_DrawText(temp_canvas, pt_text_span(bytes, length));
  Canvas *result = SetTempCanvasPosition(x, cur_list_elem_y);
  return result;
}
void DrawStaffExpenses(const Staff *staff) {

  bool holiday = IsOnHoliday() != 0;

  if (holiday)
    return;

  SetCurrentFont(font_supersma);

  draw_list_elem_style = 1;
  cur_list_elem_x = 45;

  DrawListElement(staff->first_name);

  DrawListElement(staff->last_name);

  cur_list_elem_x = 165;

  DrawDollarsInsideList((u16)staff->salary);
}

PTCanvasResult DrawTextInsideList(const u8 *pointer) {

  PTCanvasResult result = pt_canvas_result_value((u16)cur_list_elem_x);
  if (pointer) {
    i16 x = cur_list_elem_x, y = cur_list_elem_y;

    SetTempCanvasPosition(x, y);

    u16 count = (u16)pt_strlen(pointer);

    Canvas_DrawText(temp_canvas, pt_text_span(pointer, count));

    result = pt_canvas_result_object(SetTempCanvasPosition(x, y));
  }

  return result;
}
void DrawNumberInsideListRight(i32 value) {

  i16 x = cur_list_elem_x;

  itoa_(value);

  u16 count = (u8)pt_strlen(strbuf);

  u16 width = Canvas_TextWidth(temp_canvas, pt_text_span(strbuf, count));

  temp_canvas->position_x = (i16)((u16)cur_list_elem_x - width);

  Canvas_DrawText(temp_canvas, pt_text_span(strbuf, count));

  cur_list_elem_x = (i16)((u16)x + 3u);
  temp_canvas->position_x = cur_list_elem_x;
}
void DrawStringBufferAsListElement(void) {

  DrawListElement(strbuf);
}
void DrawPercentageNumberInList(i32 value) {

  DrawNumberInsideListRight(value);

  DrawTextInsideList(list_percent_text);
}

u16 DrawCaptionShadowed(i16 x, i16 y, const u8 *text) {

  u16 count = (u16)pt_strlen(text);

  SetTempCanvasPosition(x, y);

  Canvas_SetColor(temp_canvas, 13);

  Canvas_DrawText(temp_canvas, pt_text_span(text, count));

  SetTempCanvasPosition((i16)((u16)x + 1u), (i16)((u16)y - 1u));

  Canvas_SetColor(temp_canvas, 0);

  u16 result = Canvas_DrawText(temp_canvas, pt_text_span(text, count));

  return result;
}

// Daily report.

#include "graphics.h"
#include "input.h"
#include "finance.h"
#include "frame.h"

static i32 sar_word(u16 bits, unsigned count) {
  i32 v = (i16)bits;
  return v < 0 ? -((-v + (1 << count) - 1) / (1 << count)) : v / (1 << count);
}
// Original pin RNG calls run below the already pushed X and Y arguments for DrawBob.

void DrawLetterPins(i16 l, i16 t, i16 r, i16 b, u8 grid) {
  u32 height = (u32)(i32)b - (u32)(i32)t, width = (u32)(i32)r - (u32)(i32)l;

  DrawBackground_Helper(l, t, r, b, grid);
  i32 dy = sar_word((u16)height, 4);
  i16 x[2] = {(i16)((u16)((u16)l + dy)), (i16)((u16)((u16)r - (dy + 10)))};
  i16 y = (i16)((u16)(sar_word((u16)width, 5) + t + 1));
  for (unsigned i = 0; i < 2; ++i) {
    u32 random;
    random = RandomNumber(4);
    u16 image = (u16)(2u * random + 26u + i);

    DrawBob(&bob_stanbobs, image, x[i], y);
  }
}

static u32 financial_value(const Rest *rest, unsigned row, unsigned index) {
  if (row == 0)
    return (u16)rest->demand_value_per_day[index];
  if (row == 1)
    return (u16)rest->sales_per_day[index];
  if (row == 2)
    return (u16)rest->expenses_per_day[index];
  if (row == 3)
    return (u32)(u16)rest->sales_per_day[index] - (u16)rest->expenses_per_day[index];
  return (u32)rest->expenditure_per_day[index - daily_balance_flag];
}

u16 ShowDailyBalanceStats(Rest *rest) {
  u8 current = (u8)(rest->use_real_demand - 1u), previous = (u8)(rest->use_real_demand - 2u);

  FillLetterBackground(5, 0, 7);
  DrawLetterPins(17, 20, 196, 183, 0);
  temp_canvas->color = 11;
  SetSomeFieldInCanvas(0);
  SetCurrentFont(font_ruby);
  DrawText(27, 45, daily_settlement_text);
  SetCurrentFont(font_piznumb);
  const u8 *name;
  name = GetRestaurantName(rest);
  DrawText2(12, 203, 32, name);
  for (unsigned i = 0; i < 2; ++i) {
    const i16 *values = i ? rest->pizza_count_per_day : rest->orders_per_day;
    temp_canvas->color = 13;
    (void)SetTempCanvasPosition(177, (i16)(75u + 14u * i));
    if (current)
      DrawNumberInsideListRight(pt_signed_dword((u16)values[5]));
    temp_canvas->color = 0;
    (void)SetTempCanvasPositionX(137);
    DrawNumberInsideListRight(pt_signed_dword((u16)values[6]));
  }
  (void)SetTempCanvasPosition(177, 103);
  temp_canvas->color = 13;
  if (rest->orders_per_day[5])
    DrawPercentageNumberInList((u8)(rest->delivcapa_per_day[5]));
  (void)SetTempCanvasPositionX(137);
  temp_canvas->color = 0;
  if (rest->orders_per_day[6])
    DrawPercentageNumberInList((u8)(rest->delivcapa_per_day[6]));
  for (unsigned row = 0; row < 5; ++row) {
    temp_canvas->color = 13;
    (void)SetTempCanvasPosition(179, (i16)(117u + 14u * row));
    if ((i8)previous >= 0)
      DrawDollarsInsideList(pt_signed_dword(financial_value(rest, row, 5)));
    temp_canvas->color = 0;
    (void)SetTempCanvasPositionX(137);
    if ((i8)current >= 0)
      DrawDollarsInsideList(pt_signed_dword(financial_value(rest, row, 6)));
  }
  temp_canvas->color = 14;
  (void)SetTempCanvasPosition(29, 65);
  (void)DefaultLineTo(184, 65);
  (void)SetTempCanvasPosition(108, 64);
  (void)DefaultLineTo(108, 175);
  (void)SetTempCanvasPosition(148, 64);
  (void)DefaultLineTo(148, 175);
  DrawText(daily_balance_flag ? 145 : 152, 63, (daily_balance_flag ? daily_day_before_text : daily_yesterday_text));
  temp_canvas->color = 0;
  DrawText(daily_balance_flag ? 100 : 110, 63, (daily_balance_flag ? daily_yesterday_text : daily_today_text));
  static const u8 *const labels[8] = {daily_orders_text,
    daily_pizzas_sold_text,
    daily_delivery_capacity_text,
    daily_demand_value_text,
    weekly_sales_text,
    daily_ingredients_used_text,
    weekly_profit_text,
    daily_expenditure_text};
  u16 result = 0;
  for (unsigned row = 0; row < 8; ++row)
    result = DrawText(29, (i16)(75u + 14u * row), labels[row]);

  return result;
}
void ShowDailyBalanceStuff2(Rest *rest) {
  Do3x3ButtonStuff(30);
  ShowDailyBalanceStats(rest);
  do {
    mouse_pressed = WaitForMouseClick();
  } while (!pt_is_menu_button_event(mouse_pressed));

  // Original tail jump.
  DisplayStuffAfterPayment();
}

// Weekly report.

enum WeeklyReportColumn { RECIPE_COLUMN, ITEM_COUNT_COLUMN, PROFIT_COLUMN, COST_COLUMN, SALES_COLUMN, STAFF_COLUMN };

static void staff_name(u8 index, const u8 *empty) {
  if (index) {
    const Staff *s = &g_staff[index];
    snprintf((char *)strbuf, sizeof(strbuf), "%s %s", (const char *)s->first_name, (const char *)s->last_name);
  } else
    snprintf((char *)strbuf, sizeof(strbuf), "%s", (const char *)empty);
}
// Products and totals wrap in DWORDs; /100 divides the signed interpretation
// of those wrapped bit patterns.

u16 ShowWeeksTakingStats(Rest *rest) {
  u8 page = 0;
  static const u32 columns[6] = {21, 111, 190, 160, 134, 62};

  Do3x3ButtonStuff(27);
  do {
    u8 limit = rest->num_recipes;

    DrawCustomersBackground();
    SelectPalette(7);
    SetCurrentFont(font_astandar);
    temp_canvas->color = 0;
    DrawText(48, 30, weekly_week_text);
    u32 total[4] = {0};
    temp_canvas->color = 10;
    if (page <= 1) {
      unsigned first = page ? 12 : 0;
      if (!page && limit > 12)
        limit = 12;

      DrawCaptionShadowed(100, 30, weekly_takings_text);
      temp_canvas->color = 12;
      DrawText2(12, 203, 40, weekly_sales_text);
      temp_canvas->color = 0;
      SetCurrentFont(font_supersma);
      (void)SetTempCanvasPosition(96, 50);
      DrawListElement(weekly_items_text);
      cur_list_elem_x = 120;
      DrawListElement(weekly_sales_text);
      cur_list_elem_x = 146;
      DrawListElement(weekly_costs_text);
      cur_list_elem_x = 173;
      DrawListElement(weekly_profit_text);
      cur_list_elem_y = 58;

      for (unsigned i = first; i < limit; ++i) {
        const RestoRecipe *r = &rest->recipes[i];
        u32 sold = (u16)r->sold_items;
        (void)SetTempCanvasPosition((i16)columns[RECIPE_COLUMN], cur_list_elem_y);
        (void)DrawTextInsideList(r->recipe->name);
        (void)SetTempCanvasPositionX((i16)columns[ITEM_COUNT_COLUMN]);
        DrawNumberInsideListRight(pt_signed_dword(sold));
        total[0] += sold;
        u32 sales = (u32)(u16)r->price_per_pizza * sold, costs = (u32)(u16)r->costs_per_pizza * sold,
            profit = sales - costs;
        (void)SetTempCanvasPositionX((i16)columns[SALES_COLUMN]);
        DrawDollarsInsideList(pt_signed_dword((u32)(pt_signed_dword(sales) / 100)));
        total[1] += sales;
        (void)SetTempCanvasPositionX((i16)columns[COST_COLUMN]);
        DrawDollarsInsideList(pt_signed_dword((u32)(pt_signed_dword(costs) / 100)));
        total[2] += costs;
        (void)SetTempCanvasPositionX((i16)columns[PROFIT_COLUMN]);
        DrawDollarsInsideList(pt_signed_dword((u32)(pt_signed_dword(profit) / 100)));
        total[3] += profit;
        cur_list_elem_y = (i16)((u16)cur_list_elem_y + 7u);
      }
      (void)SetTempCanvasPosition(25, 173);
      (void)DrawTextInsideList(weekly_total_text);
      (void)SetTempCanvasPositionX(105);
      DrawNumberInsideListRight(pt_signed_dword(total[0]));
      (void)SetTempCanvasPositionX(132);
      DrawDollarsInsideList(pt_signed_dword((u32)(pt_signed_dword(total[1]) / 100)));
      (void)SetTempCanvasPositionX((i16)columns[COST_COLUMN]);
      DrawDollarsInsideList(pt_signed_dword((u32)(pt_signed_dword(total[2]) / 100)));
      (void)SetTempCanvasPositionX(189);
      DrawDollarsInsideList(pt_signed_dword((u32)(pt_signed_dword(total[3]) / 100)));
    } else {
      DrawCaptionShadowed(100, 30, weekly_expenses_text);
      temp_canvas->color = 14;
      (void)SetTempCanvasPosition(140, 36);
      (void)DefaultLineTo(140, 178);
      (void)SetTempCanvasPosition(60, 36);
      (void)DefaultLineTo(60, 178);
      (void)SetTempCanvasPosition(20, 68);
      (void)DefaultLineTo(190, 68);
      temp_canvas->color = 10;
      SetCurrentFont(font_supersma);
      cur_list_elem_y = 43;
      (void)SetTempCanvasPosition((i16)columns[RECIPE_COLUMN], 43);
      DrawListElement(weekly_ingredients_text);
      cur_list_elem_y = 43;
      for (unsigned i = 0; i < 4; ++i) {
        (void)SetTempCanvasPosition((i16)columns[STAFF_COLUMN], cur_list_elem_y);
        (void)DrawTextInsideList(g_stock_name_kind[i]);
        (void)SetTempCanvasPositionX((i16)columns[COST_COLUMN]);
        DrawDollarsInsideList(pt_signed_dword((u16)rest->weekly_ingredient_type_cost[i]));
        total[0] += (u16)rest->weekly_ingredient_type_cost[i];

        cur_list_elem_y = (i16)((u16)cur_list_elem_y + 7u);
      }

      (void)SetTempCanvasPosition(192, 64);
      DrawDollarsInsideList(pt_signed_dword(total[0]));
      cur_list_elem_y = 75;
      DrawText(21, 78, weekly_cooks_text);
      total[1] = 0;

      for (unsigned group = 0; group < 2; ++group) {
        if (group) {
          (void)SetTempCanvasPosition((i16)columns[RECIPE_COLUMN], 121);
          DrawListElement(weekly_waiters_text);
        }
        for (unsigned i = 0; i < 6; ++i) {
          u8 index = rest->staff_indexes[6u * group + i];

          (void)SetTempCanvasPosition((i16)columns[STAFF_COLUMN], cur_list_elem_y);
          staff_name(index, weekly_default_staff_name);
          (void)DrawTextInsideList(strbuf);
          (void)SetTempCanvasPositionX((i16)columns[COST_COLUMN]);
          DrawDollarsInsideList(pt_signed_dword((u16)g_staff[index].salary));
          total[1] += (u16)g_staff[index].salary;

          cur_list_elem_y = (i16)((u16)cur_list_elem_y + 7u);
        }
      }
      (void)SetTempCanvasPosition((i16)columns[RECIPE_COLUMN], 163);
      DrawListElement(weekly_managers_text);
      cur_list_elem_x = 62;
      u8 manager = rest->staff_indexes[12];
      if (manager) {
        staff_name(manager, weekly_default_staff_name);
        DrawStringBufferAsListElement();
        (void)SetTempCanvasPositionX((i16)columns[COST_COLUMN]);
        DrawDollarsInsideList(pt_signed_dword((u16)g_staff[manager].salary));
        total[1] += (u16)g_staff[manager].salary;

      } else
        DrawListElement(weekly_default_staff_name);
      (void)SetTempCanvasPosition(192, 163);
      DrawDollarsInsideList(pt_signed_dword(total[1]));
      cur_list_elem_y = 169;
      (void)SetTempCanvasPosition(25, 176);
      (void)DrawTextInsideList(weekly_total_text);
      (void)SetTempCanvasPositionX(187);
      DrawDollarsInsideList(pt_signed_dword(total[0] + total[1]));
    }
    do {
      mouse_pressed = WaitForMouseClick();
    } while (!pt_is_menu_button_event(mouse_pressed));
    mouse_pressed = (u16)(mouse_pressed - 240u);
    if (!mouse_pressed && page < 2)
      ++page;
    if (mouse_pressed == 1 && page)
      --page;

  } while (mouse_pressed != 2);
  return 2;
}

// Monthly overview.

#include "files.h"

PTCanvasResult DrawMonthlyOverviewCaptions(u8 extended) {
  u8 expenses = extended ? 9 : 7;
  u8 receipts = extended ? 4 : 3;
  draw_list_elem_style = 0;
  FillSomeRectWithColor(0);
  SelectPalette(7);
  DrawNotebookBackground(1);
  SetCurrentFont(font_piznumb);
  temp_canvas->color = 11;
  (void)SetTempCanvasPosition(19, 40);
  DrawListElement(monthly_overview_caption);
  (void)SetTempCanvasPosition(104, 44);
  u8 previous = ptr_apple->month < 2 ? 11 : (u8)(ptr_apple->month - 2);
  DrawListElement(invoice_month_names[previous]);
  cur_list_elem_x = 158;
  DrawListElement(invoice_month_names[ptr_apple->month - 1u]);
  temp_canvas->color = 13;
  (void)SetTempCanvasPosition(19, 55);
  (void)DefaultLineTo(201, 55);
  (void)SetTempCanvasPosition(19, 100);
  (void)DefaultLineTo(201, 100);
  (void)SetTempCanvasPosition(19, 185);
  (void)DefaultLineTo(201, 185);
  temp_canvas->color = 0;
  cur_list_elem_x = 19;
  cur_list_elem_y = 54;
  for (u8 i = 0; i < receipts; ++i) {
    DrawListElement(monthly_receipt_labels[i]);
    cur_list_elem_y = (i16)((u16)cur_list_elem_y + 9u);
  }
  cur_list_elem_y = 99;
  for (u8 i = 0; i < expenses; ++i) {
    DrawListElement(monthly_expense_labels[i]);
    cur_list_elem_y = (i16)((u16)cur_list_elem_y + 9u);
  }
  (void)SetTempCanvasPosition(19, 184);
  PTCanvasResult result = DrawTextInsideList(monthly_result_caption);
  return result;
}
// Toolbar return AL is event minus 240. Detail tables use selected slots.

u8 ShowMonthlyOverview(u8 player) {
  game_ptr->in_menu_hierarchy = 0;
  for (;;) {
    u8 kind = 0;
    DrawMonthlyOverviewCaptions(1);
    cur_list_elem_y = 54;
    for (u8 i = 0; i < 4; ++i) {
      cur_list_elem_x = 115;
      DrawDollarsInsideList(pt_signed_dword(pt_monthly_receipts(player)->category[i]));
      cur_list_elem_x = 172;
      DrawDollarsInsideList(pt_signed_dword(pt_mission_receipts(player)->category[i]));
      cur_list_elem_y = (i16)((u16)cur_list_elem_y + 9u);
    }
    cur_list_elem_y = 99;
    for (u8 i = 0; i < 9; ++i) {
      cur_list_elem_x = 115;
      DrawDollarsInsideList(pt_signed_dword(pt_monthly_expenses(player)->category[i]));
      cur_list_elem_x = 172;
      DrawDollarsInsideList(pt_signed_dword(pt_mission_expenses(player)->category[i]));
      cur_list_elem_y = (i16)((u16)cur_list_elem_y + 9u);
    }
    (void)SetTempCanvasPosition(115, 184);
    DrawDollarsInsideList(
        pt_signed_dword(pt_monthly_receipts(player)->category[0] - pt_monthly_expenses(player)->category[0]));
    cur_list_elem_x = 172;
    DrawDollarsInsideList(
        pt_signed_dword(pt_mission_receipts(player)->category[0] - pt_mission_expenses(player)->category[0]));
    for (;;) {
      u16 event = WaitForMouseClick();
      mouse_pressed = event;
      if (pt_is_menu_button_event(event))
        return (u8)(event - 240u);
      if (event != 1)
        continue;
      u8 selected = 0;
      bool inside = IsMouseInside(25, 45, 203, 72);
      if (inside) {
        i32 index = GetSelectedListItem(45, 9);
        selected = (u8)index;
        kind = 1;
      }
      inside = IsMouseInside(25, 90, 203, 153);
      if (inside) {
        i32 index = GetSelectedListItem(90, 9);
        selected = (u8)index;
        kind = 2;
      }
      if (kind) {
        u8 count = player_ptr->num_restaurants;
        if (count > 11)
          count = 11;
        u16 height = 10u * count;
        FillRect(14, 45, 53, 192, height + 78u);
        FillRect(15, 50, 51, 196, height + 72u);
        Canvas_DrawRect(11, 49, 50, 197, height + 73u);
        (void)SetTempCanvasPosition(50, 64);
        (void)DefaultLineTo(196, 64);
        (void)DefaultLineTo(196, 65);
        (void)DefaultLineTo(50, 65);
        temp_canvas->color = 9;
        (void)DrawText(52, 60, monthly_restaurants_caption);
        (void)DrawText(130, 60, kind == 1 ? monthly_receipt_labels[selected] : monthly_expense_labels[selected]);
        u8 row = 0;
        temp_canvas->color = 10;
        for (const PTForwardLink *node = player_ptr->restaurants.begin.next; node->next; node = node->next) {
          ++row;
          if (row > 11)
            break;
          (void)SetTempCanvasPosition(50, (i16)(10u * row + 70u));
          const Rest *restaurant = (Rest *)node;
          const u8 *name = GetRestaurantName(restaurant);
          DrawTextInsideList(name);
          SetTempCanvasPositionX(186);
          DrawDollarsInsideList(pt_signed_dword((
              u32)(kind == 1 ? restaurant->curr_month_receipts[selected] : restaurant->curr_month_expenses[selected])));
        }
        WaitForMouseClick2();
      }
      break;
    }
  }
}

// Newspaper.

#include "character.h"
#include "memory.h"
#include "calendar.h"
#include <string.h>

// First strictly better space wins; no space returns text + 1.

u8 *LineBreakString(u8 *text) {
  u32 size = pt_strlen(text), best = 1000;
  u8 *split = text;
  for (u32 i = 0; i < size; ++i)
    if (text[i] == ' ') {
      u32 cost = size - i > i ? size - i : i;
      if (cost < best) {
        best = cost;
        split = text + i;
      }
    }
  if (split != text)
    *split = 0;
  return split + 1;
}
void InsertStringInTemplate(u8 *target, const u8 *pattern, const u8 *insert) {
  u32 a = (u32)strlen((const char *)pattern);
  u32 b = (u32)strlen((const char *)insert);
  for (u32 i = 0; i < a; ++i) {
    u8 c = pattern[i];
    if (c == '#')
      for (u32 j = 0; j < b; ++j)
        *target++ = insert[j];
    else
      *target++ = c;
  }
  *target = 0;
}
Game *LoadNewspaperBackground(u8 background) {
  u8 filename[60];
  Game *result = 0;
  if (background) {
    (void)snprintf((char *)filename, sizeof(filename), "z\\zb%u.vga", background);
    result = LoadBackground(filename, bmp21_ptr);
  }
  return result;
}
Game *LoadNewspaperArticlesFromDisk(u8 set) {
  u8 filename[60];
  newspaper_article_buffer = pt_buffer_allocate(15000, false);
  (void)snprintf((char *)filename, sizeof(filename), "z\\artikel%u.e", set);
  (void)DisableDiskReadErrors();
  (void)OpenFileAsCurrent(filename);
  (void)ReadBytesFromCurrentFile(newspaper_article_buffer, 15000);
  CloseCurrentFile();
  Game *result = EnableDiskReadErrors();
  return result;
}
void LoadNewspaperArticleFromFile(u8 set, u8 article) {
  (void)LoadNewspaperArticlesFromDisk(set);
  u8 *start = newspaper_article_buffer;
  for (u8 found = 0; found < article; ++start)
    if (*start == 167)
      ++found;
  u8 *body = start, *p;
  do {
    p = body++;
  } while (*p != '!');
  body[-2] = 0;
  do {
    p = body++;
  } while (*p != '$');
  p = body;
  for (;;) {
    u8 *previous = p++;
    if (*previous == 167)
      break;
    if (*p == 10)
      *p = ' ';
  }
  p -= 2;
  while (*p == ' ')
    --p;
  p[1] = 0;
  (void)pt_strcpy(newspaper_text_buffer, start + 1);
  (void)pt_strcpy(newspaper_text_buffer2, body + 1);
}
void LoadNewspaperArticle(u8 article) {
  if (newpaper_rand == 200) {
    const IngredientSet *set = &ingredients[newspaper_ingredient / 24u];
    const u8 *ingredient = set->vegs[newspaper_ingredient % 24u].name;
    u8 index = (u8)(4u * newspaper_random_mode + 2u - newspaper_random_direction);
    (void)LoadLangFile(&langfile_artikel3);
    const u8 *translated = GetTranslatedString(index);
    (void)snprintf((char *)newspaper_text_buffer,
        sizeof(newspaper_text_buffer),
        "%s %s",
        (const char *)ingredient,
        (const char *)translated);
    translated = GetTranslatedString((u8)(index + 1u));
    InsertStringInTemplate(newspaper_text_buffer2, translated, ingredient);
    FreeLangFile(&langfile_artikel3);
    newspaper_background = 14;
  } else {
    u8 index = (u8)(article - 1u);
    LoadNewspaperArticleFromFile(index / 50u, index % 50u + 1u);
    newspaper_background = newspaper_article_backgrounds[index];
  }
  if (newspaper_background)
    (void)LoadNewspaperBackground(newspaper_background);
}

void DrawNewspaperArticle(void) {
  u8 heading[52];
  u8 no_picture = 0;
  game_ptr->paragraph_background_enabled = 0;
  (void)pt_strcpy(heading, newspaper_text_buffer);
  (void)FillLetterBackground(4, 31, 35);
  Do3x3ButtonStuff(17);
  FillRect(159, 133, 33, 187, 38);
  SetCurrentFont(font_supersma);
  const u8 *month = GetMonthName((u8)(ptr_apple->month - 1u));
  (void)snprintf((char *)strbuf, sizeof(strbuf), "%u-%s-%u", ptr_apple->day, (const char *)month, (u16)ptr_apple->year);
  temp_canvas->color = 8;
  (void)DrawText2(133, 187, 38, strbuf);
  if (!newspaper_background)
    no_picture = 1;
  u16 bottom = newspaper_background ? 186 : 76;
  FillRect(159, 136, 45, 186, bottom);
  game_ptr->list_element_spacing_2 = 1;
  if (newspaper_background)
    BitBlt(bmp21_ptr, 0, 0, current_bitmap, 55, 77, 100, 48);
  temp_canvas->color = 0;
  SetCurrentFont(font_ruby);
  u8 *last = newspaper_text_buffer;
  i16 y = 68;
  if ((i16)(Canvas_TextWidth0(last)) >= 156) {
    u8 *second = LineBreakString(last);
    u16 a = Canvas_TextWidth0(last), b = Canvas_TextWidth0(second);
    if ((u32)b + (a >= b ? a - b : 0) >= 156) {
      SetCurrentFont(font_astandar);
      last = heading;
      y = 63;
      if ((i16)(Canvas_TextWidth0(heading)) >= 156) {
        last = LineBreakString(heading);
        (void)DrawText2(12, 203, 59, heading);
        y = 69;
      }
    } else {
      (void)DrawText2(12, 203, 61, last);
      last = second;
      y = 73;
    }
  }
  (void)DrawText2(12, 203, y, last);
  SetCurrentFont(font_supersma);
  if (newspaper_background)
    game_ptr->list_element_spacing_2 = 0;
  (void)Canvas_TextRect(
      temp_canvas, 0, 15, newspaper_text_buffer2, 32, 32, no_picture ? 135u : 182u, newspaper_background ? 137u : 90u);
  CopyBitmap2to1();
}
u8 HandleNewspaperMenu(void) {
  newspaper_article_buffer = NULL;
  InitGraphicsMode();
  newspaper_delay = 0;
  LoadNewspaperArticle(newpaper_rand);
  DrawNewspaperArticle();
  for (;;) {
    u16 event = WaitForMouseClick();
    mouse_pressed = event;
    if (pt_is_menu_button_event(event))
      break;
  }
  if (newspaper_article_buffer)
    pt_buffer_release(newspaper_article_buffer);
  CopyBitmap2to1();
  return 0;
}

// Profit graphs.

#include "restaurant.h"

// Integral powers are exact here. Larger exponents reproduce the x87
// integer-indefinite bit pattern from the original conversion.

static u32 divisor(u16 exponent, double base) {
  u32 value = 1;
  if (exponent < 10)
    for (unsigned i = 0; i < exponent; ++i)
      value *= (u32)base;
  else
    value = UINT32_C(0x80000000);
  return value;
}
static u64 caption_power(u16 exponent, double base) {
  u64 value = 1;
  if (exponent < 19)
    for (unsigned i = 0; i < exponent; ++i)
      value *= (u64)base;
  else
    value = UINT64_C(0x8000000000000000);
  return value;
}

static i32 divide(i32 value, u32 denominator) {
  return (i32)((i64)value / pt_signed_dword(denominator));
}

void DrawGlobalProfitsBg(void) {
  DrawCustomersBackground();
  DrawBob(&bob_restaurant_stats, 0, 30, 135);
  DrawBob(&bob_restaurant_stats, 1, 18, 30);
}
u8 DrawGlobalDailyProfitsEntry(const Player *player, u8 index, i32 value, u8 scale) {
  (void)player;

  u32 unit = divisor(scale, profit_graph_scale_base);
  i32 quotient = divide(value, unit);
  u8 result = 0, column = 0, band;
  for (; column < 2; ++column) {
    u16 x = (u16)(3u * index + 36u + column);
    (void)SetTempCanvasPosition((i16)x, 143);
    if (value >= 0) {
      for (band = 1; band < 7; ++band) {
        temp_canvas->color = (u8)(profit_graph_colors[band]);
        i32 threshold = pt_signed_dword(profit_graph_thresholds[band]);
        if (value < threshold)
          break;
        DefaultLineTo(x, (u16)(143u - (u32)divide(threshold, unit)));
      }
    } else {
      temp_canvas->color = (u8)(profit_graph_colors[0]);
      if ((i16)((u16)quotient) < -20)
        quotient = -20;
    }
    result = DefaultLineTo(x, (u16)(143u - (u32)quotient));
  }
  return result;
}
static void graph_header(const u8 *title) {
  LoadBobFile(&bob_restaurant_stats);
  DrawGlobalProfitsBg();
  SelectPalette(43);
  SetCurrentFont(font_ruby);
  temp_canvas->color = 13;
  DrawText2(12, 203, 30, title);
}
static void units(u32 value, i16 y) {
  (void)snprintf((char *)&strbuf, sizeof(strbuf), "in $%d units", pt_signed_dword(value));
  DrawText2(12, 203, y, strbuf);
}
void ShowGlobalCashFlow(u8 player) {
  // Original defect repair: zero history needs an initialized cash unit.

  u32 unit = 1000;
  u8 scale = 3;
  graph_header(cash_flow_caption);
  u8 count = ptr_apple->num_valid_daily_profits_entries;
  for (u8 i = 0; i < count; ++i) {
    for (;;) {
      unit = divisor(scale, cash_graph_scale_base);
      i32 quotient = divide(pt_player_cash_history_entry(player, i)->value, unit);
      if (quotient <= 100 && quotient >= -40)
        break;
      ++scale;
    }
  }
  for (u8 i = 0; i < count; ++i)
    DrawGlobalDailyProfitsEntry(pt_player(player), i, pt_player_cash_history_entry(player, i)->value, scale);
  SetCurrentFont(font_piznumb);
  temp_canvas->color = 0;
  units(10u * unit, 40);
  SetCurrentFont(some_font_ptr);
  temp_canvas->color = 11;
  (void)SetTempCanvasPosition(175, 152);
  DrawListElement(profit_graph_weeks);
}
void ShowGlobalDailyProfits(void) {
  u8 scale = 2, start = (u8)(30u - ptr_apple->num_days_with_proper_stats);
  graph_header(daily_profit_caption);
  for (u8 i = start; i < 30; ++i) {
    for (;;) {
      u32 unit = divisor(scale, daily_profit_scale_base);
      i32 quotient = divide(player_ptr->profit_last_30_days[i], unit);
      if (quotient <= 100 && quotient >= -40)
        break;
      ++scale;
    }
  }
  SetCurrentFont(font_piznumb);
  temp_canvas->color = 0;
  u64 caption = caption_power((u16)(scale + 1u), daily_profit_scale_base);
  units((u32)caption, 42);
  for (u8 i = start; i < 30; ++i)
    DrawGlobalDailyProfitsEntry(player_ptr, (u8)(i - start), player_ptr->profit_last_30_days[i], scale);
  SetCurrentFont(some_font_ptr);
  temp_canvas->color = 11;
  (void)SetTempCanvasPosition(179, 152);
  DrawListElement(profit_graph_days);
}

// Report background.

#include "menu.h"
#include "game.h"

void FillSomeRectWithColor(u8 color) {
  temp_canvas->color = color;

  Canvas_FillRect(temp_canvas, 12, 14, 203, 189);
}
u32 DrawNotebookBackground(u8 grid) {
  DrawBackground_Helper(12, 14, 203, 189, grid);

  DrawBitmapStuff1(26, 12, 14);

  u32 result = DrawBitmapStuff1(27, 173, 14);

  for (u8 i = 0; i < 3; ++i) {
    result = DrawBitmapStuff1(28, (i16)(42 * i + 45), 14);
  }

  return result;
}
// Keep every descriptor, offset and control needed by later drawing phases.
// Packed pages may not overwrite named state or original code.

// Modes other than 0 and 5 leave state unchanged; callers ignore their result.

PTCanvasResult DrawLegacyBackground(u8 mode) {
  if (mode != 0 && mode != 5)
    return pt_canvas_result_value(0);

  FillLetterBackground(0, 15, 7);

  if (!mode) {
    Canvas_SetLinePattern(temp_canvas, 0xaaaa);

    Canvas_DrawRect(0, 14, 16, 201, 187);

    const u16 tiles[2][3] = {{14, 22, 21}, {15, 182, 21}};
    for (unsigned i = 0; i < 2; ++i) {
      CopyBitmapPredefinedRect(tiles[i][0], tiles[i][1], tiles[i][2]);
    }

    u32 result = Canvas_SetLinePattern(temp_canvas, 0xffff);

    return pt_canvas_result_value(result);
  }

  temp_canvas->color = 20;

  Canvas *canvas = temp_canvas;
  for ((canvas->position_x = 12); canvas->position_x < 203;
      (canvas->position_x = (i16)((u16)canvas->position_x + 8u))) {
    canvas->position_y = 14;

    RelativeLineTo(0, 176);
  }
  for ((canvas->position_y = 14); canvas->position_y < 189;
      (canvas->position_y = (i16)((u16)canvas->position_y + 8u))) {
    canvas->position_x = 12;

    RelativeLineTo(192, 0);
  }
  return pt_canvas_result_object(temp_canvas);
}

Canvas *DrawLegacyGridBackground(void) {
  DrawLegacyBackground(5);
  Canvas *result = temp_canvas;

  return result;
}

u32 DrawBitmapStuff1(u8 index, i16 x, i16 y) {
  const SpriteRect *r = &blitmap_offsets[index];
  u32 result = BitBlt(&bmp_blitmap,
      r->left,
      r->top,
      current_bitmap,
      (u16)x,
      (u16)y,
      (u16)(r->right - r->left + 1u),
      (u16)(r->bottom - r->top + 1u));

  return result;
}
u8 DrawMarketCompetitorsBackground(i16 l, i16 t, i16 r, i16 b) {
  u8 result = 0;
  for (i16 x = l; x < r; x = (i16)((u16)x + 6u)) {
    SetTempCanvasPosition(x, t);

    result = DefaultLineTo((u16)x, (u16)b);
  }
  for (i16 y = t; y < b; y = (i16)((u16)y + 7u)) {
    SetTempCanvasPosition(l, y);

    result = DefaultLineTo((u16)r, (u16)y);
  }

  return result;
}
void DrawBackground_Helper(i16 l, i16 t, i16 r, i16 b, u8 grid) {
  LoadBlitmapAndOffsets(2);

  FillRect(29, (u16)l, (u16)t, (u16)r, (u16)b);

  if (grid == 1) {
    temp_canvas->color = 27;

    DrawMarketCompetitorsBackground(l, t, r, b);
  }

  DrawBitmapStuff1(12, l, t);

  i16 x = (i16)((u16)r - 32u), y = (i16)((u16)b - 12u);

  DrawBitmapStuff1(13, x, t);

  u8 tile = RandomChance(30) ? 14 : 20;

  DrawBitmapStuff1(tile, l, y);

  tile = RandomChance(30) ? 15 : 21;

  DrawBitmapStuff1(tile, x, y);

  const u16 tiles[4][3] = {{16, (u16)l, (u16)((u16)t + 13u)},
    {17, (u16)((u16)r - 10u), (u16)((u16)t + 13u)},
    {18, (u16)l, (u16)((u16)b - 36u)},
    {19, (u16)((u16)r - 10u), (u16)((u16)b - 36u)}};
  for (unsigned i = 0; i < 4; ++i) {
    DrawBitmapStuff1((u8)tiles[i][0], (i16)tiles[i][1], (i16)tiles[i][2]);
  }
}
u8 DrawAnimatedExpensesBgInner(void) {
  u8 result = Canvas_DrawRect(11, 15, 17, 200, 187);

  return result;
}
void DrawMarketCompetitorsCorners(void) {
  const u16 xy[4][2] = {{18, 20}, {178, 20}, {177, 164}, {18, 163}};
  for (unsigned i = 0; i < 4; ++i) {
    DrawBob(&bob_stanbobs, (u16)(35u + i), (i16)xy[i][0], (i16)xy[i][1]);
  }
}
void DrawCustomersBackground(void) {
  LoadBlitmapAndOffsets(2);

  DrawBackground_Helper(12, 14, 203, 189, 0);

  DrawAnimatedExpensesBgInner();

  // Stock tail jump reuses this function's incoming return address.

  DrawMarketCompetitorsCorners();
}

// Global stats.

static const u8 *bank(u8 index, bool insurance) {
  const Town *town = current_town;
  if (index >= 10)
    pt_resource_bounds_failure("bank name", index, 10);
  return town->phonebook_names[insurance ? 2 : 1][index];
}
static void property_name(const Poi *poi) {
  Zone *zone;
  zone = FindZoneFromCoordinates(poi->xcoord, poi->ycoord);

  DrawListElement(zone->name);
  cur_list_elem_x = 88;
  const u8 *name = finance_stats_labels.warehouse;
  if (poi->property_type != kPropertyDepot) {
    name = GetRestaurantName(poi->restaurant);
  }
  DrawListElement(name);
  cur_list_elem_y = (i16)((u16)cur_list_elem_y + 9u);
}
// Loans compact their visible rows; investments retain their physical slot rows.

void ShowLoansInsurancePropertyStats(u8 player, u8 category) {
  u8 visible_row, slot;
  visible_row = 0;
  SelectPalette(7);
  DrawNotebookBackground(0);
  SetCurrentFont(font_ruby);
  temp_canvas->color = 0;
  (void)DrawText(39, 37, finance_stats_labels.finance_info);
  SetCurrentFont(font_astandar);
  (void)DrawText(18, 48, pt_finance_category_name(category));
  if (!category) {
    (void)DrawText(18, 112, finance_stats_labels.investments);
    SetCurrentFont(font_piznumb);
    (void)SetTempCanvasPosition(98, 48);
    DrawListElement(finance_stats_labels.amount);
    cur_list_elem_x = 133;
    DrawListElement(finance_stats_labels.time);
    cur_list_elem_x = 154;
    DrawListElement(finance_stats_labels.percent);
    cur_list_elem_x = 167;
    DrawListElement(finance_stats_labels.per_month);
    for (slot = 0; slot < 4; ++slot) {
      const Loan *loan = pt_player_loan(player, slot);
      if (!loan->months_left)
        continue;
      cur_list_elem_x = 14;
      cur_list_elem_y = (i16)(10u * visible_row + 60);
      if (loan->town_index == ptr_apple->town_index)
        DrawClippedListElement(bank(loan->bank_index, false), 16);
      else {
        DrawListElement(pt_town_name(loan->town_index));
      }
      (void)SetTempCanvasPositionX(133);
      DrawNumberInsideListRight(pt_signed_dword((u32)loan->loan_size));
      (void)SetTempCanvasPositionX(145);
      DrawNumberInsideListRight(pt_signed_dword(loan->months_left));
      (void)SetTempCanvasPositionX(160);
      DrawPercentageNumberInList((u8)loan->interest_rate);
      (void)SetTempCanvasPositionX(204);
      DrawNumberInsideListRight(pt_signed_dword((u32)loan->total_pay_per_month));
      ++visible_row;
    }
    for (slot = 0; slot < 4; ++slot) {
      u8 i = slot;
      const Investment *investment = pt_player_investment(player, i);
      if (!investment->months_left)
        continue;
      (void)SetTempCanvasPosition(14, (i16)(8u * i + 126));
      if (investment->town_index == ptr_apple->town_index)
        DrawClippedListElement(bank(investment->bank_index, false), 16);
      else {
        const u8 *p = pt_town_name(investment->town_index);
        DrawTextInsideList(p);
      }
      (void)SetTempCanvasPositionX(128);
      DrawNumberInsideListRight(pt_signed_dword((u32)investment->investment_size));
      cur_list_elem_x = 162;
      DrawPercentageNumberInList((u8)investment->interest_rate);
    }
  } else if (category == 1) {
    SetCurrentFont(font_piznumb);
    (void)DrawText(30, 60, finance_stats_labels.category_company);
    (void)SetTempCanvasPosition(160, 60);
    DrawListElement(finance_stats_labels.charge);
    cur_list_elem_y = 69;
    DrawListElement(finance_stats_labels.cover);
    for (slot = 0; slot < 5; ++slot) {
      u8 i = slot;
      const Insurance *insurance = &ramtown.insurance[player][i];
      (void)SetTempCanvasPosition(16, (i16)(20u * i + 80));
      DrawListElement(pt_insurance_category_name(i));
      cur_list_elem_x = 80;
      if (insurance->type) {
        DrawClippedListElement(bank(insurance->insurance_company, true), 15);
        cur_list_elem_x = 188;
        DrawDollarsInsideList(pt_signed_dword((u16)insurance->monthly_sum));
        cur_list_elem_x = 188;
        cur_list_elem_y = (i16)((u16)cur_list_elem_y + 9u);
        DrawDollarsInsideList(pt_signed_dword((u32)insurance->insured_amount));
      } else
        DrawListElement(pizza_menu_info_separator);
    }
  } else {
    SetCurrentFont(font_piznumb);
    cur_list_elem_x = 16;
    cur_list_elem_y = 60;
    DrawListElement(finance_stats_labels.owned);
    cur_list_elem_y = (i16)((u16)cur_list_elem_y + 14u);
    for (Poi *node = pt_town_first_property(); node && cur_list_elem_y <= 186; node = pt_property_next(node)) {
      const Poi *poi = node;
      if (poi->current_tenant == player && poi->current_owner == player) {
        cur_list_elem_x = 13;
        property_name(poi);
      }
    }
    cur_list_elem_x = 16;
    cur_list_elem_y = (i16)((u16)cur_list_elem_y + 5u);
    if (cur_list_elem_y < 180)
      DrawListElement(finance_stats_labels.rented);
    cur_list_elem_y = (i16)((u16)cur_list_elem_y + 14u);
    for (Poi *node = pt_town_first_property(); node; node = pt_property_next(node)) {
      cur_list_elem_x = 13;
      if (cur_list_elem_y > 186)
        break;
      const Poi *poi = node;
      if (poi->current_tenant == player && poi->current_owner != player) {
        cur_list_elem_x = 13;
        property_name(poi);
      }
    }
  }
}
// Negation wraps to 32 bits, including the minimum signed value.

void ShowGlobalBalance(void) {
  const GlobalBalance *b = &global_balance[current_player_index];
  SelectPalette(7);
  DrawNotebookBackground(1);
  SetCurrentFont(font_ruby);
  DrawCaptionShadowed(80, 45, balance_labels.balance);
  SetCurrentFont(font_astandar);
  DrawCaptionShadowed(17, 63, balance_labels.assets);
  DrawCaptionShadowed(133, 63, balance_labels.liabilities);
  temp_canvas->color = 0;
  SetCurrentFont(font_piznumb);
  static const u8 *const names[] = {balance_labels.property,
    balance_labels.machines,
    balance_labels.furniture,
    balance_labels.weapons,
    balance_labels.supplies,
    balance_labels.money};
  for (unsigned i = 0; i < 6; ++i) {
    DrawText(14, (i16)(82 + 11 * i), names[i]);
    (void)SetTempCanvasPositionX(100);
    DrawNumberInsideListRight(pt_signed_dword((u32)b->assets[i]));
  }
  if (b->profit < 0) {
    temp_canvas->color = 22;
    DrawText(14, 159, balance_labels.loss);
    (void)SetTempCanvasPositionX(100);
    DrawNumberInsideListRight(pt_signed_dword(0u - (u32)b->profit));
    temp_canvas->color = 0;
  }
  (void)SetTempCanvasPosition(100, 176);
  DrawNumberInsideListRight(pt_signed_dword((u32)b->total_assets));
  DrawText(107, 82, balance_labels.capital);
  (void)SetTempCanvasPositionX(200);
  DrawNumberInsideListRight(pt_signed_dword((u32)b->capital));
  DrawText(107, 93, balance_labels.loans);
  (void)SetTempCanvasPositionX(200);
  DrawNumberInsideListRight(pt_signed_dword((u32)b->liabilities[0]));
  DrawText(107, 115, balance_labels.delivery_debts);
  (void)SetTempCanvasPositionX(200);
  DrawNumberInsideListRight(pt_signed_dword((u32)b->liabilities[1]));
  DrawText(107, 104, balance_labels.debts);
  (void)SetTempCanvasPositionX(200);
  DrawNumberInsideListRight(pt_signed_dword((u32)b->liabilities[2]));
  if (b->profit > 0) {
    DrawText(107, 126, weekly_profit_text);
    (void)SetTempCanvasPositionX(200);
    DrawNumberInsideListRight(pt_signed_dword((u32)b->profit));
  }
  (void)SetTempCanvasPosition(200, 176);
  DrawNumberInsideListRight(pt_signed_dword((u32)b->total_assets));
  (void)SetTempCanvasPosition(16, 70);
  (void)DefaultLineTo(200, 70);
  (void)SetTempCanvasPosition(104, 58);
  (void)DefaultLineTo(104, 183);
  (void)SetTempCanvasPosition(14, 166);
  (void)DefaultLineTo(48, 166);
  (void)SetTempCanvasPosition(62, 166);
  (void)DefaultLineTo(102, 166);
  (void)SetTempCanvasPosition(106, 166);
  (void)DefaultLineTo(137, 166);
  (void)DefaultLineTo(166, 136);
  (void)DefaultLineTo(202, 136);
  (void)SetTempCanvasPosition(15, 178);
  (void)RelativeLineTo(188, 0);
  (void)SetTempCanvasPosition(15, 180);
  (void)RelativeLineTo(188, 0);
  (void)SetTempCanvasPosition(106, 178);
  (void)RelativeLineTo(188, 0);
  (void)SetTempCanvasPosition(106, 180);
  (void)RelativeLineTo(188, 0);
  (void)SetTempCanvasPosition(167, 165);
  (void)DefaultLineTo(201, 165);
}
// Unsupported pages stop verification before changing state.

void HandleGlobalStatsMenu(u8 player) {
  u8 previous_page;
  u8 page = 0;
  Do3x3ButtonStuff(24);
  for (;;) {
    previous_page = page;
    switch (page) {
    case 0:
      page = ShowMonthlyOverview(player);
      break;
    case 1:
      (void)ShowRestaurantsOverview();
      page = 0;
      Do3x3ButtonStuff(24);
      continue;
    case 2:
      ShowGlobalCashFlow(current_player_index);
      break;
    case 3:
    case 4:
    case 5:
      ShowLoansInsurancePropertyStats(player, (u8)(page - 3));
      break;
    case 6:
      ShowGlobalBalance();
      break;
    case 7:
      ShowGlobalDailyProfits();
      break;
    case 8:

      return;
    default:
      break;
    }
    if (page == previous_page) {
      u16 event;
      do {
        event = WaitForMouseClick();
        mouse_pressed = event;
      } while (!pt_is_menu_button_event(event));
      page = (u8)(event - PT_MENU_BUTTON_1);
    }
  }
}
