#ifndef ANALYZER_THREAD_H_
#define ANALYZER_THREAD_H_

#define UINT unsigned int

typedef struct cpu_stats_t cpu_stats_t;

/// allocate cpu_stats memory
void cpu_stats_mem_alloc(cpu_stats_t **cpu_stats, UINT cpu_count);

/// deallocates cpu_stats memory
void cpu_stats_mem_dealloc(cpu_stats_t *cpu_stats);

/// the analyzer thread start function
void *analyzer_routine();

#endif