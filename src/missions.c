// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
// Objectives.

#include "objects.h"
#include "text.h"
#include "graphics.h"
#include "missions.h"
#include "world.h"
#include "character.h"
#include "files.h"
#include "restaurant.h"
#include "input.h"
#include "sound.h"
#include "render_types.h"
#include "menu.h"
#include <stdio.h>

// goals

void MarkGoalReached(u8 kind) {
  u8 player = current_player_index;
  for (u8 slot = 0; slot < 100; ++slot) {
    PTMissionGoals *history = &pt_player_goals(player)->history;
    if (!history->year[slot]) {
      const Apple *date = ptr_apple;
      history->day[slot] = date->day;
      history->month[slot] = date->month;
      history->year[slot] = (u8)((u8)date->year - 108u);
      history->kind[slot] = kind;
      Character *character_score = character_ptr;
      u32 award = pt_goal_points(kind);
      character_score->score = pt_signed_dword((u32)character_score->score + award);
      if (kind < 100) {
        u8 *pending = pt_pending_goal(current_player_index);
        if (!(*pending))
          (*pending) = (u8)(slot + 100u);
      }
      if (kind == 140)
        HandleGameEnding();
      return;
    }
    if (history->kind[slot] == kind) {
      return;
    }
  }
}

// goal display

// Build the transparent mask using native canvas and bitmap descriptors.

void FillRect1OfGraphicsRam(void) {
  Canvas canvas = {0};
  Bitmap bitmap;

  InitializeBitmap(&bitmap, 320, 200);

  Canvas_Init(&canvas);

  pt_bitmap_bind_surface(&bitmap, pt_transparency_mask().surface);
  canvas.bitmap = &bitmap;

  canvas.color = 1;

  Canvas_ClearWholeBitmap(&canvas, 0);

  const Rect *rect = rect_1;

  Canvas_FillRect(&canvas, (u32)(i32)rect->left, (u32)(i32)rect->top, (u32)(i32)rect->right, (u32)(i32)rect->bottom);
}
void FillSomeRectInGraphicsRam(void) {
  Rect *rect = rect_1;
  rect->left = 12;
  rect->top = 14;
  rect->right = 203;
  rect->bottom = 189;

  FillRect1OfGraphicsRam();

  temp_canvas->bitmap_mask_flag = 1;
}

static u8 last_target(void) {
  return pt_goal_last_target(current_player_index);
}
static void background_name(u8 bitmap, u8 name[16]) {
  unsigned n = 6;
  memcpy(name, "gfx\\zw", 6);
  if (bitmap >= 100)
    name[n++] = (u8)('0' + bitmap / 100);
  if (bitmap >= 10)
    name[n++] = (u8)('0' + (bitmap / 10) % 10);
  name[n++] = (u8)('0' + bitmap % 10);
  memcpy(name + n, ".vga", 5);
}

void ShowStuffForReachedGoal(u8 kind) {

  u8 bitmap = pt_goal_bitmap(kind), palette = game_ptr->current_palette;
  if (!kind) {

    return;
  }
  u8 last = last_target();

  InitGraphicsMode();

  if (bitmap) {

    LoadLangFile(&langfile_levent);

    ClearCanvasField14();

    u8 name[16];
    background_name(bitmap, name);

    memcpy(strbuf, name, strlen((const char *)name) + 1);

    LoadBackground(strbuf, old_bitmap);

    BackupCanvasAttr();

    SetCurrentBitmap(old_bitmap);

    i8 scene_palette = (i8)((u8)(bitmap + 47u));

    CommitBitmapAndSetPalette(scene_palette);

    const PTGoalSceneStyle *style = pt_goal_scene_style(bitmap);
    if (style->use_contrast_palette) {

      SetPalColor3(253, 15, 15, 15);

      SetPalColor3(254, 3, 3, 3);
    }

    SetCurrentFont(font_astandar);

    SetSomeFieldInCanvas(0);

    game_ptr->paragraph_background_enabled = 0;
    game_ptr->list_element_spacing_2 = 2;
    game_ptr->paragraph_render_mode = 2;
    goal_ui_colors[2] = style->foreground;
    goal_ui_colors[3] = style->highlight;
    // Four geometry arguments precede the nested translated-string call.

    u8 *text = GetTranslatedString(kind);

    Canvas_TextRect(temp_canvas, style->foreground, style->highlight, text, 20, 20, 300, 150);

    goal_ui_colors[2] = 1;
    goal_ui_colors[3] = 2;

    ClearInputEvents();

    game_ptr->list_element_spacing_2 = 0;
    game_ptr->paragraph_render_mode = 0;

    WaitForMouseClick2();

    SetCurrentBitmap(old_bitmap);

    CommitBitmapAndSetPalette((i8)palette);

    CopyBitmap2to1();

    FreeLangFile(&langfile_levent);

    RestoreCanvasAttrs();

    FillSomeRectInGraphicsRam();
  }
  if (game_ptr->is_mission_mode == 1 && kind == pt_goal_target_at(current_player_index, last)->kind[last % 100u]) {

    u8 percentage = ComputeTotalMissionPct(current_player_index);

    (void)snprintf((char *)strbuf, sizeof(strbuf), (const char *)mission_completion_format, (int)percentage);

    ShowRecipeBookHint(strbuf);

    HandlePlayerDead(current_player_index);
  }
}

// Show the ten ending pages, then enter the shared game-over exit.

void HandleGameEnding(void) {

  LoadLangFile(&langfile_ende);

  ClearCanvasField14();

  LoadBackground(ending_background_filename, old_bitmap);

  BackupCanvasAttr();

  SetCurrentBitmap(old_bitmap);

  CommitBitmapAndSetPalette(63);

  CopyBitmap2to1();

  game_ptr->paragraph_background_enabled = 0;
  game_ptr->list_element_spacing_2 = 2;
  game_ptr->paragraph_render_mode = 2;
  paragraph_highlight_override = 1;

  SetCurrentFont(font_astandar);

  for (u8 page = 1; page <= 10; ++page) {

    Copy320x200Bitmap(old_bitmap, current_bitmap);

    // Translate and display each ending page before accepting the next click.

    u8 *text = GetTranslatedString(page);

    Canvas_TextRect(temp_canvas, 1, 2, text, 20, 20, 300, 150);

    WaitForMouseClick2();
  }

  paragraph_highlight_override = 0;
  game_ptr->list_element_spacing_2 = 0;
  game_ptr->paragraph_render_mode = 0;

  CopyBitmap2to1();

  FreeLangFile(&langfile_ende);

  ShowGameOverExitMsg();
}

// personal mission

u32 WriteMissionHistory(u8 player) {

  const PTMissionGoals *columns = &pt_player_goals(player)->history;
  const u8 *rows[4] = {columns->kind, columns->day, columns->month, columns->year};
  u32 result = 0;
  for (unsigned i = 0; i < 4; ++i) {

    result = WriteBytesToCurrentFile(rows[i], sizeof(columns->kind));
  }

  return result;
}
u32 ReadMissionTargets(u8 player) {

  game_ptr->show_disk_read_errors = 0;

  PTMissionGoals *columns = &pt_player_goals(player)->targets;
  u8 *rows[4] = {columns->kind, columns->day, columns->month, columns->year};
  u32 result = 0;
  for (unsigned i = 0; i < 4; ++i) {

    result = ReadBytesFromCurrentFile(rows[i], sizeof(columns->kind));
  }

  game_ptr->show_disk_read_errors = 1;

  return result;
}
u32 SaveGamePersonal(const u8 *filename, u8 player) {

  u8 text[256];
  size_t length = 0;
  for (;;) {
    if (length >= sizeof(strbuf) - 4u)
      abort();
    text[length] = (filename + length)[0];
    if (!text[length])
      break;
    ++length;
  }

  memcpy(strbuf, text, length);
  memcpy(strbuf + length, ".mis", 5);

  OpenFileForWritingAsCurrent(strbuf);

  WriteMissionHistory(player);

  return CloseCurrentWritingFile();
}
u32 LoadMissionTargetsFile(const u8 *filename, u8 player) {

  OpenFileAsCurrent(filename);

  ReadMissionTargets(player);

  CloseCurrentFile();
  return 0;
}

// mission completion

// . Every byte date maps to a bounded signed 360-day ordinal.

i32 ComputeMissionGoalPctInner(u8 day, u8 month, u8 year, u8 target_day, u8 target_month, u8 target_year) {

  i32 actual = (i32)day - 1 + 30 * ((i32)month - 1) + 360 * (i32)year;
  i32 target = (i32)target_day - 1 + 30 * ((i32)target_month - 1) + 360 * (i32)target_year;
  i32 difference = actual - target;

  i32 result = difference < 0 ? -difference : difference;

  return result;
}
// Callers consume AL. Physical byte-indexed rows may exceed 100.

u8 ComputeMissionGoalPercentage(u8 player, u8 slot, u8 target_slot) {

  const PTMissionGoals *history = pt_goal_history_at(player, slot);
  const PTMissionGoals *targets = pt_goal_target_at(player, target_slot);
  unsigned history_index = slot % 100u, target_index = target_slot % 100u;
  i32 difference = ComputeMissionGoalPctInner(history->day[history_index],
      history->month[history_index],
      history->year[history_index],
      targets->day[target_index],
      targets->month[target_index],
      targets->year[target_index]);

  i32 result = 100 - difference;
  if (result < 0)
    result = 0;
  if (result > 100)
    result = 100;

  return (u8)result;
}
// . Duplicate history entries contribute independently. Neither byte loop has a semantic
// 100-entry bound; the returned quotient narrows to AL.

u8 ComputeTotalMissionPct(u8 player) {

  u32 total = 0;
  u8 target = 0;
  while (pt_goal_target_at(player, target)->year[target % 100u]) {
    u8 slot = 0;
    while (pt_goal_history_at(player, slot)->year[slot % 100u]) {
      if (pt_goal_history_at(player, slot)->kind[slot % 100u] ==
          pt_goal_target_at(player, target)->kind[target % 100u]) {

        total += ComputeMissionGoalPercentage(player, slot, target);
      }
      ++slot;
    }
    ++target;
  }
  // Original bug: an empty target table divides by zero. The repaired
  // result for an empty mission is zero percent.

  u8 result = target ? (u8)(total / target) : 0;

  return result;
}
// The text remains owned across background loading and the input wait.
// Paragraph rendering can trim trailing spaces in the caller's buffer.

void ShowRecipeBookHint(u8 *text) {

  u8 palette = game_ptr->current_palette;

  ClearCanvasField14();

  InitGraphicsMode();

  LoadBackground(maestro_background_filename, old_bitmap);

  SetCurrentBitmap(old_bitmap);

  CommitBitmapAndSetPalette(41);

  SetCurrentFont(font_piznumb);

  SetSomeFieldInCanvas(0);

  temp_canvas->color = 20;

  game_ptr->paragraph_indent_y_threshold = 85;
  game_ptr->paragraph_background_enabled = 0;
  game_ptr->list_element_spacing_2 = 5;
  game_ptr->paragraph_render_mode = 1;

  Canvas_TextRect(temp_canvas, 37, 0, text, 150, 180, 310, 30);

  WaitForMouseClick2();

  SetCurrentBitmap(old_bitmap);

  CommitBitmapAndSetPalette((i8)palette);

  CopyBitmap2to1();

  game_ptr->paragraph_render_mode = 0;
  game_ptr->list_element_spacing_2 = 0;

  FillSomeRectInGraphicsRam();
}

// Limit this loop to eight players even if the configured count is larger.

void CheckIfAllPlayersDead(void) {

  if (exit_code != 100) {
    const Game *state = game_ptr;
    bool alive = false;
    for (unsigned i = 0; i < 8; ++i)
      if (i < state->num_interactive_players && players[i].days_away < 200) {
        alive = true;
        break;
      }
    if (!alive) {

      ShowGameOverExitMsg();
    }
  }
}
// Cleanup precedes the retry menu, remaining player notices and yes/no prompt. Both decisions
// enter the original process destructor; exit_code retains the selected restart decision for
// the outer runtime.

void ShowGameOverExitMsg(void) {

  has_many_players = 1;
  game_exit_requested = 1;
  FreeLotsOfStuff();
  StopSound();
  if (!exit_code) {
    LoadAndDrawMainMenu(0);
    exit_code = 100;
    for (u8 player = 0;; ++player) {

      if (player >= game_ptr->num_interactive_players)
        break;
      if (players[player].days_away != 255)
        HandlePlayerDead(player);
    }
    u8 yes = ShowMessage2(0, game_over_retry_prompt);
    exit_code = yes ? 1 : 0;
  } else
    exit_code = 0;
  ExitProcess(1);
}
// Preserve the previous player while handling the final score and record save.

void HandlePlayerDead(u8 player) {

  HighScoreRecord records[10];
  u8 filename[100];

  u8 old_player = current_player_index;

  game_ptr->mask_of_players = (u8)((u32)game_ptr->mask_of_players & (u32)((u8) ~(u8)(1u << (player & 31u))));

  SetCurrentPlayer(player);

  pt_player(player)->days_away = 255;

  LoadHighscoresFile(records);

  u32 score = pt_character(player)->score;
  unsigned rank = 10;
  if (score > 50000) {

    for (unsigned i = 0; i < 10; ++i)
      if (score > records[i].score) {
        rank = i;
        break;
      }
    if (rank < 10) {
      Character *character = pt_character(player);
      const u8 *player_name = pt_player(player)->name_text;

      for (int i = 8; i >= (int)rank; --i) {

        records[i + 1] = records[i];
      }
      records[rank].score = character->score;

      strcpy((char *)records[rank].last_name, (const char *)character->last_name);

      strcpy((char *)records[rank].first_name, (const char *)character->first_name);

      strcpy((char *)records[rank].player_name, (const char *)player_name);

      records[rank].mission_percentage = 101;
      if (game_ptr->is_mission_mode) {

        records[rank].mission_percentage = ComputeTotalMissionPct(current_player_index);
      }

      (void)snprintf((char *)strbuf,
          sizeof(strbuf),
          (const char *)high_score_name_format,
          (const char *)character->first_name,
          (const char *)character->last_name,
          (const char *)high_score_award_text);

      ShowRecipeBookHint(strbuf);

      AskForPizzaPrice(high_score_name_prompt, records[rank].record_name, 8);

      memcpy(filename, personal_record_directory, sizeof(personal_record_directory));
      const u8 *name = records[rank].record_name;

      if (*name)
        strcat((char *)filename, (const char *)name);
      else {

        strcat((char *)filename, (const char *)personal_record_fallback);
      }

      if (*name)
        for (size_t i = 0;; ++i) {

          size_t length = strlen((const char *)filename);

          if (i >= length)
            break;
          u8 *c = &filename[i];
          if (*c == '.' || *c == ' ')
            *c = '_';
        }

      u8 save = ShowMessage2(0, personal_record_save_prompt);

      if (save) {

        SaveGamePersonal(filename, current_player_index);
      }

      WriteHighscoresFile(records);
    }
  }
  if (rank >= 10) {

    ShowRecipeBookHint(game_over_low_score_text);
  }

  CheckIfAllPlayersDead();

  SetCurrentPlayer(old_player);
}

// Active mission.

#include "finance.h"
#include "frame.h"

static void notice(u8 index) {
  (void)FillLetterBackground(4, 38, 44);
  SetCurrentFont(font_piznumb);
  game_ptr->paragraph_background_enabled = 0;
  temp_canvas->color = 0;
  u8 *text;

  text = GetTranslatedString(index);
  (void)Canvas_TextRect(temp_canvas, 0, 15, text, 20, 20, 120, 156);
}
// Ignores its caller's pushed argument. Two saved registers and twenty local bytes retain the
// police/result/date BYTEs at 16/8/12.

