// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
// Restaurant.

#include "objects.h"
#include "graphics.h"
#include "missions.h"
#include "restaurant.h"
#include "character.h"
#include "staff.h"
#include "calendar.h"
#include "sound.h"
#include "world.h"
#include "messages.h"
#include "input.h"
#include "menu.h"
#include "finance.h"
#include "recipe.h"
#include "trade.h"
#include <stdio.h>

// retains all 32 incoming bits, including high bits returned on a match.

bool pt_select_van_item(u32 start, u8 direction, u32 *result) {
  u8 amount;
  u32 index = start;
  if (direction == 1) {
    for (; (i16)index < 164; ++index) {
      if (!pt_furniture_van_amount((i16)index, &amount))
        return false;
      if (amount) {
        *result = index;
        return true;
      }
    }
    for (index = 0; (i16)index < (i16)start; ++index) {
      if (!pt_furniture_van_amount((i16)index, &amount))
        return false;
      if (amount) {
        *result = index;
        return true;
      }
    }
  } else if (direction == 2) {
    for (; (i16)index >= 0; --index) {
      if (!pt_furniture_van_amount((i16)index, &amount))
        return false;
      if (amount) {
        *result = index;
        return true;
      }
    }
    for (index = 163; (i16)index > (i16)start; --index) {
      if (!pt_furniture_van_amount((i16)index, &amount))
        return false;
      if (amount) {
        *result = index;
        return true;
      }
    }
  } else {
    *result = 0;
    return true;
  }
  *result = UINT32_MAX;
  return true;
}
u32 RepositionFurnitureHelper3(u32 start, u8 direction) {
  u32 result;
  bool valid = pt_select_van_item(start, direction, &result);
  if (!valid)
    abort();
  return result;
}
// ORs only the low flags byte. Callers discard the incidental AL loop counter; the other
// three bytes stay untouched.

void DrawRestaurant_SetFlag4OnAll(Rest *rest) {
  for (unsigned i = 0; i < rest->num_furniture; ++i)
    rest->furniture[i].flags_low |= 4u;
  for (unsigned i = 0; i < guest_count; ++i)
    guests[i].flags_low |= 4u;
}

// guest frames. Each guest keeps its own pool slot, including skipped slots.

static void draw_layer(Rest *r, u8 layer) {
  DrawRestaurant_Proc8(r, layer, 0);

  Canvas_DrawAllIgloos(temp_canvas, active_palette_pointer);
}
static void draw_guests(Rest *r, unsigned part) {
  DrawRestaurant_ResetIgloos();

  const i8 *dx = part == 0 ? guest_lower_offset_x : part == 1 ? guest_upper_offset_x : guest_food_offset_x;
  const i8 *dy = part == 0 ? guest_lower_offset_y : part == 1 ? guest_upper_offset_y : guest_food_offset_y;
  for (unsigned i = 0; i < guest_count; ++i) {
    Guest *g = &guests[i];
    if (part == 2 ? !g->eating_or_waiting : !(g->flags & 4u))
      continue;
    unsigned orientation = g->flags & 3u;
    pt_igloo_pool_at(i)->draw_x = (i16)((u16)g->x - 12u * r->scrollpos_x + (u32)(i32)dx[orientation]);
    pt_igloo_pool_at(i)->draw_y = (i16)(11u * r->scrollpos_y + g->y - 88u + (u32)(i32)dy[orientation]);

    SetupHazelWithBobSprite(pt_hazelnut_pool_at(i),
        0,
        part == 2 ? &bob_restaurant : &bob_restaurant_open,
        part == 2 ? 340 : (u16)(4u * g->guest_type + (part == 0 ? 96 : 48) + orientation));

    if (part == 1)
      g->flags_low &= (u8)~4u;
  }

  Canvas_DrawAllIgloos(temp_canvas, active_palette_pointer);
}
// Guest population is reconstructed separately in guest_walking.c.

void DrawRestaurant(Rest *rest, u8 redraw) {

  if (redraw && rest == &temp_rest)
    restaurant_with_guests = NULL;
  if (rest != restaurant_with_guests) {
    restaurant_with_guests = NULL;
    guest_count = 0;
  }

  bool day = IsDayTime() != 0;

  if (day) {
    if (rest->restaurant_opened && redraw && !game_ptr->byte94 && rest != restaurant_with_guests) {
      RestCreateAllGuests(rest);
    }
  } else
    restaurant_with_guests = NULL;
  // The original debug loops consume no external state and write only
  // private loop counters. No boundary or persistent mutation is omitted.

  if (redraw) {
    DrawRestaurant_SetFlag4OnAll(rest);

    FillLetterBackground(0, 10, 100);

    DrawRestaurant_Floor(rest);
  }
  draw_layer(rest, 1);
  if (rest->restaurant_opened) {
    day = IsDayTime() != 0;

    if (day) {
      draw_guests(rest, 0);
      draw_guests(rest, 1);
    }
  }
  draw_layer(rest, 2);
  if (rest->restaurant_opened) {
    day = IsDayTime() != 0;

    if (day)
      draw_guests(rest, 2);
  }
  draw_layer(rest, 3);
  if (redraw) {
    DrawRestaurant_TopWall(rest);

    DrawRestaurant_BottomWall(rest);

    DrawRestaurant_RightWall(rest);

    DrawRestaurant_LeftWall(rest);
  }

  for (unsigned i = 0; i < rest->num_furniture; ++i)
    rest->furniture[i].flags_low &= (u8)~4u;
  for (unsigned i = 0; i < guest_count; ++i)
    guests[i].flags_low &= (u8)~4u;
  did_terminal_stuff = 0;
}
// An empty van keeps all three setup writes and returns before restoration.

u32 RePositionFurniture(Rest *rest, u8 orientation, u8 item) {
  u8 saved_room = rest->is_in_kitchen, previous_room = saved_room;
  u8 saved_hierarchy = game_ptr->in_menu_hierarchy;
  bool cursor_hidden = true, reopen = false;
  i16 turn = orientation;
  restaurant_with_guests = NULL;
  game_ptr->in_menu_hierarchy = 0;
  state_slot_016 = 1;
  u32 selected = item;
  if (item == 255) {
    selected = RepositionFurnitureHelper3(0, 1);

    if ((u16)selected == UINT16_MAX) {
      return UINT32_MAX;
    }
  }
  // Nonempty placement has a distinct body; never treat it as empty.

  FurnitureDesc *descriptor = pt_furniture_descriptor((u16)selected);
  rest->is_in_kitchen = descriptor->field_0 == 3;
  BobHost *room_bob = rest->is_in_kitchen ? &bob_kitchen : &bob_restaurant;
  if (rest->restaurant_opened && !rest->is_in_kitchen) {
    reopen = true;
    rest->restaurant_opened = 0;

    bool day = IsDayTime() != 0;

    if (day) {
      ShowMessage(15, placement_closed_text);
    }

    DoCloseRestaurant(rest);
  }

  HideMouseCursor_();

  InitGraphicsMode();

restart_selection:

  SetCurrentBitmap(old_bitmap);

  Do3x3ButtonStuff(item == 255 ? 23 : 31);

  if (rest->is_in_kitchen) {
    RestDrawKitchen(rest);

  } else {
    DrawRestaurant_SetFlag4OnAll(rest);

    DrawRestaurant(rest, 1);

    DrawMapHelper4(current_bitmap);
  }

  CommitBitmapAndSetPalette(4);

  Copy320x200Bitmap(current_bitmap, old_bitmap);

  selected = RepositionFurnitureHelper3((u32)(i32)(i16)selected, 1);

  if ((u16)selected == UINT16_MAX)
    goto finish;

  SaveAndSetInputEventFilter(7);

  turn = orientation;

  DrawMapHelper4(current_bitmap);

preview:
  descriptor = pt_furniture_descriptor((u32)(i32)(i16)selected);

  if (item == 255) {
    rest->is_in_kitchen = descriptor->field_0 == 3;
    if (rest->is_in_kitchen != previous_room) {
      room_bob = rest->is_in_kitchen ? &bob_kitchen : &bob_restaurant;
      if (rest->is_in_kitchen) {
        RestDrawKitchen(rest);

      } else {
        DrawRestaurant(rest, 1);
      }

      DrawMapHelper4(current_bitmap);

      previous_room = rest->is_in_kitchen;
    }
  }

  InitIglooLink(&temp_igloo_1, &temp_igloo_2, &temp_igloo_link);

  temp_canvas->actors = &temp_igloo_link;
  if (item != 255)
    selected = item;
  descriptor = pt_furniture_descriptor((u32)(i32)(i16)selected);
  u16 image = pt_furniture_image(descriptor, turn);

  BobHost *bob = descriptor->value ? room_bob : &bob_restaurant;

  const BobHeader *header_address = SetupHazelWithBobSprite(&temp_hazel, NULL, bob, image);

  const Poi *poi = rest->property;
  const BobHeader *header = header_address;
  i16 left, right, top, bottom;
  if (rest->is_in_kitchen) {
    left = (i16)(12u * (15u - (u32)rest->kitchen_scroll_x - poi->kitchen_width) + 12u - header->field_2);
    right = (i16)(12u * rest->kitchen_scroll_x + 203u - header->w - 12u);
    top = (i16)(11u * (15u - (u32)poi->kitchen_height + rest->kitchen_scroll_y) + 14u);
    bottom = (i16)(11u * rest->kitchen_scroll_y + 189u - header->h - 12u + header->field_2);
  } else {
    left = (i16)(24u - header->field_2 - 12u * rest->scrollpos_x);
    right = (i16)(12u * ((u32)poi->size_x + 1u) + 12u - 12u * rest->scrollpos_x - header->w);
    top = (i16)(11u * (15u - (u32)poi->size_y + rest->scrollpos_y) + 14u);
    bottom = (i16)(11u * rest->scrollpos_y + 189u - header->h - 12u + header->field_2);
  }
draw_preview:

  RepositionFurnitureHelper2(old_bitmap);

  SetCurrentBitmap(old_bitmap);

  temp_igloo.draw_y = mouse_result->y;
  if (temp_igloo.draw_y > bottom)
    temp_igloo.draw_y = bottom;
  if (temp_igloo.draw_y < top)
    temp_igloo.draw_y = top;
  temp_igloo.draw_x = mouse_result->x;
  if (temp_igloo.draw_x < left)
    temp_igloo.draw_x = left;
  if (temp_igloo.draw_x > right)
    temp_igloo.draw_x = right;

  SortCanvasIgloo(temp_canvas);

  Canvas_DrawAllIgloos(temp_canvas, active_palette_pointer);

  CommitBitmapAndSetPalette(0);

read_input:

  mouse_pressed = WaitForMouseClick();

  if (!mouse_pressed) {
    if (mouse_x < 203) {
      if (!cursor_hidden) {
        cursor_hidden = true;

        HideMouseCursor_();
      }
      goto draw_preview;
    }
    if (cursor_hidden) {
      cursor_hidden = false;

      GraphicsRestoreCursor2();
    }
    goto read_input;
  }
  if (mouse_pressed == 1) {
    if (!cursor_hidden) {
      ShowMessage(14, placement_invalid_text);

      goto draw_preview;
    }
    descriptor = pt_furniture_descriptor((u32)(i32)(i16)selected);

    u8 layer = descriptor->byte25;
    u8 count = rest->is_in_kitchen ? rest->num_ovens : rest->num_furniture;
    u8 *link = pt_rest_furniture_link(rest, count, rest->is_in_kitchen != 0);

    (*link) = 255;

    u8 contact = FindPlacementContacts(rest, header_address, layer);

    bool contains = false, bad = false;
    if (contact == 1) {
      for (i32 i = 0; i < (i8)placement_contact_count; ++i) {
        const PTPlacementContact *row = pt_placement_contact_state(i);
        u8 amount = row->amount;
        if (rest->is_in_kitchen && amount < 4)
          continue;
        u8 type = row->type;
        const FurnitureDesc *other = pt_furniture_descriptor(type);

        u8 other_layer = other->byte25;
        if (layer == 3) {
          if (other_layer != 2) {
            bad = true;
            continue;
          }
          if (amount == 100) {
            contains = true;
            continue;
          }
        }
        if (layer == 2 && other_layer == 1) {
          if (amount > 10)
            contact = 2;
        } else if (layer != 1 || other_layer != 2 || other->furn_type != 1 || descriptor->furn_type != 2 ||
            placement_contact_count != 1 || amount > ((u32)header->w + header->h) / 4u)
          contact = 2;
      }
      if (bad)
        contact = 2;
      if (contains)
        contact = 1;
    }
    if (contact == 1) {
      for (i32 i = 0; i < (i8)placement_contact_count; ++i) {
        const PTPlacementContact *row = pt_placement_contact_state(i);
        Furniture *other_item = (Furniture *)row->item;
        u8 index = row->index;
        u8 type = row->type;
        u8 amount = row->amount;
        if (rest->is_in_kitchen && amount < 4)
          continue;
        const FurnitureDesc *other = pt_furniture_descriptor(type);

        u8 other_layer = other->byte25;
        if ((layer == 3 && other_layer == 2 && amount == 100) || (layer == 2 && other_layer == 1))
          other_item->linked_furniture = rest->num_furniture;
        if (layer == 1 && other_layer == 2 && other->furn_type == 1 && descriptor->furn_type == 2 &&
            placement_contact_count == 1 && !rest->is_in_kitchen) {
          (*link) = index;
          u8 other_type = other_item->type;
          u16 image = pt_furniture_image(pt_furniture_descriptor(other_type), (i16)(other_item->flag_bits & 3u));

          const BobHeader *h = pt_resource_header_at(&bob_restaurant, (u16)image);

          i16 ox = other_item->x;
          u8 oy = other_item->y;
          i16 px = (i16)((u16)temp_igloo.draw_x + 12u * rest->scrollpos_x);
          i16 py = (i16)((u16)temp_igloo.draw_y + 88u - 11u * rest->scrollpos_y);
          i16 extent = 0, delta = (i16)((u32)(i32)ox - (u32)(i32)px);
          if (delta > 0) {
            extent = delta;
            turn = 3;
          }
          delta = (i16)((u32)(i32)px + header->w - (u32)(i32)ox - h->w);
          if (delta > extent) {
            extent = delta;
            turn = 1;
          }
          delta = (i16)((u32)oy - (u32)(i32)py);
          if (delta > extent) {
            extent = delta;
            turn = 0;
          }
          delta = (i16)((u32)(i32)py + header->h - oy - h->h);
          if (delta > extent)
            turn = 2;
        }
      }
      contact = 0;
    }
    if (contact) {
      temp_canvas->actors = &temp_igloo_link;

      RepositionFurnitureHelper2(current_bitmap);

      RepositionFurnitureHelper1(&temp_hazel, temp_canvas, active_palette_pointer);

      goto preview;
    }

    RepositionFurnitureHelper2(old_bitmap);

    SetCurrentBitmap(old_bitmap);

    Canvas_DrawAllIgloos(temp_canvas, active_palette_pointer);

    CommitBitmapAndSetPalette(0);

    temp_canvas->actors = &map_igloo_link;
    bool kitchen = rest->is_in_kitchen != 0;
    const Furniture *item_address = pt_rest_furniture_identity(rest, count, kitchen);

    Furniture *placed = (Furniture *)item_address;
    placed->type = (u8)selected;
    placed->x = (i16)((u16)temp_igloo.draw_x + (kitchen ? 0u - 12u * rest->kitchen_scroll_x : 12u * rest->scrollpos_x));
    placed->y = (u8)((u16)temp_igloo.draw_y + 88u - 11u * (kitchen ? rest->kitchen_scroll_y : rest->scrollpos_y));
    placed->flags_low = (u8)((u32)placed->flags_low & 252u);
    placed->flag_bits = (u32)((u32)placed->flag_bits | (u32)((u16)turn & 3u));
    if (kitchen)
      rest->num_ovens = (u8)((u32)rest->num_ovens + 1u);
    else {
      const u8 *quality = pt_furniture_van_quality((i16)selected);

      placed->flags_low = (u8)((u32)placed->flags_low & 7u);
      placed->flag_bits = (u32)((u32)placed->flag_bits | (u32)((u32)((*quality) & 31u) << 3));
      rest->num_furniture = (u8)((u32)rest->num_furniture + 1u);
    }

    AdvanceClock(1);

    --descriptor->amount_in_van;
    if (item == 255)
      goto restart_selection;
    goto finish;
  }
  if (mouse_pressed == 2 || mouse_pressed == PT_MENU_BUTTON_1 || mouse_pressed == PT_MENU_BUTTON_2 ||
      mouse_pressed == PT_MENU_BUTTON_3 || mouse_pressed == 0x4800 || mouse_pressed == 0x4b00 ||
      mouse_pressed == 0x4d00 || mouse_pressed == 0x5000) {
    bool scroll = mouse_pressed >= 0x4800;
    if (scroll) {
      RepositionFurnitureHelper2(current_bitmap);
    }

    RepositionFurnitureHelper1(&temp_hazel, temp_canvas, active_palette_pointer);

    if (mouse_pressed == 2) {
      turn = (i16)((u16)turn + 1u);
      if (turn > 3)
        turn = 0;
      goto preview;
    }
    if (scroll) {
      u16 key = mouse_pressed;
      u8 direction = key == 0x4d00 ? 0 : key == 0x4b00 ? 1 : key == 0x4800 ? 2 : 3;
      for (;;) {
        u16 held = (u16)ReadKeyboardPort();

        bool continuing = held == (u16)(key >> 8);
        if (!continuing) {
          continuing = WantSkipGameIntro() != 0;
        }
        if (!continuing)
          break;

        i32 bound = RestScroll(rest, direction);

        if (bound) {
          Delay(10);

          break;
        }
      }

      DrawMapHelper4(current_bitmap);

      goto preview;
    }
    if (item == 255 && mouse_pressed != PT_MENU_BUTTON_2) {
      u16 start = (u16)selected + (mouse_pressed == PT_MENU_BUTTON_1 ? 1u : UINT16_MAX);

      selected = RepositionFurnitureHelper3((u32)(i32)(i16)start, mouse_pressed == PT_MENU_BUTTON_1 ? 1 : 2);

      goto preview;
    }
    if (item != 255 && mouse_pressed == PT_MENU_BUTTON_2)
      goto finish;
    u16 sale_price = 0;
    if (mouse_pressed == PT_MENU_BUTTON_3) {
      u16 price = GetPriceOfFurniture((u16)selected);

      snprintf((char *)strbuf, sizeof(strbuf), (const char *)placement_sell_format, (int)(price >> 1));

      sale_price = (u16)(price >> 1);

      u8 accepted = ShowMessage2(0, strbuf);

      if (!accepted)
        goto preview;
    }
    if (item == 255) {
      AdvanceClock(1);
    }
    descriptor = pt_furniture_descriptor((u32)(i32)(i16)selected);

    --descriptor->amount_in_van;

    u16 price = GetPriceOfFurniture((u16)selected);

    u8 category = mouse_pressed == PT_MENU_BUTTON_1 ? 2u : (u8)(descriptor->furn_type != 8u) + 1u;

    TransferPlayerMoney(current_player_index, 10, category, price);

    if (mouse_pressed == PT_MENU_BUTTON_3) {
      TransferPlayerMoney(current_player_index, 5, 10, sale_price);
    }
    if (item == 255)
      goto restart_selection;
    goto finish;
  }
  goto draw_preview;
finish:

  SetCurrentBitmap(old_bitmap);

  if (rest->is_in_kitchen) {
    RestDrawKitchen(rest);

  } else {
    DrawRestaurant_SetFlag4OnAll(rest);

    DrawRestaurant(rest, 1);

    DrawMapHelper4(current_bitmap);
  }

  CommitBitmapAndSetPalette(4);

  Copy320x200Bitmap(current_bitmap, old_bitmap);

  SaveAndSetInputEventFilter(3);

  temp_canvas->actors = &map_igloo_link;

  GraphicsRestoreCursor2();

  rest->is_in_kitchen = saved_room;
  state_slot_016 = 0;

  Do3x3ButtonStuff(1);

  game_ptr->in_menu_hierarchy = saved_hierarchy;

  u8 result = ReComputeStaffStuff(rest);

  if (reopen && !rest->restaurant_closed_for_days)
    rest->restaurant_opened = 1;

  return result;
}
// All callers ignore incidental AL. The jukebox chooser restores its own
// saved choice after this call; no synthetic return value is introduced.

