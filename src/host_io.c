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
#include <direct.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <process.h>

static wchar_t *wide_path(const char *text);
int pt_host_mkdir(const char *path) {
  wchar_t *name = wide_path(path);
  int result = name ? _wmkdir(name) : -1;
  free(name);
  return result;
}

#include <winhttp.h>
bool pt_host_https_download(
    const char *url, u8 *data, size_t capacity, size_t *size, PTHostDownloadProgress progress, void *context) {
  wchar_t *wide = wide_path(url), host[256];
  URL_COMPONENTS parts = {.dwStructSize = sizeof(parts), .dwHostNameLength = (DWORD)-1, .dwUrlPathLength = (DWORD)-1};
  HINTERNET session = NULL, connection = NULL, request = NULL;
  bool okay = false;
  *size = 0;
  if (!wide || !WinHttpCrackUrl(wide, 0, 0, &parts) || parts.nScheme != INTERNET_SCHEME_HTTPS ||
      !parts.dwHostNameLength || parts.dwHostNameLength >= 256)
    goto done;
  memcpy(host, parts.lpszHostName, parts.dwHostNameLength * sizeof(*host));
  host[parts.dwHostNameLength] = 0;
  if (progress && !progress(context, 0, capacity))
    goto done;
  session = WinHttpOpen(
      L"OpenPizza/0.1", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
  if (!session || !WinHttpSetTimeouts(session, 15000, 15000, 15000, 15000))
    goto done;
  connection = WinHttpConnect(session, host, parts.nPort, 0);
  if (!connection)
    goto done;
  request = WinHttpOpenRequest(connection,
      L"GET",
      parts.lpszUrlPath,
      NULL,
      WINHTTP_NO_REFERER,
      WINHTTP_DEFAULT_ACCEPT_TYPES,
      WINHTTP_FLAG_SECURE);
  DWORD policy = WINHTTP_OPTION_REDIRECT_POLICY_DISALLOW_HTTPS_TO_HTTP;
  if (!request || !WinHttpSetOption(request, WINHTTP_OPTION_REDIRECT_POLICY, &policy, sizeof(policy)) ||
      !WinHttpSendRequest(request, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0) ||
      !WinHttpReceiveResponse(request, NULL))
    goto done;
  DWORD status, length = sizeof(status);
  if (!WinHttpQueryHeaders(request,
          WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
          WINHTTP_HEADER_NAME_BY_INDEX,
          &status,
          &length,
          WINHTTP_NO_HEADER_INDEX) ||
      status != 200)
    goto done;
  for (;;) {
    u8 chunk[32768];
    DWORD count = 0;
    if (!WinHttpReadData(request, chunk, sizeof(chunk), &count))
      goto done;
    if (!count)
      break;
    if (count > capacity - *size)
      goto done;
    memcpy(data + *size, chunk, count);
    *size += count;
    if (progress && !progress(context, *size, capacity))
      goto done;
  }
  okay = true;
done:
  if (request)
    WinHttpCloseHandle(request);
  if (connection)
    WinHttpCloseHandle(connection);
  if (session)
    WinHttpCloseHandle(session);
  free(wide);
  return okay;
}

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
int pt_host_rmdir(const char *path) {
  wchar_t *name = wide_path(path);
  int result = name ? _wrmdir(name) : -1;
  free(name);
  return result;
}
bool pt_host_path_exists(const char *path) {
  wchar_t *name = wide_path(path);
  bool result = name && GetFileAttributesW(name) != INVALID_FILE_ATTRIBUTES;
  free(name);
  return result;
}
bool pt_host_rename_new(const char *source, const char *destination) {
  wchar_t *from = wide_path(source), *to = wide_path(destination);
  bool result = from && to && MoveFileW(from, to);
  free(from);
  free(to);
  return result;
}
char *pt_host_game_data_directory(void) {
  wchar_t base[32768];
  DWORD length = GetEnvironmentVariableW(L"LOCALAPPDATA", base, 32768);
  if (!length || length >= 32768)
    return NULL;
  const char suffix[] = "/Pizza Tycoon/game-data";
  int bytes = WideCharToMultiByte(CP_UTF8, 0, base, -1, NULL, 0, NULL, NULL);
  char *path = bytes ? malloc((size_t)bytes + sizeof(suffix)) : NULL;
  if (path) {
    WideCharToMultiByte(CP_UTF8, 0, base, -1, path, bytes, NULL, NULL);
    strcat(path, suffix);
  }
  return path;
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
#include <pwd.h>
#include <curl/curl.h>
typedef struct PTHostTransfer {
  u8 *data;
  size_t capacity, size;
  PTHostDownloadProgress progress;
  void *context;
} PTHostTransfer;
static size_t host_transfer_write(char *bytes, size_t width, size_t count, void *opaque) {
  PTHostTransfer *transfer = opaque;
  if (width && count > SIZE_MAX / width)
    return 0;
  size_t size = width * count;
  if (size > transfer->capacity - transfer->size)
    return 0;
  memcpy(transfer->data + transfer->size, bytes, size);
  transfer->size += size;
  return size;
}
static int host_transfer_progress(
    void *opaque, curl_off_t total, curl_off_t current, curl_off_t upload_total, curl_off_t upload_current) {
  (void)total;
  (void)current;
  (void)upload_total;
  (void)upload_current;
  PTHostTransfer *transfer = opaque;
  return transfer->progress && !transfer->progress(transfer->context, transfer->size, transfer->capacity);
}
bool pt_host_https_download(
    const char *url, u8 *data, size_t capacity, size_t *size, PTHostDownloadProgress progress, void *context) {
  PTHostTransfer transfer = {data, capacity, 0, progress, context};
  *size = 0;
  if (curl_global_init(CURL_GLOBAL_DEFAULT))
    return false;
  CURL *curl = curl_easy_init();
  bool okay = false;
  if (!curl)
    goto done;
#define PT_CURL(option, value)                                                                                         \
  if (curl_easy_setopt(curl, option, value) != CURLE_OK)                                                               \
  goto done
  PT_CURL(CURLOPT_URL, url);
  PT_CURL(CURLOPT_USERAGENT, "OpenPizza/0.1");
  PT_CURL(CURLOPT_FOLLOWLOCATION, 1L);
  PT_CURL(CURLOPT_MAXREDIRS, 8L);
  PT_CURL(CURLOPT_CONNECTTIMEOUT, 15L);
  PT_CURL(CURLOPT_TIMEOUT, 180L);
  PT_CURL(CURLOPT_FAILONERROR, 1L);
  PT_CURL(CURLOPT_NOSIGNAL, 1L);
#if LIBCURL_VERSION_NUM >= 0x075500
  PT_CURL(CURLOPT_PROTOCOLS_STR, "https");
  PT_CURL(CURLOPT_REDIR_PROTOCOLS_STR, "https");
#else
  PT_CURL(CURLOPT_PROTOCOLS, CURLPROTO_HTTPS);
  PT_CURL(CURLOPT_REDIR_PROTOCOLS, CURLPROTO_HTTPS);
#endif
  PT_CURL(CURLOPT_WRITEFUNCTION, host_transfer_write);
  PT_CURL(CURLOPT_WRITEDATA, &transfer);
  PT_CURL(CURLOPT_NOPROGRESS, 0L);
  PT_CURL(CURLOPT_XFERINFOFUNCTION, host_transfer_progress);
  PT_CURL(CURLOPT_XFERINFODATA, &transfer);
  okay = curl_easy_perform(curl) == CURLE_OK;
  long status = 0;
  if (curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status) != CURLE_OK || status != 200)
    okay = false;
#undef PT_CURL
  *size = transfer.size;
done:
  if (curl)
    curl_easy_cleanup(curl);
  curl_global_cleanup();
  return okay;
}

#include <sys/stat.h>
#ifdef __linux__
#include <sys/syscall.h>
#include <fcntl.h>
#endif
int pt_host_mkdir(const char *path) {
  return mkdir(path, 0700);
}
int pt_host_rmdir(const char *path) {
  return rmdir(path);
}
bool pt_host_path_exists(const char *path) {
  struct stat status;
  return lstat(path, &status) == 0;
}
bool pt_host_rename_new(const char *source, const char *destination) {
#ifdef __APPLE__
  return renamex_np(source, destination, RENAME_EXCL) == 0;
#elif defined(__linux__)
  return syscall(SYS_renameat2, AT_FDCWD, source, AT_FDCWD, destination, 1) == 0;
#else
  errno = ENOTSUP;
  return false;
#endif
}
char *pt_host_game_data_directory(void) {
  struct passwd *user = getpwuid(getuid());
  if (!user || !user->pw_dir)
    return NULL;
#ifdef __APPLE__
  const char suffix[] = "/Library/Application Support/Pizza Tycoon/game-data";
#else
  const char suffix[] = "/.local/share/Pizza Tycoon/game-data";
#endif
  size_t size = strlen(user->pw_dir) + sizeof(suffix);
  char *path = malloc(size);
  if (path)
    snprintf(path, size, "%s%s", user->pw_dir, suffix);
  return path;
}
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
