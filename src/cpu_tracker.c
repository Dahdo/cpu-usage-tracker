#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <string.h>

#include "../include/circular_buffer.h"
#include "../include/reader_thread.h"

#define RING_BUFFER_SIZE 60
#define ERR(source) (perror(source),                                                           \
                     fprintf(stderr, "%s:%d:%s:%s\n", __FILE__, __LINE__, __DATE__, __TIME__), \
                     exit(EXIT_FAILURE))

// for circular ring buffer
char *buffer;
cbuf_handle_t ring_buff;

// threads tids
pthread_t reader_tid;

// circular ring buffer semaphores
sem_t empty_count, filled_count, ring_buff_sem;

int main()
{
    // initializing ring buffer
    buffer = malloc(RING_BUFFER_SIZE * sizeof(char));
    ring_buff = circular_buf_init(buffer, RING_BUFFER_SIZE);

    // initailizing semaphores
    sem_init(&empty_count, 0, RING_BUFFER_SIZE);
    sem_init(&filled_count, 0, 0);
    sem_init(&ring_buff_sem, 0, 1);

    // creating threads
    if (pthread_create(&reader_tid, NULL, reader_routine, NULL) != 0)
        ERR("can't create reader thread");

    // thread joining
    if (pthread_join(reader_tid, NULL) != 0)
        ERR("Can't join reader thread");

    // memory deallocation
    free(buffer);
    circular_buf_free(ring_buff);

    // semaphores destruction
    sem_destroy(&empty_count);
    sem_destroy(&filled_count);
    sem_destroy(&ring_buff_sem);
    exit(EXIT_SUCCESS);
}