void RestLoadMusic(Rest *rest, u8 transition) {
  u8 demand = rest->demand_capacity_ratio[0];
  bool stop = !rest->restaurant_opened;
  if (!stop) {
    stop = !IsDayTime();
  }
  if (!stop) {
    u16 loaded = 0;
    if (((u16)prefs.field_0 >> 8) && state_slot_007 != rest->jukebox_selection) {
      loaded = (u16)LoadMusic(rest->jukebox_selection);
    }
    if (!loaded) {
      return;
    }
    if (rest->jukebox_selection || demand > 20) {
      u8 sample = rest->jukebox_selection ? 0 : (demand > 50) + (demand > 80);

      SoundStuff2(sample);

      if (rest->jukebox_selection)
        competition_sound_pending = 1;
      if (transition) {
        SoundStuff3(0, 15);
      }

      return;
    }
  }

  SoundStuff1();

  competition_sound_pending = 0;
  state_slot_007 = 100;
}
// Carry the current room selection to the next restaurant.

Rest *SelectNextRestaurant(Rest *restaurant) {

  Player *player = player_ptr;
  if (player->num_restaurants <= 1)
    return restaurant;

  PTForwardLink *next = LinkList_getNext(((const PTForwardLink *)(restaurant)));

  Rest *r = ((Rest *)(next));
  ptr_apple->restaurant = r;
  curr_rest = r;
  r->is_in_kitchen = restaurant->is_in_kitchen;
  player->selected_property = r->property;
  restaurant_with_guests = NULL;
  guest_count = 0;
  did_terminal_stuff = 1;
  state_slot_017 = 1;

  PrintGenericInfoBox();

  return curr_rest;
}
// Retain this operation's state while waiting for input.

u8 EnterRestaurant(Rest *restaurant, u8 competitor) {
  u8 menu_result;
  const Furniture *selected = NULL;
  menu_result = 0;
  curr_rest = restaurant;
  state_slot_007 = 100;
  state_slot_015 = competitor;
  bool kitchen = restaurant->is_in_kitchen != 0;
  BobHost *bob = kitchen ? &bob_kitchen : &bob_restaurant;
  (void)LoadBobFile(bob);
  ptr_apple->is_in_kitchen = kitchen ? 1 : 0;
placement:
  restaurant = curr_rest;

  did_terminal_stuff = 0;

  if (competitor)
    restaurant->is_in_kitchen = 0;
  else {
    RePositionFurniture(restaurant, 0, 255);
  }
  player_ptr->selected_property = restaurant->property;
graphics:
  did_terminal_stuff = 0;

  InitGraphicsMode();

  if (has_many_players) {
    return 255;
  }

  RestLoadMusic(restaurant, 0);

display:

  kitchen = restaurant->is_in_kitchen != 0;
  bob = kitchen ? &bob_kitchen : &bob_restaurant;
  selected = 0;
  state_slot_008 = 0;
  game_ptr->in_menu_hierarchy = 2;
  (void)LoadBobFile(bob);
  ptr_apple->is_in_kitchen = kitchen ? 1 : 0;
  if (!kitchen && restaurant->restaurant_opened) {
    bool day = IsDayTime() != 0;

    if (day)
      (void)LoadBobFile(&bob_restaurant_open);
  }

  InitGraphicsMode();

  game_ptr->did_set_bitmap = 1;

  PrintGenericInfoBox();

  Do3x3ButtonStuff((u8)(18u * competitor + 1u));

  Copy320x200Bitmap(current_bitmap, old_bitmap);

  SetCurrentBitmap(old_bitmap);

  if (restaurant->is_in_kitchen) {
    RestDrawKitchen(restaurant);

  } else {
    DrawRestaurant(restaurant, 1);
  }

  DrawMapHelper4(current_bitmap);

  CommitBitmapAndSetPalette(4);

  Copy320x200Bitmap(current_bitmap, old_bitmap);

wait:
  if (!competitor)
    restaurant = ptr_apple->restaurant;
  curr_rest = restaurant;

  u8 pending = CheckGoalStuff();

  if (pending)
    goto graphics;

  mouse_pressed = WaitForMouseClick();

  if (!competitor)
    restaurant = ptr_apple->restaurant;
  curr_rest = restaurant;

  if (did_terminal_stuff)
    goto placement;
  if (mouse_pressed == 10 && !restaurant->is_in_kitchen && restaurant->restaurant_opened) {
    bool day = IsDayTime() != 0;

    if (day) {
      SetCurrentBitmap(old_bitmap);

      GuestHelper1();

      RestDrawOnScreen(restaurant);

      CommitBitmapAndSetPalette(0);
    }
  }
  if (mouse_pressed == 3849 && !competitor) {
    SelectNextRestaurant(restaurant);

    goto placement;
  }
  if (mouse_pressed == 14624 && !competitor) {
    restaurant->is_in_kitchen = restaurant->is_in_kitchen ? 0 : 1;
    goto display;
  }
  if (pt_is_menu_button_event(mouse_pressed)) {
    game_ptr->in_menu_hierarchy = 0;

    InitGraphicsMode();

    mouse_pressed -= PT_MENU_BUTTON_1;
    if (competitor) {
      Rest *target = restaurant == &temp_rest ? (Rest *)restaurant->linked.next : restaurant;
      if (!mouse_pressed) {
        HandleCompetitionChallenge(target);

        goto graphics;
      }
      if (mouse_pressed == 1) {
        HandleUserAttackRestaurant(target);

      } else if (mouse_pressed == 2) {
        return 1;
      }
      goto check_toolbar_goal;
    }
    ptr_apple->is_in_kitchen = (u8)(mouse_pressed + 1);

    PrintGenericInfoBox();

    if ((u16)prefs.field_0 >> 8) {
      if (state_slot_007 > 6) {
        SoundStuff1();

      } else {
        SoundStuff3(60, 15);
      }
    }
    switch (mouse_pressed) {
    case 0:
      if (restaurant->restaurant_opened) {
        u8 choice = ShowMessage2(0, restaurant_close_confirm_text);

        if (choice == 1) {
          DoCloseRestaurant(curr_rest);

          SoundStuff1();

          competition_sound_pending = 0;
        }
        goto toolbar_done;
      }
      if (!restaurant->restaurant_opened && !game_ptr->debug_mode) {
        u8 *error = !restaurant->num_ovens           ? restaurant_need_ovens_text
            : !restaurant->num_chefs                 ? restaurant_need_chefs_text
            : !restaurant->num_waiters               ? restaurant_need_waiters_text
            : !restaurant->num_recipes               ? restaurant_need_recipes_text
            : restaurant->restaurant_closed_for_days ? restaurant_closed_today_text
                                                     : 0;
        if (error) {
          DisplayErrorMessage(0, error);

          goto display;
        }
      }

      u8 choice = ShowMessage2(0, restaurant_open_confirm_text);

      if (choice != 1)
        goto toolbar_done;

      DoOpenRestaurant(curr_rest);

      restaurant_with_guests = NULL;
      Player *player = player_ptr;
      if (player->num_restaurants_open < 3) {
        u8 goal = (u8)(player->num_restaurants_open + 29u);

        MarkGoalReached(goal);

        u8 cities = 0;
        for (unsigned i = 0; i < 10; ++i)
          if (player->num_restaurants_in_city[i])
            ++cities;
        if (cities == 2) {
          MarkGoalReached(50);
        }
      }
      goto placement;
    case 1: {
      u8 stock_result = HandleStockMenu(restaurant);

      menu_result = stock_result;
      goto toolbar_done;
    }
    case 2: {
      HandlePizzaMenu(restaurant);

      goto toolbar_done;
    }
    case 3: {
      LoadBlitmapAndOffsets(1);

      FillCanvasAndSetButtons(4);

      menu_result = HandleFurnitureMenu(restaurant);

      goto toolbar_done;
    }
    case 4: {
      LoadBlitmapAndOffsets(3);

      PTActionResult staff_result = HandleStaffMenu(restaurant);

      if (staff_result.kind == PT_ACTION_VALUE && (u8)staff_result.value == 255)
        goto graphics;

      AdvanceClock(1);

      goto toolbar_done;
    }
    case 5: {
      LoadBlitmapAndOffsets(3);

      u8 recipe_result = HandleRecipeListMenu(restaurant);

      if (recipe_result == 255)
        goto graphics;
      (void)LoadBobFile(&bob_restaurant);

      Do3x3ButtonStuff(1);

      goto toolbar_done;
    }
    case 6: {
      HandleRestaurantStatsMenu(restaurant);

      goto toolbar_done;
    }
    case 7: {
      u8 terminal_result = HandleTerminalMenu();

      if (terminal_result == 255)
        goto graphics;
      did_terminal_stuff = 1;
      goto toolbar_done;
    }
    case 8: {
      HandleAnimatedStatsMenu(restaurant);

      goto toolbar_done;
    }
    }
  toolbar_done:
    if (has_many_players)
      goto graphics;
    if (menu_result == 255) {
      return 255;
    }

    AdvanceClock(1);

    if ((u16)prefs.field_0 >> 8) {
      SoundStuff3(15, 60);
    }
    if (did_terminal_stuff)
      goto placement;
    if (state_slot_017) {
      state_slot_017 = 0;
      goto placement;
    }
  check_toolbar_goal:;

    u8 pending = CheckGoalStuff();

    if (pending)
      goto graphics;
    goto display;
  }
  if (mouse_pressed == 1 && !competitor) {
    if (mouse_x < 24 && !restaurant->scrollpos_x && !restaurant->is_in_kitchen) {
      restaurant->is_in_kitchen = 1;
      goto display;
    }
    if (mouse_x > 191 && !restaurant->kitchen_scroll_x && restaurant->is_in_kitchen == 1) {
      restaurant->is_in_kitchen = 0;
      goto display;
    }

    u16 object = FindRestaurantObject(restaurant);

    if ((object >> 8) == 2 && !restaurant->is_in_kitchen) {
      HandleClickOnGuest(restaurant, &guests[(u8)object]);
    }
    if ((object >> 8) == 1) {
      bool oven = restaurant->is_in_kitchen != 0;
      u8 index = (u8)object;
      Furniture *item = (Furniture *)pt_rest_furniture_identity(restaurant, index, oven);
      if (item == selected) {
        const Furniture *f = item;
        bool can_move = true;
        if (!oven && restaurant->restaurant_opened) {
          can_move = IsDayTime() == 0;
        }
        if (!can_move) {
          FurnitureDesc *descriptor = pt_furniture_descriptor(f->type);

          const FurnitureDesc *d = descriptor;
          if (d->furn_type == 4) {
            Rest *target = curr_rest;
            u8 selection = f->type == 138 ? ChooseFromJukebox(target) : ChooseFromTV(target);

            target = curr_rest;
            if (f->type == 138)
              target->jukebox_selection = selection;
            else
              target->tv_selection = selection;
            goto wait;
          }

          ShowMessage(14, restaurant_rearrange_text);

          goto wait;
        }
        u8 count = oven ? restaurant->num_ovens : restaurant->num_furniture;

        // The source span includes the following physical record.
        // It belongs to this restaurant, including its unknown bytes.

        for (unsigned i = 0; i < count; ++i) {
          u8 *link = pt_rest_furniture_link(curr_rest, (u8)i, oven);
          if ((*link) == index)
            (*link) = 255;
          else if (!oven && (*link) != 255 && (*link) > index)
            --(*link);
        }
        ++pt_furniture_descriptor(f->type)->amount_in_van;
        u8 type = f->type;
        u32 flags = (u32)f->flags;
        furniture_quality_in_van[type] = (u8)((flags >> 3) & 31u);
        u8 orientation = (u8)(flags & 3u);
        if (oven && type == 138)
          restaurant->jukebox_selection = 0;

        memmove(item, (const u8 *)item + sizeof(Furniture), sizeof(Furniture) * (count - index));

        if (oven)
          restaurant->num_ovens = (u8)((u32)restaurant->num_ovens - 1u);
        else
          restaurant->num_furniture = (u8)((u32)restaurant->num_furniture - 1u);
        RePositionFurniture(restaurant, orientation, type);

        selected = 0;
        goto wait;
      }
      selected = item;

      const Furniture *f = item;
      if (oven)
        PrintOvenStatus(restaurant, f);
      else
        ShowFurnitureInfo(restaurant, f);

    } else {
      selected = 0;

      ShowMessage(15, restaurant_nothing_text);
    }
  }
  if (mouse_pressed == 2) {
    InitGraphicsMode();

    Do3x3ButtonStuff(0);

    pending = CheckGoalStuff();

    return pending == 255 ? 255 : 1;
  }
  if (mouse_pressed == 0x4800 || mouse_pressed == 0x4b00 || mouse_pressed == 0x4d00 || mouse_pressed == 0x5000) {
    u16 port = (u16)ReadKeyboardPort();

    u16 key = mouse_pressed;
    bool held = port == (key >> 8);
    if (!held) {
      held = WantSkipGameIntro() != 0;
    }
    while (held) {
      u8 direction = key == 0x4d00 ? 0 : key == 0x4b00 ? 1 : key == 0x4800 ? 2 : 3;

      i32 bound = RestScroll(restaurant, direction);

      if (bound) {
        Delay(10);

        break;
      }

      port = (u16)ReadKeyboardPort();

      held = port == (key >> 8);
      if (!held) {
        held = WantSkipGameIntro() != 0;
      }
    }
  }
  goto wait;
}

// AI restaurant records contain only the 163-byte simulation prefix. Their furniture template
// is loaded into the global display restaurant.

u8 EnterCompetitorRestaurant(Rest *restaurant) {
  u8 entered_result, furniture_limit;
  entered_result = 1;
  furniture_limit = restaurant->some_furniture_counter;

  if (restaurant->player_index >= game_ptr->num_interactive_players) {
    curr_rest = &temp_rest;

    LoadBobFile(&bob_restaurant);

    memcpy(&temp_rest, restaurant, PT_AI_RESTAURANT_SIZE);

    temp_rest.linked.next = (PTForwardLink *)(restaurant);
    temp_rest.property = restaurant->property;
    temp_rest.is_in_kitchen = 0;
    temp_rest.gap390[4] = 0;
    temp_rest.gap390[3] = 0;
    temp_rest.scrollpos_y = 0;
    temp_rest.scrollpos_x = 0;
    if (restaurant->template_rest) {
      (void)snprintf((char *)strbuf, sizeof(strbuf), "f\\f%d.pc", restaurant->template_rest);

      LoadRestaurantFromFile(&temp_rest, strbuf);
    }
    if (furniture_limit < temp_rest.num_furniture)
      temp_rest.num_furniture = furniture_limit;

    temp_rest.demand_capacity_ratio[0] = (u8)RandomNumber(100);

  } else
    curr_rest = restaurant;

  u32 day = IsDayTime();

  if ((u16)day) {
    entered_result = EnterRestaurant(curr_rest, 1);

  } else {
    HandleUserAttackRestaurant(curr_rest);
  }
  temp_rest.linked.next = NULL;
  u8 result = entered_result;

  return result;
}

// Restaurant lifecycle.

#include "arithmetic.h"
#include "frame.h"
#include <math.h>

#pragma STDC FP_CONTRACT OFF
static double restaurant_lifecycle_divide53(double a, double b) {
  volatile double result = a / b;
  return result;
}
static double restaurant_lifecycle_multiply53(double a, double b) {
  volatile double result = a * b;
  return result;
}
static u32 restaurant_lifecycle_integer32(double value) {
  value = TruncateX87Integral(value);
  return isfinite(value) && value >= -2147483648.0 && value < 2147483648.0 ? (u32)(i32)value : UINT32_C(0x80000000);
}

u32 PlayerSumRamtownFields(u8 player) {
  u32 total = 0;
  for (unsigned i = 0; i < 4; ++i)
    total += pt_campaign_object(player, (i8)i)->reach;
  return total;
}
u32 ComputeRecipeCost(const Recipe *recipe, const Rest *rest) {
  u32 total = 0;
  for (unsigned set = 0; set < 4; ++set)
    for (unsigned i = 0; i < 24; ++i) {
      unsigned ingredient = 24u * set + i;
      u32 price = (u16)rest->stock[ingredient].value;
      if (!price)
        price = pt_town_stock_price(set, i);
      u32 amount = pt_recipe_weight(recipe, ingredient);
      total += price * amount / 1000u;
    }
  return total;
}
u8 RestComputeBeautiness(Rest *rest, u8 age) {
  if (age >= sizeof(rest->base_beautiness))
    pt_resource_bounds_failure("customer age group", age, sizeof(rest->base_beautiness));
  u16 value = (u16)(i16)(i8)rest->base_beautiness[age];
  unsigned count = rest->num_furniture;
  for (unsigned i = 0; i < count; ++i) {
    const Furniture *item = pt_rest_furniture_identity(rest, (u8)i, false);
    const FurnitureDesc *descriptor = pt_furniture_descriptor(item->type);
    value = (u16)(value + (u16)(i16)(i8)(pt_furniture_beauty(descriptor, age)));
  }
  u8 selections[2] = {rest->jukebox_selection, rest->tv_selection};
  for (unsigned i = 0; i < 2; ++i)
    if (selections[i])
      value = (u16)(value + ((unsigned)selections[i] - 1u == age ? 12u : (u16)-4));
  i32 signed_value = (i16)value;
  i32 half = signed_value >= 0 ? signed_value / 2 : -(((-signed_value) + 1) / 2);
  return (u8)(half < 0 ? 0 : half > 150 ? 150 : half);
}
void AddRestaurantRangeStuff(Rest *rest, u8 value, u8 duration) {
  for (unsigned i = 0; i < 4; ++i)
    if (!rest->range_stuff_1[i]) {
      rest->range_stuff_1[i] = value;
      rest->range_stuff_2[i] = duration;
      break;
    }
}
void RestSetReputationEntry(Rest *rest, u8 field, u8 value, u8 show_stats) {
  if (field >= sizeof(rest->reputation))
    pt_resource_bounds_failure("restaurant reputation category", field, sizeof(rest->reputation));
  rest->reputation[field] = value;
  if (show_stats) {
    SetAnimatedReputationAndQuality(rest);
    ShowAnimatedReputation(rest, (i8)((u8)rest->sum_of_reputations));
  }
}
void RestUpdateReputations(Rest *rest) {
  const Poi *property = rest->property;
  u8 x = property->xcoord, y = property->ycoord;
  Zone *zone = FindZoneFromCoordinates(x, y);
  u8 extra = (u8)(rest->num_recipes - rest->num_standard_recipes);
  rest->sum_of_reputations = 0;
  if (rest->restaurant_opened) {
    u8 wait = rest->waiting_time;
    if (wait)
      RestSetReputationEntry(rest, 0, (u8)(((18 - (i32)wait) * zone->byte3C) / 50), 0);
    u8 standard = rest->num_standard_recipes;
    u8 penalty = standard < 3 ? (u8)(-20 * (3 - (i32)standard)) : 0;
    unsigned expected = zone->byte3F >> 4;
    if (extra > expected)
      penalty = (u8)(penalty + extra - expected);
    RestSetReputationEntry(rest, 2, (u8)(2 * ((i32)standard - zone->byte3E / 10) + (i8)penalty), 0);
    u32 total = PlayerSumRamtownFields(rest->player_index);
    RestSetReputationEntry(rest, 3, (u8)((u16)total >> 1), 0);
    u8 stars = rest->num_stars;
    RestSetReputationEntry(rest, 4, (u8)(stars * stars + (stars ? 4u : 0u)), 0);
    RestSetReputationEntry(rest, 5, (u8)(rest->byte38F >> 2), 0);
    i32 delivery = 100 - (i32)rest->delivcapa_per_day[6];
    i32 shifted = delivery >= 0 ? delivery / 8 : -(((-delivery) + 7) / 8);
    RestSetReputationEntry(rest, 6, (u8)-shifted, 0);
    for (unsigned i = 0; i < 4; ++i) {
      u8 value = rest->range_stuff_1[i];
      if (value)
        RestSetReputationEntry(rest, 7, value, 0);
    }
    rest->sum_of_reputations = 0;
    for (unsigned i = 0; i < 8; ++i) {
      u16 sum = (u16)((u16)rest->sum_of_reputations + (u16)(i16)(i8)rest->reputation[i]);
      rest->sum_of_reputations = (i16)sum;
    }
  }
}
void RestComputeCostsPerPizza(Rest *rest) {
  unsigned count = rest->num_recipes;
  for (unsigned i = 0; i < count; ++i) {
    RestoRecipe *entry = pt_menu_recipe_mutable(rest, i);
    const Recipe *recipe = entry->recipe;
    u32 value;
    if (rest->player_index < pt_interactive_player_count())
      value = ComputeRecipeCost(recipe, rest);
    else {
      value = restaurant_lifecycle_integer32(restaurant_lifecycle_multiply53(
          recipe->word3C, restaurant_lifecycle_divide53(rest->average_purchase_quality, recipe_cost_divisor)));
    }
    entry->costs_per_pizza = (i16)((u16)value);
  }
}
void RestHelper5(Rest *rest) {
  RestUpdateReputations(rest);
  u32 customer_total = SumListOfWords(rest->customer_count, 6);
  rest->guests_per_day[6] = (i16)((u16)customer_total);
  RestComputeCostsPerPizza(rest);
  if (!rest->byte388)
    for (unsigned i = 0; i < 6; ++i)
      rest->furniture_rating[i] = RestComputeBeautiness(rest, (u8)i);
  if (rest->byte388 < 2)
    ReComputeStaffStuff(rest);
}
void DoCloseRestaurant(Rest *rest) {
  rest->restaurant_opened = 0;
  Player *player = player_ptr;
  player->num_restaurants_open = (u8)((u32)player->num_restaurants_open - (u32)1u);
  AddRestaurantRangeStuff(rest, 240, 16);
}
void DoOpenRestaurant(Rest *rest) {
  rest->restaurant_opened = 1;
  rest->sum_of_reputations = 0;
  if (rest->player_index < pt_interactive_player_count()) {
    player_ptr->num_restaurants_open = (u8)((u32)(player_ptr->num_restaurants_open) + (u32)1u);
    RestHelper5(rest);
    ReComputeStaffStuff(rest);
    ReComputeAverageTimes(rest);
    ReComputeStaffStuff(rest);
  }
}

