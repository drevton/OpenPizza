// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
#ifndef PT_INPUT_H
#define PT_INPUT_H
#include "layout.h"
#include "objects.h"
#include "command.h"
#include "files.h"

// Input.

#include <stdbool.h>
typedef struct PTInputPlatform {
  u32 count, keyboard_port;
  u16 keys[64];
} PTInputPlatform;
_Static_assert(sizeof(PTInputPlatform) == 136, "input platform ABI");
typedef struct PTJoystickPlatform {
  u32 present, x, y, x2, y2, buttons;
} PTJoystickPlatform;
_Static_assert(sizeof(PTJoystickPlatform) == 24, "joystick platform ABI");
bool pt_joystick_load_platform(const PTJoystickPlatform *state);
bool pt_joystick_export_platform(PTJoystickPlatform *state);
bool pt_joystick_valid(const PTJoystickPlatform *state);
void pt_joystick_supply_platform(const PTJoystickPlatform *state);
typedef void (*PTMouseEventCallback)(u16 mask, u16 x, u16 y);
typedef struct PTMouseState {
  u32 x, y, buttons;
  PTMouseEventCallback callback;
  bool ready;
} PTMouseState;
bool pt_mouse_restore(const PTMouseState *state);
bool pt_mouse_ready_for_events(void);
u16 pt_mouse_dispatch_event(u16 x, u16 y, u16 buttons);
void pt_mouse_position_platform(u16 x, u16 y);
void pt_mouse_callback_platform(PTMouseEventCallback callback);
void pt_mouse_supply_platform(u16 x, u16 y, u16 buttons);
bool pt_input_load_platform(const PTInputPlatform *state);
// BIOS keys are scan-code/ASCII AX values. Host inputs arrive through commands.

void pt_input_reset_platform(void);
bool pt_input_can_accept_key(void);
bool pt_input_queue_valid(void);
void pt_input_supply(u32 ticks, u16 key);
void pt_input_supply_keyboard_port(u8 value);
u32 EnqueueInputEvent(u16 event_class, u16 value, u16 x, u16 y);
void PollKeyboardInput(void);
InputEvent *DequeueInputEvent(void);
u32 GetInputEventCount(void);
InputEvent *ClearInputEvents(void);
u32 ReadPlatformTicks(void);
u32 ReadKeyboardPort(void);
u16 ReadJoystickCrap2(void);
u32 ReadJoystickCrap2_(void);
u16 ReadJoystickCrap(void);
u16 WantSkipGameIntro(void);
u16 MouseThingy(void);
u32 WaitForInputOrTimer(u8 flags);
u16 ReadMouseEvent(void);
u16 WaitForMouseClick2(void);
u16 WaitForMouseLeave(void);
void SetInputEventFilter(Mouse *mouse, u32 filter);
void SaveAndSetInputEventFilter(u32 filter);

// Mouse startup.

Mouse *InitializeMouse(const MouseInitializer *initializer);
u16 MouseReset(void);
u16 BindMousePosition(PTMouseCoordinate *x, PTMouseCoordinate *y);
u32 LockFunctionInRam(PTMouseEventCallback routine, u32 size);
void MouseInterrupt(u16 mask, u16 x, u16 y);
u16 MouseFunc11(void);
u16 MouseFunc16(Mouse *object);

// Interaction.

#include <stdbool.h>

void Canvas_SetColor(Canvas *canvas, u16 color);
void Canvas_SetFieldE(Canvas *canvas, u16 value);
void Canvas_SetByte10(Canvas *canvas, u8 value);
u32 Canvas_SetLinePattern(Canvas *canvas, u32 value);
Canvas *Canvas_SetPosition(Canvas *canvas, i16 x, i16 y);
Canvas *Canvas_SetPattern(Canvas *canvas, const void *pattern, u8 value);
void Canvas_SetFont(Canvas *canvas, const Font *font);
bool IsMouseInside(i16 left, i16 top, i16 right, i16 bottom);
u8 MouseInsideFlagsToButton(u16 flags);
u32 GetMouseInsideFlags(i32 left, i32 right, i32 top, i32 bottom);

// Mouse platform.

// Original lock extent is platform bookkeeping; callbacks are native.

enum { PT_MOUSE_CALLBACK_LOCK_BYTES = 26 };
static inline u32 pt_mouse_callback_lock_size(void) {
  return PT_MOUSE_CALLBACK_LOCK_BYTES;
}
static inline void pt_mouse_register_callback(void) {
  pt_mouse_callback_platform(MouseInterrupt);
}

// Wait click.

#include <stdbool.h>

// WaitForMouseClick numbers enabled toolbar buttons in display order.
// These codes can coexist with raw pointer and keyboard events.

typedef enum PTMenuButtonEvent {
  PT_MENU_BUTTON_1 = 240,
  PT_MENU_BUTTON_2 = 241,
  PT_MENU_BUTTON_3 = 242,
  PT_MENU_BUTTON_4 = 243,
  PT_MENU_BUTTON_5 = 244,
  PT_MENU_BUTTON_6 = 245,
  PT_MENU_BUTTON_7 = 246,
  PT_MENU_BUTTON_8 = 247,
  PT_MENU_BUTTON_9 = 248,
} PTMenuButtonEvent;

static inline bool pt_is_menu_button_event(u32 event) {
  return event >= PT_MENU_BUTTON_1 && event <= PT_MENU_BUTTON_9;
}

u16 WaitForMouseClick(void);
void PizzaCompetitionSoundStuff(void);
typedef enum PTClickAction { CLICK_RETURN, CLICK_NEXT, CLICK_BUTTON, CLICK_INFO, CLICK_HELP } PTClickAction;
typedef struct PTClickDecision {
  PTClickAction action;
  u16 value;
} PTClickDecision;

bool pt_click_contains(i16 x, i16 y, int left, int top, int right, int bottom);
PTClickDecision pt_click_decide(const Game *state, u8 menu, u16 cls, u16 value, i16 x, i16 y);
bool pt_click_timer_returns(u8 hierarchy, u8 phase);

// Buffered input.

u32 LoadFileIntoFileLoaderBuffer(const u8 *filename, u32 size);

// Input decode.

static inline u16 pt_decode_mouse_event(u16 event_class, u16 value) {
  if (event_class == 1) {
    switch (value) {
    case 2:
      return 1;
    case 4:
      return 3;
    case 8:
      return 2;
    case 16:
      return 4;
    }
  }
  return event_class == 2 ? value : 0;
}

#endif
