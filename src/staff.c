// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
// Staff controller.

#include "objects.h"
#include "text.h"
#include "input.h"
#include "staff.h"
#include "restaurant.h"
#include "recipe.h"
#include "world.h"
#include "messages.h"
#include "files.h"
#include "menu.h"
#include "player.h"
#include "calendar.h"
#include "graphics.h"
#include "frame.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static u8 selected(i16 top, i16 spacing) {
  u32 result = GetSelectedListItem(top, spacing);
  return (u8)result;
}
u16 HandleStaffMenu_Helper2(Rest *restaurant) {
  u8 labels[2][5] = {"NO", "YES"};
  u16 event;
again:
  event = WaitForMouseClick();
  mouse_pressed = event;
  if (staff_display_flag)
    goto done;
  if (event != 1) {
    if (event == PT_MENU_BUTTON_3)
      goto again;
    goto done;
  }
  if ((IsMouseInside(154, 64, 190, 92) != 0)) {
    u8 row = selected(64, 10);

    u8 *setting = pt_rest_manager_toggle(restaurant, row);
    (*setting) ^= 1;

    i16 y = (i16)(65u + 10u * row);
    DrawDepotPriorityEntry(154, y, 37, labels[(*setting)], 1);
    Delay(2);
    DrawDepotPriorityEntry(154, y, 37, labels[(*setting)], 0);
    event = 0;
  } else if ((IsMouseInside(128, 105, 196, 143) != 0)) {
    u8 row = selected(105, 10);

    u8 *setting = pt_rest_manager_reaction(restaurant, row);
    ++(*setting);
    if ((*setting) > 2)
      (*setting) = 0;
    i16 y = (i16)(106u + 10u * row);
    DrawDepotPriorityEntry(128, y, 68, staff_ui_manager_reactions[(*setting)], 1);
    WaitForMouseLeave();
    DrawDepotPriorityEntry(128, y, 68, staff_ui_manager_reactions[(*setting)], 0);
    event = 0;
  } else if ((IsMouseInside(128, 177, 196, 185) != 0)) {
    Rest *rest = restaurant;
    rest->mgr_staff_behavior = (u8)((u32)rest->mgr_staff_behavior + (u32)1u);
    if (rest->mgr_staff_behavior > 6)
      rest->mgr_staff_behavior = 0;
    DrawDepotPriorityEntry(128, 177, 68, staff_ui_manager_behaviors[rest->mgr_staff_behavior], 1);
    Delay(2);
    DrawDepotPriorityEntry(128, 177, 68, staff_ui_manager_behaviors[rest->mgr_staff_behavior], 0);
    event = 0;
  }
done:

  return event;
}

static u8 index_for(Rest *restaurant, u8 role, u8 row) {
  const Rest *rest = restaurant;
  return rest->staff_indexes[role == 2 ? 12 : 6u * role + row];
}
PTActionResult HandleStaffMenu(Rest *restaurant) {
  u16 row, previous_row;
  u8 role, count;
  role = 0;
  i16 index = -1;
  Staff *staff = NULL;
  PTActionResult result = pt_action_value(0);
restart:
  did_terminal_stuff = 0;
  row = 65535;
  result = pt_action_object(InitGraphicsMode());
  if (has_many_players)
    goto done;
  ReClearEmptyStaffSlots(restaurant);
  LoadBlitmapAndOffsets(3);
  (void)Do3x3ButtonStuff(12);
category:
  previous_row = row;
  index = -1;
  row = 65535;
  staff_display_flag = 0;
redraw: {
  u8 pending;
  pending = CheckGoalStuff();
  if (pending)
    goto restart;
}
  ReClearEmptyStaffSlots(restaurant);
  temp_canvas->color = 0;
  SetCurrentFont(font_astandar);
  SetSomeFieldInCanvas(0);
  if ((i16)row >= 0) {
    index = index_for(restaurant, role, (u8)row);
    staff = pt_staff_record((u8)index);
  }
  SetCurrentBitmap(old_bitmap);
  ReClearEmptyStaffSlots(restaurant);
  count = RestDrawStaffSelector(restaurant, role, 255);
  DrawLetterOnBitmap(current_bitmap);
  Copy320x200Bitmap(old_bitmap, current_bitmap);
  CommitBitmapAndSetPalette(13);
  if (previous_row == 65535) {
    (void)CopyBmp6AToHere(current_bitmap);
  } else {
    (void)HandleStaffMenu_Helper1();
  }
wait:
  Copy320x200Bitmap(current_bitmap, old_bitmap);
  if (role == 2) {
    mouse_pressed = HandleStaffMenu_Helper2(restaurant);
    if (!mouse_pressed)
      goto wait;
  } else {
    mouse_pressed = WaitForMouseClick();
  }
  if (mouse_pressed == 0x0f09) {
    restaurant = SelectNextRestaurant(restaurant);

    goto restart;
  }
  if (did_terminal_stuff) {
    restaurant = ptr_apple->restaurant;

    goto restart;
  }
  if (mouse_pressed == 2)
    goto wait;
dispatch:
  if (!pt_is_menu_button_event(mouse_pressed))
    goto input;
  switch (mouse_pressed) {
  case PT_MENU_BUTTON_1:
    role = 0;
    goto category;
  case PT_MENU_BUTTON_2:
    role = 1;
    goto category;
  case PT_MENU_BUTTON_3:
    role = 2;
    goto category;
  case PT_MENU_BUTTON_4:
    if ((i16)row < 0)
      DisplayErrorMessage(0, staff_fire_selection_error);
    else {
      const Staff *record = staff;
      if (record->absent_days) {
        (void)snprintf((char *)strbuf,
            sizeof(strbuf),
            "%s %s is away right now and cannot be fired!",
            record->first_name,
            record->last_name);
        DisplayErrorMessage(0, strbuf);
      } else {
        u8 yes = ShowMessage2(record->face, staff_fire_prompt);
        if (yes) {
          RestRemoveStaff(restaurant, role, (u8)row);
        }
      }
    }
    goto restart;
  case PT_MENU_BUTTON_5:
    if ((i16)row < 0) {
      DisplayErrorMessage(0, staff_wages_selection_error);
      goto changed;
    }
    {
      Staff *record = staff;
      u16 old_salary = (u16)record->salary;

      unit_for_number_query = staff_wage_unit;
      i32 new_value = ShowAskForNumberQuery(staff_wage_prompt, old_salary, 50, old_salary / 2u, 2u * old_salary);
      record->salary = (i16)((u16)new_value);
      u16 new_salary = (u16)record->salary;
      i32 percent = Mul100Div(new_salary, old_salary);
      u8 previous = record->motivation;
      i32 updated;
      if (new_salary > old_salary) {
        percent = pt_signed_dword(2u * ((u32)percent - 100u));

        percent = percent / ((i32)(ptr_apple->difficulty_level / 2u + 1u)) + 100;
        updated = MulDiv100(previous, percent);
      } else {
        updated = MulDiv100(previous, percent / 2);
      }
      i16 delta = (i16)((u16)((u32)updated - previous));
      StaffAddMotivation(record, (u32)(i32)delta);
      i32 capacity_percent = percent > 100 ? pt_signed_dword((u32)percent + 120u) / 2 : percent;
      i32 capacity = MulDiv100((u16)record->cooking_or_serving_capacity, capacity_percent);
      if (capacity > 2000)
        capacity = 2000;
      else {
        capacity = MulDiv100((u16)record->cooking_or_serving_capacity, capacity_percent);
      }
      record->cooking_or_serving_capacity = (i16)((u16)capacity);
    }
    goto changed;
  case PT_MENU_BUTTON_6:
    if (index < 0) {
      DisplayErrorMessage(0, staff_course_selection_error);
      goto changed;
    }
    {
      const Staff *record = staff;
      if (!record->absent_days) {
        mouse_pressed = SendStaffOnCourse(staff, role);
        if (mouse_pressed == 50)
          goto restart;
        goto dispatch;
      }

      if (record->absent_reason >= 6) {
        (void)snprintf((char *)strbuf, sizeof(strbuf), "%s %s is not here!", record->first_name, record->last_name);
      } else {
        const StaffTrainingCourse *course = &staff_training_courses[role][record->absent_reason];
        (void)snprintf((char *)strbuf,
            sizeof(strbuf),
            "%s %s already is on the '%s' course.",
            record->first_name,
            record->last_name,
            course->name);
      }
      DisplayErrorMessage(0, strbuf);
      goto changed;
    }
  case PT_MENU_BUTTON_7:
    HandleHireStaff(restaurant, role);
    (void)Do3x3ButtonStuff(12);
    goto restart;
  case PT_MENU_BUTTON_8:
    result = pt_action_value(7);
    goto done;
  default:
    goto recompute;
  }
changed:
  did_terminal_stuff = 0;
recompute: { (void)ReComputeStaffStuff(restaurant); }
  (void)ReComputeAverageTimes(restaurant);
input:
  if (mouse_pressed != 1)
    goto redraw;
  if (row == 65535) {
    if ((IsMouseInside(14, 40, 200, (i16)(12u * count + 38u)) != 0)) {
      u8 selected_row = selected(38, 12);
      row = selected_row;
      index = index_for(restaurant, role, selected_row);
      staff = pt_staff_record((u8)index);
      DrawStaffRow(staff, row, row);
      (void)InitGraphicsMode();
      SetCurrentBitmap(old_bitmap);
      ShowStaffFile(staff, 0);
      DrawLetterOnBitmap(current_bitmap);
      Copy320x200Bitmap(old_bitmap, current_bitmap);
      CommitBitmapAndSetPalette(0);
      AnimateStaffFile();
      staff_display_flag = 1;
    }
  } else {
    SetCurrentBitmap(old_bitmap);
    count = RestDrawStaffSelector(restaurant, role, (u8)row);
    DrawLetterOnBitmap(current_bitmap);
    Copy320x200Bitmap(old_bitmap, current_bitmap);
    CommitBitmapAndSetPalette(0);
    (void)HandleStaffMenu_Helper1();
    DrawStaffRow(staff, (u8)row, 255);
    row = 65535;
    staff_display_flag = 0;
  }
  goto wait;
done:

  return result;
}

// Staff render.

#include "character.h"
#include "reports.h"
#include "finance.h"
#include <math.h>

