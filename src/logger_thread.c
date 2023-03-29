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

void logger_log(int severity, const char *date, const char *time, const char *file, int line, const char *descr, ...)
{
    //write data to file
}

static void close_fd(void *args)
{
    int fd = *((int *)args);
    TEMP_FAILURE_RETRY(close(fd));
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