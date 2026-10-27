// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
// Character loader.

#include "objects.h"
#include "files.h"
#include "graphics.h"
#include "game.h"
#include "character.h"
#include "text.h"

static i16 add_word(i16 a, i32 b) {
  return (i16)((u16)a + (u16)b);
}
void pt_character_filename(u8 index, u8 name[16]) {
  unsigned i = 5;
  memcpy(name, "chr\\c", 5);
  index %= 100;
  if (index >= 10)
    name[i++] = (u8)('0' + index / 10);
  name[i++] = (u8)('0' + index % 10);
  memcpy(name + i, "e.pc", 5);
}

// Retained no-op used by the language-file lifecycle.

void DummyFunc1(const u8 *unused_name) {
  (void)unused_name;
}
void FreeLangFile(LangFileDesc *descriptor) {

  if (descriptor) {

    DummyFunc1(descriptor->name);

    game_ptr->language = NULL;
    LangFileDesc *desc = descriptor;
    if (desc->num_entries && desc->buffer) {

      pt_buffer_release(desc->buffer);

      desc->num_entries = 0;
      desc->buffer = NULL;
    }
  }
}
Game *LoadLangFile(LangFileDesc *descriptor) {
  LangFileDesc *previous = game_ptr->language;
  LangFileDesc *desc = descriptor;
  if (previous && !(previous == descriptor && desc->num_entries)) {

    FreeLangFile(previous);
  }
  if (!desc->num_entries) {

    strbuf[0] = 't';
    strbuf[1] = '\\';
    pt_strcpy(strbuf + 2, descriptor->name);

    OpenFileAsCurrent(strbuf);

    desc->buffer = (u8 *)AllocateBufferOrMessage((u16)desc->size);

    if (desc->buffer) {

      u16 count = (u16)ReadFile(cur_loading_filehandle, desc->buffer, (u16)desc->size);

      u8 *data = desc->buffer;
      for (u32 i = 0; i < count; ++i)
        if (data[i] == 10) {
          data[i] = 0;
          desc->num_entries = (u8)(desc->num_entries + (1u));
        }
    }

    CloseCurrentFile();
  }
  game_ptr->language = descriptor;

  return game_ptr;
}
u8 *GetTranslatedString(u8 index) {
  LangFileDesc *active = game_ptr->language;
  u8 *result;
  if (!active || index > active->num_entries)
    result = translation_error_text;
  else if (!index)
    result = empty_translation;
  else {
    result = active->buffer;
    while (index > 1) {
      result = pt_next_language_text(result);
      --index;
    }
  }

  return result;
}
u32 LoadBitmapFromCharacterFile(void) {

  Bitmap source;
  InitializeBitmap(&source, 52, 59);

  pt_bitmap_bind_pixels(&source, &loaded_character.pixels[0][0], sizeof(loaded_character.pixels));

  u32 result = BitBlt(&source, 0, 0, bmp_character_face, 0, 0, 52, 59);

  return result;
}
// Preserve byte/word truncation before signed character-property arithmetic.

u32 LoadCharacter(u8 index, u8 name_override) {

  u8 role = staff_is_type[index];
  if (role > 2)
    role = (u8)(role - 3);
  u8 name[16];
  pt_character_filename(index, name);

  memcpy(strbuf, name, strlen((const char *)name) + 1);

  OpenFileAsCurrent(strbuf);

  ReadBytesFromCurrentFile((&pt_native_globals.character_file), sizeof(PTCharacterFileRecord));

  CloseCurrentFile();

  if (index >= 100) {
    if (name_override) {

      LoadLangFile(&langfile_double);

      for (u32 n = 1; n <= 2; ++n) {

        u8 *name_pointer = GetTranslatedString((u8)(2u * (index % 100u) + n));

        pt_strcpy(n == 1 ? temp_character.first_name : temp_character.last_name, name_pointer);
      }

      FreeLangFile(&langfile_double);
    }
    i32 capacity = (u16)temp_character.cooking_or_serving_capacity + 2 * (u8)temp_character.last_name[0] - 156;

    temp_character.cooking_or_serving_capacity = (i16)Clamp(capacity, 50, 350);

    u8 letter = temp_character.last_name[1];
    if (letter > 'z')
      letter = 'l';
    if (letter < 'a')
      letter = 'a';
    temp_character.age = (u8)(temp_character.age + ((letter - 'a') >> 2));
    for (u32 i = 0; i < 13; ++i) {

      u32 length = pt_strlen(temp_character.first_name);

      // A one-character first name traps at the stock unsigned DIV.
      // The separately labelled repair is handled in the guard.

      u32 denominator = length - 1u;
      if (!denominator)
        denominator = 1;
      letter = temp_character.first_name[1u + i % denominator];
      if (letter > 'z')
        letter = 's';
      if (letter < 'a')
        letter = 'a';
      u8 delta = (u8)(2 * ((i32)letter - 'k')), value = temp_character.prop[i + 1];
      if ((i32)value + (i8)delta < 0)
        delta = (u8)(0u - delta);
      if ((i32)value + (i8)delta > 100)
        delta = (u8)(0u - delta);

      temp_character.prop[i + 1] = (u8)Clamp((i32)value + (i8)delta, 0, 100);
    }
    u8 *chosen = pt_character_property(&temp_character, 12u + role);
    for (u32 i = 0; i < 3; ++i) {
      u32 candidate = (u32)temp_character.prop[12u + i] + 2u;
      if ((*chosen) > candidate)
        candidate = (*chosen);
      (*chosen) = (u8)candidate;
    }
    if ((*chosen) > 100)
      (*chosen) = 100;
  }

  LoadBitmapFromCharacterFile();

  return 1;
}
u32 PaintCharacterFace(u8 index, u8 background, i16 x, i16 y) {

  LoadCharacter(index, 0);

  i32 inset = 0;
  if (background) {

    FillRect(background, (u16)x, (u16)y, (u16)add_word(x, 58), (u16)add_word(y, 65));

    for (i32 i = 1; i <= 2; ++i) {

      temp_canvas->color = (u8)(10 + i);

      SetTempCanvasPosition(add_word(x, -i), add_word(y, i));

      DefaultLineTo((u16)add_word(x, -i), (u16)add_word(y, 65 + i));

      DefaultLineTo((u16)add_word(x, 58 - i), (u16)add_word(y, 65 + i));
    }
    inset = 3;
  }

  u32 result =
      BitBlt(bmp_character_face, 0, 0, current_bitmap, (u16)add_word(x, inset), (u16)add_word(y, inset), 52, 59);

  return result;
}

// Character menu.

#include <assert.h>
#include "menu.h"
#include "world.h"
#include "missions.h"
#include "player.h"
#include "staff.h"
#include "reports.h"
#include "input.h"
#include "calendar.h"
#include <stdio.h>

// Names and start time belong to the cached Character slot.

void DrawInitialCharacterUi(u8 player) {
  const Character *c = &characters[player];

  FillLetterBackground(4, 33, 36);
  SetCurrentFont(font_astandar);
  temp_canvas->color = 16;
  (void)DrawText2(48, 140, 54, c->first_name);
  (void)DrawText2(54, 140, 72, c->last_name);
  SetCurrentFont(font_piznumb);
  temp_canvas->color = 0;
  (void)SetTempCanvasPosition(69, 143);
  draw_list_elem_style = 0;

  (void)DrawTextInsideList(character_mode_names[game_ptr->is_mission_mode]);
  cur_list_elem_x = 140;
  DrawNumberInsideListRight(pt_signed_dword(game_ptr->num_interactive_players));
  (void)SetTempCanvasPosition(69, 154);
  (void)DrawTextInsideList(character_initial_labels.score);
  cur_list_elem_x = 140;
  DrawNumberInsideListRight(pt_signed_dword((u32)c->score));
  (void)SetTempCanvasPosition(72, 165);
  DrawListElement(character_initial_labels.start);
  (void)SetTempCanvasPosition(85, 176);
  snprintf((char *)strbuf, sizeof(strbuf), (const char *)character_initial_labels.clock_format, c->start_of_day / 6u);
  DrawStringBufferAsListElement();
  (void)SetTempCanvasPosition(70, 187);
  Canvas_DrawText(temp_canvas, pt_text_span(character_initial_labels.stars, ptr_apple->difficulty_level));
  DrawBob(&bob_stanbobs, 39, 139, 156);
  DrawBob(&bob_stanbobs, 40, 139, 171);
}
// 19404 saves three registers. Mode offsets address physical guest tables;
// the history scan uses a wrapping BYTE, including rows past entry 99.

void DrawCurriculumEntry(u8 player, u8 entry, u8 target_mode) {
  u8 percentage = 0;
  game_ptr->list_element_spacing_2 = 1;
  if (entry < 100) {
    assert(target_mode <= 1);
    const PTMissionGoals *goal = target_mode ? pt_goal_target_at(player, entry) : pt_goal_history_at(player, entry);

    if (goal->year[entry]) {
      SetCurrentFont(font_piznumb);
      temp_canvas->color = 0;
      (void)SetTempCanvasPosition(49, (i16)(20u * (entry % 6u) + 60u));
      u8 day = goal->day[entry];
      u8 month = goal->month[entry];
      snprintf((char *)strbuf, sizeof(strbuf), "%u-%u-%u", day, month, goal->year[entry] % 100u);
      DrawStringBufferAsListElement();
      SetTempCanvasPositionX(90);
      const u8 *label;
      label = GetNameOfGoal(goal->kind[entry]);
      DrawListElement(label);
      if (target_mode) {
        for (u8 i = 0;; ++i) {
          if (!pt_goal_history_at(player, i)->year[i % 100u])
            break;
          if (pt_goal_history_at(player, i)->kind[i % 100u] == goal->kind[entry])
            percentage = ComputeMissionGoalPercentage(current_player_index, i, entry);
          if (i == 255)
            abort();
        }
        (void)SetTempCanvasPosition(70, (i16)(20u * (entry % 6u) + 69u));
        temp_canvas->color = 10;
        DrawPercentageNumberInList(percentage);
      }
    }
  }
}
// 19729 always displays the current player's history, regardless of the
// player selected in the surrounding character menu.

