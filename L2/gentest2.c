// gentest2.c
// Tests the generic stack with 10 single-precision floating-point values.

#include "genstacklib.h"
#include <stdio.h>

int main(int argc, char *argv[]) {

    const float PI = 3.141592653589f;
    
    float val = 0;

    genStack floatStack;

    GenStackNew(&floatStack, sizeof(float), NULL);
    for (int l = 0; l < 8; l++) {
        val = PI * l;
        GenStackPush(&floatStack, &val);
        printf("%d / %d\n", floatStack.logLength, floatStack.allocLength);
    }

    while(!GenStackEmpty(&floatStack)) {
        GenStackPop(&floatStack, &val);
        printf("Popped: %f\n", val);
    }

    GenStackDispose(&floatStack);
}
