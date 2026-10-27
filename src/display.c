// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
// Display.

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include "display.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

static SDL_Window *window;
static SDL_Renderer *renderer;
static SDL_Texture *texture;
static SDL_AudioDeviceID audio_device;
static u32 pointer_buttons;
static i32 pointer_x, pointer_y;
static bool pointer_release_pending;
// Host BIOS service state: DOS Alt-number-pad accumulator, modulo 256.

static u8 alt_numeric_token;
// Ownership lasts for the whole physical press, even if Command is released
// before the letter or the key repeats. Synthetic producers without physical
// scancodes use a separate slot indexed by the DOS scan.

static u8 key_owner[SDL_NUM_SCANCODES + 128];
enum { KEY_GUEST = 1, KEY_HOST = 2 };
static SDL_GameController *controller;
static bool controller_pending, controller_attached;
_Static_assert(sizeof(PTDisplayEvent) == 32, "display event ABI");

void pt_display_close(void) {
  SDL_GameControllerClose(controller);
  controller = NULL;
  controller_pending = controller_attached = false;
  SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER);
  if (audio_device)
    SDL_CloseAudioDevice(audio_device);
  audio_device = 0;
  SDL_QuitSubSystem(SDL_INIT_AUDIO);
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  texture = NULL;
  renderer = NULL;
  window = NULL;
  pointer_release_pending = false;
  alt_numeric_token = 0;
  memset(key_owner, 0, sizeof(key_owner));
  SDL_ShowCursor(SDL_ENABLE);
  SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_TIMER);
}

int pt_display_open(const char *title, unsigned scale, int hidden) {
  if (!title || scale < 1 || scale > 8 || window)
    return SDL_SetError("Invalid display parameters");
  SDL_SetMainReady();
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
  if (SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
    return -1;
  window = SDL_CreateWindow(title,
      SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED,
      320 * (int)scale,
      200 * (int)scale,
      (hidden ? SDL_WINDOW_HIDDEN : SDL_WINDOW_SHOWN) | SDL_WINDOW_RESIZABLE);
  if (!window)
    goto failed;
  SDL_SetWindowMinimumSize(window, 320, 200);
  renderer = SDL_CreateRenderer(window, -1, 0);
  if (!renderer)
    goto failed;
  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, 320, 200);
  if (!texture)
    goto failed;
  pointer_buttons = 0;
  pointer_release_pending = false;
  alt_numeric_token = 0;
  memset(key_owner, 0, sizeof(key_owner));
  pointer_x = pointer_y = 0;
  if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) == 0) {
    for (int i = 0; i < SDL_NumJoysticks() && !controller; ++i)
      if (SDL_IsGameController(i))
        controller = SDL_GameControllerOpen(i);
    controller_attached = controller != NULL;
    controller_pending = controller_attached;
  }
  // the original renderer owns its cursor
  SDL_ShowCursor(SDL_DISABLE);
  return 0;
failed:
  pt_display_close();
  return -1;
}

// SDL consumes an immutable presentation packet. All DMA state and sample
// timing belong to recorded game commands, never an SDL audio callback.

int pt_display_audio_write(const u8 *samples, size_t size) {
  if (size > 32768 || (size & 1u) || (!samples && size))
    return SDL_SetError("Invalid PCM packet");
  if (!size)
    return 0;
  if (!audio_device) {
    SDL_SetMainReady();
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
      return -1;
    SDL_AudioSpec wanted = {0}, obtained;
    wanted.freq = 44100;
    wanted.format = AUDIO_S16SYS;
    wanted.channels = 1;
    wanted.samples = 512;
    audio_device = SDL_OpenAudioDevice(NULL, 0, &wanted, &obtained, 0);
    if (!audio_device)
      return -1;
    SDL_PauseAudioDevice(audio_device, 0);
  }
  return SDL_QueueAudio(audio_device, samples, (Uint32)size);
}

static SDL_Rect display_rect(void) {
  int width = 320, height = 200;
  SDL_GetRendererOutputSize(renderer, &width, &height);
  int scale = width / 320 < height / 200 ? width / 320 : height / 200;
  if (scale < 1)
    scale = 1;
  return (SDL_Rect){(width - 320 * scale) / 2, (height - 200 * scale) / 2, 320 * scale, 200 * scale};
}

