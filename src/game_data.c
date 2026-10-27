// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
// Game data.

#include "game_data.h"
#include "files.h"
#include "host_io.h"
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

typedef struct PTGameDataFile {
  const char *name;
  size_t size;
  const char *sha256;
} PTGameDataFile;

static const PTGameDataFile required_files[] = {
#define PT_GAME_DATA(name, size, sha256) {name, size, sha256},
#include "src/game_data_checksums.def"
#undef PT_GAME_DATA
};

size_t pt_game_data_file_count(void) {
  return sizeof(required_files) / sizeof(*required_files);
}

bool pt_game_data_verify(PTGameDataReader reader, void *context, FILE *errors) {
  if (!reader)
    return false;
  size_t failures = 0;
  for (size_t i = 0; i < pt_game_data_file_count(); ++i) {
    const PTGameDataFile *file = &required_files[i];
    const u8 *data = NULL;
    size_t size = 0;
    const char *problem = NULL;
    if (!reader(context, file->name, &data, &size))
      problem = "missing or unreadable";
    else if (size != file->size || (size && !data))
      problem = "incorrect size";
    else {
      u8 digest[PT_SHA256_SIZE];
      char hex[PT_SHA256_SIZE * 2 + 1];
      static const char digits[] = "0123456789abcdef";
      pt_sha256(data, size, digest);
      for (unsigned byte = 0; byte < PT_SHA256_SIZE; ++byte) {
        hex[byte * 2] = digits[digest[byte] >> 4];
        hex[byte * 2 + 1] = digits[digest[byte] & 15u];
      }
      hex[sizeof(hex) - 1] = 0;
      if (strcmp(hex, file->sha256))
        problem = "SHA-256 mismatch";
    }
    if (problem) {
      ++failures;
      if (errors)
        fprintf(errors, "Game data: %s: %s\n", file->name, problem);
    }
  }
  if (failures && errors)
    fprintf(errors,
        "Game not started: %zu of %zu required English DOS V2.0 data files failed verification.\n",
        failures,
        pt_game_data_file_count());
  return failures == 0;
}

static bool read_asset(void *context, const char *name, const u8 **data, size_t *size) {
  (void)context;
  return pt_file_asset_contents(name, data, size);
}

bool pt_game_data_verify_assets(FILE *errors) {
  return pt_game_data_verify(read_asset, NULL, errors);
}

typedef struct PTGameDataDirectory {
  const char *root;
  u8 *bytes;
} PTGameDataDirectory;

// Resolve DOS filenames without depending on the host filesystem's case rules.
static char *resolve_file(const char *root, const char *name) {
  size_t capacity = strlen(root) + strlen(name) + 2;
  char *path = malloc(capacity);
  if (!path)
    return NULL;
  strcpy(path, root);
#ifndef _WIN32
  const char *component = name;
  while (*component) {
    const char *slash = strchr(component, '/');
    size_t length = slash ? (size_t)(slash - component) : strlen(component);
    DIR *directory = opendir(path);
    if (!directory) {
      free(path);
      return NULL;
    }
    struct dirent *entry;
    unsigned matches = 0;
    size_t end = strlen(path);
    while ((entry = readdir(directory))) {
      if (strlen(entry->d_name) != length || strncasecmp(entry->d_name, component, length))
        continue;
      if (!matches) {
        path[end] = '/';
        memcpy(path + end + 1, entry->d_name, length + 1);
      }
      ++matches;
    }
    closedir(directory);
    if (matches != 1) {
      free(path);
      return NULL;
    }
    component += length + (slash ? 1 : 0);
  }
#else
  strcat(path, "/");
  strcat(path, name);
#endif
  return path;
}

static bool read_directory(void *opaque, const char *name, const u8 **data, size_t *size) {
  PTGameDataDirectory *context = opaque;
  free(context->bytes);
  context->bytes = NULL;
  char *path = resolve_file(context->root, name);
  if (!path)
    return false;
  FILE *file = pt_host_fopen(path, "rb");
  free(path);
  if (!file)
    return false;
  bool okay = !fseek(file, 0, SEEK_END);
  long length = okay ? ftell(file) : -1;
  okay = length >= 0 && !fseek(file, 0, SEEK_SET);
  if (okay) {
    context->bytes = malloc(length ? (size_t)length : 1);
    okay = context->bytes && fread(context->bytes, 1, (size_t)length, file) == (size_t)length && fgetc(file) == EOF &&
        !ferror(file);
  }
  if (fclose(file))
    okay = false;
  if (!okay)
    return false;
  *data = context->bytes;
  *size = (size_t)length;
  return true;
}

bool pt_game_data_verify_directory(const char *directory, FILE *errors) {
  if (!directory || !*directory)
    return false;
  PTGameDataDirectory context = {.root = directory};
  bool okay = pt_game_data_verify(read_directory, &context, errors);
  free(context.bytes);
  return okay;
}

// Sha256.

static u32 rotate_right(u32 value, unsigned count) {
  return (value >> count) | (value << (32u - count));
}

