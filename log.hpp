/**
 * Copyright (c) 2025 ZhangzrJerry
 *
 * Modified from https://github.com/rxi/log.c
 */

#ifndef LOG_HPP
#define LOG_HPP

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <mutex>

#define LOG_VERSION "0.1.0"
#define MAX_CALLBACKS 32
#define LOG_USE_COLOR

#ifdef _WIN32
#include <windows.h>
void show_console() {
  DWORD processId = GetCurrentProcessId();
  HWND hwnd = GetConsoleWindow();
  if (!hwnd) {
    AllocConsole();
    hwnd = GetConsoleWindow();
  }
  ShowWindow(hwnd, SW_SHOW);
}
#else
void show_console() {}
#endif

typedef struct {
  va_list ap;
  const char *fmt;
  const char *file;
  struct tm *time;
  void *udata;
  int line;
  int level;
} log_Event;

typedef void (*log_LogFn)(log_Event *ev);
typedef void (*log_LockFn)(bool lock, void *udata);

typedef struct {
  log_LogFn fn;
  void *udata;
  int level;
} Callback;

static struct {
  void *udata;
  log_LockFn lock;
  int level;
  bool quiet;
  Callback callbacks[MAX_CALLBACKS];
} L;

enum { LOG_TRACE, LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_ERROR, LOG_FATAL };

static std::mutex log_mutex;
static const char *level_strings[] = {"TRACE", "DEBUG", "INFO",
                                      "WARN",  "ERROR", "FATAL"};

#ifdef LOG_USE_COLOR
static const char *level_colors[] = {"\x1b[94m", "\x1b[36m", "\x1b[32m",
                                     "\x1b[33m", "\x1b[31m", "\x1b[35m"};
#endif

#define log_trace(...) log_log(LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define log_debug(...) log_log(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define log_info(...) log_log(LOG_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define log_warn(...) log_log(LOG_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...) log_log(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define log_fatal(...) log_log(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)

static void stdout_callback(log_Event *ev) {
  char buf[16];
  buf[strftime(buf, sizeof(buf), "%H:%M:%S", ev->time)] = '\0';
#ifdef LOG_USE_COLOR
  fprintf((FILE *)ev->udata, "%s %s%-5s\x1b[0m \x1b[90m%s:%d:\x1b[0m ", buf,
          level_colors[ev->level], level_strings[ev->level], ev->file,
          ev->line);
#else
  fprintf((FILE *)ev->udata, "%s %-5s %s:%d: ", buf, level_strings[ev->level],
          ev->file, ev->line);
#endif
  vfprintf((FILE *)ev->udata, ev->fmt, ev->ap);
  fprintf((FILE *)ev->udata, "\n");
  fflush((FILE *)ev->udata);
}

static void file_callback(log_Event *ev) {
  char buf[64];
  buf[strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", ev->time)] = '\0';
  fprintf((FILE *)ev->udata, "%s %-5s %s:%d: ", buf, level_strings[ev->level],
          ev->file, ev->line);
  vfprintf((FILE *)ev->udata, ev->fmt, ev->ap);
  fprintf((FILE *)ev->udata, "\n");
  fflush((FILE *)ev->udata);
}

static void lock(void) {
  log_mutex.lock();
  if (L.lock) {
    L.lock(true, L.udata);
  }
}

static void unlock(void) {
  if (L.lock) {
    L.lock(false, L.udata);
  }
  log_mutex.unlock();
}

const char *log_level_string(int level) { return level_strings[level]; }

void log_set_lock(log_LockFn fn, void *udata) {
  L.lock = fn;
  L.udata = udata;
}

void log_set_level(int level) { L.level = level; }

void log_set_quiet(bool enable) { L.quiet = enable; }

int log_add_callback(log_LogFn fn, void *udata, int level) {
  for (int i = 0; i < MAX_CALLBACKS; i++) {
    if (!L.callbacks[i].fn) {
      Callback cb;
      cb.fn = fn;
      cb.udata = udata;
      cb.level = level;
      L.callbacks[i] = cb;
      return 0;
    }
  }
  return -1;
}

int log_add_fp(FILE *fp, int level) {
  return log_add_callback(file_callback, fp, level);
}

static void init_event(log_Event *ev, void *udata) {
  if (!ev->time) {
    time_t t = time(NULL);
    ev->time = localtime(&t);
  }
  ev->udata = udata;
}

void log_log(int level, const char *file, int line, const char *fmt, ...) {
  log_Event ev;
  ev.fmt = fmt;
  ev.file = file;
  ev.line = line;
  ev.level = level;
  ev.time = nullptr;

  lock();

  if (!L.quiet && level >= L.level) {
    init_event(&ev, stderr);
    va_start(ev.ap, fmt);
    stdout_callback(&ev);
    va_end(ev.ap);
  }

  for (int i = 0; i < MAX_CALLBACKS && L.callbacks[i].fn; i++) {
    Callback *cb = &L.callbacks[i];
    if (level >= cb->level) {
      init_event(&ev, cb->udata);
      va_start(ev.ap, fmt);
      cb->fn(&ev);
      va_end(ev.ap);
    }
  }

  unlock();
}

#endif
