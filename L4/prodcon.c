// prodcon.c
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <assert.h>
#include <pthread.h>
#include <semaphore.h>

#include "consumer.h"
#include "producer.h"
#include "gitversion.h"

#define RESET   "\033[0m"
#define GREEN   "\033[32m"
#define RED     "\033[31m"
#define INT_MAX 2147483647

void freeBuffer(char **buffer, int lines);
char **createBuffer(int numLines, int numColumns);
void printHelp(void);
int parse_value(const char *opt_arg, int default_val);

int main(int argc, char *argv[]){
    char c;
    int v_flag = 0;
    FILE *i_file = NULL;
    FILE *o_file = NULL;
    int lines = 1;
    int columns = 20;
    int consumer = 1;
    int producer = 1;
    int t_value = 0;
    int r_lower = 0;
    int r_limit = 1;
    int a_value = 0;
    int f_lines = 0;
    opterr = 0;

    while( (c = getopt(argc, argv, "vhi:o:L:C:c:p:t:r:R:a:")) != -1 )
    {
        switch(c)
        {
        case 'v':
            v_flag = 1;
            break;
        case 'h':
            printHelp();
            return 0;
            break;
        case 'i':
            i_file = fopen (optarg, "r");
            if (i_file == NULL)
            {
                fprintf (stderr,
                         "%sCan't open the input file!\n%s",
                         RED,
                         RESET);
            } else {
                f_lines = 0;
                ssize_t read;
                char *line = NULL;
                size_t len;
                while ((read = getline (&line, &len, i_file)) != -1)
                {
                    f_lines += 1;
                }
                fclose (i_file);
                i_file = fopen (optarg, "r");
                free(line);
            }
            break;
        case 'o':
            o_file = fopen (optarg, "w");
            if (o_file == NULL)
            {
                fprintf (stderr,
                         "%sCan't open the output file, just writing to standard output\n%s",
                         RED,
                         RESET);
            }
            break;
        case 'L':
            lines = parse_value(optarg, lines);
            break;
        case 'C':
            columns = parse_value(optarg, columns);
            break;
        case 'c':
            consumer = parse_value(optarg, consumer);
            break;
        case 'p':
            producer = parse_value(optarg, producer);
            break;
        case 't':
            t_value = parse_value(optarg, t_value);
            break;
        case 'r':
            r_lower = parse_value(optarg, r_lower);
            break;
        case 'R':
            r_limit = parse_value(optarg, r_limit);
            break;
        case 'a':
            if (optarg != NULL)
            {
               a_value = parse_value(optarg, a_value);
            }
            break;
        case '?':
            if (optopt == 'i' || optopt == 'o' || optopt == 'L' ||
                optopt == 'C' || optopt == 'c' || optopt == 'p' ||
                optopt == 't' || optopt == 'r' || optopt == 'R' ||
                optopt == 'a')
              fprintf (stderr, "%sOption -%c requires an argument.\n%s",
                       RED, optopt, RESET);
            else if (isprint (optopt))
              fprintf (stderr, "%sUnknown option `-%c'.\n%s",
                       RED, optopt, RESET);
            else
              fprintf (stderr,
                       "%sUnknown option character `\\x%x'.\n%s",
                       RED, optopt, RESET);
        default:
            printHelp();
            return 0;
            break;
        }
    }
    if (r_limit < r_lower)
    {
        fprintf (stderr,
                 "%sUpper is smaller than lower limit!\n%s",
                 RED, RESET);
        printHelp();
        exit(EXIT_FAILURE);
    }
    
    if (lines == 0)
    {
        lines = (producer > consumer) ? producer : consumer;
    }
    
    if (i_file == NULL)
    {
        f_lines = INT_MAX;
        i_file = stdin;
        while (EOF != (c = fgetc(i_file))) {}
    }
    
    if(v_flag == 1 && i_file != NULL){
        printf("\n Input file has %d lines\n", f_lines);
    }
    
    volatile int elems_in_buffer = 0;
    volatile int termin_reading = 0;
    
    static pthread_mutex_t mutex_buffer = PTHREAD_MUTEX_INITIALIZER;
    static pthread_mutex_t mutex_outFile = PTHREAD_MUTEX_INITIALIZER;
    sem_t sem_read;

    /* Generate random number generator for different sleep times */
    srandom( random() );

    // Init Semaphores
    sem_init(&sem_read, 0, 1);


    // Create Buffer
    char **buffer = createBuffer(lines, columns);
    if(buffer == NULL){
        fprintf(stderr, "%sError allocating memory for the buffer!\n%s",
                RED, RESET);
        exit(EXIT_FAILURE);
    }
    int *assignment = calloc(lines, sizeof(int));
    if(assignment == NULL){
        freeBuffer(buffer, lines);
        fprintf(stderr, "%sError allocating memory for the buffer!\n%s",
                RED, RESET);
        exit(EXIT_FAILURE);
    }

    // Create Threads
    pthread_t producers[producer];
    pthread_t consumers[consumer];

    pthread_cond_t buffer_notempty;
    pthread_cond_init(&buffer_notempty, NULL);
    
    pthread_cond_t buffer_empty;
    pthread_cond_init(&buffer_empty, NULL);
    
    // Producer
    producer_arg prod_arg[producer];
    int count_line = 0;
    
    int linesPerProducer = f_lines / producer;
    int remainingLines = f_lines;
    
    for (int i = 0; i < producer; i++)
    {
        prod_arg[i].thread_num = i;
        prod_arg[i].thread_lines = remainingLines < linesPerProducer ?
                                   remainingLines : linesPerProducer;
        prod_arg[i].inFile = i_file;
        prod_arg[i].bufferLines = lines;
        prod_arg[i].bufferColumns = columns;
        prod_arg[i].linesInBuffer = &elems_in_buffer;
        prod_arg[i].buffer = buffer;
        prod_arg[i].mutex_buffer = &mutex_buffer;
        prod_arg[i].buffer_notempty = &buffer_notempty;
        prod_arg[i].buffer_empty = &buffer_empty;
        prod_arg[i].sem_read = &sem_read;
        prod_arg[i].timeToSleep = t_value;
        prod_arg[i].v_flag = v_flag;
        prod_arg[i].count_line = count_line;
        prod_arg[i].assignment = assignment;
        

        pthread_create(&producers[i], NULL, produce,
                       (void *) &prod_arg[i]);
        count_line += linesPerProducer;
        remainingLines -= linesPerProducer;
    }

    // Consumer
    consumer_arg cons_arg[consumer];
    for (int i = 0; i < consumer; i++) {
        cons_arg[i].thread_num = i;
        cons_arg[i].outFile = o_file;
        cons_arg[i].mutex_outFile = &mutex_outFile;
        cons_arg[i].buffer_notempty = &buffer_notempty;
        cons_arg[i].buffer_empty = &buffer_empty;

        cons_arg[i].bufferLines = lines;
        cons_arg[i].bufferColumns = columns;
        cons_arg[i].linesInBuffer = &elems_in_buffer;
        cons_arg[i].buffer = buffer;
        
        cons_arg[i].assignment = assignment;
        cons_arg[i].termin_reading = &termin_reading;

        cons_arg[i].mutex_buffer = &mutex_buffer;
        cons_arg[i].randLow = r_lower;
        cons_arg[i].randHigh = r_limit;
        cons_arg[i].v_flag = v_flag;
        cons_arg[i].w_flag = a_value;

        pthread_create(&consumers[i], NULL,
                       consume, (void *) &cons_arg[i]);
    }
    // Join Threads
    for(int l = 0; l < producer; l++)
    {
        pthread_join(producers[l], NULL);
        printf("P%d finished\n", l);
    }

    pthread_mutex_lock(&mutex_buffer);
    termin_reading = 1;
    pthread_mutex_unlock(&mutex_buffer);

    for(int l = 0; l < consumer; l++){
        pthread_join(consumers[l], NULL);
        printf("C%d finished\n", l);
    }

    // Free mutex and semaphores
    pthread_mutex_destroy(&mutex_buffer);
    pthread_mutex_destroy(&mutex_outFile);
    sem_destroy(&sem_read);
    // Close files
    if (i_file != NULL) {
        fclose(i_file);
    }
    if (o_file != NULL) {
        fclose(o_file);
    }
    // Free memory
    freeBuffer(buffer, lines);
    free(assignment);
}


