// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
// Input.

#include "input.h"
#include "objects.h"
#include "calendar.h"
#include "frame.h"

static u16 bios_keys[64];
static unsigned bios_key_head, bios_key_count;
static u8 keyboard_port = 0x80;
static PTMouseState mouse_platform;
static PTJoystickPlatform joystick;
bool pt_input_queue_valid(void) {
  return input_queue_capacity == sizeof(input_events) / sizeof(input_events[0]) &&
      input_queue_head < input_queue_capacity && input_queue_count <= input_queue_capacity;
}
bool pt_joystick_valid(const PTJoystickPlatform *state) {
  return state && state->present <= 1 && state->x <= 65535 && state->y <= 65535 && state->x2 <= 65535 &&
      state->y2 <= 65535 && state->buttons <= 15;
}
bool pt_joystick_load_platform(const PTJoystickPlatform *state) {
  if (pt_frame_active() || !pt_joystick_valid(state))
    return false;
  joystick = *state;
  return true;
}
bool pt_joystick_export_platform(PTJoystickPlatform *state) {
  if (!state)
    return false;
  *state = joystick;
  return true;
}
void pt_joystick_supply_platform(const PTJoystickPlatform *state) {
  if (!pt_joystick_valid(state))
    abort();
  joystick = *state;
}

bool pt_mouse_restore(const PTMouseState *state) {
  if (!state || pt_frame_active() || state->x > 65535 || state->y > 65535 || state->buttons > 3)
    return false;
  mouse_platform = *state;
  return true;
}
bool pt_mouse_ready_for_events(void) {
  return mouse_platform.ready && mouse_platform.callback;
}
u16 pt_mouse_dispatch_event(u16 x, u16 y, u16 buttons) {
  if (!pt_mouse_ready_for_events() || buttons > 3)
    abort();
  u16 mask = mouse_platform.x != x || mouse_platform.y != y;
  for (unsigned button = 0; button < 2; ++button)
    if ((mouse_platform.buttons ^ buttons) & (1u << button))
      mask |= (buttons & (1u << button) ? 2u : 4u) << (2 * button);
  if (mask)
    mouse_platform.callback(mask, x, y);
  pt_mouse_supply_platform(x, y, buttons);
  return mask;
}

void pt_mouse_position_platform(u16 x, u16 y) {
  if (!mouse_platform.ready)
    abort();
  mouse_platform.x = x;
  mouse_platform.y = y;
}
void pt_mouse_callback_platform(PTMouseEventCallback callback) {
  if (!mouse_platform.ready)
    abort();
  mouse_platform.callback = callback;
}
void pt_mouse_supply_platform(u16 x, u16 y, u16 buttons) {
  if (!mouse_platform.ready || buttons > 3)
    abort();
  mouse_platform.x = x;
  mouse_platform.y = y;
  mouse_platform.buttons = buttons;
}
bool pt_input_load_platform(const PTInputPlatform *state) {
  if (!state || state->count > 64 || state->keyboard_port > 255 || pt_frame_active())
    return false;
  for (unsigned i = 0; i < state->count; ++i)
    if (!state->keys[i])
      return false;
  memcpy(bios_keys, state->keys, sizeof(bios_keys));
  bios_key_head = 0;
  bios_key_count = state->count;
  keyboard_port = (u8)state->keyboard_port;
  return true;
}

void pt_input_reset_platform(void) {
  pt_mouse_bindings_reset();
  bios_key_head = bios_key_count = 0;
  keyboard_port = 0x80;
  mouse_platform = (PTMouseState){0};
  joystick = (PTJoystickPlatform){0};
}
void pt_input_supply_keyboard_port(u8 value) {
  keyboard_port = value;
}
bool pt_input_can_accept_key(void) {
  return bios_key_count < 64;
}

void pt_input_supply(u32 ticks, u16 key) {
  pt_platform_clock()->ticks = ticks;
  if (key) {
    if (!pt_input_can_accept_key())
      abort();
    bios_keys[(bios_key_head + bios_key_count) % 64] = key;
    ++bios_key_count;
  }
}