void DrawCurriculumOnLetter(u8 target_mode) {
  SetCurrentFont(font_astandar);
  FillLetterBackground(4, 32, 36);
  if (target_mode) {
    (void)SetTempCanvasPosition(34, 40);
    draw_list_elem_style = 0;
    DrawListElement(target_curriculum_heading);
    cur_list_elem_y = (i16)((u16)(cur_list_elem_y + 20u));
    if (!game_ptr->is_mission_mode) {
      DrawListElement(curriculum_mission_only);

      return;
    }
  } else {
    (void)DrawText2(26, 195, 28, curriculum_heading);
    const Character *c = character_ptr;
    snprintf((char *)strbuf, sizeof(strbuf), "%s %s", (const char *)c->first_name, (const char *)c->last_name);
    (void)DrawText2(26, 195, 46, strbuf);
  }
  for (unsigned i = 0; i < 6; ++i)
    DrawCurriculumEntry(current_player_index, (u8)(6u * (u8)curriculum_page + i), target_mode);
}
// Decrement the caller's tier before drawing. Callers ignore the no-award result.

void DrawAwardOnLetter(u8 category, u8 tier, u8 position_index) {
  if (position_index <= 8 && tier) {
    --tier;

    (void)BitBlt(&bmp_awards,
        (u16)(41u * category),
        (u16)(51u * tier),
        current_bitmap,
        (u16)(43u * (position_index % 3u) + 60u),
        (u16)(52u * (position_index / 3u) + 62u),
        40,
        50);
  }
}

static void rank_bar(u16 value, u16 left, u16 full) {
  u8 rank;
  u16 lower, upper;
  rank = GetRankIndexFromValue(value);

  lower = GetRankValueFromIndex((u8)(rank - 1u));
  upper = GetRankValueFromIndex(rank);

  u16 right = full;
  if (rank && rank != 13) {
    assert(upper != lower);
    right = (u16)(69 * ((i32)value - lower) / ((i32)upper - lower) + left);
  }
  FillRect(28, left, 170, right, 181);

  temp_canvas->color = 0;
  if (left == 121) {
    (void)SetTempCanvasPosition(125, 177);

    const u8 *label = GetTranslatedString((u8)(rank + 1u));
    DrawClippedListElement(label, 16);

  } else {
    const u8 *label = GetTranslatedString((u8)(rank + 15u));
    (void)DrawText(45, 177, label);
  }
}
// Render rank words and the shifting award value.

u32 DrawOnCharacterLetter(u8 player, u8 mode, u8 extra) {
  u32 award_bits;

  const Character *c = &characters[player];
  const Player *p = &players[player];
  if (mode == 3 || mode == 6) {
    if (6u * extra >= 100) {
      return 0;
    }
    character_curriculum_flag = 1;
  }
  SetCurrentBitmap(old_bitmap);
  switch (mode) {
  case 0:
    DrawInitialCharacterUi(player);
    break;
  case 1: {
    FillLetterBackground(4, 32, 36);
    PaintCharacterFace(p->character_index, 15, 33, 22);
    temp_canvas->color = 0;
    SetCurrentFont(font_piznumb);
    (void)DrawText(100, 27, character_value_labels.values);
    snprintf((char *)strbuf, sizeof(strbuf), "%s %s", (const char *)c->first_name, (const char *)c->last_name);
    (void)DrawText(100, 37, strbuf);
    (void)SetTempCanvasPosition(112, 48);
    DrawNumberInsideListRight(pt_signed_dword(c->age));
    (void)DrawText(120, 48, character_value_labels.years);
    cur_list_elem_x = 60;
    cur_list_elem_y = 106;
    player_stats_value_x = 180;
    game_ptr->list_element_spacing_2 = 0;
    static const u8 *const labels[] = {character_value_labels.health,
      character_value_labels.energy,
      character_value_labels.popularity,
      character_value_labels.cooking,
      character_value_labels.selling,
      character_value_labels.management,
      character_value_labels.personality,
      character_value_labels.intelligence,
      character_value_labels.charisma,
      character_value_labels.courage,
      character_value_labels.experience};
    static const unsigned props[] = {PT_CHARACTER_HEALTH,
      PT_CHARACTER_ENERGY,
      PT_CHARACTER_POPULARITY,
      PT_CHARACTER_COOKING,
      PT_CHARACTER_SELLING,
      PT_CHARACTER_MANAGEMENT,
      PT_CHARACTER_PERSONALITY,
      PT_CHARACTER_INTELLIGENCE,
      PT_CHARACTER_CHARISMA,
      PT_CHARACTER_COURAGE,
      PT_CHARACTER_EXPERIENCE};
    for (unsigned i = 0; i < 11; ++i) {
      DrawPlayerStatsListEntry(labels[i], c->prop[props[i]]);
    }
    break;
  }
  case 2:
    FillLetterBackground(4, 32, 36);
    temp_canvas->color = 0;
    SetCurrentFont(font_piznumb);
    (void)DrawText2(26, 195, 28, character_company_heading);
    (void)DrawText2(26, 195, 38, pt_player(player)->name_text);
    snprintf((char *)strbuf, sizeof(strbuf), "Owner: %s %s", (const char *)c->first_name, (const char *)c->last_name);
    (void)SetTempCanvasPosition(36, 48);
    DrawStringBufferAsListElement();
    cur_list_elem_y = 70;
    cur_list_elem_x = 136;
    DrawListElement(character_company_profit);
    // TempCanvasDrawLine replaces zero destination coordinates with
    // the source coordinate and tail-calls DefaultLineTo.

    TempCanvasDrawLine(36, 57, 185, 0);
    for (unsigned i = 0; i < 10; ++i) {
      (void)SetTempCanvasPosition(60, (i16)(9u * i + 90));
      DrawListElement(town_names[i]);
      if (ptr_apple->is_town_inited[i]) {
        cur_list_elem_x = 184;
        DrawDollarsInsideList(p->weekly_profit_by_town[i]);
      } else {
        cur_list_elem_x = 116;
        DrawListElement(pizza_menu_info_separator);
      }
    }
    break;
  case 3:
  case 6:
    DrawCurriculumOnLetter((u8)(mode / 6u));
    break;
  case 4:
    FillLetterBackground(4, 34, 36);
    SetCurrentFont(font_supersma);
    rank_bar(c->social_status, 121, 189);
    rank_bar(c->underworld_status, 39, 107);
    break;
  case 5:
    if (!loaded_awards_bitmap) {
      LoadBackground(character_awards_filename, &bmp_awards);
      game_ptr->last_loaded_blitmap_index = 100;
      loaded_awards_bitmap = 1;
    }
    award_bits = (u32)c->awards;
    FillLetterBackground(4, 35, 36);
    SetCurrentFont(font_ruby);
    temp_canvas->color = 16;
    (void)DrawText2(12, 203, 40, character_awards_heading);
    for (unsigned j = 0; j < 6; ++j) {
      DrawAwardOnLetter((u8)j, (u8)(award_bits & 3u), (u8)j);

      award_bits = award_bits >> 2;
    }
    break;
  default:
    break;
  }
  DrawLetterOnBitmap(current_bitmap);
  SetCurrentBitmap(old_bitmap);

  return 1;
}
u32 ScrollInLetter(void) {
  u8 delay_ticks, step_index;
  CopyBitmap2to1();
  step_index = 0;
  for (;;) {
    u8 i = step_index;

    assert(i < 20);
    u8 y = staff_ui_file_animation[i][0];
    if (!y) {
      return 2u * i;
    }
    delay_ticks = staff_ui_file_animation[i][1];
    SetCurrentBitmap(old_bitmap);
    (void)BitBlt(game_ptr->background, 14, 0, current_bitmap, 26, (u16)(y + 14u), 170, (u16)(176u - y));
    if (y < 67)
      (void)BitBlt(game_ptr->background, 0, 109, current_bitmap, 12, (u16)(y + 123u), 15, (u16)(67u - y));
    CommitBitmapAndSetPalette(0);
    if (delay_ticks)
      Delay(delay_ticks);
    ++step_index;
  }
}
u32 ScrollOutLetter(void) {
  u8 delay_ticks, step_index;
  CopyBitmap2to1();
  step_index = 18;
  for (; step_index; --step_index) {
    u8 i = step_index, y = staff_ui_file_animation[i - 1u][0];
    delay_ticks = (u8)(staff_ui_file_animation[i][1] - 1u);
    if (y < 176)
      (void)BitBlt(current_bitmap,
          12,
          (u16)(staff_ui_file_animation[i][0] + 14u),
          old_bitmap,
          12,
          (u16)(y + 14u),
          184,
          (u16)(176u - y));
    SetCurrentBitmap(old_bitmap);
    (void)BitBlt(game_ptr->background, 0, 0, current_bitmap, 12, 14, 14, (u16)(y + 109u > 176 ? 176 : y + 109u));
    (void)BitBlt(game_ptr->background, 14, 0, current_bitmap, 26, 14, 178, y);
    CommitBitmapAndSetPalette(0);
    if (delay_ticks)
      Delay(delay_ticks);
  }
  u32 result = CopyBmp6AToHere(current_bitmap);

  return result;
}

