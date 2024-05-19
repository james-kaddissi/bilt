/*
 The purpose of this header file is to give level specific control over what is being outputted,
 and how it is being outputted. Only enabled levels will be logged.
*/
#pragma once
#include "defines.h"

// toggle desired logs, fatal and error always logged
#define WARN_ON 1
#define INFO_ON 1
#define DEBUG_ON 1
#define TRACE_ON 1
// manually turn off trace and debug logs if building in release mode
#if RELEASE == 1
#define DEBUG_ON 0
#define TRACE_ON 0
#endif

// define log levels
typedef enum log_level {
    LOG_LEVEL_FATAL = 0,
    LOG_LEVEL_ERROR = 1, 
    LOG_LEVEL_WARN = 2,
    LOG_LEVEL_INFO = 3,
    LOG_LEVEL_DEBUG = 4,
    LOG_LEVEL_TRACE = 5
} log_level;
// fucntions to start and stop the logger
b8 start_log();
void stop_log();

// function to log output with variable arguments
BILT_API void log_output(log_level level, const char* message, ...);

// control definitions for logging at different levels
#ifndef LOG_FATAL
#define LOG_FATAL(message, ...) log_output(LOG_LEVEL_FATAL, message, ##__VA_ARGS__);
#endif
#ifndef LOG_ERROR
#define LOG_ERROR(message, ...) log_output(LOG_LEVEL_ERROR, message, ##__VA_ARGS__);
#endif
#if WARN_ON == 1
#define LOG_WARN(message, ...) log_output(LOG_LEVEL_WARN, message, ##__VA_ARGS__);
#else
#define LOG_WARN(message, ...)
#endif
#if INFO_ON == 1
#define LOG_INFO(message, ...) log_output(LOG_LEVEL_INFO, message, ##__VA_ARGS__);
#else
#define LOG_INFO(message, ...)
#endif
#if DEBUG_ON == 1
#define LOG_DEBUG(message, ...) log_output(LOG_LEVEL_DEBUG, message, ##__VA_ARGS__);
#else
#define LOG_DEBUG(message, ...)
#endif
#if TRACE_ON == 1
#define LOG_TRACE(message, ...) log_output(LOG_LEVEL_TRACE, message, ##__VA_ARGS__);
#else
#define LOG_TRACE(message, ...)
#endif