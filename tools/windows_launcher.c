// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
// Keep save ownership in the launcher while native sessions run and restart.
#include "types.h"
#include <stdbool.h>
#include <windows.h>
#include <shellapi.h>
#include <shlobj.h>
#include <wchar.h>
#include <stdlib.h>
#include <string.h>

enum { PT_PATH_CAPACITY = 32768 };
static void fail(const wchar_t *message) {
  MessageBoxW(NULL, message, L"Pizza Tycoon", MB_OK | MB_ICONERROR);
  ExitProcess(2);
}
static void join(wchar_t *output, const wchar_t *root, const wchar_t *name) {
  if (wcslen(root) + wcslen(name) + 2 > PT_PATH_CAPACITY)
    fail(L"A game or save path is too long.");
  wcscpy(output, root);
  wcscat(output, L"\\");
  wcscat(output, name);
}
static void directory(const wchar_t *path) {
  int error = SHCreateDirectoryExW(NULL, path, NULL);
  if (error != ERROR_SUCCESS && error != ERROR_ALREADY_EXISTS && error != ERROR_FILE_EXISTS)
    fail(L"Cannot create the save directory. Check PT_SAVE_DIRECTORY and folder permissions.");
}
static void check_assets(const wchar_t *path) {
  DWORD flags = GetFileAttributesW(path);
  if (flags == INVALID_FILE_ATTRIBUTES || !(flags & FILE_ATTRIBUTE_DIRECTORY))
    fail(L"Game assets could not be found.\n\nCopy your Pizza Tycoon English DOS V2.0 game files into "
         L"the PIZZA folder beside PizzaTycoon.exe, or set PT_GAME_DATA_DIR to their location.");
}

