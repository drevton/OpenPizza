// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
#ifndef PT_NATIVE_H
#define PT_NATIVE_H
#include "frame.h"

// Native boot.

// Load verified user game data before starting the native C game.

typedef struct PTNativeAssetsInfo {
  u32 clock;
} PTNativeAssetsInfo;
bool pt_native_assets_load(const char *path, PTNativeAssetsInfo *info);
// Missing disk is a fresh installation; malformed/unreadable disk is an error.

bool pt_native_disk_open(const char *path);
bool pt_native_disk_save(const char *path);

// Native app.

int pt_native_app_run(int argc, char **argv);

#endif