// Restaurant floor.

#include "files.h"
#include "text.h"

enum WallEdge { WALL_TOP, WALL_RIGHT, WALL_LEFT };

void pt_floor_filename(u8 index, u8 name[13]) {
  unsigned pos = 5;
  memcpy(name, "gfx\\k", pos);
  if (index >= 100)
    name[pos++] = (u8)('0' + index / 100u);
  if (index >= 10)
    name[pos++] = (u8)('0' + index / 10u % 10u);
  name[pos++] = (u8)('0' + index % 10u);
  memcpy(name + pos, ".vga", 5);
}
// Formatting leaves the shared filename buffer's unused tail intact.

void LoadFloorBitmaps(u8 index) {
  if (game_ptr->last_loaded_rest_bg == index)
    return;

  if (index) {
    u8 name[13];
    pt_floor_filename(index, name);

    memcpy(strbuf, name, strlen((const char *)name) + 1);

    LoadBackground(strbuf, bmp24_ptr);

  } else {
    LoadBackground(floor_zero_filename, bmp25_ptr);
  }
  game_ptr->last_loaded_rest_bg = index;
}

static u32 draw_cell(bool wall, u8 tile, u8 variant, i8 x, i8 y) {
  if ((u8)x & 240u)
    return (u32)(i32)x;
  if ((u8)y & 240u)
    return (u32)(i32)y;

  u32 result = BitBlt(wall ? bmp25_ptr : bmp24_ptr,
      12u * tile,
      wall ? 0 : 11u * variant,
      current_bitmap,
      12u * (unsigned)x + 12u,
      11u * (unsigned)y + 14u,
      12,
      11);

  return result;
}
u32 DrawFloorPiece(u8 tile, u8 variant, i8 x, i8 y) {
  return draw_cell(false, tile, variant, x, y);
}
u32 DrawRestaurant_DrawWallCell(u8 tile, i8 x, i8 y) {
  return draw_cell(true, tile, 0, x, y);
}

u8 pt_floor_row_tile(const Rest *r, unsigned base, i8 pattern) {
  // Signed IDIV remainder can be -1 after pattern wraps through 128.
  // That is the floor_bg byte, a physical read inside the Rest owner.

  return pt_floor_pattern_piece(&r->floor, (int)base + pattern % 2);
}

u32 DrawRestaurant_SingleRow(Rest *r, i8 row) {
  const Poi *p = r->property;
  i8 y = (i8)(r->scrollpos_y + 15u + (u32)(i32)row - p->size_y);
  if ((u8)y & 240u) {
    return (u32)(i32)y;
  }
  unsigned base = row % 2 ? 2 : 0;
  i8 x = (i8)(1u - r->scrollpos_x), end = (i8)(p->size_x - r->scrollpos_x), pattern = 0;

  while (x <= (int)end - 2) {
    DrawFloorPiece(pt_floor_row_tile(r, base, pattern), row < 2, x, y);

    x = (i8)((u8)x + 1u);
    pattern = (i8)((u8)pattern + 1u);
  }

  DrawFloorPiece(pt_floor_row_tile(r, base, pattern), 1, (i8)((u8)end - 1u), y);

  u32 result = DrawFloorPiece(pt_floor_row_tile(r, base, (i8)((u8)pattern + 1u)), 1, end, y);

  return result;
}
static void draw_floor_column(const Rest *r, i8 col) {
  i8 x = (i8)((u8)col + 1u - r->scrollpos_x);
  if ((u8)x & 240u)
    return;

  const Poi *p = r->property;
  u8 variant = col >= (int)p->size_x - 2;
  unsigned parity = col % 2 != 0;
  i8 y = (i8)(r->scrollpos_y + 15u - p->size_y), end = (i8)(r->scrollpos_y + 15u);
  for (unsigned i = 0; i < 2; ++i) {
    u8 tile = pt_floor_pattern_piece(&r->floor, (int)(parity + 2u * i));
    i8 at = (i8)((u8)y + i);

    DrawFloorPiece(tile, 1, x, at);
  }
  y = (i8)((u8)y + 2u);
  i8 pattern = 0;
  for (; y < end; y = (i8)((u8)y + 1u), pattern = (i8)((u8)pattern + 1u)) {
    // Signed remainder after byte wrap can select the preceding bytes.

    u8 tile = pt_floor_pattern_piece(&r->floor, (int)parity + 2 * (pattern % 2));

    DrawFloorPiece(tile, variant, x, y);
  }
}
// Has 20 local bytes and three saved registers; no caller consumes its incidental last-blit
// register return.

void DrawRestaurant_SingleCol(Rest *r, i8 col) {
  draw_floor_column(r, col);
}

Poi *DrawRestaurant_Floor(Rest *r) {
  LoadFloorBitmaps(r->floor.floor_bg);

  for (unsigned i = 0; i < r->property->size_y; ++i) {
    DrawRestaurant_SingleRow(r, (i8)i);
  }
  Poi *result = r->property;

  return result;
}
static void draw_wall_outline(enum WallEdge edge, const Rest *r) {
  const Poi *p = r->property;
  if (edge == WALL_TOP) {
    i8 y = (i8)(r->scrollpos_y + 14u - p->size_y);
    if (y >= 0)
      for (unsigned x = 0; x < 16; ++x)
        if (x < p->size_x + 2u) {
          (void)DrawRestaurant_DrawWallCell(1, (i8)x, y);
        }
  } else if (edge == WALL_RIGHT) {
    i8 x = (i8)(p->size_x + 1u - r->scrollpos_x);
    if (x <= 15)
      for (i8 y = (i8)(14u - p->size_y); y < 16; y = (i8)((u8)y + 1u)) {
        (void)DrawRestaurant_DrawWallCell(0, x, y);
      }
  } else if (!r->scrollpos_x) {
    u8 size = p->size_y > p->kitchen_height ? p->size_y : p->kitchen_height;
    for (i8 y = (i8)(14u - size); y < 16; y = (i8)((u8)y + 1u)) {
      (void)DrawRestaurant_DrawWallCell(0, 0, y);
    }
    i8 y = (i8)(r->scrollpos_y + p->left_door_y_end + 15u - p->size_y);
    for (unsigned i = 0; i < 6; ++i, y = (i8)((u8)y + 1u)) {
      (void)DrawRestaurant_DrawWallCell(i < 3 ? 2 : 7, 0, y);
    }
  }
}

// The original callers discard the incidental register returns from these
// wall outlines. Each boundary retains its own saved-register frame.

void DrawRestaurant_TopWall(Rest *r) {
  draw_wall_outline(WALL_TOP, r);
}
void DrawRestaurant_RightWall(Rest *r) {
  draw_wall_outline(WALL_RIGHT, r);
}
void DrawRestaurant_LeftWall(Rest *r) {
  draw_wall_outline(WALL_LEFT, r);
}

// The three-cell doorway uses low byte x coordinates; the overlaid open/closed door uses low
// words.

void DrawRestaurant_BottomWall(Rest *r) {
  if (!r->scrollpos_y) {
    const Poi *p = r->property;
    for (unsigned i = 0; i < 16; ++i)
      if (i < p->size_x + 2u)
        (void)DrawRestaurant_DrawWallCell(1, (i8)i, 15);
    u16 x = (u16)(p->door_x_start - r->scrollpos_x);
    for (unsigned i = 0; i < 3; ++i, ++x)
      (void)DrawRestaurant_DrawWallCell(3, (i8)x, 15);
  } else if (r->scrollpos_y > 2) {
    return;
  }
  if (!disable_draw_door) {
    const Poi *p = r->property;
    i16 x = (i16)(12u * (p->door_x_start - r->scrollpos_x) + 11u);

    bool day = IsDayTime() != 0;

    DrawBob(&bob_restaurant, day && r->restaurant_opened ? 60 : 59, x, (i16)(11u * (r->scrollpos_y - 1u) + 189u));
  }
}

void RestDrawKitchen_HorizWall(i8 y, i8 x, i8 end) {
  for (; x <= end; x = (i8)((u8)x + 1u))
    (void)DrawRestaurant_DrawWallCell(1, x, y);
}

static void draw_kitchen_background(const Rest *r) {
  u8 scroll_x = r->kitchen_scroll_x, scroll_y = r->kitchen_scroll_y;
  const Poi *p = r->property;
  for (unsigned x = 0; x < 16; ++x) {
    (void)DrawRestaurant_DrawWallCell(2, (i8)x, 0);
  }
  for (unsigned y = 1; y < 16; ++y) {
    BitBlt(current_bitmap, 12, 14, current_bitmap, 12, 11u * y + 14u, 192, 11);
  }
  i8 rows[2] = {15, (i8)(scroll_y + 14u - p->kitchen_height)};
  for (unsigned i = scroll_y ? 1 : 0; i < 2; ++i) {
    RestDrawKitchen_HorizWall(rows[i], 0, 15);
  }
  i8 x = (i8)(scroll_x + 14u - p->kitchen_width);
  for (unsigned y = 0; y < 16; ++y) {
    (void)DrawRestaurant_DrawWallCell(0, x, (i8)y);
  }
  if (p->kitchen_height < 14) {
    FillRect(0, 12, 14, (u16)203, (u16)(11u * (14u - p->kitchen_height) + 14u));
  }
  if (p->kitchen_width < 14) {
    FillRect(0, 12, 14, (u16)(12u * (14u - p->kitchen_width) + 12u), (u16)189);
  }
  if (!scroll_x) {
    u8 size = p->size_y > p->kitchen_height ? p->size_y : p->kitchen_height;
    for (i8 y = (i8)(14u - size); y < 16; y = (i8)((u8)y + 1u)) {
      (void)DrawRestaurant_DrawWallCell(0, 15, (i8)((u8)y + scroll_y));
    }
    i8 y = (i8)(scroll_y + p->left_door_y_end + 15u - p->size_y);
    for (unsigned i = 0; i < 6; ++i, y = (i8)((u8)y + 1u)) {
      (void)DrawRestaurant_DrawWallCell(i < 3 ? 2 : 7, 15, y);
    }
  }
}

// One texture row is copied into the following 15 rows, then walls, uncovered margins and the
// shared doorway are composed.

void RestDrawKitchen_Background(Rest *r) {
  draw_kitchen_background(r);
}

// Restaurant sprites.

// These originals reset only the list links and the three specified actor
// fields. The pool's background, last coordinates and other flags survive.

void DrawRestaurant_ResetIgloos(void) {
  InitIglooLink(&map_igloo_head, &map_igloo_tail, &map_igloo_link);

  temp_canvas->actors = &map_igloo_link;
  for (unsigned i = 0; i < 150; ++i) {
    pt_igloo_pool_at(i)->draw_y = 0;
    pt_igloo_pool_at(i)->draw_x = 0;
    pt_igloo_pool_at(i)->orientation = 0;
  }
}
void SetupCanvasTempIglooLink(void) {
  InitIglooLink(&temp_igloo_1, &temp_igloo_2, &temp_igloo_link);

  temp_canvas->actors = &temp_igloo_link;
}

// Including its captured count and low-word coordinate wrapping. The return is an incidental
// byte loop counter, discarded by all known callers.

void DrawRestaurant_Proc8(Rest *r, u8 layer, u8 inverse) {
  u8 count = r->num_furniture, drawn = 0;

  DrawRestaurant_ResetIgloos();

  for (unsigned i = 0; i < count; ++i) {
    const Furniture *f = &r->furniture[i];
    if (!(f->flags & 4u))
      continue;
    const FurnitureDesc *d = pt_furniture_descriptor(f->type);
    if (inverse ? d->byte25 == layer : d->byte25 != layer)
      continue;
    pt_igloo_pool_at(drawn)->draw_x = (i16)((u16)f->x - 12u * r->scrollpos_x);
    pt_igloo_pool_at(drawn)->draw_y = (i16)(11u * r->scrollpos_y + f->y - 88u);

    SetupHazelWithBobSprite(pt_hazelnut_pool_at(drawn++), 0, &bob_restaurant, (u16)d->images[f->flags & 3u]);
  }
}

// Invalid unsigned word indices return before reading the Canvas or changing its current
// list.

void DrawBob(BobHost *bob, u16 image, i16 x, i16 y) {
  if (image >= (u16)bob->num_entries)
    return;

  SetupCanvasTempIglooLink();

  SetupHazelWithBobSprite(&temp_hazel, 0, bob, image);

  temp_igloo.draw_x = x;
  temp_igloo.draw_y = y;

  Canvas_DrawAllIgloos((temp_canvas), active_palette_pointer);
}

// Oven coordinates add kitchen scrolling, unlike furniture scrolling.

void RestDrawKitchen(Rest *r) {
  RestDrawKitchen_Background(r);

  // Direct commands use the complete projected scene. Retained callers
  // validate each original call, allowing a shared BOB lazy reload before
  // its selected header exists. The completed live list is checked below.

  DrawRestaurant_ResetIgloos();

  for (unsigned i = 0; i < r->num_ovens; ++i) {
    const Furniture *f = &r->ovens[i];
    const FurnitureDesc *d = pt_furniture_descriptor(f->type);
    pt_igloo_pool_at(i)->draw_x = (i16)(12u * r->kitchen_scroll_x + (u16)f->x);
    pt_igloo_pool_at(i)->draw_y = (i16)(11u * r->kitchen_scroll_y + f->y - 88u);

    SetupHazelWithBobSprite(
        pt_hazelnut_pool_at(i), 0, d->value ? &bob_kitchen : &bob_restaurant, (u16)d->images[f->flags & 3u]);

    pt_igloo_pool_at(i)->movement_pause = f->type;
  }

  Canvas_DrawAllIgloos((temp_canvas), active_palette_pointer);

  did_terminal_stuff = 0;
}

// Restaurant selection.

// Selection reads physical sprite headers, including indices beyond the
// loaded entry count. It never loads assets or clamps those indices.

static const BobHeader *furniture_image(const Furniture *f, const BobHost *images) {
  const FurnitureDesc *descriptor = pt_furniture_descriptor_identity(f->type);
  return pt_resource_header_at(images, (u16)((u16)descriptor->images[f->flag_bits & 3u]));
}
// Furniture rectangles include their right and bottom edges.

bool IsPointInFurniture(const Furniture *f, i16 x, i16 y) {

  bool found = false;
  if (x >= f->x && y >= f->y) {
    const BobHeader *image = furniture_image(f, &bob_restaurant);
    found = x <= (i32)f->x + image->w && y <= (i32)f->y + image->h;
  }

  return found;
}
// Outside-room selections retain their distinct byte codes.

u8 FindFurnitureAtPoint(Rest *r, i16 x, i16 y) {

  u8 result = 255;
  if (!r->is_in_kitchen) {
    if (x < 26) {
      result = 252;
      goto done;
    }
    const Poi *property = r->property;
    if (x > 12 * (i32)property->size_x + 22) {
      result = 254;
      goto done;
    }
    if (y > 265) {
      result = 251;
      goto done;
    }
    if (y < 270 - 11 * (i32)property->size_y) {
      result = 253;
      goto done;
    }
  }
  for (unsigned layer = 3; layer; --layer)
    for (unsigned i = 0; i < r->num_furniture; ++i) {
      const Furniture *f = pt_rest_furniture_identity(r, (u8)i, false);
      const FurnitureDesc *descriptor = pt_furniture_descriptor_identity(f->type);
      if (descriptor->byte25 != layer)
        continue;

      bool hit = IsPointInFurniture(f, x, y);

      if (hit) {
        result = (u8)i;
        goto done;
      }
    }
done:

  return result;
}
// Return the first physical guest record with the requested index.

u8 FindGuestForFurniture(u8 index) {
  for (unsigned i = 0; i < guest_count; ++i)
    if (pt_guest_identity((u8)i)->furniture_idx == index)
      return (u8)i;
  return 255;
}
// The result combines the selection class and byte record index.

u16 FindRestaurantObject(Rest *r) {

  u16 result = 0;
  restaurant_hit_test_owner = r;
  if (r->is_in_kitchen) {
    for (unsigned i = 0; i < r->num_ovens; ++i) {
      const Furniture *f = pt_rest_furniture_identity(r, (u8)i, true);
      const BobHeader *image = furniture_image(f, &bob_kitchen);
      i16 left = (i16)((u32)f->x + 12u * r->kitchen_scroll_x);
      i16 top = (i16)(f->y + 11u * r->kitchen_scroll_y - 88u);

      bool hit = IsMouseInside(left, top, (i16)((u32)left + image->w), (i16)((u32)top + image->h));

      if (hit) {
        result = (u16)(0x100u | i);
        break;
      }
    }
  } else {
    i16 x = (i16)((u32)mouse_x + 12u * r->scrollpos_x);
    i16 y = (i16)((u32)mouse_y + 88u - 11u * r->scrollpos_y);

    u8 index = FindFurnitureAtPoint(r, x, y);

    if (index < 250) {
      result = (u16)(0x100u | index);
      if (r->restaurant_opened && r == restaurant_with_guests) {

        u8 guest = FindGuestForFurniture(index);

        if (guest != 255)
          result = (u16)(0x200u | guest);
      }
    }
  }

  return result;
}

// Restaurant media.

// The original loads all four fill coordinates directly from its arguments.

void DrawGuestEatingFrame_Helper3(i16 left, i16 top, i16 right, i16 bottom) {
  FillRect(15, (u16)left, (u16)top, (u16)right, (u16)bottom);
  static const u8 colors[] = {17, 16, 17, 18};
  for (u32 i = 0; i < 4; ++i)
    (void)Canvas_DrawRect((u8)(colors[i]),
        (u16)(i16)((i16)((u32)(i32)left + i)),
        (u16)(i16)((i16)((u32)(i32)top + i)),
        (u16)(i16)((i16)((u32)(i32)right - i)),
        (u16)(i16)((i16)((u32)(i32)bottom - i)));
}