u8 HandleActiveMission(u32 ignored_argument) {
  (void)ignored_argument;

  u8 police_type, mission_ready;
  Mission *m = &player_ptr->mission;
  police_type = 0;
  mission_ready = 1;
  u8 compared;
  compared = CompareDateTime(
      m->target_month, m->target_day, m->target_time, ptr_apple->month, ptr_apple->day, ptr_apple->minutesx10);

  if (compared == 1) {
    mission_ready = 0;
    goto tail;
  }
  if (m->mission_cancelled) {
    (void)ChangeCurrentPlayerSocialRating(m->some_rating);
    police_type = m->type;
    (void)ClearMission(m);
    goto tail;
  }
  (void)LoadLangFile(&langfile_mafia);
  if (m->type == kMissionAttackRestaurant) {
    const Player *p = player_ptr;

    HandleUserAttackRestaurant(p->selected_property->restaurant);
  }
  if (m->type == kMissionDeliver) {
    Player *p = player_ptr;

    if (p->num_weapons[m->what_to_bring] >= m->number) {
      u32 amount = (u32)p->average_weapon_price[m->what_to_bring] * m->number;

      (void)TransferPlayerMoney(current_player_index, 10, 3, (float)amount);
      p->num_weapons[m->what_to_bring] = (u8)(p->num_weapons[m->what_to_bring] - m->number);
      m->num_accounts_in_set = 0;
      notice(21);
      goto complete;
    }
  }
  if (m->type == kMissionPickupDeliver && !compared) {
    notice((u8)(22u - m->num_accounts_in_set));
    --m->num_accounts_in_set;
    m->target_month = m->src_month;
    m->target_day = m->src_day;
    m->target_time = m->src_time;
    m->target_x = m->source_x;
    m->target_y = m->source_y;
  complete:
    (void)WaitForMouseClick2();
    if (!m->num_accounts_in_set)
      (void)HandleMissionCompleted(player_ptr);
  }
  FreeLangFile(&langfile_mafia);
tail:
  if (police_type) {
    m->type = 0;
    ReportToThePolice(police_type);
  }
  (void)CheckGoalStuff();
  u8 result = mission_ready;

  return result;
}
// Returns AL zero. A successful phone already changes underworld rating; stock then applies
// the same signed mission delta again here.

u8 HandleMissionCompleted(Player *player) {
  Mission *m = &player->mission;
  bool successful = !m->num_accounts_in_set;
  (void)HandlePhoneCall(4, m->mafia_index, mission_completion_phone_text, successful ? 1 : 2);
  if (successful) {
    MarkGoalReached(mission_completion_goals[m->type]);
    (void)ChangeCurrentPlayerUnderworldRating(m->underworld_rating);
  }
  (void)ClearMission(m);

  return 0;
}

// Mafia.

#include "player.h"
#include "runtime_state.h"
#include "messages.h"
#include <math.h>
#include <stdarg.h>
#pragma STDC FP_CONTRACT OFF

static const char *format(const char *expected) {
  return expected;
}
static void mafia_print(u8 *p, const char *format_, ...) {
  char text[1024];
  va_list args;
  va_start(args, format_);
  int n = vsnprintf(text, sizeof(text), format_, args);
  va_end(args);

  memcpy(p, text, (size_t)n + 1);
}

static u32 pay(u8 credit, u8 debit, u32 n) {
  u32 r;
  float value = (float)n;
  r = TransferPlayerMoney(current_player_index, credit, debit, value);
  return r;
}
Mission *ClearMission(Mission *mission) {
  return memset(mission, 0, sizeof(Mission));
}
u32 ReduceHalfLife(void) {
  MarkGoalReached(83);
  u8 n;
  u32 r;
  n = GetCharacterProperty(PT_CHARACTER_HEALTH);
  r = UpdateCharacterProp(PT_CHARACTER_HEALTH, (i8) - (n / 2));
  n = GetCharacterProperty(PT_CHARACTER_ENERGY);
  r = UpdateCharacterProp(PT_CHARACTER_ENERGY, (i8) - (n / 2));
  return r;
}
void DiedFromFailedMission(void) {
  MarkGoalReached(85);
  ShowStuffForReachedGoal(85);
  HandlePlayerDead(current_player_index);
}
i32 GetPriceOfWeaponArticle(u8 article) {
  u8 next = (u8)(article + 1u);
  i32 base = next > 4 ? 1800 * ((int)next - 3) : 400 * next, result;
  result = MulDiv100(base, (weapon_base_price[4u * ptr_apple->town_index + article % 4] & 127u) + 50);

  return result;
}

u8 HandleMafiaCall(u8 index, const u8 *argument, u8 reason) {
  (void)argument;

  u8 tip[252];
  u8 reply, previous_town, dealer;
  RamMafia *mafia = &ramtown.ram_mafia[index];
  TownMafia *town_mafia = &current_town->town_mafia[index];
  Character *character = character_ptr;
  Mission *mission = &player_ptr->mission;

  double money = character->money > mafia_cost_cash_scale ? character->money : 5000.;

  double scale;
  if (money / mafia_cost_cash_scale >= mafia_cost_scale_limit)
    scale = 6.;
  else {
    scale = money / mafia_cost_cash_scale;
  }

  u32 draw = RandomNumber(300);
  i32 value;
  value = MulDiv100(pt_signed_dword(draw + 1500u - character->underworld_status), town_mafia->loan_interest_rate + 60);

  volatile double product = (double)value * scale;
  double integral;
  integral = TruncateX87Integral(product);

  u64 bits = (u64)(i64)integral;

  u32 base;
  base = RoundValueToMultipleOf((u32)bits, 200);

  u32 cost = (2u * base * ptr_apple->difficulty_level) >> 1;

  if (!reason)
    MarkGoalReached(70);
  game_ptr->list_element_spacing_2 = -1;
  LoadLangFile(&langfile_mafia);
  switch (reason) {
  case 1:
    AnimatePhonePleased();
    if (mission->type == 5 && mission->mission_cancelled) {
      mafia_print(strbuf, "%s", (const char *)mafia_failed_business_text);
      DrawStrbufPhoneMessage();
    } else
      DrawPhoneMessageFromLangfile(mission->type == 5 ? 8 : 6);
    if (!mission->mission_cancelled) {
      u32 amount = (u32)mission->money_if_successful;

      (void)pay(5, 10, amount);
      UpdateMoneyFromMissions(current_player_index, (u32)mission->money_if_successful, 3);
      (void)ChangeSocialOrUnderworldRating(current_player_index, 1, mission->underworld_rating);
    }
    break;
  case 2: {
    AnimatePhoneDispleased();
    DrawPhoneMessageFromLangfile(7);
    u8 r = GetUnderworldRank();
    (void)ChangeCurrentPlayerUnderworldRating((i16)((u16)(-80 * (i32)mission->type * r)));
    if (mission->type == 4)
      DiedFromFailedMission();
    (void)ReduceHalfLife();
    (void)ClearMission(&player_ptr->mission);
    break;
  }
  case 3: {
    Rest *rest = curr_rest;

    const Poi *poi = rest->property;
    Zone *zone;
    zone = FindZoneFromCoordinates(poi->xcoord, poi->ycoord);

    u8 protection = (u8)(2u * (100u - zone->crime_statistics));
    if (ptr_apple->difficulty_level > 1)
      protection = (u8)(protection / (ptr_apple->difficulty_level - 1u));
    if (protection > 100)
      protection = 100;
    if (rest->protection_value == 200) {
      cost >>= 1;
    }

    const u8 *text = GetTranslatedString((u8)(12u + (rest->protection_value < 200)));
    mafia_print(strbuf,
        format("%s For just $%d we will offer you protection of restaurant %d! OK?"),
        (const char *)text,
        pt_signed_dword(cost),
        rest->restaurant_id + 1);

    if (AskPhoneYesNoQuestionFromBuffer() == 102) {
      AnimatePhonePleased();
      special_debt_flag = 1;

      (void)pay(10, 5, cost);
      AddCostForRestaurant(rest, cost, 6);
      special_debt_flag = 0;
      rest->protection_value = (u8)(protection + RandomNumber(20));
      DrawPhoneMessageFromLangfile(14);
    } else {
      (void)ChangeCurrentPlayerUnderworldRating(-10);
      if (RandomChance(character->personality) && RandomChance(character->courage)) {
        DoPhoneFaces(0);
        DrawPhoneMessageFromLangfile(15);
        rest->protection_value = (u8)(RandomNumber(protection) + 3u);
      } else {
        u8 hits = (u8)(RandomNumber(10) + 5u);
        attack_damage = 0;
        AnimatePhoneDispleased();
        DrawPhoneMessageFromLangfile(16);
        rest->protection_value = (u8)(RandomNumber(protection) + 14u);
        if (rest->num_furniture) {
          for (u8 i = 0; i < hits; ++i) {
            unsigned n = RandomNumber(rest->num_furniture);
            Furniture *item = pt_rest_furniture_view(rest, (u8)n, false);
            (void)CrashFurniture(item);
          }

          (void)pay(10, 2, (u32)attack_damage);
        }
        (void)ReComputeStaffStuff(rest);
      }
    }
    break;
  }
  case 4: {
    u32 amount = (u32)mafia->pay_size[current_player_index];
    mafia->loan_size[current_player_index] = 0;

    if ((u16)pay(10, 5, amount))
      DrawPhoneMessageFromLangfile(28);
    else {
      DrawPhoneMessageFromLangfile(29);
      DiedFromFailedMission();
    }
    break;
  }
  case 0: {
    i32 greeting;
    u8 r = GetUnderworldRank();
    greeting = Clamp(((i32)r - 1) / 2 + 128, 128, 133);

    reply = DrawPhoneMessageFromLangfile((u8)greeting);
    while (reply) {
      reply = ShowPhonecallAlternatives(2, 3, 4, 0, 0, 0);
      switch (reply) {
      case 0:
        goto done;
      case 1: {
        if (mission->type) {
          DrawPhoneMessageFromLangfile(5);
          break;
        }
        u32 made;
        made = MakeNewMission(index, current_player_index);
        if (!(u16)made) {
          DrawPhoneMessageFromLangfile(22);
          (void)ClearMission(mission);
          break;
        }
        const u8 *text;
        text = GetTextOfMission(mission);
        mafia_print(strbuf, "%s", (const char *)text);
        DrawStrbufPhoneMessage();
        if (mission->type < 5)
          mafia_print(tip, format("If you are successful, you'll get $%d."), mission->money_if_successful);
        mafia_print(strbuf,
            format("%sThe risk is %d%%. Do you accept the mission?"),
            (const char *)(mission->type < 5 ? tip : empty_string),
            mission->risk);
        reply = AskPhoneYesNoQuestionFromBuffer();
        if (reply != 102) {
          (void)ChangeCurrentPlayerUnderworldRating((i16)((u16)(-10 * (int)mission->type)));
          (void)ClearMission(mission);
          break;
        }
        DoPhoneFaces(0);
        if (mission->type == 4) {
          (void)pay(5, 10, mission->amount);
        }
        if (mission->type == 5) {
          special_debt_flag = 1;

          (void)pay(10, 5, mission->amount);
          special_debt_flag = 0;
        }
        DrawPhoneMessageFromLangfile(9);
        goto done;
      }
      case 2: {
        if (mafia->loan_size[current_player_index]) {
          const u8 *tail = GetTranslatedString(27);

          const u8 *head = GetTranslatedString(26);
          mafia_print(strbuf,
              format("%s %d %s"),
              (const char *)head,
              (u16)mafia->loan_due_days[current_player_index],
              (const char *)tail);

          DrawStrbufPhoneMessage();
          goto done;
        }
        if (GetUnderworldRank() <= 1) {
          DrawPhoneMessageFromLangfile(10);
          goto done;
        }
        u32 amount = 50u * character->underworld_status;
        mafia->loan_size[current_player_index] = pt_signed_dword(amount);
        i32 total;
        total = MulDiv100(pt_signed_dword(amount), town_mafia->loan_interest_rate + 100);
        mafia->pay_size[current_player_index] = total;

        const u8 *tail = GetTranslatedString(24);

        const u8 *head = GetTranslatedString(23);
        mafia_print(strbuf,
            format("%s $ %d %s $%d in exactly 3 months. OK?"),
            (const char *)head,
            pt_signed_dword(amount),
            (const char *)tail,
            total);

        reply = AskPhoneYesNoQuestionFromBuffer();
        if (reply == 102) {
          amount = (u32)mafia->loan_size[current_player_index];

          (void)pay(5, 10, amount);
          DrawPhoneMessageFromLangfile(25);
          mafia->loan_due_days[current_player_index] = 90;
          (void)ChangeCurrentPlayerUnderworldRating(40);
        } else
          mafia->loan_size[current_player_index] = 0;
        break;
      }
      case 3: {
        if (GetUnderworldRank() < 3) {
          AnimatePhonePleased();
          DrawPhoneMessageFromLangfile(11);
          AnimatePhoneDispleased();
          DrawPhoneMessageFromLangfile(17);
          goto done;
        }
        if (!RandomChance(50)) {
          DrawPhoneMessageFromLangfile(101);
          break;
        }
        u32 type = RandomNumber(3);

        previous_town = ptr_apple->town_index;
        u32 selected = RandomNumber(4);

        dealer = (u8)selected;
        u32 price;
        if (!type) {
          DrawPhoneMessageFromLangfile((u8)(2u * selected + 107));
          const u8 *text = GetTranslatedString((u8)(2u * selected + 108));

          (void)pt_strcpy(tip, text);
          price = (selected + 5u) * (RandomNumber(500) + 50u);
        } else if (type == 1) {
          price = 250u * (RandomNumber(5) + 2u);
          u32 town = RandomNumber(10);

          ptr_apple->town_index = (u8)town;
          u32 article = RandomNumber(4);

          u8 next = (u8)((article + 1u) % 4u);

          i32 first, second;
          first = GetPriceOfWeaponArticle((u8)(article + 4));

          second = GetPriceOfWeaponArticle((u8)(next + 4));

          ptr_apple->town_index = previous_town;
          const u8 *town_name = town_names[town];

          const u8 *text = GetTranslatedString(103);
          mafia_print(strbuf, format("%s %s!"), (const char *)text, (const char *)town_name);

          DrawStrbufPhoneMessage();

          const u8 *second_name = GetTranslatedString((u8)(next + 124));

          const u8 *first_name = GetTranslatedString((u8)(article + 124));
          mafia_print(tip,
              format("In %s %s cost $%d and %s $%d at the moment!"),
              (const char *)town_name,
              (const char *)first_name,
              first,
              (const char *)second_name,
              second);

        } else {
          price = 250u * (RandomNumber(10) + 2u);

          if (!arms_dealer_phone_nr[ptr_apple->town_index][selected][0])
            dealer = 0;
          DrawPhoneMessageFromLangfile(104);
          const u8 *phone = arms_dealer_phone_nr[ptr_apple->town_index][dealer];

          const u8 *text = GetTranslatedString(105);
          mafia_print(tip, format("%s %s!!!"), (const char *)text, (const char *)phone);
        }

        const u8 *text = GetTranslatedString(100);
        mafia_print(strbuf, format("%s $%d Interested?"), (const char *)text, pt_signed_dword(price));

        reply = AskPhoneYesNoQuestionFromBuffer();
        if (reply == 102) {
          if ((u16)pay(10, 5, price)) {
            AddCostForPlayer(current_player_index, price, 8);

            (void)pt_strcpy(strbuf, tip);
            DrawStrbufPhoneMessage();
            (void)ChangeCurrentPlayerUnderworldRating(10);
          }
        }
        break;
      }
      default:
        break;
      }
    }
    break;
  }
  default:
    break;
  }
done:
  FreeLangFile(&langfile_mafia);

  return 1;
}
// The two price products use an unsigned 64-bit FILD scratch; sale profit uses signed FILD.

