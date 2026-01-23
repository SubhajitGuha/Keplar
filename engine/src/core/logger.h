#pragma once
#include "defines.h"

#define MAX_LOG_CHARACTERS 36000

#define LOG_WARN_ENABLE 1
#define LOG_INFO_ENABLE 1
#define LOG_DEBUG_ENABLE 1
#define LOG_TRACE_ENABLE 1

#ifdef KRELEASE
#define LOG_DEBUG_ENABLE 0
#define LOG_TRACE_ENABLE 0
#endif

typedef enum LogLevel
{
    LOG_FETAL,
    LOG_ERROR,
    LOG_WARN,
    LOG_INFO,
    LOG_DEBUG,
    LOG_TRACE,
}LogLevel;

b8 initilize_logging();
void shutdown_logging();

KAPI void log_output(LogLevel logLevel,const char* message, ...);

#define KFETAL(message,...) log_output(LOG_FETAL, message, ##__VA_ARGS__)

#define KERROR(message, ...) log_output(LOG_ERROR, message, ##__VA_ARGS__)

#if LOG_WARN_ENABLE == 1
#define KWARN(message, ...) log_output(LOG_WARN, message, ##__VA_ARGS__)
#else
#define KWARN(message, ...)
#endif

#if LOG_INFO_ENABLE == 1
#define KINFO(message, ...) log_output(LOG_INFO, message, ##__VA_ARGS__)
#else
#define KINFO(message, ...)
#endif

#if LOG_DEBUG_ENABLE == 1
#define KDEBUG(message, ...) log_output(LOG_DEBUG, message, ##__VA_ARGS__)
#else
#define KDEBUG(message, ...)
#endif

#if LOG_TRACE_ENABLE == 1
#define KTRACE(message, ...) log_output(LOG_TRACE, message, ##__VA_ARGS__)
#else
#define KTRACE(message, ...)
#endif