u8 ChooseItemFromTvOrJukebox(u8 kind, u8 selected) {
  if (kind > 1)
    abort();
  const u8 *choices[8];
  SetSomeFieldInCanvas(0);
  Copy320x200Bitmap(current_bitmap, old_bitmap);
  DrawGuestEatingFrame_Helper3(33, 36, 176, 148);
  (void)Canvas_DrawRect(20, 54, (u16)(i16)((i16)(10u * selected + 55u)), 162, (u16)(i16)((i16)(10u * selected + 65u)));
  const u8 *title = restaurant_media_labels[kind][0];
  for (u32 i = 0; i < 7; ++i) {
    choices[i] = restaurant_media_labels[kind][i + 1];
  }
  choices[7] = NULL;
  temp_canvas->color = 8;
  SetCurrentFont(font_astandar);
  DrawText2(33, 176, 50, title);
  SetCurrentFont(font_piznumb);
  game_ptr->list_element_spacing_2 = 2;
  temp_canvas->color = 14;
  DrawText(58, 141, restaurant_media_prompt);
  u8 result = AskForNewspaperChoice(7, 0, choices, 56, 57, 8, 15, 14);
  Copy320x200Bitmap(old_bitmap, current_bitmap);
  return result;
}
u8 ChooseFromTV(Rest *rest) {
  game_ptr->did_choose_tv_thingy = 10;
  u8 selected = ChooseItemFromTvOrJukebox(1, rest->tv_selection);
  return selected;
}
u8 ChooseFromJukebox(Rest *rest) {
  game_ptr->did_choose_tv_thingy = 10;
  u8 selected = ChooseItemFromTvOrJukebox(0, rest->jukebox_selection);
  if (selected != rest->jukebox_selection) {
    if (selected) {
      u32 loaded = LoadMusic(selected);
      if ((u16)loaded && ((u16)prefs.field_0 >> 8)) {
        SoundStuff2(0);
        competition_sound_pending = 1;
      }
    } else {
      rest->jukebox_selection = 0;
      state_slot_007 = 100;
      RestLoadMusic(rest, 0);
    }
  }
  return selected;
}

// Restaurant scroll.

#include "game.h"

void RefreshOverflow(Rest *rest) {
  static const char format[] = "RefreshOverflow: f->things=%d, noguests=%d";
  (void)snprintf((char *)strbuf, sizeof(strbuf), format, (unsigned)rest->num_furniture, (unsigned)guest_count);
  ShowMessage(16, strbuf);
  DisplayErrorMessage(0, strbuf);
}

void FurnitureSetFlag4(Rest *rest, Furniture *furniture) {
  if (furniture_refresh_depth > 10) {
    RefreshOverflow(rest);
    return;
  }
  ++furniture_refresh_depth;
  u8 link = furniture->linked_furniture;
  if (link != 255)
    FurnitureSetFlag4(rest, pt_rest_furniture_view(rest, link, false));
  furniture->flags_low = (u8)((u32)furniture->flags_low | 4u);
  --furniture_refresh_depth;
}

static bool rectangle_hit(const Furniture *item, const BobHeader *extent, i16 left, i16 top) {
  return (int)item->x + extent->w > left && (int)item->y + extent->h > top;
}
void SetGuestsAndFurnitureFlag4(Rest *rest, i16 left, i16 top, i16 right, i16 bottom) {
  unsigned count = rest->num_furniture;
  for (unsigned i = 0; i < count; ++i) {
    Furniture *item = pt_rest_furniture_view(rest, (u8)i, false);
    item->flags_low = (u8)(item->flags_low & (u8)~4u);
  }
  for (unsigned i = 0; i < count; ++i) {
    Furniture *item = pt_rest_furniture_view(rest, (u8)i, false);
    if (right < item->x || item->y > bottom)
      continue;
    const FurnitureDesc *descriptor = pt_furniture_descriptor(item->type);
    u16 image = pt_furniture_image(descriptor, (i16)(item->flag_bits & 3u));
    if (rectangle_hit(item, pt_resource_header_at(&bob_restaurant, (u16)image), left, top))
      FurnitureSetFlag4(rest, item);
  }
  for (unsigned i = 0; i < guest_count; ++i) {
    Guest *guest = pt_guest_at(i);
    const Furniture *linked = pt_rest_furniture_view(rest, guest->furniture_idx, false);
    bool hit = (linked->flags_low & 4u) != 0;
    if (!hit) {
      if (right < guest->x || guest->y > bottom)
        continue;
      unsigned image = 4u * guest->guest_type + 48u + (guest->flag_bits & 3u);
      hit = rectangle_hit(((Furniture *)(guest)), pt_resource_header_at(&bob_restaurant_open, (u16)image), left, top);
    }
    if (hit)
      FurnitureSetFlag4(rest, ((Furniture *)(guest)));
  }
}

static void guest_setup(unsigned slot, u16 image) {
  if (slot >= 150)
    abort();
  SetupHazelWithBobSprite(pt_hazelnut_pool_at(slot), 0, &bob_restaurant_open, image);
}

static void guest_reset(void) {
  SetupCanvasTempIglooLink();
  DrawRestaurant_ResetIgloos();
}
u8 GuestTurnCounterclockwise(u8 direction) {
  return (u8)((direction + 3u) & 3u);
}

i16 DrawAllGuests_Helper(Igloo *igloo, Guest *guest) {

  u8 direction = (u32)guest->flags & 3u;
  guest->byteE = direction;
  igloo->draw_x = (i16)((u16)igloo->draw_x + (u16)(i16)attack_step_x[direction]);
  i16 result = attack_step_y[guest->byteE];
  igloo->draw_y = (i16)((u16)igloo->draw_y + (u16)result);

  return result;
}

void DrawAllGuests(void) {

  guest_reset();
  u8 index = 0;
  while (index < guest_count) {
    unsigned i = index;
    Guest *g = pt_guest_at(i);
    Rest *r = curr_rest;
    unsigned direction = (u32)g->flags & 3u;
    pt_igloo_pool_at(i)->draw_x = (i16)((u16)g->x - 12u * r->scrollpos_x + (u32)(i32)guest_body_x[direction]);
    pt_igloo_pool_at(i)->draw_y = (i16)(11u * r->scrollpos_y + g->y - 88u + (u32)(i32)guest_body_y[direction]);
    if (g->byteB == 3)
      DrawAllGuests_Helper(&temp_igloo, g);
    if (g->byteD) {
      u8 chance = RandomChance(50);
      if (chance)
        DrawAllGuests_Helper(&temp_igloo, g);
      if (!--g->byteD)
        g->byteE = (u8)direction;
    }
    guest_setup(i, (u16)(4u * g->guest_type + g->byteE));
    ++index;
  }
  (void)Canvas_DrawAllIgloos(temp_canvas, active_palette_pointer);
  guest_reset();
  index = 0;
  while (index < guest_count) {
    unsigned i = index;
    const Guest *g = pt_guest_at(i);
    const Rest *r = curr_rest;
    unsigned direction = (u32)g->flags & 3u, pose = g->byteB, offset = 4u * pose + direction;

    if (offset >= sizeof(guest_pose_x))
      abort();
    pt_igloo_pool_at(i)->draw_x = (i16)((u16)g->x - 12u * r->scrollpos_x + (u32)(i32)guest_pose_x[offset]);
    pt_igloo_pool_at(i)->draw_y = (i16)(g->y + 11u * r->scrollpos_y - 88u + (u32)(i32)guest_pose_y[offset]);
    guest_setup(i, (u16)(4u * g->guest_type + 48u * pose + 144u + direction));
    ++index;
  }
  (void)Canvas_DrawAllIgloos(temp_canvas, active_palette_pointer);
}

void RestDrawOnScreen(Rest *r) {
  if (r->restaurant_opened) {
    u32 day = IsDayTime();
    if ((u16)day) {
      RepositionFurnitureHelper2(current_bitmap);
      u8 index = 0;
      while (index < guest_count) {
        unsigned i = index;
        Guest *g = pt_guest_at(i);
        if (++g->byte10 > 100) {
          g->byte10 = 0;
          ++g->byte11;
        }
        if (g->eating_or_waiting) {
          if (!g->byteD) {
            if (g->byteC) {
              if (g->byteC == 1) {
                if (g->byteB >= 3)
                  g->byteC = 2;
                else
                  ++g->byteB;
              }
              if (g->byteC == 2) {
                if (g->byteB <= 1)
                  g->byteC = 0;
                else
                  --g->byteB;
              }
            } else {
              u8 chance = RandomChance(20);
              if (chance)
                g->byteC = 1;
            }
          }
        } else if (g->byte12) {
          ++g->byte12;

          if (g->byte12 >= sizeof(guest_idle_script))
            abort();
          switch (guest_idle_script[g->byte12]) {
          case 0:
            break;
          case 1:
            g->byteE = GuestTurnCounterclockwise(g->byteE);
            break;
          case 2:
            g->byteE = InteractiveAttackRest_Plus1And3(g->byteE);
            break;
          case 3:
            g->byteB = 1;
            break;
          case 4:
            g->byteB = 2;
            break;
          default:
            g->byte12 = 0;
            g->byteB = 0;
            g->byteE = (u32)g->flags & 3u;
            break;
          }
        } else {
          u8 chance = RandomChance(2);
          if (chance)
            g->byte12 = 1;
        }
        ++index;
      }
      DrawAllGuests();
      if (state_slot_008) {
        DrawRestaurant_ResetIgloos();
        RestDrawOnScreen_Helper1(r);
        if (state_slot_008 > 1) {
          DrawRestaurant(r, 1);
          DrawMapHelper4(current_bitmap);
          DrawAllGuests();
          state_slot_008 = (u8)(state_slot_008 - 2u);
        }
        (void)Canvas_DrawAllIgloos(temp_canvas, active_palette_pointer);
      }
    }
  }
}

bool pt_rest_scroll_move(Rest *r, u8 direction) {
  if (direction > 3)
    return false;
  if ((r->is_in_kitchen && direction == 0) || (!r->is_in_kitchen && direction == 1)) {
    u8 *x = r->is_in_kitchen ? &r->kitchen_scroll_x : &r->scrollpos_x;
    if (!*x)
      return true;
    else
      --*x;
  } else if (direction == 3) {
    u8 *y = r->is_in_kitchen ? &r->kitchen_scroll_y : &r->scrollpos_y;
    if (!*y)
      return true;
    else
      --*y;
  } else {
    const Poi *p = r->property;
    u8 *at = direction == 2 ? (r->is_in_kitchen ? &r->kitchen_scroll_y : &r->scrollpos_y)
                            : (r->is_in_kitchen ? &r->kitchen_scroll_x : &r->scrollpos_x);
    u8 size = direction == 2 ? (r->is_in_kitchen ? p->kitchen_height : p->size_y)
                             : (r->is_in_kitchen ? p->kitchen_width : p->size_x);
    if ((int)size - *at < 15)
      return true;
    else
      ++*at;
  }
  return false;
}

PTBlitRect pt_rest_scroll_shift(u8 direction) {
  static const PTBlitRect rectangles[] = {
    {24, 14, 12, 14, 180, 176}, {12, 14, 24, 14, 180, 176}, {12, 14, 12, 25, 192, 165}, {12, 25, 12, 14, 192, 165}};
  return rectangles[direction];
}

bool pt_rest_scroll_bounds(const Rest *r, u8 direction, i16 out[4]) {
  const Poi *p = r->property;
  int x = 12 * r->scrollpos_x, y = 11 * r->scrollpos_y;
  int left, top, right, bottom;
  if (direction == 0) {
    if ((int)p->size_x - r->scrollpos_x < 15)
      return false;
    left = x + 191;
    right = x + 203;
    top = 102 - y;
    bottom = 277 - y;
  } else if (direction == 1) {
    if (!r->scrollpos_x)
      return false;
    left = x + 12;
    right = x + 24;
    top = 102 - y;
    bottom = 277 - y;
  } else if (direction == 2) {
    if ((int)p->size_y - r->scrollpos_y < 15)
      return false;
    left = x + 12;
    right = x + 203;
    top = 102 - y;
    bottom = 113 - y;
  } else {
    if (!r->scrollpos_y)
      return false;
    left = x + 12;
    right = x + 203;
    top = 266 - y;
    bottom = 277 - y;
  }
  out[0] = (i16)left;
  out[1] = (i16)top;
  out[2] = (i16)right;
  out[3] = (i16)bottom;
  return true;
}

static void mark_strip(Rest *rest, u8 direction) {
  i16 bounds[4];
  if (pt_rest_scroll_bounds(rest, direction, bounds))
    SetGuestsAndFurnitureFlag4(rest, bounds[0], bounds[1], bounds[2], bounds[3]);
}

i32 RestScroll(Rest *r, u8 direction) {
  Bitmap source = {0};
  pt_bitmap_bind_surface(&source, pt_graphics_pixels()->background);
  state_slot_014 = 1;
  bool blocked = pt_rest_scroll_move(r, direction);
  if (blocked) {
    state_slot_014 = 0;
    return 1;
  }
  if (r->is_in_kitchen) {

    SetCurrentBitmap(old_bitmap);
    RestDrawKitchen(r);
  } else {
    if (direction < 4) {
      PTBlitRect rect = pt_rest_scroll_shift(direction);

      BitBlt320Width(&source, rect.sx, rect.sy, old_bitmap, rect.dx, rect.dy, rect.width, rect.height);

      SetCurrentBitmap(old_bitmap);
      if (direction < 2) {
        i8 col = (i8)(r->scrollpos_x + (direction == 0 ? 14u : UINT32_MAX));
        DrawRestaurant_SingleCol(r, col);
      } else {
        const Poi *p = r->property;
        i8 row = (i8)(p->size_y - r->scrollpos_y - (direction == 2 ? 15u : 0u));
        DrawRestaurant_SingleRow(r, row);
      }
      mark_strip(r, direction);

      DrawRestaurant(r, 0);
    }
    DrawRestaurant_RightWall(r);
    DrawRestaurant_LeftWall(r);
    DrawRestaurant_TopWall(r);
    DrawRestaurant_BottomWall(r);

    DrawMapHelper4(current_bitmap);
    if (game_ptr->byte94) {
      InteractiveAttackRest_Proc8();
    }
    RestDrawOnScreen(r);
  }

  CommitBitmapAndSetPalette(0);
  state_slot_014 = 0;
  return 0;
}

// Placement contacts.

static i32 magnitude(i16 value) {
  return value < 0 ? -(i32)value : value;
}
// Signed word crossing tests and the original 100 containment marker.

u32 FurnitureContactAmount(const Rest *rest, const Igloo *preview, const BobHeader *header, const Furniture *item) {
  bool kitchen = rest->is_in_kitchen != 0;
  u8 type = item->type;
  u32 flags = item->flag_bits;
  const FurnitureDesc *descriptor = pt_furniture_descriptor(type);
  u16 image = pt_furniture_image(descriptor, (i16)(flags & 3u));
  const BobHost *bob = kitchen ? &bob_kitchen : &bob_restaurant;
  const BobHeader *other = pt_resource_header_at(bob, (u16)image);
  u8 scroll_x = kitchen ? rest->kitchen_scroll_x : rest->scrollpos_x;
  i32 x = preview->draw_x + (kitchen ? -12 : 12) * (i32)scroll_x;
  i16 item_x = item->x;
  u8 width = header->w, other_width = other->w;
  i16 a = (i16)((u32)(x - item_x - other_width));
  i16 b = (i16)((u32)(x + width - item_x));
  if ((a >= 0) == (b >= 0)) {
    return 0;
  }
  i32 horizontal = magnitude(a) < magnitude(b) ? magnitude(a) : magnitude(b);
  if ((width < other_width ? width : other_width) <= (i16)horizontal)
    horizontal = 100;
  u8 scroll_y = kitchen ? rest->kitchen_scroll_y : rest->scrollpos_y;
  i32 y = preview->draw_y + 88 - 11 * (i32)scroll_y;
  u8 item_y = item->y;
  u8 height = header->h, other_height = other->h;
  a = (i16)((u32)(y - item_y - other_height));
  b = (i16)((u32)(y + height - item_y));
  if ((a >= 0) == (b >= 0)) {
    return 0;
  }
  i32 vertical = magnitude(a) < magnitude(b) ? magnitude(a) : magnitude(b);
  if ((i16)vertical >= (height < other_height ? height : other_height))
    vertical = 100;
  return (u32)((i16)vertical > (i16)horizontal ? horizontal : vertical);
}

// Released furniture has no usable native contact identity. Cached placement
// results belong to the old room and cannot authorize another placement.

void pt_placement_contacts_retire(const void *storage, size_t size) {
  bool changed = false;
  for (i32 i = INT8_MIN; i <= INT8_MAX; ++i) {
    PTPlacementContact *contact = pt_placement_contact_state(i);
    if (contact->item && (uintptr_t)contact->item - (uintptr_t)storage < size) {
      *contact = (PTPlacementContact){0};
      changed = true;
    }
  }
  if (changed) {
    placement_contact_count = 0;
    placement_contact_recorded = 0;
  }
}

// Writes only the counter; it retains all old contact rows.

void ResetFurnitureContacts(void) {
  placement_contact_count = 0;
}
// Callers ignore the result when the signed count exceeds six. Negative counters retain their physical writes.

void RecordFurnitureContact(const Furniture *item, u8 index, u8 amount) {

  i32 slot = (i8)placement_contact_count;
  if (slot <= 6) {
    PTPlacementContact *row = pt_placement_contact_state(slot);
    row->item = item;
    row->index = index;

    row->type = item->type;

    row->amount = amount;

    placement_contact_recorded = 1;
    ++placement_contact_count;
  }
}
// Capacity uses equality, and same-layer collisions retain prior contact rows.

u8 FindPlacementContacts(Rest *rest, const BobHeader *header, u8 layer) {
  u8 result = 0;
  bool kitchen = rest->is_in_kitchen != 0;
  if ((kitchen && rest->num_ovens == 12) || (!kitchen && rest->num_furniture == 100)) {
    u8 *text = kitchen ? placement_maximum_ovens_text : placement_maximum_items_text;
    ShowMessage(16, text);
    result = 2;
    goto done;
  }
  ResetFurnitureContacts();
  u8 count = kitchen ? rest->num_ovens : rest->num_furniture;
  for (unsigned i = 0; i < count; ++i) {
    const Furniture *item = pt_rest_furniture_identity(rest, (u8)i, kitchen);
    u8 amount = (u8)FurnitureContactAmount(rest, &temp_igloo, header, item);
    if (amount) {
      if (pt_furniture_descriptor(item->type)->byte25 == layer) {
        result = 2;
        goto done;
      }
      RecordFurnitureContact(item, (u8)i, amount);
    }
  }
  result = placement_contact_count != 0;
done:
  return result;
}

// Capacity.

#pragma STDC FP_CONTRACT OFF
static double capacity_add53(double a, double b) {
  volatile double r = a + b;
  return r;
}

static double capacity_multiply53(double a, double b) {
  volatile double r = a * b;
  return r;
}

static double capacity_divide53(double a, double b) {
  volatile double r = a / b;
  return r;
}

static float narrow32(double a) {
  volatile float r = (float)a;
  return r;
}

static u32 capacity_integer32(double value) {
  value = TruncateX87Integral(value);
  return isfinite(value) && value >= -2147483648.0 && value < 2147483648.0 ? (u32)(i32)value : UINT32_C(0x80000000);
}

void ReClearEmptyStaffSlots(Rest *rest) {
  for (unsigned i = rest->num_chefs; i < 6; ++i)
    rest->staff_indexes[i] = 0;
  u8 start = (u8)(rest->num_waiters + 6u);
  for (unsigned i = start; i < 12; ++i)
    rest->staff_indexes[i] = 0;
}

u32 RestComputePizzaCapacity(Rest *rest) {
  unsigned count = rest->num_recipes;
  if (count > 24)
    abort();
  u32 weights[24] = {0};
  float total = 0;
  for (unsigned i = 0; i < count; ++i) {
    weights[i] = 2u * rest->recipes[i].orders;
    total = narrow32(capacity_add53((double)pt_signed_dword(weights[i]), total));
  }
  if (total < 1)
    for (unsigned i = 0; i < count; ++i) {
      weights[i] = 100;
      total = narrow32(capacity_add53(100, total));
    }
  for (;;) {
    u32 usage[96] = {0};
    for (unsigned ingredient = 0; ingredient < 96; ++ingredient)
      for (unsigned i = 0; i < count; ++i) {
        u16 amount = pt_recipe_weight(rest->recipes[i].recipe, ingredient);
        if (amount)
          usage[ingredient] += weights[i] * amount;
      }
    i32 largest = 0;
    u8 chosen = 0;
    for (unsigned ingredient = 0; ingredient < 96; ++ingredient) {
      u32 available = 10u * (u16)rest->stock[ingredient].weight;
      i32 excess = pt_signed_dword(usage[ingredient] - available);
      if (excess > largest) {
        largest = excess;
        chosen = (u8)ingredient;
      }
    }
    if (largest <= 0)
      break;
    u16 available = (u16)rest->stock[chosen].weight;
    float factor = available ? narrow32(capacity_divide53((double)pt_signed_dword(usage[chosen] - (u32)largest),
                                   pt_signed_dword(usage[chosen])))
                             : 0;
    bool changed = false;
    for (unsigned i = 0; i < count; ++i) {
      if (pt_recipe_weight(rest->recipes[i].recipe, chosen)) {
        u32 value = available ? capacity_integer32(capacity_multiply53(pt_signed_dword(weights[i]), factor)) : 0;
        changed |= value != weights[i];
        weights[i] = value;
      }
    }
    if (!changed)
      abort();
  }
  total = 0;
  for (unsigned i = 0; i < count; ++i)
    total = narrow32(capacity_add53(pt_signed_dword(weights[i]), total));
  return capacity_integer32(total);
}