void DrawStaffRow(const Staff *address, u8 row, u8 selected) {
  draw_list_elem_style = 0;
  if (row == 100) {
    (void)DrawBitmapStuff1(5, 16, 29);
    for (unsigned i = 0; i < 6; ++i)
      (void)DrawBitmapStuff1(6, (i16)(20u * i + 48u), 29);
    (void)DrawBitmapStuff1(7, 169, 29);
    SetCurrentFont(some_font_ptr);
    temp_canvas->color = 0;
    DrawText(23, 36, staff_ui_name);
    DrawText(80, 36, staff_ui_days_absent);
    DrawText(149, 36, staff_ui_capacity);
    DrawText(176, 36, staff_ui_salary);

    return;
  }
  i16 y = (i16)(12u * row);
  (void)DrawStaffRowBg(y + 38, selected == row ? 3 : 0);
  if (address) {
    const Staff *staff = address;
    temp_canvas->color = 0;
    SetCurrentFont(font_piznumb);
    (void)snprintf((char *)strbuf, sizeof(strbuf), "%s %s", staff->first_name, staff->last_name);
    (void)SetTempCanvasPosition(19, y + 47);
    (void)DrawTextInsideList(strbuf);
    if (!staff->absent_days) {
      (void)SetTempCanvasPositionX(166);
      u16 capacity = (u16)staff->cooking_or_serving_capacity;
      if (game_ptr->debug_mode)
        DrawNumberInsideListRight(pt_signed_dword(staff->absent_reason));
      else {
        if (staff->absent_reason == 101 || staff->absent_reason == 201) {
          capacity = (u16)MulDiv100(capacity, 80);
        }
        DrawNumberInsideListRight(pt_signed_dword(capacity));
      }
    } else {
      (void)SetTempCanvasPositionX(118);
      DrawNumberInsideListRight(pt_signed_dword((u16)staff->absent_days));
      (void)SetTempCanvasPositionX(124);
      if (staff->absent_reason < 6)
        (void)DrawTextInsideList(staff_ui_course);
      else {
        if (staff->absent_reason % 100u == 2)
          (void)DrawTextInsideList(staff_ui_sick);
        if (staff->absent_reason % 100u == 3)
          (void)DrawTextInsideList(staff_ui_pregnant);
      }
    }
    (void)SetTempCanvasPositionX(188);
    (void)DrawDollarsInsideList((u16)staff->salary);
  }
}
void RestDrawManagerConfig_Helper(u8 tile, i32 left, i32 top, i32 right, i32 bottom) {
  const SpriteRect *r = &blitmap_offsets[tile];
  int width = (int)r->right - r->left + 1, height = (int)r->bottom - r->top + 1;

  i16 x = (i16)((u16)left), y = (i16)((u16)top), endx = (i16)((u16)right), endy = (i16)((u16)bottom);
  while ((int)x <= (int)endx - width) {
    (void)DrawBitmapStuff1(tile, x, y);
    x = (i16)((u16)x + (u16)width);
  }
  if (x <= endx) {
    (void)DrawPizzaPopularityChart_Helper1(tile, x, y, (i16)((u16)right + 1u - (u16)x));
  }
  if (height < (i16)((u16)(bottom + 1u - top))) {
    while ((int)y < (int)endy - height) {
      i16 amount = (i16)((u16)bottom + 1u - (u16)y);
      if (amount >= height)
        amount = (i16)height;
      i16 next = (i16)((u16)y + (u16)height);
      BitBlt(current_bitmap,
          (u16)((i16)((u16)left)),
          (u16)y,
          current_bitmap,
          (u16)((i16)((u16)left)),
          (u16)next,
          (u16)((i16)((u16)right + 1u - (u16)left)),
          (u16)amount);
      y = next;
    }
  }
}

void RestDrawManagerConfig(Rest *restaurant) {
  u8 labels[10] = {'N', 'O', 0, 0, 0, 'Y', 'E', 'S', 0, 0};

  const Rest *rest = restaurant;
  RestDrawManagerConfig_Helper(12, 17, 29, 197, 189);
  DrawStaffRow(NULL, 100, 0);
  SetCurrentFont(font_piznumb);
  if (rest->staff_indexes[12]) {
    DrawStaffRow(pt_staff_record(rest->staff_indexes[12]), 0, 100);
  } else {
    (void)DrawStaffRowBg(38, 0);
    DrawText(30, 47, staff_ui_no_manager);
  }
  (void)DrawStaffRowBg(51, 3);
  (void)DrawStaffRowBg(92, 3);
  (void)DrawStaffRowBg(163, 3);
  temp_canvas->color = 0;
  DrawText(22, 60, staff_ui_authority);
  DrawText(22, 101, staff_ui_reactions);
  DrawText(22, 172, staff_ui_behavior_towards_staff);
  temp_canvas->color = 0;
  SetCurrentFont(some_font_ptr);
  DrawText(20, 70, staff_ui_shopping_if_needed);
  DrawText(20, 80, staff_ui_change_prices);
  DrawText(20, 90, staff_ui_overdraw_account);
  DrawText(20, 111, staff_ui_turnover_decrease);
  DrawText(20, 121, staff_ui_turnover_increase);
  DrawText(20, 131, staff_ui_supply_shortage);
  DrawText(20, 141, staff_ui_staff_shortage);
  // Manager values select physical fixed-width labels. Reject values whose
  // label ownership has not been established instead of clamping them.

  for (unsigned i = 0; i < 3; ++i) {
    DrawDepotPriorityEntry(154, (i16)((i16)(65u + 10u * i)), 37, labels + 5u * rest->manager_authority[i], 0);
  }
  for (unsigned i = 0; i < 4; ++i) {
    DrawDepotPriorityEntry(
        128, (i16)((i16)(106u + 10u * i)), 68, staff_ui_manager_reactions[rest->manager_reactions[i]], 0);
  }

  DrawDepotPriorityEntry(128, 177, 68, staff_ui_manager_behaviors[rest->mgr_staff_behavior], 0);
}
u8 RestDrawStaffSelector(Rest *restaurant, u8 role, u8 selected) {
  u32 capacity, demand_ratio, time_per_pizza;
  Rest *rest = restaurant;
  u8 count = 0, quality = 0;
  u32 costs = 0;
  for (unsigned i = 0; i < 16; ++i)
    (void)DrawBitmapStuff1(0, (i16)(12u * i + 12u), 14);
  for (unsigned i = 0; i < 16; ++i)
    (void)DrawBitmapStuff1(1, 12, (i16)(10u * i + 29u));
  for (unsigned i = 0; i < 9; ++i)
    (void)DrawBitmapStuff1(1, (i16)(20u * i + 17u), 89);
  DrawStaffRow(NULL, 100, 0);
  if (role < 2) {
    SetCurrentFont(font_astandar);
    temp_canvas->color = 15;
    if (selected != 100)
      DrawText(25, 25, (role ? staff_ui_waiters : staff_ui_chefs));
    SetCurrentFont(font_piznumb);
    unsigned n = role ? rest->num_waiters : rest->num_chefs, base = 6u * role;

    for (unsigned i = 0; i < n; ++i) {
      const Staff *staff = pt_staff_record(rest->staff_indexes[base + i]);
      costs += (u16)staff->salary;
      ++count;
      DrawStaffRow(staff, (u8)i, selected);
    }
    for (unsigned i = n; i < 6; ++i) {
      rest->staff_indexes[base + i] = 0;
      DrawStaffRow(NULL, (u8)i, selected);
    }
    unsigned kind = role ? 4 : 2;
    capacity = (u16)rest->capacity_of_kind[kind];
    quality = role ? rest->average_waiter_rating : rest->average_cooking_rating;
    double time;
    time = TruncateX87Integral(role ? rest->average_waiter_time : rest->average_chef_time);
    u32 integer =
        isfinite(time) && time >= -2147483648.0 && time < 2147483648.0 ? (u32)(i32)time : UINT32_C(0x80000000);
    time_per_pizza = integer;
    demand_ratio = rest->demand_capacity_ratio[kind];
  } else if (role == 2) {
    RestDrawManagerConfig(restaurant);
    count = rest->staff_indexes[12] != 0;
  }
  if (role < 2) {
    FillRect(13, 12, 111, 203, 189);
    FillRect(15, 31, 114, 178, 186);
    FillRect(14, 37, 120, 172, 180);
    (void)DrawBitmapStuff1(8, 30, 113);
    (void)DrawBitmapStuff1(9, 164, 113);
    (void)DrawBitmapStuff1(10, 30, 166);
    (void)DrawBitmapStuff1(11, 164, 166);
    temp_canvas->color = 13;
    for (unsigned y = 131; y < 180; y += 9) {
      (void)SetTempCanvasPosition(46, (i16)y);
      DefaultLineTo(165, (u16)((i16)y));
    }
    FillRect(12, 190, 112, 203, 189);
    temp_canvas->color = 9;
    (void)SetTempCanvasPosition(42, 113);
    DefaultLineTo(166, 113);
    (void)SetTempCanvasPosition(179, 124);
    DefaultLineTo(179, 176);
    (void)SetTempCanvasPosition(30, 125);
    DefaultLineTo(30, 176);
    (void)SetTempCanvasPosition(42, 187);
    DefaultLineTo(166, 187);
    (void)SetTempCanvasPosition(45, 130);
    (void)DrawTextInsideList(staff_ui_number);
    (void)SetTempCanvasPositionX(164);
    DrawNumberInsideListRight(pt_signed_dword(count));
    (void)SetTempCanvasPosition(45, 139);
    (void)DrawTextInsideList(staff_ui_total_capacity);
    (void)SetTempCanvasPositionX(164);
    DrawNumberInsideListRight(pt_signed_dword(capacity));
    (void)SetTempCanvasPosition(45, 148);
    (void)DrawTextInsideList(staff_ui_average_quality);
    (void)SetTempCanvasPositionX(164);
    DrawNumberInsideListRight(pt_signed_dword(quality));
    (void)SetTempCanvasPosition(45, 157);
    (void)DrawTextInsideList(staff_ui_time_per_pizza);
    (void)SetTempCanvasPositionX(164);
    DrawNumberInsideListRight(pt_signed_dword(time_per_pizza));
    (void)SetTempCanvasPosition(45, 166);
    (void)DrawTextInsideList(staff_ui_work);
    (void)SetTempCanvasPositionX(164);
    DrawNumberInsideListRight(pt_signed_dword(demand_ratio));
    (void)SetTempCanvasPosition(45, 175);
    (void)DrawTextInsideList(staff_ui_total_costs);
    (void)SetTempCanvasPositionX(164);
    DrawNumberInsideListRight(pt_signed_dword(costs));
  }
  FillRect(9, 199, 30, 203, 189);

  return count;
}
u16 DrawStaffFileBackground(u8 face) {
  LoadBlitmapAndOffsets(3);
  (void)FillLetterBackground(0, 0, 13);
  RestDrawManagerConfig_Helper(12, 17, 28, 199, 171);
  temp_canvas->color = 14;
  (void)SetTempCanvasPosition(199, 25);
  (void)RelativeLineTo(0, 40);
  for (unsigned i = 0; i < 7; ++i)
    BitBlt(current_bitmap, 12, 28, current_bitmap, 12, (u16)((i16)(20u * i + 48u)), 192, 20);
  (void)DrawBitmapStuff1(13, 128, 20);
  (void)DrawBitmapStuff1(13, 17, 28);
  (void)DrawBitmapStuff1(14, 17, 170);
  (void)DrawBitmapStuff1(15, 77, 170);
  (void)DrawBitmapStuff1(16, 106, 170);
  (void)PaintCharacterFace(face, 15, 23, 33);
  (void)LoadCharacter(face, 1);
  temp_canvas->color = 0;
  SetCurrentFont(font_astandar);
  DrawText(98, 42, temp_character.first_name);
  u16 result = DrawText(98, 53, temp_character.last_name);

  return result;
}
void ShowStaffFile(const Staff *address, u8 face) {
  const Staff *staff = address;
  if (staff) {
    face = staff->face;
  }
  (void)DrawStaffFileBackground(face);
  SetCurrentFont(some_font_ptr);
  (void)snprintf((char *)strbuf, sizeof(strbuf), "%d years", temp_character.age);
  DrawText(96, 66, strbuf);
  u16 salary;
  if (staff)
    salary = (u16)staff->salary;
  else {
    salary = ComputeCharacterSalary(&temp_character);
  }
  u16 capacity = staff ? (u16)staff->cooking_or_serving_capacity : (u16)temp_character.cooking_or_serving_capacity;
  u8 cooking = staff ? staff->cooking : temp_character.cooking,
     selling = staff ? staff->selling : temp_character.selling,
     management = staff ? staff->management : temp_character.management;
  u8 experience = staff ? staff->experience : temp_character.experience,
     intelligence = staff ? staff->intelligence : temp_character.intelligence,
     trust = staff ? staff->trustworthiness : temp_character.trustworthiness;
  u8 health = staff ? staff->health : temp_character.health,
     motivation = staff ? staff->motivation : temp_character.motivation;

  DrawText(94, 81, staff_ui_payment);
  (void)SetTempCanvasPositionX(184);

  DrawRightAlignedFloat((float)salary);
  DrawText(94, 90, staff_ui_pizzas_day);
  (void)SetTempCanvasPositionX(184);
  DrawNumberInsideListRight(pt_signed_dword(capacity));
  DrawBarChart(staff_ui_cooking, cooking, 21, 103);
  DrawBarChart(staff_ui_serving, selling, 21, 121);
  DrawBarChart(staff_ui_management, management, 21, 139);
  DrawBarChart(staff_ui_experience, experience, 21, 157);
  DrawBarChart(staff_ui_intelligence, intelligence, 112, 103);
  DrawBarChart(staff_ui_trustworthiness, trust, 112, 121);
  DrawBarChart(staff_ui_health, health, 112, 139);
  DrawBarChart(staff_ui_motivation, motivation, 112, 157);
}
void DrawLetterOnBitmap(const Bitmap *source) {
  BitBlt(source, 12, 14, game_ptr->background, 0, 0, 192, 176);
  game_ptr->last_loaded_background = 0;
}
u32 HandleStaffMenu_Helper1(void) {
  for (unsigned i = 1; i < 11; ++i) {
    SetCurrentBitmap(old_bitmap);
    int y = 16 * (int)i + 20, height = 170 - 16 * (int)i;
    if (height > 0 && y < 189)
      BitBlt(old_bitmap, 17, (u16)((i16)(y - 16)), current_bitmap, 17, (u16)((i16)y), 183, (u16)((i16)height));
    BitBlt(game_ptr->background, 0, 0, current_bitmap, 12, 14, 116, (u16)((i16)(y - 6)));
    BitBlt(game_ptr->background, 116, 0, current_bitmap, 128, 14, 76, (u16)((i16)(y - 14)));
    CommitBitmapAndSetPalette((i8)0);
    Delay(3u - (i >> 2));
  }
  return CopyBmp6AToHere(current_bitmap);
}
void AnimateStaffFileHelper(i16 y) {
  if (y > 170)
    return;
  SetCurrentBitmap(old_bitmap);
  BitBlt(game_ptr->background, 117, 6, current_bitmap, 129, (u16)y, 71, 8);
  BitBlt(game_ptr->background, 6, 14, current_bitmap, 17, (u16)((i16)((u16)y + 8u)), 183, (u16)((i16)(182u - (u16)y)));
  CommitBitmapAndSetPalette((i8)0);
}
void AnimateStaffFile(void) {
  CommitBitmapAndSetPalette((i8)0);
  for (unsigned i = 0;; ++i) {
    u8 y = staff_ui_file_animation[i][0];
    if (!y)
      break;
    u8 delay = staff_ui_file_animation[i][1];
    AnimateStaffFileHelper((i16)(y + 20u));
    if (delay) {
      Delay(delay);
    }
  }
}