// Original register ABI is AX=class, BX=value, CX=x, DX=y.

u32 EnqueueInputEvent(u16 event_class, u16 value, u16 x, u16 y) {
  if (!(input_event_filter & event_class))
    return event_class;
  u32 last = input_queue_head;
  if (input_queue_count)
    last += input_queue_count - 1;
  if (last >= input_queue_capacity)
    last -= input_queue_capacity;
  InputEvent *event = &input_events[last];
  bool combine = event_class == 4 && input_queue_count && event->event_class == 4;
  if (input_queue_count == input_queue_capacity && !combine)
    return event_class;
  if (combine) {
    value = (u16)(event->value + 1u);
  } else {
    if (input_queue_count) {
      ++last;
      if (last == 80)
        // hard-coded queue end in original
        last = 0;
      event = &input_events[last];
    }
    ++input_queue_count;
  }
  event->event_class = event_class;
  event->value = value;
  event->x = x;
  event->y = y;
  return event_class;
}

// Drain BIOS keys even when the event filter drops them.

void PollKeyboardInput(void) {
  while (bios_key_count) {
    last_bios_key = bios_keys[bios_key_head];
    bios_key_head = (bios_key_head + 1) % 64;
    --bios_key_count;
    EnqueueInputEvent(2, last_bios_key, mouse_pos_x, mouse_pos_y);
  }
}

// Guest pointer to a retained queue slot, or zero.

InputEvent *DequeueInputEvent(void) {
  PollKeyboardInput();
  if (!input_queue_count)
    return NULL;
  InputEvent *result = &input_events[input_queue_head];
  --input_queue_count;
  ++input_queue_head;
  if (input_queue_head == input_queue_capacity)
    input_queue_head = 0;
  return result;
}
u32 GetInputEventCount(void) {
  PollKeyboardInput();
  return input_queue_count;
}
InputEvent *ClearInputEvents(void) {
  while (DequeueInputEvent()) {
  }
  return NULL;
}

// Yield at the input clock boundary before reading the native clock.

u32 ReadPlatformTicks(void) {
  pt_frame_poll(PT_FRAME_POLL_CLOCK);
  return pt_platform_clock()->ticks;
}
// Temporarily toggles keyboard filtering around IN 0x60, then restores the saved low word as
// a dword, clearing the original high word.

u32 ReadKeyboardPort(void) {
  // A held key can reach a room bound without another presentation.
  // Keep its actual stock polling entry as a recorded input boundary.

  pt_frame_poll(PT_FRAME_POLL_KEYBOARD);
  u16 saved = (u16)input_event_filter;
  input_event_filter ^= 2u;
  u32 value = keyboard_port;
  input_event_filter = saved;
  return value;
}
// Is also the held-left-button predicate during scrolling.

u16 WantSkipGameIntro(void) {
  return (mouse_buttons & 1u) != 0;
}

u16 MouseThingy(void) {
  return (mouse_buttons & 2u) != 0;
}

// Unsigned arithmetic matches x86. Command preflight stops waits requiring future clock/input
// samples until the scheduler is reconstructed.

u32 WaitForInputOrTimer(u8 flags) {
  u32 result = 0;
  do {
    u32 ticks = ReadPlatformTicks();

    if (ticks < last_input_tick)
      last_input_tick = ticks;
    if ((flags & 16) && ticks - last_second_tick >= 18) {
      result |= 16;
      last_second_tick += 18;
    }
    if ((flags & 8) && ticks > last_input_tick) {
      last_input_tick = ticks;
      result |= 8;
    }
    if (result)
      break;

    u16 count = (u16)GetInputEventCount();

    if (count)
      break;
  } while (true);

  return result;
}
void SetInputEventFilter(Mouse *mouse, u32 filter) {
  mouse->dwordC = filter;
  input_event_filter = filter;
}
void SaveAndSetInputEventFilter(u32 filter) {
  Mouse *mouse = mouse_result;
  game_ptr->dwordF9 = mouse->dwordC;
  SetInputEventFilter(mouse, filter);
}