void HandleCharacterMenu(void) {
  u8 page_offset, page_mode;
  u8 clicked = 0, player = current_player_index;

  page_offset = page_mode = 0;
  LoadLangFile(&langfile_leben);
  Do3x3ButtonStuff(18);
  (void)DrawOnCharacterLetter(player, 0, 0);
  CopyBmp6AToHere(current_bitmap);
  for (;;) {
    u16 event = WaitForMouseClick();
    mouse_pressed = event;
    if (event == 1) {
      if (clicked) {
        (void)DrawOnCharacterLetter(player, 0, 0);
        (void)ScrollOutLetter();
        clicked = page_mode = 0;
        continue;
      }
      if (player != current_player_index)
        continue;
      if ((IsMouseInside(177, 109, 184, 115) != 0)) {
        u8 yes = ShowMessage2(0, character_holiday_question);
        if (yes) {
          unit_for_number_query = character_holiday_days;
          i32 days = ShowAskForNumberQuery(character_holiday_prompt, 7, 1, 2, 28);
          players[player].days_away = (u8)days;
          game_ptr->mask_of_players = (u8)((u32)game_ptr->mask_of_players & (u32)((u8) ~(1u << player)));
          (void)DoEndTurn();

          return;
        }
      }
      Character *c = character_ptr;
      if ((IsMouseInside(139, 156, 144, 164) != 0)) {
        if (c->start_of_day < 54)
          c->start_of_day = (u8)(c->start_of_day + 6u);
        DrawInitialCharacterUi(player);
      }
      if ((IsMouseInside(139, 171, 144, 180) != 0)) {
        if (c->start_of_day)
          c->start_of_day = (u8)(c->start_of_day - 6u);
        DrawInitialCharacterUi(player);
      }
      continue;
    }
    if (!pt_is_menu_button_event(event))
      continue;
    u8 old = clicked;
    clicked = (u8)(event - PT_MENU_BUTTON_1 + 1);
    if (clicked < 7)
      page_mode = 0;
    if (event == PT_MENU_BUTTON_9) {
      if (loaded_awards_bitmap) {
        LoadBlitmapAndOffsets(0);
        loaded_awards_bitmap = 0;
      }
      FreeLangFile(&langfile_leben);

      return;
    }
    if (event == PT_MENU_BUTTON_3 || event == PT_MENU_BUTTON_6) {
      if (player != current_player_index) {
        clicked = old;
        u8 *translated;
        translated = GetTranslatedString(29);
        ShowMessage(15, translated);
        continue;
      }
      page_mode = (u8)(event == PT_MENU_BUTTON_3 ? 1 : 2);
    }
    if (event == PT_MENU_BUTTON_7 || event == PT_MENU_BUTTON_8) {
      clicked = old;
      if (page_mode) {
        if (event == PT_MENU_BUTTON_7) {
          if (!curriculum_page)
            continue;
          --curriculum_page;
        } else {
          if (curriculum_page >= 16)
            continue;
          ++curriculum_page;
        }
        old = 100;
      } else {
        if (event == PT_MENU_BUTTON_7 && player) {
          --player;
          old = 100;
        }
        if (event == PT_MENU_BUTTON_8 && player < 7) {
          ++player;
          old = 100;
        }
        if (old == 100 && !clicked) {
          DrawInitialCharacterUi(player);
          continue;
        }
      }
    }
    if (clicked != old) {
      if (old) {
        (void)DrawOnCharacterLetter(player, 0, 0);
        (void)ScrollOutLetter();
      }
      if (clicked) {
        u32 result = DrawOnCharacterLetter(player, clicked, page_offset);
        if ((u16)result)
          (void)ScrollInLetter();
      }
    }
  }
}

// Guest dialogue.

#include "restaurant.h"
static u8 *comment(u8 i) {
  u8 *p = GetTranslatedString(i);
  return pt_strcpy(strbuf, p);
}
static u16 blit(u16 sx, u16 sy, u16 dx, u16 dy, u16 w, u16 h) {
  return (u16)BitBlt(bmp22_ptr, sx, sy, current_bitmap, dx, dy, w, h);
}
void DrawGuestEatingFrame_Helper1(u8 portrait) {
  (void)snprintf((char *)strbuf, sizeof(strbuf), "gfx\\g%u.vga", portrait);
  LoadBackground(strbuf, bmp22_ptr);
  game_ptr->byteAA = portrait;
}
void DrawGuestEatingFrame_Helper2(void) {
  (void)blit(0, 0, (u16)guest_portrait_x, (u16)guest_portrait_y, 38, 38);
}
u16 ShowGuestBlinkEyesInner(u8 frame) {
  return blit(frame ? (u16)(21u * (u32)(frame - 1u)) : 9,
      frame ? 38 : 10,
      (u16)((u16)guest_portrait_x + 9u),
      (u16)((u16)guest_portrait_y + 10u),
      21,
      8);
}
u16 ShowGuestEatingFrame(u8 frame) {
  return blit(frame ? 38 : 6,
      frame ? (u16)(19u * (u32)(frame - 1u)) : 19,
      (u16)((u16)guest_portrait_x + 6u),
      (u16)((u16)guest_portrait_y + 19u),
      26,
      19);
}
void ShowGuestEatingAnimation(u8 cycles) {
  do {
    ShowGuestEatingFrame(1);
    Delay(3);
    ShowGuestEatingFrame(2);
    Delay(4);
    ShowGuestEatingFrame(1);
    Delay(3);
    ShowGuestEatingFrame(0);
    Delay(7);
    --cycles;
  } while (cycles);
}
void ShowGuestBlinkEyes(void) {
  ShowGuestBlinkEyesInner(1);
  Delay(5);
  ShowGuestBlinkEyesInner(0);
}
void DrawGuestEatingFrame(u8 portrait) {
  guest_portrait_x = 38;
  guest_portrait_y = 38;
  DrawGuestEatingFrame_Helper1(portrait);
  DrawGuestEatingFrame_Helper3(30, 30, 180, 130);
  DrawGuestEatingFrame_Helper2();
  Delay(10);
  // Stock falls through into ShowGuestBlinkEyes: no extra CALL frame.

  ShowGuestBlinkEyes();
}
PTActionResult PrintGuestEating(Guest *guest) {
  u8 variant = (u8)RandomNumber(4) >> 1;
  Rest *rest = curr_rest;
  u8 index = (u8)RandomNumber(rest->num_recipes), type = guest->guest_type % 6u;

  const RestoRecipe *item = pt_menu_recipe_row(rest, index);
  const Recipe *selected_recipe = item->recipe;

  const Poi *poi = rest->property;
  Zone *district = FindZoneFromCoordinates(poi->xcoord, poi->ycoord);
  u8 text = 0;
  bool formatted = false;
  if (guest->has_commented)
    text = 57;
  else {
    guest->has_commented = 1;
    if (RandomChance(10))
      text = 56;
    else if (!guest->eating_or_waiting || !rest->num_recipes) {
      if (RandomChance(50)) {
        u8 rating = rest->furniture_rating[type];
        if (rating < 5)
          text = 37;
        else
          text = (u8)Clamp(36 - (rating >> 4), 31, 36);
      } else if (rest->jukebox_selection && RandomChance(20) &&
          (rest->jukebox_selection - 1 == type || abs((int)rest->jukebox_selection - 1 - type) > 1))
        text = rest->jukebox_selection - 1 == type ? 44 : 45;
      else
        text = (u8)(rest->average_waiter_rating / 35u + 35u);
    } else {
      u32 subject = RandomNumber(3);
      formatted = true;
      if (!subject) {
        const Recipe *recipe = selected_recipe;
        u8 rating = recipe->age_type_rating[type] / 19u;
        if (rating >= 4)
          rating = 4;
        text = (u8)(variant + 9u - 2u * rating);
      } else if (subject == 1) {
        const Zone *zone = district;
        i16 price = (i16)((u32)(u16)item->price_per_pizza - (900u - 6u * zone->how_low_income));
        if (price < 0)
          price = 0;
        text = (u8)(price / 300 + 11);
        if ((i8)text > 14)
          text = 14;
        if (variant)
          text = (u8)(text + 4u);
      } else if (subject == 2) {
        const Zone *zone = district;
        int quality = (int)item->sum_of_qualities - (zone->byte3A >> 2);
        if (quality < 0)
          quality = 0;
        quality /= 14;
        if (quality > 5)
          quality = 5;
        text = (u8)(2 * (5 - quality) + variant + 19);
      } else
        abort();
    }
  }
  PTActionResult result;
  if (formatted) {
    u8 *p = GetTranslatedString(text);

    result = pt_action_value((u32)snprintf(
        (char *)strbuf, sizeof(strbuf), "I am eating %s. %s", (const char *)selected_recipe->name, (const char *)p));
  } else
    result = pt_action_object(comment(text));
  return result;
}
u32 HandleClickOnGuest(Rest *rest, Guest *guest) {
  u8 type = guest->guest_type % 6u;
  if (!rest) {
    u32 result = guest->guest_type / 6u;
    return result;
  }
  game_ptr->in_menu_hierarchy = 0;
  restaurant_hit_test_owner = rest;
  u8 portrait = guest->portrait_index;
  LangFileDesc *language = type / 3u ? &langfile_gtalk1 : &langfile_gtalk0;
  LoadLangFile(language);
  CopyBitmap2to1();
  DrawGuestEatingFrame(portrait);
  PrintGuestEating(guest);
  SetCurrentFont(font_piznumb);
  game_ptr->list_element_spacing_2 = 0;
  game_ptr->paragraph_indent_y_threshold = 85;
  Canvas_TextRect(temp_canvas, 9, 15, strbuf, 80, 40, 174, 46);
  game_ptr->paragraph_indent_y_threshold = 0;
  ShowGuestEatingAnimation(5);
  u16 skip = WantSkipGameIntro();
  if (!skip) {
    Delay(30);
    ShowGuestBlinkEyes();
    WaitForMouseClick2();
  }
  ShowGuestBlinkEyes();
  Copy320x200Bitmap(old_bitmap, current_bitmap);
  FreeLangFile(language);
  game_ptr->in_menu_hierarchy = 2;
  return 0;
}