// Staff hire.

static u8 *translated(u8 index, bool tail_arguments) {
  u8 *result;
  (void)tail_arguments;
  result = GetTranslatedString(index);

  return result;
}
static u16 paragraph(u8 *p, i16 y) {
  u16 result;
  result = Canvas_TextRect(temp_canvas, 0, 26, p, 92, 24, 189, (u32)(i32)y);
  return result;
}
void ShowStaffApplication(u8 face) {
  LangFileDesc *descriptor = &langfile_staff_applications[(face % 100u) / 20u];
  u8 paragraph_index;
  paragraph_index = (u8)(2u * (face % 20u) + 1u);
  (void)DrawStaffFileBackground(face);
  (void)LoadLangFile(descriptor);
  SetCurrentFont(some_font_ptr);
  temp_canvas->color = 0;
  (void)DrawText(96, 63, staff_application_title);
  Game *state = game_ptr;
  state->paragraph_indent_y_threshold = 110;
  state->paragraph_background_enabled = 0;
  state->list_element_spacing_2 = 1;
  temp_canvas->color = 12;
  i16 y = (i16)(paragraph(translated(paragraph_index, true), 72));
  u8 second = (u8)(paragraph_index + 1u);
  if (*translated(second, false))
    (void)paragraph(translated(second, true), y);
  game_ptr->paragraph_indent_y_threshold = 0;
  FreeLangFile(descriptor);
}
static u8 availability(u8 index) {
  return staff_is_type[index];
}
void HandleHireStaff(Rest *restaurant, u8 role) {

  u8 staff_slot, navigation, application_visible;
  u8 face = 0;
  navigation = 0;

  Do3x3ButtonStuff(14);
  SetCurrentBitmap(old_bitmap);
  (void)RestDrawStaffSelector(restaurant, role, 100);
  DrawLetterOnBitmap(current_bitmap);
  SetCurrentBitmap(old_bitmap);
  (void)CopyBmp6AToHere(current_bitmap);
  CopyBitmap2to1();
search:

  // initial/category search.

  {
    unsigned i;
    for (i = 0; i < 200; ++i)
      if (availability((u8)i) == role) {
        face = (u8)i;
        break;
      }
    if (i == 200)
      face = 200;
  }
display:
  if (face >= 200) {
    DisplayErrorMessage(0, staff_hire_none_error);
    goto done;
  }
  SetCurrentBitmap(old_bitmap);
  ShowStaffFile(NULL, (u8)face);
  SetCurrentFont(font_supersma);
  temp_canvas->color = 0;
  (void)DrawText(134, 28, staff_hire_role_labels[0][role]);
  DrawLetterOnBitmap(current_bitmap);
  Copy320x200Bitmap(old_bitmap, current_bitmap);
  CommitBitmapAndSetPalette(0);
  if (navigation == 2) {
    (void)HandleStaffMenu_Helper1();
  } else {
    AnimateStaffFile();
  }
  navigation = 0;
  temp_canvas->color = 15;
  SetCurrentFont(font_astandar);
reset_application:
  application_visible = 0;
wait:
  mouse_pressed = WaitForMouseClick();
  if (!pt_is_menu_button_event(mouse_pressed))
    goto reset_application;
  switch (mouse_pressed) {
  case PT_MENU_BUTTON_1:
  case PT_MENU_BUTTON_2:
  case PT_MENU_BUTTON_3:
    role = (u8)(mouse_pressed - PT_MENU_BUTTON_1);
    face = 0;
    goto search;
  case PT_MENU_BUTTON_4:
    for (unsigned i = (unsigned)face + 1u; i < 200; ++i)
      if (availability((u8)i) == role) {
        face = (u8)i;
        navigation = 1;
        goto display;
      }
    for (unsigned i = 0; i < face; ++i)
      if (availability((u8)i) == role) {
        face = (u8)i;
        navigation = 1;
        goto display;
      }
    goto wait;
  case PT_MENU_BUTTON_5:
    if (application_visible == 1) {
      application_visible = 0;
      ShowStaffFile(NULL, (u8)face);
    }
    goto wait;
  case PT_MENU_BUTTON_6:
    if (!application_visible) {
      application_visible = 1;
      ShowStaffApplication(face);
    }
    goto wait;
  case PT_MENU_BUTTON_7:
    // DEC AL wraps when face is zero; preserve the physical BYTE-table
    // reads at 255..200 without indexing beyond the declared C array.

    {
      u8 i = face;
      while (--i)
        if (availability(i) == role) {
          face = i;
          navigation = 2;
          goto display;
        }
      for (i = 199; i > face; --i)
        if (availability(i) == role) {
          face = i;
          navigation = 2;
          goto display;
        }
    }
    goto wait;
  case PT_MENU_BUTTON_8: {
    CopyBitmap2to1();
    Rest *rest = restaurant;
    if ((role == 0 && rest->num_chefs >= 6) || (role == 1 && rest->num_waiters >= 6) ||
        (role == 2 && rest->staff_indexes[12])) {
      (void)snprintf(
          (char *)strbuf, sizeof(strbuf), "You cannot hire another %s here!", (char *)staff_hire_role_labels[0][role]);
      DisplayErrorMessage(0, strbuf);
      goto reset_application;
    }
    if ((UINT32_C(1) << (current_player_index & 31u)) & staff_hates_player[face]) {
      DisplayErrorMessage((u8)face, staff_hire_refusal_error);
      goto reset_application;
    }

    (void)snprintf((char *)strbuf,
        sizeof(strbuf),
        "Do you want to hire %s %s as %s in this restaurant?",
        temp_character.first_name,
        temp_character.last_name,
        (char *)staff_hire_role_labels[temp_character.is_woman][role]);
    u8 yes;
    yes = ShowMessage2(face, strbuf);
    if (!yes)
      goto reset_application;
    staff_slot = role == 0 ? rest->num_chefs : role == 1 ? (u8)(rest->num_waiters + 6u) : 12;
    u8 hired;
    hired = HireStaff(face, 255);
    if (hired) {
      u8 slot = staff_slot;

      rest->staff_indexes[slot] = hired;
      rest->gap1CE[slot] = face;
      if (!role)
        rest->num_chefs = (u8)((u32)rest->num_chefs + (u32)1u);
      if (role == 1)
        rest->num_waiters = (u8)((u32)rest->num_waiters + (u32)1u);
    }
    (void)ReComputeStaffStuff(rest);
    AdvanceClock(4);
    goto done;
  }
  case PT_MENU_BUTTON_9: {
    AdvanceClock(1);
  }
    goto done;
  default:
    goto reset_application;
  }
done:

  return;
}

// Staff courses.

// IsMouseInside uses an exclusive bottom edge, so only rows 0..5 qualify.

static const StaffTrainingCourse *course_at(u8 role, u8 row) {
  return &staff_training_courses[role][row];
}

