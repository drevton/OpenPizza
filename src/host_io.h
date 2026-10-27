// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
#ifndef PT_HOST_IO_H
#define PT_HOST_IO_H
#include <stdio.h>
#include <stdbool.h>
#include "types.h"

FILE *pt_host_fopen(const char *path, const char *mode);
int pt_host_tempfile(char *path);
int pt_host_remove(const char *path);
int pt_host_sync(int descriptor);
bool pt_host_replace(const char *source, const char *destination);
int pt_host_mkdir(const char *path);
int pt_host_rmdir(const char *path);
bool pt_host_path_exists(const char *path);
bool pt_host_rename_new(const char *source, const char *destination);
char *pt_host_game_data_directory(void);
typedef bool (*PTHostDownloadProgress)(void *context, size_t done, size_t total);
bool pt_host_https_download(
    const char *url, u8 *data, size_t capacity, size_t *size, PTHostDownloadProgress progress, void *context);
int pt_host_restart(char *const *arguments);
#ifdef _WIN32
int pt_host_main(int argc, wchar_t **wide, int (*entry)(int, char **));
#endif
#endif
