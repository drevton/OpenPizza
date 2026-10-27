// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
#ifndef PT_GAME_DATA_H
#define PT_GAME_DATA_H
#include "types.h"

// Sha256.

#include <stddef.h>

enum { PT_SHA256_SIZE = 32 };
void pt_sha256(const void *data, size_t size, u8 digest[PT_SHA256_SIZE]);

// Game data.

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

// The returned span is borrowed until the next reader call. Verification does not alter game state.
typedef bool (*PTGameDataReader)(void *context, const char *name, const u8 **data, size_t *size);
size_t pt_game_data_file_count(void);
bool pt_game_data_verify(PTGameDataReader reader, void *context, FILE *errors);
bool pt_game_data_verify_assets(FILE *errors);
bool pt_game_data_verify_directory(const char *directory, FILE *errors);

bool pt_game_data_read_file(const char *directory, const char *name, u8 **data, size_t *size);
bool pt_game_data_load_directory(const char *directory);

#endif