u8 HandleWeaponstoreCall(u8 index, const u8 *argument, u8 reason) {
  (void)argument;
  (void)reason;

  u8 bought_weapons;
  bought_weapons = 0;
  LoadLangFile(&langfile_waffen);
  u8 next_message = 1, answer = 0, article = 0, selection = 0;
  u32 price = 0, total = 0;
print_next:
  answer = DrawPhoneMessageFromLangfile(next_message);
alternatives:
  if (!answer)
    goto done;
  answer = ShowPhonecallAlternatives(11, 12, 13, 14, 0, 0);
  switch (answer) {
  case 0:
    goto done;
  case 1: {
    if (!DrawPhoneMessageFromLangfile(23))
      goto done;
    selection = ShowPhonecallAlternatives(3, 4, 5, 6, 0, 0);
    if (!selection)
      goto done;
    article = (u8)(selection - 1u);

    price = (u32)GetPriceOfWeaponArticle(article);
    const u8 *text = GetTranslatedString(24);

    mafia_print(strbuf, format("%s $%d a piece! OK?"), (const char *)text, pt_signed_dword(price));

    answer = DrawStrbufPhoneMessage();
    if (answer == 101) {
      next_message = 31;
      goto print_next;
    }
    if (!answer)
      goto done;
    DrawPhoneMessageFromLangfile(25);
    selection = ShowPhonecallAlternatives(26, 27, 28, 29, 30, 0);
    answer = selection;
    if (!selection)
      goto done;
    if (selection > 5)
      goto alternatives;
    total = (u32)weapon_store_quantities[selection] * price;

    u32 paid = pay(3, 5, total);
    if ((u16)paid) {
      Player *player = player_ptr;
      player->num_weapons[article] = (u8)(player->num_weapons[article] + weapon_store_quantities[selection]);
      next_message = 32;
    } else
      next_message = 33;
    goto print_next;
  }
  case 2:
    DrawPhoneMessageFromLangfile(2);
    bought_weapons = 1;
    goto done;
  case 3: {
    if (!DrawPhoneMessageFromLangfile(34))
      goto done;
    selection = ShowPhonecallAlternatives(15, 16, 17, 18, 0, 0);
    if (!selection)
      goto done;
    article = (u8)(selection + 3u);

    i32 base;
    base = GetPriceOfWeaponArticle(article);
    u32 draw = RandomNumber(10);
    price = (u32)MulDiv100(base, pt_signed_dword(draw + 90u));
    const u8 *text = GetTranslatedString(20);

    mafia_print(strbuf, format("%s $%d each. OK?"), (const char *)text, pt_signed_dword(price));

    if (AskPhoneYesNoQuestionFromBuffer() == 101) {
      DrawPhoneMessageFromLangfile(38);
      ramtown.number_has_no_connection[5][index] = 1;
      goto done;
    }
    DrawPhoneMessageFromLangfile(22);
    selection = ShowPhonecallAlternatives(26, 27, 28, 29, 30, 0);
    answer = selection;

    u8 amount = weapon_store_quantities[selection];
    if (player_ptr->num_weapons[article] < amount) {
      DrawPhoneMessageFromLangfile(39);
      goto done;
    }
    total = (u32)amount * price;

    u32 paid = pay(5, 3, total);
    if (!(u16)paid)
      goto alternatives;
    Player *player = player_ptr;
    u32 profit = total - (u32)player->average_weapon_price[article] * amount;

    player->num_weapons[article] = (u8)(player->num_weapons[article] - amount);
    answer = DrawPhoneMessageFromLangfile(40);
    float value = (float)pt_signed_dword(profit);
    (void)TransferPlayerMoney(current_player_index, 3, 10, value);
    if (pt_signed_dword(profit) >= 3000)
      MarkGoalReached(97);
    (void)ChangeCurrentPlayerUnderworldRating((i16)((u16)(pt_signed_dword(profit) / 100)));
    goto alternatives;
  }
  case 4: {
    if (!DrawPhoneMessageFromLangfile(35))
      goto done;
    selection = ShowPhonecallAlternatives(15, 16, 17, 18, 0, 0);
    if (!selection)
      goto done;
    article = (u8)(selection + 3u);

    price = (u32)GetPriceOfWeaponArticle(article);
    const u8 *text = GetTranslatedString(19);

    mafia_print(strbuf, format("%s $%d. OK?"), (const char *)text, pt_signed_dword(price));

    answer = DrawStrbufPhoneMessage();
    if (answer == 101) {
      next_message = 31;
      goto print_next;
    }
    if (!answer)
      goto done;
    DrawPhoneMessageFromLangfile(36);
    selection = ShowPhonecallAlternatives(26, 27, 28, 29, 30, 0);
    answer = selection;
    if (!selection)
      goto done;
    if (selection > 5)
      goto alternatives;
    u16 amount = weapon_store_quantities[selection];

    u8 held = player_ptr->num_weapons[article];
    if ((unsigned)held + amount > 255u) {
      amount = (u16)(255u - held);

      if (held == 255) {
        DrawPhoneMessageFromLangfile(42);
        goto alternatives;
      }
      DrawPhoneMessageFromLangfile(41);
    }
    total = (u32)amount * price;

    u32 paid = pay(3, 5, total);
    if ((u16)paid) {
      (void)ChangeCurrentPlayerUnderworldRating((i16)((u16)(total / 500u)));
      Player *player = player_ptr;
      u32 combined = total + (u32)player->num_weapons[article] * (u32)player->average_weapon_price[article];
      player->num_weapons[article] = (u8)(player->num_weapons[article] + amount);

      player->average_weapon_price[article] = pt_signed_dword(combined / player->num_weapons[article]);
      next_message = 37;
    } else
      next_message = 33;
    goto print_next;
  }
  default:
    goto alternatives;
  }
done: { FreeLangFile(&langfile_waffen); }
  if (bought_weapons)
    ReportToThePolice(3);

  return 1;
}

// Mafia mission.

#include "game.h"
#include "ai_day.h"
#include "calendar.h"

static void mafia_mission_print(u8 *p, const char *f, ...) {
  char text[1024];
  va_list args;
  va_start(args, f);
  int n = vsnprintf(text, sizeof(text), f, args);
  va_end(args);

  memcpy(p, text, (size_t)n + 1);
}

// Type 2 is deliberately remapped to type 3.

u32 MakeNewMission(u8 mafia_index, u8 player_index) {

  u8 account_index, first_day;
  Mission *m = &players[player_index].mission;
  u8 rank;
  rank = GetUnderworldRank();

  (void)ClearMission(m);
  m->num_accounts_in_set = 1;
  m->town_index = ptr_apple->town_index;
  u32 choices = rank / 2u;
  if (choices > 5)
    choices = 5;
  m->type = (u8)(RandomNumber(choices) + 1u);
  if (m->type == 2)
    m->type = 3;
  m->mafia_index = mafia_index;
  m->risk = (u8)(RandomNumber(10u * ptr_apple->difficulty_level + 10u) + 5u);
  u32 initial = mission_reward_bases[m->type - 1u] * (u32)m->risk;
  u32 rounded;
  rounded = RoundNumberToZeros(initial / 10u, (i8)((u8)(m->type - 1u)));
  m->money_if_successful = pt_signed_dword(rounded);
  m->some_rating = (i16)((u16)(-10 * (int)m->type));
  Poi *first = GetRandomPoiOfKind(0), *second;
  do {
    second = GetRandomPoiOfKind(0);
  } while (first == second);
  Poi *restaurant = GetRandomPoiOfKind(1);
  m->target_month = ptr_apple->month;
  first_day = (u8)(ptr_apple->day + 1u);
  m->target_day = (u8)(RandomNumber(3) + first_day);

  if (m->target_day > days_in_month[ptr_apple->month]) {
    m->target_day = 1;
    ++m->target_month;
    if (m->target_month > 12) {
      return 0;
    }
  }
  m->target_time = (u8)RandomNumber(72);
  m->src_month = m->target_month;
  first_day = (u8)(m->target_day + 1u);
  m->src_day = (u8)(RandomNumber(2) + first_day);
  // Stock uses the current month again, including when target_month rolled.

  if (m->src_day > days_in_month[ptr_apple->month]) {
    m->src_day = 1;
    ++m->src_month;
    if (m->src_month > 12) {
      return 0;
    }
  }
  m->src_time = (u8)RandomNumber(72);
  switch (m->type) {
  case kMissionPickupDeliver:
    m->what_to_bring = (u8)RandomNumber(32);
    m->risk = (u8)(RandomNumber(3u * ptr_apple->difficulty_level) + m->what_to_bring + ptr_apple->difficulty_level);
    m->money_if_successful = pt_signed_dword((m->risk + 5u) * (RandomNumber(60) + 60u));
    m->num_accounts_in_set = 2;

    m->target_x = first->xcoord;
    m->target_y = first->ycoord;
    m->source_x = (((Poi *)(second)))->xcoord;
    m->source_y = (((Poi *)(second)))->ycoord;
    break;
  case kMissionAttackRestaurant:

    m->target_x = restaurant->xcoord;
    m->target_y = restaurant->ycoord;
    m->target_time = (u8)RandomNumber(48);
    m->money_if_successful = pt_signed_dword(m->risk * (RandomNumber(200) + 250u));
    if (RandomChance(50))
      m->number = (u8)(RandomNumber(50) + 50u);
    else
      m->amount = 100u * (RandomNumber(100) + 20u);
    break;
  case kMissionDeliver: {
    m->number = (u8)(RandomNumber(20) + 2u);
    m->what_to_bring = (u8)(RandomNumber(4) + 4u);

    m->target_x = (((Poi *)(second)))->xcoord;
    m->target_y = (((Poi *)(second)))->ycoord;
    u32 percent = 2u * m->risk + 80u + RandomNumber(80);

    i32 price, value;
    price = GetPriceOfWeaponArticle(m->what_to_bring);
    value = MulDiv100(price, pt_signed_dword(percent));

    m->money_if_successful = pt_signed_dword(m->number * (u32)value);
    break;
  }
  case kMissionMoneyLaunder: {
    m->num_accounts_in_set = 4;
    m->amount = 10000u * (RandomNumber(120) + 30u);
    u32 percent = m->risk + RandomNumber(10);
    i32 value;
    value = MulDiv100(pt_signed_dword(m->amount), pt_signed_dword(percent));
    m->money_if_successful = value;
    account_index = 0;
    while (account_index < 4) {
      u8 i = account_index;
      m->account_amount[i] = pt_signed_dword(RandomNumber(900000) + 100000u);
      m->did_pay_to_acct[i] = 0;
      ++account_index;
    }
    break;
  }
  case kMissionHotDeal:
    m->what_to_bring = (u8)RandomNumber(27);
    m->risk = (u8)(RandomNumber(3u * ptr_apple->difficulty_level) + m->what_to_bring + ptr_apple->difficulty_level);
    m->money_if_successful = pt_signed_dword((m->risk + 5u) * (RandomNumber(15000) + 8000u));
    m->num_accounts_in_set = 0;
    m->amount = ((u32)m->money_if_successful >> 1) + 10000u * RandomNumber(10);
    break;
  default:
    break;
  }
  m->underworld_rating = (i16)((u16)(m->risk * m->type));
  if (RandomChance(m->risk))
    m->mission_cancelled = 1;

  return 1;
}
// Both date buffers remain global; callers can retain two strings.

u8 *GetStringWithDate(u8 month, u8 day, u8 timeofday) {
  mission_date_buffer_index ^= 1;
  const u8 *suffix;

  suffix = GetStringThExpression(day);
  u8 *out = mission_date_buffers[mission_date_buffer_index];
  mafia_mission_print(out,
      "on %s %d%s at %d:%d0 ",
      (const char *)(invoice_month_names[month - 1u]),
      day,
      (const char *)suffix,
      timeofday / 6,
      timeofday % 6);

  return out;
}
// Returns a pointer into the original town's extra-name table.

const u8 *GetNameFromExtraTownData(u8 x, u8 y) {
  u8 kind;
  kind = (u8)((loaded_map_file[y][x] >> 4) - 8u);
  Poi *poi;
  poi = FindPoiFromCoords(x, y);
  const u8 *result = mission_invalid_place;
  if (poi) {
    u8 owner = poi->current_owner;

    result = pt_extra_town_name(kind, owner);
  }

  return result;
}
// Two independently toggled place strings, original 52-byte locals.

u8 *GetMissionAttackTargetName(u8 x, u8 y, u8 direction) {

  mission_place_buffer_index ^= 1;
  u8 *out = mission_place_buffers[mission_place_buffer_index];
  u8 flag = loaded_map_file[y][x];
  if ((flag & 15) == 8) {
    u8 kind = (u8)((flag >> 4) - 8u);

    Zone *zone;
    zone = FindZoneFromCoordinates(x, y);

    const u8 *after = empty_string;
    const u8 *before = empty_string;

    if (!mission_place_name_prefixes[kind])
      after = GetNameFromExtraTownData(x, y);

    if (mission_place_name_prefixes[kind])
      before = GetNameFromExtraTownData(x, y);
    u8 group = mission_place_article_groups[kind];

    mafia_mission_print(out,
        "%s %s %s %s in %s",
        mission_place_articles[2u * group + direction],
        (const char *)before,
        (const char *)(property_names[kind]),
        (const char *)after,
        (const char *)zone->name);

  } else {
    Poi *poi;
    const u8 *name;
    poi = FindPoiFromCoords(x, y);

    Rest *rest = poi->restaurant;
    name = GetRestaurantName(rest);
    mafia_mission_print(out, "%s", (const char *)name);
  }

  return out;
}
static u8 *date(const Mission *m, bool source) {
  return GetStringWithDate(source ? m->src_month : m->target_month,
      source ? m->src_day : m->target_day,
      source ? m->src_time : m->target_time);
}
static u8 *place(const Mission *m, bool source, u8 direction) {
  return GetMissionAttackTargetName(source ? m->source_x : m->target_x, source ? m->source_y : m->target_y, direction);
}
// Argument pushes stay live across helper calls, including the toggling date/place buffers
// used by pickup-and-deliver text.

u8 *GetTextOfMission(const Mission *m) {
  (void)memset(mission_text_string, 0, sizeof(mission_text_string));
  switch (m->type) {
  case kMissionPickupDeliver: {
    const u8 *end_date = date(m, true);
    const u8 *end_place = place(m, true, 1);

    const u8 *pronoun = mission_pronouns[mission_item_pronouns[m->what_to_bring]];
    const u8 *start_date = date(m, false);

    const u8 *start_place = place(m, false, 0);
    const u8 *item = GetTranslatedString((u8)(m->what_to_bring + 30u));

    mafia_mission_print(mission_text_string,
        "Pick up %s %s %s and bring %s %s %s!",
        (const char *)item,
        (const char *)start_place,
        (const char *)start_date,
        (const char *)pronoun,
        (const char *)end_place,
        (const char *)end_date);

    break;
  }
  case kMissionAttackRestaurant: {
    const u8 *target = place(m, false, 0);
    const u8 *when = date(m, false);

    if (m->number)
      mafia_mission_print(mission_text_string,
          "Attack %s restaurant %s and cause at least %d%% chemical contamination!",
          (const char *)when,
          (const char *)target,
          m->number);
    else
      mafia_mission_print(mission_text_string,
          "Attack %s restaurant %s and cause at least $%d damage!",
          (const char *)when,
          (const char *)target,
          pt_signed_dword(m->amount));

    break;
  }
  case kMissionDeliver: {
    const u8 *target = place(m, false, 1);
    const u8 *when = date(m, false);
    const u8 *item = GetTranslatedString((u8)(m->what_to_bring + 120u));

    mafia_mission_print(mission_text_string,
        "Get %d %s and bring them %s %s",
        m->number,
        (const char *)item,
        (const char *)when,
        (const char *)target);

    break;
  }
  case kMissionMoneyLaunder: {
    const u8 *when = date(m, false);

    mafia_mission_print(mission_text_string,
        "You will now receive $%d from us. Divide the money equally among the following accounts before or %s: %d, %d, "
        "%d, %d",
        pt_signed_dword(m->amount),
        (const char *)when,
        m->account_amount[0],
        m->account_amount[1],
        m->account_amount[2],
        m->account_amount[3]);

    break;
  }
  case kMissionHotDeal: {
    const u8 *item = GetTranslatedString((u8)(m->what_to_bring + 70u));

    mafia_mission_print(mission_text_string,
        "We can offer you a share in a very hot deal: about %s. It will cost you $%d The profit would be $%d",
        (const char *)item,
        pt_signed_dword(m->amount),
        m->money_if_successful);

    break;
  }
  default:
    break;
  }

  return mission_text_string;
}

// Police.

