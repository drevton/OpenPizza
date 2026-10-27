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

static char *data_copy_path(const char *path) {
  char *copy = malloc(strlen(path) + 1);
  if (copy)
    strcpy(copy, path);
  return copy;
}
char *pt_game_data_find_directory(const char *requested, bool *installed) {
  *installed = false;
  if (requested && pt_game_data_verify_installation(requested, NULL)) {
    char *copy = data_copy_path(requested);
    *installed = copy != NULL;
    return copy;
  }
  char *base = pt_host_game_data_directory();
  char *destination = base ? malloc(strlen(base) + 16) : NULL;
  char *available = NULL;
  if (destination) {
    for (unsigned i = 1; i <= 100; ++i) {
      if (i == 1)
        strcpy(destination, base);
      else
        snprintf(destination, strlen(base) + 16, "%s-%u", base, i);
      if (!pt_host_path_exists(destination)) {
        if (!available)
          available = data_copy_path(destination);
      } else if (pt_game_data_verify_installation(destination, NULL)) {
        free(base);
        free(available);
        *installed = true;
        return destination;
      }
    }
  }
  free(base);
  free(destination);
  destination = available;
  fputs("Game data: required English DOS V2.0 assets were not found or failed verification.\n", stderr);
  return destination;
}

// The archive is fetched only after an explicit user choice. Its bytes never enter a release package.
#include "../vendor/puff/puff.c"
#include <errno.h>
#include <unistd.h>

static const PTGameDataFile download_executable = {
  "pt.exe", 470411, "9cce82355b466fc0e47c51ae1de4ba850a666b5f12c3831ee19ba5c4c4ef6add"};
static const char download_url[] = "https://archive.org/download/msdos_Pizza_Tycoon_1994/Pizza_Tycoon_1994.zip";
static const char download_hash[] = "ea5dca5d8eab9008f48b10e8fd0215618df233ce36853067144590bf99cede5f";
enum { DOWNLOAD_BYTES = 2580572 };