// Guest walking.

#include "arithmetic.h"
#include "state_types.h"
#include "frame.h"
#include "messages.h"
#include <math.h>
#include "finance.h"

typedef struct GuestPopulationSearch {
  u8 furniture_index;
  u8 eating_chance;
} GuestPopulationSearch;
typedef struct GuestWalkDrawing {
  u32 row_offset;
  u8 carrying_in_front;
} GuestWalkDrawing;
typedef struct GuestActionSearch {
  u8 candidate_index;
  u8 guest_index;
} GuestActionSearch;

static const BobHeader *furniture_header(const Furniture *f) {
  const FurnitureDesc *d = pt_furniture_descriptor(f->type);
  return pt_resource_header_at(&bob_restaurant, d->images[(u32)f->flags & 3u]);
}

void DummyProc1(void) {}
u8 GuestOppositeDirection(u8 direction) {
  return (u8)((direction + 2u) & 3u);
}
// Its first argument is unused. Each distance/penalty wraps as a WORD before signed

u8 GuestChooseLegacyDirection(Rest *rest, Pig *pig, u8 avoid_first, u8 avoid_second) {
  (void)rest;

  // Original bug: sentinel 4 indexes beyond the four score WORDs.
  // Accept it only with the labelled original bounds repair installed.

  u16 scores[4];
  u8 result = GuestOppositeDirection((*pt_pig_path_direction(pig)));
  u16 dx = (u16)((u16)pig->x_coord - (u16)pig->x);
  u16 dy = (u16)((u16)pig->y_coord - (u16)pig->y);
  scores[0] = dy;
  scores[1] = (u16)(0u - dx);
  scores[2] = (u16)(0u - dy);
  scores[3] = dx;
  if (avoid_first < 4)
    scores[avoid_first] = (u16)(scores[avoid_first] - 800u);
  if (avoid_second < 4)
    scores[avoid_second] = (u16)(scores[avoid_second] - 700u);
  i16 best = -900;
  for (u8 i = 0; i < 4; ++i) {
    i16 score = (i16)scores[i];
    if (score > best) {
      best = score;
      result = i;
    }
  }

  return result;
}
// Try clockwise, then counterclockwise, then reverse. The two chosen BYTEs occupy separate
// DWORD slots in its private frame.

u8 GuestChooseLegacyDetour(Pig *pig) {
  u8 candidate, hit;
  candidate = InteractiveAttackRest_Plus1And3((*pt_pig_path_direction(pig)));

  hit = InteractiveAttackRest_Move_Helper1(restaurant_hit_test_owner, pig, candidate);
  if (hit == UINT8_MAX) {
    return candidate;
  }
  candidate = GuestTurnCounterclockwise((*pt_pig_path_direction(pig)));

  hit = InteractiveAttackRest_Move_Helper1(restaurant_hit_test_owner, pig, candidate);
  if (hit == UINT8_MAX) {
    return candidate;
  }
  candidate = GuestOppositeDirection((*pt_pig_path_direction(pig)));

  return candidate;
}
// Finish an exit action or target the next doorway.

u8 GuestAdvanceLegacyAction(Pig *pig) {
  pig->byteE = 0;
  if (pig->byte7 == 2 || pig->byte7 == 4) {
    state_slot_008 = 0;

    return 0;
  }
  ++pig->byte7;

  Rest *r = curr_rest;

  const Poi *poi = r->property;
  pig->x_coord = (i16)((u16)(12u * poi->door_x_start + 30u));
  pig->y_coord = 250;
  if (pig->byte7 == 4) {
    pig->x_coord = 32;
    pig->y_coord = (i16)((u16)(11 * (poi->left_door_y_end + 23 - poi->size_y) + 30));
  }
  pig->path_directions[0] = GuestChooseLegacyDirection(r, pig, 4, 4);
  pig->path_depth = 0;

  return 1;
}
// React to furniture or a wall, retaining direction history.

u8 GuestHandleLegacyCollision(Pig *pig, u8 furniture_index) {
  u8 current = (*pt_pig_path_direction(pig)), candidate, found;

  if (furniture_index >= 200) {
    if (pig->path_depth) {
      candidate = GuestOppositeDirection(current);
      (*pt_pig_path_direction(pig)) = candidate;
    } else {
      candidate = GuestChooseLegacyDirection(curr_rest, pig, pig->path_directions[0], 4);
      pig->path_directions[0] = candidate;
    }
    goto done;
  }
  found = FindGuestForFurniture(furniture_index);

  if (pig->byte7 == 1) {
    Furniture *item = pt_rest_furniture_view(restaurant_hit_test_owner, furniture_index, false);

    const FurnitureDesc *desc = pt_furniture_descriptor(item->type);
    if (desc->furn_type == 2 && found == 255) {
      pig->field_2B = furniture_index;
      (void)RestDrawOnScreen_Helper2(pig);
      goto done;
    }
  }
  if (pig->byte7 == 3 && found != 255 && !(&guests[found])->eating_or_waiting) {
    pig->gap2C[0] = found;
    (void)RestDrawOnScreen_Helper2(pig);
    goto done;
  }
  if (pig->path_depth) {
    u8 clockwise = RandomChance(50);
    if (clockwise)
      candidate = InteractiveAttackRest_Plus1And3(current);
    else
      candidate = GuestTurnCounterclockwise(current);

    if (candidate == (*pt_pig_previous_direction(pig)))
      candidate = GuestOppositeDirection(candidate);
  } else {
    u8 opposite = GuestOppositeDirection((*pt_pig_path_direction(pig)));
    candidate = GuestChooseLegacyDirection(curr_rest, pig, (*pt_pig_path_direction(pig)), opposite);
  }

  ++pig->path_depth;
  u8 hit = InteractiveAttackRest_Move_Helper1(curr_rest, pig, candidate);
  if (hit != 255)
    candidate = GuestOppositeDirection(candidate);

  (*pt_pig_path_direction(pig)) = candidate;
  (*pt_pig_path_steps(pig)) = 0;
  if (found != 255) {
    Guest *g = (&guests[found]);
    if (g->guest_type / 2u == pig->byte4 / 2u) {
      g->byteD = 16;
      g->byteE = GuestOppositeDirection((*pt_pig_previous_direction(pig)));
      pig->byte22 = 10;
    }
  }
done:

  return 0;
}
i32 DivideBy6(i16 value) {
  i32 r = value / 6;

  return r;
}
i32 DivideBy6_(i16 value) {
  i32 r = value / 6;

  return r;
}
u32 ResetFloormapNonzeroToOne(void) {
  for (unsigned y = 0; y < 46; ++y)
    for (unsigned x = 0; x < 46; ++x)
      if (rest_floormap[y][x])
        rest_floormap[y][x] = 1;
  rest_path_depth = 0;

  return 2115;
}
void RestInitFloormap(Rest *r) {
  memset(rest_floormap, 0, sizeof(rest_floormap));

  for (unsigned y = 0; y < 46; ++y) {
    for (unsigned x = 0; x < 46; ++x) {
      u8 hit = FindFurnitureAtPoint(r, (i16)(6 * x), (i16)(6 * y));
      if (hit == 255)
        rest_floormap[y][x] = 1;
    }
  }
  rest_path_depth = 0;
}
u32 SetFloormapForFurniture(Furniture *item) {
  u32 result = 0;
  for (unsigned y = 0; y < 46; ++y) {
    for (unsigned x = 0; x < 46; ++x) {
      bool hit;
      hit = IsPointInFurniture(item, (i16)(6 * x + 3), (i16)(6 * y + 3));
      result = hit;
      if (hit) {
        result = 46 * y;
        rest_floormap[y][x] = 1;
      }
    }
  }

  return result;
}
void ClearFloormapForFurniture(Furniture *item) {
  for (unsigned y = 0; y < 46; ++y) {
    for (unsigned x = 0; x < 46; ++x) {
      bool hit;
      hit = IsPointInFurniture(item, (i16)(6 * x + 3), (i16)(6 * y + 3));
      if (hit)
        rest_floormap[y][x] = 0;
    }
  }
}
void GuestHelper8(u8 result[4], u8 x, u8 y) {
  u16 scores[4];
  scores[0] = (u16)((int)rest_path_target_y - y);
  scores[1] = (u16)((int)x - rest_path_target_x);
  scores[2] = (u16)((int)y - rest_path_target_y);
  scores[3] = (u16)((int)rest_path_target_x - x);
  for (unsigned i = 0; i < 4; ++i) {
    i16 best = -200;
    u8 choice = 0;
    for (unsigned j = 0; j < 4; ++j) {
      i16 value = (i16)scores[j];
      if (best <= value) {
        best = value;
        choice = (u8)j;
      }
    }
    scores[choice] = (u16)-200;
    result[i] = choice;
  }
}
typedef struct GuestPathSearch {
  u8 directions[4];
  u8 found;
} GuestPathSearch;
u8 GuestHelper7(u8 x, u8 y) {
  GuestPathSearch storage;
  GuestPathSearch *search = &storage;
  search->found = 0;
  if (x >= 46 || y >= 46) {
    return 0;
  }
  if ((rest_floormap[y][x] & 2u) || !rest_floormap[y][x]) {
    DummyProc1();

    return 0;
  }
  if (x == rest_path_target_x && y == rest_path_target_y) {
    DummyProc1();

    return 1;
  }
  ++rest_path_depth;
  ++rest_path_visits;
  if (rest_path_depth <= 100 && rest_path_visits <= 300) {
    DummyProc1();
    rest_floormap[y][x] |= 2u;
    GuestHelper8(search->directions, x, y);
    for (unsigned i = 0; i < 4; ++i) {
      u8 nx = x, ny = y, dir = search->directions[i];
      switch (dir) {
      case 0:
        ++ny;
        break;
      case 1:
        --nx;
        break;
      case 2:
        --ny;
        break;
      case 3:
        ++nx;
        break;
      }
      search->found = GuestHelper7(nx, ny);
      if (search->found) {
        rest_floormap[y][x] = (u8)((4u * dir) | 0xf3u);
        break;
      }
    }
  }
  DummyProc1();
  --rest_path_depth;
  u8 result = search->found;

  return result;
}
u8 GuestHelper6(u8 x, u8 y, u8 tx, u8 ty) {
  rest_path_target_x = tx;
  rest_path_target_y = ty;
  DummyProc1();
  rest_path_depth = 0;
  rest_path_visits = 0;
  u8 result = GuestHelper7(x, y);
  return result != 0;
}
u32 GuestHelper4(Pig *p) {
  i32 ty, tx, y, x;
  ty = DivideBy6(p->y_coord);

  tx = DivideBy6_(p->x_coord);

  y = DivideBy6(p->y);

  x = DivideBy6_(p->x);
  u8 result = GuestHelper6((u8)x, (u8)y, (u8)tx, (u8)ty);

  if (!result)
    (void)ResetFloormapNonzeroToOne();

  return result != 0;
}
u16 GuestHelper5(u8 type) {
  game_ptr->last_loaded_blitmap_index = 200;
  bob_walk.num_entries = 0;
  pt_bob_format_filename(&bob_walk, "bob\\walk%u.bob", type);
  return LoadBobFile(&bob_walk);
}
u32 GuestHelper3(Rest *r, u8 type, i16 image, u32 dir, u8 action, i16 x, i16 y, i16 tx, i16 ty) {
  (void)dir;
  restaurant_hit_test_owner = r;
  walking_guest.personality_attrib = 3;
  walking_guest.byte4 = type;
  walking_guest.byte7 = action;
  walking_guest.word26 = image;
  walking_guest.x = x;
  walking_guest.y = y;
  walking_guest.x_coord = tx;
  walking_guest.y_coord = ty;
  walking_guest.path_depth = 0;
  walking_guest.byte22 = 0;
  walking_guest.byteE = 0;
  walking_guest.byteF = 0;
  walking_guest.byte21 = 0;
  if (action == 2 || action == 4) {
    u32 found = GuestHelper4(&walking_guest);
    if (!(u16)found)
      return found;
  }
  (void)GuestHelper5(type);
  state_slot_008 = 1;
  return 1;
}
// Initializes occupancy when opening a dining-room view.

