#define _GNU_SOURCE
#include <pthread.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include "../include/reader_thread.h"
#include "../include/logger_thread.h"
#include "../include/watchdog_thread.h"

#define SEMAPHORES_WAIT     \
    sem_wait(&empty_count); \
    sem_wait(&ring_buff_sem);
#define SEMAPHORES_POST       \
    sem_post(&ring_buff_sem); \
    sem_post(&filled_count);

static char fd_read_single(int fd)
{
    char *char_buff = malloc(sizeof(char));
    if(NULL == char_buff) {
        log_fatal("can't allocate char_buff. exiting...");
        kill(getpid(), SIGTERM);
    }
    int c = TEMP_FAILURE_RETRY(read(fd, char_buff, sizeof(char)));
    if (c < 0)
        log_error("can't read in fd_read_single");
    else if (c == 0)
        log_error("EOF reached in fd_read_single");
    char read_char = *char_buff;
    free(char_buff);
    return read_char;
}


void *reader_routine()
{
    extern sem_t empty_count;
    extern sem_t filled_count;
    extern sem_t ring_buff_sem;
    extern cbuf_handle_t ring_buff;

    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    while (1)
    {
        // notify watchdog
        watchdog_notify(0);

        int fd;
        if ((fd = TEMP_FAILURE_RETRY(open("/proc/stat", O_RDONLY))) < 0) {
            log_fatal("error opening file descriptor fd. exiting...");
            kill(getpid(), SIGTERM);
        }

        char curr_char = fd_read_single(fd);
        char prev_char;

        do
        {
            SEMAPHORES_WAIT
            prev_char = curr_char;
            curr_char = fd_read_single(fd);
            circular_buf_put(ring_buff, prev_char);
            SEMAPHORES_POST

        } while (!(prev_char == '\n' && curr_char != 'c'));

        SEMAPHORES_WAIT
        circular_buf_put(ring_buff, '|');
        log_debug("finished reading data for all cpus");
        SEMAPHORES_POST

        close(fd);
        sleep(1);
    }
    pthread_exit(0);
}