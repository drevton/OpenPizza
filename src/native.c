// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
#include "native.h"
#include "frame.h"
#include "calendar.h"
#include "savegame.h"
#include "render_types.h"
#include "sound.h"
#include "input.h"
#include "runtime_state.h"
#include "files.h"
#include "game_data.h"
#include "host_io.h"
#include "initial_data.h"
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

bool pt_native_assets_load(const char *directory, PTNativeAssetsInfo *info) {
  if (!info || !pt_native_defaults_load(directory) || !pt_game_data_verify_directory(directory, stderr))
    return false;
  u8 *empty = calloc(1, PT_MEMORY_SIZE);
  if (!empty)
    return false;
  bool okay = pt_savegame_begin_state_image(empty, PT_MEMORY_SIZE);
  free(empty);
  if (!okay)
    return false;
  pt_initial_data_apply();
  pt_saved_global_values_restore();
  pt_native_font_defaults_apply();
  pt_saved_graphics_bind_native_defaults();
  pt_saved_file_label_restore();
  pt_saved_graphics_objects_bind_native_defaults();
  pt_saved_stream_bind_native_defaults();
  pt_saved_palette_state_restore();
  pt_saved_game_objects_bind_native_defaults();
  pt_saved_runtime_state_restore();
  pt_saved_process_bind_native_defaults();
  pt_saved_world_bind_native_defaults();
  PTHeapPlatform heap = {.next_address = PT_HEAP_ARENA_BASE, .next_handle = 1, .reuse_released = 1};
  PTDOSMemoryPlatform dos = {.arena_base = PT_DOS_ARENA_BASE, .arena_end = PT_DOS_ARENA_END, .next_selector = 256};
  if (!pt_heap_load_platform(&heap) || !pt_dos_memory_load_platform(&dos) || !pt_file_load_profile(20, 31) ||
      !pt_game_data_load_directory(directory))
    return false;
  pt_platform_clock()->ticks = 0;
  PTMouseState mouse = {.x = 320, .y = 100, .ready = true};
  if (!pt_mouse_restore(&mouse))
    return false;
  pt_native_audio_start();
  info->clock = 0;
  return true;
}

bool pt_native_disk_open(const char *path) {
  if (!path)
    return false;
  FILE *f = pt_host_fopen(path, "rb");
  if (!f)
    return errno == ENOENT;
  bool okay = pt_file_disk_load(f);
  if (fclose(f))
    okay = false;
  return okay;
}
bool pt_native_disk_save(const char *path) {
  if (!path || !pt_file_disk_quiet())
    return false;
  size_t n = strlen(path);
  if (n > SIZE_MAX - 12u)
    return false;
  char *temporary = malloc(n + 12u);
  if (!temporary)
    return false;
  memcpy(temporary, path, n);
  memcpy(temporary + n, ".tmp.XXXXXX", 12);
  int fd = pt_host_tempfile(temporary);
  bool okay = false;
  if (fd >= 0) {
    FILE *f = fdopen(fd, "wb");
    if (f) {
      okay = pt_file_disk_write(f) && fflush(f) == 0 && pt_host_sync(fd) == 0;
      if (fclose(f))
        okay = false;
    } else
      close(fd);
    if (okay)
      okay = pt_host_replace(temporary, path);
    if (!okay)
      pt_host_remove(temporary);
  }
  free(temporary);
  return okay;
}

// Native app.

#include "display.h"
#include <inttypes.h>

// The DOS launcher starts PT again on exit code zero. Close the platform
// resources before starting the next native session.

static int restart_game(int argc, char **argv, const char *record, const char *record_root) {
  char **next = calloc((size_t)argc + 3u, sizeof(*next));
  if (!next)
    return 2;
  for (int i = 0; i < argc; ++i)
    next[i] = argv[i];
  char *session = NULL;
  bool has_root = false;
  if (record) {
    const char *root = record_root ? record_root : record;
    size_t size = strlen(root) + sizeof(".restart-XXXXXX");
    session = malloc(size);
    if (!session) {
      free(next);
      return 2;
    }
    snprintf(session, size, "%s.restart-XXXXXX", root);
    int fd = pt_host_tempfile(session);
    if (fd < 0) {
      fprintf(stderr, "Cannot create restart input trace: %s\n", strerror(errno));
      free(session);
      free(next);
      return 2;
    }
    close(fd);
    for (int i = 1; i < argc; ++i) {
      if (!strcmp(argv[i], "--record") && i + 1 < argc)
        next[++i] = session;
      else if (!strcmp(argv[i], "--record-root") && i + 1 < argc) {
        has_root = true;
        ++i;
      }
    }
    if (!has_root) {
      next[argc] = "--record-root";
      next[argc + 1] = (char *)root;
    }
  }
  int result = pt_host_restart(next);
  if (result < 0)
    fprintf(stderr, "Cannot restart Pizza Tycoon: %s\n", strerror(errno));
  if (session && result < 0)
    pt_host_remove(session);
  free(session);
  free(next);
  return result < 0 ? 2 : result;
}