u8 DoServeJailTime(u8 days) {
  if (days > 99) {
    days = 99;
  }
  Character *character = character_ptr;
  Player *player = player_ptr;
  character->num_crimes = (i16)((u16)character->num_crimes + 1u);
  player->days_away = (u8)(days + 100u);
  game_ptr->mask_of_players = (u8)((u32)game_ptr->mask_of_players & (u32)((u8) ~(UINT32_C(1) << current_player_index)));
  MarkGoalReached(80);
  (void)ChangeCurrentPlayerSocialRating((i16)(-4 * (i32)days));
  (void)ChangeCurrentPlayerUnderworldRating(10);
  u8 result;
  result = DoEndTurn();
  has_many_players = 1;
  return result;
}
u8 HandlePoliceCall(u8 index, const u8 *argument, u8 crime) {
  (void)index;
  (void)argument;

  u8 result, visit, options[4];
  u32 jail_days;

  result = 1;
  visit = 100;
  Player *player = player_ptr;
  Character *character = character_ptr;
  PTForwardLink *node = player->restaurants.begin.next;
  const Rest *nearest = NULL;
  for (;;) {
    if (!node->next)
      break;

    Rest *restaurant = (Rest *)node;
    if (restaurant->days_until_major_visit < visit) {
      visit = restaurant->days_until_major_visit;
      nearest = (const Rest *)node;
    }
    node = restaurant->linked.next;
  }
  LoadLangFile(&langfile_police);

  u8 underworld;
  underworld = GetUnderworldRank();
  u32 value = crime * (ptr_apple->difficulty_level + (u32)underworld - 2u);
  i32 days;
  days = Clamp(pt_signed_dword(value), 3, 60);

  jail_days = (u32)days;
  if ((u32)days < 1)
    jail_days = 2;

  u32 random;
  u32 sum = (u16)character->num_crimes + jail_days;
  random = RandomNumber(500);
  u32 rounded;
  rounded = RoundValueToMultipleOf(sum * (random + 500u), 1000);

  i32 fine;
  fine = Clamp(pt_signed_dword(rounded), 4000, 500000);

  if (crime) {
    character->num_crimes = (i16)((u16)character->num_crimes + 1u);
    switch (crime) {
    case 3:
    case 7:
      jail_days = crime == 3 ? 8 : 4;
      for (unsigned i = 4; i < 8; ++i) {
        u32 amount = (u32)player->average_weapon_price[i] * player->num_weapons[i];

        TransferPlayerMoney(current_player_index, 10, 3, (float)amount);
        player->num_weapons[i] = 0;
      }
      break;
    case 4:
      for (unsigned i = 0; i < 4; ++i) {
        u32 amount = (u32)player->mission.account_amount[i];

        TransferPlayerMoney(current_player_index, 10, 5, (float)amount);
        player->mission.type = 0;
      }
      break;
    case 8:
      jail_days = 20;
      TransferPlayerMoney(current_player_index, 10, 3, current_purchase_amount);
      break;
    case 9: {
      node = (PTForwardLink *)curr_rest;
      const u8 *tail = GetTranslatedString(19);

      // Stock EBX is the list sentinel: stop before its unsafe name read.
      // The labelled original repair reloads the queued caller's curr_rest.

      const u8 *name = GetRestaurantName((const Rest *)node);
      snprintf((char *)strbuf, sizeof(strbuf), "Your restaurant %s %s", (const char *)name, (const char *)tail);

      DrawStrbufPhoneMessage();

      u8 closed = ((Rest *)node)->restaurant_closed_for_days;

      tail = GetTranslatedString(20);
      snprintf((char *)strbuf, sizeof(strbuf), "%s %d days!", (const char *)tail, closed);

      DrawStrbufPhoneMessage();
      goto done;
    }
    default:
      break;
    }
    if (crime < 9) {
      DrawPhoneMessageFromLangfile(crime);
      if (GetPlayerSocialRank() > 6) {
        DrawPhoneMessageFromLangfile(10);
        (void)ChangeSocialOrUnderworldRating(current_player_index, 0, (i16)((i16)(-80 * (i32)crime)));
        goto done;
      }
      snprintf((char *)strbuf,
          sizeof(strbuf),
          "You have been sentenced to %d days in jail! Or would you rather pay a fine of $%d?",
          pt_signed_dword(jail_days),
          fine);
      if (AskPhoneYesNoQuestionFromBuffer() == 102) {
        if ((u16)TransferPlayerMoney(current_player_index, 10, 5, (float)(u32)fine)) {
          player->last_insurance_damage_type = 4;
          MarkGoalReached(78);
          (void)ChangeSocialOrUnderworldRating(current_player_index, 0, (i16)((i16)(-10 * (i32)crime)));
          result = 0;
          goto done;
        }
      }
      u8 response = ShowPhonecallAlternatives(12, 13, 14, 0, 0, 0);
      if (response == 2 || response == 3) {
        i32 probability =
            response == 2 ? 70 - 12 * (i32)ptr_apple->difficulty_level : 80 - 10 * (i32)ptr_apple->difficulty_level;
        if (RandomChance(probability) &&
            (u16)TransferPlayerMoney(current_player_index, 10, 5, response == 2 ? 10000.f : 50000.f)) {
          DrawPhoneMessageFromLangfile(response == 2 ? 17 : 18);
          result = 0;
          goto done;
        }
        DrawPhoneMessageFromLangfile(16);
        jail_days = jail_days * 2u;
      }
      (void)DoServeJailTime((u8)jail_days);
      goto done;
    }
  }
  for (unsigned i = 0; i < 4; ++i)
    options[i] = RandomChance(50);
  if (GetPlayerSocialRank() < 2) {
    DrawPhoneMessageFromLangfile(22);
    goto done;
  }
  DrawPhoneMessageFromLangfile(21);
  switch (ShowPhonecallAlternatives(
      (u8)(options[0] + 23u), (u8)(options[1] + 25u), (u8)(options[2] + 27u), (u8)(options[3] + 29u), 0, 0)) {
  case 1:
    AnimatePhoneDispleased();
    DrawPhoneMessageFromLangfile(31);
    special_debt_flag = 1;
    TransferPlayerMoney(current_player_index, 10, 5, 1000.f);
    special_debt_flag = 0;
    break;
  case 2: {
    u8 mayor = options[1], available = 0;
    if (mayor) {
      if (player->num_restaurants)
        available = RandomChance(character->charisma);
      if (!nearest)
        available = 0;
    } else {
      if (pt_player_depot(player, ptr_apple->town_index) && ramtown.next_inspection_zone_id != 100)
        available = ptr_apple->day >= 20;
    }
    u8 response;
    response = AskPhoneYesNoQuestionFromLangfile((u8)(2u * mayor + 34u - available));
    if (!available || response != 102)
      break;

    TransferPlayerMoney(current_player_index, 10, 5, mayor ? 2000.f : 4000.f);
    if (mayor) {
      const u8 *name = GetRestaurantName(nearest);
      const u8 *head = GetTranslatedString(38);

      snprintf(
          (char *)strbuf, sizeof(strbuf), "%s %s in exactly %d days!", (const char *)head, (const char *)name, visit);

      DrawStrbufPhoneMessage();
    } else {
      PTForwardLink *zone = current_town->zones.begin.next;
      for (;;) {
        if (!zone->next)
          break;

        Zone *p = (Zone *)zone;
        if (p->zone_id == ramtown.next_inspection_zone_id) {
          const u8 *head = GetTranslatedString(37);
          snprintf((char *)strbuf, sizeof(strbuf), "%s %s.", (const char *)head, (const char *)p->name);

          DrawStrbufPhoneMessage();
          break;
        }
        zone = p->link.next;
      }
    }
    break;
  }
  case 3:
    AnimatePhoneDispleased();
    DrawPhoneMessageFromLangfile(32);
    (void)DoServeJailTime(2);
    break;
  case 4: {
    u32 amount = 2000u * (options[3] + 1u) * GetPlayerSocialRank();

    if ((u16)TransferPlayerMoney(current_player_index, 10, 5, (float)amount)) {
      MarkGoalReached(76);
      const u8 *tail = GetTranslatedString(40);

      snprintf((char *)strbuf,
          sizeof(strbuf),
          "Thanks. What would I have done without the $%d. %s",
          pt_signed_dword(amount),
          (const char *)tail);

      DrawStrbufPhoneMessage();
    } else
      DrawPhoneMessageFromLangfile(41);
    break;
  }
  default:
    break;
  }
done:
  FreeLangFile(&langfile_police);
  u8 answer = result;

  return answer;
}

// Interactive attack.

void InteractiveAttackRest_ClearScreenStuff(void) {
  canvas_1->color = 0;
  Canvas_FillRect(canvas_1, 230, 11, 306, 99);
  Canvas_FillRect(canvas_1, 225, 18, 308, 94);
}
void InteractiveAttackRest_Proc3(void) {
  if (game_ptr->byte107 && game_ptr->byte86) {
    (void)BitBlt(current_bitmap, 225, 11, old_bitmap, 225, 11, 84, 90);
  }
}
void InteractiveAttackRest_DrawHealth(Character *character, u8 index) {
  i16 left = attack_health_left[index], right = attack_health_right[index];

  i32 height = 70u * character->energy / 100u;
  i16 top = (i16)((u16)(96 - height));
  canvas_1->color = 0;
  Canvas_FillRect(canvas_1, (u32)(i32)(i16)left, 25, (u32)(i32)(i16)right, (u32)(i32)(i16)top);
  canvas_1->color = (u16)(6u * index + 21u);
  if ((u16)height)
    Canvas_FillRect(canvas_1, (u32)(i32)(i16)left, (u32)(i32)(i16)top, (u32)(i32)(i16)right, 96);
  InteractiveAttackRest_Proc3();
}
void InteractiveAttackRest_LoadWeaponBitmaps(u8 weapon) {
  (void)LoadBobFile(&bob7);
  {
    int n = pt_bob_format_filename(&bob11, "bob\\nightw%u.bob", weapon);
    if (n < 0 || (size_t)n >= sizeof(bob11.filename))
      abort();
  }
  (void)LoadBobFile(&bob11);
  game_ptr->last_loaded_blitmap_index = 200;
}
void UnloadBob7AndBob11(void) {
  UnloadLinkedBob(&bob7);
  UnloadLinkedBob(&bob11);
}
void InteractiveAttackRest_Init(Rest *rest, u8 num_players, u8 weapon) {
  curr_rest = rest;
  restaurant_hit_test_owner = curr_rest;
  g_num_active_pigs = num_players;
  const Poi *poi = rest->property;
  for (unsigned i = 0; i < (num_players == 2 ? 2u : 1u); ++i) {
    Pig *pig = &g_pigs[i];
    pig->byte4 = (u8)i;
    pig->byte25 = 20;
    pig->byte5 = 0;
    pig->path_directions[0] = i ? 3 : 2;
    pig->path_depth = 0;
    pig->x = i ? 32 : (i16)(12u * poi->door_x_start + 30u);
    pig->y = i ? (i16)(11 * ((int)poi->left_door_y_end + 23 - (int)poi->size_y) + 30) : 250;
    pig->byte2E = 0;
    pig->weapon_type = weapon;
    pig->byteD = 0;
    pig->partner = &g_pigs[i ^ 1u];
    pig->character = i ? interactive_attack_victim : interactive_attack_attacker;
    pig->personality_attrib = (u8)(pig->character->dexterity / 34u + 2u);
    pig->byte28 = 255;
    pig->byte29 = 0;
    pig->dead_unk = 0;
  }
  if (num_players != 2)
    g_pigs[1].byte25 = 0;
}
u16 InteractiveAttackRest_AttackChar(Character *c) {
  u16 result = 1;
  if (c->energy) {
    u8 chance = RandomChance(100 - (i32)c->age);
    if (chance && --c->energy)
      result = 0;
  }

  return result;
}
void InteractiveAttackRest_Proc16(Pig *pig) {
  if (state_slot_031 & 1u)
    ++pig->byteD;
}
u8 InteractiveAttackRest_Move_Helper1(Rest *rest, Pig *pig, u8 direction) {
  u8 first, second;
  first = FindFurnitureAtPoint(rest,
      (i16)((u16)(pig->x + attack_collision_x[2u * direction])),
      (i16)((u16)(pig->y + attack_collision_y[2u * direction])));

  second = FindFurnitureAtPoint(rest,
      (i16)((u16)(pig->x + attack_collision_x[2u * direction + 1u])),
      (i16)((u16)(pig->y + attack_collision_y[2u * direction + 1u])));
  u8 result = first < second ? first : second;

  return result;
}
u32 InteractiveAttackRest_Move_Helper2(Pig *pig) {
  u8 index = (*pt_pig_path_direction(pig));
  if (pig->byte21)
    index = (u8)((index + 2u) & 3u);

  pig->x = (i16)((u16)(pig->x + attack_step_x[index] * pig->personality_attrib));
  pig->y = (i16)((u16)(pig->y + attack_step_y[index] * pig->personality_attrib));
  if (++pig->byte5 > 3)
    pig->byte5 = 0;
  u8 *counter = pt_pig_path_steps(pig);

  u32 result;
  if (pig->byte21) {
    --(*counter);

    result = pig->path_depth;
    // A path counter can alias the selector itself. The original reloads
    // the selected counter for its zero test after decrementing it.

    if (!(*pt_pig_path_steps(pig))) {
      pig->byte21 = 0;
      u8 opposite = (u8)(((*pt_pig_path_direction(pig)) + 2u) & 3u);
      result = pig->path_depth;
      u8 *direction = pt_pig_path_direction(pig);
      (*direction) = opposite;
    }
  } else {
    result = pig->path_depth;
    ++(*counter);
  }
  ++pig->byteE;

  return result;
}
void InteractiveAttackRest_Move(Pig *pig, u8 direction) {
  if (!pig->dead_unk) {
    if (pig->path_directions[0] != direction)
      pig->path_directions[0] = direction;
    else {
      bool blocked = false;
      if (pig->partner) {
        const Pig *other = pig->partner;
        blocked = abs(5 * attack_step_x[direction] + pig->x - other->x) < 16 &&
            abs(pig->y + 5 * attack_step_y[direction] - other->y) < 16;
      }
      if (!blocked) {
        u8 hit = InteractiveAttackRest_Move_Helper1(restaurant_hit_test_owner, pig, pig->path_directions[0]);
        if (hit == 255)
          (void)InteractiveAttackRest_Move_Helper2(pig);
        else if ((!pig->byte4 || g_num_active_pigs == 1) && hit == 251) {
          const Poi *poi = curr_rest->property;
          if (abs(pig->x - (12 * poi->door_x_start + 30)) < 16) {
            (void)InteractiveAttackRest_Move_Helper2(pig);
            --pig->byte25;
          }
        }
        if (!pig->byte25)
          state_slot_019 = 0;
      }
    }
  }
}
void InteractiveAttackRest_FurnAttack0(Furniture *f) {
  if (!f)
    return;

  if (f->flags & 0xf8u) {
    u32 strength = ((u8)f->flags >> 3) - 1u;
    f->flags = (f->flags & ~0xf8u) | ((strength & 31u) << 3);
  }
}
PTFurnitureHit InteractiveAttackRest_FindFurniture(Rest *rest, i16 x, i16 y) {
  PTFurnitureHit result = {PT_FURNITURE_CLEAR, NULL};

  if (!rest->is_in_kitchen) {
    if (x < 12) {
      result = (PTFurnitureHit){PT_FURNITURE_DOOR, NULL};
      goto done;
    }
    if (x < 23) {
      result = (PTFurnitureHit){PT_FURNITURE_WALL, NULL};
      goto done;
    }

    const Poi *poi = rest->property;
    if (x > 12 * poi->size_x + 24 || y > 258 || y < 258 - 11 * poi->size_y) {
      result = (PTFurnitureHit){PT_FURNITURE_WALL, NULL};
      goto done;
    }
  }

  for (u32 i = 0; i < rest->num_furniture; ++i) {
    const Furniture *address = pt_rest_furniture_identity(rest, (u8)i, false);

    Furniture *f = (Furniture *)address;
    bool hit;
    hit = IsPointInFurniture(f, x, y);
    if (hit) {
      result = (PTFurnitureHit){PT_FURNITURE_ITEM, f};
      break;
    }
  }
done:

  return result;
}