int pt_display_present(const u8 *rgb, size_t size, u8 *readback) {
  if (!texture || !rgb || size != 320 * 200 * 3)
    return SDL_SetError("Invalid RGB frame");
  SDL_Rect destination = display_rect();
  if (SDL_UpdateTexture(texture, NULL, rgb, 320 * 3) < 0 || SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255) < 0 ||
      SDL_RenderClear(renderer) < 0 || SDL_RenderCopy(renderer, texture, NULL, &destination) < 0)
    return -1;
  if (readback) {
    int width, height;
    if (SDL_GetRendererOutputSize(renderer, &width, &height) < 0)
      return -1;
    if (width != 320 || height != 200)
      return SDL_SetError("Readback requires a 320x200 output");
    if (SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_RGB24, readback, 320 * 3) < 0)
      return -1;
  }
  SDL_RenderPresent(renderer);
  return 0;
}

// The resident DOS BIOS profile uses the US scan table. SDL key symbols
// follow the host layout, whereas scancodes identify the physical key. A
// missing scancode remains supported for synthetic/legacy event producers.

static SDL_Keycode us_keyboard_key(const SDL_Keysym *key) {
  SDL_Scancode scan = key->scancode;
  if (scan >= SDL_SCANCODE_A && scan <= SDL_SCANCODE_Z)
    return SDLK_a + (scan - SDL_SCANCODE_A);
  if (scan >= SDL_SCANCODE_1 && scan <= SDL_SCANCODE_9)
    return SDLK_1 + (scan - SDL_SCANCODE_1);
  if (scan >= SDL_SCANCODE_F1 && scan <= SDL_SCANCODE_F12)
    return SDLK_F1 + (scan - SDL_SCANCODE_F1);
  if (scan >= SDL_SCANCODE_KP_1 && scan <= SDL_SCANCODE_KP_9)
    return SDLK_KP_1 + (scan - SDL_SCANCODE_KP_1);
  switch (scan) {
  case SDL_SCANCODE_0:
    return SDLK_0;
  case SDL_SCANCODE_RETURN:
    return SDLK_RETURN;
  case SDL_SCANCODE_ESCAPE:
    return SDLK_ESCAPE;
  case SDL_SCANCODE_BACKSPACE:
    return SDLK_BACKSPACE;
  case SDL_SCANCODE_TAB:
    return SDLK_TAB;
  case SDL_SCANCODE_SPACE:
    return SDLK_SPACE;
  case SDL_SCANCODE_MINUS:
    return SDLK_MINUS;
  case SDL_SCANCODE_EQUALS:
    return SDLK_EQUALS;
  case SDL_SCANCODE_LEFTBRACKET:
    return SDLK_LEFTBRACKET;
  case SDL_SCANCODE_RIGHTBRACKET:
    return SDLK_RIGHTBRACKET;
  case SDL_SCANCODE_BACKSLASH:
    return SDLK_BACKSLASH;
  case SDL_SCANCODE_SEMICOLON:
    return SDLK_SEMICOLON;
  case SDL_SCANCODE_APOSTROPHE:
    return SDLK_QUOTE;
  case SDL_SCANCODE_GRAVE:
    return SDLK_BACKQUOTE;
  case SDL_SCANCODE_COMMA:
    return SDLK_COMMA;
  case SDL_SCANCODE_PERIOD:
    return SDLK_PERIOD;
  case SDL_SCANCODE_SLASH:
    return SDLK_SLASH;
  case SDL_SCANCODE_CAPSLOCK:
    return SDLK_CAPSLOCK;
  case SDL_SCANCODE_PRINTSCREEN:
    return SDLK_PRINTSCREEN;
  case SDL_SCANCODE_SCROLLLOCK:
    return SDLK_SCROLLLOCK;
  case SDL_SCANCODE_PAUSE:
    return SDLK_PAUSE;
  case SDL_SCANCODE_INSERT:
    return SDLK_INSERT;
  case SDL_SCANCODE_HOME:
    return SDLK_HOME;
  case SDL_SCANCODE_PAGEUP:
    return SDLK_PAGEUP;
  case SDL_SCANCODE_DELETE:
    return SDLK_DELETE;
  case SDL_SCANCODE_END:
    return SDLK_END;
  case SDL_SCANCODE_PAGEDOWN:
    return SDLK_PAGEDOWN;
  case SDL_SCANCODE_RIGHT:
    return SDLK_RIGHT;
  case SDL_SCANCODE_LEFT:
    return SDLK_LEFT;
  case SDL_SCANCODE_DOWN:
    return SDLK_DOWN;
  case SDL_SCANCODE_UP:
    return SDLK_UP;
  case SDL_SCANCODE_NUMLOCKCLEAR:
    return SDLK_NUMLOCKCLEAR;
  case SDL_SCANCODE_KP_DIVIDE:
    return SDLK_KP_DIVIDE;
  case SDL_SCANCODE_KP_MULTIPLY:
    return SDLK_KP_MULTIPLY;
  case SDL_SCANCODE_KP_MINUS:
    return SDLK_KP_MINUS;
  case SDL_SCANCODE_KP_PLUS:
    return SDLK_KP_PLUS;
  case SDL_SCANCODE_KP_ENTER:
    return SDLK_KP_ENTER;
  case SDL_SCANCODE_KP_0:
    return SDLK_KP_0;
  case SDL_SCANCODE_KP_PERIOD:
    return SDLK_KP_PERIOD;
  case SDL_SCANCODE_LCTRL:
    return SDLK_LCTRL;
  case SDL_SCANCODE_LSHIFT:
    return SDLK_LSHIFT;
  case SDL_SCANCODE_LALT:
    return SDLK_LALT;
  case SDL_SCANCODE_LGUI:
    return SDLK_LGUI;
  case SDL_SCANCODE_RCTRL:
    return SDLK_RCTRL;
  case SDL_SCANCODE_RSHIFT:
    return SDLK_RSHIFT;
  case SDL_SCANCODE_RALT:
    return SDLK_RALT;
  case SDL_SCANCODE_RGUI:
    return SDLK_RGUI;
  case SDL_SCANCODE_NONUSHASH:
  case SDL_SCANCODE_NONUSBACKSLASH:
    return SDLK_BACKSLASH;
  default:
    return key->sym;
  }
}