static const char *disk_path;
static u64 persisted_revision;
static bool persist_disk(void) {
  u64 revision = pt_file_disk_revision();
  if (!disk_path || revision == persisted_revision || !pt_file_disk_quiet())
    return true;
  if (!pt_native_disk_save(disk_path)) {
    fprintf(stderr, "Cannot persist DOS disk: %s\n", disk_path);
    return false;
  }
  persisted_revision = revision;
  return true;
}
static bool apply(PTCommand *command) {
  PTCommandResult result = pt_command_apply(command);
  PTFrameState state;
  if (!pt_frame_export(&state))
    return false;
  if (!persist_disk())
    return false;
  if (result.status || state.status) {
    fprintf(stderr, "Native execution stopped (operation %u, status %u).\n", state.stopped_at, (unsigned)result.status);
    return false;
  }
  return true;
}
static bool controller_input(const PTDisplayEvent *event, u32 ticks, unsigned frame) {
  PTJoystickPlatform previous;
  if (!pt_joystick_export_platform(&previous))
    return false;
  PTCommand command = {.kind = PT_CMD_FRAME_JOYSTICK, .platform_clock = ticks, .frame = frame};
  command.args[0] = event->key;
  command.args[1] = command.args[2] = command.args[3] = command.args[4] = 1000;
  if (event->key && !previous.present) {
    command.args[6] = 1;
    if (!apply(&command))
      return false;
    command.args[6] = 0;
  }
  command.args[1] = (u32)event->x;
  command.args[2] = (u32)event->y;
  command.args[5] = event->buttons;
  return apply(&command);
}
static bool audio_until(u64 target, u64 *position, u32 ticks, unsigned frame, bool installed) {
  if (!installed) {
    *position = target;
    return true;
  }
  while (*position < target) {
    u64 left = target - *position;
    u32 count = left > PT_PCM_OUTPUT_CAPACITY ? PT_PCM_OUTPUT_CAPACITY : (u32)left;
    PTCommand command = {0};
    command.kind = PT_CMD_PCM_SAMPLES;
    command.args[0] = count;
    command.platform_clock = ticks;
    command.frame = frame;
    if (!apply(&command))
      return false;
    u8 packet[PT_PCM_OUTPUT_CAPACITY * 8];
    size_t size;
    if (!pt_audio_output(packet, sizeof(packet), &size) || size != count * 8u ||
        pt_display_audio_write(packet, size) < 0) {
      fprintf(stderr, "PCM output failed: %s\n", pt_display_error());
      return false;
    }
    *position += count;
  }
  return true;
}
int pt_native_app_run(int argc, char **argv) {
  if (argc == 3 && !strcmp(argv[1], "--download-data"))
    return pt_game_data_download(argv[2], NULL, NULL) == PT_GAME_DATA_INSTALLED ? 0 : 2;
  if (argc == 4 && !strcmp(argv[1], "--install-data-archive"))
    return pt_game_data_install_archive(argv[2], argv[3], NULL, NULL) == PT_GAME_DATA_INSTALLED ? 0 : 2;
  if (argc == 3 && !strcmp(argv[1], "--check-data")) {
    if (!pt_game_data_verify_directory(argv[2], stderr))
      return 2;
    printf("Verified all %zu English DOS V2.0 resource files (SHA-256).\n", pt_game_data_file_count());
    return 0;
  }
  const char *boot = NULL, *record = NULL, *record_root = NULL;
  unsigned limit = 0;
  disk_path = NULL;
  for (int i = 1; i < argc; ++i) {
    if (!strcmp(argv[i], "--data") && i + 1 < argc)
      boot = argv[++i];
    else if (!strcmp(argv[i], "--record") && i + 1 < argc)
      record = argv[++i];
    else if (!strcmp(argv[i], "--record-root") && i + 1 < argc)
      record_root = argv[++i];
    else if (!strcmp(argv[i], "--disk") && i + 1 < argc)
      disk_path = argv[++i];
    else if (!strcmp(argv[i], "--frames") && i + 1 < argc)
      limit = (unsigned)strtoul(argv[++i], NULL, 10);
    else {
      fprintf(stderr, "Usage: %s --data DIRECTORY [--record FILE] [--frames N] [--disk FILE]\n", argv[0]);
      return 2;
    }
  }
  if (!boot)
    boot = getenv("PT_GAME_DATA_DIR");
  if (!boot || !*boot)
    boot = "PIZZA";
  bool installed = false;
  char *game_data = pt_game_data_find_directory(boot, &installed);
  if (!installed && !pt_display_download_game_data(game_data, pt_game_data_download)) {
    free(game_data);
    return 2;
  }
  PTNativeAssetsInfo context;
  bool loaded = pt_native_assets_load(game_data, &context);
  if (!loaded) {
    fprintf(stderr, "Cannot load game assets: %s\n", game_data);
    free(game_data);
    return 2;
  }
  free(game_data);
  if (disk_path && !pt_native_disk_open(disk_path)) {
    fprintf(stderr, "Cannot import DOS disk: %s\n", disk_path);
    pt_frame_reset();
    return 2;
  }
  if (disk_path && !pt_game_data_verify_assets(stderr)) {
    pt_frame_reset();
    return 2;
  }
  persisted_revision = pt_file_disk_revision();
  const char *arguments[] = {"OpenPizza"};
  if (!pt_game_start(1, arguments))
    return 2;
  FILE *trace = record ? pt_host_fopen(record, "wb") : NULL;
  if (record && !trace) {
    pt_frame_reset();
    return 2;
  }
  pt_command_set_record_file(trace);
  int code = 0;
  bool restart = false;
  if (pt_display_open("Pizza Tycoon", 3, 0) < 0) {
    fprintf(stderr, "%s\n", pt_display_error());
    code = 2;
  } else {
    u8 rgb[PT_SCREEN_PIXELS * sizeof(PTRGB)];
    bool running = true, pit_running = false;
    unsigned presented = 0;
    u64 epoch = pt_display_milliseconds(), audio_samples = 0, pit_deadline = 0, frame_index = 0;
    u32 ticks = context.clock;
    PTDisplayEvent *pending = NULL;
    size_t queued = 0, capacity = 0;
    while (running && (!limit || presented < limit)) {
      PTDisplayEvent event;
      while (running && pt_display_poll(&event) > 0) {
        if (event.kind == PT_DISPLAY_QUIT) {
          running = false;
          break;
        }
        if (event.kind == PT_DISPLAY_JOYSTICK) {
          if (!controller_input(&event, ticks, presented)) {
            running = false;
            code = 3;
          }
          continue;
        }
        if (queued == capacity) {
          size_t next = capacity ? capacity * 2u : 256u;
          if (next < capacity || next > SIZE_MAX / sizeof(*pending)) {
            fprintf(stderr, "Too many pending input events.\n");
            running = false;
            code = 2;
            break;
          }
          PTDisplayEvent *grown = realloc(pending, next * sizeof(*pending));
          if (!grown) {
            fprintf(stderr, "Cannot allocate pending input events.\n");
            running = false;
            code = 2;
            break;
          }
          pending = grown;
          capacity = next;
        }
        pending[queued++] = event;
      }
      bool input_ready = game_ptr && ptr_apple && mouse_result && pt_mouse_ready_for_events();
      for (size_t i = 0; running && input_ready && i < queued; ++i) {
        event = pending[i];
        PTCommand command = {0};
        command.platform_clock = ticks;
        command.frame = presented;
        if (event.kind == PT_DISPLAY_QUIT) {
          running = false;
          break;
        }
        if (event.kind == PT_DISPLAY_UNSUPPORTED_KEY) {
          fprintf(stderr, "Unsupported SDL key or modifier input.\n");
          running = false;
          code = 3;
          break;
        }
        command.modifiers = event.modifiers;
        if (event.kind == PT_DISPLAY_POINTER) {
          command.kind = PT_CMD_FRAME_POINTER;
          command.pointer_count = 1;
          command.pointer_path[0] = (PTPointerSample){event.x * 2, event.y, event.buttons};
        } else if (event.kind == PT_DISPLAY_KEY || event.kind == PT_DISPLAY_KEYBOARD_PORT) {
          command.kind = PT_CMD_FRAME_KEYBOARD_PORT;
          command.args[0] = event.kind == PT_DISPLAY_KEY ? event.keyboard_port : event.key;
          if (!apply(&command)) {
            running = false;
            code = 3;
            break;
          }
          if (event.kind == PT_DISPLAY_KEYBOARD_PORT)
            continue;
          command.kind = PT_CMD_FRAME_KEY;
          command.args[0] = 0;
          command.key = event.key;
        } else
          continue;
        if (!apply(&command)) {
          running = false;
          code = 3;
        }
        // The original runs between mouse/BIOS input interrupts.
        // In particular its menu selects a row before waiting for
        // the next press. Drain each delivered event before the next
        // one, rather than batching a move/press/release into a single
        // 18 Hz resume. This yields at the next platform poll without
        // inventing an elapsed clock tick.

        if (running) {
          PTCommand resume = {.kind = PT_CMD_FRAME, .platform_clock = ticks, .frame = presented};
          if (!apply(&resume)) {
            running = false;
            code = 3;
          }
        }
        PTProcessState process;
        if (!pt_process_export_platform(&process)) {
          running = false;
          code = 2;
        } else if (process.terminated) {
          running = false;
          code = (int)process.return_code;
          restart = code == 0;
        }
      }
      if (input_ready)
        queued = 0;
      u64 elapsed = pt_display_milliseconds() - epoch, sample_now = elapsed * 11025u / 1000u;
      bool pcm_installed = pt_pcm_driver_state()->installed != 0;
      bool fm_enabled = pt_fm_output_state()->enabled;
      u32 timer_period = pt_sound_timer_state()->reload;
      if ((pt_sound_timer_installed() || pt_fm_timer_installed()) && timer_period) {
        if (!pit_running) {
          pit_running = true;
          pit_deadline = elapsed * 1193182u + (u64)timer_period * 1000u;
        }
        // Interleave sample clocks and IRQ0 at the programmed PIT
        // period. The recorded commands retain this exact ordering.

        u64 deadline = pit_deadline;
        while (running && deadline <= elapsed * 1193182u) {
          u64 samples = deadline * 11025u / (1193182u * 1000u);
          if (!audio_until(samples, &audio_samples, ticks, presented, pcm_installed || fm_enabled)) {
            running = false;
            code = 3;
            break;
          }
          PTCommand command = {0};
          command.kind = PT_CMD_SOUND_TIMER;
          command.platform_clock = ticks;
          command.frame = presented;
          if (!apply(&command)) {
            running = false;
            code = 3;
            break;
          }
          ticks = pt_platform_clock()->ticks;
          timer_period = pt_sound_timer_state()->reload;
          if (!timer_period) {
            running = false;
            code = 3;
            break;
          }
          deadline += (u64)timer_period * 1000u;
          pit_deadline = deadline;
        }
      } else
        pit_running = false;
      if (running && !audio_until(sample_now, &audio_samples, ticks, presented, pcm_installed || fm_enabled)) {
        running = false;
        code = 3;
      }
      u64 now = elapsed * 182065u / 10000000u;
      if (running && now != frame_index) {
        frame_index = now;
        if (!pit_running)
          ticks = context.clock + (u32)now;
        PTCommand command = {0};
        command.kind = PT_CMD_FRAME;
        command.platform_clock = ticks;
        command.frame = presented;
        if (!apply(&command)) {
          running = false;
          code = 3;
        }
        PTProcessState process;
        if (!pt_process_export_platform(&process)) {
          running = false;
          code = 2;
        } else if (process.terminated) {
          running = false;
          code = (int)process.return_code;
          restart = code == 0;
        }
        if (running && pt_framebuffer_rgb(rgb, sizeof(rgb))) {
          if (pt_display_present(rgb, sizeof(rgb), NULL) < 0) {
            fprintf(stderr, "%s\n", pt_display_error());
            running = false;
            code = 2;
          } else
            ++presented;
        }
      }
      pt_display_delay(5);
    }
    free(pending);
    pt_display_close();
  }
  if (!persist_disk())
    code = 2;
  pt_command_set_record_file(NULL);
  if (trace)
    fclose(trace);
  pt_frame_reset();
  if (restart && !code)
    return restart_game(argc, argv, record, record_root);
  return code;
}
#ifdef _WIN32
int wmain(int argc, wchar_t **wide) {
  return pt_host_main(argc, wide, pt_native_app_run);
}
#else
int main(int argc, char **argv) {
  return pt_native_app_run(argc, argv);
}
#endif
