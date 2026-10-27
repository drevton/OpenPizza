// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
// Info.

#include "objects.h"
#include "messages.h"
#include "calendar.h"
#include "finance.h"
#include "render_types.h"
#include <math.h>
#include <stdio.h>

static unsigned name_ready(const Player *player, size_t *length) {
  const u8 *name = player->name_text;
  const u8 *end = memchr(name, 0, sizeof(player->name_text));
  if (!end)
    return 1;
  *length = (size_t)(end - name);
  return 0;
}
static unsigned restaurant_text(const Rest *restaurant, u8 text[37], size_t *length) {
  if (restaurant->player_index >= 8)
    return 1;
  const Player *owner = &players[restaurant->player_index];
  size_t name_length;
  unsigned status = name_ready(owner, &name_length);
  if (status)
    return status;
  if (name_length >= sizeof(restaurant_name_text))
    return 1;
  int count = snprintf((char *)text,
      sizeof(restaurant_name_text),
      (const char *)restaurant_name_format,
      (const char *)owner->name_text,
      (u8)restaurant->restaurant_id + 1);
  if (count < 0 || (size_t)count >= sizeof(restaurant_name_text))
    return 1;
  *length = (size_t)count;
  return 0;
}
const u8 *GetRestaurantName(const Rest *restaurant) {
  u8 text[37];
  size_t length;
  if (restaurant_text(restaurant, text, &length))
    abort();
  memcpy(restaurant_name_text, text, length + 1);
  return restaurant_name_text;
}
unsigned pt_restaurant_name_text(const Rest *restaurant, u8 text[37], size_t *length) {
  return restaurant_text(restaurant, text, length);
}

// Formatted lines use a 36-byte buffer; direct labels retain their source.