static u16 bios_key(const SDL_KeyboardEvent *event) {
  static const u8 letter_scan[26] = {0x1e,
    0x30,
    0x2e,
    0x20,
    0x12,
    0x21,
    0x22,
    0x23,
    0x17,
    0x24,
    0x25,
    0x26,
    0x32,
    0x31,
    0x18,
    0x19,
    0x10,
    0x13,
    0x1f,
    0x14,
    0x16,
    0x2f,
    0x11,
    0x2d,
    0x15,
    0x2c};
  u8 scan = 0, ascii = 0;
  SDL_Keycode key = event->keysym.sym;
  u16 mod = event->keysym.mod;
  bool function = key >= SDLK_F1 && key <= SDLK_F12;
  if (mod & KMOD_GUI)
    return 0;
  // BIOS translation gives Alt priority over Control. Ctrl-Alt-Delete is
  // a machine reset, outside this input-only platform contract.

  if ((mod & KMOD_ALT) && (mod & KMOD_CTRL)) {
    if (key == SDLK_DELETE || key == SDLK_KP_PERIOD)
      return 0;
    mod &= (u16)~KMOD_CTRL;
  }
  // The extra ISO key has its own physical scan and US BIOS translation.
  // Control/Alt have no buffer word for this key in the selected table.

  if (event->keysym.scancode == SDL_SCANCODE_NONUSBACKSLASH) {
    if (mod & (KMOD_CTRL | KMOD_ALT))
      return 0;
    return (mod & KMOD_SHIFT) ? 0x567c : 0x565c;
  }
  if (function) {
    unsigned index = (unsigned)(key - SDLK_F1);
    unsigned base = mod & KMOD_ALT ? 0x68 : mod & KMOD_CTRL ? 0x5e : mod & KMOD_SHIFT ? 0x54 : 0x3b;
    if (index >= 10)
      base = mod & KMOD_ALT ? 0x8b : mod & KMOD_CTRL ? 0x89 : mod & KMOD_SHIFT ? 0x87 : 0x85;
    return (u16)((base + (index < 10 ? index : index - 10)) << 8);
  }
  // BIOS Alt number-row codes are distinct from the physical port-60h
  // scans. Option is Alt on macOS; Shift does not change these codes.

  if ((mod & KMOD_ALT) && !(mod & KMOD_CTRL)) {
    if (key >= SDLK_1 && key <= SDLK_9)
      return (u16)((0x78u + (unsigned)(key - SDLK_1)) << 8);
    if (key == SDLK_0)
      return 0x8100;
    if (key == SDLK_MINUS)
      return 0x8200;
    if (key == SDLK_EQUALS)
      return 0x8300;
    if (key == SDLK_SPACE)
      return 0x3920;
    // F0 and enhanced words are filtered by the existing legacy INT 16h
    // adapter below; their physical scans still reach port 60h.

    switch (key) {
    case SDLK_ESCAPE:
      return 0x01f0;
    case SDLK_BACKSPACE:
      return 0x0ef0;
    case SDLK_RETURN:
      return 0x1cf0;
    case SDLK_TAB:
      return 0xa5f0;
    case SDLK_LEFTBRACKET:
      return 0x1af0;
    case SDLK_RIGHTBRACKET:
      return 0x1bf0;
    case SDLK_BACKSLASH:
      return 0x2bf0;
    case SDLK_SEMICOLON:
      return 0x27f0;
    case SDLK_QUOTE:
      return 0x28f0;
    case SDLK_BACKQUOTE:
      return 0x29f0;
    case SDLK_COMMA:
      return 0x33f0;
    case SDLK_PERIOD:
      return 0x34f0;
    case SDLK_SLASH:
      return 0x35f0;
    case SDLK_KP_MULTIPLY:
      return 0x37f0;
    case SDLK_KP_MINUS:
      return 0x4af0;
    case SDLK_KP_PLUS:
      return 0x4ef0;
    case SDLK_KP_ENTER:
      return 0xa600;
    case SDLK_KP_DIVIDE:
      return 0xa400;
    case SDLK_HOME:
      return 0x9700;
    case SDLK_UP:
      return 0x9800;
    case SDLK_PAGEUP:
      return 0x9900;
    case SDLK_LEFT:
      return 0x9b00;
    case SDLK_RIGHT:
      return 0x9d00;
    case SDLK_END:
      return 0x9f00;
    case SDLK_DOWN:
      return 0xa000;
    case SDLK_PAGEDOWN:
      return 0xa100;
    case SDLK_INSERT:
      return 0xa200;
    case SDLK_DELETE:
      return 0xa300;
    default:
      break;
    }
  }
  // BIOS Control mappings for editing and navigation. Enhanced codes are
  // retained here so the legacy INT 16h filter below can discard them.

  if ((mod & KMOD_CTRL) && !(mod & KMOD_ALT)) {
    switch (key) {
    case SDLK_BACKSPACE:
      return 0x0e7f;
    case SDLK_RETURN:
    case SDLK_KP_ENTER:
      return 0x1c0a;
    case SDLK_ESCAPE:
      return 0x011b;
    case SDLK_SPACE:
      return 0x3920;
    case SDLK_2:
      return 0x0300;
    case SDLK_6:
      return 0x071e;
    case SDLK_MINUS:
      return 0x0c1f;
    case SDLK_LEFTBRACKET:
      return 0x1a1b;
    case SDLK_RIGHTBRACKET:
      return 0x1b1d;
    case SDLK_BACKSLASH:
      return 0x2b1c;
    case SDLK_LEFT:
    case SDLK_KP_4:
      return 0x7300;
    case SDLK_RIGHT:
    case SDLK_KP_6:
      return 0x7400;
    case SDLK_END:
    case SDLK_KP_1:
      return 0x7500;
    case SDLK_PAGEDOWN:
    case SDLK_KP_3:
      return 0x7600;
    case SDLK_HOME:
    case SDLK_KP_7:
      return 0x7700;
    case SDLK_PAGEUP:
    case SDLK_KP_9:
      return 0x8400;
    case SDLK_UP:
    case SDLK_KP_8:
      return 0x8d00;
    case SDLK_DOWN:
    case SDLK_KP_2:
      return 0x9100;
    case SDLK_INSERT:
    case SDLK_KP_0:
      return 0x9200;
    case SDLK_DELETE:
    case SDLK_KP_PERIOD:
      return 0x9300;
    case SDLK_TAB:
      return 0x9400;
    case SDLK_KP_MINUS:
      return 0x8e00;
    case SDLK_KP_5:
      return 0x8f00;
    case SDLK_KP_PLUS:
      return 0x9000;
    case SDLK_KP_MULTIPLY:
      return 0x9600;
    case SDLK_KP_DIVIDE:
      return 0x9500;
    default:
      break;
    }
  }
  // Other unsupported BIOS combinations retain an explicit stop.

  if (((mod & (KMOD_CTRL | KMOD_ALT)) && (key < SDLK_a || key > SDLK_z)) || ((mod & KMOD_CTRL) && (mod & KMOD_ALT)))
    return 0;
  if (key >= SDLK_a && key <= SDLK_z) {
    scan = letter_scan[key - SDLK_a];
    ascii = (u8)key;
    if (!!(mod & KMOD_SHIFT) != !!(mod & KMOD_CAPS))
      ascii -= 'a' - 'A';
    if (mod & KMOD_CTRL)
      ascii = (u8)(key - SDLK_a + 1);
  } else if (key >= SDLK_1 && key <= SDLK_9) {
    scan = (u8)(key - SDLK_1 + 2);
    ascii = (u8)((mod & KMOD_SHIFT) ? "!@#$%^&*("[key - SDLK_1] : key);
  } else if (key == SDLK_0) {
    scan = 0x0b;
    ascii = (mod & KMOD_SHIFT) ? ')' : '0';
  } else if (key >= SDLK_KP_1 && key <= SDLK_KP_9) {
    static const u8 keypad_scan[9] = {0x4f, 0x50, 0x51, 0x4b, 0x4c, 0x4d, 0x47, 0x48, 0x49};
    scan = keypad_scan[key - SDLK_KP_1];
    if (!!(mod & KMOD_NUM) != !!(mod & KMOD_SHIFT))
      ascii = (u8)('1' + key - SDLK_KP_1);
  } else {
    switch (key) {
    case SDLK_KP_0:
      scan = 0x52;
      ascii = (!!(mod & KMOD_NUM) != !!(mod & KMOD_SHIFT)) ? '0' : 0;
      break;
    case SDLK_KP_PERIOD:
      scan = 0x53;
      ascii = (!!(mod & KMOD_NUM) != !!(mod & KMOD_SHIFT)) ? '.' : 0;
      break;
    case SDLK_KP_DIVIDE:
      scan = 0x35;
      ascii = '/';
      break;
    case SDLK_KP_MULTIPLY:
      scan = 0x37;
      ascii = '*';
      break;
    case SDLK_KP_MINUS:
      scan = 0x4a;
      ascii = '-';
      break;
    case SDLK_KP_PLUS:
      scan = 0x4e;
      ascii = '+';
      break;
    case SDLK_ESCAPE:
      scan = 1;
      ascii = 27;
      break;
    case SDLK_BACKSPACE:
      scan = 14;
      ascii = 8;
      break;
    case SDLK_TAB:
      scan = 15;
      ascii = (mod & KMOD_SHIFT) ? 0 : 9;
      break;
    case SDLK_RETURN:
    case SDLK_KP_ENTER:
      scan = 28;
      ascii = 13;
      break;
    case SDLK_SPACE:
      scan = 57;
      ascii = 32;
      break;
    case SDLK_UP:
      scan = 0x48;
      break;
    case SDLK_DOWN:
      scan = 0x50;
      break;
    case SDLK_LEFT:
      scan = 0x4b;
      break;
    case SDLK_RIGHT:
      scan = 0x4d;
      break;
    case SDLK_HOME:
      scan = 0x47;
      break;
    case SDLK_END:
      scan = 0x4f;
      break;
    case SDLK_PAGEUP:
      scan = 0x49;
      break;
    case SDLK_PAGEDOWN:
      scan = 0x51;
      break;
    case SDLK_INSERT:
      scan = 0x52;
      break;
    case SDLK_DELETE:
      scan = 0x53;
      break;
    case SDLK_MINUS:
      scan = 12;
      ascii = (mod & KMOD_SHIFT) ? '_' : '-';
      break;
    case SDLK_EQUALS:
      scan = 13;
      ascii = (mod & KMOD_SHIFT) ? '+' : '=';
      break;
    case SDLK_COMMA:
      scan = 51;
      ascii = (mod & KMOD_SHIFT) ? '<' : ',';
      break;
    case SDLK_PERIOD:
      scan = 52;
      ascii = (mod & KMOD_SHIFT) ? '>' : '.';
      break;
    case SDLK_SLASH:
      scan = 53;
      ascii = (mod & KMOD_SHIFT) ? '?' : '/';
      break;
    case SDLK_SEMICOLON:
      scan = 39;
      ascii = (mod & KMOD_SHIFT) ? ':' : ';';
      break;
    case SDLK_QUOTE:
      scan = 40;
      ascii = (mod & KMOD_SHIFT) ? '"' : '\'';
      break;
    case SDLK_LEFTBRACKET:
      scan = 26;
      ascii = (mod & KMOD_SHIFT) ? '{' : '[';
      break;
    case SDLK_RIGHTBRACKET:
      scan = 27;
      ascii = (mod & KMOD_SHIFT) ? '}' : ']';
      break;
    case SDLK_BACKSLASH:
      scan = 43;
      ascii = (mod & KMOD_SHIFT) ? '|' : '\\';
      break;
    case SDLK_BACKQUOTE:
      scan = 41;
      ascii = (mod & KMOD_SHIFT) ? '~' : '`';
      break;
    default:
      break;
    }
  }
  if (mod & KMOD_ALT)
    ascii = 0;
  return (u16)((u16)scan << 8) | ascii;
}