// Prove the wait can finish using only the recorded sample and retained queues.
// BIOS polling drains all keys before the first dequeue, dropping keys when
// the fixed event queue is full. No future sample is fabricated here.

// The meaningful result is the low 16 bits.

u16 ReadMouseEvent(void) {
  for (;;) {
    last_input_event = DequeueInputEvent();

    if (last_input_event)
      break;

    WaitForInputOrTimer(0);
  }
  const InputEvent *event = last_input_event;
  active_input_class = event->event_class;
  active_input_value = event->value;
  mouse_x = (i16)event->x;
  mouse_y = (i16)event->y;
  return pt_decode_mouse_event(event->event_class, event->value);
}

// Left-click decoder result, also accepting the BIOS value one.

u16 WaitForMouseClick2(void) {
  u16 result;
  do {
    result = ReadMouseEvent();

  } while (result != 1);
  return result;
}

// Release decoder result, also accepting the BIOS value three.

u16 WaitForMouseLeave(void) {
  u16 result;
  do {
    result = ReadMouseEvent();

  } while (result != 3);
  return result;
}

// Queue contents visible after the first BIOS poll, without consuming either
// retained input FIFO. Capacity/head validation belongs to the command API.

// BIOS axes are WORDs. SUB's signed branch tests the mathematical difference,
// then CMP observes the wrapped WORD, including NEG(0x8000).

static int joystick_axis(u16 value, u16 center) {
  i32 difference = (i32)(i16)value - (i16)center;
  u16 distance = (u16)(value - center);
  if (difference <= 0)
    distance = (u16)(0u - distance);
  if ((i16)distance < (i32)(center >> 1))
    return 0;
  return difference > 0 ? 1 : -1;
}
// Original bug: both BIOS callers ignored CF, consuming incidental registers.
// Accept neutral failure only for the explicitly labelled machine-code repair.

u16 ReadJoystickCrap2(void) {
  if (command_line_arg) {
    if (!joystick.present) {

      return 255;
    }

    int vertical = joystick_axis((u16)joystick.y, joystick_center_y);
    if (vertical)
      return vertical > 0 ? 0 : 2;
    int horizontal = joystick_axis((u16)joystick.x, joystick_center_x);
    if (horizontal)
      return horizontal > 0 ? 3 : 1;
  }
  return 255;
}
u32 ReadJoystickCrap2_(void) {
  u32 value = ReadJoystickCrap2();

  return value;
}
u16 ReadJoystickCrap(void) {
  if (command_line_arg) {
    if (!joystick.present) {

      return 0;
    }

    return joystick.buttons != 0;
  }
  return 0;
}

// Buffered input.

#include "character.h"
#include "graphics.h"

u32 LoadFileIntoFileLoaderBuffer(const u8 *filename, u32 size) {
  u32 result = 0;
  u8 *buffer = AllocateBufferOrMessage(size);
  game_ptr->read_buffer.allocation = buffer;
  if (!buffer) {
    return 0;
  }
  game_ptr->loading_buffer_size = size;
  pt_game_read_set_cursor(game_ptr, buffer, size);
  DisableDiskReadErrors();
  u32 opened = OpenFileAsCurrent(filename);
  if ((u16)opened) {
    ReadBytesFromCurrentFile(pt_loading_buffer(), size);
    game_ptr->is_loading_from_buffer = 1;
    CloseFile(cur_loading_filehandle);
  }
  EnableDiskReadErrors();
  return result;
}

// Interaction.

// Stock stores a word across each byte field and its retained neighbour.
// Keep both bytes, even when callers normally provide a colour below 256.

void Canvas_SetColor(Canvas *canvas, u16 color) {
  canvas->color = color;
}
void Canvas_SetFieldE(Canvas *canvas, u16 value) {
  canvas->background_color = value;
}
void Canvas_SetByte10(Canvas *canvas, u8 value) {
  canvas->text_background_enabled = value;
}
// Stores AX but returns the complete input DWORD.