u16 ReComputeAverageTimes(Rest *rest) {
  float weighted = 0, total = 0;
  rest->average_stay_time = 0;
  unsigned oven_count = rest->num_ovens;
  for (unsigned i = 0; i < oven_count; ++i) {
    const Furniture *item = pt_rest_furniture_identity(rest, (u8)i, true);
    const FurnitureDesc *fd = pt_furniture_descriptor(item->type);
    if (fd->furn_type == 8) {
      u32 capacity = fd->capacity;
      weighted = narrow32(capacity_add53(capacity * fd->baking_time, weighted));
      total = narrow32(capacity_add53(capacity, total));
    }
  }
  if (!pt_float_is_zero(total))
    rest->average_oven_time = narrow32(capacity_divide53(weighted, total));
  for (unsigned part = 0; part < 2; ++part) {
    weighted = total = 0;
    unsigned count = part ? rest->num_waiters : rest->num_chefs;
    for (unsigned i = 0; i < count; ++i) {
      const Staff *staff = pt_rest_staff_member(rest, (part ? 6u : 0u) + i);
      if (!staff->absent_days) {
        weighted = narrow32(capacity_add53(weighted, float_720));
        total = narrow32(capacity_add53((u16)staff->cooking_or_serving_capacity, total));
      }
    }
    if (total > 1) {
      float value = narrow32(capacity_divide53(weighted, total));
      if (part)
        rest->average_waiter_time = value;
      else
        rest->average_chef_time = value;
    }
  }
  float chef = rest->average_chef_time, oven = rest->average_oven_time;
  u8 wait = (u8)capacity_integer32(capacity_add53(chef > oven ? chef : oven, rest->average_waiter_time));
  if (wait < 2)
    wait = 30;
  rest->waiting_time = wait;
  if (wait <= 4)
    MarkGoalReached(49);
  weighted = total = 0;
  for (unsigned i = 0; i < 6; ++i) {
    u32 people = 1u + (u16)rest->customer_count[i];
    weighted = narrow32(capacity_add53(customer_eating_times[i] * people, weighted));
    total = narrow32(capacity_add53(people, total));
  }
  if (total > 1)
    rest->average_eating_time = narrow32(capacity_divide53(weighted, total));
  rest->average_stay_time = narrow32(capacity_add53(rest->waiting_time, rest->average_eating_time));
  return rest->waiting_time;
}

static u16 staff_capacity(Rest *rest, bool waiters) {
  unsigned count = waiters ? rest->num_waiters : rest->num_chefs;
  if (waiters && count > 249)
    abort();
  u16 capacity = 0;
  u32 weighted = 0;
  for (unsigned i = 0; i < count; ++i) {
    const Staff *staff = pt_rest_staff_member(rest, i + (waiters ? 6u : 0u));
    if (!staff->absent_days) {
      u32 amount = (u16)staff->cooking_or_serving_capacity;
      u8 reason = staff->absent_reason;
      if (reason == 101 || reason == 201)
        amount = (u32)MulDiv100((i32)amount, 80);
      capacity = (u16)(capacity + amount);
      weighted += amount * (waiters ? staff->selling : staff->cooking);
    }
  }
  rest->capacity_of_kind[waiters ? 4 : 2] = (i16)capacity;
  if (capacity) {
    u8 rating = (u8)(pt_signed_dword(weighted) / capacity);
    if (waiters)
      rest->average_waiter_rating = rating;
    else
      rest->average_cooking_rating = rating;
  }
  return capacity;
}

u8 ReComputeStaffStuff(Rest *rest) {
  u16 demand = rest->use_real_demand ? (u16)rest->guests_per_day[6] : 20;
  rest->demand_of_kind[0] = (i16)demand;
  ReClearEmptyStaffSlots(rest);
  unsigned count = rest->num_furniture, rated = 0;
  u16 seats = 0;
  u32 quality = 0;
  for (unsigned i = 0; i < count; ++i) {
    const Furniture *item = pt_rest_furniture_identity(rest, (u8)i, false);
    const FurnitureDesc *fd = pt_furniture_descriptor(item->type);
    if (fd->furn_type == 2 && item->linked_furniture != 255) {
      ++seats;
      quality += fd->quality;
      ++rated;
    }
  }
  // The original checks a table only in the following physical record,
  // including counts beyond the declared furniture array.

  const Furniture *following = pt_rest_furniture_identity(rest, (u8)count, false);
  const FurnitureDesc *fd = pt_furniture_descriptor(following->type);
  if (fd->furn_type == 1) {
    quality += fd->quality;
    ++rated;
  }
  rest->total_num_seats = (i16)seats;
  float stay = rest->average_stay_time;
  u16 capacity = 0;
  if (!pt_float_is_zero(stay))
    capacity = (u16)capacity_integer32(capacity_divide53(720u * seats, stay));
  rest->capacity_of_kind[0] = (i16)capacity;
  if (rated)
    rest->furniture_quality = (u8)(quality / rated);
  demand = demand < capacity ? demand : capacity;
  rest->demand_of_kind[1] = (i16)demand;
  capacity = (u16)RestComputePizzaCapacity(rest);
  rest->capacity_of_kind[1] = (i16)capacity;
  demand = demand < capacity ? demand : capacity;
  rest->demand_of_kind[2] = (i16)demand;
  capacity = staff_capacity(rest, false);
  demand = demand < capacity ? demand : capacity;
  rest->demand_of_kind[3] = (i16)demand;
  u16 ovens = 0;
  u32 weighted = 0;
  capacity = 0;
  count = rest->num_ovens;
  for (unsigned i = 0; i < count; ++i) {
    const Furniture *item = pt_rest_furniture_identity(rest, (u8)i, true);
    fd = pt_furniture_descriptor(item->type);
    u8 amount = fd->capacity, time = fd->baking_time;
    ovens = (u16)(ovens + amount);
    float rate = time ? narrow32(capacity_divide53(double_720, time)) : 0;
    float baked = narrow32(capacity_multiply53(amount, rate));
    capacity = (u16)capacity_integer32(capacity_add53(capacity, baked));
    float prior = narrow32(pt_signed_dword(weighted));
    weighted = capacity_integer32(capacity_add53(capacity_multiply53(fd->quality, baked), prior));
  }
  rest->capacity_of_kind[3] = (i16)capacity;
  rest->oven_capacity = (i16)ovens;
  if (capacity)
    rest->oven_quality = (u8)(pt_signed_dword(weighted) / capacity);
  demand = demand < capacity ? demand : capacity;
  rest->demand_of_kind[4] = (i16)demand;
  capacity = staff_capacity(rest, true);
  u16 final = demand < capacity ? demand : capacity;
  ReComputeAverageTimes(rest);
  for (unsigned i = 0; i < 5; ++i) {
    capacity = (u16)rest->capacity_of_kind[i];
    u32 ratio = 0;
    if (capacity) {
      u32 value = 100u * (u16)rest->demand_of_kind[i] / capacity;
      ratio = (u32)Clamp((i32)value, 0, 100);
    }
    rest->demand_capacity_ratio[i] = (u8)ratio;
  }
  demand = (u16)rest->demand_of_kind[1];
  return final == demand ? 100 : demand ? (u8)(100u * final / demand) : 0;
}

// Furniture info.

const u8 *pt_furniture_info_format(enum PTFurnitureInfoFormat format) {
  switch (format) {
  case OVEN_CAPACITY:
    return furniture_info_formats.oven_capacity;
  case OVEN_RATE:
    return furniture_info_formats.oven_rate;
  case OVEN_MINUTES:
    return furniture_info_formats.oven_minutes;
  case OVEN_PRICE:
    return furniture_info_formats.oven_price;
  case OVEN_LINK:
    return furniture_info_formats.oven_link;
  case ITEM_NUMBER:
    return furniture_info_formats.item_number;
  case FURNITURE_PRICE:
    return furniture_info_formats.furniture_price;
  case TABLE_CAPACITY:
    return furniture_info_formats.table_capacity;
  case TABLE_CHAIRS:
    return furniture_info_formats.table_chairs;
  case CHAIR_LINK:
    return furniture_info_formats.chair_link;
  case ITEM_QUALITY:
    return furniture_quality_format;
  }
  abort();
}
u16 pt_furniture_info_number_text(u8 text[256], enum PTFurnitureInfoFormat format, u32 bits) {
  const u8 *f = pt_furniture_info_format(format);
  size_t n = 0;
  while (*f != '%')
    text[n++] = *f++;
  f += 2;
  u32 magnitude = bits;
  if (bits > INT32_MAX) {
    text[n++] = '-';
    magnitude = 0u - bits;
  }
  u8 reverse[10];
  unsigned digits = 0;
  do {
    reverse[digits++] = (u8)('0' + magnitude % 10u);
    magnitude /= 10u;
  } while (magnitude);
  while (digits)
    text[n++] = reverse[--digits];
  while (*f)
    text[n++] = *f++;
  text[n] = 0;
  return (u16)n;
}
u16 GetPriceOfFurniture(u16 type) {
  return (u16)pt_furniture_descriptor(type)->value;
}
static void info_number(enum PTFurnitureInfoFormat format, u32 value) {
  u8 text[256];
  u16 length = pt_furniture_info_number_text(text, format, value);
  memcpy(strbuf, text, length + 1u);
  (void)DrawInfoBoxText(strbuf);
}
static void begin_info(void) {
  FillGenericInfoBox();
  Canvas_SetPosition(canvas_1, 228, 28);
}
void ShowFurnitureInfo(Rest *r, const Furniture *f) {
  const FurnitureDesc *d = pt_furniture_descriptor(f->type);
  u8 index = (u8)pt_furniture_record_number(r, f, false), chairs = 0;
  begin_info();
  (void)DrawInfoBoxText(pt_furniture_category_name(d->furn_type));
  (void)DrawInfoBoxText(d->name);
  info_number(ITEM_QUALITY, d->quality);
  u16 price = GetPriceOfFurniture(f->type);
  info_number(FURNITURE_PRICE, price);
  if (d->furn_type == 1) {
    info_number(TABLE_CAPACITY, d->capacity);
    for (unsigned i = 0; i < r->num_furniture; ++i)
      if (pt_rest_furniture_identity(r, (u8)i, false)->linked_furniture == index)
        ++chairs;
    info_number(TABLE_CHAIRS, chairs);
  }
  if (d->furn_type == 2 && f->linked_furniture != 255) {
    (void)DrawInfoBoxText(pt_furniture_info_format(CHAIR_LINK));
    u8 type = pt_rest_furniture_identity(r, f->linked_furniture, false)->type;
    (void)DrawInfoBoxText(pt_furniture_descriptor(type)->name);
  }
  if (game_ptr->debug_mode)
    info_number(ITEM_NUMBER, index);
}
void PrintOvenStatus(Rest *r, const Furniture *f) {
  const FurnitureDesc *d = pt_furniture_descriptor(f->type);
  begin_info();
  (void)DrawInfoBoxText(d->name);
  info_number(ITEM_QUALITY, d->quality);
  info_number(OVEN_CAPACITY, d->capacity);
  if (d->baking_time)
    info_number(OVEN_RATE, 60u * d->capacity / d->baking_time);
  info_number(OVEN_MINUTES, d->baking_time);
  u16 price = GetPriceOfFurniture(f->type);
  info_number(OVEN_PRICE, price);
  if (f->linked_furniture != 255) {
    (void)DrawInfoBoxText(pt_furniture_info_format(OVEN_LINK));
    u8 type = pt_rest_furniture_identity(r, f->linked_furniture, true)->type;
    (void)DrawInfoBoxText(pt_furniture_descriptor(type)->name);
  }
  if (game_ptr->debug_mode)
    info_number(ITEM_NUMBER, (u32)pt_furniture_record_number(r, f, true) / sizeof(Furniture));
}

// Restaurant stats.

#include "reports.h"
#include "player.h"

void ShowMonthReviewStats(Rest *r) {
  const i16 previous_column = 115, current_column = 172, row_height = 9;
  (void)DrawMonthlyOverviewCaptions(0);
  cur_list_elem_y = 54;
  for (unsigned i = 0; i < 3; ++i) {
    cur_list_elem_x = previous_column;
    DrawDollarsInsideList(pt_signed_dword((u32)r->last_month_receipts[i]));
    cur_list_elem_x = current_column;
    DrawDollarsInsideList(pt_signed_dword((u32)r->curr_month_receipts[i]));
    cur_list_elem_y = (i16)((u16)cur_list_elem_y + (u16)row_height);
  }
  cur_list_elem_y = 99;
  for (unsigned i = 0; i < 7; ++i) {
    cur_list_elem_x = previous_column;
    DrawDollarsInsideList(pt_signed_dword((u32)r->last_month_expenses[i]));
    cur_list_elem_x = current_column;
    DrawDollarsInsideList(pt_signed_dword((u32)r->curr_month_expenses[i]));
    cur_list_elem_y = (i16)((u16)cur_list_elem_y + (u16)row_height);
  }
  (void)SetTempCanvasPosition(previous_column, 184);
  DrawDollarsInsideList(pt_signed_dword((u32)r->last_month_receipts[0] - (u32)r->last_month_expenses[0]));
  cur_list_elem_x = current_column;
  DrawDollarsInsideList(pt_signed_dword((u32)r->curr_month_receipts[0] - (u32)r->curr_month_expenses[0]));
}
i32 GetDayMonthHistoryString(u8 index) {
  i8 month = (i8)ptr_apple->month, day = (i8)(index + ptr_apple->day - 7u);
  if (day < 1) {
    month = (i8)((u8)month - 1u);
    if (month < 1)
      month = 12;
    i32 distance;
    distance = abs(day);
    day = (i8)(pt_calendar_month_days(month) - (u32)distance);
  }
  return snprintf((char *)strbuf, sizeof(strbuf), "%d-%d", day, month);
}
void ShowWeeklyOverviewStats(Rest *r) {
  u8 row;
  FillSomeRectWithColor(15);
  DrawAnimatedExpensesBgInner();
  DrawMarketCompetitorsCorners();
  SelectPalette(7);
  temp_canvas->color = 10;
  SetCurrentFont(font_ruby);
  (void)DrawText2(12, 203, 30, rest_weekly_caption);
  SetCurrentFont(font_piznumb);
  (void)SetTempCanvasPosition(32, 82);
  game_ptr->list_element_spacing_2 = 1;
  temp_canvas->color = 11;
  (void)SetTempCanvasPosition(37, 71);
  DrawListElement(rest_weekly_columns);
  temp_canvas->color = 13;
  for (row = 0; row < 8; ++row) {
    (void)SetTempCanvasPosition(23, (i16)(10 * row + 73));
    (void)RelativeLineTo(170, 0);
  }
  (void)SetTempCanvasPosition(23, 154);
  (void)RelativeLineTo(170, 0);
  (void)SetTempCanvasPosition(23, 157);
  (void)RelativeLineTo(170, 0);
  (void)SetTempCanvasPosition(44, 73);
  (void)RelativeLineTo(0, 80);
  (void)SetTempCanvasPosition(68, 63);
  (void)RelativeLineTo(0, 90);
  (void)SetTempCanvasPosition(92, 73);
  (void)RelativeLineTo(0, 80);
  (void)SetTempCanvasPosition(141, 63);
  (void)RelativeLineTo(0, 90);
  (void)SetTempCanvasPosition(180, 63);
  (void)RelativeLineTo(0, 90);
  temp_canvas->color = 10;
  draw_list_elem_style = 0;
  for (row = 0; row < 7; ++row) {
    unsigned i = row;
    (void)GetDayMonthHistoryString((u8)i);
    cur_list_elem_y = (i16)(10 * i + 82);
    (void)SetTempCanvasPosition(26, cur_list_elem_y);
    (void)DrawText2(16, 43, cur_list_elem_y, strbuf);
    cur_list_elem_x = 66;
    DrawNumberInsideListRight(pt_signed_dword((u16)r->guests_per_day[i]));
    cur_list_elem_x = 90;
    DrawNumberInsideListRight(pt_signed_dword((u16)r->pizza_count_per_day[i]));
    cur_list_elem_x = 130;
    DrawDollarsInsideList(pt_signed_dword((u16)r->sales_per_day[i]));
    cur_list_elem_x = 169;
    DrawDollarsInsideList(pt_signed_dword((u32)(u16)r->sales_per_day[i] - (u16)r->expenses_per_day[i]));
    cur_list_elem_x = 196;
    DrawNumberInsideListRight(pt_signed_dword(r->delivcapa_per_day[i]));
  }
  (void)SetTempCanvasPosition(26, 152);
  const i16 *histories[] = {r->guests_per_day, r->pizza_count_per_day, r->sales_per_day};
  static const i16 columns[] = {66, 90, 130};
  for (unsigned i = 0; i < 3; ++i) {
    cur_list_elem_x = columns[i];
    u32 total;
    total = SumListOfWords(histories[i], 7);
    if (i == 2)
      DrawDollarsInsideList(pt_signed_dword(total));
    else
      DrawNumberInsideListRight(pt_signed_dword(total));
  }
  cur_list_elem_x = 169;
  u32 sales, expenses;
  sales = SumListOfWords(r->sales_per_day, 7);
  expenses = SumListOfWords(r->expenses_per_day, 7);
  DrawDollarsInsideList(pt_signed_dword(sales - expenses));
  cur_list_elem_x = 196;
  u32 capacity;
  capacity = GetSumOfBytes(r->delivcapa_per_day, 7);
  DrawNumberInsideListRight(pt_signed_dword(capacity / 7u));
}
void ShowSalesCurve_Helper(u16 image, i16 bottom, i32 height) {
  u16 remaining = (u16)height;
  while (--remaining != UINT16_MAX && bottom > 40) {
    DrawBob(&bob_restaurant_stats, image, cur_list_elem_x, (i16)(bottom - 4));
    --bottom;
  }
}
void ShowCustomerStats(Rest *r) {
  const Poi *poi = r->property;
  const i16 area_bottom = 148, column_width = 26, local_bottom = 150;
  u8 row;
  DrawCustomersBackground();
  SelectPalette(7);
  temp_canvas->color = 12;
  SetCurrentFont(font_ruby);
  (void)DrawText2(12, 203, 30, rest_customers_caption);
  SetCurrentFont(font_supersma);
  for (row = 0; row < 6; ++row) {
    unsigned i = row;
    i16 x = (i16)(column_width * i);
    DrawBitmapStuff1((u8)i, (i16)(x + 31), 158);
    u32 local = (u16)r->customer_count[i] >> 2, area = (u16)poi->customer_count_in_area[i] >> 2;

    cur_list_elem_x = (i16)(x + 41);
    ShowSalesCurve_Helper(11, area_bottom, (i32)area);
    i16 y = (i16)((u16)area_bottom - (area + 4u));
    if (y < 36)
      y = 36;
    (void)SetTempCanvasPosition((i16)(x + 54), y);
    temp_canvas->color = 0;
    DrawNumberInsideListRight(pt_signed_dword((u16)poi->customer_count_in_area[i]));
    cur_list_elem_x = (i16)(x + 39);
    ShowSalesCurve_Helper(10, local_bottom, (i32)local);
    (void)SetTempCanvasPosition((i16)(x + 50), 156);
    temp_canvas->color = 0;
    DrawNumberInsideListRight(pt_signed_dword((u16)r->customer_count[i]));
  }
}
void ShowSalesCurve(Rest *r) {
  u8 first_day = (u8)(7u - r->use_real_demand);
  u8 day;
  i16 scale = 100;
  DrawGlobalProfitsBg();
  temp_canvas->color = 13;
  SelectPalette(7);
  SetCurrentFont(font_ruby);
  (void)DrawText2(12, 203, 28, rest_sales_caption);
  for (day = first_day; day < 7; ++day) {
    unsigned i = day;
    for (;;) {
      if ((u32)r->expenditure_per_day[i] / (u32)(i32)scale <= 100 && (i32)(u16)r->sales_per_day[i] / scale <= 100)
        break;
      scale = (i16)((u16)scale + 100u);
    }
  }
  SetCurrentFont(font_piznumb);
  {
    int n = snprintf((char *)strbuf, sizeof(strbuf), "in $%d units", 10 * (int)scale);
    (void)n;
  }
  (void)DrawText2(12, 203, 38, strbuf);
  temp_canvas->color = 0;
  SetCurrentFont(font_supersma);
  draw_list_elem_style = 0;
  (void)SetTempCanvasPosition(48, 152);
  cur_list_elem_x = 44;
  for (day = first_day; day < 7; ++day) {
    (void)GetDayMonthHistoryString(day);
    DrawStringBufferAsListElement();
    cur_list_elem_x = (i16)((u16)cur_list_elem_x + 22u);
  }
  cur_list_elem_x = 52;
  for (day = first_day; day < 7; ++day) {
    u32 height = (u32)r->expenditure_per_day[day] / (u32)(i32)scale;
    ShowSalesCurve_Helper(12, 137, (i16)height);
    cur_list_elem_x = (i16)((u16)cur_list_elem_x + 21u);
  }
  cur_list_elem_x = 45;
  for (day = first_day; day < 7; ++day) {
    i32 height = (i32)(u16)r->sales_per_day[day] / scale;
    ShowSalesCurve_Helper(10, 139, (i16)((u32)height));
    cur_list_elem_x = (i16)((u16)cur_list_elem_x + 21u);
  }
}
PTActionResult HandleRestaurantStatsMenu(Rest *r) {
  u8 page = 0, previous = 0;
  PTActionResult result = pt_action_value(0);
  LoadBobFile(&bob_restaurant_stats);
refresh:
  did_terminal_stuff = 0;
  result = Do3x3ButtonStuff(25);
display:
  switch (page) {
  case 0:
    daily_balance_flag = 1;
    (void)ShowDailyBalanceStats(r);
    daily_balance_flag = 0;
    break;
  case 1:
    ShowWeeklyOverviewStats(r);
    break;
  case 2:
    ShowMonthReviewStats(r);
    break;
  case 3:
    (void)ShowWeeksTakingStats(r);
    page = previous;
    goto refresh;
  case 4:
    ShowCustomerStats(r);
    break;
  case 5:
    (void)ShowSalesCurve(r);
    break;
  default:

    return result;
  }
  previous = page;
  for (;;) {
    u16 event;
    event = WaitForMouseClick();
    result = pt_action_event(result, event);
    mouse_pressed = event;
    if (event == 3849) {
      Rest *selected;
      selected = SelectNextRestaurant(r);
      r = selected;
      goto refresh;
    }
    if (pt_is_menu_button_event(event)) {
      page = (u8)(event + 16u);
      if (page != previous)
        goto display;
    }
  }
}