void DrawCoursesList(const Staff *staff, u8 role) {
  u8 skill, row_index;
  skill = staff->skills[role];
  (void)DrawStaffFileBackground(staff->face);
  SetCurrentFont(some_font_ptr);
  temp_canvas->color = 10;
  (void)DrawText(132, 27, staff_course_title);
  DrawBarChart(staff_course_role_labels[role], skill, 101, 79);
  SetCurrentFont(font_piznumb);
  (void)DrawText(38, 110, staff_course_name_title);
  (void)DrawText(100, 110, staff_course_scores_title);
  (void)DrawText(138, 110, staff_course_costs_title);
  (void)DrawText(172, 110, staff_course_days_title);
  for (row_index = 0; row_index < 6; ++row_index) {
    u8 row = row_index;
    const StaffTrainingCourse *course = course_at(role, row);
    i16 y = (i16)(113u + 10u * row);
    if (skill >= course->minimum_skill)
      (void)DrawBitmapStuff1(22, 22, y);
    (void)SetTempCanvasPosition(34, y + 7);
    SetCurrentFont(font_piznumb);
    DrawTextInsideList(course->name);
    (void)SetTempCanvasPositionX(137);
    DrawNumberInsideListRight(pt_signed_dword(course->minimum_skill));
    (void)SetTempCanvasPositionX(168);
    (void)DrawDollarsInsideList(course->cost);
    (void)SetTempCanvasPositionX(192);
    DrawNumberInsideListRight(pt_signed_dword(course->duration_days));
  }
}

u8 SendStaffOnCourse(Staff *staff, u8 role) {
  u32 cost;
  u8 skill, course_index;
  u8 result;
  skill = staff->skills[role];
  SetCurrentBitmap(old_bitmap);
  DrawCoursesList(staff, role);
  DrawLetterOnBitmap(current_bitmap);
  Copy320x200Bitmap(old_bitmap, current_bitmap);
  CommitBitmapAndSetPalette(0);
  AnimateStaffFile();
  for (;;) {
    mouse_pressed = WaitForMouseClick();
    if (mouse_pressed == 1 && IsMouseInside(22, 113, 30, 173)) {
      i32 selected;
      selected = GetSelectedListItem(113, 10);
      course_index = (u8)selected;
      const StaffTrainingCourse *course = course_at(role, course_index);
      if (skill < course->minimum_skill)
        continue;
      i16 y = (i16)(114u + 10u * course_index);
      (void)DrawBitmapStuff1(23, 21, y);
      WaitForMouseLeave();
      (void)DrawBitmapStuff1(22, 21, y);
      u8 yes;
      yes = ShowMessage2(staff->face, staff_course_confirm_prompt);
      if (!yes)
        continue;
      cost = course->cost;
      float amount = (float)cost;
      u32 paid;
      // Stock sign-extends the player BYTE before its call. The finance
      // entry consumes the BYTE and retains its own selection rules.

      paid = TransferPlayerMoney(current_player_index, 10, 5, amount);
      if ((u16)paid) {
        Staff *record = staff;
        record->absent_days = course->duration_days;
        record->absent_reason = course_index;
        result = 50;
        break;
      }
    }
    if (pt_is_menu_button_event(mouse_pressed) && mouse_pressed != PT_MENU_BUTTON_6) {
      result = (u8)mouse_pressed;
      break;
    }
  }

  return result;
}

// Staff menu.

u16 ComputeCharacterSalary(const Character *character) {
  // Salary consumes these six fields; the other Staff fields are unused.

  Staff staff;
  staff.cooking = character->cooking;
  staff.selling = character->selling;
  staff.management = character->management;
  staff.cooking_or_serving_capacity = character->cooking_or_serving_capacity;
  staff.trustworthiness = character->trustworthiness;
  staff.intelligence = character->intelligence;
  return ComputeWantedStaffSalary(&staff);
}
u8 HireStaff(u8 face, u8 assignment) {
  u8 index = 1;
  while (index < 200 && pt_staff_record(index)->first_name[0])
    ++index;
  if (index >= 200) {
    DisplayErrorMessage(0, staff_pool_full_error);
    return 0;
  }
  (void)LoadCharacter(face, 1);
  Staff *staff = pt_staff_record(index);
  (void)pt_strcpy(staff->first_name, temp_character.first_name);
  (void)pt_strcpy(staff->last_name, temp_character.last_name);
  u16 salary;
  salary = ComputeCharacterSalary(&temp_character);
  staff->salary = (i16)salary;
  staff->cooking_or_serving_capacity = temp_character.cooking_or_serving_capacity;
  staff->woman = temp_character.is_woman;
  staff->absent_reason = 0;
  staff->byte2D = assignment;
  staff->face = face;
  // The copied physical tail includes both character clock bytes.

  (void)memmove(staff->traits, temp_character.staff_traits, sizeof(staff->traits));
  staff_is_type[face] = (u8)(current_player_index + 8u);
  if ((unsigned)ramtown.num_staff + 1u == index)
    ++ramtown.num_staff;
  return index;
}
void RestRemoveStaff(Rest *rest, u8 role, u8 index) {
  u8 slot = (u8)(6u * role + index);
  Staff *staff = pt_rest_staff_member(rest, slot);
  u8 face = staff->face;
  (*pt_rest_staff_slot(rest, slot)) = 0;
  if (role < 2) {
    u8 *row = &(*pt_rest_staff_slot(rest, slot));
    (void)memmove(row, row + 1, 5u - index);
    if (!role)
      rest->num_chefs = (u8)((u32)rest->num_chefs - (u32)1u);
    else
      rest->num_waiters = (u8)((u32)rest->num_waiters - (u32)1u);
  }
  (void)ReComputeStaffStuff(rest);
  ReClearEmptyStaffSlots(rest);
  staff->first_name[0] = 0;
  if (staff == pt_staff_record(ramtown.num_staff))
    --ramtown.num_staff;
  u8 kind;
  kind = LemonComputeValue(staff);
  staff_is_type[face] = kind;
  staff_hates_player[face] |= (u8)(UINT32_C(1) << (rest->player_index & 31u));
}

u32 DrawStaffRowBg(i16 y, u8 style) {
  DrawBitmapStuff1((u8)(style + 2u), 16, y);
  for (unsigned i = 0; i < 6; ++i)
    DrawBitmapStuff1((u8)(style + 3u), (i16)(20u * i + 48u), y);
  u32 result = DrawBitmapStuff1((u8)(style + 4u), 169, y);
  return result;
}

// Staff daily.

#include "game.h"
u8 LemonComputeValue(const Staff *staff) {
  if (staff->selling > staff->cooking && staff->selling > staff->management)
    return 1;
  return staff->management > staff->cooking && staff->management > staff->selling ? 2 : 0;
}

static u8 motivation(u8 previous, u32 delta) {
  i32 value = (i16)((u16)(previous + delta));
  return (u8)(value < 0 ? 0 : value > 100 ? 100 : value);
}

void StaffAddMotivation(Staff *staff, u32 delta) {
  staff->motivation = motivation(staff->motivation, delta);
}

u16 ComputeWantedStaffSalary(const Staff *staff) {
  unsigned role = LemonComputeValue(staff);

  u32 skills = staff->selling + (role == 0 ? staff->cooking : role == 1 ? staff->management : 6u * staff->management);
  u16 base = (u16)(skills + (u16)staff->cooking_or_serving_capacity + staff->trustworthiness);

  u16 adjusted = (u16)MulDiv100(base, 100 + (staff->intelligence >> 1));

  u16 result = (u16)RoundNumberToZeros(adjusted, 1);

  return result;
}

static u32 live_random(PTStaffDay *c, u32 range) {
  (void)c;

  u32 value = RandomNumber(range);

  return value;
}

static u8 live_chance(PTStaffDay *c, i32 probability) {
  (void)c;

  u8 value = RandomChance(probability);

  return value;
}

static void live_motivate(PTStaffDay *c, unsigned index, int delta) {
  (void)c;
  StaffAddMotivation(&g_staff[index], (u32)delta);
}

static void live_training_goal(PTStaffDay *c, unsigned role) {
  (void)c;
  MarkGoalReached((u8)(role + 66));
}

static void live_training_message(PTStaffDay *c, Staff *staff, unsigned index, const u8 *text, size_t n) {
  // Make skill/capacity changes visible before the original callees.

  g_staff[index] = *staff;

  memcpy(strbuf, text, n + 1u);

  ShowDelayedMessage(c->view.player_index, 100, 60, strbuf);
}

static void live_money_loss(PTStaffDay *c, u16 amount) {
  TransferPlayerMoney(current_player_index, 10, 5, (float)amount);

  AddCostForRestaurant(c->restaurant, amount, 8);

  Player *player = player_ptr;
  player->last_insurance_damage_type = 1;
  player->last_crashed_furniture_value = amount;
}

static u16 live_salary(PTStaffDay *c, const Staff *staff, unsigned index) {
  (void)c;
  (void)staff;
  u16 desired = ComputeWantedStaffSalary(&g_staff[index]);
  return desired;
}

static unsigned live_cash(PTStaffDay *c, double *cash) {
  (void)c;
  *cash = character_ptr->money;
  return 0;
}

static void live_staff_changed(PTStaffDay *c, Staff *staff, unsigned index) {
  (void)c;
  g_staff[index] = *staff;
}

static void live_finished(PTStaffDay *c) {
  ptr_apple->gap3C = c->apple.gap3C;
}

static const PTStaffDayServices staff_live_services = {
  .random_number = live_random,
  .chance = live_chance,
  .motivate = live_motivate,
  .training_goal = live_training_goal,
  .training_message = live_training_message,
  .money_loss = live_money_loss,
  .salary = live_salary,
  .cash = live_cash,
  .staff_changed = live_staff_changed,
  .finished = live_finished,
};