static bool data_hash_matches(const u8 *data, size_t size, const char *expected) {
  u8 digest[32];
  char hex[65];
  const char digits[] = "0123456789abcdef";
  pt_sha256(data, size, digest);
  for (unsigned i = 0; i < 32; ++i) {
    hex[2 * i] = digits[digest[i] >> 4];
    hex[2 * i + 1] = digits[digest[i] & 15];
  }
  hex[64] = 0;
  return !strcmp(hex, expected);
}
bool pt_game_data_verify_installation(const char *directory, FILE *errors) {
  if (!pt_game_data_verify_directory(directory, errors))
    return false;
  PTGameDataDirectory reader = {.root = directory};
  const u8 *data;
  size_t size;
  bool okay = false;
  const char *names[] = {"pt.exe", "pt_original.exe"};
  for (unsigned i = 0; i < 2 && !okay; ++i)
    okay = read_directory(&reader, names[i], &data, &size) && size == download_executable.size &&
        data_hash_matches(data, size, download_executable.sha256);
  free(reader.bytes);
  if (!okay && errors)
    fputs("Game data: the verified English DOS V2.0 executable is missing or incompatible.\n", errors);
  return okay;
}
static u16 zip_word(const u8 *p) {
  return (u16)(p[0] | (u16)p[1] << 8);
}
static u32 zip_dword(const u8 *p) {
  return (u32)zip_word(p) | (u32)zip_word(p + 2) << 16;
}
static bool zip_range(size_t at, size_t length, size_t size) {
  return at <= size && length <= size - at;
}
static u8 *download_zip_file(const u8 *zip, size_t size, const char *name, size_t expected) {
  if (size < 22)
    return NULL;
  size_t end_at = size - 22, earliest = size > 65557 ? size - 65557 : 0;
  while (zip_dword(zip + end_at) != 0x06054b50 || zip_word(zip + end_at + 20) != size - end_at - 22) {
    if (end_at == earliest)
      return NULL;
    --end_at;
  }
  const u8 *end = zip + end_at;
  u16 count = zip_word(end + 10);
  size_t at = zip_dword(end + 16), extent = zip_dword(end + 12);
  if (zip_word(end + 4) || zip_word(end + 6) || count != zip_word(end + 8) || !zip_range(at, extent, end_at) ||
      at + extent != end_at)
    return NULL;
  char wanted[1024];
  if (snprintf(wanted, sizeof(wanted), "PizzaTyc/PIZZA/%s", name) >= (int)sizeof(wanted))
    return NULL;
  const u8 *selected = NULL;
  for (unsigned i = 0; i < count; ++i) {
    if (!zip_range(at, 46, end_at) || zip_dword(zip + at) != 0x02014b50)
      return NULL;
    const u8 *record = zip + at;
    size_t width = zip_word(record + 28), extra = zip_word(record + 30), comment = zip_word(record + 32);
    if (!zip_range(at + 46, width + extra + comment, end_at))
      return NULL;
    if (width == strlen(wanted) && !strncasecmp((const char *)record + 46, wanted, width)) {
      if (selected)
        return NULL;
      selected = record;
    }
    at += 46 + width + extra + comment;
  }
  if (at != end_at || !selected || zip_word(selected + 8) & 1 || zip_dword(selected + 24) != expected)
    return NULL;
  size_t compressed = zip_dword(selected + 20), offset = zip_dword(selected + 42);
  if (!zip_range(offset, 30, size) || zip_dword(zip + offset) != 0x04034b50)
    return NULL;
  size_t name_size = zip_word(zip + offset + 26), extra_size = zip_word(zip + offset + 28);
  if (!zip_range(offset + 30, name_size + extra_size, size) || name_size != strlen(wanted) ||
      strncasecmp((const char *)zip + offset + 30, wanted, name_size))
    return NULL;
  offset += 30 + name_size + extra_size;
  if (!zip_range(offset, compressed, size))
    return NULL;
  u8 *data = malloc(expected ? expected : 1);
  if (!data)
    return NULL;
  unsigned method = zip_word(selected + 10);
  bool okay = false;
  if (method == 0 && compressed == expected) {
    memcpy(data, zip + offset, expected);
    okay = true;
  } else if (method == 8) {
    unsigned long output = (unsigned long)expected, input = (unsigned long)compressed;
    okay = !puff(data, &output, zip + offset, &input) && output == expected && input == compressed;
  }
  if (!okay) {
    free(data);
    return NULL;
  }
  return data;
}
static char *download_path(const char *directory, const char *name) {
  size_t size = strlen(directory) + strlen(name) + 2;
  char *path = malloc(size);
  if (path)
    snprintf(path, size, "%s/%s", directory, name);
  return path;
}
static bool download_parent_directories(char *path) {
  for (char *at = path + 1; *at; ++at) {
    bool slash = *at == '/';
#ifdef _WIN32
    slash = slash || *at == '\\';
#endif
    if (!slash || (at == path + 2 && path[1] == ':'))
      continue;
    char separator = *at;
    *at = 0;
    int result = pt_host_mkdir(path), error = errno;
    *at = separator;
    if (result && error != EEXIST)
      return false;
  }
  return true;
}
static void download_cleanup(const char *stage) {
  size_t count = pt_game_data_file_count();
  for (size_t i = 0; i < count + 2; ++i) {
    const char *name = i < count ? required_files[i].name : i == count ? "pt.exe" : "sound.cfg";
    char *path = download_path(stage, name);
    if (path)
      pt_host_remove(path);
    free(path);
  }
  for (size_t i = 0; i < count; ++i) {
    char *path = download_path(stage, required_files[i].name);
    if (!path)
      continue;
    char *slash = strrchr(path, '/');
    while (slash && (size_t)(slash - path) > strlen(stage)) {
      *slash = 0;
      pt_host_rmdir(path);
      slash = strrchr(path, '/');
    }
    free(path);
  }
  pt_host_rmdir(stage);
}
static PTGameDataInstallResult download_install(
    const u8 *zip, size_t size, const char *directory, PTGameDataProgress progress, void *context) {
  if (size != DOWNLOAD_BYTES || !data_hash_matches(zip, size, download_hash)) {
    fputs("Game data: Internet Archive ZIP checksum mismatch; nothing was installed.\n", stderr);
    return PT_GAME_DATA_INSTALL_FAILED;
  }
  if (!directory || !*directory || pt_host_path_exists(directory)) {
    fputs("Game data: choose a new installation directory; existing files are never overwritten.\n", stderr);
    return PT_GAME_DATA_INSTALL_FAILED;
  }
  size_t width = strlen(directory) + 24;
  char *stage = malloc(width);
  if (!stage)
    return PT_GAME_DATA_INSTALL_FAILED;
  snprintf(stage, width, "%s.partial-XXXXXX", directory);
  if (!download_parent_directories(stage)) {
    free(stage);
    return PT_GAME_DATA_INSTALL_FAILED;
  }
  int descriptor = pt_host_tempfile(stage);
  if (descriptor < 0) {
    free(stage);
    return PT_GAME_DATA_INSTALL_FAILED;
  }
  close(descriptor);
  pt_host_remove(stage);
  if (pt_host_mkdir(stage)) {
    free(stage);
    return PT_GAME_DATA_INSTALL_FAILED;
  }
  PTGameDataInstallResult result = PT_GAME_DATA_INSTALL_FAILED;
  size_t count = pt_game_data_file_count();
  for (size_t i = 0; i < count + 2; ++i) {
    PTGameDataFile config = {"sound.cfg", 18, NULL};
    const PTGameDataFile *file = i < count ? &required_files[i] : i == count ? &download_executable : &config;
    if (progress && !progress(context, 1, i, count + 2)) {
      result = PT_GAME_DATA_CANCELED;
      goto done;
    }
    u8 *data = download_zip_file(zip, size, file->name, file->size);
    char *path = download_path(stage, file->name);
    bool okay = data && path && (!file->sha256 || data_hash_matches(data, file->size, file->sha256));
    FILE *output = okay && download_parent_directories(path) ? pt_host_fopen(path, "wb") : NULL;
    okay = output && fwrite(data, 1, file->size, output) == file->size;
    if (output && fclose(output))
      okay = false;
    free(path);
    free(data);
    if (!okay)
      goto done;
  }
  if (progress && !progress(context, 1, count + 2, count + 2)) {
    result = PT_GAME_DATA_CANCELED;
    goto done;
  }
  if (!pt_game_data_verify_installation(stage, stderr) || !pt_host_rename_new(stage, directory))
    goto done;
  result = PT_GAME_DATA_INSTALLED;
done:
  if (result != PT_GAME_DATA_INSTALLED) {
    download_cleanup(stage);
    if (result != PT_GAME_DATA_CANCELED)
      fputs("Game data: download installation failed; no existing files were changed.\n", stderr);
  }
  free(stage);
  return result;
}
PTGameDataInstallResult pt_game_data_install_archive(
    const char *archive, const char *directory, PTGameDataProgress progress, void *context) {
  u8 *data = malloc(DOWNLOAD_BYTES + 1);
  FILE *input = pt_host_fopen(archive, "rb");
  if (!data || !input) {
    free(data);
    if (input)
      fclose(input);
    return PT_GAME_DATA_INSTALL_FAILED;
  }
  size_t size = fread(data, 1, DOWNLOAD_BYTES + 1, input);
  bool okay = !ferror(input);
  if (fclose(input))
    okay = false;
  PTGameDataInstallResult result =
      okay ? download_install(data, size, directory, progress, context) : PT_GAME_DATA_INSTALL_FAILED;
  free(data);
  return result;
}
typedef struct PTDownloadContext {
  PTGameDataProgress progress;
  void *context;
  bool canceled;
} PTDownloadContext;
static bool download_progress(void *opaque, size_t done, size_t total) {
  PTDownloadContext *transfer = opaque;
  if (transfer->progress && !transfer->progress(transfer->context, 0, done, total))
    transfer->canceled = true;
  return !transfer->canceled;
}
PTGameDataInstallResult pt_game_data_download(const char *directory, PTGameDataProgress progress, void *context) {
  if (!directory || !*directory || pt_host_path_exists(directory)) {
    fputs("Game data: choose a new installation directory; existing files are never overwritten.\n", stderr);
    return PT_GAME_DATA_INSTALL_FAILED;
  }
  u8 *data = malloc(DOWNLOAD_BYTES);
  if (!data)
    return PT_GAME_DATA_INSTALL_FAILED;
  PTDownloadContext transfer = {progress, context, false};
  size_t size = 0;
  bool okay = pt_host_https_download(download_url, data, DOWNLOAD_BYTES, &size, download_progress, &transfer);
  PTGameDataInstallResult result = transfer.canceled ? PT_GAME_DATA_CANCELED
      : okay                                         ? download_install(data, size, directory, progress, context)
                                                     : PT_GAME_DATA_INSTALL_FAILED;
  free(data);
  if (!okay && !transfer.canceled)
    fputs("Game data: Internet Archive download failed. Check your connection and retry.\n", stderr);
  return result;
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