u8 RestCreateAllGuests(Rest *r) {
  GuestPopulationSearch storage;
  GuestPopulationSearch *locals = &storage;
  RestInitFloormap(r);
  guest_count = 0;
  if (!pt_float_is_zero(r->average_stay_time)) {

    volatile double product = (double)r->average_eating_time * 100.0;
    volatile double quotient = product / r->average_stay_time;
    double integral = TruncateX87Integral(quotient);
    u32 value =
        isfinite(integral) && integral >= -2147483648.0 && integral < 2147483648.0 ? (u32)(i32)integral : 0x80000000u;

    locals->eating_chance = (u8)value;
  } else
    locals->eating_chance = 50;
  locals->furniture_index = 0;
  while (locals->furniture_index < r->num_furniture) {
    u8 i = locals->furniture_index;
    Furniture *item = pt_rest_furniture_view(r, i, false);

    const FurnitureDesc *desc = pt_furniture_descriptor(item->type);
    if (desc->furn_type == 2 && desc->value && item->linked_furniture != 255) {
      Furniture *linked = pt_rest_furniture_view(r, item->linked_furniture, false);

      const FurnitureDesc *other = pt_furniture_descriptor(linked->type);
      if (other->value) {
        u8 chance = RandomChance(r->demand_capacity_ratio[0]);
        if (chance) {
          u8 type;
          type = RestGetNewGuestType(r);
          RestCreateNewGuest(r, type, i);

          chance = RandomChance(locals->eating_chance);
          if (chance) {
            Guest *g = pt_guest_at((u32)guest_count - 1u);

            g->eating_or_waiting = 1;
          }
        }
      }
    }
    ++locals->furniture_index;
  }
  restaurant_with_guests = r;
  u8 result = locals->furniture_index;

  return result;
}
void RestCreateNewGuest(Rest *r, u8 type, u8 index) {
  Guest *g = (&guests[guest_count]);
  Furniture *item = pt_rest_furniture_view(r, index, false);
  unsigned dir = (u32)item->flags & 3u;
  const BobHeader *h = furniture_header(item);
  g->furniture_idx = index;
  g->flags = pt_signed_dword(((u32)g->flags & ~3u) | dir);
  g->byteE = (u8)dir;
  g->linked_furniture = item->linked_furniture;
  g->x = (i16)((u16)(item->x + (h->w >> 1) + guest_seat_x[dir]));
  g->y = (u8)(item->y + (h->h >> 1) + guest_seat_y[dir]);
  g->guest_type = type;
  g->byteC = 0;
  g->byteB = 1;
  g->byteD = 0;
  g->byte10 = 0;
  g->byte11 = 0;
  g->eating_or_waiting = 0;
  g->byteF = 0;
  g->byte13 = (u8)(2u * type);
  u8 chance = RandomChance(50);
  if (chance)
    ++g->byte13;
  ++guest_count;
}
u8 RestDrawOnScreen_Helper2(Pig *p) {
  Rest *r = restaurant_hit_test_owner;
  switch (p->byte7) {
  case 1:
    RestCreateNewGuest(r, p->byte4, p->field_2B);
    FurnitureSetFlag4(r, pt_rest_furniture_view(r, p->field_2B, false));
    state_slot_008 = 2;
    break;
  case 2: {
    const Poi *poi = r->property;
    i32 delta = 12 * poi->door_x_start + 30 - p->x;
    delta = abs(delta);
    // The final path cell spans six pixels, including a five-pixel offset.

    if (delta > 5) {
      return 2;
    }
    p->dead_unk = 1;
    p->gap6 = 20;
    p->path_directions[0] = 0;
    p->path_depth = 0;
    state_slot_008 = 3;
    break;
  }
  case 3: {
    Guest *g = (&guests[p->gap2C[0]]);
    g->eating_or_waiting = 1;
    FurnitureSetFlag4(r, pt_rest_furniture_view(r, g->furniture_idx, false));
    (void)ResetFloormapNonzeroToOne();

    const Poi *poi = r->property;
    i16 ty = (i16)((u16)(11 * (poi->left_door_y_end + 23 - poi->size_y) + 30));
    u8 opposite;

    opposite = GuestOppositeDirection((*pt_pig_path_direction(p)));
    u32 found = GuestHelper3(r, p->byte4, -1, opposite, 4, p->x, p->y, 32, ty);

    if ((u16)found) {
      p->byte22 = 8;
      state_slot_008 = 3;

      return 0;
    }
    state_slot_008 = 2;
    break;
  }
  case 4:
    p->dead_unk = 1;
    p->gap6 = 20;
    p->path_directions[0] = 1;
    p->path_depth = 0;
    state_slot_008 = 3;
    break;
  default:
    break;
  }
  RestInitFloormap(r);

  return 0;
}
void RestDrawOnScreen_Helper3(Pig *p) {
  u8 phase = attack_walk_images[p->byte5];

  SetupCanvasTempIglooLink();
  u8 dir = (*pt_pig_path_direction(p)), image = (u8)(3u * dir + phase);

  SetupCanvasTempIglooLink();
  const BobHeader *h = pt_resource_header_at(&bob_walk, image);
  SetupHazelWithBobSprite(&temp_hazel, NULL, &bob_restaurant, (u16)p->word26);

  const Rest *r = curr_rest;
  unsigned offset = 4u * phase + dir;

  temp_igloo.draw_x = (i16)((u16)(guest_carry_x[offset] + p->x - 12 * r->scrollpos_x - (h->w >> 1) - 5));
  temp_igloo.draw_y = (i16)((u16)(p->y + 11 * r->scrollpos_y - 88 - (h->h >> 1) + guest_carry_y[offset]));
  (void)Canvas_DrawAllIgloos(temp_canvas, active_palette_pointer);
}
u8 RestDrawOnScreen_Helper1(Rest *r) {
  GuestWalkDrawing storage;
  GuestWalkDrawing *locals = &storage;
  locals->carrying_in_front = 0;
  restaurant_hit_test_owner = r;
  Pig *p = &walking_guest;
  u8 countdown = (u8)(p->gap6 - 1u);
  bool move = true;
  if (p->dead_unk == 1) {
    p->gap6 = countdown;
    if (!countdown) {
      p->dead_unk = 0;
      state_slot_008 = 0;

      return 0;
    }
  } else if (p->dead_unk == 2) {
    p->gap6 = countdown;
    if (!countdown) {
      p->dead_unk = 0;
      u32 found = GuestHelper4(p);
      if (!(u16)found) {
        p->dead_unk = 1;
        p->gap6 = 20;
        p->path_directions[0] = GuestOppositeDirection(p->path_directions[0]);
      }
      move = false;
    }
  } else if (p->byte22) {
    --p->byte22;
    move = false;
  } else {
    i32 dx, dy;
    dx = abs(p->x - p->x_coord);
    if (dx < 18) {
      dy = abs(p->y - p->y_coord);
      if (dy <= 18) {
        u8 done = RestDrawOnScreen_Helper2(p);
        if (!done)
          move = false;
      }
    }
    if (move) {
      i32 y, x;
      y = DivideBy6(p->y);
      if ((u8)y >= 46u) {
        state_slot_008 = 2;
        return 0;
      }
      locals->row_offset = 46u * (u8)y;
      x = DivideBy6_(p->x);
      if ((u8)x >= 46u) {
        state_slot_008 = 2;
        return 0;
      }
      p->path_directions[0] = (pt_rest_floor_cell((u8)y, (u8)x) >> 2) & 3u;
    }
  }
  if (move)
    (void)InteractiveAttackRest_Move_Helper2(p);
  u8 dir = (*pt_pig_path_direction(p));
  if (p->word26 != -1 && (dir == 1 || dir == 2)) {
    locals->carrying_in_front = 1;
    RestDrawOnScreen_Helper3(p);
  }

  u8 image = (u8)(3u * (*pt_pig_path_direction(p)) + attack_walk_images[p->byte5]);

  SetupCanvasTempIglooLink();
  const BobHeader *h = SetupHazelWithBobSprite(&temp_hazel, NULL, &bob_walk, image);
  temp_igloo.draw_x = (i16)((u16)(p->x - 12 * r->scrollpos_x - (h->w >> 1) - 5));
  temp_igloo.draw_y = (i16)((u16)(p->y + 11 * r->scrollpos_y - 88 - (h->h >> 1) - 8));
  (void)Canvas_DrawAllIgloos(temp_canvas, active_palette_pointer);
  if (p->word26 != -1 && !locals->carrying_in_front)
    RestDrawOnScreen_Helper3(p);

  return 0;
}
u8 RestGetNewGuestType(Rest *r) {
  u32 sum, selected;
  sum = SumListOfWords(r->customer_count, 6);
  selected = RandomNumber(sum);
  u32 age = 0;
  if (sum) {
    for (; age < 6; ++age) {
      u32 cumulative = SumListOfWords(r->customer_count, age + 1);
      if (selected < cumulative)
        break;
    }
  } else {
    u32 value = RandomNumber(6);
    age = (u8)value;
  }
  u8 chance = RandomChance(50);
  if (chance)
    age += 6;

  return (u8)age;
}
static void redraw(Rest *r) {
  DrawRestaurant(r, 1);
  (void)DrawMapHelper4(current_bitmap);
}
void GuestHelper2(Rest *r, u8 action) {
  GuestActionSearch storage;
  GuestActionSearch *locals = &storage;
  restaurant_hit_test_owner = r;
  if (!r->num_furniture)
    goto done;
  RestInitFloormap(r);
  if (action == 1) {
    u32 initial = RandomNumber(r->num_furniture);
    locals->candidate_index = (u8)initial;
    for (; locals->candidate_index < 2u * r->num_furniture; ++locals->candidate_index) {
      u8 i = locals->candidate_index, index = i % r->num_furniture;
      locals->guest_index = index;
      Furniture *item = pt_rest_furniture_view(r, index, false);

      const FurnitureDesc *d = pt_furniture_descriptor(item->type);
      if (d->furn_type != 2 || item->linked_furniture == 255)
        continue;
      u8 seated = FindGuestForFurniture(index);
      if (seated != 255)
        continue;
      walking_guest.field_2B = index;
      (void)SetFloormapForFurniture(item);
      const BobHeader *h = furniture_header(item);
      i32 tx = item->x + (h->w >> 1), ty = item->y + (h->h >> 1);

      u8 type;
      type = RestGetNewGuestType(r);

      if (type > 12) {
        ShowMessage(16, guest_type_error_text);
        type = 0;

        Delay(100);
      }

      const Poi *poi = r->property;
      u32 found = GuestHelper3(r, type, -1, 2, 1, (i16)(12 * poi->door_x_start + 30), 266, (i16)tx, (i16)ty);
      if ((u16)found) {
        walking_guest.dead_unk = 2;
        walking_guest.direction_unk = 2;
        walking_guest.gap6 = 8;
        walking_guest.field_2B = index;
        goto done;
      }
      ClearFloormapForFurniture(item);
    }
  } else if (action == 2) {
    locals->candidate_index = 0;
    for (; locals->candidate_index < guest_count; ++locals->candidate_index) {
      u8 index = locals->candidate_index;
      walking_guest.field_2B = index;
      Guest *g = (&guests[index]);
      u8 dir = (u32)g->flags & 3u;

      Furniture *item = pt_rest_furniture_view(r, g->furniture_idx, false);
      i16 x = g->x, y = g->y;
      u8 type = g->guest_type;

      (void)SetFloormapForFurniture(item);

      const Poi *poi = r->property;
      u32 found = GuestHelper3(r, type, -1, dir, 2, x, y, (i16)(12 * poi->door_x_start + 30), 250);
      if ((u16)found) {
        memmove(g, (&guests[(u8)(index + 1)]), sizeof(Guest) * (guest_count - (index + 1u)));
        --guest_count;
        redraw(r);
        goto done;
      }
      ClearFloormapForFurniture(item);
    }
  } else if (action == 3 && r->num_waiters) {
    locals->guest_index = 0;
    for (; locals->guest_index < guest_count; ++locals->guest_index) {
      u8 i = locals->guest_index;
      Guest *g = (&guests[i]);
      if (g->eating_or_waiting)
        continue;
      u8 index = g->furniture_idx;

      Furniture *item = pt_rest_furniture_view(r, index, false);
      const BobHeader *h = furniture_header(item);
      i32 tx = item->x + (h->w >> 1), ty = item->y + (h->h >> 1);

      (void)SetFloormapForFurniture(item);

      const Poi *poi = r->property;
      i16 y = (i16)((u16)(11 * (poi->left_door_y_end + 23 - poi->size_y) + 30));
      u32 found = GuestHelper3(r, 12, 340, 3, 3, 0, y, (i16)tx, (i16)ty);
      if ((u16)found) {
        walking_guest.gap2C[0] = i;
        walking_guest.field_2B = index;
        walking_guest.dead_unk = 2;
        walking_guest.direction_unk = 3;
        walking_guest.gap6 = 20;
        goto done;
      }
      ClearFloormapForFurniture(item);
    }
  }
// A label must precede a statement in C11.
done:;
}
PTActionResult GuestHelper1(void) {
  // Callers discard incidental AL on inactive branches.

  if (state_slot_008 || state_slot_014 || state_slot_015)
    return pt_action_value(0);

  Rest *r = curr_rest;
  if (!r->pizza_count_per_day[6])
    return pt_action_object(curr_rest);
  u8 chance = RandomChance(10);
  if (!chance)
    return pt_action_value(0);
  chance = RandomChance(10);
  if (!chance)
    return pt_action_value(0);
  u32 action = RandomNumber(3);
  GuestHelper2(r, (u8)(action + 1u));
  DrawRestaurant(r, 1);
  u8 result;
  result = DrawMapHelper4(current_bitmap);
  return pt_action_value(result);
}

