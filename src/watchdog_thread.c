#include <signal.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
#include "../include/watchdog_thread.h"
#include "../include/logger_thread.h"

#define NUMTHREADS 4 //excluding itself

static struct timespec *time_start;
extern sem_t watchdog_time_sem;

static void init_time()
{
    sem_wait(&watchdog_time_sem);

    time_start = malloc(NUMTHREADS * sizeof(struct timespec));
    if (NULL == time_start)
        log_error("can't allocate mem for time_start");

    for (int i = 0; i < NUMTHREADS; i++)
    {
        if (clock_gettime(CLOCK_REALTIME, &time_start[i]) == -1)
            log_error("can't initialize time_start array");
    }

    sem_post(&watchdog_time_sem);
}

void *watchdog_routine()
{
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    init_time();
    pthread_cleanup_push(free, time_start);
    while (1)
    {
        
    }
    pthread_cleanup_pop(0);
    pthread_exit(0);
}