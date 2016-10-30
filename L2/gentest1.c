// gentest1.c
// Tests the generic stack with 6 integer values.

#include "genstacklib.h"
#include <stdio.h>

int main(int argc, char *argv[]) {

    int val;
    genStack IntegerStack;
    IntegerStack.elems = NULL;

    GenStackNew(&IntegerStack, sizeof(int), NULL);
    for (val = 0; val < 16; val++) {
        GenStackPush(&IntegerStack, &val);
        printf("%d / %d\n", IntegerStack.logLength, IntegerStack.allocLength);
    }

    while(!GenStackEmpty(&IntegerStack)) {
        GenStackPop(&IntegerStack, &val);
        printf("Popped: %d\n", val);
    }

    GenStackDispose(&IntegerStack);
}