// These Control combinations have no BIOS buffer entry. Physical port 60h
// still reports the key, so preserve its make/break pair without declaring
// an unsupported platform input. See SeaBIOS src/kbd.c scan_to_keycode.
// Alt combinations remain separate because Alt has its own BIOS table.

static bool control_key_without_bios(const SDL_KeyboardEvent *event) {
  if (!(event->keysym.mod & KMOD_CTRL) || (event->keysym.mod & KMOD_ALT))
    return false;
  switch (event->keysym.sym) {
  case SDLK_1:
  case SDLK_3:
  case SDLK_4:
  case SDLK_5:
  case SDLK_7:
  case SDLK_8:
  case SDLK_9:
  case SDLK_0:
  case SDLK_EQUALS:
  case SDLK_SEMICOLON:
  case SDLK_QUOTE:
  case SDLK_BACKQUOTE:
  case SDLK_COMMA:
  case SDLK_PERIOD:
  case SDLK_SLASH:
    return true;
  default:
    return false;
  }
}

// Port 60h reports physical scans, independent of BIOS modifier translation.
// F11/F12 are enhanced BIOS keys discarded by legacy INT 16h AH=00/01.

static u8 keyboard_scan(const SDL_KeyboardEvent *event) {
  if (event->keysym.scancode == SDL_SCANCODE_NONUSBACKSLASH)
    return 0x56;
  // Modifier/lock keys update BIOS flags without adding a BIOS-buffer
  // word, but the original IN 60h still observes their physical scans.
  // Right Control/Alt use the final byte of the enhanced key sequence,
  // matching the existing single-byte port sample contract.

  switch (event->keysym.sym) {
  case SDLK_LSHIFT:
    return 0x2a;
  case SDLK_RSHIFT:
    return 0x36;
  case SDLK_LCTRL:
  case SDLK_RCTRL:
    return 0x1d;
  case SDLK_LALT:
  case SDLK_RALT:
    return 0x38;
  case SDLK_CAPSLOCK:
    return 0x3a;
  case SDLK_NUMLOCKCLEAR:
    return 0x45;
  case SDLK_SCROLLLOCK:
    return 0x46;
  default:
    break;
  }
  if (event->keysym.sym == SDLK_F11)
    return 0x57;
  if (event->keysym.sym == SDLK_F12)
    return 0x58;
  SDL_KeyboardEvent physical = *event;
  physical.keysym.mod = 0;
  return (u8)(bios_key(&physical) >> 8);
}

