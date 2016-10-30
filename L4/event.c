// event.c

#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

static pthread_mutex_t mutex;
static pthread_cond_t cond_consumed = PTHREAD_COND_INITIALIZER;
static pthread_cond_t cond_produced = PTHREAD_COND_INITIALIZER;

/*
 * Global variable that shows if the shared buffer of producer and
 * consumer is empty (MT):
 * 0 is "empty".
 * 1 is "filled".
 */
int MT = 1;

static void *producer( void *arg )
{
        unsigned long count=0;

        while( 1 ) {
                printf("producer: wait...\n");
                pthread_mutex_lock( &mutex ); // enter critical section
                while (MT == 0) {
                    pthread_cond_wait(&cond_consumed, &mutex);
                }
                MT = 0;
                printf("producer: PRODUCE %ld...\n", count++);
                pthread_cond_signal( &cond_produced );
                pthread_mutex_unlock( &mutex ); // leave critical section
        }
        return NULL;
}

static void *consumer( void *arg )
{
        unsigned long count=0; 

        sleep( 1 );
        pthread_cond_signal( &cond_consumed );
        while( 1 ) {
                printf("consumer: wait...\n");
                pthread_mutex_lock( &mutex );
                while (MT == 1) {
                    pthread_cond_wait(&cond_produced, &mutex);
                }
                MT = 1;
                printf("consumer: CONSUME %ld...\n", count++);
                pthread_cond_signal( &cond_consumed );
                pthread_mutex_unlock( &mutex );
        }
        return NULL;
}

int main( int argc, char **argv, char **envp )
{
        pthread_t p1, p2;

        if (pthread_mutex_init( &mutex, NULL )) {
                perror("pthread_mutex_init");
                return -1;
        }

        pthread_create( &p2, NULL, consumer, NULL );
        pthread_create( &p1, NULL, producer, NULL );

        pthread_join( p1, NULL );
        pthread_join( p2, NULL );

        pthread_mutex_destroy( &mutex );
        return 0;
}
