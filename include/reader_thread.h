#ifndef READER_THREAD_H_
#define READER_THREAD_H_

#include <semaphore.h>
#include "circular_buffer.h"

/// the reader thread start function
void *reader_routine();

#endif