// gentest4.c
// Tests the stack with 5 double-values in a array.

#include "genstacklib.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void freefn(void* element) {
    free(element);
}

int main(int argc, char *argv[]) {
    double numbers[] = {3.0, 3.1, 3.15, 3.141, 3.1415};
    genStack doubleStack;

    GenStackNew (&doubleStack, sizeof(double*), freefn);
    for (int i = 0; i < 5; i++) {
        float *copy = malloc(sizeof(double));
        memcpy(copy, &numbers[i], sizeof(double));
        GenStackPush(&doubleStack, &copy);
    }

    double *number;
    for (int i=0; i < 3; i++) {
        GenStackPop(&doubleStack, &number);
        printf("%f\n", *number);
        free(number);
    }
    GenStackDispose(&doubleStack);
}
