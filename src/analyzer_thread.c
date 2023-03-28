#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdatomic.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include "../include/circular_buffer.h"
#include "../include/analyzer_thread.h"

extern sem_t empty_count;
extern sem_t filled_count;
extern sem_t ring_buff_sem;

extern cbuf_handle_t ring_buff;

void *analyzer_routine()
{
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    char *tmp_char = malloc(sizeof(char)); // will be used to hold char read form ring buffer.
    int n = 500;
    while (n--)
    {
        sem_wait(&filled_count);
        sem_wait(&ring_buff_sem);

        circular_buf_get(ring_buff, tmp_char);

        sem_post(&ring_buff_sem);
        sem_post(&empty_count);
    }
    free(tmp_char);
    pthread_exit(0);
}