u32 Canvas_SetLinePattern(Canvas *canvas, u32 value) {
  canvas->line_pattern = (u16)value;
  return value;
}
Canvas *Canvas_SetPosition(Canvas *canvas, i16 x, i16 y) {
  canvas->position_x = x;
  canvas->position_y = y;
  return canvas;
}
Canvas *Canvas_SetPattern(Canvas *canvas, const void *pattern, u8 value) {
  canvas->fillrect_pattern_value = value;

  canvas->pattern = pattern;
  return canvas;
}
void Canvas_SetFont(Canvas *canvas, const Font *font) {
  canvas->font = font;
  // Font may overlap Canvas; each read follows the preceding write.

  canvas->font_spacing_mode = font->spacing_mode;

  canvas->font_height = (i16)font->height;

  canvas->font_baseline = font->baseline;
}
bool IsMouseInside(i16 left, i16 top, i16 right, i16 bottom) {
  return mouse_x >= left && mouse_x < right && mouse_y >= top && mouse_y < bottom;
}
u8 MouseInsideFlagsToButton(u16 flags) {
  switch (flags) {
  case 5:
  case 0x4737:
    return 0;
  case 4:
  case 0x4838:
    return 1;
  case 6:
  case 0x4939:
    return 2;
  case 1:
  case 0x4b34:
    return 3;
  case 0:
  case 0x4c35:
    return 4;
  case 2:
  case 0x4d36:
    return 5;
  case 9:
  case 0x4f31:
    return 6;
  case 8:
  case 0x5032:
    return 7;
  case 10:
  case 0x5133:
    return 8;
  default:
    return 10;
  }
}
static bool positive_difference(i32 a, i32 b) {
  // Stock SUB followed by TEST discards arithmetic overflow.

  u32 bits = (u32)a - (u32)b;
  return bits && !(bits & UINT32_C(0x80000000));
}
u32 GetMouseInsideFlags(i32 left, i32 right, i32 top, i32 bottom) {
  const Mouse *mouse = mouse_result;
  return (u32)positive_difference(left, mouse->x) | ((u32)positive_difference(mouse->x, right) << 1) |
      ((u32)positive_difference(top, mouse->y) << 2) | ((u32)positive_difference(mouse->y, bottom) << 3);
}

// Mouse startup.

#include "runtime_state.h"
#include <assert.h>

u16 MouseFunc11(void) {
  if (cursor_is_visible) {
    HideMouseCursor();
  }
  return UINT16_MAX;
}
u16 MouseFunc16(Mouse *object) {
  Jatoba *parent = object->parent;
  parent->mouse = NULL;
  return MouseFunc11();
}
u16 MouseReset(void) {
  cursor_is_visible = 0;
  did_hide_mouse_cursor = 0;
  cursor_hide_depth = 1;
  halve_mouse_x = 0;
  mouse_binding_count = 0;
  cursor_offset_x = 0;
  cursor_offset_y = 0;
  pt_mouse_position_platform(600, 10);
  halve_mouse_x = 1;
  pt_mouse_position_platform(320, 100);
  MoveMousePosition(160, 100);
  return 1;
}
u16 BindMousePosition(PTMouseCoordinate *x, PTMouseCoordinate *y) {
  u16 index = mouse_binding_count;
  assert(index < sizeof(mouse_position_bindings) / sizeof(mouse_position_bindings[0]));
  mouse_position_bindings[index].x = x;
  x->value = mouse_pos_x;

  mouse_position_bindings[index].y = y;
  y->value = mouse_pos_y;

  mouse_binding_count = (u16)(index + 1);
  return mouse_binding_count;
}
u32 LockFunctionInRam(PTMouseEventCallback routine, u32 size) {
  (void)routine;
  (void)size;
  // Native C callbacks require no DOS page-lock service.

  u32 ready = 1;
  return ready;
}
void MouseInterrupt(u16 mask, u16 x, u16 y) {
  (void)HandleMouseDriverEvent(mask, x, y);
}
Mouse *InitializeMouse(const MouseInitializer *initializer) {
  Mouse *mouse = pt_mouse_create();
  Jatoba *parent = initializer->parent;
  assert(mouse != NULL);
  mouse->parent = parent;
  parent->mouse = mouse;
  mouse->canvas = parent->canvas;
  input_event_filter = (u32)initializer->field_0;
  mouse->input_filter = input_event_filter;

  mouse_bitmap_2.width = 11;
  mouse_bitmap_1.width = 11;
  mouse_bitmap_2.height = 11;
  mouse_bitmap_1.height = 11;
  pt_bitmap_bind_pixels(&mouse_bitmap_1, mouse_bitmap_data_1, sizeof(mouse_bitmap_data_1));
  pt_bitmap_bind_pixels(&mouse_bitmap_2, mouse_bitmap_data_2, sizeof(mouse_bitmap_data_2));
  mouse_bitmap_2.mask_enabled = 0;
  mouse_bitmap_1.mask_enabled = 0;
  cursor_bitmap = &mouse_bitmap_1;
  cursor_background_bitmap = &mouse_bitmap_2;
  (void)MouseReset();
  (void)BindMousePosition(((PTMouseCoordinate *)(&mouse->x)), ((PTMouseCoordinate *)(&mouse->y)));
  (void)BindMousePosition(&parent->bound_mouse_x, &parent->bound_mouse_y);
  u32 locked = LockFunctionInRam(MouseInterrupt, pt_mouse_callback_lock_size());
  if (!locked) {
    return NULL;
  }
  pt_mouse_register_callback();
  return mouse;
}

