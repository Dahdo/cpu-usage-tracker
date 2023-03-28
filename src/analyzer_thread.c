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

#define ERR(source) (perror(source),                                                           \
                     fprintf(stderr, "%s:%d:%s:%s\n", __FILE__, __LINE__, __DATE__, __TIME__), \
                     exit(EXIT_FAILURE))

#define DEFAULT_STRING_SIZE 128
extern sem_t empty_count;
extern sem_t filled_count;
extern sem_t ring_buff_sem;
extern cbuf_handle_t ring_buff;

struct cpu_stats_t // struct to hold one second's cpu data
{
    UINT cpu_count;
    UINT num_sections;
    long int *attr[]; //an cpu_count array(of size cpu_count) of pointers to arrays of length num_sections
};

static void stats_attr_init(long int *stats_attr, char *cpu_line, UINT num_sections)
{
    char *context = NULL;
    char *token = strtok_r(cpu_line, " ", &context);

    UINT count = 0;
    while ((token = strtok_r(NULL, " ", &context))) {

        if (count < num_sections) {
            stats_attr[count++] = strtol(token, NULL, 10);
        }
    }
}

static void cpu_stats_init(cpu_stats_t *cpu_stats, char *ring_buff_str, UINT cpu_count)
{
    cpu_stats->cpu_count = cpu_count;
    cpu_stats->num_sections = 10; // num of sections in the file /proc/stat as of current linux kernel

    char *context = NULL;
    char *cpu_line_token = strtok_r(ring_buff_str, "\n", &context);

    UINT count = 0;
    if (NULL != cpu_line_token) {
        stats_attr_init(cpu_stats->attr[count++], cpu_line_token, cpu_stats->num_sections);
    }

    while ((cpu_line_token = strtok_r(NULL, "\n", &context))) {
        if (count < cpu_stats->cpu_count)
            stats_attr_init(cpu_stats->attr[count++], cpu_line_token, cpu_stats->num_sections);
    }
}

void cpu_stats_mem_alloc(cpu_stats_t **cpu_stats, UINT cpu_count)
{
    *cpu_stats = malloc(sizeof(cpu_stats_t) + cpu_count * sizeof(long int *));
    if (NULL == cpu_stats) {
        ERR("error allocating cpu_stats. exiting...");
    }
    (*cpu_stats)->num_sections = 10;
    for (UINT i = 0; i < cpu_count; i++)
        (*cpu_stats)->attr[i] = malloc(sizeof(long int) * (*cpu_stats)->num_sections);
}

void cpu_stats_mem_dealloc(cpu_stats_t *cpu_stats)
{
    UINT cpu_count = cpu_stats->cpu_count;
    for (UINT i = 0; i < cpu_count; i++)
        free(cpu_stats->attr[i]);
    free(cpu_stats);
}

void *analyzer_routine()
{
    extern cpu_stats_t *curr_stats; //to hold the current second data
    extern cpu_stats_t *prev_stats; //to hold the prev second data
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    extern char *ring_buff_str;

    ring_buff_str = calloc(DEFAULT_STRING_SIZE, sizeof(char));
    size_t curr_str_size = DEFAULT_STRING_SIZE;
    UINT cpu_count = 0;

    char *tmp_char = malloc(sizeof(char)); //to temporarily hold each char read form ring buffer.
    pthread_cleanup_push(free, tmp_char);

    while (1)
    {

        sem_wait(&filled_count);
        sem_wait(&ring_buff_sem);

        circular_buf_get(ring_buff, tmp_char);

        if (strlen(ring_buff_str) == curr_str_size - 1) // if ring_buff_str is full
        {
            ring_buff_str = realloc(ring_buff_str, sizeof(char) * (curr_str_size += 8));
            if (NULL == ring_buff_str){
                ERR("can't realloc ring_buff_str");
            }
        }

        if (*tmp_char != '|') // not yet finished reading full data for all cpus
        {
            strncat(ring_buff_str, tmp_char, 1); // add the read char to ring_buff_str
            if (*tmp_char == '\n')
                cpu_count++;
        }

        else // finished rading one cpu data
        {
            // allocating curr_stats only at the beginning 
            if (NULL == curr_stats)
                cpu_stats_mem_alloc(&curr_stats, cpu_count);

            // initializing the curr_stats
            cpu_stats_init(curr_stats, ring_buff_str, cpu_count);

            //read data i.e send to printer thread

            if (NULL == prev_stats)
                cpu_stats_mem_alloc(&prev_stats, curr_stats->cpu_count);
            //cpu_stats_copy(prev_stats, curr_stats);//keep the prev second's data
        }
        sem_post(&ring_buff_sem);
        sem_post(&empty_count);
    }
    pthread_cleanup_pop(0);
    pthread_exit(0);
}