void InteractiveAttackRest_Shoot_Helper(Pig *pig) {
  if (pig->byte2E && !pig->byteD) {
    bool hit = false;
    if (g_num_active_pigs > 1) {
      const Pig *other = pig->partner;
      hit = abs(pig->byte2E - other->x) < 12 && abs(pig->word30 - other->y) < 12;
    }
    if (hit) {
      pig->byte2A = 1;
      pig->byte29 = 0;
      ++pig->byteD;
    } else {
      bool end = pig->byte28 == 0 ? pig->word30 >= pig->word34
          : pig->byte28 == 1      ? pig->byte2E <= pig->word32
          : pig->byte28 == 2      ? pig->word30 <= pig->word34
          : pig->byte28 == 3      ? pig->byte2E >= pig->word32
                                  : false;
      if (end) {
        ++pig->byteD;
        pig->byte2A = 0;
      } else {
        ++pig->byte29;
        pig->byte2E = (i16)((u16)(pig->byte2E + attack_step_x[pig->byte28] * 6 * pig->byte29));
        pig->word30 = (i16)((u16)(pig->word30 + attack_step_y[pig->byte28] * 6 * pig->byte29));
      }
    }
  }
}
u8 InteractiveAttackRest_BeginShoot(Pig *pig) {
  bool stopped = false;

  u8 result = pig->weapon_type;
  switch (pig->weapon_type) {
  case 0:
    if (pig->byteD < 6)
      pig->byte28 = pig->path_directions[0];
    if (pig->byteD == 5)
      --pig->byteD;
    else
      InteractiveAttackRest_Proc16(pig);
    SoundCrap1(2);
    break;
  case 1:
    if (!pig->byteD) {
      const Rest *rest = curr_rest;
      pig->byte28 = pig->path_directions[0];
      const BobHeader *h = pt_resource_header_at(&bob7, (u16)((u8)(pig->byte28 + 4u * pig->byteD)));
      pig->byte2E = (i16)((u16)(pig->x - 12 * rest->scrollpos_x - (h->w >> 1)));
      pig->word30 = (i16)((u16)(pig->y + 11 * rest->scrollpos_y - 88 - (h->h >> 1)));
    }
    if (pig->byteD >= 8)
      pig->byteD = 1;
    else
      InteractiveAttackRest_Proc16(pig);
    break;
  case 2:
    if (!pig->byte2E) {
      pig->byte28 = pig->path_directions[0];
      pig->byte29 = 0;
      pig->byte2E = (i16)((u16)(attack_projectile_x[pig->byte28] + pig->x));
      pig->word30 = (i16)((u16)(attack_projectile_y[pig->byte28] + pig->y));
      pig->word32 = pig->byte2E;
      pig->word34 = pig->word30;
      while (!stopped) {
        SoundCrap1(4);
        pig->word32 = (i16)((u16)(pig->word32 + 6 * attack_step_x[pig->byte28]));
        pig->word34 = (i16)((u16)(pig->word34 + 6 * attack_step_y[pig->byte28]));
        PTFurnitureHit f;
        f = InteractiveAttackRest_FindFurniture(restaurant_hit_test_owner, pig->word32, pig->word34);
        if (f.kind != PT_FURNITURE_CLEAR) {
          if (f.kind != PT_FURNITURE_ITEM)
            break;

          u8 type = f.furniture->type;
          if (type != 148) {
            stopped = type != 147;
          }
        }
      }
      pig->byteD = 0;
    }
    InteractiveAttackRest_Shoot_Helper(pig);
    break;
  case 3:
    if (!pig->byte29 && !pig->byteD) {
      pig->x_coord = pig->x;
      pig->y_coord = pig->y;
      pig->byte29 = 20;
      result = (u8)pig->y;
    }
    break;
  }

  // Only default and grenade AL paths are meaningful; callers discard other incidental AL.
  return result;
}
void InteractiveAttackRest_EndShoot(Pig *pig) {
  if (!pig->weapon_type) {
    if (!pig->byteD) {
      SoundCrap1(3);
      return;
    }
    if (pig->byteD <= 4) {
      --pig->byteD;
      return;
    }
    if (pig->byteD > 8) {
      pig->byteD = 0;
      return;
    }
    InteractiveAttackRest_Proc16(pig);
  } else if (pig->weapon_type == 1) {
    if (pig->byteD >= 8)
      pig->byteD = 0;
    else if (pig->byteD)
      InteractiveAttackRest_Proc16(pig);
  } else if (pig->weapon_type == 2)
    InteractiveAttackRest_Shoot_Helper(pig);
}

u8 InteractiveAttackRest_Plus1And3(u8 direction) {
  return (u8)((direction + 1u) & 3u);
}
void InteractiveAttackRest_PoliceComing(void) {
  ShowMessage(1, attack_police_message);
  SoundCrap1(6);
  state_slot_018 = 1;
  state_slot_010 = 1;
}
u8 InteractiveAttackRest_Proc9(Character *attacker, Character *victim, u8 count) {
  (void)InitGraphicsMode();
  SoundStuff1();
  u8 crime;
  if (count == 1) {
    attack_police_outcome = 2;
    crime = 2;
  } else {
    const Character *selected = attacker->energy ? attacker : victim;

    u8 player = pt_character_player_index(selected);

    SetCurrentPlayer(player);
    attack_police_outcome = attacker->energy ? 3 : 4;
    crime = 6;
  }
  return HandlePhoneCall(3, 1, attack_police_argument, crime);
}
void InteractiveAttackRest_ShowDamageText(void) {
  canvas_1->color = 0;
  Canvas_FillRect(canvas_1, 228, 30, 307, 40);
  canvas_1->color = 15;
  (void)Canvas_SetPosition(canvas_1, 232, 40);
  {
    int count = attack_damage ? snprintf((char *)strbuf, sizeof(strbuf), "damage $%d", pt_signed_dword(attack_damage))
                              : snprintf((char *)strbuf, sizeof(strbuf), "contamination %d%%", attack_contamination);
    if (count < 0 || (size_t)count >= sizeof(strbuf))
      abort();
  }
  u16 length = (u16)pt_strlen(strbuf);
  (void)Canvas_DrawText(canvas_1, pt_text_span(strbuf, length));
  // Original tail call.
  InteractiveAttackRest_Proc3();
}

static void attack_position_sprite(unsigned slot, i32 x, i32 y) {
  pt_igloo_pool_at(slot)->draw_x = (i16)((u16)x);
  pt_igloo_pool_at(slot)->draw_y = (i16)((u16)y);
}
u8 InteractiveAttackRest_Proc17(void) {
  struct {
    i16 x, y;
  } positions[2];
  bool redraw = false;

  Rest *rest = restaurant_hit_test_owner;

  for (unsigned i = 0; i < rest->num_furniture; ++i) {
    Furniture *f = pt_rest_furniture_view(rest, (u8)i, false);
    if (!(f->flags & 0xf8u)) {
      f->flags = (f->flags & ~0xf8u) | 8u;
      u8 changed = CrashFurniture(f);
      if (changed) {
        redraw = true;
      }
      InteractiveAttackRest_ShowDamageText();
    }
  }
  if (redraw) {
    DrawRestaurant(rest, 1);
    (void)DrawMapHelper4(current_bitmap);
  } else {
    (void)RepositionFurnitureHelper2(current_bitmap);
  }
  DrawRestaurant_ResetIgloos();
  for (unsigned i = 0; i < g_num_active_pigs; ++i) {
    const Pig *pig = &g_pigs[i];
    if (pig->weapon_type != 3 || (!pig->byte29 && !pig->byteD))
      continue;
    if (pig->byte29) {
      (void)SetupHazelWithBobSprite(pt_hazelnut_pool_at((unsigned)i), 0, &bob11, (u16)(pig->byte29 % 2u));
      attack_position_sprite(i, pig->x_coord - 12 * rest->scrollpos_x, pig->y_coord + 11 * rest->scrollpos_y - 88);
    }
    if (pig->byteD) {
      (void)SetupHazelWithBobSprite(pt_hazelnut_pool_at((unsigned)i), 0, &bob11, (u16)pig->byteD);
      attack_position_sprite(i,
          attack_explosion_x[pig->byteD] + pig->x_coord - 12 * rest->scrollpos_x,
          attack_explosion_y[pig->byteD] + pig->y_coord + 11 * rest->scrollpos_y - 88);
    }
  }
  Canvas_DrawAllIgloos(temp_canvas, active_palette_pointer);
  DrawRestaurant_ResetIgloos();
  for (unsigned i = 0; i < g_num_active_pigs; ++i) {
    const Pig *pig = &g_pigs[i];
    unsigned image;
    if (pig->dead_unk <= 10) {
      image = attack_walk_images[pig->byte5] + 3u * pig->path_directions[0] + 12u * i;
    } else
      image = pig->dead_unk - 11u + 7u * i + 32u;

    const BobHeader *header = pt_resource_header_at(&bob7, (u16)image);
    i32 x = pig->x - 12 * rest->scrollpos_x - (header->w >> 1),
        y = 11 * rest->scrollpos_y + pig->y - 88 - (header->h >> 1);
    positions[i].x = (i16)((u16)x);
    positions[i].y = (i16)((u16)y);

    (void)SetupHazelWithBobSprite(pt_hazelnut_pool_at((unsigned)i), 0, &bob7, (u16)image);
    attack_position_sprite(i, positions[i].x, positions[i].y);
    Canvas_DrawAllIgloos(temp_canvas, active_palette_pointer);
  }
  DrawRestaurant_ResetIgloos();
  for (unsigned i = 0; i < g_num_active_pigs; ++i) {
    const Pig *pig = &g_pigs[i];
    if (pig->dead_unk || pig->weapon_type >= 3)
      continue;
    unsigned direction = pig->path_directions[0];

    (void)SetupHazelWithBobSprite(pt_hazelnut_pool_at((unsigned)i), 0, &bob11, (u16)direction);
    unsigned offset = 16u * pig->byte4 + 4u * pig->weapon_type + direction;

    attack_position_sprite(
        i, attack_held_weapon_x[offset] + positions[i].x, attack_held_weapon_y[offset] + positions[i].y);
  }
  Canvas_DrawAllIgloos(temp_canvas, active_palette_pointer);
  DrawRestaurant_ResetIgloos();
  for (unsigned i = 0; i < g_num_active_pigs; ++i) {
    Pig *pig = &g_pigs[i];
    if (pig->dead_unk > 10)
      continue;
    unsigned direction = pig->path_directions[0];

    (void)SetupHazelWithBobSprite(pt_hazelnut_pool_at((unsigned)i), 0, &bob7, (u16)((u16)(direction + 4u * i + 24u)));
    i16 x = positions[i].x, y = positions[i].y;
    attack_position_sprite(
        i, (i ? attack_head1_x : attack_head0_x)[direction] + x, (i ? attack_head1_y : attack_head0_y)[direction] + y);
    if (!pig->byteD && !pig->byte2E)
      continue;
    unsigned phase = pig->byteD, offset;
    u16 image;
    switch (pig->weapon_type) {
    case 0:
      if (phase > 9) {
        pig->byteD = 0;
        break;
      }
      offset = 4u * phase + pig->byte28;

      image = (u16)(phase <= 7 ? offset : phase + 24u);

      (void)SetupHazelWithBobSprite(pt_hazelnut_pool_at((unsigned)(i + 2)), 0, &bob11, (u16)image);

      attack_position_sprite(i + 2, x + attack_knife_x[offset], y + attack_knife_y[offset]);
      break;
    case 1:
      if (!phase)
        break;
      offset = 4u * phase + pig->byte28;

      (void)SetupHazelWithBobSprite(pt_hazelnut_pool_at((unsigned)(i + 2)), 0, &bob11, (u16)offset);

      attack_position_sprite(i + 2, pig->byte2E + attack_spray_x[offset], pig->word30 + attack_spray_y[offset]);
      break;
    case 2:
      if (phase) {
        SoundCrap1(5);
        image = (u16)(phase == 1 ? pig->byte28 + 16u : phase + 18u);
      } else
        image = (u16)(4u * (pig->byte29 & 1u) + pig->byte28 + 8u);

      attack_position_sprite(i + 2, pig->byte2E - 12 * rest->scrollpos_x, pig->word30 + 11 * rest->scrollpos_y - 88);
      (void)SetupHazelWithBobSprite(pt_hazelnut_pool_at((unsigned)(i + 2)), 0, &bob11, (u16)image);
      break;
    }
  }
  u8 result = Canvas_DrawAllIgloos(temp_canvas, active_palette_pointer);

  return result;
}

u16 InteractiveAttackRest_Proc8(void) {
  ++state_slot_031;
  if (state_slot_018 && ++state_slot_018 > 100) {
    (void)GraphicsRestoreCursor2();
    (void)InteractiveAttackRest_Proc9(interactive_attack_attacker,
        // The original one-person police branch never reads the
        // victim argument; Init leaves it null in real AI attacks.

        g_num_active_pigs == 1 ? NULL : interactive_attack_victim,
        g_num_active_pigs);
    (void)HideMouseCursor_();
    state_slot_019 = 0;

    return 1;
  }
  if (g_pigs[0].byte25 < 20)
    InteractiveAttackRest_Move(&g_pigs[0], 0);
  else if (!g_pigs[0].dead_unk) {
    (void)DequeueInputEvent();
    u32 key = ReadKeyboardPort();
    if ((u16)key == 72)
      InteractiveAttackRest_Move(&g_pigs[0], 2);
    if ((u16)key == 77)
      InteractiveAttackRest_Move(&g_pigs[0], 3);
    if ((u16)key == 80)
      InteractiveAttackRest_Move(&g_pigs[0], 0);
    if ((u16)key == 75)
      InteractiveAttackRest_Move(&g_pigs[0], 1);
    if ((u16)key == 57)
      (void)InteractiveAttackRest_BeginShoot(&g_pigs[0]);
    else
      InteractiveAttackRest_EndShoot(&g_pigs[0]);
  }
  if (g_num_active_pigs <= 1) {
    if (state_slot_031 > 250 - state_slot_020) {
      u8 first, second = 0;
      first = RandomChance(4);
      if (first)
        second = RandomChance(4);
      if (second)
        InteractiveAttackRest_PoliceComing();
    }
  } else {
    if (g_pigs[1].byte25 < 20)
      InteractiveAttackRest_Move(&g_pigs[1], 0);
    else if (!g_pigs[1].dead_unk) {
      // Original BIOS joystick direction and fire use recorded platform input.

      u8 direction = (u8)ReadJoystickCrap2_();
      if (direction != 255)
        InteractiveAttackRest_Move(&g_pigs[1], direction);
      u16 fire = ReadJoystickCrap();
      if (fire)
        (void)InteractiveAttackRest_BeginShoot(&g_pigs[1]);
      else
        InteractiveAttackRest_EndShoot(&g_pigs[1]);
    }
  }

  for (u8 i = 0; i < g_num_active_pigs; ++i) {
    Pig *pig = &g_pigs[i];
    if (pig->dead_unk) {
      if (pig->dead_unk == 16) {
        InteractiveAttackRest_PoliceComing();
        pig->dead_unk = 17;
      } else {
        pig->path_directions[0] = InteractiveAttackRest_Plus1And3(pig->path_directions[0]);
        if (pig->dead_unk < 17 && (pig->dead_unk != 10 || !pig->path_directions[0]))
          ++pig->dead_unk;
      }
      continue;
    }
    if (pig->weapon_type == 3) {
      if (pig->byte29) {
        if (!(state_slot_031 % 4))
          --pig->byte29;
        if (!pig->byte29) {
          pig->byteD = 1;
          SoundStuff2(5);
        }
      }
      if (pig->byteD) {
        if (!(state_slot_031 % 2))
          ++pig->byteD;
        if (pig->byteD > 8) {
          pig->byteD = 0;

          Rest *rest = curr_rest;
          for (unsigned j = 0; j < rest->num_furniture; ++j) {
            Furniture *f = pt_rest_furniture_view(rest, (u8)j, false);
            if (abs(f->x - pig->x_coord) < 60 && abs(f->y - pig->y_coord) < 60)
              f->flags = pt_signed_dword((u32)f->flags & (u32)(~0xf8u));
          }
          did_terminal_stuff = 1;
          for (u8 j = 0; j < g_num_active_pigs; ++j)
            if (abs(g_pigs[j].x - pig->x_coord) < 60 && abs(g_pigs[j].y - pig->y_coord) < 60) {
              for (unsigned k = 0; k < 40; ++k)
                (void)InteractiveAttackRest_AttackChar(g_pigs[j].character);
              InteractiveAttackRest_DrawHealth(g_pigs[j].character, j ^ 1u);
            }
        }
      }
    }
    if (pig->weapon_type < 3) {
      if (pig->byteD) {
        i16 x, y;
        if (pig->weapon_type <= 1) {
          // Releasing melee fire advances phase 8 to 9 before the
          // next EndShoot clears it. Its original reach byte at

          int reach = attack_weapon_reach[pig->byteD];
          x = (i16)((u16)(pig->x + reach * attack_step_x[pig->byte28]));
          y = (i16)((u16)(pig->y + reach * attack_step_y[pig->byte28]));

          if (pig->weapon_type) {
            if (attack_contamination < 100) {
              u8 chance = RandomChance(20);
              if (chance)
                ++attack_contamination;
            }
            InteractiveAttackRest_ShowDamageText();
          } else {
            PTFurnitureHit f;
            f = InteractiveAttackRest_FindFurniture(curr_rest, x, y);
            if (f.kind != PT_FURNITURE_CLEAR) {
              InteractiveAttackRest_FurnAttack0(f.furniture);
              InteractiveAttackRest_FurnAttack0(f.furniture);
            }
          }
        } else {
          x = pig->byte2A ? pig->byte2E : pig->word32;
          y = pig->byte2A ? pig->word30 : pig->word34;

          if (pig->byteD == 4) {
            PTFurnitureHit f;
            f = InteractiveAttackRest_FindFurniture(curr_rest, x, y);
            if (f.kind == PT_FURNITURE_ITEM) {
              f.furniture->flags = pt_signed_dword((u32)f.furniture->flags & (u32)(~0xf8u));
            }
          }
          InteractiveAttackRest_Proc16(pig);
          if (pig->byteD > 6) {
            pig->byteD = 0;
            pig->byte2E = 0;
          }
        }
        if (g_num_active_pigs > 1) {
          Pig *other = pig->partner;
          if (abs(x - other->x) < 12 && abs(y - other->y) < 12) {
            u16 dead;
            dead = InteractiveAttackRest_AttackChar(other->character);
            if (dead)
              other->dead_unk = 1;
            else
              InteractiveAttackRest_DrawHealth(other->character, i ^ 1u);
          }
        }
      } else
        pig->byte2A = 0;
    }
  }
  (void)InteractiveAttackRest_Proc17();

  return 0;
}