static void add_motivation(PTStaffDay *c, Staff *staff, unsigned index, int delta) {
  staff->motivation = motivation(staff->motivation, (u32)delta);
  c->services->motivate(c, index, delta);
}
static double multiply53(double a, double b) {
  volatile double result = a * b;
  return result;
}
static void training(PTStaffDay *c, Staff *staff, unsigned role, unsigned index) {
  unsigned course = staff->absent_reason;
  const StaffTrainingCourse *record = &staff_training_courses[role][course];
  bool success = c->services->chance(c, (int)staff->motivation + 20) || course <= 1;
  if (success) {
    u8 *skill = role == 0 ? &staff->cooking : role == 1 ? &staff->selling : &staff->management;
    *skill = (u8)(*skill + record->skill_gain);
    staff->cooking_or_serving_capacity = (i16)((u16)((u16)staff->cooking_or_serving_capacity + record->capacity_gain));
    if (course == 5) {
      c->services->training_goal(c, role);
      if (c->status)
        return;
    }
  }
  if (!memchr(staff->first_name, 0, sizeof(staff->first_name)) ||
      !memchr(staff->last_name, 0, sizeof(staff->last_name)) || !memchr(record->name, 0, sizeof(record->name))) {
    c->status = 1;
    return;
  }
  u8 text[256];
  int n = snprintf((char *)text,
      sizeof(text),
      "%s %s has  been on the %s training course and has %s completed it.",
      staff->first_name,
      staff->last_name,
      record->name,
      success ? "successfully" : "not");
  if (n < 0 || n >= 256) {
    c->status = 1;
    return;
  }
  c->services->training_message(c, staff, index, text, (size_t)n);
  if (c->status)
    return;
  if (staff->intelligence < 90)
    staff->intelligence = (u8)(staff->intelligence + c->services->random_number(c, course + 1u) + 4u);
}
void pt_staff_day_run(PTStaffDay *c) {
  bool incident = false;
  for (unsigned slot = 0; slot < 13 && !c->status; ++slot) {
    unsigned index = c->view.staff_indexes[slot];
    if (!index)
      continue;
    if (index >= 200) {
      c->status = 1;
      return;
    }
    Staff *staff = &c->staff[index];
    unsigned role = slot / 6u;
    if (staff->absent_reason == 201)
      staff->absent_reason = 0;
    if ((u16)staff->absent_days) {
      staff->absent_days = (i16)((u16)((u16)staff->absent_days - 1u));
      if (!staff->absent_days && staff->absent_reason < 6)
        training(c, staff, role, index);
    } else if (!incident && c->view.restaurant_opened) {
      bool event = false;
      if (!c->services->chance(c, staff->trustworthiness) && !c->services->chance(c, staff->motivation)) {
        staff->absent_reason = 101;
        event = true;
      } else if (c->services->chance(c, 10) && !c->services->chance(c, (int)staff->health + 20) &&
          !c->services->chance(c, staff->motivation)) {
        staff->absent_reason = 102;
        staff->absent_days = (i16)(c->services->random_number(c, 3) + 1u);
        event = true;
      } else if (staff->woman && c->services->chance(c, 4) && c->services->chance(c, 4) &&
          c->services->chance(c, staff->prop_5) && c->services->chance(c, staff->prop_5)) {
        staff->absent_reason = 103;
        staff->absent_days = 90;
        event = true;
      } else if (c->services->chance(c, staff->prop_6)) {
        u16 desired = c->services->salary(c, staff, index);
        u16 salary = (u16)staff->salary;
        if (desired > salary) {
          add_motivation(c, staff, index, -2);
          if ((double)desired > multiply53(salary, staff_daily_underpaid_factor))
            add_motivation(c, staff, index, -2);
          if (staff->absent_reason == 205)
            add_motivation(c, staff, index, -8);
        }
        if (salary > desired && staff->motivation < 70)
          add_motivation(c, staff, index, 1);
        if (multiply53(desired, staff_daily_overpaid_factor) < salary && staff->motivation < 90)
          add_motivation(c, staff, index, 1);
        event = true;
      } else if (!staff->motivation && c->services->chance(c, staff->prop_8)) {
        staff->absent_reason = 104;
        event = true;
      } else if (staff->motivation < 15 && c->services->chance(c, staff->prop_8)) {
        staff->absent_reason = 105;
        event = true;
      } else if (c->services->chance(c, 3) && !c->services->chance(c, (int)staff->prop_11 + 20)) {
        u16 amount = (u16)(10u * c->services->random_number(c, (u16)staff->salary));
        double cash;
        c->status = c->services->cash(c, &cash);
        if (c->status)
          return;
        if ((double)amount < cash) {
          c->services->money_loss(c, amount);
          if (c->status)
            return;
          staff->absent_reason = 106;
          add_motivation(c, staff, index, 30);
          event = true;
        }
      }
      // The theft condition can be true without sufficient cash; the
      // stock then continues to the final two-draw behavior event.

      if (!event && c->services->chance(c, 5) && !c->services->chance(c, (int)staff->trustworthiness + 10)) {
        u8 reason = (u8)(c->services->random_number(c, 2) + 2u * role + 110u);
        c->apple.gap3C = (u8)((u32)c->apple.gap3C + (u32)1u);
        staff->absent_reason = reason > 114 ? 114 : reason;
        event = true;
      }
      if (event) {
        incident = true;
        c->apple.gap3C = (u8)((u32)c->apple.gap3C + (u32)1u);
      }
    }
    c->services->staff_changed(c, staff, index);
  }
  if (!c->status)
    c->services->finished(c);
}
void RestRunStaffDaily(Rest *rest) {
  PTStaffDay c = {
    .restaurant = rest, .view = *rest, .apple = *ptr_apple, .rng = pt_random_state, .services = &staff_live_services};

  memcpy(c.staff, g_staff, sizeof(c.staff));
  pt_staff_day_run(&c);
  if (c.status)
    abort();
}

// Staff disaster.

static void message(u8 face, u8 index) {
  u8 *text = GetTranslatedString(index);
  DisplayErrorMessage(face, text);
}
static const char *string(const u8 *text, size_t maximum) {
  (void)maximum;
  return (const char *)text;
}
u8 GetManagerPolicy1(Rest *rest) {
  u32 policy = rest->mgr_staff_behavior;
  if (policy == 6)
    policy = 5;
  return (u8)((5u - policy) >> 1);
}
u8 AskStaffDisasterQuestion(Rest *rest, u8 face, u8 message_set) {
  u8 question[500], variants[4];
  u8 base = (u8)(20u * message_set);
  for (unsigned i = 0; i < 4; ++i) {
    u32 n = RandomNumber(3);
    variants[i] = (u8)(n + 1u);
  }
  const u8 *text = GetTranslatedString((u8)(base + variants[0])), *name;
  name = GetRestaurantName(rest);
  int n = snprintf((char *)question,
      sizeof(question),
      (const char *)staff_disaster_question_format,
      string(name, 128),
      string(text, 500));
  (void)n;
  DisplayErrorMessage(face, question);
  u8 *third = GetTranslatedString((u8)(base + variants[3] + 9u));
  u8 *second = GetTranslatedString((u8)(base + variants[2] + 6u));
  u8 *first = GetTranslatedString((u8)(base + variants[1] + 3u));
  return ShowMultichoiceQuestion(3, first, second, third, NULL);
}
u8 HandleStaffDisasterInner(Rest *rest, Staff *staff, u8 index) {
  u8 event = (u8)(staff->absent_reason - 110u);
  switch (event) {
  case 0:
    for (unsigned i = 73; i < 80; ++i)
      rest->stock[i].weight = 0;
    break;
  case 1: {
    if (!rest->num_ovens) {
      return 0;
    }
    u32 selected = RandomNumber(rest->num_ovens);
    Furniture *oven = pt_rest_furniture_view(rest, (u8)selected, true);
    (void)CrashFurniture(oven);
    (void)ReComputeStaffStuff(rest);
    Player *player = player_ptr;
    player->last_insurance_damage_type = 3;
    u16 price = GetPriceOfFurniture(oven->type);
    player = player_ptr;
    player->last_crashed_furniture_value = price;
    attack_damage = 0;
    break;
  }
  case 2:
    special_debt_flag = 1;
    (void)TransferPlayerMoney(current_player_index, 10, 5, 1000.0f);
    special_debt_flag = 0;
    AddCostForRestaurant(rest, 1000, 8);
    break;
  case 3:
    AddRestaurantRangeStuff(rest, (u8)-30, 10);
    break;
  case 4:
    rest->restaurant_closed_for_days = 1;
    rest->restaurant_opened = 0;
    break;
  default:
    break;
  }
  u8 selected = AskStaffDisasterQuestion(rest, staff->face, event);
  if (selected == 2)
    RestRemoveStaff(rest, (u8)(event / 2u), index);
  if (RandomChance(60))
    message(staff->face, (u8)(selected + 20u * event + 13u));
  staff->absent_reason = (u8)(staff->absent_reason + 100u);
  return 1;
}
u8 HandleStaffDisaster(Rest *rest, Staff *staff, u8 role, u8 index) {
  u8 result = 0;
  u8 reason = staff->absent_reason, face = staff->face;
  if (reason > 100 && reason < 200 && !game_ptr->disable_staff_disasters) {
    LangFileDesc *lang = reason >= 110 ? &langfile_persf : &langfile_pers;
    (void)LoadLangFile(lang);
    if (reason >= 110) {
      u8 value = HandleStaffDisasterInner(rest, staff, index);
      result = value;
    } else {
      u8 selected = 0;
      switch (reason) {
      case 101:
      case 102:
        if (rest->staff_indexes[12]) {
          selected = GetManagerPolicy1(rest);
        } else {
          selected = AskStaffDisasterQuestion(rest, face, (u8)(reason - 100u));
          if (RandomChance(80))
            message(face, (u8)(selected + (reason == 101 ? 33u : 53u)));
        }
        StaffAddMotivation(staff, (u32)(i32)(reason == 101 ? 2 - 4 * (i32)selected : 3 - 8 * (i32)selected));
        break;
      case 103:
        (void)AskStaffDisasterQuestion(rest, face, 5);
        break;
      case 104: {
        selected = AskStaffDisasterQuestion(rest, face, 4);
        u8 probability = 0;
        if (!selected)
          RestRemoveStaff(rest, role, index);
        else if (selected == 1) {
          probability = 40;
          double integral;
          integral = TruncateX87Integral((double)(u16)staff->salary * staff_disaster_raise_factor);
          u32 bits = isfinite(integral) && integral >= -2147483648.0 && integral < 2147483648.0 ? (u32)(i32)integral
                                                                                                : UINT32_C(0x80000000);
          staff->salary = (i16)bits;
        } else if (selected == 2) {
          probability = 80;
          staff->salary = (i16)((u16)staff->salary * 2u);
        }
        if (probability) {
          if (RandomChance(probability)) {
            message(face, 94);
            staff->motivation = 20;
          } else {
            message(face, 93);
            RestRemoveStaff(rest, role, index);
          }
        }
        break;
      }
      case 105:
        selected = AskStaffDisasterQuestion(rest, face, 3);
        StaffAddMotivation(staff, (u32)(i32)(8 * (1 - (i32)selected)));
        if (RandomChance(80))
          message(face, (u8)(selected + 73u));
        break;
      case 106:
        ShowDelayedMessage(rest->player_index, rest->restaurant_id, ptr_apple->minutesx10, staff_disaster_theft_text);
        int n = snprintf((char *)strbuf,
            sizeof(strbuf),
            (const char *)staff_disaster_car_format,
            string(staff->first_name, sizeof(staff->first_name)),
            string(staff->last_name, sizeof(staff->last_name)));
        (void)n;
        ShowDelayedMessage(rest->player_index, rest->restaurant_id, (u8)(ptr_apple->minutesx10 + 2u), strbuf);
        break;
      default:
        goto freed;
      }
      result = 1;
      staff->absent_reason = (u8)(staff->absent_reason + 100u);
    }
  freed:
    FreeLangFile(lang);
  }
  return result;
}

u8 HandleAllStaffDisaster(Rest *rest) {
  u8 result = 0;
  u8 i;
  if (ptr_apple->minutesx10 >= 60) {
    for (i = 0; i < rest->num_chefs; ++i)
      if (!result)
        result = HandleStaffDisaster(rest, pt_rest_staff_member(rest, i), 0, i);
    for (i = 0; i < rest->num_waiters; ++i)
      if (!result)
        result = HandleStaffDisaster(rest, pt_rest_staff_member(rest, i + 6u), 1, i);
    if (rest->staff_indexes[12] && !result)
      result = HandleStaffDisaster(rest, pt_rest_staff_member(rest, 12), 2, 0);
  }
  return result;
}

// Furniture menu.

#include "trade.h"
#include "runtime_state.h"

static u16 wait_input(void) {
  u16 v = WaitForMouseClick();
  mouse_pressed = v;
  return v;
}