// Restaurant schedule.

#include <stdlib.h>
#include <string.h>
#include "state_types.h"

// restaurant day

// The original boundary is a no-op with no meaningful return.

void DummyProc(Rest *restaurant) {
  (void)restaurant;
}
static void shift_history(i16 history[7]) {
  memmove(history, history + 1, 6 * sizeof(*history));
}
void RunRestaurantDay(Rest *rest) {

  RestCheckIncreaseRange2(rest);
  if (rest->protection_value > 200 || (rest->protection_value > 1 && rest->protection_value != 200))
    rest->protection_value = (u8)((u32)rest->protection_value - 1u);
  shift_history(rest->guests_per_day);
  shift_history(rest->orders_per_day);
  shift_history(rest->pizza_count_per_day);
  shift_history(rest->demand_value_per_day);
  shift_history(rest->sales_per_day);
  shift_history(rest->expenses_per_day);
  memmove(rest->delivcapa_per_day, rest->delivcapa_per_day + 1, sizeof(rest->delivcapa_per_day) - 1);
  rest->guests_per_day[6] =
      (rest->orders_per_day[6] =
              (rest->pizza_count_per_day[6] =
                      (rest->demand_value_per_day[6] = (rest->sales_per_day[6] = (rest->expenses_per_day[6] = 0)))));
  if (!rest->restaurant_opened)
    rest->delivcapa_per_day[6] = 0;
  for (unsigned i = 0; i < rest->num_recipes; ++i) {
    const Recipe *recipe = rest->recipes[i].recipe;
    u32 cost = ComputeRecipeCost(recipe, rest);
    rest->recipes[i].costs_per_pizza = (i16)((u16)cost);
  }
  if (rest->restaurant_opened) {
    RestHelper5(rest);
    RestComputeNumOrders(rest);
    float supply_percentage = RestCheckIfEnoughSupplies(rest);

    double integral = TruncateX87Integral(supply_percentage);
    u32 capacity =
        !isfinite(integral) || integral < -0x1p31 || integral >= 0x1p31 ? UINT32_C(0x80000000) : (u32)(i32)integral;

    rest->delivcapa_per_day[6] = (u8)capacity;
    RestUpdateSupplyStats(rest);
    RestComputeRecipeQualities(rest);
  }
  u32 total = ComputeRestaurantSalariesSum(rest);
  i32 salaries = pt_signed_dword(total) / 7;
  const Poi *poi = rest->property;
  Player *player = player_ptr;
  if (poi->current_owner != poi->current_tenant)
    player->profit_last_30_days[29] =
        pt_signed_dword((u32)player->profit_last_30_days[29] - (u32)poi->rent_price / 30u);
  player = player_ptr;
  player->profit_last_30_days[29] = pt_signed_dword((u32)player->profit_last_30_days[29] - (u32)salaries);
  for (unsigned i = 0; i < 4; ++i) {
    i32 newer = (i8)rest->range_stuff_2[i], older = (i8)rest->range_stuff_1[i];
    newer = abs(newer);
    older = abs(older);
    if (newer > older)
      rest->range_stuff_1[i] = (rest->range_stuff_2[i] = 0);
    rest->range_stuff_1[i] = (u8)(rest->range_stuff_1[i] + rest->range_stuff_2[i]);
  }
  DummyProc(rest);
  if (rest->use_real_demand < 7)
    rest->use_real_demand = (u8)((u32)rest->use_real_demand + 1u);
  if (rest->restaurant_opened && prefs.automatic_report) {
    u8 holiday = IsOnHoliday();
    if (!holiday) {
      ShowDailyBalanceStuff2(rest);
    }
  }
  if (ptr_apple->weekday == 6) {
    RunRestaurantWeek(rest);
  }
  if (ptr_apple->day == days_in_month[ptr_apple->month]) {
    RunRestaurantMonth(rest);
  }
}

// restaurant week

static bool report_visible(const Rest *rest) {
  return rest->restaurant_opened && rest->player_index < game_ptr->num_interactive_players &&
      !players[rest->player_index].days_away;
}
// The original never reads the supplied restaurant argument.

u8 DummyProc2(Rest *restaurant) {
  (void)restaurant;
  return 0;
}

void RunRestaurantWeek(Rest *rest) {

  u32 pizzas = 0, sales = 0, expenses = 0;

  UpdatePoiCustomerCount(rest->property);
  i32 reputation = rest->byte29;
  reputation = abs(reputation);
  if (reputation < 120) {
    i8 amount = (i8)((u8)((u16)rest->word1F / 200u - 1u));
    RestUpdateByte29(rest, amount);
  }
  if (report_visible(rest)) {
    ShowWeeksTakingStats(rest);
  }
  ShowStaffExpenses(rest);
  DummyProc2(rest);
  for (unsigned i = 0; i < rest->num_recipes; ++i) {
    RestoRecipe *recipe = &rest->recipes[i];
    u32 sold = (u16)recipe->sold_items;
    pizzas += sold;
    sales += sold * (u16)recipe->price_per_pizza;
    expenses += sold * (u16)recipe->costs_per_pizza;

    recipe->last_price_per_pizza = recipe->price_per_pizza;
    recipe->sold_items = 0;
  }
  i32 profit = pt_signed_dword(sales - expenses);
  if (pt_signed_dword(pizzas) >= 10000) {
    MarkGoalReached(36);
  }
  if (profit >= 2500000) {
    MarkGoalReached(121);
  }
  if (profit >= 5000000) {
    MarkGoalReached(122);
  }
  if (rest->player_index < game_ptr->num_interactive_players) {
    memset(rest->weekly_ingredient_type_cost, 0, sizeof(rest->weekly_ingredient_type_cost));
  }
}

// restaurant daily

void RestRunDailyLoop(Rest *rest) {

  if (!rest) {
    return;
  }

  memmove(rest->expenditure_per_day, rest->expenditure_per_day + 1, 6 * sizeof(rest->expenditure_per_day[0]));

  rest->expenditure_per_day[6] = 0;
  restaurant_with_guests = NULL;

  RestCheckStockRotten(rest);

  RestDoManagerDuties(rest);

  RestComputeAveragePizzaPrice(rest);

  memset(&rest->manager_flag_1, 0, 8);

  RestRunStaffDaily(rest);

  ReComputeStaffStuff(rest);

  ReComputeAverageTimes(rest);

  if (!ptr_apple->weekday) {
    RestRunNewWeek(rest);
  }
  if (ptr_apple->day == 1) {
    RestRunNewMonth(rest);
  }

  u32 total = GetSumOfBytes(rest->delivcapa_per_day, 7);

  if (total == 700) {
    MarkGoalReached(48);
  }
  if (rest->attack_var) {
    unsigned closed = rest->attack_var / 20u;
    rest->restaurant_closed_for_days = (u8)(closed >= 2 ? 2 : closed);

    AddPeachEntry(rest, 56, 3, 1, 9);

    rest->attack_var = 0;
    rest->restaurant_opened = 0;
  }
  if (rest->restaurant_opened) {
    if (rest->days_until_major_visit & 127u)
      rest->days_until_major_visit = (u8)((u32)rest->days_until_major_visit - 1u);
    else {
      u8 argument = (u8)(1u + (rest->days_until_major_visit == 128));

      u8 time = (u8)(RandomNumber(30) + 54u);

      AddPeachEntry(rest, time, 3, 0, argument);

      rest->days_until_major_visit = (u8)(RandomNumber(60) + 60u);
    }
  }
  for (unsigned pass = 0; pass < 4; ++pass) {
    for (unsigned index = 0; index < 96; ++index) {
      bool valid = IsValidIngredient((u8)index) != 0;

      if (!valid || !rest->stock[index].need)
        continue;

      u16 days = ComputeStockDaysLeft(&rest->stock[index]);

      if (days >= 2)
        continue;
      u16 amount = rest->stock[index].need >> 1;
      if (amount <= 1)
        amount = 1;

      CopyFromDepotToRest(rest, (u8)(index / 24u), (u8)(index % 24u), amount);
    }
  }
}

// restaurant billing

u8 RunRestaurantMonthInner(Rest *rest) {

  special_debt_flag = 1;
  game_ptr->list_element_spacing_2 = 0;
  u32 electricity = 3u * (u16)rest->capacity_of_kind[3];

  u32 income = ComputeSlotMachineIncome(rest);

  u32 difference = electricity - income;

  bool holiday = IsOnHoliday() != 0;

  if (!holiday) {

    ShowInvoice(rest, invoice_month_names[ptr_apple->month - 1u]);

    draw_list_elem_style = 0;
    cur_list_elem_x = 45;

    DrawListElement(restaurant_electricity_label);

    cur_list_elem_x = 165;

    DrawDollarsInsideList(pt_signed_dword(electricity));

    cur_list_elem_x = 45;
    cur_list_elem_y = (i16)((u16)cur_list_elem_y + 16u);

    DrawListElement(restaurant_slot_income_label);

    cur_list_elem_x = 165;

    DrawDollarsInsideList(pt_signed_dword(income));

    cur_list_elem_x = 45;
    cur_list_elem_y = (i16)((u16)cur_list_elem_y + 10u);

    DrawListElement(restaurant_invoice_total_label);

    cur_list_elem_x = 165;

    DrawDollarsInsideList(pt_signed_dword(difference));

    WaitForMouseClick2();
  }

  AddCostForRestaurantInner(rest, electricity, 4);

  AddIncomeForRestaurantInner(rest, income, 2);

  TransferPlayerMoney(rest->player_index, 10, 5, (float)pt_signed_dword(difference));

  special_debt_flag = 0;

  return 0;
}
u8 RunRestaurantMonth(Rest *rest) {

  u8 result = RunRestaurantMonthInner(rest);

  return result;
}

// staff expenses

#pragma STDC FP_CONTRACT OFF
static double restaurant_schedule_multiply53(double a, double b) {
  volatile double value = a * b;
  return value;
}

void ShowStaffExpenses(Rest *rest) {

  u32 total = 0;

  bool holiday = IsOnHoliday() != 0;

  if (!holiday) {

    ShowInvoice(rest, staff_expenses_label);
  }
  for (unsigned group = 0; group < 2; ++group) {
    unsigned count = group ? rest->num_waiters : rest->num_chefs;
    for (unsigned i = 0; i < count; ++i) {
      Staff *staff = &g_staff[rest->staff_indexes[6u * group + i]];

      DrawStaffExpenses(staff);

      cur_list_elem_y = (i16)((u16)cur_list_elem_y + 6u);
      total += (u16)staff->salary;
    }
    cur_list_elem_y = (i16)((u16)cur_list_elem_y + 5u);
  }
  if (rest->staff_indexes[12]) {
    Staff *staff = &g_staff[rest->staff_indexes[12]];
    total += (u16)staff->salary;

    DrawStaffExpenses(staff);

    cur_list_elem_y = (i16)((u16)cur_list_elem_y + 6u);
  }
  cur_list_elem_x = 165;

  holiday = IsOnHoliday() != 0;

  if (!holiday) {

    DrawDollarsInsideList(pt_signed_dword(total));

    WaitForMouseClick2();
  }
  special_debt_flag = 1;

  TransferPlayerMoney(rest->player_index, 10, 5, (float)total);

  special_debt_flag = 0;

  AddCostForRestaurantInner(rest, total, 1);

  for (unsigned i = 0; i < 12; ++i) {
    u8 index = rest->staff_indexes[i];
    if (!index)
      continue;
    Staff *staff = &g_staff[index];
    if (!staff->absent_days) {

      u16 wanted = ComputeWantedStaffSalary(staff);

      if ((u16)staff->salary < wanted) {

        StaffAddMotivation(staff, UINT32_MAX);

        if (wanted > restaurant_schedule_multiply53((u16)staff->salary, staff_expenses_underpaid_factor)) {

          StaffAddMotivation(staff, UINT32_MAX - 1u);
        }
        if (staff->absent_reason == 205) {

          StaffAddMotivation(staff, UINT32_MAX - 9u);
        }
      }

      if (restaurant_schedule_multiply53(wanted, staff_expenses_overpaid_factor) < (u16)staff->salary &&
          staff->motivation < 70) {

        StaffAddMotivation(staff, 1);
      }
    }
    if ((i8)staff->byte2D > 0)
      --staff->byte2D;
  }
}

// Restaurant lookup.

// A zero next link marks the sentinel. Return the first matching physical
// record without reading its suffix or the later game/debug state.

Rest *FindRestaurantById(u8 id) {
  PTForwardLink *node = player_ptr->restaurants.begin.next;
  for (;;) {
    PTForwardLink *next = node->next;
    if (!next)
      break;
    Rest *rest = ((Rest *)(node));
    if (rest->restaurant_id == id)
      return rest;
    node = next;
  }
  if (game_ptr->debug_mode)
    DisplayErrorMessage(0, restaurant_not_found_text);
  return NULL;
}

// Restaurant overview.

static i16 add_y(i32 y, unsigned delta) {
  return (i16)((u16)((u32)y + delta));
}

// Map origins wrap to bytes; history values are unsigned words.

Canvas *ShowRestaurantsOverviewEntry(const Rest *r, i32 y) {
  const Poi *poi = r->property;
  u8 map_x = (u8)(poi->xcoord - 4u);
  u8 map_y = (u8)(poi->ycoord - 4u);
  Canvas_DrawRect(13, 35, (u16)add_y(y, 16), 70, (u16)add_y(y, 51));
  for (unsigned row = 0; row < 8; ++row)
    for (unsigned column = 0; column < 8; ++column) {
      u8 tile = pt_map_tile_read(map_x + column, map_y + row);
      BitBlt(bmp20_ptr,
          (tile & 240u) >> 2,
          4u * (tile & 15u),
          current_bitmap,
          37u + 4u * column,
          (u16)add_y(y, 18u + 4u * row),
          4,
          4);
    }
  SetCurrentFont(font_piznumb);
  (void)SetTempCanvasPosition(26, add_y(y, 7));
  const u8 *name = GetRestaurantName(r);
  DrawListElement(name);
  cur_list_elem_x = 125;
  Zone *zone = FindZoneFromCoordinates(poi->xcoord, poi->ycoord);

  DrawListElement((const u8 *)zone->name);
  temp_canvas->color = 11;
  (void)SetTempCanvasPosition(81, add_y(y, 25));
  (void)RelativeLineTo(110, 0);
  (void)SetTempCanvasPosition(81, add_y(y, 37));
  (void)RelativeLineTo(110, 0);
  (void)SetTempCanvasPosition(111, add_y(y, 16));
  (void)RelativeLineTo(0, 34);
  (void)SetTempCanvasPosition(153, add_y(y, 16));
  (void)RelativeLineTo(0, 34);
  (void)SetTempCanvasPosition(124, add_y(y, 23));
  DrawListElement(restaurant_overview_day);
  cur_list_elem_x = 161;
  DrawListElement(restaurant_overview_week);
  temp_canvas->color = 13;
  (void)SetTempCanvasPosition(20, add_y(y, 60));
  (void)RelativeLineTo(150, 0);
  (void)SetTempCanvasPosition(74, add_y(y, 34));
  DrawListElement(weekly_sales_text);
  (void)SetTempCanvasPosition(74, add_y(y, 46));
  DrawListElement(weekly_profit_text);
  temp_canvas->color = 0;
  (void)SetTempCanvasPosition(141, add_y(y, 34));
  u32 sales = (u16)r->sales_per_day[6], expenses = (u16)r->expenses_per_day[6];
  u32 daily_profit = sales - expenses;
  DrawDollarsInsideList(pt_signed_dword(sales));
  (void)SetTempCanvasPosition(141, add_y(y, 46));
  DrawDollarsInsideList(pt_signed_dword(daily_profit));
  sales = expenses = 0;
  for (unsigned i = 0; i < 7; ++i) {
    sales += (u16)r->sales_per_day[i];
    expenses += (u16)r->expenses_per_day[i];
  }
  (void)SetTempCanvasPosition(188, add_y(y, 34));
  DrawDollarsInsideList(pt_signed_dword(sales));
  (void)SetTempCanvasPosition(188, add_y(y, 46));
  Canvas *result = DrawDollarsInsideList(pt_signed_dword(sales - expenses));

  return result;
}
// The signed count-minus-two bound keeps empty and singleton pages at zero.

u16 ShowRestaurantsOverview(void) {
  const Rest *restaurants[40] = {0};
  const Player *player = player_ptr;
  PTForwardLink *node = player->restaurants.begin.next;
  u8 count = 0, page = 0;
  while (node->next) {
    if (count >= 40)
      break;

    if (count >= 40)
      abort();
    restaurants[count] = (Rest *)node;
    node = node->next;
    ++count;
  }
  Do3x3ButtonStuff(27);
  for (;;) {
    FillSomeRectWithColor(15);
    SelectPalette(0);
    DrawAnimatedExpensesBgInner();
    DrawMarketCompetitorsCorners();
    SetCurrentFont(font_astandar);
    DrawCaptionShadowed(40, 30, restaurant_overview_caption);

    if (page + 1u >= 40)
      abort();
    const Rest *first = restaurants[page];
    if (first)
      ShowRestaurantsOverviewEntry(first, 36);

    const Rest *second = restaurants[page + 1u];
    if (second)
      ShowRestaurantsOverviewEntry(second, 104);
    u16 event;
    do {
      event = WaitForMouseClick();
      mouse_pressed = event;
    } while (!pt_is_menu_button_event(event));
    u16 result = (u16)(event - PT_MENU_BUTTON_1);
    mouse_pressed = result;
    if (!result) {
      i32 declared = player->num_restaurants;
      if (declared > 40)
        declared = 40;
      if ((i32)page < declared - 2)
        page = (u8)(page + 2u);
    }
    if (mouse_pressed == 1 && page)
      page = (u8)(page - 2u);
    if (mouse_pressed == 2) {
      return result;
    }
  }
}

