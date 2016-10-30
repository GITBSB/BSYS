/**mutex.c
 * mutex - simple mutex example.
 */

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

/* warning:  due to this GNUism, this code is not clean ANSI */
#ifdef _DEBUG_
#define DEBUG(format, args...) kprintf(format, ##args)
#else 
#define DEBUG(format, args...) ((void)0)
#endif

// The number of thread that will be started
#define THREAD_COUNT 10 
// sleeping factor 
#define N 5

/*
 * argument given to the thread
 *
 */
typedef struct thread_args {
    int ident;                  // theads identity
    int *global_counter;        // global counter 
    pthread_mutex_t *mutex;      // protect access to global_counter
    pthread_barrier_t *barrier;  // Barrier to keep thread order
} thread_args_t;

/*
 * forward declaration of functions
 */
void print_ident(thread_args_t *args);


/*
 *  Main code
 */
int main(int argc, char * argv[]){

    pthread_t        worker[THREAD_COUNT] ;  // threads
    thread_args_t    args[THREAD_COUNT];     // thread arguments
    int i; 
    int counter = 0;                        // global counter 
    long *statusp;                         

    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_barrier_t barrier;
    
    int errVal = pthread_barrier_init(&barrier, NULL, THREAD_COUNT);
    if (errVal != 0) {
        perror("pthread_barrier_init()");
        return -1;
    }

    printf("Mutex program starting. \n");

    /* Set evil seed (initial seed) */
    srand( (unsigned)time( NULL ) );


    // start up threads
    for(i=0;i<THREAD_COUNT;i++){
        /* Set up argument */
        args[i].ident = i  ;
        args[i].global_counter = &counter;
        args[i].mutex = &mutex;
        args[i].barrier = &barrier;

        // set any other argument you want to pass to the newly created thread
        errVal = pthread_mutex_lock(args->mutex);
        pthread_create(&worker[i], NULL, (void *) print_ident, (void *) &args[i]) ;
    }

    /* sleep for 60 secs before cleaning up*/
    sleep(60);

    /*
     * cleaning up after threads
     */
    for(i=0; i < THREAD_COUNT; i++){
        /* just in case the thread returns, make sure we handle it */
        pthread_join(worker[i], (void *) &statusp);
        if (PTHREAD_CANCELED == statusp) {
            printf("thread %d was canceld\n",args[i].ident);
        }else{
            DEBUG("thread %d completed. it's return value isf %ld\n",
               args[i].ident,*statusp);

        }
    }
    
    pthread_barrier_destroy(&barrier);
    /* and we're done */
    return(0);
} 



/**
 * this function is started as a new thread.
 *
 */
void print_ident(thread_args_t *args){
    int s;
    int errVal;

    printf("Hello world, I'm thread %d\n", args->ident);
    errVal = pthread_mutex_unlock(args->mutex);

        int rc = pthread_barrier_wait (args->barrier);
        if (rc != 0 && rc != PTHREAD_BARRIER_SERIAL_THREAD) {
            perror("Could not wait for barrier");
        }
    for(;;){

        /*
         *  sleep for a random time to make thing random. 
         */
        s = 1 + (int) (N * ((double) rand() / (double)(RAND_MAX + 1.0)));
        sleep(s);

        /*
         * up date the global counter and start go back to sleep.
         *
         * you need to add some code here so that threads obay thier 
         * order in the queue.
         *
         */

        while(*args->global_counter % THREAD_COUNT != args->ident){}
        errVal = pthread_mutex_lock(args->mutex);
        if (errVal != 0) {
            break;
        }
        {
            *args->global_counter +=1 ;
            printf("thread %2d  counting  %2d\n",
                   args->ident, *args->global_counter);
        }
        errVal = pthread_mutex_unlock(args->mutex);
        if (errVal != 0) {
            break;
        }
    }
    /* should never happen */
    fprintf(stderr,"I'm returning.. [%d]\n", args->ident);

}