void SetupSomeSpacingStuff(i16 left, i16 spacing) {
  if (left)
    game_ptr->word92 = left;
  if (spacing)
    game_ptr->word90 = spacing;
  game_ptr->list_element_spacing_2 = (u8)((u16)game_ptr->word90 - (u16)temp_canvas->font_height);
}
PTActionResult FillCanvasAndSetButtons(u8 menu) {
  temp_canvas->color = 17;
  Canvas_FillRect(temp_canvas, 228, 118, 310, 194);
  return Do3x3ButtonStuff(menu);
}
u16 DrawInventoryListHeader(void) {
  FillRect(9, 29, 116, 181, 173);
  FillRect(13, 30, 117, 180, 126);
  FillRect(14, 30, 128, 172, 172);
  FillRect(13, 174, 128, 180, 149);
  FillRect(13, 174, 151, 180, 172);
  SetCurrentFont(some_font_ptr);
  temp_canvas->color = 10;
  (void)DrawText(33, 124, furniture_inventory_num_text);
  return DrawText(52, 124, furniture_inventory_name_text);
}
void DrawInventoryListContents(FurnitureInventoryRow *rows, u8 *count) {
  i8 remaining = (i8)*count;
  SetCurrentFont(some_font_ptr);
  temp_canvas->color = 11;
  for (unsigned i = 0; i < 7; ++i) {
    if (remaining > 0) {
      (void)SetTempCanvasPosition(32, (i16)(6u * i + 135u));
      DrawTextInsideList(rows[i].text);
    }
    remaining = (i8)((u8)remaining - 1u);
  }
}
u32 ComputeFurnitureCapacityPerDay(Rest *rest, FurnitureInventoryRow *rows, u8 *count, Furniture *items, u8 category) {
  u32 total;
  u8 written, type_index, item_count;
  written = 0;
  item_count = category == 3 ? rest->num_ovens : rest->num_furniture;
  total = 0;
  for (type_index = 0; type_index < 164; ++type_index)
    if (furniture_desc[type_index].field_0 == category) {
      for (unsigned j = 0; j < item_count; ++j)
        if (items[j].type == type_index) {
          u8 amount = 0;
          for (; j < item_count; ++j)
            if (items[j].type == type_index)
              ++amount;
          FurnitureInventoryRow *target = &rows[written];
          {
            int length =
                snprintf((char *)target->text, 36, "%u %s", amount, (const char *)furniture_desc[type_index].name);
            (void)length;
          }
          u16 price = GetPriceOfFurniture(type_index);
          total += (u32)amount * price;
          ++written;
          break;
        }
    }
  *count = written;
  return total;
}
void DrawSampleFloor(Floor *floor, i8 x, i8 y) {
  LoadFloorBitmaps(floor->floor_bg);
  for (u8 i = 0; i < 4; ++i)
    for (u8 j = 0; j < 4; ++j) {
      i8 tx = (i8)((u8)x + i), ty = (i8)((u8)y + j);
      u8 tile = floor->piece[j % 2u][i % 2u];
      (void)DrawFloorPiece(tile, 0, tx, ty);
    }
}
void DrawFurnitureMainPage(Rest *rest, FurnitureInventoryRow *rows, u8 *count, u8 mode) {

  u8 extra_type, extra_count;
  LoadBlitmapAndOffsets(1);
  FillLetterBackground(4, 9, 4);
  SetSomeFieldInCanvas(0);
  SetCurrentFont(font_astandar);
  u32 value = 0;
  if (mode == 1) {
    DrawBitmapStuff1(9, 34, 18);
    temp_canvas->color = 0;
    DrawText2(60, 190, 30, furniture_floor_title);
    DrawSampleFloor(&rest->floor, 3, 8);
    DrawSampleFloor(&rest->floor, 7, 8);
    SetCurrentFont(font_piznumb);

    const Poi *poi = rest->property;
    u32 tiles = (u32)poi->size_x * poi->size_y;
    (void)DrawText(30, 60, furniture_value_text);
    (void)SetTempCanvasPositionX(90);
    (void)DrawDollarsInsideList((i32)(tiles * rest->floor.price));
    (void)DrawText(30, 70, furniture_quality_text);
    (void)SetTempCanvasPositionX(90);

    (void)DrawDollarsInsideList(furniture_suppliers[rest->floor.type].quality);
    snprintf((char *)strbuf, sizeof(strbuf), "number of tiles: %u * %u = %u", poi->size_x, poi->size_y, tiles);
    (void)DrawText(30, 80, strbuf);
    snprintf((char *)strbuf, sizeof(strbuf), "area: %u sqm", tiles >> 2);
    (void)DrawText(30, 90, strbuf);
    goto done;
  }
  if (mode == 5) {
    (void)DrawCustomerRatings(rest, 0);
    goto done;
  }
  if (mode == 2) {
    DrawBitmapStuff1(7, 34, 18);
    temp_canvas->color = 0;
    DrawText2(60, 190, 30, furniture_equipment_title);
    SetCurrentFont(font_piznumb);
    temp_canvas->color = 0;
    (void)SetTempCanvasPosition(29, 65);
    DrawTextInsideList(furniture_in_use_text);
    (void)SetTempCanvasPositionX(142);
    DrawPercentageNumberInList((u8)(rest->demand_capacity_ratio[0]));
    (void)SetTempCanvasPosition(29, 75);
    DrawTextInsideList(furniture_value_text);
    (void)SetTempCanvasPosition(29, 85);
    DrawTextInsideList(furniture_quality_text);
    (void)SetTempCanvasPositionX(142);
    DrawNumberInsideListRight(rest->furniture_quality);
    (void)SetTempCanvasPosition(29, 95);
    DrawTextInsideList(poi_capacity_text);
    (void)SetTempCanvasPositionX(142);
    DrawNumberInsideListRight((u16)rest->total_num_seats);
    (void)SetTempCanvasPositionX(150);
    DrawTextInsideList(furniture_seats_text);
    (void)SetTempCanvasPosition(142, 105);
    DrawNumberInsideListRight((u16)rest->capacity_of_kind[0]);
    (void)SetTempCanvasPositionX(150);
    DrawTextInsideList(furniture_per_day_text);
    value = ComputeFurnitureCapacityPerDay(rest, rows, count, rest->furniture, 2);
    (void)SetTempCanvasPosition(142, 75);
    DrawNumberInsideListRight(pt_signed_dword(value));
  } else if (mode == 3) {
    value = ComputeFurnitureCapacityPerDay(rest, rows, count, rest->ovens, 3);
    u16 ovens = 0;
    for (unsigned i = 0; i < 164; ++i)
      if (i != 162 && furniture_desc[i].field_0 == 3)
        for (unsigned j = 0; j < rest->num_ovens; ++j)
          if (rest->ovens[j].type == i)
            ++ovens;
    DrawBitmapStuff1(8, 34, 18);
    temp_canvas->color = 0;
    DrawText2(60, 190, 30, furniture_kitchenware_title);
    Canvas_SetFont(temp_canvas, font_piznumb);
    temp_canvas->color = 0;
    (void)SetTempCanvasPosition(29, 45);
    DrawTextInsideList(furniture_ovens_text);
    (void)SetTempCanvasPositionX(123);
    DrawNumberInsideListRight(ovens);
    (void)SetTempCanvasPosition(28, 55);
    DrawTextInsideList(furniture_bake_time_text);
    (void)SetTempCanvasPositionX(129);
    DrawRightAlignedFloat(rest->average_oven_time);
    (void)SetTempCanvasPositionX(142);
    DrawTextInsideList(furniture_minutes_text);
    (void)SetTempCanvasPosition(29, 65);
    DrawTextInsideList(furniture_in_use_text);
    (void)SetTempCanvasPositionX(123);
    DrawPercentageNumberInList((u8)(rest->demand_capacity_ratio[3]));
    (void)SetTempCanvasPosition(29, 75);
    DrawTextInsideList(furniture_value_text);
    (void)SetTempCanvasPositionX(128);
    (void)DrawDollarsInsideList(pt_signed_dword(value));
    (void)SetTempCanvasPosition(29, 85);
    DrawTextInsideList(furniture_quality_text);
    (void)SetTempCanvasPositionX(123);
    DrawNumberInsideListRight(rest->oven_quality);
    (void)SetTempCanvasPosition(29, 95);
    DrawTextInsideList(furniture_baking_capacity_text);
    (void)SetTempCanvasPositionX(123);
    DrawNumberInsideListRight((u16)rest->oven_capacity);
    (void)SetTempCanvasPositionX(130);
    DrawTextInsideList(furniture_number_text);
    (void)SetTempCanvasPosition(123, 105);
    DrawNumberInsideListRight((u16)rest->capacity_of_kind[3]);
    (void)SetTempCanvasPositionX(130);
    DrawTextInsideList(furniture_per_day_text);
  } else if (mode == 4) {

    (void)ComputeFurnitureCapacityPerDay(rest, rows, count, rest->furniture, 4);
    DrawBitmapStuff1(10, 34, 18);
    temp_canvas->color = 0;
    DrawText2(60, 190, 30, furniture_extras_title);
    SetCurrentFont(font_piznumb);
    for (extra_type = 0; extra_type < 4; ++extra_type) {
      u8 i = extra_type;
      extra_count = 0;
      (void)SetTempCanvasPosition(32, (i16)(12u * i + 50u));
      DrawListElement(furniture_extras_labels[i]);
      cur_list_elem_x = 146;
      for (unsigned j = 0; j < rest->num_furniture; ++j)
        if (furniture_desc[rest->furniture[j].type].furn_type == i + 3u)
          ++extra_count;
      DrawNumberInsideListRight(extra_count);
    }
  } else
    goto done;
  (void)DrawInventoryListHeader();
  DrawInventoryListContents(rows, count);
done:
  return;
}
void HandleFurnitureStuff1(const u8 *value, i8 direction, u8 rows, u8 c, u8 bg, i16 x, u16 y, u16 width) {
  i16 delta = direction < 0 ? -1 : 1;
  BackupCanvasAttr();
  u16 spacing = (u16)game_ptr->word90;
  (void)BitBlt(current_bitmap, (u16)x, (u16)(y - 1u), old_bitmap, (u16)x, 29, width, (u16)(rows * spacing + 1u));
  temp_canvas->bitmap = old_bitmap;
  FillRect((u8)bg,
      (u16)x,
      (u16)((i32)direction * game_ptr->word90 + 30),
      (u16)(width + (u16)x - 1u),
      (u16)((i32)direction * game_ptr->word90 + game_ptr->word90 + 29));
  temp_canvas->text_background_enabled = 0;
  temp_canvas->color = (u8)c;
  (void)SetTempCanvasPosition(x, (i16)((u32)((i32)direction * game_ptr->word90 + 35)));
  DrawTextInsideList(value);
  u16 source_y = 30;
  for (i32 row = 0; row <= game_ptr->word90; ++row) {
    (void)BitBlt(old_bitmap, (u16)x, source_y, current_bitmap, (u16)x, y, width, (u16)(rows * (u16)game_ptr->word90));
    if (row < game_ptr->word90)
      Delay(1);
    source_y = (u16)(source_y + delta);
  }
  temp_canvas->bitmap = current_bitmap;
  RestoreCanvasAttrs();
}
static const BobHeader *catalogue_sprite(const BobHost *bob, u8 type) {
  u16 image = (u16)furniture_desc[type].images[0];

  return pt_resource_header_at(bob, (u16)image);
}
static void catalogue_format(const u8 *format, unsigned n) {
  snprintf((char *)strbuf, sizeof(strbuf), (const char *)format, (int)n);
  DrawStringBufferAsListElement();
}
void HandleBuyStuff_Helper(const FurnitureCatalogueEntry *row) {
  DrawBob(&bob_restaurant, 325, 160, row->overlay_y);
  temp_canvas->color = 0;
  (void)SetTempCanvasPosition(184, (i16)(row->overlay_y + 16u));
  DrawNumberInsideListRight((i8)row->quantity);
}
u8 HandleBuyStuff(u8 supplier_index) {
  FurnitureCatalogueEntry rows[32];
  u8 catalogue_category, redraw, next_y, on_page, count, first;
  u8 total_i, line_count;
  u32 reset_type;
  redraw = 0;
  catalogue_category = furniture_suppliers[supplier_index].category;

  count = 0;
  u8 page = 1;
  next_y = 42;
  u32 total = 0;
  for (unsigned i = 0; i < 32; ++i) {
    rows[i].quantity = 0;
    rows[i].page = 100;
  }
  BobHost *load_bob = catalogue_category == 3 ? &bob_kitchen : &bob_restaurant_open;

  (void)LoadBobFile(load_bob);
  BobHost *bob = catalogue_category == 3 ? &bob_kitchen : &bob_restaurant;
  SetCurrentFont(font_piznumb);

  for (unsigned type = 0; type < 164; ++type) {
    FurnitureDesc *d = &furniture_desc[type];
    d->amount_in_van = 0;
    if (d->field_0 != catalogue_category || d->field_2 != supplier_index)
      continue;

    FurnitureCatalogueEntry *row = &rows[count];
    row->type = (u8)type;
    const BobHeader *sprite = catalogue_sprite(bob, (u8)type);
    ++count;
    if ((unsigned)next_y + sprite->h > 172u) {
      next_y = 46;
      ++page;
    }
    row->y = next_y;
    unsigned advance = sprite->h + 16u;
    if (catalogue_category == 3 && advance < 48u)
      advance = 48;
    next_y = (u8)(next_y + advance);
    row->page = page;
    row->overlay_y = row->y;
    row->quantity = 0;
  }
  // Empty catalogues and 32-item sentinel reads require original bounds
  // repairs before those branches can receive a passing verdict.

  page = 1;
redraw_catalogue:;
  unsigned j = 0;
  while (j < count && rows[j].page != page)
    ++j;
  first = (u8)j;
  while (j < count && rows[j].page == page)
    ++j;
  on_page = (u8)(j - first);
  (void)InitIglooLink(&map_igloo_head, &map_igloo_tail, &map_igloo_link);
  temp_canvas->actors = &map_igloo_link;
  SetSomeFieldInCanvas(0);
  FillLetterBackground(4, 9, 4);
  if (catalogue_category >= 1 && catalogue_category <= 4)
    DrawBitmapStuff1((u8[]){9, 7, 8, 10}[catalogue_category - 1u], 34, 18);
  SetCurrentFont(font_piznumb);
  temp_canvas->color = 0;
  DrawText2(60, 190, 30, furniture_suppliers[supplier_index].name);
  (void)SetTempCanvasPosition(97, 177);
  snprintf((char *)strbuf, sizeof(strbuf), "- %u -", page);
  DrawTextInsideList(strbuf);
  for (unsigned k = first;; ++k) {
    if (k >= (unsigned)first + on_page)
      break;
    FurnitureCatalogueEntry *row = &rows[k];
    FurnitureDesc *d = &furniture_desc[row->type];
    const BobHeader *sprite = catalogue_sprite(bob, row->type);

    FillRect(14, 24, (u16)(row->y - 3u), (u16)(sprite->w + 30u), (u16)(row->y + sprite->h + 3u));
    (void)Canvas_DrawRect(13, 21, (u16)(row->y - 6u), (u16)(sprite->w + 33u), (u16)(row->y + sprite->h + 6u));
    unsigned slot = k - first;

    (void)SetupHazelWithBobSprite(pt_hazelnut_pool_at(slot), 0, bob, (u16)d->images[0]);
    pt_igloo_pool_at(slot)->draw_x = 27;
    pt_igloo_pool_at(slot)->draw_y = row->y;
    temp_canvas->color = 9;
    i16 left = (u16)d->value == 50000u ? 113 : 101;

    (void)SetTempCanvasPosition(left, (i16)(row->y + 5u));
    DrawTextInsideList(d->name);
    (void)SetTempCanvasPosition(left, (i16)(row->y + 14u));
    DrawTextInsideList(furniture_price_text);
    (void)SetTempCanvasPositionX((i16)(left + 58));
    u16 price = GetPriceOfFurniture(row->type);
    (void)DrawDollarsInsideList(price);
    draw_list_elem_style = 0;
    if (catalogue_category == 3) {
      (void)SetTempCanvasPosition(left, (i16)(row->y + 23u));
      catalogue_format(furniture_capacity_format, d->capacity);
      (void)SetTempCanvasPosition(left, (i16)(row->y + 32u));
      catalogue_format(furniture_baking_time_format, d->baking_time);
      cur_list_elem_y = (i16)((u16)cur_list_elem_y + 9u);
      catalogue_format(furniture_quality_format, d->quality);
    }
  }
  SortCanvasIgloo(temp_canvas);
  (void)Canvas_DrawAllIgloos(temp_canvas, active_palette_pointer);
  for (unsigned k = first; k < (unsigned)first + on_page; ++k)
    if (rows[k].quantity)
      HandleBuyStuff_Helper(&rows[k]);
  for (;;) {
    u16 event = wait_input();

    const Mouse *mouse = mouse_result;
    if (event < 3 && mouse->x > 20) {
      cur_list_elem_x = mouse->x;
      cur_list_elem_y = mouse->y;
      unsigned selected = 100;
      for (unsigned k = first; k < (unsigned)first + on_page; ++k) {
        const BobHeader *sprite = catalogue_sprite(bob, rows[k].type);

        if (cur_list_elem_y < (i32)sprite->h + rows[k].y + 6) {
          selected = k;
          break;
        }
      }
      if (selected == 100)
        continue;
      FurnitureCatalogueEntry *row = &rows[selected];
      if (event == 1) {
        if ((i8)row->quantity >= 99)
          ShowMessage(16, furniture_selection_limit_text);
        else
          ++row->quantity;
      }
      if (event == 2 && row->quantity)
        --row->quantity;
      total = 0;
      for (total_i = 0; total_i < 32; ++total_i)
        if (rows[total_i].quantity) {
          i32 qty = (i8)rows[total_i].quantity;

          u16 price;
          price = GetPriceOfFurniture(rows[total_i].type);
          total += (u32)qty * price;
        }
      if (!row->quantity)
        goto redraw_catalogue;
      HandleBuyStuff_Helper(&rows[selected]);
      continue;
    }
    switch (event) {
    case PT_MENU_BUTTON_1:
      if (page < rows[count - 1u].page) {
        redraw = 1;
        ++page;
      }
      break;
    case PT_MENU_BUTTON_2:
      if (page > 1) {
        redraw = 1;
        --page;
      }
      break;
    case PT_MENU_BUTTON_3:
      if (!total) {
        DisplayErrorMessage(0, furniture_no_selection_text);
        goto cancel_catalogue;
      }
      line_count = 0;
      (void)ShowInvoice(ptr_apple->restaurant ? ptr_apple->restaurant : NULL, furniture_invoice_caption);
      SetCurrentFont(font_supersma);
      for (unsigned k = 0; k < count; ++k) {
        FurnitureCatalogueEntry *row = &rows[k];
        FurnitureDesc *d = &furniture_desc[row->type];

        d->amount_in_van = row->quantity;
        if (!row->quantity)
          continue;
        furniture_quality_in_van[row->type] = 30;
        ++line_count;
        (void)SetTempCanvasPosition(49, (i16)(6u * line_count + 77u));
        DrawTextInsideList(d->name);
        (void)SetTempCanvasPositionX(135);
        DrawNumberInsideListRight((i8)row->quantity);
        (void)SetTempCanvasPositionX(166);
        i32 qty = (i8)row->quantity;

        u16 price;
        price = GetPriceOfFurniture(row->type);
        (void)DrawDollarsInsideList(pt_signed_dword((u32)qty * price));
      }
      SetCurrentFont(some_font_ptr);
      (void)SetTempCanvasPosition(166, (i16)(6u * line_count + 87u));
      (void)DrawDollarsInsideList(pt_signed_dword(total));
      DrawText2(12, 203, 170, furniture_purchase_thanks);
      SetCurrentFont(font_supersma);
      (void)wait_input();

      u32 paid;
      paid = TransferPlayerMoney(ptr_apple->player_index, catalogue_category == 3 ? 1 : 2, 5, (float)total);
      if ((u16)paid) {
        AddCostForRestaurant(ptr_apple->restaurant, total, catalogue_category == 3 ? 4 : 5);
        u8 result = count;

        return result;
      }
      reset_type = 0;
      for (; reset_type < 164; ++reset_type)
        furniture_desc[reset_type].amount_in_van = 0;
      goto redraw_catalogue;
    case PT_MENU_BUTTON_4:
      goto cancel_catalogue;
    default:
      break;
    }
    if (redraw) {
      redraw = 0;
      goto redraw_catalogue;
    }
  }
cancel_catalogue:
  AdvanceClock(1);

  return 0;
}
// Writes thirteen bytes: the floor and its six appearance ratings.