static unsigned keyboard_owner_index(const SDL_KeyboardEvent *event, u8 scan) {
  unsigned physical = (unsigned)event->keysym.scancode;
  return physical > SDL_SCANCODE_UNKNOWN && physical < SDL_NUM_SCANCODES ? physical
                                                                         : (unsigned)SDL_NUM_SCANCODES + scan;
}

static void controller_sample(PTDisplayEvent *result) {
  *result = (PTDisplayEvent){
    .kind = PT_DISPLAY_JOYSTICK, .x = 1000, .y = 1000, .key = controller_attached, .milliseconds = SDL_GetTicks64()};
  if (!controller)
    return;
  result->x = ((i32)SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX) + 32768) * 2000 / 65535;
  result->y = ((i32)SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY) + 32768) * 2000 / 65535;
  if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT))
    result->x = 0;
  if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT))
    result->x = 2000;
  if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_UP))
    result->y = 0;
  if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_DOWN))
    result->y = 2000;
  for (unsigned i = 0; i < 4; ++i)
    result->buttons |=
        (u32)SDL_GameControllerGetButton(controller, (SDL_GameControllerButton)(SDL_CONTROLLER_BUTTON_A + i)) << i;
}
int pt_display_poll(PTDisplayEvent *result) {
  if (!window || !result)
    return 0;
  if (controller_pending) {
    controller_pending = false;
    controller_sample(result);
    return 1;
  }
  SDL_Event event;
  for (;;) {
    if (pointer_release_pending) {
      // Focus loss has already emitted the keyboard release. Deliver
      // the mouse release next through the ordinary recorded pointer
      // path, retaining the last window position and scaling rules.

      event = (SDL_Event){.type = SDL_MOUSEMOTION};
      event.motion.x = pointer_x;
      event.motion.y = pointer_y;
      pointer_release_pending = false;
    } else if (!SDL_PollEvent(&event))
      break;
    if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)
      event.key.keysym.sym = us_keyboard_key(&event.key.keysym);
    memset(result, 0, sizeof(*result));
    result->milliseconds = SDL_GetTicks64();
    result->modifiers = SDL_GetModState();
    if (event.type == SDL_CONTROLLERDEVICEADDED && !controller && SDL_IsGameController(event.cdevice.which)) {
      controller = SDL_GameControllerOpen(event.cdevice.which);
      if (controller) {
        controller_attached = true;
        controller_sample(result);
        return 1;
      }
    }
    if (event.type == SDL_CONTROLLERDEVICEREMOVED && controller &&
        event.cdevice.which == SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller))) {
      SDL_GameControllerClose(controller);
      controller = NULL;
      // Keep the emulated BIOS device connected and neutral on unplug.

      controller_sample(result);
      return 1;
    }
    if (controller &&
        ((event.type == SDL_CONTROLLERAXISMOTION &&
             event.caxis.which == SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller))) ||
            ((event.type == SDL_CONTROLLERBUTTONDOWN || event.type == SDL_CONTROLLERBUTTONUP) &&
                event.cbutton.which == SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller))))) {
      controller_sample(result);
      return 1;
    }
    if (event.type == SDL_QUIT) {
      result->kind = PT_DISPLAY_QUIT;
      return 1;
    }
    if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
      pointer_release_pending = pointer_buttons != 0;
      alt_numeric_token = 0;
      // Keep host-owned presses suppressed until their physical release.
      // The guest receives its existing focus-loss release sample below.

      for (unsigned i = 0; i < sizeof(key_owner); ++i)
        if (key_owner[i] == KEY_GUEST)
          key_owner[i] = 0;
      result->kind = PT_DISPLAY_KEYBOARD_PORT;
      result->key = 0x80;
      return 1;
    }
    if (event.type == SDL_KEYUP) {
      u8 scan = keyboard_scan(&event.key);
      if (!scan)
        continue;
      unsigned owner_index = keyboard_owner_index(&event.key, scan);
      u8 owner = key_owner[owner_index];
      key_owner[owner_index] = 0;
      if (owner == KEY_HOST || (owner != KEY_GUEST && (event.key.keysym.mod & KMOD_GUI)))
        continue;
      // BIOS emits the accumulated ASCII byte only when both Alt keys
      // are released. Focus loss cancels it above, preventing a phantom
      // character when returning to the window.

      if ((event.key.keysym.sym == SDLK_LALT || event.key.keysym.sym == SDLK_RALT) &&
          !(event.key.keysym.mod & KMOD_ALT) && alt_numeric_token) {
        result->kind = PT_DISPLAY_KEY;
        result->key = alt_numeric_token;
        result->keyboard_port = 0xb8;
        result->modifiers = event.key.keysym.mod;
        alt_numeric_token = 0;
        return 1;
      }
      result->kind = PT_DISPLAY_KEYBOARD_PORT;
      result->key = scan | 0x80u;
      result->modifiers = event.key.keysym.mod;
      return 1;
    }
    if (event.type == SDL_KEYDOWN) {
      // Command shortcuts and keys absent from the DOS keyboard belong
      // to the host. They must not terminate the game or invent BIOS
      // input. Unsupported combinations of real DOS keys still stop.

      u8 scan = keyboard_scan(&event.key);
      if (!scan)
        continue;
      unsigned owner_index = keyboard_owner_index(&event.key, scan);
      if ((event.key.keysym.mod & KMOD_GUI) || key_owner[owner_index] == KEY_HOST) {
        if (key_owner[owner_index] != KEY_GUEST)
          key_owner[owner_index] = KEY_HOST;
        continue;
      }
      key_owner[owner_index] = KEY_GUEST;
      result->modifiers = event.key.keysym.mod;
      result->keyboard_port = keyboard_scan(&event.key);
      if (event.key.keysym.sym == SDLK_LALT || event.key.keysym.sym == SDLK_RALT) {
        result->kind = PT_DISPLAY_KEYBOARD_PORT;
        result->key = 0x38;
        return 1;
      }
      // DOS BIOS Alt digit presses contribute to a BYTE token without
      // generating a BIOS key. Num Lock, Shift and Control do not change
      // Alt priority. The non-extended keypad period contributes zero,
      // matching the DOSBox BIOS profile; Ctrl-Alt-Delete stays a reset.

      if ((event.key.keysym.mod & KMOD_ALT) &&
          ((event.key.keysym.sym >= SDLK_KP_1 && event.key.keysym.sym <= SDLK_KP_9) ||
              event.key.keysym.sym == SDLK_KP_0 ||
              (event.key.keysym.sym == SDLK_KP_PERIOD && !(event.key.keysym.mod & KMOD_CTRL)))) {
        unsigned digit = event.key.keysym.sym >= SDLK_KP_1 && event.key.keysym.sym <= SDLK_KP_9
            ? (unsigned)(event.key.keysym.sym - SDLK_KP_1) + 1u
            : 0u;
        alt_numeric_token = (u8)((unsigned)alt_numeric_token * 10u + digit);
        result->kind = PT_DISPLAY_KEYBOARD_PORT;
        result->key = result->keyboard_port;
        return 1;
      }
      result->key = bios_key(&event.key);
      if ((result->key >> 8) > 0x84 || ((result->key & 0xffu) == 0xf0 && (result->key >> 8))) {
        result->kind = PT_DISPLAY_KEYBOARD_PORT;
        result->key = result->keyboard_port;
        return 1;
      }
      if (!result->key) {
        if (control_key_without_bios(&event.key) || event.key.keysym.scancode == SDL_SCANCODE_NONUSBACKSLASH) {
          result->kind = PT_DISPLAY_KEYBOARD_PORT;
          result->key = result->keyboard_port;
          return 1;
        }
        switch (event.key.keysym.sym) {
        case SDLK_LSHIFT:
        case SDLK_RSHIFT:
        case SDLK_LCTRL:
        case SDLK_RCTRL:
        case SDLK_LALT:
        case SDLK_RALT:
        case SDLK_CAPSLOCK:
        case SDLK_NUMLOCKCLEAR:
        case SDLK_SCROLLLOCK:
          result->kind = PT_DISPLAY_KEYBOARD_PORT;
          result->key = result->keyboard_port;
          return 1;
        case SDLK_LGUI:
        case SDLK_RGUI:
          continue;
        default:
          break;
        }
        result->kind = PT_DISPLAY_UNSUPPORTED_KEY;
        result->key = (u32)event.key.keysym.sym;
      } else
        result->kind = PT_DISPLAY_KEY;
      return 1;
    }
    if (event.type == SDL_MOUSEMOTION) {
      result->x = event.motion.x;
      result->y = event.motion.y;
      pointer_x = result->x;
      pointer_y = result->y;
      pointer_buttons =
          (u32) !!(event.motion.state & SDL_BUTTON_LMASK) | ((u32) !!(event.motion.state & SDL_BUTTON_RMASK) << 1);
    } else if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP) {
      unsigned button = event.button.button == SDL_BUTTON_LEFT ? 1 : event.button.button == SDL_BUTTON_RIGHT ? 2 : 0;
      if (!button)
        continue;
      if (event.type == SDL_MOUSEBUTTONDOWN)
        pointer_buttons |= button;
      else
        pointer_buttons &= ~button;
      result->x = event.button.x;
      result->y = event.button.y;
      // Some synthesized macOS releases omit their position. Retain the
      // last delivered pointer sample, rather than inventing a move to
      // the origin. A real move to the origin already updates that sample.

      if (event.type == SDL_MOUSEBUTTONUP && result->x == 0 && result->y == 0) {
        result->x = pointer_x;
        result->y = pointer_y;
      } else {
        pointer_x = result->x;
        pointer_y = result->y;
      }
    } else
      continue;
    result->kind = PT_DISPLAY_POINTER;
    result->buttons = pointer_buttons;
    int window_width, window_height, output_width, output_height;
    SDL_GetWindowSize(window, &window_width, &window_height);
    SDL_GetRendererOutputSize(renderer, &output_width, &output_height);
    SDL_Rect rectangle = display_rect();
    if (getenv("PT_DISPLAY_TRACE_INPUT"))
      fprintf(stderr,
          "SDL pointer sample=%d,%d window=%d,%d output=%d,%d\n",
          result->x,
          result->y,
          window_width,
          window_height,
          output_width,
          output_height);
    if (window_width <= 0 || window_height <= 0)
      continue;
    i64 x = ((i64)result->x * output_width / window_width - rectangle.x) * 320 / rectangle.w;
    i64 y = ((i64)result->y * output_height / window_height - rectangle.y) * 200 / rectangle.h;
    result->x = (i32)(x < 0 ? 0 : x > 319 ? 319 : x);
    result->y = (i32)(y < 0 ? 0 : y > 199 ? 199 : y);
    return 1;
  }
  return 0;
}
u64 pt_display_milliseconds(void) {
  return SDL_GetTicks64();
}
void pt_display_delay(unsigned milliseconds) {
  SDL_Delay(milliseconds);
}
const char *pt_display_error(void) {
  return SDL_GetError();
}
