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

static double get_time_diff(struct timespec *start)
{
    struct timespec end;
    if (clock_gettime(CLOCK_REALTIME, &end))
        log_error("can't initialize end");

    return (end.tv_sec - start->tv_sec) + (end.tv_nsec - start->tv_nsec) / BILLION;
}

void *watchdog_routine()
{
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    init_time();
    pthread_cleanup_push(free, time_start);
    while (1)
    {
        for (int i = 0; i < NUMTHREADS; i++)
        {
            sem_wait(&watchdog_time_sem);
            
            double diff_time = get_time_diff(&time_start[i]);

            sem_post(&watchdog_time_sem);

            if (diff_time > TIMEOUT)
            {
                switch (i)
                {
                case 0:
                    log_fatal("reader thread time out. exiting...");
                    break;
                case 1:
                    log_fatal("analyzer thread time out. exiting...");
                    break;
                case 2:
                    log_fatal("printer thread time out. exiting...");
                    break;
                case 3:
                    log_fatal("logger thread time out. exiting...");
                    break;
                default:
                    log_fatal("unknown time out. exiting...");
                    break;
                }
                sleep(1); // some time to log before termination
                kill(getpid(), SIGTERM);
            }
        }
    }
    pthread_cleanup_pop(0);
    pthread_exit(0);
}