u8 HandleSupplierStuff_Helper2(const FloorPattern *pattern, FloorPurchase *purchase) {
  const FurnitureSupplier *supplier = &furniture_suppliers[pattern->supplier_id - 1u];
  for (unsigned i = 0; i < 6; ++i)
    purchase->base_beautiness[i] = (u8)((i32)supplier->floor_beautiness[i] * pattern->beauty_scale / 2);
  purchase->floor.type = (u8)(pattern->supplier_id - 1u);
  purchase->floor.price = supplier->floor_price;
  purchase->floor.floor_bg = pattern->supplier_id;
  for (unsigned y = 0; y < 2; ++y)
    for (unsigned x = 0; x < 2; ++x)
      purchase->floor.piece[y][x] = (u8)(pattern->piece[y][x] - 1u);
  u8 result = purchase->floor.piece[1][1];

  return result;
}

static void floor_draw_buffer(void) {
  u32 length = pt_strlen(strbuf);
  (void)Canvas_DrawText(temp_canvas, pt_text_span(strbuf, (u16)length));
}
u8 HandleSupplierStuff_Helper(Rest *restaurant, u8 supplier) {
  i16 indices[80];
  FloorPurchase purchase;
  u16 match_count;
  u32 selected_index;
  u8 pattern_count;
  u8 first = 0;
  match_count = 0;
  selected_index = UINT32_MAX;
  (void)OpenFileAsCurrent(furniture_floor_patterns_filename);
  (void)ReadBytesFromCurrentFile(&pattern_count, 1);

  u32 bytes = (u32)sizeof(FloorPattern) * pattern_count;
  FloorPattern *patterns;
  patterns = AllocateBufferOrMessage(bytes);

  (void)ReadBytesFromCurrentFile(patterns, bytes);
  CloseCurrentFile();
  for (unsigned i = 0; i < pattern_count; ++i)
    if ((i32)patterns[i].supplier_id - 1 == supplier) {
      u16 count = match_count;

      indices[count] = (i16)i;
      match_count = (u16)(count + 1u);
    }
  for (unsigned i = (u8)match_count; i < 80; ++i)
    indices[i] = -1;
redraw_floor:
  FillLetterBackground(4, 9, 4);
  SetCurrentFont(font_piznumb);
  temp_canvas->color = 0;
  DrawText2(60, 190, 30, furniture_suppliers[supplier].name);
  for (unsigned row = 0; row < 3; ++row) {
    unsigned index = first + row;

    i16 record = indices[index];
    if (record == -1)
      continue;

    (void)HandleSupplierStuff_Helper2(&patterns[(unsigned)record], &purchase);
    DrawSampleFloor(&purchase.floor, 1, (i8)(5u * row + 1u));
    unsigned y = 54u * row;
    FillRect(162, 80, (u16)(y + 40u), 188, (u16)(y + 65u));
    temp_canvas->color = 0;
    (void)Canvas_SetPosition(temp_canvas, 85, (i16)((i16)(y + 50u)));
    snprintf((char *)strbuf, sizeof(strbuf), "price: $%u per tile", purchase.floor.price);
    floor_draw_buffer();
    const Poi *poi = restaurant->property;
    unsigned tiles = (unsigned)poi->size_x * poi->size_y;
    snprintf((char *)strbuf, sizeof(strbuf), "x %u = $%u", tiles, tiles * purchase.floor.price);
    (void)Canvas_SetPosition(temp_canvas, 90, (i16)((i16)(y + 60u)));
    floor_draw_buffer();
    if ((i16)selected_index != -1 && (i16)selected_index == (i32)index)
      DrawBob(&bob_restaurant, 325, 160, (i16)(y + 35u));
  }
  i16 selection = (i16)selected_index;
  if (selection != -1) {
    i16 record = indices[(unsigned)selection];

    (void)HandleSupplierStuff_Helper2(&patterns[(unsigned)record], &purchase);
  }
  for (;;) {
    u16 event = wait_input();
    i16 bottom = (i16)(((u32)match_count - first) * 54u + 29u);
    if (event == 1 && (IsMouseInside(20, 30, 190, bottom) != 0)) {
      i32 selected = GetSelectedListItem(30, 54);
      selected_index = (u32)(u8)selected + first;
      goto redraw_floor;
    }
    if (event == 2) {
      selected_index = UINT32_MAX;
      goto redraw_floor;
    }
    switch (event) {
    case PT_MENU_BUTTON_1:
      if ((i32)(i16)match_count - first > 3) {
        first = (u8)(first + 3u);
        goto redraw_floor;
      }
      break;
    case PT_MENU_BUTTON_2:
      if (first) {
        first = (u8)(first - 3u);
        goto redraw_floor;
      }
      break;
    case PT_MENU_BUTTON_3:
      if ((i16)selected_index != -1) {
        const Poi *poi = restaurant->property;
        u32 total = (u32)poi->size_x * poi->size_y * purchase.floor.price;

        u32 paid;
        paid = TransferPlayerMoney(current_player_index, 2, 5, (float)total);
        if (!(u16)paid)
          goto redraw_floor;
        AddCostForRestaurant(restaurant, total, 5);
        // Original copied 32 bytes from this 13-byte initialized
        // record, overwriting nineteen restaurant bytes with stack
        // contents. Match the separately labelled 13-byte repair.

        (void)memmove(&restaurant->floor_purchase, &purchase, sizeof(purchase));
      }
      goto close_floor;
    case PT_MENU_BUTTON_4:
      goto close_floor;
    default:
      break;
    }
  }
close_floor:
  AdvanceClock(2);
  pt_buffer_release(patterns);

  return 0;
}
u8 HandleSupplierStuff(Rest *restaurant, u8 category) {
  u8 supplier_indices[16];
  u8 row_offset, quality_band, page, first, supplier_count, row;
  supplier_count = 0;
  first = 0;
  page = 1;
  (void)memset(supplier_indices, 0, sizeof(supplier_indices));
  for (quality_band = 1; quality_band <= 6; ++quality_band) {
    for (row = 0; row < furniture_supplier_count; ++row) {
      FurnitureSupplier *supplier = &furniture_suppliers[row];
      if (supplier->category != category)
        continue;
      i32 quality;
      quality = GetSupplierQuality(supplier->quality);
      if ((u8)quality == quality_band && supplier->first_year <= ptr_apple->year &&
          supplier->last_year >= ptr_apple->year) {
        // The original has only sixteen slots. Do not certify a write
        // past that stack array if an unsupported supplier set grows.

        supplier_indices[(supplier_count)++] = row;
      }
    }
  }
  if (!supplier_count) {
    DisplayErrorMessage(0, furniture_no_supplier_text);

    return 0;
  }
redraw_suppliers:
  row = 0;
  (void)Do3x3ButtonStuff(27);
  FillLetterBackground(4, 9, 5);
  if (category >= 1 && category <= 4)
    DrawBitmapStuff1((u8[]){9, 7, 8, 10}[category - 1u], 34, 18);
  temp_canvas->color = 8;
  SetSomeFieldInCanvas(0);
  SetCurrentFont(font_astandar);
  DrawText2(60, 190, 30, furniture_suppliers_caption);
  SetCurrentFont(font_piznumb);
  (void)SetTempCanvasPosition(97, 177);
  snprintf((char *)strbuf, sizeof(strbuf), "- %u -", page);
  DrawTextInsideList(strbuf);
  while (row < 4 && (unsigned)first + row < supplier_count) {
    unsigned index = first + row;
    FurnitureSupplier *supplier = &furniture_suppliers[supplier_indices[index]];
    row_offset = (u8)(32u * row);
    i32 quality;
    // The row argument is already pushed during the original quality call.

    quality = GetSupplierQuality(supplier->quality);
    (void)DrawSupplierBitmap((u8)(quality - 1), row);
    // MOVZX AX has an eight-bit source and clears the destination word.

    i16 y = (i16)(row_offset + 39u);
    (void)DrawSupplierName(supplier->name, 54, y);
    ++row;
  }
  for (;;) {
    u16 event = wait_input();
    if (pt_is_menu_button_event(event)) {
      switch (event) {
      case PT_MENU_BUTTON_1:
        if ((unsigned)first + 4u < supplier_count) {
          first += 4;
          ++page;
          goto redraw_suppliers;
        }
        break;
      case PT_MENU_BUTTON_2:
        if (first) {
          first -= 4;
          --page;
          goto redraw_suppliers;
        }
        break;
      case PT_MENU_BUTTON_3: {
        u8 result = CheckGoalStuff();

        return result;
      }
      default:
        goto redraw_suppliers;
      }
      continue;
    }
    if (!(IsMouseInside(22, 41, 198, (i16)(row * 32u + 40u)) != 0))
      continue;
    (void)Do3x3ButtonStuff(15);

    unsigned selected = first + ((u8)(mouse_result->y - 40u) >> 5);

    if (category == 1) {
      u8 result = HandleSupplierStuff_Helper(restaurant, supplier_indices[selected]);
      if (!result)
        goto redraw_suppliers;

      return 0;
    }
    u8 result = HandleBuyStuff(supplier_indices[selected]);
    if (!result)
      goto redraw_suppliers;
    AdvanceClock(3);

    return result;
  }
}

