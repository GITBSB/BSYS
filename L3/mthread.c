// mthread.c

#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <assert.h>
#include <pthread.h>
#include "gitversion.c"

extern const char *gitversion;

// These values are not discussed in the exercise, but I limit the time
// to wait for easy testing; you do not want to wait for minutes ;) ...
int minTimeToWait = 0;
int maxTimeToWait = 10;

void printHelp()
{
    printf("\nThis program creates N threads, each waiting a random\n");
    printf("time from O (inclusive) to P (inclusive) seconds.\n");
    printf("The current git version is: %s\n", gitversion);
    printf("Options:\n");
    printf("\t-t N [Number of threads]\n");
    printf("\t-h Print this help\n");
    printf("\t-v More output\n");
    printf("\t-r O [Minimal time to wait in seconds]\n");
    printf("\t-R P [Minimal time to wait in seconds]\n");
}

void *PrintHello(void *threadarg)
{
    int *threadNum = (int *) threadarg;
    int sleeptime = rand() % (maxTimeToWait - minTimeToWait + 1) + minTimeToWait;

    sleep(sleeptime);

    printf("%d: Thread is done after sleeping %d[s]\n", *threadNum, sleeptime);

    return(&errno);
}

int main(int argc, char *argv[])
{
    int status = 0;
    int *statusPtr = &status;
    long NumberOfThreads = 1;

    // Initialize the RNG with currnet microseconds
    struct timeval seed;
    gettimeofday(&seed, NULL);
    srand(seed.tv_usec);

        /* getopt parsing arguments */
    char c;
    int v_flag = 0;
    opterr = 0;
    while( (c = getopt(argc, argv, "hvr:R:t:")) != -1 )
    {
        switch(c)
        {
        case 't':
            NumberOfThreads = atoi(optarg);
            break;
        case 'v':
            v_flag = 1;
            break;
        case 'r':
            minTimeToWait = atoi(optarg);
            break;
        case 'R':
            maxTimeToWait = atoi(optarg);
            break;
        case '?':
            if(optopt == 'c')
                fprintf(stderr, "Option -%c requires an argument.\n", optopt);
            else if( isprint(optopt) )	/* If option is printable */
                fprintf(stderr, "Unknown option -%c.\n", optopt);
            else
                fprintf(stderr, "Unrecognized Argument(s)!\n");
            /* break; is not here by intent;
             * so help is printed to show valid arguments */
        case 'h':
            printHelp();
            return 0;
            break;
        default:
            break;
        }
    }

        /********************************************
        * Initialise thread attributes here        *
        ********************************************/
    if (v_flag == 1)
        printf("Number of threads to start: %ld\n", NumberOfThreads);

    pthread_t threads[NumberOfThreads];
    int thread_args[NumberOfThreads];
    int rc;
    
    for (long t = 0; t < NumberOfThreads; t++) {
        printf("Creating thread %ld\n", t);
        thread_args[t] = t;
        rc = pthread_create(&threads[t], NULL, PrintHello, (void*)
                                        &thread_args[t]);
            if (v_flag == 1) {
                if (rc==0) {
                    printf("Thread %ld sucessfully created.", t);
                } else {
                    printf("Thread %ld could not be created!", t);
                    perror("pthread_create()");
                }
            }
        assert(rc==0);
    }

    for (long t = 0; t < NumberOfThreads; t++) {
        printf("Main joining with thread %ld\n", t);
        rc = pthread_join(threads[t], (void *) &statusPtr);

        if (v_flag == 1) {
            if (rc == 0) {
                printf("Joined thread %ld successfully", t);
            } else {
                printf("Joining thread %ld was not successful!", t);
                perror("pthread_join()");
            }
        }
        assert(rc == 0);

        printf("Completed joining with thread %ld status = %d\n", t, status);
    }

    if (v_flag == 1) {
        printf("All %ld threads joined", NumberOfThreads);
    }

    return (0);
}
