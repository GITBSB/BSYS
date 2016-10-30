/*
 * This programm creates a zombie by creating a fork and the child exits
 * immediarely. The parent process calls ps before querying the child
 * pcrocess information => child in zombie state.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(){
    pid_t child_pid = fork();
    if(child_pid == 0)
    {
        _exit(0);
    }
    else if (child_pid == -1)
    {
        perror("fork");
    }
    else
    {
        sleep(2);
        system("/bin/ps -aux | grep 'Z'");
    }
    return 0;
}