void InteractiveAttackRest(Rest *rest, u8 is_player, u8 weapon) {
  (void)HideMouseCursor_();
  attack_damage = 0;
  attack_contamination = 0;
  disable_draw_door = 1;
  game_ptr->flagB9 = 0;
  state_slot_018 = 0;
  state_slot_031 = 0;
  interactive_attack_attacker = character_ptr;
  InteractiveAttackRest_ClearScreenStuff();
  if (is_player) {
    interactive_attack_victim = pt_character_record_slot(rest->player_index);
    InteractiveAttackRest_DrawHealth(interactive_attack_attacker, 0);
    InteractiveAttackRest_DrawHealth(interactive_attack_victim, 1);
  }
  (void)LoadMusic(10);
  SoundStuff2(0);
  (void)SaveAndSetInputEventFilter(1);
  InteractiveAttackRest_LoadWeaponBitmaps(weapon);
  InteractiveAttackRest_Init(rest, (u8)(is_player + 1u), weapon);
  CopyBitmap2to1();
  SetCurrentBitmap(old_bitmap);
  DrawRestaurant_SetFlag4OnAll(rest);

  const Poi *poi = rest->property;
  if (poi->door_x_start > 13)
    rest->scrollpos_x = (u8)(poi->size_x - 14u);
  DrawRestaurant(rest, 1);
  DrawMapHelper4(current_bitmap);
  CommitBitmapAndSetPalette(4);
  Copy320x200Bitmap(current_bitmap, old_bitmap);
  game_ptr->in_menu_hierarchy = 2;
  mouse_pressed = 0;
  state_slot_019 = 1;
  while (state_slot_019) {
    for (unsigned i = 0; i < g_num_active_pigs; ++i) {
      const Pig *pig = &g_pigs[i];

      const Rest *current = curr_rest;
      int x = pig->x - 12 * current->scrollpos_x, y = pig->y + 11 * current->scrollpos_y - 88, d = -1;
      if (y > 159 && !pig->path_directions[0])
        d = 3;
      else if (y < 44 && pig->path_directions[0] == 2)
        d = 2;
      else if (x < 42 && pig->path_directions[0] == 1)
        d = 1;
      else if (x > 173 && pig->path_directions[0] == 3)
        d = 0;
      if (d >= 0)
        (void)RestScroll(rest, (u8)d);
    }
    SetCurrentBitmap(old_bitmap);
    u16 done = InteractiveAttackRest_Proc8();
    if (done)
      break;
    CommitBitmapAndSetPalette(0);
  }
  (void)SaveAndSetInputEventFilter(3);
  state_slot_010 = 0;
  game_ptr->in_menu_hierarchy = 0;
  disable_draw_door = 0;
  game_ptr->flagB9 = 1;
  UnloadBob7AndBob11();
  (void)GraphicsRestoreCursor2();
}

// Social rating.

u16 GetRankValueFromIndex(u8 index) {
  u32 value = 0;
  if (index != 255) {
    if (index > 13)
      index = 13;
    value = index_to_rank_value[index];
    i32 adjustment = (i8)((u8)(ptr_apple->difficulty_level - 3u));
    if (index == 1)
      value += (u32)(50 * adjustment);
    else if (index == 2)
      value += (u32)(100 * adjustment);
    else if (index) {
      value = (value * (ptr_apple->difficulty_level + 1u)) >> 2;
      if (value >= 65000)
        value = 65000;
    }
  }

  return (u16)value;
}
u8 GetRankIndexFromValue(u16 value) {
  u8 index = 0;
  while (index < 14) {
    u16 threshold;
    threshold = GetRankValueFromIndex(index);
    if (value < threshold)
      break;
    ++index;
  }
  if (index > 13)
    index = 13;

  return index;
}
u8 GetPlayerSocialRank(void) {
  u8 rank;
  rank = GetRankIndexFromValue(character_ptr->social_status);
  return rank > 13 ? 13 : rank;
}
u8 GetUnderworldRank(void) {
  u8 value;
  value = GetRankIndexFromValue(character_ptr->underworld_status);
  return value > 12 ? 12 : value;
}
u32 MarkSocialOrUnderworldGoalReached(u8 player, u8 kind) {
  u8 rank;
  rank = GetRankIndexFromValue(kind ? characters[player].underworld_status : characters[player].social_status);
  u8 goal = (u8)(14u * kind + rank + 1u);
  MarkGoalReached(goal);

  return 1;
}
// Original bug: negative underflow skipped restoring the player index.
// Restore it on the early zero return as well as the ordinary path.

u32 ChangeSocialOrUnderworldRating(u8 player, u8 kind, i16 delta) {
  u8 previous_player;
  u16 *status = kind ? &characters[player].underworld_status : &characters[player].social_status;
  previous_player = current_player_index;
  current_player_index = player;
  u16 old = *status;
  if (delta < 0) {
    i32 magnitude;
    magnitude = -(i32)delta;
    if ((u32)magnitude > old) {
      *status = 0;
      current_player_index = previous_player;

      return 0;
    }
  }
  u32 sum = old + (u32)(i32)delta;
  *status = sum <= 65535 ? (u16)sum : 65535;
  u16 cap;
  cap = GetRankValueFromIndex(kind ? 12 : 13);
  if (cap <= *status) {
    cap = GetRankValueFromIndex(kind ? 12 : 13);
    *status = (u16)(cap - 1u);
  }
  u8 rank, previous;
  rank = GetRankIndexFromValue(*status);

  previous = GetRankIndexFromValue((u16)(*status - (u32)(i32)delta));
  if (previous < rank) {
    (void)UpdateCharacterProp(kind ? 8 : 1, 5);
    (void)UpdateCharacterProp(PT_CHARACTER_PERSONALITY, 4);

    (void)MarkSocialOrUnderworldGoalReached(player, kind);
  }
  rank = GetRankIndexFromValue(*status);

  previous = GetRankIndexFromValue((u16)(*status - (u32)(i32)delta));
  if (previous > rank) {
    (void)UpdateCharacterProp(kind ? 8 : 1, -7);
    (void)UpdateCharacterProp(PT_CHARACTER_PERSONALITY, -3);
  }
  current_player_index = previous_player;
  // Existing repaired-release return policy.
  u32 result = 1;

  return result;
}

u32 ChangeCurrentPlayerUnderworldRating(i16 delta) {
  return ChangeSocialOrUnderworldRating(current_player_index, 1, delta);
}
u32 ChangeCurrentPlayerSocialRating(i16 delta) {
  return ChangeSocialOrUnderworldRating(current_player_index, 0, delta);
}

// Advertising.

#include "reports.h"

u32 DrawFlyerCosts(const u8 *label, u32 value) {
  Canvas *canvas = canvas_1;
  i16 x = canvas->position_x;

  const u8 *text;
  text = itoa_(pt_signed_dword(value));
  u16 n = (u16)pt_strlen(text), length = (u16)pt_strlen(label);

  Canvas_DrawText(canvas, pt_text_span(label, length));
  u16 width = Canvas_TextWidth(canvas, pt_text_span(text, n));
  canvas->position_x = (i16)(294u - width);

  Canvas_DrawText(canvas, pt_text_span(advert_dollar, 1));

  Canvas_DrawText(canvas, pt_text_span(text, n));
  canvas->position_x = 301;
  canvas->position_x = x;
  canvas->position_y = (i16)((u16)canvas->position_y + 6u);
  return DisplayStuffAfterPaymentInner();
}
// The stock strcpy/sprintf write physical memory; a long substitution can
// cross a 50-byte slot. Keep that behavior without C array-bound arithmetic.

u8 *GetNewspaperAdLine(u8 index, u8 slot) {
  u8 *source = GetTranslatedString(index);
  u8 *scratch = advert_newspaper_buffer;
  size_t n = pt_strlen(source);
  memcpy(scratch, source, n + 1);
  u8 *hash;
  hash = memchr(scratch, '#', n);
  if (hash) {
    *hash = 0;
    const u8 *suffix = hash + 1;
    source = pt_advert_line_window(slot);
    const u8 *name = player_ptr->name_text;
    size_t a = pt_strlen(scratch), b = pt_strlen(name), c = pt_strlen(suffix);
    snprintf((char *)source, a + b + c + 1, "%s%s%s", (const char *)scratch, (const char *)name, (const char *)suffix);
  }
  return source;
}
Game *DrawFlyerText(Campaign *c) {
  FillLetterBackground(4, 26, 45);
  temp_canvas->color = 0;
  SetCurrentFont(some_font_ptr);
  (void)SetTempCanvasPosition(100, 60);
  snprintf((char *)strbuf, sizeof(strbuf), "output: %d items", 100 * c->which);
  DrawStringBufferAsListElement();
  SetCurrentFont(font_astandar);
  const u8 *name = player_ptr->name_text;

  snprintf((char *)strbuf, sizeof(strbuf), "Come to %s and try our pizzas!", (const char *)name);
  game_ptr->paragraph_background_enabled = 0;
  game_ptr->paragraph_indent_y_threshold = 90;
  temp_canvas->color = 12;

  Canvas_TextRect(temp_canvas, 0, 30, strbuf, 104, 80, 186, 70);
  game_ptr->paragraph_indent_y_threshold = 0;
  return game_ptr;
}
u8 ViewOrBuyFlyer(Campaign *c, u8 buy) {
  u16 event;
  if (!buy) {
    (void)DrawFlyerText(c);
    event = WaitForMouseClick();
  } else {
    c->which = 1;
    for (;;) {
      (void)DrawFlyerText(c);
      c->dollars = (i16)(50u * c->which);

      FillGenericInfoBox();

      DrawFlyerOutput(advert_output_label, 100u * c->which);
      (void)DrawFlyerCosts(advert_costs_label, (u16)c->dollars);
      event = WaitForMouseClick();
      mouse_pressed = event;
      if (event == PT_MENU_BUTTON_1) {
        if (c->which < 5)
          ++c->which;
      } else if (event == PT_MENU_BUTTON_3) {
        if (c->which > 1)
          --c->which;
      } else if (event == PT_MENU_BUTTON_4) {
        c->days = 7;
        event = (u16)((event & 0xff00u) | (u8)(2u * c->which));
        c->reach = (u8)event;
        break;
      } else if (event == PT_MENU_BUTTON_5)
        break;
    }
  }

  return (u8)event;
}
void DrawPowerAdvertisement(Campaign *c) {
  Bitmap bitmap;
  InitializeBitmap(&bitmap, 120, 90);
  SetBitmapBuffer(&bitmap, old_bitmap->surface.pixels);
  snprintf((char *)strbuf, sizeof(strbuf), "gfx\\plakat%d.vga", c->which);
  (void)LoadBackground(strbuf, &bitmap);
  BitBlt(&bitmap, 0, 0, current_bitmap, 44, 23, 120, 90);
  CopyBitmap2to1();
}
void ViewOrBuyPoster(Campaign *c, u8 buy) {
  u8 poster_reach;
  bool redraw = true;
  for (;;) {
    if (redraw) {
      FillLetterBackground(4, 25, 46);
      DrawPowerAdvertisement(c);
      redraw = false;
    }
    if (!buy) {
      WaitForMouseClick();
      break;
    }

    poster_reach = ramtown.poster_reach[c->which];
    c->dollars = ramtown.poster_cost[c->which];
    FillGenericInfoBox();
    (void)DrawFlyerCosts(advert_costs_label, (u16)c->dollars);
    u16 event = WaitForMouseClick();
    if (event == PT_MENU_BUTTON_1 && c->which < 8) {
      ++c->which;
      redraw = true;
    } else if (event == PT_MENU_BUTTON_3 && c->which) {
      --c->which;
      redraw = true;
    } else if (event == PT_MENU_BUTTON_4) {
      c->days = 7;
      c->reach = poster_reach;
      break;
    } else if (event == PT_MENU_BUTTON_5)
      break;
  }
}
void DrawNewspaperAdInner(Campaign *c) {
  u8 large = c->which / 2u;
  FillRect(159, 29, 50, 183, 180);
  if (!large)
    FillRect(159, 29, 120, 183, 180);
  SetCurrentFont(large ? font_ruby : font_astandar);
  temp_canvas->color = 0;
  (void)DrawText2(12, 203, (i16)(20u * large + 60u), player_ptr->name_text);
  SetCurrentFont(large ? font_piznumb : some_font_ptr);
  temp_canvas->color = 11;
  for (u8 i = 0; i < 5; ++i) {
    if (c->which & 1u)
      temp_canvas->color = (u8)((u8)(i + 22u));
    u8 *line;
    line = GetNewspaperAdLine((u8)(c->texts[i] + 30u * i), i);
    (void)DrawText2(12, 206, (i16)(25u * large + 9u * i + 80u), line);
  }
}
void ViewOrBuyNewspaperAd(Campaign *c, u8 buy) {
  u8 line_choice, quality;
  u32 line_base;

  const u8 *choices[5];
  FillLetterBackground(4, 31, 35);
  FillRect(159, 136, 45, 186, 186);
  if (!buy) {
    DrawNewspaperAdInner(c);
    WaitForMouseClick();

    return;
  }
  quality = 50;
  u32 random = RandomNumber(22);
  line_choice = (u8)random;
  for (u8 row = 0; row < 5; ++row) {
    random = RandomNumber(5);
    line_choice = (u8)(line_choice + random - 2u);
    i32 base = (i8)line_choice;
    base = Clamp(base, 0, 23);
    line_choice = (u8)base;
    line_base = (u32)base + 30u * row;
    FillRect(159, 31, 76, 186, 174);
    for (u8 i = 0; i < 5; ++i) {
      u8 *line;
      line = GetNewspaperAdLine((u8)(line_base + i), i);
      choices[i] = line;
    }

    game_ptr->did_choose_tv_thingy = 10;
    u8 choice;
    for (;;) {
      choice = AskForNewspaperChoice(5, 0, choices, 35, 90, 0, 159, 14);
      if ((i8)choice < 0)
        continue;
      if (choice <= 4)
        break;
      if (choice == 104 && !row) {
        return;
      }
    }
    u8 picked = (u8)(choice + line_choice);
    c->texts[row] = picked;
    if (row && picked != c->texts[row - 1])
      quality >>= 1;
  }
  bool redraw = true;
  for (;;) {
    if (redraw) {
      c->dollars = (i16)((u16)(500u * (c->which + 1u)));

      const u8 *label = GetTranslatedString((u8)(c->which + 150u));
      snprintf((char *)strbuf, sizeof(strbuf), "%s costs: $%d", (const char *)label, (u16)c->dollars);

      ShowMessage(15, strbuf);
      DrawNewspaperAdInner(c);
      redraw = false;
    }
    u16 event = WaitForMouseClick();
    if (event == PT_MENU_BUTTON_2)
      continue;
    if (event == PT_MENU_BUTTON_1 && c->which < 3)
      ++c->which;
    else if (event == PT_MENU_BUTTON_3 && c->which)
      --c->which;
    else if (event == PT_MENU_BUTTON_4 || event == PT_MENU_BUTTON_5) {
      i32 reach = MulDiv100(quality, 20 * c->which + 80);
      c->reach = (u8)reach;
      c->days = 7;
      break;
    }
    redraw = true;
  }
}
// The frame helper decrements only its caller's argument BYTE.