// Wait click.

#include "messages.h"
#include "text.h"
#include "menu.h"
#include "sound.h"

bool pt_click_contains(i16 x, i16 y, int left, int top, int right, int bottom) {
  return x >= left && x < right && y >= top && y < bottom;
}

// deliberately reads Mouse, which can differ from the retained event.

PTClickDecision pt_click_decide(const Game *state, u8 menu, u16 cls, u16 value, i16 x, i16 y) {
  if (cls == 2) {
    u8 button = menu == 255 ? 10 : MouseInsideFlagsToButton(value);
    return button < 10 && value > 0x0b30 ? (PTClickDecision){CLICK_BUTTON, button}
                                         : (PTClickDecision){CLICK_RETURN, value};
  }
  if (cls != 1)
    return (PTClickDecision){CLICK_RETURN, 0};
  if (!state->byte107) {
    if (value == 2)
      return (PTClickDecision){CLICK_RETURN, 1};
    if (value == 4 || value == 8 || value == 16)
      return (PTClickDecision){CLICK_NEXT, 0};
  }
  if (pt_click_contains(x, y, 12, 14, 203, 189) || force_map_input) {
    if (value == 2)
      return (PTClickDecision){CLICK_RETURN, 1};
    if (value == 8)
      return (PTClickDecision){CLICK_RETURN, 2};
    if (value == 4 || value == 16)
      return (PTClickDecision){CLICK_NEXT, 0};
  }
  if (pt_click_contains(x, y, 224, 10, 315, 102) && state->byte87) {
    return (PTClickDecision){value == 2 ? CLICK_INFO : value == 8 ? CLICK_HELP : CLICK_NEXT, 0};
  }
  if (pt_click_contains(x, y, 228, 118, 321, 201) && value == 2) {
    if ((u8)state->current_button_menu == 255)
      return (PTClickDecision){CLICK_RETURN, 1};
    return (PTClickDecision){CLICK_BUTTON, MouseInsideFlagsToButton(GetMouseInsideFlags(255, 283, 143, 170))};
  }
  if (pt_click_contains(x, y, 0, 0, 203, 12))
    return (PTClickDecision){CLICK_RETURN, 0x4800};
  if (pt_click_contains(x, y, 0, 189, 203, 200))
    return (PTClickDecision){CLICK_RETURN, 0x5000};
  if (pt_click_contains(x, y, 0, 0, 12, 189))
    return (PTClickDecision){CLICK_RETURN, 0x4b00};
  if (pt_click_contains(x, y, 203, 0, 215, 200))
    return (PTClickDecision){CLICK_RETURN, 0x4d00};
  return (PTClickDecision){CLICK_NEXT, 0};
}
bool pt_click_timer_returns(u8 hierarchy, u8 phase) {
  return hierarchy && ((hierarchy != 2 && hierarchy != 4) || (phase & 1));
}
static u32 wait_input_timer(u8 flags) {
  u32 result = WaitForInputOrTimer(flags);

  return result;
}
static void retain_event(bool coordinates) {
  const InputEvent *event = last_input_event;
  active_input_class = event->event_class;
  active_input_value = event->value;
  if (coordinates) {
    mouse_x = (i16)event->x;
    mouse_y = (i16)event->y;
  }
}
void PizzaCompetitionSoundStuff(void) {
  if (sound_config_flag && competition_sound_pending && !pt_music_is_active()) {
    SoundStuff2(competition_sound_id);
  }
}
// Captured menu, retained queue slots, and meaningful AX return.

