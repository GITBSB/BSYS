// ulstack.h
// Header for the unsigned-int-stack and its API

typedef struct
{
	unsigned long *elems;
	unsigned int logLength;
	unsigned int allocLength;
} ulstack;

void ULStackNew (ulstack * s);
void ULStackDispose (ulstack * s);
void ULStackPush (ulstack * s, unsigned long value);
unsigned long ULStackPop (ulstack * s);
unsigned int GetULStackNumberOfElements (ulstack * s);