static bool asset_error(const wchar_t *path) {
  HANDLE file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (file == INVALID_HANDLE_VALUE)
    return false;
  char data[65536];
  DWORD size = 0;
  bool okay = ReadFile(file, data, sizeof(data) - 1, &size, NULL);
  CloseHandle(file);
  data[size] = 0;
  return okay && (strstr(data, "Game data:") || strstr(data, "Cannot load game assets:"));
}
static void argument(wchar_t *command, const wchar_t *text) {
  size_t used = wcslen(command), length = wcslen(text);
  if (used + 2 * length + 4 >= PT_PATH_CAPACITY)
    fail(L"The command line is too long.");
  wchar_t *out = command + used;
  if (used)
    *out++ = L' ';
  *out++ = L'"';
  unsigned slashes = 0;
  for (;; ++text) {
    if (*text == L'\\') {
      ++slashes;
      continue;
    }
    unsigned copies = (*text == L'"' || !*text) ? slashes * 2 : slashes;
    while (copies--)
      *out++ = L'\\';
    slashes = 0;
    if (!*text)
      break;
    if (*text == L'"')
      *out++ = L'\\';
    *out++ = *text;
  }
  *out++ = L'"';
  *out = 0;
}
int WINAPI wWinMain(HINSTANCE instance, HINSTANCE previous, wchar_t *text, int show) {
  (void)instance;
  (void)previous;
  (void)text;
  (void)show;
  wchar_t base[PT_PATH_CAPACITY], native[PT_PATH_CAPACITY], boot[PT_PATH_CAPACITY], folder[PT_PATH_CAPACITY];
  wchar_t disk[PT_PATH_CAPACITY], path[PT_PATH_CAPACITY], trace[PT_PATH_CAPACITY], command[PT_PATH_CAPACITY] = {0};
  DWORD length = GetModuleFileNameW(NULL, base, PT_PATH_CAPACITY);
  if (!length || length >= PT_PATH_CAPACITY)
    fail(L"Cannot locate the game folder.");
  wchar_t *slash = wcsrchr(base, L'\\');
  if (!slash)
    fail(L"Cannot locate the game folder.");
  *slash = 0;
  join(native, base, L"pizza-native.exe");
  length = GetEnvironmentVariableW(L"PT_GAME_DATA_DIR", boot, PT_PATH_CAPACITY);
  if (length >= PT_PATH_CAPACITY)
    fail(L"PT_GAME_DATA_DIR is too long.");
  if (!length)
    join(boot, base, L"PIZZA");
  check_assets(boot);
  length = GetEnvironmentVariableW(L"PT_SAVE_DIRECTORY", folder, PT_PATH_CAPACITY);
  if (length >= PT_PATH_CAPACITY)
    fail(L"PT_SAVE_DIRECTORY is too long.");
  if (!length) {
    if (SHGetFolderPathW(NULL, CSIDL_LOCAL_APPDATA, NULL, SHGFP_TYPE_CURRENT, base) != S_OK)
      fail(L"Cannot locate Local AppData.");
    join(folder, base, L"Pizza Tycoon");
  }
  directory(folder);
  join(disk, folder, L"native-disk.ptdisk");
  join(path, folder, L"native-disk.lock");
  HANDLE lock = CreateFileW(path, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if (lock == INVALID_HANDLE_VALUE) {
    if (GetLastError() == ERROR_SHARING_VIOLATION) {
      MessageBoxW(NULL, L"Pizza Tycoon is already running with this save directory.", L"Pizza Tycoon", MB_OK);
      return 0;
    }
    fail(L"Cannot lock the save directory.");
  }
  join(path, folder, L"last-run.log");
  SECURITY_ATTRIBUTES security = {sizeof(security), NULL, TRUE};
  HANDLE log = CreateFileW(path, GENERIC_WRITE, FILE_SHARE_READ, &security, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if (log == INVALID_HANDLE_VALUE)
    fail(L"Cannot open last-run.log in the save directory.");
  int argc;
  wchar_t **argv = CommandLineToArgvW(GetCommandLineW(), &argc);
  if (!argv)
    fail(L"Cannot read the launch arguments.");
  argument(command, native);
  argument(command, L"--data");
  argument(command, boot);
  argument(command, L"--disk");
  argument(command, disk);
  bool explicit_record = false;
  for (int i = 1; i < argc; ++i)
    if (!wcscmp(argv[i], L"--record"))
      explicit_record = true;
  if (!explicit_record) {
    join(path, folder, L"input-traces");
    directory(path);
    if (!GetTempFileNameW(path, L"run", 0, trace))
      fail(L"Cannot create an input trace.");
    argument(command, L"--record");
    argument(command, trace);
  }
  for (int i = 1; i < argc; ++i)
    argument(command, argv[i]);
  LocalFree(argv);
  STARTUPINFOW startup = {0};
  startup.cb = sizeof(startup);
  startup.dwFlags = STARTF_USESTDHANDLES;
  startup.hStdOutput = startup.hStdError = log;
  startup.hStdInput = CreateFileW(
      L"NUL", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, &security, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  PROCESS_INFORMATION process;
  if (!CreateProcessW(native, command, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, folder, &startup, &process))
    fail(L"Cannot start pizza-native.exe. Keep it beside PizzaTycoon.exe and SDL2.dll.");
  CloseHandle(process.hThread);
  WaitForSingleObject(process.hProcess, INFINITE);
  DWORD code = 2;
  GetExitCodeProcess(process.hProcess, &code);
  CloseHandle(process.hProcess);
  CloseHandle(startup.hStdInput);
  CloseHandle(log);
  CloseHandle(lock);
  if (code) {
    join(path, folder, L"last-run.log");
    MessageBoxW(NULL,
        asset_error(path)
            ? L"Game assets are missing, damaged or from an incompatible version.\n\n"
              L"Restore the English DOS V2.0 game assets. See last-run.log in the save directory for the failing files."
            : L"The game stopped. See last-run.log in the save directory for details.",
        L"Pizza Tycoon",
        MB_OK | MB_ICONERROR);
  }
  return (int)code;
}
