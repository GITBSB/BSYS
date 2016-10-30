// producer.c

#include "producer.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#define BLUE   "\033[34m"
#define RESET   "\033[0m"

void *produce(void *arg)
{
    producer_arg *p_arg = (producer_arg *) arg;
    int numCols = p_arg->bufferColumns;
    char *readLine = NULL;
    size_t len = 0;
    ssize_t errNum;


    int n = 0;
    while (n < p_arg->thread_lines)
    {
        int wait_fullBuffer;
        
        pthread_mutex_lock(p_arg->mutex_buffer);
        wait_fullBuffer = *p_arg->linesInBuffer >= p_arg->bufferLines;
        pthread_mutex_unlock(p_arg->mutex_buffer);
        
        while(wait_fullBuffer) {
            if (p_arg->v_flag == 1) {
                printf("P%d: Waitnig for condition variable\n\n", p_arg->thread_num);
            }
            pthread_cond_wait(p_arg->buffer_empty, p_arg->mutex_buffer);
            
            pthread_mutex_lock(p_arg->mutex_buffer);
            wait_fullBuffer = *p_arg->linesInBuffer >= p_arg->bufferLines;
            pthread_mutex_unlock(p_arg->mutex_buffer);
        }


        sem_wait(p_arg->sem_read);
        errNum = getline(&readLine, &len, p_arg->inFile);
        sem_post(p_arg->sem_read);
        if (errNum == -1)
        {
            break;
        }

        // Crop line to the length of columns in buffer
        char* croppedLine = malloc(numCols * sizeof(char));
        if(croppedLine == NULL) {
            break;
        }
        for(int i = 0; i < numCols; i++) {
            croppedLine[i] = '\0';
        }
        strncpy(croppedLine, readLine, numCols-1);
        free(readLine);
        readLine = NULL;
        if (p_arg->v_flag == 1)
        {
            printf("\n%sP%d produces %s\n%s",
                   BLUE,
                   p_arg->thread_num,
                   croppedLine,
                   RESET);
        }

        sleep((double) p_arg->timeToSleep / 1000);

        pthread_mutex_lock(p_arg->mutex_buffer); // lock mutex to write in buffer
        
        // Search free line in Buffer
        int freeLine = 0;
        while (p_arg->assignment[freeLine] == 1) {
            freeLine++;
        }
        if(freeLine >= p_arg->bufferLines) {
            pthread_cond_signal(p_arg->buffer_notempty);
            continue;
        }
        
        // Swap ptr to cropped line with respective line in buffer
        free(p_arg->buffer[freeLine]);
        p_arg->buffer[freeLine] = &croppedLine[0];
        printf("P%d: Line in Buffer -%d-: %s\n",
               p_arg->thread_num,
               freeLine,
               p_arg->buffer[freeLine]);
        p_arg->assignment[freeLine] = 1;
        *p_arg->linesInBuffer += 1;
        
        pthread_cond_signal(p_arg->buffer_notempty);
        printf("P%d: Assigned Line: %d\n\n",
               p_arg->thread_num,
               freeLine);
        pthread_mutex_unlock(p_arg->mutex_buffer);

        n++;
    }
    if (p_arg->v_flag == 1) {
        printf("P%d: Goodbye:\n\n", p_arg->thread_num);
    }
    return NULL;
}
