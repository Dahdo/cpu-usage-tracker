#define _GNU_SOURCE
#include <pthread.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include "../include/reader_thread.h"

#define ERR(source) (perror(source),                                                           \
                     fprintf(stderr, "%s:%d:%s:%s\n", __FILE__, __LINE__, __DATE__, __TIME__), \
                     exit(EXIT_FAILURE))

static char fd_read_single(int fd)
{
    char *char_buff = malloc(sizeof(char));
    int c = TEMP_FAILURE_RETRY(read(fd, char_buff, sizeof(char)));
    if (c < 0)
        ERR("can't read in fd_read_single");
    else if (c == 0)
        ERR("EOF reached in fd_read_single");
    char read_char = *char_buff;
    free(char_buff);
    return read_char;
}