static void transform(u32 state[8], const u8 block[64]) {
  static const u32 constants[64] = {0x428a2f98,
    0x71374491,
    0xb5c0fbcf,
    0xe9b5dba5,
    0x3956c25b,
    0x59f111f1,
    0x923f82a4,
    0xab1c5ed5,
    0xd807aa98,
    0x12835b01,
    0x243185be,
    0x550c7dc3,
    0x72be5d74,
    0x80deb1fe,
    0x9bdc06a7,
    0xc19bf174,
    0xe49b69c1,
    0xefbe4786,
    0x0fc19dc6,
    0x240ca1cc,
    0x2de92c6f,
    0x4a7484aa,
    0x5cb0a9dc,
    0x76f988da,
    0x983e5152,
    0xa831c66d,
    0xb00327c8,
    0xbf597fc7,
    0xc6e00bf3,
    0xd5a79147,
    0x06ca6351,
    0x14292967,
    0x27b70a85,
    0x2e1b2138,
    0x4d2c6dfc,
    0x53380d13,
    0x650a7354,
    0x766a0abb,
    0x81c2c92e,
    0x92722c85,
    0xa2bfe8a1,
    0xa81a664b,
    0xc24b8b70,
    0xc76c51a3,
    0xd192e819,
    0xd6990624,
    0xf40e3585,
    0x106aa070,
    0x19a4c116,
    0x1e376c08,
    0x2748774c,
    0x34b0bcb5,
    0x391c0cb3,
    0x4ed8aa4a,
    0x5b9cca4f,
    0x682e6ff3,
    0x748f82ee,
    0x78a5636f,
    0x84c87814,
    0x8cc70208,
    0x90befffa,
    0xa4506ceb,
    0xbef9a3f7,
    0xc67178f2};
  u32 words[64];
  for (unsigned i = 0; i < 16; ++i) {
    const u8 *p = block + i * 4;
    words[i] = (u32)p[0] << 24 | (u32)p[1] << 16 | (u32)p[2] << 8 | p[3];
  }
  for (unsigned i = 16; i < 64; ++i) {
    u32 a = words[i - 15], b = words[i - 2];
    u32 small0 = rotate_right(a, 7) ^ rotate_right(a, 18) ^ (a >> 3);
    u32 small1 = rotate_right(b, 17) ^ rotate_right(b, 19) ^ (b >> 10);
    words[i] = words[i - 16] + small0 + words[i - 7] + small1;
  }
  u32 a = state[0], b = state[1], c = state[2], d = state[3];
  u32 e = state[4], f = state[5], g = state[6], h = state[7];
  for (unsigned i = 0; i < 64; ++i) {
    u32 sum1 = rotate_right(e, 6) ^ rotate_right(e, 11) ^ rotate_right(e, 25);
    u32 choose = (e & f) ^ (~e & g);
    u32 first = h + sum1 + choose + constants[i] + words[i];
    u32 sum0 = rotate_right(a, 2) ^ rotate_right(a, 13) ^ rotate_right(a, 22);
    u32 majority = (a & b) ^ (a & c) ^ (b & c);
    h = g;
    g = f;
    f = e;
    e = d + first;
    d = c;
    c = b;
    b = a;
    a = first + sum0 + majority;
  }
  state[0] += a;
  state[1] += b;
  state[2] += c;
  state[3] += d;
  state[4] += e;
  state[5] += f;
  state[6] += g;
  state[7] += h;
}

void pt_sha256(const void *data, size_t size, u8 digest[PT_SHA256_SIZE]) {
  u32 state[8] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};
  const u8 *bytes = data;
  u64 bits = (u64)size * 8u;
  while (size >= 64) {
    transform(state, bytes);
    bytes += 64;
    size -= 64;
  }
  u8 tail[128] = {0};
  if (size)
    memcpy(tail, bytes, size);
  tail[size] = 0x80;
  size_t padded = size < 56 ? 64 : 128;
  for (unsigned i = 0; i < 8; ++i)
    tail[padded - 1 - i] = (u8)(bits >> (8u * i));
  transform(state, tail);
  if (padded == 128)
    transform(state, tail + 64);
  for (unsigned i = 0; i < 8; ++i)
    for (unsigned j = 0; j < 4; ++j)
      digest[i * 4 + j] = (u8)(state[i] >> (24u - j * 8u));
}

bool pt_game_data_read_file(const char *root, const char *name, u8 **data, size_t *size) {
  PTGameDataDirectory context = {.root = root};
  const u8 *bytes = NULL;
  if (!read_directory(&context, name, &bytes, size)) {
    free(context.bytes);
    return false;
  }
  *data = context.bytes;
  return true;
}
bool pt_game_data_load_directory(const char *root) {
  if (!pt_game_data_verify_directory(root, stderr))
    return false;
  for (size_t i = 0; i < sizeof(required_files) / sizeof(*required_files); ++i) {
    u8 *data = NULL;
    size_t size;
    const char *name = required_files[i].name;
    if (!pt_game_data_read_file(root, name, &data, &size))
      return false;
    bool okay = pt_file_asset_load(name, data, size);
    free(data);
    if (!okay)
      return false;
  }
  static const char *const optional[] = {"prefs", "sound.cfg", "d/hiscores.piz"};
  for (size_t i = 0; i < sizeof(optional) / sizeof(*optional); ++i) {
    u8 *data = NULL;
    size_t size;
    if (!pt_game_data_read_file(root, optional[i], &data, &size))
      continue;
    bool okay = pt_file_asset_load(optional[i], data, size);
    free(data);
    if (!okay)
      return false;
  }
  return pt_file_directory_load("game");
}