u32 DrawTvAdFrame(u8 index) {
  index = (u8)(index - 1u);

  u32 result = BitBlt(old_bitmap, (u16)(74u * (index % 4u)), (u16)(50u * (index / 4u)), current_bitmap, 66, 40, 74, 50);

  return result;
}
u32 DrawTvAdAnim(u8 index) {
  u32 result =
      BitBlt(&bmp_awards, (u16)(74u * (index % 3u)), (u16)(50u * (index / 3u)), current_bitmap, 66, 40, 74, 50);

  return result;
}
Game *LoadTvAdBitmap(u8 index) {
  snprintf((char *)strbuf, sizeof(strbuf), "gfx\\spot%d.vga", index);
  return LoadBackground(strbuf, old_bitmap);
}
void ShowNewspaperTvNoise(u8 count) {
  while (--count != 255) {
    DrawTvAdAnim((u8)((count & 1u) + 5u));
    Delay(5);
  }
}
void DrawTvAdIntro(void) {
  for (u8 i = 0; i < 5; ++i) {
    DrawTvAdAnim(i);
    Delay(8);
  }
}
void ShowTvAd(Campaign *c) {
  u8 current_frame, sequence_index;
  sequence_index = 0;
  game_ptr->byte87 = 0;
  game_ptr->byte107 = game_ptr->byte87;
  (void)LoadTvAdBitmap(c->which);
  game_ptr->in_menu_hierarchy = 2;
  for (;;) {
    current_frame = advert_tv_frames[c->which][sequence_index];
    if (!current_frame) {
      sequence_index = 0;
      continue;
    }
    mouse_pressed = WaitForMouseClick();
    u16 held;
    do {
      held = MouseThingy();
    } while (held);
    if (mouse_pressed == 1)
      break;
    DrawTvAdFrame(current_frame);
    ++sequence_index;
  }
  game_ptr->byte87 = 1;
  game_ptr->byte107 = game_ptr->byte87;
  game_ptr->in_menu_hierarchy = 0;
}
void ViewOrBuyTvAd(Campaign *c, u8 buy) {
  u8 tv_reach;
  if (buy)
    c->which = 0;
  bool redraw = true, accepted = !buy;
  for (;;) {
    if (redraw) {
      FillLetterBackground(4, 24, 47);
      ShowNewspaperTvNoise(10);
      FillRect(0, 66, 40, 139, 89);
      temp_canvas->color = 15;
      SetCurrentFont(font_supersma);
      (void)SetTempCanvasPosition(71, 50);
      draw_list_elem_style = 2;
      game_ptr->list_element_spacing_2 = 0;
      DrawListElement(advert_tv_title);
      DrawListElement(GetTranslatedString((u8)(c->which + 154u)));
      DrawListElement(advert_tv_director);
      DrawListElement(GetTranslatedString((u8)(c->which + 164u)));
      draw_list_elem_style = 0;
      if (!buy) {
        Delay(50);
        break;
      }

      tv_reach = ramtown.tv_ad_reach[c->which];
      c->dollars = ramtown.tv_ad_dollars[c->which];
      FillGenericInfoBox();
      (void)DrawFlyerCosts(advert_costs_label, (u16)c->dollars);
      redraw = false;
    }
    u16 event = WaitForMouseClick();
    if (event == PT_MENU_BUTTON_1 && c->which < 8) {
      ++c->which;
      redraw = true;
    } else if (event == PT_MENU_BUTTON_3 && c->which) {
      --c->which;
      redraw = true;
    } else if (event == PT_MENU_BUTTON_4) {
      c->reach = tv_reach;
      c->days = 7;
      accepted = true;
      break;
    } else if (event == PT_MENU_BUTTON_5)
      break;
  }
  if (accepted) {
    DrawTvAdIntro();
    ShowTvAd(c);
  }
  CopyBitmap2to1();
}
void ViewOrBuyCampaign(Campaign *c, u8 category, u8 buy) {
  switch (category) {
  case 0:
    (void)ViewOrBuyFlyer(c, buy);
    break;
  case 1:
    ViewOrBuyPoster(c, buy);
    break;
  case 2:
    ViewOrBuyNewspaperAd(c, buy);
    break;
  case 3:
    ViewOrBuyTvAd(c, buy);
    break;
  default:
    break;
  }
}
void DrawAllCampaigns(i8 selected) {
  u8 category;
  category = 0;
  while (category < 4) {
    u8 i = category;
    FillRect((u8)(i == selected ? 14 : 63), 41, (u16)(28u * i + 73u), 168, (u16)(28u * i + 87u));
    SetSomeFieldInCanvas(0);
    temp_canvas->color = 0;
    SetCurrentFont(font_piznumb);
    const Campaign *c = &ramtown.campaigns[current_player_index][i];
    if (c->days) {
      (void)SetTempCanvasPosition(56, (i16)(28u * i + 84u));
      DrawNumberInsideListRight(c->days);
      cur_list_elem_x = 59;
      DrawListElement(advert_days);
      (void)SetTempCanvasPositionX(115);
      DrawDollarsInsideList((u16)c->dollars);
      (void)SetTempCanvasPositionX(158);
      DrawPercentageNumberInList(c->reach);
    }
    ++category;
  }
}
void HandleAdvertisingMenu(void) {

  u8 previous_selection, purchased_selection, selection;
  u8 initial_days[4];

  Campaign *c = NULL;
  selection = 255;
  purchased_selection = 255;
  (void)LoadBackground(advert_misc_filename, &bmp_awards);
  game_ptr->last_loaded_blitmap_index = 100;

  LoadLangFile(&langfile_werbung);
  for (unsigned i = 0; i < 4; ++i)
    initial_days[i] = ramtown.campaigns[current_player_index][i].days;
  bool redraw = true;
  for (;;) {
    if (redraw) {
      FillLetterBackground(4, 27, 26);

      Do3x3ButtonStuff(33);
      DrawAllCampaigns((i8)selection);
      redraw = false;
    }
    WaitForMouseClick();
    i8 selected = (i8)selection;
    if (mouse_pressed == PT_MENU_BUTTON_1 && selected >= 0 && c->days && c->days < 30 &&
        purchased_selection == (u8)selected) {
      ++c->days;
      redraw = true;
      continue;
    }
    if (mouse_pressed == PT_MENU_BUTTON_3 && selected >= 0 && c->days > initial_days[(u8)selected] &&
        purchased_selection == (u8)selected) {
      --c->days;
      redraw = true;
      continue;
    }
    if (mouse_pressed == PT_MENU_BUTTON_5) {
      FreeLangFile(&langfile_werbung);
      game_ptr->last_loaded_blitmap_index = 99;
      LoadBlitmapAndOffsets(0);

      return;
    }
    if (mouse_pressed == PT_MENU_BUTTON_2) {
      if (selected >= 0 && c->days) {
        ViewOrBuyCampaign(c, (u8)selected, 0);
        redraw = true;
      }
      continue;
    }
    if (mouse_pressed == PT_MENU_BUTTON_4 && selected >= 0) {
      if (c->days) {
        DisplayErrorMessage(0, advert_already_active);
        continue;
      }
      ViewOrBuyCampaign(c, (u8)selected, 1);
      initial_days[(u8)selected] = c->days;
      purchased_selection = (u8)selected;
      redraw = true;
      continue;
    }
    if (mouse_pressed != 1)
      continue;
    bool inside;
    inside = IsMouseInside(39, 62, 169, 168);
    if (!inside)
      continue;
    previous_selection = (u8)selected;
    i32 picked = GetSelectedListItem(64, 26);
    selection = (u8)picked;

    c = &ramtown.campaigns[current_player_index][(u8)picked];
    if ((u8)picked == previous_selection) {
      if ((i8)((u8)picked) >= 0 && c->days) {
        ViewOrBuyCampaign(c, (u8)picked, 0);
        redraw = true;
      }
    } else
      redraw = true;
  }
}

// Market research.

#pragma STDC FP_CONTRACT OFF

static u32 fist32(double x) {
  return isfinite(x) && x >= -2147483648.0 && x < 2147483648.0 ? (u32)(i32)x : 0x80000000u;
}
PTCanvasResult DrawHowMuchTextInsideList(u32 value, u32 low, u32 high) {
  u32 divisor = (high - low) >> 3;
  if (!divisor)
    abort();

  static const u8 *const labels[] = {market_low,
    market_very_little,
    market_little,
    market_average,
    market_above_average,
    market_high,
    market_very_high,
    market_extremely_high,
    market_out_of_range};
  u32 i = value / divisor;
  PTCanvasResult result = DrawTextInsideList(labels[i < 8 ? i : 8]);
  return result;
}
u32 DrawMarketCompetitorsBitmap(u8 index, i16 x, i16 y) {
  return BitBlt(&bmp_awards, index & 240, 16u * (index & 15), current_bitmap, (u16)x, (u16)y, 16, 16);
}
PTCanvasResult ShowMarketAreaInfo(Zone *zone) {
  Zone *z = zone;
  float total = 0;
  for (unsigned i = 0; i < 6; ++i) {
    volatile float next = total + z->population_distribution[i];
    total = next;
  }
  (void)FillLetterBackground(5, 0, 7);
  DrawLetterPins(17, 20, 196, 183, 0);
  SetCurrentFont(font_ruby);
  temp_canvas->color = 11;
  SetSomeFieldInCanvas(0);
  (void)DrawText2(12, 203, 34, GetTranslatedString(35));
  (void)DrawText2(12, 203, 48, (const u8 *)z->name);
  SetCurrentFont(font_piznumb);
  (void)DrawText2(12, 203, 62, GetTranslatedString(36));
  for (unsigned i = 0; i < 6; ++i) {
    (void)SetTempCanvasPosition(56, (i16)(10 * i + 72));
    DrawTextInsideList(customer_age_names[i]);
    (void)SetTempCanvasPositionX(142);
    volatile double product = (double)z->population_distribution[i] * market_percent_scale;
    volatile double quotient = product / total;
    double integral = TruncateX87Integral(quotient);
    u32 n = fist32(integral);
    DrawPercentageNumberInList(pt_signed_dword(n));
  }
  (void)DrawText(24, 140, GetTranslatedString(37));
  (void)SetTempCanvasPositionX(128);
  DrawDollarsInsideList(pt_signed_dword(9000u - 80u * z->how_low_income));
  (void)DrawText(24, 150, GetTranslatedString(38));
  (void)SetTempCanvasPositionX(108);
  DrawTextInsideList(market_zone_names[z->type_of_zone]);
  (void)DrawText(24, 160, GetTranslatedString(39));
  (void)SetTempCanvasPositionX(108);
  DrawHowMuchTextInsideList(z->traffic_density, 0, 100);
  (void)DrawText(24, 170, GetTranslatedString(40));
  (void)SetTempCanvasPositionX(108);
  return DrawHowMuchTextInsideList(z->crime_statistics, 0, 100);
}
static u8 available_count(unsigned kind) {
  PTForwardLink *p = current_town->poi.begin.next;
  u8 n = 0;
  for (;;) {

    Poi *poi = ((Poi *)(p));
    if (!poi->link.next)
      return n;

    u8 f = poi->for_sale_or_rent_flags;
    if ((kind == 0 && (f == 2 || f == 3)) || (kind == 1 && (f == 1 || f == 3)) || (kind == 2 && !f))
      ++n;
    p = poi->link.next;
  }
}
void ShowMarketPropertyInfo(Zone *zone) {
  Zone *z = zone;
  (void)FillLetterBackground(5, 0, 7);
  DrawLetterPins(17, 20, 196, 183, 0);
  SetCurrentFont(font_ruby);
  temp_canvas->color = 11;
  SetSomeFieldInCanvas(0);
  (void)DrawText2(12, 203, 41, market_property_title);
  (void)DrawText2(12, 203, 55, (const u8 *)z->name);
  SetCurrentFont(font_piznumb);
  (void)SetTempCanvasPosition(30, 76);
  DrawTextInsideList(market_premises_label);
  (void)SetTempCanvasPositionX(160);
  DrawPercentageNumberInList(pt_signed_dword(z->premises_availability));
  (void)DrawText(30, 86, market_rent_label);
  (void)SetTempCanvasPositionX(157);
  DrawDollarsInsideList(pt_signed_dword(4u * (u16)z->base_rent_level));
  cur_list_elem_x = (u16)(cur_list_elem_x + 11u);
  DrawListElement(market_per_square_metre);
  (void)DrawText(30, 96, market_price_label);
  (void)SetTempCanvasPositionX(157);
  DrawDollarsInsideList(pt_signed_dword(4u * (u16)z->base_price_level));
  cur_list_elem_x = (u16)(cur_list_elem_x + 11u);
  DrawListElement(market_per_square_metre);
  (void)DrawText(30, 106, market_tendency_label);
  (void)SetTempCanvasPositionX(155);
  DrawTextInsideList(
      (i8)ramtown.price_level_new >= (i8)ramtown.price_level_old ? market_rising_label : market_falling_label);
  (void)DrawText(30, 116, market_average_size_label);
  (void)SetTempCanvasPositionX(160);
  u32 square = (u32)(u16)z->base_size * (u16)z->base_size;
  DrawNumberInsideListRight(pt_signed_dword((square >> 2) | ((square & 0x80000000u) ? 0xc0000000u : 0)));
  DrawListElement(market_square_metres);
  temp_canvas->color = 12;
  (void)DrawText(32, 134, market_rooms_label);
  static const u8 *const captions[] = {market_to_rent_label, market_for_sale_label, market_not_available_label};
  for (unsigned i = 0; i < 3; ++i) {
    (void)DrawText(30, (i16)(145 + 10 * i), captions[i]);
    cur_list_elem_x = 170;
    DrawNumberInsideListRight(pt_signed_dword(available_count(i)));
  }
}
void ShowMarketEconomy(u8 initial_history_index) {
  (void)FillLetterBackground(5, 0, 7);
  DrawLetterPins(17, 20, 196, 126, 1);
  temp_canvas->color = 11;
  SetCurrentFont(font_astandar);
  (void)DrawText(55, 31, GetTranslatedString(41));
  (void)SetTempCanvasPositionX(168);
  DrawNumberInsideListRight(pt_signed_dword((u16)ptr_apple->year));
  temp_canvas->color = 0;
  (void)SetTempCanvasPosition(41, 35);
  RelativeLineTo(0, 80);
  (void)SetTempCanvasPosition(36, 79);
  RelativeLineTo(140, 0);
  if (ramtown.economy_history_index)
    (void)SetTempCanvasPosition(42, (i16)(pt_economy_history_value(initial_history_index) / 3 + 79));
  temp_canvas->color = 23;
  for (u8 i = 1; i < ramtown.economy_history_index; ++i)
    (void)DefaultLineTo((u16)(2u * i + 42u), (u16)(pt_economy_history_value(i) / 3 + 79));
  DrawLetterPins(47, 133, 167, 187, 0);
  temp_canvas->color = 12;
  SetCurrentFont(font_ruby);
  (void)DrawText(66, 152, GetTranslatedString(42));
  (void)SetTempCanvasPosition(104, 169);
  DrawPercentageNumberInList(pt_signed_dword(ramtown.inflation));
}
void ShowMarketCustomerTaste(u8 numeric) {
  (void)FillLetterBackground(5, 0, 7);
  DrawLetterPins(12, 14, 203, 189, 0);
  SetCurrentFont(font_ruby);
  temp_canvas->color = 10;
  (void)DrawText2(12, 203, 24, market_taste_title);
  SetCurrentFont(font_piznumb);
  for (unsigned i = 0; i < 5; ++i)
    (void)DrawText2((i16)(30 * i + 55), (i16)(30 * i + 55), 36, GetTranslatedString((u8)(i + 43)));
  for (unsigned age = 0; age < 6; ++age) {
    (void)DrawBitmapStuff1((u8)age, 18, (i16)(26 * age + 37));
    const PTAgeTastePreference *taste = pt_age_taste(age);
    i32 values[] = {taste->fat, taste->sweet, taste->salty, taste->hot, (i8)taste->unknown_b};
    if (numeric) {
      (void)SetTempCanvasPosition(62, (i16)(26 * age + 55));
      for (unsigned i = 0; i < 5; ++i) {
        if (i)
          cur_list_elem_x = (u16)(cur_list_elem_x + 26u);
        DrawPercentageNumberInList(pt_signed_dword((u32)(10 * values[i])));
      }
    } else
      for (unsigned i = 0; i < 5; ++i)
        (void)DrawBitmapStuff1((u8)(22 + (values[i] <= 5)), (i16)(47 + 30 * i), (i16)(26 * age + 40));
  }
}
void ShowMarketTrends(void) {
  u8 positive_count = 0, negative_count = 0;
  i16 positive_y = 108, negative_y = 108;
  (void)FillLetterBackground(5, 0, 7);
  DrawLetterPins(12, 14, 203, 189, 0);
  SetCurrentFont(font_ruby);
  temp_canvas->color = 0;
  (void)DrawText2(12, 203, 51, GetTranslatedString(48));
  SetCurrentFont(font_piznumb);
  (void)DrawText2(12, 203, 33, GetTranslatedString(49));
  (void)DrawBitmapStuff1(24, 39, 56);
  (void)DrawBitmapStuff1(25, 129, 60);
  for (u8 level = 100; level > 8; --level) {
    for (u8 i = 0; i < 96; ++i) {
      u16 valid = IsValidIngredient(i);
      if (!valid)
        continue;
      i32 trend = current_town->ingredient_trend[i];
      const u8 *name = (const u8 *)ingredients[i / 24u].vegs[i % 24u].name;
      if (trend == level && positive_count < 8) {
        (void)DrawText(20, positive_y, name);
        (void)SetTempCanvasPositionX(101);
        DrawNumberInsideListRight(pt_signed_dword((u32)trend));
        ++positive_count;
        positive_y += 10;
      }
      if (-trend == level && negative_count < 8) {
        (void)DrawText(110, negative_y, name);
        (void)SetTempCanvasPositionX(195);
        DrawNumberInsideListRight(pt_signed_dword((u32)trend));
        ++negative_count;
        negative_y += 10;
      }
    }
  }
}
void ShowMarketPizzaCharts(void) {
  (void)FillLetterBackground(4, 30, 21);
  SetCurrentFont(font_piznumb);
  draw_list_elem_style = 0;
  for (u8 i = 0; i < 10; ++i) {
    RecipeRankEntry *r = &ramtown.recipe_rank[i];
    if (!r->recipe_kind && r->player_index >= game_ptr->num_interactive_players)
      continue;
    u8 last_position = r->last_position;
    temp_canvas->color = 0;
    (void)SetTempCanvasPosition(32, (i16)(11u * i + 59u));
    DrawNumberInsideListRight(pt_signed_dword(i + 1u));
    temp_canvas->color = 12;
    if (last_position) {
      cur_list_elem_x = 42;
      DrawNumberInsideListRight(pt_signed_dword(last_position));
    } else {
      cur_list_elem_x = 36;
      DrawListElement(market_rank_dash);
    }
    temp_canvas->color = 0;
    cur_list_elem_x = 43;
    const u8 *p;
    if (r->player_index >= game_ptr->num_interactive_players)
      p = GetTranslatedString((u8)(r->recipe_kind + 50u));
    else {
      p = ResolveRecipe(pt_player(r->player_index), r->recipe_kind)->name;
    }
    DrawClippedListElement(p, 18);
    cur_list_elem_x = 128;
    DrawClippedListElement(pt_player(r->player_index)->name_text, 13);
  }
}
u8 DrawMarketCompetitorsEntry(i16 x, u8 height) {
  u8 result = 0;
  for (unsigned i = 0; i < 10; ++i) {
    (void)SetTempCanvasPosition((i16)((u16)x + i), 160);
    temp_canvas->color = (u8)(market_bar_colors[i]);
    result = RelativeLineTo(0, (u32) - (i32)height);
  }
  return result;
}
void DrawMarketCompetitorsInner(u32 scale) {
  LoadBlitmapAndOffsets(0);
  FillSomeRectWithColor(15);
  SelectPalette(62);
  temp_canvas->color = 14;
  DrawMarketCompetitorsBackground(26, 40, 189, 160);
  DrawMarketCompetitorsCorners();
  SetCurrentFont(font_astandar);
  temp_canvas->color = 10;
  (void)DrawText2(12, 203, 26, market_chain_title);
  if (scale) {
    snprintf((char *)strbuf, sizeof(strbuf), "turnover in $%d", pt_signed_dword(scale));
  } else {
    snprintf((char *)strbuf, sizeof(strbuf), "market shares in %%");
  }
  (void)DrawText2(12, 203, 37, strbuf);
  SetCurrentFont(some_font_ptr);
  temp_canvas->color = 0;
  for (unsigned i = 1; i < 11; ++i) {
    (void)SetTempCanvasPosition(26, (i16)(160 - 10 * i));
    DrawNumberInsideListRight(pt_signed_dword(i));
  }
  draw_list_elem_style = 0;
  for (unsigned i = 0; i < 8; ++i)
    DrawMarketCompetitorsBitmap((u8)((16 * i) | 10), (i16)(30 + 20 * i), 163);
  LoadBlitmapAndOffsets(2);
}
void ShowMarketCompetitors(void) {

  u8 city = ptr_apple->town_index;
  u32 scale = 1000;
  bool repeat;
  do {
    repeat = false;
    for (unsigned i = 0; i < 8; ++i)
      if ((u32)savegame_sect_0.past_sales_per_day_for_players[i][city] / scale > 100u) {
        scale *= 10u;

        repeat = true;
        break;
      }
  } while (repeat);
  DrawMarketCompetitorsInner(scale * 10u);
  for (unsigned i = 0; i < 8; ++i)
    DrawMarketCompetitorsEntry(
        (i16)(20 * i + 32), (u8)((u32)savegame_sect_0.past_sales_per_day_for_players[i][city] / scale));
}
void ShowMarketShare(void) {
  DrawMarketCompetitorsInner(0);

  for (unsigned i = 0; i < 8; ++i)
    DrawMarketCompetitorsEntry((i16)(20 * i + 32), savegame_sect_0.market_share[i][ptr_apple->town_index]);
}
void HandleMarketResearchMenu(void) {
  u8 page = 0, numeric = 0;
  Do3x3ButtonStuff(26);
  LoadBlitmapAndOffsets(2);
  LoadLangFile(&langfile_markt);
  if (!mouse_over_zone)
    mouse_over_zone = (Zone *)(current_town->zones).begin.next;
  do {
    Zone *z = mouse_over_zone;
    switch (page) {
    case 0:
      ShowMarketAreaInfo(z);
      break;
    case 1:
      ShowMarketPropertyInfo(z);
      break;
    case 2:
      ShowMarketEconomy(page);
      break;
    case 3:
      ShowMarketCustomerTaste(numeric);
      break;
    case 4:
      ShowMarketTrends();
      break;
    case 5:
      ShowMarketPizzaCharts();
      break;
    case 6:
      ShowMarketCompetitors();
      break;
    case 7:
      ShowMarketShare();
      break;
    default:
      break;
    }
    u16 event = WaitForMouseClick();
    if ((u8)event == 1)
      numeric ^= 1u;
    if (pt_is_menu_button_event((u8)event))
      page = (u8)(event + 16u);
  } while (page != 8);
  FreeLangFile(&langfile_markt);
}