static unsigned line(InfoPlan *plan, const u8 *text, size_t maximum, const u8 *source) {
  if (plan->count == 12)
    return 1;
  const u8 *end = memchr(text, 0, maximum);
  if (!end)
    return 1;
  size_t length = (size_t)(end - text);
  if (length + 1 > (source ? 37u : 36u))
    return 1;
  InfoLine *l = &plan->lines[plan->count++];
  memcpy(l->text, text, length + 1);
  l->length = (u16)length;
  l->source = source;
  return 0;
}
static unsigned number_line(InfoPlan *plan, const u8 *format, i32 number) {
  u8 text[36];
  int count = snprintf((char *)text, sizeof(text), (const char *)format, number);
  if (count < 0 || (size_t)count >= sizeof(text))
    return 1;
  return line(plan, text, sizeof(text), NULL);
}
static i32 stay_minutes(float value) {
  // __CHP truncates ST0, then FISTP stores a signed qword. sprintf reads

  double integral = TruncateX87Integral((double)value);
  u64 bits = !isfinite(integral) || integral < -0x1p63 || integral >= 0x1p63 ? UINT64_C(0x8000000000000000)
                                                                             : (u64)(i64)integral;
  u32 low = (u32)bits;
  return low <= INT32_MAX ? (i32)low : -1 - (i32)(UINT32_MAX - low);
}
unsigned pt_info_plan(InfoPlan *plan) {
  memset(plan, 0, sizeof(*plan));
  const Apple *apple = ptr_apple;
  plan->restaurant = apple->view_state >= 128;
  unsigned status;
  if (!plan->restaurant) {
    if (apple->town_index >= 10)
      return 1;
    status = line(plan, town_names[apple->town_index], 12, town_names[apple->town_index]);
    if (status)
      return status;
    u8 season = GetArstidIndex();
    if (season >= 4)
      return 1;
    const u8 *name = GetNameOfArstid();
    if (!memchr(name, 0, season == 3 ? 8 : 9))
      return 1;
    u8 text[36];
    int count = snprintf((char *)text, sizeof(text), (const char *)info_box_formats.season, (const char *)name);
    if (count < 0 || (size_t)count >= sizeof(text))
      return 1;
    status = line(plan, text, sizeof(text), 0);
    if (status)
      return status;
    if (!player_ptr)
      return 2;
    const Player *player = player_ptr;
    size_t length;
    status = name_ready(player, &length);
    if (status)
      return status;
    status = line(plan, player->name_text, length + 1, 0);
    if (status)
      return status;
    if ((status = number_line(plan, info_box_formats.restaurants, player->num_restaurants)) ||
        (status = number_line(plan, info_box_formats.recipes, player->num_recipes)) ||
        (status = number_line(plan, info_box_formats.contracts, player->num_contracts)))
      return status;
    status = line(plan, info_box_formats.district, sizeof(info_box_formats.district), info_box_formats.district);
    if (status)
      return status;
    if (!mouse_over_zone)
      return line(
          plan, info_box_formats.no_district, sizeof(info_box_formats.no_district), info_box_formats.no_district);
    return line(plan, mouse_over_zone->name, sizeof(mouse_over_zone->name), mouse_over_zone->name);
  }
  if (!apple->restaurant)
    return 2;
  if (apple->is_in_kitchen >= 10)
    return 1;
  const Rest *rest = apple->restaurant;
  status = line(plan, info_location_names[apple->is_in_kitchen], 20, info_location_names[apple->is_in_kitchen]);
  if (status)
    return status;
  u8 text[37];
  size_t length;
  status = restaurant_text(rest, text, &length);
  if (status)
    return status;
  status = line(plan, text, sizeof(text), restaurant_name_text);
  if (status)
    return status;
  // Original defect: more than seventeen stars overrun the text buffer.
  // The repaired release caps the display at seventeen.

  unsigned stars = rest->num_stars;
  if (stars > 17)
    stars = 17;
  memset(text, ' ', 17);
  memset(text, '-', 4);
  memset(text + 13, '-', 4);
  text[17] = 0;
  for (unsigned i = 0; i < stars; ++i)
    text[i + 8 - stars / 2u] = '*';
  status = line(plan, text, 18, 0);
  if (status)
    return status;
  if ((status = number_line(plan, info_box_formats.popularity, rest->sum_of_reputations)) ||
      (status = number_line(plan, info_box_formats.cooks, rest->num_chefs)) ||
      (status = number_line(plan, info_box_formats.waiters, rest->num_waiters)))
    return status;
  const u8 *manager = rest->staff_indexes[12] ? info_box_formats.yes : info_box_formats.no;
  int count = snprintf((char *)text, sizeof(text), (const char *)info_box_formats.manager, (const char *)manager);
  if (count < 0 || (size_t)count >= sizeof(text))
    return 1;
  status = line(plan, text, sizeof(text), 0);
  if (status)
    return status;
  if ((status = number_line(plan, info_box_formats.pizzas, rest->num_recipes)) ||
      (status = number_line(plan, info_box_formats.pizza_quality, rest->pizza_quality)) ||
      (status = number_line(plan, info_box_formats.waiting_time, rest->waiting_time)) ||
      (status = number_line(plan, info_box_formats.stay_time, stay_minutes(rest->average_stay_time))))
    return status;
  return game_ptr->debug_mode ? number_line(plan, info_box_formats.tester_count, rest->days_until_major_visit)
                              : number_line(plan, info_box_formats.local_share, rest->local_share);
}
void PrintGenericInfoBox(void) {
  const Rest *restaurant_pointer = ptr_apple->restaurant;
  if (!game_ptr->byte87 || ptr_apple->message_ticks_left)
    return;
  InfoPlan plan;
  if (pt_info_plan(&plan))
    abort();
  const Rest *restaurant = plan.restaurant ? restaurant_pointer : NULL;
  u8 buffer[36];
  FillGenericInfoBox();
  for (unsigned i = 0; i < plan.count; ++i) {
    const InfoLine *l = &plan.lines[i];
    const u8 *text = l->source;
    if (plan.restaurant && i == 1)
      text = GetRestaurantName(restaurant);
    if (!text) {
      memcpy(buffer, l->text, l->length + 1u);
      text = buffer;
    }
    DrawInfoBoxText(text);
  }
  game_ptr->info_box_state = 0;
  PrintCurrentDateAndTime();
  DisplayStuffAfterPayment();
}

// Message.

#include "text.h"
#include "graphics.h"

