#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <semaphore.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include "../include/logger_thread.h"

#define ERR(source) (perror(source),                                 \
                     fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), \
                     exit(EXIT_FAILURE))

extern char *logger_buff;
extern sem_t logger_buff_sem;

static char *levels_str[] = {
    "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};
static int write_data(int fd)
{
    int c = TEMP_FAILURE_RETRY(write(fd, logger_buff, strlen(logger_buff)));
    return c;
}

static void realloc_logger_buff(int size)
{
    logger_buff = realloc(logger_buff, sizeof(char) * size);
    if (NULL == logger_buff)
        ERR("Can't reallocate memory for logger_str_buff");
}

void alloc_logger_buff(int size)
{
    logger_buff = malloc(size * sizeof(char));
    if (NULL == logger_buff)
        ERR("Can't allocate memory for logger_str_buff");
    logger_buff[0] = '\0'; //to supress errors with writing to uninitialized buffer
}

static void close_fd(void *args)
{
    int fd = *((int *)args);
    TEMP_FAILURE_RETRY(close(fd));
}
int digit_count(int n)
{
    if (n / 10 == 0)
        return 1;
    return 1 + digit_count(n / 10);
}
static int calc_buff_size(const char *severity, const char *date, const char *time, const char *file, int line, const char *descr)
{
    int size = strlen(severity) + strlen(date) + strlen(time) + strlen(file) + strlen(descr) + digit_count(line);
    size += 17; // extra chars for formatting (: and spaces)  and '\0'
    return size;
}
void logger_log(int severity, const char *date, const char *time, const char *file, int line, const char *descr, ...)
{
    sem_wait(&logger_buff_sem);

    if ('\0' == logger_buff[0]) {// modify the buffer only after it has been written to file
        int size = calc_buff_size(levels_str[severity], date, time, file, line, descr);
        realloc_logger_buff(size);
        sprintf(logger_buff, "%s : %s : %s : %s : %d : %s\n", date, time, levels_str[severity], file, line, descr);
    }

    sem_post(&logger_buff_sem);
}

void *logger_routine()
{
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    int fd;
    if ((fd = open("log-cputracker", O_WRONLY | O_CREAT | O_TRUNC | O_APPEND, 0777)) < 0)
        ERR("error opening creating fd - logger thread");
    pthread_cleanup_push(close_fd, &fd);

    while (1)
    {

    }
    pthread_cleanup_pop(0);
    pthread_exit(0);
}