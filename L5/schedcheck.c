//schedcheck.c

#define _GNU_SOURCE
#include <sched.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <pthread.h>

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>

#include "gitversion.h"


void printHelp();       // Print help text
void printSchedUnix();  // Do schduling queries with POSIX API
void printSchedLinux(); // Do scheduling queries with linux sched API


int main(int argc, char *argv[]) {
    
    int p_flag = 0;
    int l_flag = 0;
    
    char c;
    while( (c = getopt(argc, argv, "hpl")) != -1 )
    {
        switch(c)
        {
        case 'h':
            printHelp();
            return 0;
            break;
        case 'p':
            p_flag = 1;
            break;
        case 'l':
            l_flag = 1;
            break;
        case '?':
            if (isprint(optopt)) {
              fprintf(stderr, "Unknown option `-%c'.\n", optopt);
            } else {
              fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
            }
        default:
            printHelp();
            return 0;
            break;
        }
    }
    
    
    if (l_flag == 1) {
        printSchedLinux();
    } else if (p_flag == 1) {
        printSchedUnix();
    } else {
        printf("No flag set! Doing UNIX query.\n\n");
        printSchedUnix();
    }
    
    return 0;
}

void printHelp() {
    printf("Git revision: %s\n", gitversion);
    printf("This Program determines which scheduling parameters can be altered.\n");
    printf("These are namely the scheduling algorithm and priority.\n");
    printf("\t-h\tPrint tihs Help\n");
    printf("\t-p\tTest with the POSIX (pthread_) Thread functions.\n");
    printf("\t-l\tTest fir Linux systems. The Linux specific functions can be used here.\n");
}


void testSchedPolicyPthread(pthread_attr_t *attribute, const int policy);

void printSchedUnix() {
    // Use unix things to query params
    pthread_attr_t attribute;
    int policy;

    int errNum = pthread_attr_getschedpolicy(&attribute, &policy);
    if (errNum < 0) {
        perror("");
        return;
    }
    
    testSchedPolicyPthread(&attribute, SCHED_FIFO);
    testSchedPolicyPthread(&attribute, SCHED_RR);
    testSchedPolicyPthread(&attribute, SCHED_OTHER);
}

void testSchedPolicyPthread(pthread_attr_t *attribute, const int policy) {
    char *policy_string;
    if (policy == SCHED_FIFO) {
        policy_string = "FIFO";
    } else if (policy == SCHED_RR) {
        policy_string = "RR";
    } else if (policy == SCHED_OTHER) {
        policy_string = "Other";
    } else {
        printf("Unrecognized scheduling policy!\n");
        return;
    }
    printf("Policy to set: %s\n", policy_string);

    // Priority
    int priority;
    int errNum = pthread_attr_getschedpolicy(attribute, &priority);
    printf("Setting prioriry: ");
    if (errNum < 0) {
        printf("FAILED\n");
        perror("pthread_attr_getschedpolicy()");
    } else {
        errNum = pthread_attr_setschedpolicy(attribute, priority);
        if (errNum < 0) {
            printf("FAILED\n");
            perror("pthread_attr_setschedpolicy()");
        } else {
            printf("SUCCESSFUL\n");
        }
    }
    
    // Affinity
    cpu_set_t affinityMask;
    pthread_t current_thread = pthread_self();
    errNum = pthread_getaffinity_np(current_thread, sizeof(cpu_set_t), &affinityMask);
    printf("Setting affinity: ");
    if (errNum < 0) {
        printf("FAILED\n");
        perror("pthread_getaffinity_np()");
    } else {
        errNum = pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &affinityMask);
        if (errNum < 0) {
            printf("FAILED\n");
            perror("pthread_setaffinity_np()");
        } else {
            printf("SUCCESSFUL\n");
        }
    }
    
    // Policy
    errNum = pthread_attr_setschedpolicy(attribute, policy);
    printf("Setting policy to %s: ", policy_string);
    if (errNum != 0) {
        printf("FAILED\n");
        perror("pthread_attr_setschedpolicy()");
    } else {
        printf("SUCCESFULL\n");
    }
    printf("\n");
}

void testSchedPolicyLinux(int policy);  // Test a ceratin policy on linux

void printSchedLinux() {
    #ifdef __linux__
    struct sched_param parameterInitial;
    
    // Query current scheduling policy.
    int errNum = sched_getparam(0, &parameterInitial);
    if (errNum < 0) {
        perror("sched_getparam()");
        return;
    }
    
    testSchedPolicyLinux(SCHED_FIFO);
    testSchedPolicyLinux(SCHED_RR);
    testSchedPolicyLinux(SCHED_OTHER);
    
    #endif
}

void testSchedPolicyLinux(int policy) {
    #ifdef __linux__
    
    char *policy_string;
    if (policy == SCHED_FIFO) {
        policy_string = "FIFO";
    } else if (policy == SCHED_RR) {
        policy_string = "RR";
    } else if (policy == SCHED_OTHER) {
        policy_string = "Other";
    } else {
        printf("Unrecognized scheduling policy!\n");
        return;
    }
    printf("Policy to set: %s\n", policy_string);

    int priorityMin = sched_get_priority_min(policy);
    if (priorityMin == -1) {
        perror("sched_get_priority_min()");
    }
    int priorityMax = sched_get_priority_min(policy);
    if (priorityMax == -1) {
        perror("sched_get_priority_max()");
    }
    printf("Priorities for %s, (min, max):\t(%d, %d)\n",
           policy_string, priorityMin, priorityMax);
    
    // Get current parameter to only change one thing
    struct sched_param param;
    int errNum = sched_getparam(0, &param);
    if (errNum < 0) {
        perror("sched_getparam()");
        return;
    }
    
    // Try to set sched priority
    errno = 0;
    int prio = getpriority(PRIO_PROCESS, 0);
    if (errno != 0) {
        perror("getpriority()");
    } else {
        errNum = setpriority(PRIO_PROCESS, 0, prio);
        printf("Setting priority: ");
        if (errNum < 0) {
            printf("SUCCESSFUL\n");
        } else {
            printf("FAILED\n");
        }
    }
    
    // Try to set affinity
    cpu_set_t affinityMask;
    errNum = sched_getaffinity(0, sizeof(cpu_set_t), &affinityMask);
    if (errNum < 0 && !errno == EPERM) {
        perror("sched_getaffinity()");
        return;
    }
    errNum = sched_setaffinity(0, sizeof(cpu_set_t), &affinityMask);
    if (errNum < 0) {
        if (!errno == EPERM) {
            perror("sched_setaffinity()");
            return;
        } else {
            printf("Setting cpu affinity: FAILED\n");
        }
    }
    printf("Setting cpu affinity: SUCCESSFUL\n");
    
    // try to set policy
    errNum = sched_setscheduler(0, policy, &param);
    printf("Setting scheduling policy to %s:\t", policy_string);
    if (errNum != 0) {
        printf("FAILED\n\n");
    } else {
        printf("SUCCESSFUL\n\n");
    }
    #endif
}
