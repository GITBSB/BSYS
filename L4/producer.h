// producer.h

#ifndef PRODUCER_H
#define PRODUCER_H

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

typedef struct producer_arg {
    int thread_num;     // thread-id

    FILE *inFile;

    int bufferLines;
    int bufferColumns;
    char **buffer;
    volatile int *linesInBuffer;
    
    pthread_mutex_t *mutex_buffer;      // Mutex that locks accesses to the buffer
    pthread_cond_t *buffer_notempty;    // CV to signal that the consumers may consume
    pthread_cond_t *buffer_empty;       // CV to signal that the producers may produce

    sem_t *sem_read;  
    int v_flag;         // verbose flag
    int timeToSleep;    // passiv sleeping time
    int count_line;     // starting line number of file to write in buffer
    int thread_lines;   // number of lines for producer
    int *assignment;    // Array of flags to determine which lines to read
} producer_arg;

void* produce(void *argument);

#endif
