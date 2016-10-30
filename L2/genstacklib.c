// Implementation of the generic stack

#include "genstacklib.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

void GenStackNew(genStack *s, int elemSize, void (*freefn)(void*)) {
    assert(s != NULL);

    s->elems = malloc(GenStackInitialAllocationSize * elemSize);
    if(s->elems != NULL) {
        s->elemSize = elemSize;
        s->logLength = 0;
        s->allocLength = GenStackInitialAllocationSize;
        s->freefn = freefn;
    } else {
        perror("__LINE__malloc()");
        s->elemSize = 0;
        s->logLength = 0;
        s->allocLength = 0;
        s->freefn = NULL;
    }
}

void GenStackDispose(genStack *s) {
    assert(s != NULL);

    if (s->elems == NULL) {
        return;
    }

    if (s->freefn != NULL) {
        int offset = 0;
        char *newPtr = s->elems;        // Create new pointer for arithmetic
        for (int i = 0; i < s->logLength; i++) {
            s->freefn(newPtr + offset); // Dispose current element
            offset += s->elemSize;
        }
    }

    free(s->elems);                     // Dispose the whole stack
}

bool GenStackEmpty(const genStack *s) {
    assert(s != NULL);

    return s->logLength == 0;
}

void GenStackPush(genStack *s, const void *elemAddr) {
    assert(s != NULL);
    assert(s->elems != NULL);
    
    // Check whether to allocate more space
    if (s->allocLength == s->logLength) {
        // Allocate more space (and copy)
        size_t newSize = s->allocLength * 2;
        void *newSpace = realloc(s->elems, newSize);
        if (newSpace == NULL) {
            perror("__LINE__realloc()");
            return;     // If reallocation failed do nothing
        }
        s->elems = newSpace;
        s->allocLength = newSize;
    }

    // Then make a copy of the pointed thing to the next free space om stack
    char *freeSlotOnStack = s->elems;
    freeSlotOnStack += s->logLength * s->elemSize;
    memcpy((void *) freeSlotOnStack, elemAddr, s->elemSize);
    s->logLength++;
}

void GenStackPop(genStack *s, void *elemAddr) {
    assert(s != NULL);
    assert(s->elems != NULL);
    assert(s->logLength > 0);

    char *elemToPop = s->elems;
    elemToPop += (--s->logLength) * s->elemSize;

    memcpy((void *) elemAddr, elemToPop, s->elemSize);
    
    if (s->freefn != NULL) {
        s->freefn(elemToPop);
    }
}