// Restaurant demand.

static bool human_restaurant(const Rest *rest) {
  return rest->player_index < game_ptr->num_interactive_players;
}
static u32 demand_root(u32 value) {
  double result = sqrt((double)value);
  result = TruncateX87Integral(result);
  return (u32)result;
}
static u16 scale_customer(u16 value, unsigned percent) {
  return (u16)MulDiv100(value, (i32)percent);
}

u8 ReIsValidStaffIndex(Rest *rest, u8 slot) {
  u8 result;
  if (slot < 6)
    result = slot < rest->num_chefs;
  else
    result = (unsigned)slot - 6u < rest->num_waiters || (slot == 12 && rest->staff_indexes[12]);
  return result;
}
u32 ComputeRestaurantSalariesSum(Rest *rest) {
  u32 total = 0;
  unsigned i = 0;
  for (; i < 13; ++i) {
    if (rest->staff_indexes[i]) {
      u8 valid = ReIsValidStaffIndex(rest, (u8)i);
      if (valid)
        total += (u16)pt_rest_staff_member(rest, i)->salary;
    }
  }
  return total;
}
void RestUpdateByte29(Rest *rest, i8 amount) {
  i32 value = (i16)((u16)((u16)rest->byte29 + (i32)amount));
  rest->byte29 = (i16)(value < 0 ? 0 : value > 200 ? 200 : value);
}

// Signed recipe IDs below fourteen participate in the imitation average.

u8 RestaurantProc4(Rest *rest) {
  unsigned total = 0, count = 0;
  for (unsigned i = 0; i < rest->num_recipes; ++i) {
    const Recipe *entry = pt_menu_recipe(rest, (u8)i);
    if ((i8)entry->id < 14) {
      total += entry->imitation;
      ++count;
    }
  }
  return count ? (u8)(total / count) : 0;
}
i32 RestaurantScaledCloseness(const Rest *rest, u32 popularity, u32 distance) {
  if ((unsigned)rest->restaurant_range + 1u < distance)
    return 0;
  i32 result = pt_signed_dword(popularity - 4u * distance);
  return result < 0 ? 0 : result;
}

// Update popularity for each customer age group.

void RestaurantProc2(Rest *rest) {
  unsigned total = 0;
  u8 age = 0;
  for (; age < 6; ++age) {
    i32 product = (rest->sum_of_reputations + rest->furniture_rating[age]) * rest->menu_rating[age];
    if (product < 0)
      product = 0;
    u32 root = demand_root((u32)product);
    i32 value = (i32)root * rest->byte29 / 100;
    if (human_restaurant(rest) && (rest->num_standard_recipes < 3 || RestaurantProc4(rest) < 30))
      value = value >= 0 ? value / 8 : -((-value + 7) / 8);
    if (value < 1)
      value = 1;
    if (value > 250)
      value = 250;
    rest->popularity_with_customer[age] = (u8)value;
    total += (unsigned)value;
  }
  rest->word1F = (i16)((u16)total);
}

// The labelled original repair retains one strongest competitor, replacing
// the original overflowing thirty-pointer temporary table.

u32 RestaurantProc3(Rest *rest) {
  const Poi *poi = rest->property;
  u8 x = poi->xcoord, y = poi->ycoord;
  FindZoneFromCoordinates(x, y);
  u8 range = (u8)demand_root(rest->restaurant_range_squared);
  rest->restaurant_range = range;
  int left = (int)x - range - 10, right = (int)x + range + 10;
  int top = (int)y - range - 10, bottom = (int)y + range + 10;
  if (left < 0)
    left = 0;
  if (right >= 160)
    right = 159;
  if (top < 0)
    top = 0;
  if (bottom >= 120)
    bottom = 119;
  u16 scores[6] = {100, 100, 100, 100, 100, 100};
  const Rest *best = NULL;
  unsigned count = 0;
  u16 best_score = 0;
  u8 age = 0;
  for (u8 player = 0; player < 8; ++player) {
    PTForwardLink *node = players[player].restaurants.begin.next;
    for (; node->next; node = node->next) {
      const Rest *other = (const Rest *)node;
      if (other == rest || !other->restaurant_opened || other->restaurant_closed_for_days)
        continue;
      const Poi *other_poi = other->property;
      u8 ox = other_poi->xcoord, oy = other_poi->ycoord;
      if (ox < left || ox > right || oy < top || oy > bottom)
        continue;
      unsigned horizontal = (unsigned)abs((int)x - ox);
      u8 distance = (u8)(horizontal + abs((int)y - oy));
      u32 total = 0;
      for (age = 0; age < 6; ++age) {
        i32 score = RestaurantScaledCloseness(other, other->popularity_with_customer[age], distance);
        scores[age] = (u16)(scores[age] + score);
        total += (u32)score;
      }
      if ((u16)total > best_score) {
        best = other;
        best_score = (u16)total;
      }
      ++count;
    }
  }
  u32 score_sum = SumListOfUnsignedWords(scores, 6);
  rest->byte8F = (u8)count;
  rest->word91 = (i16)((u16)(score_sum - 600u));
  const Poi *best_poi = best ? best->property : NULL;
  rest->target_coordinate_x = best ? best_poi->xcoord : 0;
  rest->target_coordinate_y = best ? best_poi->ycoord : 0;
  for (age = 0; age < 6; ++age) {
    u16 population = (u16)poi->customer_count_in_area[age];
    u16 value = population;
    i32 score = (i16)scores[age];
    if (rest->popularity_with_customer[age] < score) {
      value = (u16)((unsigned)rest->popularity_with_customer[age] * population / (unsigned)score);
      if (human_restaurant(rest) && rest->num_standard_recipes < 3)
        value = (u16)(value / 3u);
    }
    rest->customer_count[age] = (i16)value;
    if (human_restaurant(rest))
      rest->customer_count[age] = (i16)(scale_customer(value, RestaurantProc4(rest)));
  }
  rest->byte99 = 0;
  u32 customers = SumListOfWords(rest->customer_count, 6);
  u16 capacity = (u16)rest->capacity_of_kind[0];
  if (customers > capacity) {
    u32 percent = (u32)Mul100Div(capacity, (i32)customers);
    for (age = 0; age < 6; ++age)
      rest->customer_count[age] = (i16)(scale_customer((u16)rest->customer_count[age], percent));
    rest->byte99 = (u8)(100u - percent);
  }
  // Invalid WORD sums must not enter an undefined native division.

  if (!(u16)score_sum)
    abort();
  u32 result = 100u * (u16)rest->word1F / (u16)score_sum;
  result = (u32)Clamp((i32)result, 0, 100);
  rest->local_share = (u8)result;
  return result;
}

static void range_notice(Rest *rest, u8 previous, u8 next) {
  const u8 *name = GetRestaurantName(rest);
  (void)snprintf((char *)strbuf,
      sizeof(strbuf),
      "The range of the restaurant %s rises from %d to %d.",
      (const char *)name,
      previous,
      next);
  ShowMessage(15, strbuf);
}
void RestCheckIncreaseRange(Rest *rest) {
  u8 previous = (u8)demand_root(rest->restaurant_range_squared);
  rest->restaurant_range = previous;
  if (rest->restaurant_closed_for_days) {
    rest->restaurant_closed_for_days = (u8)((u32)rest->restaurant_closed_for_days - (u32)1u);
    if (!rest->restaurant_closed_for_days) {
      AddRestaurantRangeStuff(rest, 226, 15);
      if (!human_restaurant(rest))
        rest->restaurant_opened = 1;
    }
    return;
  }
  u8 squared = rest->restaurant_range_squared;
  if (squared < 120 && RandomChance(50))
    ++squared;
  if (squared > 200)
    squared = 200;
  rest->restaurant_range_squared = squared;
  u8 next = (u8)demand_root(squared);
  if (next > previous && human_restaurant(rest))
    range_notice(rest, previous, next);
  rest->restaurant_range = next;
  for (unsigned i = 0; i < 4; ++i) {
    i32 amount = (i8)rest->range_stuff_1[i];
    i32 step = (i8)rest->range_stuff_2[i];
    if (abs(step) >= abs(amount)) {
      rest->range_stuff_2[i] = 0;
      rest->range_stuff_1[i] = 0;
    } else
      rest->range_stuff_1[i] = (u8)(amount + step);
  }
  RestaurantProc2(rest);
  RestaurantProc3(rest);
}
void RestCheckIncreaseRange2(Rest *rest) {
  if (rest->restaurant_opened && rest->byte29 < 100 && RandomChance(character_ptr->charisma))
    RestUpdateByte29(rest, 1);
  RestCheckIncreaseRange(rest);
}

// Restaurant orders.

#include <fenv.h>
#pragma STDC FP_CONTRACT OFF
static double restaurant_orders_add53(double a, double b) {
  volatile double r = a + b;
  return r;
}
static double sub53(double a, double b) {
  volatile double r = a - b;
  return r;
}
static double restaurant_orders_mul53(double a, double b) {
  volatile double r = a * b;
  return r;
}
static double restaurant_orders_div53(double a, double b) {
  volatile double r = a / b;
  return r;
}
static float restaurant_orders_single(double a) {
  volatile float r = (float)a;
  return r;
}
static i32 integer(double a) {
  a = TruncateX87Integral(a);
  return isfinite(a) && a >= -2147483648.0 && a < 2147483648.0 ? (i32)a : INT32_MIN;
}

u16 IsValidIngredient2(u8 index) {
  u16 result = index / 24u < 4 && index % 24u < num_ingredients_in_set[index / 24u];
  return result;
}

i8 RecipeComputeTrend(Recipe *recipe) {
  FindMajorRecipeIngredients(recipe);

  int value = 0;
  for (unsigned i = 0; i < 4; ++i) {
    u8 index = recipe->major_ingredient[i];

    u16 valid = IsValidIngredient2(index);

    if (valid)
      value += (i8)current_town->ingredient_trend[index];
  }
  if (value < -125)
    value = -125;
  if (value > 125)
    value = 125;

  return (i8)value;
}
static u8 rating(const Rest *rest, const RestoRecipe *entry, u8 age) {
  const Poi *poi = rest->property;
  Zone *district = FindZoneFromCoordinates(poi->xcoord, poi->ycoord);
  const Zone *zone = district;
  Recipe *recipe = entry->recipe;
  double low_price =
      sub53(pizza_income_price_offset, restaurant_orders_div53(zone->how_low_income, pizza_income_price_divisor));
  double base = restaurant_orders_add53(
      restaurant_orders_div53(zone->how_low_income, pizza_income_base_divisor), pizza_income_base_offset);
  i32 trend = RecipeComputeTrend(recipe);
  unsigned age_rating = (*pt_recipe_age_rating_field(recipe, age));
  if (trend > 0) {
    i32 bonus = MulDiv100(trend, (i32)age_rating * 2 - 100);
    trend += bonus;
  }
  double relative = restaurant_orders_div53(
      restaurant_orders_add53((u16)entry->price_per_pizza, pizza_price_offset), pizza_price_divisor);
  float price = restaurant_orders_single(relative > 1 ? relative : 1);
  i32 adjusted = ((i32)entry->sum_of_qualities - zone->byte3A) * zone->byte38 / 200 + entry->sum_of_qualities;
  float quality = restaurant_orders_single(restaurant_orders_div53(adjusted, zone->byte3A));
  // Stock's DPOW result is consumed only through integer truncation. Keep
  // each surrounding x87 operation rounded at the established 53-bit profile.

  double power = pow(price, base);
  i32 penalty = integer(power);
  if (price <= low_price)
    penalty = integer(sub53(penalty, restaurant_orders_mul53(sub53(low_price, price), pizza_price_bonus_scale)));
  i32 result = integer(sub53(restaurant_orders_mul53(trend + (i32)age_rating, quality), penalty));
  return (u8)(result < 0 ? 0 : result > 200 ? 200 : result);
}
u8 RestComputePizzaRating(const Rest *rest, const RestoRecipe *entry, u8 age) {
  u8 value = rating(rest, entry, age);

  return value;
}

u8 RestComputeNumOrders(Rest *rest) {
  unsigned count = rest->num_recipes;
  float accumulated[24] = {0}, ratings[25] = {0};
  u8 visited[25] = {0};
  for (unsigned i = 0; i < count; ++i) {
    rest->recipes[i].orders = 0;
  }
  for (unsigned age = 0; age < 6; ++age) {
    float sum = 0;
    for (unsigned i = 0; i < count; ++i) {
      ratings[i] = RestComputePizzaRating(rest, &rest->recipes[i], (u8)age);

      sum = restaurant_orders_single(restaurant_orders_add53(sum, ratings[i]));
      visited[i] = 0;
    }
    ratings[count] = 0;

    unsigned top_sum = 0;
    for (unsigned slot = 0; slot < 5; ++slot) {
      unsigned best = count;
      for (unsigned i = 0; i < count; ++i)
        if (!visited[i] && ratings[i] > ratings[best])
          best = i;
      top_sum += (unsigned)ratings[best];
      visited[best] = 1;
    }
    rest->menu_rating[age] = (u8)(top_sum / 5u + rest->average_waiter_rating / 8u);
    for (unsigned i = 0; i < count; ++i) {
      if (sum != 0) {
        double sold = restaurant_orders_div53(restaurant_orders_mul53((u16)rest->customer_count[age], ratings[i]), sum);

        accumulated[i] = restaurant_orders_single(restaurant_orders_add53(sold, accumulated[i]));
      }
    }
  }
  for (unsigned i = 0; i < count; ++i) {
    if (accumulated[i] > pizza_order_random_threshold) {
      accumulated[i] = (float)(RandomNumber(5) + 250u);
    }
    rest->recipes[i].orders = (u8)integer(accumulated[i]);
  }
  return (u8)count;
}

// Recipe qualities preserve binary32 accumulators and FILD's signed low dword,
// including quality*weight*factor products that cross INT32_MAX.

static i32 ingredient_quality(const Rest *rest, const Recipe *recipe, bool *has_weight) {
  float total = 0, quality = 0;
  for (unsigned i = 0; i < 96; ++i) {
    u16 weight = (u16)recipe->weight_of_type[i];
    if (!weight)
      continue;
    u32 amount = weight * ingredients[i / 24].vegs[i % 24].factor;
    total = restaurant_orders_single(restaurant_orders_add53(amount, total));
    quality =
        restaurant_orders_single(restaurant_orders_add53(pt_signed_dword(amount * rest->stock[i].quality), quality));
  }
  *has_weight = total != 0;
  return *has_weight ? integer(restaurant_orders_div53(quality, total)) : 0;
}
void RestComputeRecipeQualities(Rest *rest) {
  u16 total = 0;
  i32 prior_quality = 0;
  rest->quality_of_things[0] = rest->average_purchase_quality / 4;
  rest->quality_of_things[1] = rest->average_cooking_rating / 4;
  rest->quality_of_things[2] = rest->oven_quality / 4;
  i32 other = (i8)rest->byte2C;
  rest->quality_of_things[3] = (u8)(other >= 0 ? other / 4 : -((-other + 3) / 4));
  i32 cooking = rest->average_cooking_rating < rest->oven_quality ? rest->average_cooking_rating : rest->oven_quality;

  for (unsigned i = 0; i < rest->num_recipes; ++i) {
    bool has_weight;
    i32 quality = ingredient_quality(rest, rest->recipes[i].recipe, &has_weight);
    if (has_weight)
      prior_quality = quality;
    else
      prior_quality = 0;
    i32 limited = cooking < (i16)((u16)prior_quality) ? cooking : (i16)((u16)prior_quality);
    u8 sum = (u8)(rest->quality_of_things[0] + rest->quality_of_things[1] + rest->quality_of_things[2] +
        rest->quality_of_things[3]);
    if (limited < 16) {
      i32 value = sum + limited;
      sum = (u8)(value >= 0 ? value / 2 : -((-value + 1) / 2));
    }
    rest->recipes[i].sum_of_qualities = sum;
    total = (u16)(total + sum);
  }
  if (rest->num_recipes)
    rest->pizza_quality = (u8)(total / rest->num_recipes);
}

// Restaurant sales.

#pragma STDC FP_CONTRACT OFF
static double restaurant_sales_add53(double a, double b) {
  volatile double r = a + b;
  return r;
}
static double restaurant_sales_div53(double a, double b) {
  volatile double r = a / b;
  return r;
}
static double restaurant_sales_mul53(double a, double b) {
  volatile double r = a * b;
  return r;
}
static float restaurant_sales_single(double a) {
  volatile float r = (float)a;
  return r;
}
static u32 integral(double a) {
  return (u32)(i32)TruncateX87Integral(a);
}

u32 RoundUpAndDivideBy100(u32 value) {
  if (value % 100u >= 50u)
    value += 100u;
  u32 result = value / 100u;

  return result;
}
u32 DeductFromStock(Rest *rest, u8 set, u8 item, u16 requested) {
  // Stock checks the full promoted dword for zero, then uses DX. The public
  // command passes that dword separately; a zero low word still costs zero.

  if (!requested)
    return 0;
  RestaurantStock *stock = &rest->stock[24u * set + item];
  u16 available = (u16)stock->weight;
  u16 used = requested < available ? requested : available;
  stock->weight = (i16)((u16)(available - used));

  u32 result = RoundUpAndDivideBy100((u32)used * (u16)stock->value);

  return result;
}
// Accumulates prices in CX before the unsigned divide. An empty menu leaves the previous
// average untouched.

void RestComputeAveragePizzaPrice(Rest *rest) {
  u16 total = 0;
  if (rest->num_recipes) {
    for (unsigned i = 0; i < rest->num_recipes; ++i)
      total = (u16)(total + (u16)rest->recipes[i].price_per_pizza);
    rest->average_pizza_price = (i16)((u16)(total / rest->num_recipes));
  }
}
u16 ComputeStockDaysLeft(const RestaurantStock *stock) {
  u16 result = 0;
  if (stock->need) {
    u32 denominator = 100u * stock->need;

    result = (u16)((u16)stock->weight / denominator);
  }

  return result;
}

void RestRunNewWeek(Rest *rest) {
  (void)rest;
}
static u32 live_decay_draw(void *context) {
  (void)context;

  u32 result = RandomNumber(3);

  return result;
}
static unsigned live_decay_notice(void *context, const Rest *rest, const u8 *text, size_t length) {
  (void)context;

  memcpy(strbuf, text, length);

  ShowDelayedMessage(rest->player_index, rest->restaurant_id, 72, strbuf);

  return 0;
}
static unsigned live_decay_charge(void *context, float amount) {
  (void)context;

  TransferPlayerMoney(current_player_index, 10, 4, amount);

  return 0;
}
static const PTStockDecayEffects live_decay_effects = {
  .draw = live_decay_draw, .notice = live_decay_notice, .charge = live_decay_charge};
void pt_run_stock_decay(PTStockDecayState *c) {
  bool notified = false;
  for (unsigned set = 0; set < 4 && !c->status; ++set) {
    if (num_ingredients_in_set[set] > 24) {
      c->status = 1;
      return;
    }

    for (unsigned item = 0; item < num_ingredients_in_set[set]; ++item) {
      unsigned index = 24u * set + item;
      RestaurantStock *stock = &c->view.stock[index];

      if (!stock->quality)
        continue;
      int quality = (int)stock->quality - ramtown.base_decrease_ingredient_quality - (int)c->effects->draw(c->context);
      if (quality < 0)
        quality = 0;
      stock->quality = (u8)quality;
      if (c->destination) {
        c->destination->stock[index].quality = (u8)quality;
      }
      if (quality >= 5)
        continue;
      if (!notified) {
        const u8 *name = ingredients[set].vegs[item].name;
        if (!memchr(name, 0, sizeof(ingredients[set].vegs[item].name))) {
          c->status = 1;
          return;
        }
        u8 text[256];
        int n = snprintf((char *)text, sizeof(text), "The supplies of %s were rotten and had to be thrown away!", name);
        if (n < 0 || n >= 256) {
          c->status = 1;
          return;
        }
        c->status = c->effects->notice(c->context, &c->view, text, (size_t)n + 1u);
        if (c->status)
          return;
      }
      Rest *stock_source = c->destination ? c->destination : &c->view;
      u32 amount = DeductFromStock(stock_source, (u8)set, (u8)item, (u16)stock->weight);
      amount = RoundUpAndDivideBy100(amount);

      c->status = c->effects->charge(c->context, (float)amount);
      if (c->status)
        return;

      stock->weight = 0;
      notified = true;
    }
  }
}
void RestCheckStockRotten(Rest *rest) {
  PTStockDecayState c = {.destination = rest, .view = *rest, .effects = &live_decay_effects};

  pt_run_stock_decay(&c);
}