// Animated stats.

#include "recipe.h"

static u32 integral(double value) {
  double n = TruncateX87Integral(value);
  return isfinite(n) && n >= -2147483648.0 && n < 2147483648.0 ? (u32)(i32)n : 0x80000000u;
}
static double mul53(double a, double b) {
  volatile double v = a * b;
  return v;
}
static double div53(double a, double b) {
  volatile double v = a / b;
  return v;
}
// The original animation sign-extends reputation and both temporary-effect arrays.

void SetAnimatedReputationAndQuality(Rest *rest) {
  for (unsigned i = 0; i < 16; ++i)
    animated_stats_reputation[i] = (i8)rest->reputation_and_effects[i];
  for (unsigned i = 0; i < 4; ++i)
    animated_stats_quality[i] = (i8)rest->quality_of_things[i];
}
u8 ClearAnimatedstatsReputation(void) {
  // The original clears sixteen reputation WORDs and the adjacent four quality WORDs.

  memset(animated_stats_reputation, 0, sizeof(animated_stats_reputation));
  memset(animated_stats_quality, 0, sizeof(animated_stats_quality));
  return 20;
}
void DrawAnimatedExpensesBg(void) {
  FillSomeRectWithColor(29);
  SelectPalette(8);
  (void)DrawAnimatedExpensesBgInner();
  // Falls through into the original corners function, without another CALL frame.

  DrawMarketCompetitorsCorners();
}
i32 ClampValueToGraphSize(i16 value) {
  i32 shifted = value >= 0 ? value / 8 : -((-(i32)value + 7) / 8), result;
  result = Clamp(shifted, 0, 139);
  return result;
}
void ShowAnimatedQualityInner(u8 kind) {
  static const i16 rep[8][2] = {{30, 38}, {30, 78}, {30, 111}, {30, 141}, {155, 145}, {155, 106}, {155, 80}, {155, 38}},
                   quality[4][2] = {{28, 60}, {28, 115}, {158, 123}, {146, 60}};
  if (!kind)
    for (unsigned i = 0; i < 8; ++i)
      (void)DrawBitmapStuff1((u8)i, rep[i][0], rep[i][1]);
  if (kind == 1)
    for (unsigned i = 0; i < 4; ++i)
      (void)DrawBitmapStuff1((u8)(22 + i), quality[i][0], quality[i][1]);
}
void ShowAnimatedHelper4(u8 style) {
  i8 value = (i8)animated_stats_needle;
  if (value >= -10 && value <= 100)
    (void)DrawBitmapStuff1((u8)(style + 16u), 102, (i16)(140 - value));
}
u8 ShowAnimatedHelper1(i8 initial) {
  for (i16 y = 60; y < 151; y += 10)
    (void)DrawBitmapStuff1(20, 99, y);
  (void)DrawBitmapStuff1(19, 99, 33);
  (void)DrawBitmapStuff1(21, 96, 157);
  for (animated_stats_needle = 0; (i8)animated_stats_needle <= initial;
      animated_stats_needle = (u8)(animated_stats_needle + (initial >= 0 ? 1u : 255u)))
    ShowAnimatedHelper4(1);
  u8 result = animated_stats_needle;
  return result;
}
void ShowAnimatedHelper5(i16 x, i16 y, i16 width, u8 direction) {
  if (direction == 1 || direction == 3) {
    (void)DrawPizzaPopularityChart_Helper1(direction == 1 ? 13 : 15, x, y, width);
    (void)DrawBitmapStuff1(direction == 1 ? 9 : 11, (i16)((u16)x + (direction == 1 ? -9 : 30)), (i16)((u16)y - 3u));
  }
}
void ShowAnimatedHelper3(i16 x, i16 y, i16 width, u8 direction) {
  if (!animated_stats_arrow_drawn) {
    ShowAnimatedHelper5(x, y, width, direction);
    animated_stats_arrow_drawn = 1;
  }
  for (unsigned i = 0; i < 8; ++i) {
    WaitForRetrace();
    if (direction == 1 || direction == 3) {
      u16 sx = (u16)(direction == 1 ? 68u + i : 78u - i);
      u16 sy = direction == 1 ? 0 : 11;
      (void)BitBlt(&bmp_blitmap, sx, sy, current_bitmap, (u16)x, (u16)y, (u16)width, 11);
    }
    if (WantSkipGameIntro())
      break;
    Delay(1);
  }
}
u16 ShowAnimatedHelper2(i8 amount, i16 x, i16 y) {
  i16 target = (i16)(amount + (i8)animated_stats_needle);

  animated_stats_arrow_drawn = 0;
  u16 result;
  for (;;) {
    result = (u16)(i16)(i8)animated_stats_needle;
    if ((i16)result == target)
      break;
    ShowAnimatedHelper3(x, y, 30, x <= 100 ? 3 : 1);
    ShowAnimatedHelper4(1);
    result = WantSkipGameIntro();
    if (result)
      break;
    animated_stats_needle = (u8)(animated_stats_needle + (amount >= 0 ? 1u : 255u));
  }
  return result;
}
void ShowAnimatedQuality(Rest *r, i8 initial) {

  u8 quality_index;

  if (r->player_index < game_ptr->num_interactive_players) {
    LoadBlitmapAndOffsets(4);
    DrawAnimatedExpensesBg();
    SetCurrentFont(font_astandar);
    temp_canvas->color = 0;
    DrawText2(12, 203, 30, animated_quality_caption);
    ShowAnimatedQualityInner(1);
    (void)ShowAnimatedHelper1((i8)animated_stats_needle);
    animated_stats_needle = (u8)initial;
    for (quality_index = 0; quality_index < 4; ++quality_index) {
      unsigned i = quality_index;
      if (!r->quality_of_things[i])
        continue;
      DrawAnimatedExpensesBg();
      ShowAnimatedQualityInner(1);
      (void)ShowAnimatedHelper1((i8)animated_stats_needle);
      (void)ShowAnimatedHelper2((i8)r->quality_of_things[i], animated_quality_x[i], animated_quality_y[i]);
      if (WantSkipGameIntro())
        break;
    }
  }
}
void ShowAnimatedReputation(Rest *r, i8 initial) {
  u8 reputation_index;
  if (r->player_index < game_ptr->num_interactive_players) {
    LoadBlitmapAndOffsets(4);
    DrawAnimatedExpensesBg();
    ShowAnimatedQualityInner(0);
    temp_canvas->color = 0;
    SetCurrentFont(font_astandar);
    DrawText2(12, 203, 30, animated_reputation_caption);
    (void)ShowAnimatedHelper1(initial);
    for (reputation_index = 0; reputation_index < 16; ++reputation_index) {
      unsigned i = reputation_index;
      i16 difference = (i16)((u16)((int)(i8)r->reputation_and_effects[i] - animated_stats_reputation[i]));
      if (!difference)
        continue;
      DrawAnimatedExpensesBg();
      ShowAnimatedQualityInner(0);
      (void)ShowAnimatedHelper1((i8)animated_stats_needle);
      if (WantSkipGameIntro())
        break;
      static const i16 xy[8][2] = {
        {50, 50}, {50, 90}, {50, 130}, {50, 170}, {134, 170}, {130, 130}, {130, 90}, {130, 50}};
      unsigned index = i < 8 ? i : 7;
      (void)ShowAnimatedHelper2((i8)((u16)difference), xy[index][0], xy[index][1]);
    }
  }
}
void ShowAnimatedProduction_Helper5(i16 x, i16 y) {
  DrawBob(&bob9, 13, x, y);
}
u8 ShowAnimatedProduction_Helper6(i16 x, i16 y) {
  temp_canvas->color = 14;
  (void)SetTempCanvasPosition((i16)(x + 1), y);
  DefaultLineTo((u16)((i16)(x + 9)), (u16)y);
  (void)SetTempCanvasPosition(x, (i16)(y + 1));
  DefaultLineTo((u16)((i16)(x + 9)), (u16)((i16)(y + 1)));
  temp_canvas->color = 12;
  (void)SetTempCanvasPosition(x, (i16)(y + 2));
  u8 result = DefaultLineTo((u16)((i16)(x + 8)), (u16)((i16)(y + 2)));
  return result;
}
u8 ShowAnimatedProduction_Helper1(u8 count) {
  u8 drawn;
  FillRect(29, 15, 86, 78, 130);
  for (drawn = 0; drawn < count; ++drawn) {
    unsigned i = drawn;
    DrawBob(&bob9, 16, (i16)(12 * (i % 5) + 18), (i16)(7 * (i / 5) + 88));
  }
  u8 result = drawn;
  return result;
}
void ShowAnimatedProduction_Helper2(u8 count) {
  u8 remaining;
  count = count > 20 ? 20 : count;
  remaining = count;
  FillRect(29, 76, 34, 139, 78);
  if (count) {
    for (i16 y = 36; y < 60; y += 11)
      for (i16 x = 80; x < 125; x += 10)
        if (remaining) {
          ShowAnimatedProduction_Helper5(x, y);
          --remaining;
        }
  } else {
    SetCurrentFont(some_font_ptr);
    (void)DrawText(78, 50, animated_no_text);
    (void)DrawText(78, 58, animated_orders_text);
  }
}
u8 ShowAnimatedProduction_Helper3(u8 count) {
  u8 drawn;
  if (count > 20) {
    count = 20;
  }
  FillRect(29, 76, 139, 139, 183);
  for (drawn = 0; drawn < count; ++drawn) {
    unsigned i = drawn;
    (void)ShowAnimatedProduction_Helper6((i16)(82 * (i / 10)), (i16)(180 - 3 * (i % 10)));
  }
  u8 result = drawn;
  return result;
}
void ShowAnimatedProduction_Helper4(u8 item, u8 visible) {
  DrawBob(&bob9, (u16)(item + 4u * (visible % 2u)), animated_production_x[item], animated_production_y[item]);
}
void ShowAnimatedProduction_Helper7(u8 count, const u8 *amounts) {
  FillRect(29, 139, 102, 200, 129);
  for (unsigned i = 0; i < count; ++i)
    for (unsigned j = 0; j < amounts[i]; ++j)
      DrawBob(&bob9, 14, (i16)(14 * i + 142), (i16)(5 * j + 102));
}
void ShowAnimatedProduction(Rest *r) {
  u8 visible_customers;
  u16 oven_capacity;
  u16 customers = 0;
  for (unsigned i = 0; i < 6; ++i)
    customers = (u16)(customers + (u16)r->customer_count[i]);
  u32 value = integral(div53(mul53(customers, r->average_stay_time), animated_day_minutes));
  visible_customers = (u8)value;
  if (visible_customers > 20)
    visible_customers = 20;
  (void)ReComputeStaffStuff(r);
  (void)ReComputeAverageTimes(r);
  animated_order_max = 10;
  animated_pizza_max = 10;
  animated_order_count = (u16)r->demand_of_kind[1] >= 160 ? 10 : (u8)((u16)r->demand_of_kind[1] >> 4);
  u8 ratio = r->demand_capacity_ratio[1] > r->demand_capacity_ratio[2] ? r->demand_capacity_ratio[1]
                                                                       : r->demand_capacity_ratio[2];
  volatile float factor = (float)div53(ratio, animated_percent_scale);
  value = integral(mul53(mul53(animated_order_count, factor), factor));
  animated_order_count = (u8)(value + 1u);
  animated_order_snapshot = animated_order_count;
  animated_finished_count = (u16)r->demand_of_kind[4] >= 160 ? 10 : (u8)((u16)r->demand_of_kind[4] >> 4);
  factor = (float)div53(r->demand_capacity_ratio[4], animated_percent_scale);
  value = integral(mul53(animated_finished_count, factor));
  animated_finished_count = (u8)value;
  animated_finished_snapshot = animated_finished_count;
  u32 capacity = (u16)r->capacity_of_kind[3], used = capacity * r->demand_capacity_ratio[3] / 100u;
  if (capacity < used)
    used = capacity;
  animated_pizza_count = (u8)used;
  animated_pizza_snapshot = (u8)used;
  if (!r->restaurant_opened)
    animated_finished_count = animated_pizza_count = animated_order_count = 0;
  FillLetterBackground(4, 6, 14);
  temp_canvas->color = 12;
  SetCurrentFont(font_astandar);
  DrawText2(12, 203, 30, animated_output_caption);
  animated_oven_count = 0;
  oven_capacity = 0;
  for (unsigned i = 0; i < (r->num_ovens < 4 ? r->num_ovens : 4u); ++i) {
    const Furniture *oven = pt_rest_furniture_identity(r, (u8)i, true);
    const FurnitureDesc *desc = pt_furniture_descriptor(oven->type);
    animated_oven_capacity[animated_oven_count] = desc->capacity;
    oven_capacity = (u16)(oven_capacity + desc->capacity);
    animated_oven_baking_time[animated_oven_count] = desc->baking_time;
    DrawBob(&bob9, 8, (i16)(13 * i + 139), 87);
    ++animated_oven_count;
  }
  capacity = oven_capacity;
  used = capacity * r->demand_capacity_ratio[3] / 100u;
  if (capacity < used)
    used = (used & 0xffffff00u) | (u8)capacity;
  animated_pizza_count = (u8)used;
  if (animated_pizza_count) {
    if (r->demand_capacity_ratio[3] > 90)
      for (unsigned i = 0; i < animated_oven_count; ++i)
        animated_oven_used[i] = animated_oven_capacity[i];
    else if (animated_oven_count == 1)
      animated_oven_used[0] = animated_pizza_count;
    else
      for (unsigned i = 0; i < animated_oven_count; ++i) {
        u32 n = MulDiv100(animated_oven_capacity[i], r->demand_capacity_ratio[3]);
        animated_oven_used[i] = (u8)n;
      }
  } else
    for (unsigned i = 0; i < animated_oven_count; ++i)
      animated_oven_used[i] = 0;
  (void)ShowAnimatedProduction_Helper1(visible_customers);
  ShowAnimatedProduction_Helper2(animated_order_count);
  (void)ShowAnimatedProduction_Helper3(animated_finished_count);
  ShowAnimatedProduction_Helper7(animated_oven_count, animated_oven_used);
  for (unsigned i = 1; i < 16; ++i) {
    ShowAnimatedProduction_Helper4((u8)((i - 1) % 4), 0);
    ShowAnimatedProduction_Helper4((u8)(i % 4), 1);
    if (WantSkipGameIntro())
      break;
    Delay(5);
  }
}
void ShowAnimatedCapacity_Helper(i16 y, i16 width) {

  for (unsigned i = 0; i < 18; ++i) {
    (void)SetTempCanvasPosition(58, (i16)((u16)y + i));
    temp_canvas->color = i < sizeof(animated_capacity_colors) ? animated_capacity_colors[i] : i == 16 ? 22 : 10;
    RelativeLineTo((u32)(i32)width, 0);
    Delay(1);
    if (WantSkipGameIntro())
      break;
  }
}
void ShowAnimatedCapacity(Rest *r) {
  u32 limiting_capacity;
  (void)ReComputeStaffStuff(r);
  FillLetterBackground(4, 37, 40);
  for (unsigned i = 0; i < 5; ++i)
    for (i16 x = 0;; ++x) {
      i32 bound = ClampValueToGraphSize(r->capacity_of_kind[i]);
      if (x >= bound)
        break;
      DrawBob(&bob9, 19, (i16)(x + 58), (i16)(18 * i + 65));
      if (WantSkipGameIntro())
        goto done;
    }
  for (i16 x = 0;; ++x) {
    i32 bound = ClampValueToGraphSize(r->demand_of_kind[0]);
    if (x >= bound)
      break;
    DrawBob(&bob9, 17, (i16)(x + 58), 40);
    if (WantSkipGameIntro())
      goto done;
    Delay(1);
  }
  limiting_capacity = (u32)(i32)r->demand_of_kind[0];
  for (unsigned i = 0; i < 5; ++i) {
    u32 value = (u16)r->capacity_of_kind[i];
    if ((i32)(i16)((u16)limiting_capacity) < (i32)value)
      value = (value & 0xffff0000u) | (u16)limiting_capacity;
    limiting_capacity = value;
    i32 bound = ClampValueToGraphSize((i16)value);
    ShowAnimatedCapacity_Helper((i16)(18 * i + 65), (i16)bound);
    if (WantSkipGameIntro())
      goto done;
  }
  for (i16 x = 0;; ++x) {
    i32 bound = ClampValueToGraphSize((i16)((u16)limiting_capacity));
    if (x >= bound)
      break;
    DrawBob(&bob9, 18, (i16)(x + 58), 162);
    if (WantSkipGameIntro())
      goto done;
    Delay(1);
  }
  temp_canvas->color = 0;
  SetCurrentFont(font_piznumb);
  (void)SetTempCanvasPosition(64, 54);
  i32 bound = ClampValueToGraphSize(r->demand_of_kind[0]);
  if (bound + 58 < 110)
    cur_list_elem_x = 123;
  DrawListElement(animated_demand_text);
  (void)SetTempCanvasPosition(64, 176);
  bound = ClampValueToGraphSize((i16)((u16)limiting_capacity));
  if (bound + 58 < 110)
    cur_list_elem_x = 123;
  DrawListElement(animated_capacity_text);
done:;
}
void ShowAnimatedPopularity(Rest *r) {
  LoadBlitmapAndOffsets(2);
  DrawAnimatedExpensesBg();
  SelectPalette(7);
  temp_canvas->color = 12;
  SetCurrentFont(font_ruby);
  DrawText2(12, 203, 30, animated_popularity_caption);
  DrawText2(12, 203, 42, animated_customers_caption);
  for (unsigned i = 0; i < 6; ++i) {
    (void)DrawBitmapStuff1((u8)i, (i16)(26 * i + 31), 160);
    DrawBob(&bob9, 20, (i16)(26 * i + 32), 50);
  }
  for (unsigned i = 0; i < 6; ++i)
    if (r->popularity_with_customer[i]) {
      i16 top = (i16)(139 - (r->popularity_with_customer[i] >> 2));
      if (top <= 66)
        top = 66;
      for (unsigned y = 139; y > (unsigned)top; --y) {
        (void)DrawBitmapStuff1(29, (i16)(26 * i + 35), (i16)y);
        if (WantSkipGameIntro())
          goto done;
        Delay(1);
      }
    }
done:;
}
void ShowAnimatedTurnover(Rest *r) {
  u32 sales = (u16)r->sales_per_day[5], expenses = r->expenditure_per_day[5];
  u32 a = sales / 100, b = expenses / 100;
  while (a > 15 || b > 15) {
    a /= 10;
    b /= 10;
  }
  DrawAnimatedExpensesBg();
  SelectPalette(42);
  temp_canvas->color = 13;
  SetCurrentFont(font_astandar);
  (void)DrawText(28, 30, animated_takings_text);
  (void)DrawText(120, 30, animated_expenses_text);
  (void)SetTempCanvasPosition(106, 34);
  DefaultLineTo(106, 179);
  for (unsigned i = 0; i < 15; ++i) {
    if (a > i) {
      u32 x = RandomNumber(5);
      DrawBob(&bob9, (u16)(21 + (i != 0)), (i16)((u16)x + 40u), (i16)(138 - 7 * i));
    }
    if (b > i) {
      u32 x = RandomNumber(5);
      DrawBob(&bob9, (u16)(21 + (i != 0)), (i16)((u16)x + 130u), (i16)(138 - 7 * i));
    }
  }
  SetCurrentFont(font_astandar);
  temp_canvas->color = 12;
  (void)SetTempCanvasPosition(60, 45);
  DrawDollarsInsideList(pt_signed_dword(sales));
  cur_list_elem_x = 160;
  DrawDollarsInsideList(pt_signed_dword(expenses));
}
void HandleAnimatedStatsMenu(Rest *r) {
  u8 page;
  page = 0;
refresh:
  did_terminal_stuff = 0;
  (void)Do3x3ButtonStuff(13);
  (void)LoadBobFile(&bob9);
  for (;;) {
    (void)ClearAnimatedstatsReputation();
    SetSomeFieldInCanvas(0);
    switch (page) {
    case 0:
      ShowAnimatedProduction(r);
      break;
    case 1:
      ShowAnimatedReputation(r, 0);
      break;
    case 2:
      ShowAnimatedCapacity(r);
      break;
    case 3:
      ShowAnimatedQuality(r, 0);
      break;
    case 4:
      ShowAnimatedPopularity(r);
      break;
    case 5:
      ShowAnimatedTurnover(r);
      break;
    case 6: {
      UnloadLinkedBob(&bob9);
    }
      return;
    default:
      break;
    }
    u16 event;
    do {
      event = WaitForMouseClick();
      mouse_pressed = event;
      if (event == 3849) {
        Rest *selected;
        selected = SelectNextRestaurant(r);
        r = selected;
        goto refresh;
      }
      if (did_terminal_stuff) {
        r = ptr_apple->restaurant;
        goto refresh;
      }
    } while (event < 10);
    if (pt_is_menu_button_event(event))
      page = (u8)(event + 16u);
  }
}
