#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    int status;
    /*
     * file descriptor 0 Standard input and 1 Standard output
     */
    //int fd[2] =  {0, 1};

    if(argc < 2)
    {
        perror("Too less Arguments\nEND");
        perror("myexec <programname> <optional Arguments>\nENDE");
        exit(1);
    }

    // Dir and file name to the program to execute
    char *filename = (char*)malloc(sizeof(char)*50);
    strcpy(filename, "/bin/");
    strcat(filename, argv[1]);
    printf("%s\n", filename);

    // Allocate memory for arguments and copy
    char **charArgc = (char**)calloc(argc, sizeof(char*));
    for(int i = 0; i < argc - 1; i++)
    {
        charArgc[i] = (char*) calloc(60, sizeof(char));
        strcpy(charArgc[i], argv[i + 1]);
    }
    charArgc[argc - 1] = NULL;  // Last argument mus ne NULL

    pid_t childpid = fork();
    if(childpid == 0)
    {
        return execv(filename, charArgc);
    }
    else if (childpid == (-1))
    {
        perror("fork");
        return -1;
    } else {
        wait(&status);
        if(WIFEXITED(status)) {
            printf("Exec terminated normally with: %d\n", WEXITSTATUS(status));
            if (WEXITSTATUS(status) != 0) {
                printf("Error: File or directory not found\n");
            }
        } else {
            printf("Exec did not terminate normally with exit\n");
        }
    }

    return 0;
}
