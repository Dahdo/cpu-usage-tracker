#define _XOPEN_SOURCE 500 // incorporating POSIX 1995 to avoid errors/warnings(in editor) with struct sigaction
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <string.h>
#include <stdatomic.h>
#include <signal.h>

#include "../include/circular_buffer.h"
#include "../include/reader_thread.h"
#include "../include/analyzer_thread.h"
#include "../include/printer_thread.h"
#include "../include/logger_thread.h"

#define RING_BUFFER_SIZE 60
#define ERR(source) (perror(source),                                                           \
                     fprintf(stderr, "%s:%d:%s:%s\n", __FILE__, __LINE__, __DATE__, __TIME__), \
                     exit(EXIT_FAILURE))

// for circular ring buffer
char *buffer;
cbuf_handle_t ring_buff;

// threads tids
pthread_t reader_tid, analyzer_tid, printer_tid, logger_tid;

// circular ring buffer semaphores
sem_t empty_count, filled_count, ring_buff_sem, logger_buff_sem;

// buffers to be used in analyzer thread
cpu_stats_t *curr_stats;
cpu_stats_t *prev_stats;
char *ring_buff_str;

// logger thread buffer
char *logger_buff;

// for analyzer thread - printer thread communication
double *analyzr_printr_arr;
sem_t analyzr_printr_arr_sem;
UINT num_cpus;
volatile atomic_int analyzr_printr_sync = 0;

// sigterm handler
void sigterm_handler()
{
    pthread_cancel(reader_tid);
    pthread_cancel(analyzer_tid);
    pthread_cancel(printer_tid);
    pthread_cancel(logger_tid);
}

int main()
{
    //sigterm initialization
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = sigterm_handler;
    if (-1 == sigaction(SIGTERM, &action, NULL))
        ERR("error sigterm");

    // initializing ring buffer
    buffer = malloc(RING_BUFFER_SIZE * sizeof(char));
    ring_buff = circular_buf_init(buffer, RING_BUFFER_SIZE);

    // initailizing semaphores
    sem_init(&empty_count, 0, RING_BUFFER_SIZE);
    sem_init(&filled_count, 0, 0);
    sem_init(&ring_buff_sem, 0, 1);
    sem_init(&analyzr_printr_arr_sem, 0, 1);
    sem_init(&logger_buff_sem, 0, 1);

    // creating threads
    if (pthread_create(&reader_tid, NULL, reader_routine, NULL) != 0)
        ERR("can't create reader thread");
     if (pthread_create(&analyzer_tid, NULL, analyzer_routine, NULL) != 0)
        ERR("can't create analyzer thread");
    if (pthread_create(&printer_tid, NULL, printer_routine, NULL) != 0)
        ERR("can't create printer thread");
    if(pthread_create(&logger_tid, NULL, logger_routine, NULL) != 0)
        ERR("can't create logger thread");

    // thread joining
    if (pthread_join(reader_tid, NULL) != 0)
        ERR("Can't join reader thread");
    if (pthread_join(analyzer_tid, NULL) != 0)
        ERR("Can't join analyzer thread");
    if (pthread_join(printer_tid, NULL) != 0)
        ERR("Can't join printer thread");
    if (pthread_join(logger_tid, NULL) != 0)
        ERR("Can't join logger thread");

    // memory deallocation
    free(buffer);
    circular_buf_free(ring_buff);
    free(ring_buff_str);
    if (curr_stats)
        cpu_stats_mem_dealloc(curr_stats);
    if (prev_stats)
        cpu_stats_mem_dealloc(prev_stats);
    free(logger_buff);

    // semaphores destruction
    sem_destroy(&empty_count);
    sem_destroy(&filled_count);
    sem_destroy(&ring_buff_sem);
    sem_destroy(&analyzr_printr_arr_sem);
    sem_destroy(&logger_buff_sem);
    exit(EXIT_SUCCESS);
}