char **createBuffer(int numLines, int numColumns) {
    assert(numLines > 0);
    assert(numColumns > 0);
    
    char **buffer = (char**) malloc(sizeof(char *) * numLines);
    if(buffer == NULL){
        return NULL;
    }
    for(int l = 0; l < numLines; l++){
        buffer[l] = malloc(sizeof(char) * numColumns);
        if (buffer[l] == NULL){
            freeBuffer(buffer, l);
            return NULL;
        }
    }
    
    return buffer;
}

void freeBuffer(char **buffer, int lines) {
    for(int l = 0; l < lines; l++){
        free(buffer[l]);
    }
    free(buffer);
}

void printHelp(void) {
    printf("%s\nGit commit: %s\n", GREEN, gitversion);
    printf("Options:\n");
    printf("\t-v Verbose\n");
    printf("\t-h Print this help\n");
    printf("\t-i Input-Datei\n");
    printf("\t-o Output-Datei\n");
    printf("\t-L NR Anzahl der Zeilen des Buffers\n");
    printf("\t-C NR Laenge der Zeilen\n");
    printf("\t-c NR Anzahl der Consumer-Threads\n");
    printf("\t-p NR Anzahl der Producer-Threads\n");
    printf("\t-t NR Zeit des passiven Wartens der Producer in ms\n");
    printf("\t-r NR Untere random Zahl Grenze\n");
    printf("\t-R NR Obere random Zahl Grenze\n");
    printf("\t-a NR Offset fuer den Bereich zum Warten\n%s", RESET);
}

// parses the arguments which were given
int parse_value(const char *opt_arg, int default_val)
{
    int val = 0;
    char *endptr = NULL;
    val = (int)strtol(optarg, &endptr,10);
    if (val == 0 || (*endptr != '\0'))
    {
        fprintf(stderr, 
                "%sInvalid argument: %s\nDefault Value: %d\n%s",
                RED,
                opt_arg,
                default_val,
                RESET);
        return default_val;
    } else {
        return val;
    }
}
