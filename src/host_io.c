// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
// Host io.

#include "host_io.h"
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <process.h>

int pt_host_main(int argc, wchar_t **wide, int (*entry)(int, char **)) {
  char **argv = calloc((size_t)argc + 1, sizeof(*argv));
  if (!argv)
    return 2;
  int result = 2, converted = 0;
  for (; converted < argc; ++converted) {
    int length = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wide[converted], -1, NULL, 0, NULL, NULL);
    argv[converted] = length ? malloc((size_t)length) : NULL;
    if (!argv[converted] || !WideCharToMultiByte(CP_UTF8, 0, wide[converted], -1, argv[converted], length, NULL, NULL))
      break;
  }
  if (converted == argc)
    result = entry(argc, argv);
  for (int i = 0; i < argc; ++i)
    free(argv[i]);
  free(argv);
  return result;
}
static wchar_t *wide_path(const char *text) {
  int length = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text, -1, NULL, 0);
  wchar_t *result = length ? malloc((size_t)length * sizeof(*result)) : NULL;
  if (!result || !MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text, -1, result, length)) {
    free(result);
    errno = EINVAL;
    return NULL;
  }
  return result;
}
FILE *pt_host_fopen(const char *path, const char *mode) {
  wchar_t *name = wide_path(path), *flags = wide_path(mode);
  FILE *file = name && flags ? _wfopen(name, flags) : NULL;
  free(name);
  free(flags);
  return file;
}
int pt_host_tempfile(char *path) {
  wchar_t *name = wide_path(path);
  if (!name)
    return -1;
  int descriptor = -1;
  if (!_wmktemp_s(name, wcslen(name) + 1)) {
    descriptor = _wopen(name, _O_CREAT | _O_EXCL | _O_RDWR | _O_BINARY | _O_NOINHERIT, _S_IREAD | _S_IWRITE);
    if (descriptor >= 0)
      WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, name, -1, path, (int)strlen(path) + 1, NULL, NULL);
  }
  free(name);
  return descriptor;
}
int pt_host_sync(int descriptor) {
  return _commit(descriptor);
}
bool pt_host_replace(const char *source, const char *destination) {
  wchar_t *from = wide_path(source), *to = wide_path(destination);
  bool okay = from && to && MoveFileExW(from, to, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH);
  free(from);
  free(to);
  return okay;
}
int pt_host_remove(const char *path) {
  wchar_t *name = wide_path(path);
  int result = name ? _wremove(name) : -1;
  free(name);
  return result;
}
int pt_host_restart(char *const *arguments) {
  wchar_t *command = calloc(32768, sizeof(*command));
  wchar_t *executable = wide_path(arguments[0]);
  if (!command || !executable) {
    free(command);
    free(executable);
    return -1;
  }
  size_t used = 0;
  bool okay = true;
  for (size_t i = 0; arguments[i] && okay; ++i) {
    wchar_t *text = wide_path(arguments[i]);
    if (!text || used + 2 * wcslen(text) + 4 >= 32768) {
      free(text);
      okay = false;
      errno = E2BIG;
      break;
    }
    if (used)
      command[used++] = L' ';
    command[used++] = L'"';
    unsigned slashes = 0;
    for (const wchar_t *at = text;; ++at) {
      if (*at == L'\\') {
        ++slashes;
        continue;
      }
      unsigned copies = (*at == L'"' || !*at) ? slashes * 2 : slashes;
      while (copies--)
        command[used++] = L'\\';
      slashes = 0;
      if (!*at)
        break;
      if (*at == L'"')
        command[used++] = L'\\';
      command[used++] = *at;
    }
    command[used++] = L'"';
    free(text);
  }
  STARTUPINFOW startup = {0};
  startup.cb = sizeof(startup);
  PROCESS_INFORMATION process;
  int result = -1;
  if (okay && CreateProcessW(executable, command, NULL, NULL, TRUE, 0, NULL, NULL, &startup, &process)) {
    CloseHandle(process.hThread);
    WaitForSingleObject(process.hProcess, INFINITE);
    DWORD status;
    if (GetExitCodeProcess(process.hProcess, &status))
      result = (int)status;
    CloseHandle(process.hProcess);
  }
  free(command);
  free(executable);
  return result;
}
#else
FILE *pt_host_fopen(const char *path, const char *mode) {
  return fopen(path, mode);
}
int pt_host_tempfile(char *path) {
  return mkstemp(path);
}
int pt_host_sync(int descriptor) {
  return fsync(descriptor);
}
bool pt_host_replace(const char *source, const char *destination) {
  return rename(source, destination) == 0;
}
int pt_host_remove(const char *path) {
  return remove(path);
}
int pt_host_restart(char *const *arguments) {
  return execvp(arguments[0], arguments);
}
#endif