// initialize the inventory count or row strings; neither arrow owns them.

u8 HandleFurnitureMenu(Rest *rest) {
  FurnitureInventoryRow inventory_rows[32];
  u8 inventory_count;
  u8 mode = 2, row = 0;

  did_terminal_stuff = 0;
  if (pt_float_is_zero(rest->average_stay_time))
    rest->average_stay_time = 30;
  (void)InitGraphicsMode();
  FillLetterBackground(4, 3, 60);
  FillRect(91, 60, 109, 163, 152);
  temp_canvas->color = 0;
  SetCurrentFont(font_piznumb);
  SetSomeFieldInCanvas(0);
  DrawText2(78, 152, 83, furniture_menu_caption);
  DrawText2(78, 152, 95, furniture_equipment_caption);
  temp_canvas->color = 14;
  (void)Do3x3ButtonStuff(29);
  u16 event = wait_input();
  if (pt_is_menu_button_event(event)) {
    row = (u8)(event - 240u);
    if (row == 4) {
      return 0;
    }
    if (row < 4)
      mode = (u8[]){2, 3, 4, 1}[row];
  }
redraw:
  (void)Do3x3ButtonStuff(4);
  SetupSomeSpacingStuff(32, 6);
  (void)ReComputeStaffStuff(rest);
  DrawFurnitureMainPage(rest, inventory_rows, &inventory_count, mode);

  for (;;) {
    u8 pending = CheckPeriodicPhoneCalls();
    if (pending)
      goto redraw;
    event = wait_input();
    if (event == 3849) {
      rest = ((Rest *)(SelectNextRestaurant(rest)));
      goto redraw;
    }
    if (event == 1) {
      if (mode == 1 && savegame_sect_0.extra_flooring_enabled && (IsMouseInside(48, 102, 144, 146) != 0)) {
        i32 ix = (mouse_x - 48) / 12, iy = (mouse_y - 102) / 11;
        u8 *tile = &rest->floor.piece[iy % 2][ix % 2];
        *tile = (u8)((*tile + 1u) & 7u);
        goto redraw;
      }
      if (mode == 1 || mode == 5)
        continue;
      if ((IsMouseInside(173, 127, 181, 149) != 0)) {
        while (row && (WantSkipGameIntro() != 0)) {
          --row;
          HandleFurnitureStuff1(inventory_rows[row].text, -1, 7, 11, 14, 32, 130, 140);
        }
      }
      if ((IsMouseInside(173, 151, 181, 173) != 0)) {
        while ((int)inventory_count - row > 9 && (WantSkipGameIntro() != 0)) {
          ++row;
          HandleFurnitureStuff1(inventory_rows[row + 8u].text, 7, 7, 11, 14, 32, 130, 140);
        }
      }
      continue;
    }
    switch (event) {
    case PT_MENU_BUTTON_1:
      mode = 2;
      goto redraw;
    case PT_MENU_BUTTON_2:
      mode = 3;
      goto redraw;
    case PT_MENU_BUTTON_3:
      mode = 5;
      goto redraw;
    case PT_MENU_BUTTON_4:
      mode = 4;
      goto redraw;
    case PT_MENU_BUTTON_5:
      mode = 1;
      goto redraw;
    case PT_MENU_BUTTON_6:
      for (unsigned i = 0; i < 164; ++i)
        if (furniture_desc[i].amount_in_van) {
          DisplayErrorMessage(0, furniture_van_warning);
          break;
        }
      if (mode == 5)
        goto redraw;
      u8 result = HandleSupplierStuff(rest, mode);
      if (result == 255) {
        return 255;
      }
      if (result)
        (void)RePositionFurniture(rest, 0, 255);
      UpdateCharacterPropRandomly(PT_CHARACTER_ENERGY, 0, -2);
      CheckGoalStuff();
      if (has_many_players) {
        return 255;
      }
      if (!did_terminal_stuff)
        (void)Do3x3ButtonStuff(4);
      goto redraw;
    case PT_MENU_BUTTON_7: {
      AdvanceClock(1);
      u8 result = CheckGoalStuff();

      return result;
    }
    default:
      goto redraw;
    }
  }
}
