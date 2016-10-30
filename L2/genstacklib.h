// Definition and API for the generic stack

#include <stdbool.h>

#define GenStackInitialAllocationSize 4

typedef struct{
    void *elems; 
    int elemSize; 
    int logLength; 
    int allocLength;
    void (*freefn)(void*);
}genStack;

void GenStackNew(genStack *s,int elemSize, void (*freefn)(void*));
void GenStackDispose(genStack *s);
bool GenStackEmpty(const genStack *s);
void GenStackPush(genStack *s, const void *elemAddr); 
void GenStackPop(genStack *s, void *elemAddr);