u8 *pt_strcpy(u8 *target, const u8 *source) {
  // Byte-wise forward copying also defines overlap toward lower addresses.

  u8 *out = target;
  u8 value;
  do {
    value = *source++;
    *out++ = value;
  } while (value);

  return target;
}
void BackupCanvasAttr(void) {
  game_ptr->canvas_color_backup = temp_canvas->color;
  game_ptr->canvas_byteE_backup = temp_canvas->background_color;
  game_ptr->canvas_byte10_backup = temp_canvas->text_background_enabled;
  game_ptr->font_backup = temp_canvas->font;
  game_ptr->canvas_position_x_backup = temp_canvas->position_x;
  game_ptr->canvas_position_y_backup = temp_canvas->position_y;
  game_ptr->paragraph_first_indent_height_backup = game_ptr->paragraph_first_indent_height;
  game_ptr->paragraph_indent_y_threshold_backup = game_ptr->paragraph_indent_y_threshold;
  game_ptr->list_element_spacing_backup = game_ptr->list_element_spacing_2;
  game_ptr->paragraph_background_enabled_backup = game_ptr->paragraph_background_enabled;
  canvas_menu_backup = game_ptr->in_menu_hierarchy;
}
void RestoreCanvasAttrs(void) {
  temp_canvas->color = (u8)game_ptr->canvas_color_backup;
  temp_canvas->background_color = game_ptr->canvas_byteE_backup;
  temp_canvas->text_background_enabled = game_ptr->canvas_byte10_backup;
  SetCurrentFont(game_ptr->font_backup);
  SetTempCanvasPosition(game_ptr->canvas_position_x_backup, game_ptr->canvas_position_y_backup);
  game_ptr->paragraph_first_indent_height = game_ptr->paragraph_first_indent_height_backup;
  game_ptr->paragraph_indent_y_threshold = game_ptr->paragraph_indent_y_threshold_backup;
  game_ptr->list_element_spacing_2 = game_ptr->list_element_spacing_backup;
  game_ptr->paragraph_background_enabled = game_ptr->paragraph_background_enabled_backup;
  game_ptr->in_menu_hierarchy = canvas_menu_backup;
}
void FillGenericInfoBox(void) {
  canvas_1->color = 0;
  Canvas_FillRect(canvas_1, 228, 20, 310, 93);
  if (game_ptr->did_set_bitmap) {
    canvas_1->bitmap = old_bitmap;
    Canvas_FillRect(canvas_1, 228, 20, 310, 93);
    canvas_1->bitmap = current_bitmap;
  }
  canvas_1->color = 14;
  canvas_1->background_color = 0;
  canvas_1->text_background_enabled = 0;
  Canvas_SetFont(canvas_1, font_supersma);
  canvas_1->position_x = 230;
  canvas_1->position_y = 26;
}
Canvas *DrawInfoBoxText(const u8 *text) {
  Canvas_DrawText(canvas_1, pt_text_span(text, (u16)pt_strlen(text)));
  if (game_ptr->did_set_bitmap) {
    canvas_1->bitmap = display_bitmaps[game_ptr->current_bitmap_index ^ 1u];
    Canvas *canvas = canvas_1;
    u16 width = Canvas_TextWidth(canvas_1, pt_text_span(text, (u16)pt_strlen(text)));
    canvas->position_x = (i16)((u16)canvas->position_x - width);
    Canvas_DrawText(canvas_1, pt_text_span(text, (u16)pt_strlen(text)));
    canvas_1->bitmap = display_bitmaps[game_ptr->current_bitmap_index];
  }
  Canvas *canvas = canvas_1;
  u16 width = Canvas_TextWidth(canvas_1, pt_text_span(text, (u16)pt_strlen(text)));
  canvas->position_x = (i16)((u16)canvas->position_x - width);
  canvas_1->position_y = (i16)((u16)canvas_1->position_y + 6u);
  return canvas_1;
}
void ShowMessage(u8 color, u8 *text) {
  if (!game_ptr->byte87)
    return;
  BackupCanvasAttr();
  if (text)
    pt_strcpy(retained_message_text, text);
  else
    text = retained_message_text;
  FillGenericInfoBox();
  Canvas_SetFont(canvas_1, font_supersma);
  game_ptr->paragraph_background_enabled = 0;
  game_ptr->list_element_spacing_2 = 0;
  game_ptr->list_element_spacing = 6;
  game_ptr->info_box_state = 2;
  ptr_apple->message_ticks_left = 6;
  Canvas_TextRect(canvas_1, color, 0, text, 228, 228, 305, 28);
  if (game_ptr->did_set_bitmap)
    BitBlt(current_bitmap, 228, 20, old_bitmap, 228, 20, 82, 68);
  RestoreCanvasAttrs();
}
