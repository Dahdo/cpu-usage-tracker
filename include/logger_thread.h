#ifndef LOGGER_THREAD_H_
#define LOGGER_THREAD_H_

enum
{
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_FATAL
};

#define log_debug(...) logger_log(LOG_DEBUG, __DATE__, __TIME__, __FILE__, __LINE__, __VA_ARGS__)
#define log_info(...) logger_log(LOG_INFO, __DATE__, __TIME__, __FILE__, __LINE__, __VA_ARGS__)
#define log_warn(...) logger_log(LOG_WARN, __DATE__, __TIME__, __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...) logger_log(LOG_ERROR, __DATE__, __TIME__, __FILE__, __LINE__, __VA_ARGS__)
#define log_fatal(...) logger_log(LOG_FATAL, __DATE__, __TIME__, __FILE__, __LINE__, __VA_ARGS__)

/// void log_log(int level, const char *file, int line, const char *fmt, ...);
void logger_log(int severity, const char *date, const char *time, const char *file, int line, const char *descr, ...);
void alloc_logger_buff(int size);
void *logger_routine();

#endif