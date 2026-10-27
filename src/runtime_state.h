// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
#ifndef PT_RUNTIME_STATE_H
#define PT_RUNTIME_STATE_H
#include "state_types.h"
#include <stdbool.h>
enum { PT_RUNTIME_FINALIZER_COUNT = 2, PT_RUNTIME_EXIT_HOOK_COUNT = 3 };
typedef struct PTRuntimeCallback {
  void (*invoke)(void *);
  void *context;
} PTRuntimeCallback;
typedef struct PTRuntimeFinalizer {
  u8 state, priority;
  PTRuntimeCallback callback;
} PTRuntimeFinalizer;
typedef struct PTRuntimeState {
  PTRuntimeFinalizer finalizers[PT_RUNTIME_FINALIZER_COUNT];
  PTRuntimeCallback exit_hooks[PT_RUNTIME_EXIT_HOOK_COUNT];
  PTRuntimeCallback fpu_finalizer;
  PTRuntimeCallback sound_timer;
} PTRuntimeState;
extern PTRuntimeState pt_runtime;
struct GuestFileNode {
  GuestFileNode *next;
  GuestFileStream *stream;
};
typedef struct PTProcessState {
  u32 terminated, return_code;
} PTProcessState;
_Static_assert(sizeof(PTProcessState) == 8, "process platform record");
void pt_runtime_reset_platform(void);
bool pt_process_load_platform(const PTProcessState *);
bool pt_process_export_platform(PTProcessState *);
i32 GuestStrncmp(const u8 *first, const u8 *second, u32 size);
u32 GuestCloseAllStreams(u32 first);
void GuestPurgeFileNodes(void);
void GuestFinalizeFiles(void);
void GuestFiniRtns(u8 first, u8 last);
void GuestFini387(void);
_Noreturn void GuestExit(u32 status);
_Noreturn void GuestImmediateExit(u32 status);

extern GuestFileNode *guest_free_file_nodes, *guest_active_file_nodes;
void pt_runtime_state_reset(void);
bool pt_runtime_initialize_file_nodes(void);
void pt_runtime_invoke(PTRuntimeCallback callback);
GuestFileNode *pt_file_node_create(GuestFileStream *stream);
void pt_file_node_release(GuestFileNode *node);
_Noreturn void pt_runtime_interrupt_unavailable(u32 interrupt);
_Noreturn void pt_runtime_finish_process(u8 status);
#endif
