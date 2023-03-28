#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

#include "../include/printer_thread.h"
#define UINT unsigned int

void *printer_routine()
{
    extern UINT num_cpus;
    extern double *analyzr_printr_arr;
    extern sem_t analyzr_printr_arr_sem;

    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    while (1)
    {
        sem_wait(&analyzr_printr_arr_sem);
        printf("CPUs: %f%%\n", analyzr_printr_arr[0]);
        for (UINT i = 1; i < num_cpus; i++)
            printf("cpu%u: %f%%\n", i, analyzr_printr_arr[i]);
        printf("\n");
        sem_post(&analyzr_printr_arr_sem);
    }
    pthread_exit(0);
}