static u16 wait_mouse_body(void) {
  u8 menu = (u8)game_ptr->current_button_menu;
  for (;;) {
    HandleMouseOver3x3Buttons();

    PizzaCompetitionSoundStuff();

    last_input_event = DequeueInputEvent();
    if (!last_input_event) {
      if (wait_input_timer(8) & 8) {
        if (++input_timer_phase == 18) {
          input_timer_phase = 0;
          WaitForMouseClick_Helper();
        }
        if (pt_click_timer_returns(game_ptr->in_menu_hierarchy, input_timer_phase)) {
          mouse_pressed = 10;
          return mouse_pressed;
        }
      }
      continue;
    }
    retain_event(true);
    if (active_input_class == 4) {
      mouse_pressed = 0;
      bool nonmovement = false;
      while (((last_input_event = DequeueInputEvent()))) {
        retain_event(false);
        if (active_input_class != 4) {
          nonmovement = true;
          break;
        }
      }
      const Mouse *mouse = mouse_result;
      mouse_x = mouse->x;
      mouse_y = mouse->y;
      if (!nonmovement)
        return mouse_pressed;
    }
    PTClickDecision decision =
        pt_click_decide(game_ptr, menu, (u16)active_input_class, active_input_value, mouse_x, mouse_y);
    switch (decision.action) {
    case CLICK_RETURN:
      mouse_pressed = decision.value;
      return mouse_pressed;
    case CLICK_NEXT:
      continue;
    case CLICK_INFO:
      game_ptr->info_box_state = 0;
      ptr_apple->message_ticks_left = 0;

      PrintGenericInfoBox();
      continue;
    case CLICK_HELP:

      ShowMessage(14, NULL);
      continue;
    case CLICK_BUTTON:
      break;
    }
    u8 button = (u8)decision.value;

    if (!g_button_config[menu][button])
      continue;
    if (g_button_config[menu][button] == 4)
      --button;
    if (g_button_config[menu][button] == 5)
      button = (u8)(button - 3u);
    u16 slot = UINT16_MAX;
    for (unsigned i = 0; i <= button; ++i)
      if (g_button_config[menu][i] > 0 && g_button_config[menu][i] < 4)
        ++slot;

    Do3x3ButtonStuff_Helper2(
        button_icon_ids[menu][slot], g_button_config[menu][button], button_x[button][0], button_y[button][0], 1);
    if (active_input_class == 2) {
      for (;;) {
        u32 port = ReadKeyboardPort();

        if (port & 0x80)
          break;

        Delay(10);
      }
    } else {
      do {
        while (!((last_input_event = DequeueInputEvent())))
          wait_input_timer(0);
        retain_event(false);
      } while (active_input_class != 1 || active_input_value != 4);
    }

    Do3x3ButtonStuff_Helper2(
        button_icon_ids[menu][slot], g_button_config[menu][button], button_x[button][0], button_y[button][0], 0);
    mouse_pressed = (u16)(slot + PT_MENU_BUTTON_1);
    return mouse_pressed;
  }
}

u16 WaitForMouseClick(void) {
  u16 result = wait_mouse_body();

  return result;
}

// Protect inputs used by later loop iterations, in addition to each callee's
// internal ownership checks. These pages can contain arbitrary unknown RAM;
