// ulstack.c
// Implementation of the unsigned-int-stack and its API

#include "ulstack.h"
#include <stdlib.h>
#include <assert.h>

/*
 * Allocates space for 4 unsigned longs on the stack.
 * The agrument must not be null.
 */
void
ULStackNew (ulstack * s)
{
	assert (s != NULL);

	s->elems = (unsigned long *) malloc (4 * sizeof (unsigned long));
	if (s != NULL)
	{
		s->allocLength = 4;
		s->logLength = 0;
	}
}

/*
 * Frees the allocated space and sets the variables to 0 values.
 * Does nothing if the stack is null.
 */
void
ULStackDispose (ulstack * s)
{
	assert (s != NULL);
	assert (s->elems != NULL);

	free (s->elems);
	s->elems = NULL;
	s->allocLength = 0;
	s->logLength = 0;
}

/*
 * Pushes an unsigned long on the stack and allocates more space if not enough
 * space was allocated.
 * s must not be null; stack must be created prior to a call to this function.
 */
void
ULStackPush (ulstack * s, unsigned long value)
{
	assert (s != NULL);
	assert (s->elems != NULL);

	if (s->allocLength == s->logLength)
	{
		// If max size reached try to allocate more space and check if
		// allocation was successful; if not do nothing
		unsigned long *newSpace =
			realloc (s->elems, s->allocLength * 2);
		if (newSpace != NULL)
		{
			s->elems = newSpace;
			s->allocLength *= 2;
			s->elems[s->logLength++] = value;
		}
	}
	else
	{
		s->elems[s->logLength++] = value;
	}
}

/*
 * Returns the newest element from the stack.
 * Stack must be initialized, space allocated and at least on element on the
 * stack prior to a call to this function.
 */
unsigned long
ULStackPop (ulstack * s)
{
	assert (s != NULL);
	assert (s->elems != NULL);
	assert (s->logLength > 0);

	return s->elems[--s->logLength];
}

/*
 * Returns the number of elements in the stack, or -1 if the stack is NULL.
 */
unsigned int
GetULStackNumberOfElements (ulstack * s)
{
	assert (s != NULL);

	return s->logLength;
}
