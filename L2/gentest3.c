// gentest3.c
// Tests the stack with 4 Strings (char-array) with different lengths.

#include "genstacklib.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void freefn(void* element) {
    free(element);
}

int main(int argc, char *argv[]) {
    const char *words[] = {"foo", "bar", "peanuts", "I need a Dollar"};
    genStack stringStack;

    GenStackNew (&stringStack, sizeof(char*), freefn);
    for (int i = 0; i < 4; i++) {
        char *copy = strdup(words[i]);
        GenStackPush(&stringStack, &copy);
    }

    char *oneWord;
    for (int i=0; i<3; i++) {
        GenStackPop(&stringStack,&oneWord);
        printf("%s\n",oneWord);
        free(oneWord);
    }
    GenStackDispose(&stringStack);
}
