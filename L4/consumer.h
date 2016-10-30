// consumer.h

#ifndef CONSUMER_H
#define CONSUMER_H

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

typedef struct consumer_arg {
    unsigned int thread_num;

    FILE *outFile;
    pthread_mutex_t *mutex_outFile;

    int bufferLines;
    int bufferColumns;
    volatile int *linesInBuffer;
    char **buffer;
    
    volatile int *assignment;       // Array of flags to determine which lines to read
    volatile int *termin_reading;   // Global flag to determine if all producers inactive

    pthread_mutex_t *mutex_buffer;
    pthread_cond_t *buffer_notempty;
    pthread_cond_t *buffer_empty;
    sem_t *sem_read;
    sem_t *sem_write;

    int randLow;
    int randHigh;
    
    int v_flag;         // Verbose flag
    int w_flag;         // Wait flag and offset for active waiting
} consumer_arg;

void *consume(void *arg);

#endif
