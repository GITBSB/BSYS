// work.c
/*
 * Aktives Warten für x Sekunden und y Millisekunden (2 Parameter) ohne
 * Systemfunktionen; es wird die Bibliotheksfunktion ftime() benutzt.
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <sys/timeb.h>


void waitLoop(int iterations) {
	assert(iterations >= 0);
	
	int a = 2;
	for (int i = 0; i < iterations; i++) {
		a *= 2;
	}
}

int main(int argc, char *argv[])
{
    if(argc != 3)
    {
        printf("2 Argumnts required: [seconds] [milliseconds]\n");
        return -1;
    }
    int time_s = atoi(argv[1]);
    int time_ms = atoi(argv[2]);
    int time_to_wait = time_s * 1000 + time_ms;
    printf("Time to wait: %d milliseconds\n", time_to_wait);

    struct timeb start, current;
    ftime(&start);
    int diff = 0;

    while(diff <= time_to_wait)
    {
		int arg =  time_to_wait - diff;
		waitLoop(arg < 0 ? 10000 : arg * 350000);
		
        ftime(&current);
        diff = 1000 * (current.time - start.time) + (current.millitm - start.millitm);
    }
    printf("Time waited: %d milliseconds\n", diff);

    return 0;
}
