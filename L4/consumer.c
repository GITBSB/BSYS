//consumer.c

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/timeb.h>

#include "consumer.h"

#define BLUE   "\033[34m"
#define RESET   "\033[0m"

/*
 * Converts a c-string to uppercase and copies the result in a newly
 * allocated space. You must free the resulting space youself.
 * 
 * It is asumed that "length" is the length of the string without the
 * nulltermination-symbol '\0'.
 * 
 * The resulting string will be (length +1) chars long.
 */
char *stringToupper(char *string);
void activeWait(int sec);


void *consume(void *arg) {
    assert(arg != NULL);
    
    consumer_arg *argument = (consumer_arg *) arg;
    assert(argument->w_flag >= 0);

    int numLinesInBuffer = argument->bufferLines;

    int errNum = 0;
    int rng = 0;
    printf("C%d: Consumer starts\n",argument->thread_num);
    
    int resumeConsumer;
    
    pthread_mutex_lock(argument->mutex_buffer);
    resumeConsumer = argument->termin_reading == 0 || *argument->linesInBuffer > 0;
    pthread_mutex_unlock(argument->mutex_buffer);
    
    while(resumeConsumer) {

        pthread_mutex_lock(argument->mutex_buffer);

        int wait = *argument->linesInBuffer < 1;
        pthread_mutex_unlock(argument->mutex_buffer);
        while (wait)
        {
            printf("C%d: Consumer waits\n", argument->thread_num);
            pthread_cond_wait(argument->buffer_notempty, argument->mutex_buffer);
            
            // linesInBuffer must be locked to be thread-safe
            pthread_mutex_lock(argument->mutex_buffer);
            wait = *argument->linesInBuffer < 1;
            pthread_mutex_unlock(argument->mutex_buffer);
        }


        pthread_mutex_lock(argument->mutex_buffer); // Lock
    
        int line = 0;
        while(argument->assignment[line] == 0 && line < numLinesInBuffer) {
            line++;
        }
        if (argument->assignment[line] == 0) {
            resumeConsumer = argument->termin_reading == 0 || *argument->linesInBuffer > 0;
            pthread_mutex_unlock(argument->mutex_buffer);
            continue;
        }
        printf("C%d: get Assigned Line -%d-\n",
               argument->thread_num,
               line);
        
        char *string = malloc(argument->bufferColumns * sizeof(char));
        strncpy(string, argument->buffer[line], argument->bufferColumns - 1);
        string[argument->bufferColumns - 1] = '\0';
        printf("C%d: Line -%d- from buffer %s\n",argument->thread_num,
                line, argument->buffer[line]);
        argument->assignment[line] = 0;
        *argument->linesInBuffer -= 1;
        printf("C%d: Lines in Buffer -%d-\n",argument->thread_num, *argument->linesInBuffer);
        
        pthread_mutex_unlock(argument->mutex_buffer);

        if(argument->v_flag == 1) {
            printf("%sC%d consumes %s\n%s",
                   BLUE,
                   argument->thread_num,
                   string,
                   RESET);
        }


        // Convert element in buffer to UPPERCASE
        char *uppercase = stringToupper(string);
        free(string);

        // Wait for a random time passively or actively
        int active_offset = argument->w_flag;
        int min = argument->randLow + active_offset;
        int max = argument->randHigh + active_offset;
        struct timeb start, end;
        
        rng = random() % (max - min + 1) + min;
        // START TIME

        
        ftime(&start);
        if (active_offset > 0) {
            activeWait(rng);    // Active waiting
        } else {
            sleep(rng);         // Passive waiting
        }
        ftime(&end);
        int diff = 1000 * (end.time - start.time) + (end.millitm - start.millitm);
        
        // Write to Output file
        pthread_mutex_lock(argument->mutex_outFile);
        if(argument->outFile != NULL) {
            errNum = fprintf(argument->outFile, "%s\n", uppercase);
            if (errNum < 0) {
                perror("fprintf()");
            }
        }
        pthread_mutex_unlock(argument->mutex_outFile);

        // Printf if verbose
        if(argument->v_flag == 1) {
            printf("%sC%d (%05d) reports %s\n%s",
                   BLUE,
                   argument->thread_num,
                   diff,
                   uppercase,
                   RESET);
        }
        free(uppercase);
        
        pthread_mutex_lock(argument->mutex_buffer);
        resumeConsumer = argument->termin_reading == 0 || *argument->linesInBuffer > 0;
        if(*argument->termin_reading == 0) {
            // send signal
            printf("%sC%d HELLO PRODUCERS \n%s",
                   BLUE, argument->thread_num, RESET);
            pthread_cond_signal(argument->buffer_empty);
        }
        pthread_mutex_unlock(argument->mutex_buffer);
    }

    return NULL;
}


char *stringToupper(char *string) {

    int stringLength = 0;
    while (string[stringLength] != '\0') {
        stringLength++;
    }

    char *uppercase = malloc((stringLength + 1) * sizeof(char));
    for (int l = 0; l < stringLength; l++) {
        uppercase[l] = toupper(string[l]);
    }
    uppercase[stringLength] = '\0';

    return uppercase;
}

void activeWait(int sec) {
    struct timeb start, current;
    ftime(&start);
    int diff = 0;

    while(diff <= sec) {
        ftime(&current);
        diff = 1000 * (current.time - start.time) + (current.millitm - start.millitm);
    }
}
