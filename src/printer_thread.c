#include <pthread.h>
#include <semaphore.h>
#include <stdatomic.h>
#include <stdio.h>
#include "../include/printer_thread.h"
#include "../include/logger_thread.h"
#include "../include/watchdog_thread.h"

#define UINT unsigned int

void *printer_routine()
{
    extern UINT num_cpus;
    extern double *analyzr_printr_arr;
    extern sem_t analyzr_printr_arr_sem;
    extern volatile atomic_int analyzr_printr_sync;

    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    while (1)
    {
        // notify watchdog
        watchdog_notify(2);

        if (analyzr_printr_sync)
        {
            sem_wait(&analyzr_printr_arr_sem);
            printf("CPUs: %f%%\n", analyzr_printr_arr[0]);
            for (UINT i = 1; i < num_cpus; i++)
                printf("cpu%u: %f%%\n", i, analyzr_printr_arr[i]);
            printf("\n");
            sem_post(&analyzr_printr_arr_sem);
            analyzr_printr_sync = 0;
            log_debug("data printed");
        }
    }
    pthread_exit(0);
}