// Terminal.

static u16 terminal_wait(void) {
  u16 r = WaitForMouseClick();
  mouse_pressed = r;
  return r;
}

u32 TransformDialledNumber(u32 number) {
  u32 r = 0u - ((number + number / 1000000u) << 2);

  return r;
}
u32 ExtractPhoneNrFromRam(u32 encoded) {
  u32 x = (~(encoded - 1u)) >> 2;
  u32 r = x - x / 1000000u;

  return r;
}
u32 ConvertDialledNumberToInt(const u8 *buffer) {
  u32 result = 0, scale = 1000000;
  for (unsigned i = 0; i < 7; ++i) {
    result += scale * buffer[i];
    scale /= 10;
  }

  return result;
}
void ConvertDialledNumberToString(u32 number, u8 *buffer) {
  u32 scale = 1000000;
  for (unsigned i = 0; i < 7; ++i) {
    u8 digit = (u8)(number / scale);
    buffer[i] = digit;
    number -= scale * digit;
    scale /= 10;
  }
}
u8 *CopyDialNumberToStringBuffer(const u8 *buffer, u8 last_index) {
  for (unsigned i = 0; i <= last_index; ++i)
    strbuf[i] = (u8)(buffer[i] + 48u);
  strbuf[last_index + 1u] = 0;

  return strbuf;
}
void DrawTextSlowly(const u8 *p) {
  for (;;) {
    if (!*p)
      break;
    Delay(1);

    Canvas *c = temp_canvas;
    if (c->position_x > 198 || *p == '|') {
      c->position_x = 20;
      c->position_y = (i16)((u16)c->position_y + 7u);
    }
    Canvas_DrawText(c, pt_text_span(p, 1));
    ++p;
  }
}
u32 DiallingCopyBackgroundToCurrent(void) {
  return BitBlt(game_ptr->background, 0, 160, current_bitmap, 12, 174, 192, 11);
}
void DrawWaitingForConnection(void) {
  SoundStuff2(10);
  DiallingCopyBackgroundToCurrent();
  temp_canvas->color = 15;
  (void)SetTempCanvasPosition(20, 184);
  DrawTextSlowly(terminal_waiting_message);
  Delay(120);
}
u16 DrawTerminalPhonebookEntry(u8 row, const u8 *label, u32 phone) {
  u16 y = 12u * row;
  Canvas_DrawRect(26, 30, y + 50u, 188, y + 62u);
  (void)SetTempCanvasPosition(144, y + 50u);
  RelativeLineTo(0, 11);
  y += 59;
  DrawText(34, y, label);
  const u8 *number;
  number = itoa_(pt_signed_dword(phone));
  u16 r = DrawText(151, y, number);

  return r;
}
static u8 history_count(u8 kind) {
  return player_ptr->city_part.num_last_dialled_number[kind];
}
static u8 history_index(u8 kind, u8 slot) {
  return (*pt_player_phone_history(player_ptr, kind, slot));
}
static u32 phone_number(u8 kind, u8 index) {
  return pt_town_phone_number(current_town, kind, index);
}
static u8 town_count(u8 kind) {
  return current_town->num_phone_nr_of_kind[kind];
}
u8 TryDialNumber(u32 number) {
  u8 status;
  status = 0;
  AdvanceClock(1);
  ConvertDialledNumberToString(number, dialled_number_string);
  for (u8 kind = 0; kind < 6; ++kind)
    for (u8 index = 0; index < town_count(kind); ++index) {
      u32 encoded = TransformDialledNumber(number);
      if (encoded != phone_number(kind, index))
        continue;
      if ((*pt_town_phone_connection(kind, index))) {
        DrawWaitingForConnection();
        status = 2;
        goto out;
      }
      for (u8 i = 0; i < history_count(kind); ++i)
        if (history_index(kind, i) == index)
          status = 1;
      if (!status) {
        u8 n = history_count(kind);
        player_ptr->city_part.num_last_dialled_number[kind] = (u8)(n + 1u);
        (*pt_player_phone_history(player_ptr, kind, n)) = index;
      }
      DrawWaitingForConnection();
      SoundStuff2(11);
      Delay(25);
      status = HandlePhoneCall(kind, index, pt_terminal_caption(current_town, kind, index), 0);
      if (has_many_players) {
        return 255;
      }
      goto out;
    }
  switch ((number >> 4) % 5u) {
  case 1:
    status = 2;
    break;
  case 2:
    status = 3;
    break;
  default:
    SoundStuff2(10);
    status = 0;
    break;
  }
out:
  if (status == 3)
    SoundStuff2(12);
  FillLetterBackground(4, 28, 9);
  (void)Do3x3ButtonStuff(32);
  DiallingCopyBackgroundToCurrent();
  temp_canvas->color = 15;
  (void)SetTempCanvasPosition(20, 184);
  DrawTextSlowly(terminal_dial_messages[status]);
  Delay(10);
  dial_number_position = 0;
  u8 r = status;

  return r;
}
u8 HandleNumberDialling(u8 digit) {
  if (digit)
    SoundStuff2(digit);
  SetSomeFieldInCanvas(0);
  temp_canvas->color = 26;
  if (!digit) {
    dial_number_position = 0;
    DiallingCopyBackgroundToCurrent();
    (void)SetTempCanvasPosition(48, 184);
    temp_canvas->color = 15;
    DrawTextSlowly(terminal_new_number_prompt);
    return 0;
  }
  (*pt_terminal_dial_digit(dial_number_position)) = digit;
  u8 *s;
  s = CopyDialNumberToStringBuffer(dial_number_buffer, dial_number_position);
  DrawText(108, 184, s);
  ++dial_number_position;
  if (dial_number_position < 7)
    return 0;
  u32 number = ConvertDialledNumberToInt(dial_number_buffer);
  TryDialNumber(number);
  if (has_many_players)
    return 255;
  (void)Do3x3ButtonStuff(32);
  dial_number_position = 0;
  return 1;
}
u8 HandleTerminalSubmenu(u8 kind) {
  u8 status;

restart:
  CheckGoalStuff();
  if (has_many_players) {
    return 255;
  }
  SetCurrentFont(font_piznumb);
  status = 0;
  (void)Do3x3ButtonStuff(32);
  FillLetterBackground(4, 28, 9);
  SetSomeFieldInCanvas(0);
  temp_canvas->color = 26;
  (void)DrawText2(51, 159, 43, pt_terminal_category_name(kind));
  u8 count = history_count(kind);
  if ((kind == 4 && (count < 3 || count > 7)) || (kind != 4 && count != town_count(kind)))
    player_ptr->city_part.num_last_dialled_number[kind] = town_count(kind);
  if (history_count(kind)) {
    for (u8 row = 0; row < history_count(kind); ++row) {
      u8 index = history_index(kind, row);
      u32 n = ExtractPhoneNrFromRam(phone_number(kind, index));
      DrawTerminalPhonebookEntry(row, pt_terminal_caption(current_town, kind, index), n);
    }
  } else
    (void)DrawText2(51, 159, 52, terminal_no_entries);
  for (;;) {
    terminal_wait();
    if (mouse_pressed == 1) {
      if ((IsMouseInside(34, 50, 183, 12u * history_count(kind) + 50u) != 0)) {
        HandleNumberDialling(0);
        u8 row = (u8)GetSelectedListItem(50, 12), index = history_index(kind, row);
        u32 n = ExtractPhoneNrFromRam(phone_number(kind, index));
        ConvertDialledNumberToString(n, dialled_number_string);
        for (unsigned j = 0; j < 7; ++j) {
          HandleNumberDialling(dialled_number_string[j]);
          Delay(10);
        }
        goto restart;
      }
      if ((IsMouseInside(34, 156, 148, 168) != 0)) {
        return 0;
      }
    }
    if (!pt_is_menu_button_event(mouse_pressed))
      goto restart;
    u8 r;
    if (!status) {
      if (mouse_pressed > PT_MENU_BUTTON_1) {
        return 0;
      }
      status = 1;
      terminal_manual_mode = 1;
      (void)Do3x3ButtonStuff(16);
      r = HandleNumberDialling(0);
      if (r)
        goto restart;
    } else {
      r = HandleNumberDialling((u8)(mouse_pressed + 17u));
      if (!r)
        continue;
      terminal_manual_mode = 0;
      CheckGoalStuff();
      if (has_many_players) {
        return 255;
      }
      goto restart;
    }
  }
}
u8 HandleTerminalMenu(void) {
  InitGraphicsMode();
restart:
  if (has_many_players || CheckGoalStuff() == 255 || has_many_players) {
    return 255;
  }
  bool manual = false;
  FillLetterBackground(4, 28, 9);
  (void)Do3x3ButtonStuff(32);
  SetCurrentFont(some_font_ptr);
  temp_canvas->color = 15;
  temp_canvas->background_color = 0;
  (void)SetTempCanvasPosition(20, 22);
  (void)DrawTextInsideList(terminal_title);
  (void)SetTempCanvasPosition(20, 29);
  (void)DrawTextInsideList(terminal_loading_message);
  LoadBlitmapAndOffsets(5);
  LoadMusic(8);
  SetSomeFieldInCanvas(0);
  SetCurrentFont(font_piznumb);
  for (unsigned row = 0; row < 5; ++row) {
    BitBlt(&bmp_blitmap, 0, 23u * row, current_bitmap, 50, 24u * row + 37u, 110, 22);
    temp_canvas->color = 26;
    (void)DrawText2(51, 159, 24u * row + 53u, pt_terminal_category_name(row));
  }
  HandleNumberDialling(0);
  SetSomeFieldInCanvas(1);
  for (;;) {
    terminal_wait();
    if (mouse_pressed == 1) {
      if ((IsMouseInside(49, 37, 158, 153) != 0)) {
        u8 r, row = (u8)GetSelectedListItem(37, 24);
        r = HandleTerminalSubmenu(row);
        if (r == 255) {
          return r;
        }
        goto restart;
      }
      continue;
    }
    if (!pt_is_menu_button_event(mouse_pressed))
      goto restart;
    u8 r;
    if (!manual) {
      if (mouse_pressed > PT_MENU_BUTTON_1) {
        return 0;
      }
      manual = true;
      terminal_manual_mode = 1;
      (void)Do3x3ButtonStuff(16);
      r = HandleNumberDialling(0);
      if (has_many_players) {
        return 255;
      }
      if (r && CheckGoalStuff())
        goto restart;
    } else {
      r = HandleNumberDialling((u8)(mouse_pressed + 17u));
      if (r == 2) {
        terminal_manual_mode = 0;

        return 0;
      }
      if (!r)
        continue;
      if (has_many_players) {
        return 255;
      }
      terminal_manual_mode = 0;
      goto restart;
    }
  }
}