void UpdateAllCostsForPlayer(u8 player) {
  u32 total = 0;
  for (unsigned i = 1; i < 9; ++i)
    total += pt_mission_expenses(player)->category[i];
  pt_mission_expenses(player)->category[0] = total;
  total = 0;
  for (unsigned i = 1; i < 4; ++i)
    total += pt_mission_receipts(player)->category[i];
  pt_mission_receipts(player)->category[0] = total;
}
void AddCostForPlayer(u8 player, u32 amount, u8 category) {
  pt_mission_expenses(player)->category[category] += amount;

  UpdateAllCostsForPlayer(player);
}
void AddCostForRestaurant(Rest *rest, u32 amount, u8 category) {
  rest->expenditure_per_day[6] = pt_signed_dword((u32)rest->expenditure_per_day[6] + amount);

  AddCostForRestaurantInner(rest, amount, category);
}
void AddCostForRestaurantInner(Rest *rest, u32 amount, u8 category) {
  rest->curr_month_expenses[category] = pt_signed_dword((u32)rest->curr_month_expenses[category] + amount);

  AddCostForPlayer(rest->player_index, amount, category);

  u32 total = 0;
  for (unsigned i = 1; i < 9; ++i)
    total += (u32)rest->curr_month_expenses[i];
  rest->curr_month_expenses[0] = pt_signed_dword(total);
}
u32 ComputeSlotMachineIncome(Rest *rest) {
  u32 total = 0;
  for (unsigned i = 0; i < rest->num_furniture; ++i) {
    const FurnitureDesc *description = pt_furniture_descriptor(rest->furniture[i].type);
    u8 type = description->furn_type;

    if (type == 5 || type == 6) {
      u32 customer_draw = RandomNumber((u16)rest->customer_count[1]);

      u32 extra = RandomNumber(20);

      total += (customer_draw + extra) * (8u - type);
    }
  }

  return total;
}
i32 *RestRunNewMonth(Rest *rest) {
  if (!state_slot_009) {
    memmove(rest->last_month_expenses, rest->curr_month_expenses, sizeof(rest->curr_month_expenses));

    memmove(rest->last_month_receipts, rest->curr_month_receipts, sizeof(rest->curr_month_receipts));
  }
  memset(rest->curr_month_expenses, 0, sizeof(rest->curr_month_expenses));
  memset(rest->curr_month_receipts, 0, sizeof(rest->curr_month_receipts));

  return rest->curr_month_receipts;
}
void UpdateMoneyFromMissions(u8 player, u32 money, u8 category) {
  pt_mission_receipts(player)->category[category] += money;

  UpdateAllCostsForPlayer(player);
}
u8 AddIncomeForRestaurantInner(Rest *rest, u32 money, u8 category) {
  rest->curr_month_receipts[category] = pt_signed_dword((u32)rest->curr_month_receipts[category] + money);

  UpdateMoneyFromMissions(rest->player_index, money, category);

  u32 total = 0;
  for (unsigned i = 1; i < 4; ++i)
    total += (u32)rest->curr_month_receipts[i];
  rest->curr_month_receipts[0] = pt_signed_dword(total);

  return 4;
}
static unsigned live_supply_message(void *context, const Rest *rest, u8 *text) {
  (void)context;
  (void)rest;

  ShowMessage(16, text);

  return 0;
}
static unsigned live_supply_goal(void *context, u8 kind) {
  (void)context;

  MarkGoalReached(kind);

  return 0;
}
static unsigned live_supply_charge(void *context, float cost) {
  (void)context;

  TransferPlayerMoney(current_player_index, 10, 4, cost);

  return 0;
}
static const PTSupplyEffects live_supply_effects = {
  .message = live_supply_message, .goal = live_supply_goal, .charge = live_supply_charge};
static void message(PTSupplyState *c, u8 *text) {
  if (!c->status)
    c->status = c->effects->message(c->context, &c->view, text);
}

static void supply_sales(PTSupplyState *c, unsigned index, u8 value) {
  c->view.recipes[index].sales = value;
  if (c->destination)
    c->destination->recipes[index].sales = value;
}
static void supply_stock_need(PTSupplyState *c, unsigned ingredient, u8 value) {
  c->view.stock[ingredient].need = value;
  if (c->destination)
    c->destination->stock[ingredient].need = value;
}
static void supply_manager_shortage(PTSupplyState *c, u8 value) {
  c->view.manager_flag_1 = value;
  if (c->destination)
    c->destination->manager_flag_1 = value;
}
static void supply_manager_help(PTSupplyState *c, u8 value) {
  c->view.manager_had_to_help = value;
  if (c->destination)
    c->destination->manager_had_to_help = value;
}
static u32 deduct(PTSupplyState *c, unsigned ingredient, u16 amount) {
  Rest *stock_source = c->destination ? c->destination : &c->view;
  u32 result = DeductFromStock(stock_source, (u8)(ingredient / 24u), (u8)(ingredient % 24u), amount);
  c->view.stock[ingredient].weight = stock_source->stock[ingredient].weight;
  return result;
}
float pt_run_restaurant_supplies(PTSupplyState *c) {
  unsigned count = c->view.num_recipes;
  u32 orders[24] = {0};
  float total = 0;
  for (unsigned i = 0; i < count; ++i) {
    orders[i] = c->view.recipes[i].orders;
    total = restaurant_sales_single(restaurant_sales_add53(total, orders[i]));
  }
  float initial = total;
  if (total < 1) {
    for (unsigned i = 0; i < count; ++i)
      supply_sales(c, i, 0);
    return 100.0f;
  }
  bool first = true, notified = false;
  u32 needed[96] = {0};
  for (unsigned outer = 0; outer < 7000; ++outer) {
    bool capacity_message = false;
    for (unsigned reduction = 0; reduction < 7000; ++reduction) {
      memset(needed, 0, sizeof(needed));
      for (unsigned ingredient = 0; ingredient < 96; ++ingredient) {
        for (unsigned i = 0; i < count; ++i) {
          const Recipe *r = c->view.recipes[i].recipe;
          needed[ingredient] += (u16)r->weight_of_type[ingredient] * orders[i];
        }
        if (first && needed[ingredient])
          supply_stock_need(c, ingredient, needed[ingredient] > 250000u ? 250 : (u8)(needed[ingredient] / 1000u + 1u));
      }
      first = false;
      i32 difference = 0;
      unsigned chosen = 0;
      for (unsigned ingredient = 0; ingredient < 96; ++ingredient) {
        i32 excess = pt_signed_dword(needed[ingredient] - 10u * (u16)c->view.stock[ingredient].weight);
        if (excess > difference) {
          difference = excess;
          chosen = ingredient;
        }
      }
      if (difference <= 0)
        break;
      if (!notified) {
        message(c, restaurant_supplies_shortage_text);
        notified = true;
      }
      if (c->status)
        return 0;
      float ratio = restaurant_sales_single(restaurant_sales_div53(needed[chosen] - (u32)difference, needed[chosen]));
      bool changed = false;
      for (unsigned i = 0; i < count; ++i)
        if ((c->view.recipes[i].recipe)->weight_of_type[chosen]) {
          u32 value = (u16)c->view.stock[chosen].weight ? integral(restaurant_sales_mul53(orders[i], ratio)) : 0;
          changed |= value != orders[i];
          orders[i] = value;
        }
      if (!changed || reduction == 6999) {
        c->status = 1;
        return 0;
      }
    }
    total = 0;
    for (unsigned i = 0; i < count; ++i)
      total = restaurant_sales_single(restaurant_sales_add53(total, orders[i]));
    float previous = total;
    u32 available = (u16)c->view.capacity_of_kind[3];
    if (available < total) {
      total = (float)available;
      message(c, restaurant_ovens_shortage_text);
      capacity_message = true;
    }
    for (unsigned role = 0; role < 2; ++role) {
      unsigned kind = role ? 4 : 2;
      available = (u16)c->view.capacity_of_kind[kind];
      if (available < total) {
        supply_manager_shortage(c, (u8)(role + 1));
        if (c->view.staff_indexes[12] && c->view.mgr_staff_shortage == 2) {
          available += (u16)pt_staff_record(c->view.staff_indexes[12])->cooking_or_serving_capacity;
          supply_manager_help(c, (u8)(role + 1));
        }
        if (available < total) {
          total = (float)available;
          message(c, role ? restaurant_waiters_shortage_text : restaurant_cooks_shortage_text);
          capacity_message = true;
        }
      }
    }
    if (c->status)
      return 0;
    if (total == 0) {
      for (unsigned i = 0; i < count; ++i)
        supply_sales(c, i, 0);
      return 0;
    }
    if (total < previous) {
      float ratio = restaurant_sales_single(restaurant_sales_div53(total, previous));
      float used = 0;
      for (unsigned i = 0; i < count; ++i)
        if (orders[i]) {
          orders[i] = integral(restaurant_sales_mul53(orders[i], ratio));
          used = restaurant_sales_single(restaurant_sales_add53(used, orders[i]));
        }
      for (unsigned i = 0; i < count; ++i)
        if (orders[i] && used < total) {
          ++orders[i];
          used = restaurant_sales_single(restaurant_sales_add53(used, 1));
        }
      if (capacity_message)
        continue;
    }
    u32 value = 0, amount = 0;
    for (unsigned ingredient = 0; ingredient < 96; ++ingredient)
      if (needed[ingredient]) {
        value += deduct(c, ingredient, (u16)(needed[ingredient] / 10u));
        amount += needed[ingredient];
      }
    for (unsigned goal = 0; goal < 2; ++goal)
      if (amount >= (goal ? 5000000u : 1000000u)) {
        u8 kind = goal ? 120 : 96;
        c->status = c->effects->goal(c->context, kind);
        if (c->status)
          return 0;
      }
    total = 0;
    for (unsigned i = 0; i < count; ++i) {
      total = restaurant_sales_single(restaurant_sales_add53(total, orders[i]));
      supply_sales(c, i, (u8)orders[i]);
    }

    u32 rounded = RoundUpAndDivideBy100(value);

    float cost = restaurant_sales_single(rounded);
    c->status = c->effects->charge(c->context, cost);
    return restaurant_sales_single(
        restaurant_sales_div53(restaurant_sales_mul53(total, restaurant_supply_percentage_scale), initial));
  }
  c->status = 1;
  return 0;
}
float RestCheckIfEnoughSupplies(Rest *rest) {
  PTSupplyState c = {.destination = rest, .view = *rest, .effects = &live_supply_effects};

  float result = pt_run_restaurant_supplies(&c);

  return result;
}

void pt_restaurant_sales_totals(
    const Rest *rest, float *demand, float *sales, float *expenses, u16 *orders, u16 *sold) {
  *demand = *sales = *expenses = 0;
  *orders = *sold = 0;
  for (unsigned i = 0; i < rest->num_recipes; ++i) {
    const RestoRecipe *r = &rest->recipes[i];
    *orders = (u16)(*orders + r->orders);
    *sold = (u16)(*sold + r->sales);
    *demand = restaurant_sales_single(restaurant_sales_add53(
        restaurant_sales_div53((u16)r->price_per_pizza * (u32)r->orders, restaurant_sales_currency_divisor), *demand));
    *sales = restaurant_sales_single(restaurant_sales_add53(
        restaurant_sales_div53((u16)r->price_per_pizza * (u32)r->sales, restaurant_sales_currency_divisor), *sales));
    *expenses = restaurant_sales_single(restaurant_sales_add53(
        restaurant_sales_div53((u16)r->costs_per_pizza * (u32)r->sales, restaurant_sales_currency_divisor), *expenses));
  }
}
u32 RestUpdateSupplyStats(Rest *rest) {
  float demand, sales, expenses;
  u16 orders, sold;
  pt_restaurant_sales_totals(rest, &demand, &sales, &expenses, &orders, &sold);
  for (unsigned i = 0; i < rest->num_recipes; ++i) {
    RestoRecipe *r = &rest->recipes[i];
    Recipe *recipe = r->recipe;
    recipe->rating[6] = (i16)((u16)((u16)recipe->rating[6] + r->sales));
    r->sold_items = (i16)((u16)((u16)r->sold_items + r->sales));
  }
  rest->orders_per_day[6] = (i16)orders;
  rest->pizza_count_per_day[6] = (i16)sold;
  rest->demand_value_per_day[6] = (i16)(integral(demand));
  rest->sales_per_day[6] = (i16)(integral(sales));
  rest->expenses_per_day[6] = (i16)(integral(expenses));
  if (sold >= 1000) {
    MarkGoalReached(35);
  }
  if (sales >= restaurant_turnover_goal_threshold) {
    MarkGoalReached(45);
  }
  i32 *history = &savegame_sect_0.past_sales_per_day_for_players[rest->player_index][ptr_apple->town_index];
  *history = pt_signed_dword((u32)(u64)TruncateX87Integral(restaurant_sales_add53((u32)*history, sales)));
  Player *p = player_ptr;
  p->sales_per_day_monthly[29] = (i16)((u16)((u16)p->sales_per_day_monthly[29] + sold));
  p->accum_sales_per_day_monthly[29] = pt_signed_dword(
      (u32)(u64)TruncateX87Integral(restaurant_sales_add53((u32)p->accum_sales_per_day_monthly[29], sales)));
  p->expenditure_monthly[29] = pt_signed_dword((u32)p->expenditure_monthly[29] + (u32)rest->expenditure_per_day[6]);

  TransferPlayerMoney(rest->player_index, 5, 10, sales);

  AddIncomeForRestaurantInner(rest, integral(sales), 1);

  volatile double profit = (double)sales - expenses;
  u32 result = integral(profit);
  p = player_ptr;
  p->profit_last_30_days[29] = pt_signed_dword((u32)p->profit_last_30_days[29] + result);

  return result;
}

// Restaurant attack.

#include "ai_day.h"
u8 CrashFurniture(Furniture *item) {
  u8 type = item->type;
  restaurant_with_guests = NULL;
  if (type == 147 || type == 148) {
    return 0;
  }
  const FurnitureDesc *desc = pt_furniture_descriptor(type);
  u8 category = desc->byte25;
  if (type >= 149) {
    item->type = 162;
    value_of_crashed_furniture_1 += (u16)desc->value;
  } else {
    item->type = category == 1 || category == 3 ? 148 : 147;

    FurnitureSetFlag4(item->linked_furniture == 255 ? NULL : curr_rest, item);
    // The descriptor value is read after the recursive flag updates.

    attack_damage += (u16)pt_furniture_descriptor(type)->value;
  }

  return 1;
}
void CrashWholeRestaurant(Rest *rest) {
  u8 furniture_index;
  furniture_index = 0;
  while (furniture_index < rest->num_furniture) {
    (void)CrashFurniture(pt_rest_furniture_view(rest, furniture_index, false));
    ++furniture_index;
  }
  furniture_index = 0;
  while (furniture_index < rest->num_ovens) {
    (void)CrashFurniture(pt_rest_furniture_view(rest, furniture_index, true));
    ++furniture_index;
  }

  const Poi *poi = rest->property;
  attack_damage += (u32)poi->size_x * poi->size_y * rest->floor.price;
  memcpy(&rest->floor_state, empty_restaurant_floor, sizeof(rest->floor_state));
}
i32 ChangeRestaurantByte2C(Rest *rest, i8 delta) {
  i32 result = (i8)rest->byte2C + (i32)delta;
  if (result > 120)
    result = 120;
  if (result < -120)
    result = -120;
  rest->byte2C = (u8)result;

  return result;
}
static void translated_notice(Rest *rest, u8 kind) {
  u8 *text;
  text = GetTranslatedString(kind);

  ShowDelayedMessage(rest->player_index, rest->restaurant_id, (u8)(kind + 54u), text);
}
void HandleRestaurantAttackedStink(Rest *rest, u8 kind) {
  if (kind < 4) {
    static const i8 delta[] = {-30, -40, -60};
    static const u8 penalty[] = {236, 226, 216, 196}, duration[] = {4, 5, 8, 10};
    if (kind < 3)
      (void)ChangeRestaurantByte2C(rest, delta[kind]);

    AddRestaurantRangeStuff(rest, penalty[kind], duration[kind]);
  }
  (void)ChangeCurrentPlayerUnderworldRating((i16)(5u * (kind + 1u)));

  if (rest->player_index < game_ptr->num_interactive_players)
    translated_notice(rest, kind);
}
void HandleRestaurantAttackedWeapon(Rest *rest, u8 kind) {
  (void)ChangeCurrentPlayerUnderworldRating((i16)(10u * kind));

  if (rest->player_index < game_ptr->num_interactive_players) {
    if (2u * attack_damage) {
      Player *owner = pt_player(rest->player_index);
      owner->last_insurance_damage_type = 3;
      owner->last_crashed_furniture_value = pt_signed_dword(value_of_crashed_furniture_1 + attack_damage);
    }
    translated_notice(rest, kind);
  } else {
    if (attack_contamination)
      rest->restaurant_closed_for_days = (u8)(attack_contamination / 20u);
    i32 loss = pt_signed_dword(attack_damage) / 1000;
    rest->attack_var_1 = (i16)((u16)((u16)rest->attack_var_1 - 5u - (u32)(loss + 1)));
    rest->capacity_of_kind[0] = 0;
    if (attack_damage) {
      i32 days = (i16)((u16)loss);
      days = days >= 0 ? days / 2 : -((-days + 1) / 2);
      rest->restaurant_closed_for_days = (u8)days;
    }
    rest->attacked_by_whom =
        (u8)((u32)rest->attacked_by_whom | (u32)((u8)(UINT32_C(1) << (current_player_index & 31u))));
  }
  for (unsigned i = 0; i < 2; ++i) {
    float amount = (float)pt_signed_dword(i ? value_of_crashed_furniture_1 : attack_damage);
    u8 debit = i ? 1 : 2;

    (void)TransferPlayerMoney(rest->player_index, 10, debit, amount);
  }
}

void HandleUserAttackRestaurant(Rest *rest) {
  const u8 *choices[8];

  Do3x3ButtonStuff(17);

  LoadLangFile(&langfile_action);
  Rest *target = rest == &temp_rest && rest->linked.next ? (Rest *)rest->linked.next : rest;
  bool interactive = rest != &temp_rest || !rest->linked.next;
  for (unsigned i = 0; i < 8; ++i) {
    u8 *text;
    text = GetTranslatedString((u8)(i + 1));
    choices[i] = text;
  }

  u32 day;
  day = IsDayTime();

  (void)FillLetterBackground(4, 14, 61);
  temp_canvas->color = 0;
  SetCurrentFont(font_piznumb);
  const u8 *caption = (u16)day ? attack_action_caption : attack_weapon_caption;

  (void)DrawText2(115, 116, 87, caption);
  for (unsigned i = 0; i < 4; ++i) {
    SetTempCanvasPosition(158, (i16)(104 + 10 * i));
    u8 count = player_ptr->num_weapons[i + ((u16)day ? 0 : 4)];

    DrawNumberInsideListRight(count);
  }
  game_ptr->did_choose_tv_thingy = 10;
  for (;;) {
    u8 choice;
    choice = AskForNewspaperChoice(4, 0, choices + ((u16)day ? 0 : 4), 73, 97, 0, 162, 14);
    if ((i8)choice >= 100)
      break;

    Player *player = player_ptr;
    u8 item = (u8)(choice + ((u16)day ? 0 : 4));
    if (!player->num_weapons[item] && !game_ptr->debug_mode)
      continue;
    if (player->num_weapons[item] || !(u16)day) {
      u32 amount = (u32)player->average_weapon_price[item];

      (void)TransferPlayerMoney(current_player_index, 10, 3, (float)amount);
      player->num_weapons[item] = (u8)((u32)(player->num_weapons[item]) - (u32)1u);
    }
    if (!(u16)day) {
      InteractiveAttackRest(rest, interactive, (u8)(item - 4u));

      LoadLangFile(&langfile_action);
      MarkGoalReached(94);
      HandleRestaurantAttackedWeapon(target, item);
      break;
    }
    HandleRestaurantAttackedStink(target, item);
    break;
  }
  FreeLangFile(&langfile_action);
}
