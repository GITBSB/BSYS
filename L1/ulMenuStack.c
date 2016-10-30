//ulMenuStack.c

// HELPTEXT

#include "ulstack.h"
#include <stdio.h>

/*
 * Prints the menu and input-line on the terminal.
 */
void
printMenu ()
{
	printf ("\n Enter your choice:\n");
	printf ("1) Create Stack\n");
	printf ("2) Push Value\n");
	printf ("3) Pop Value\n");
	printf ("4) Print Number of Elements on Stack\n");
	printf ("5) Remove Stack\n");
	printf ("6) Exit\n");
	printf ("\t >>");
}

/*
 * Removes all characters (not newlines and end-of-files) from stdin.
 * Use this after a input, that is not a number to remove all unsufficient chars
 * from beeing read individually and each resulting an error message.
 */
void
flushInput ()
{
	char ch;		// placeholder for a char if not a number as input
	while ((ch = getchar ()) != '\n' && ch != EOF);
}


int
main ()
{

	ulstack stack;
	stack.elems = NULL;
	stack.logLength = 0;
	stack.allocLength = 0;

	int choice = 6;		// Entered value from user
	int errScanf = 0;	// Error code from scanf
	unsigned long value = 0;	// placeholder for the value to push or pop

	do
	{
		printMenu ();
		choice = -1;
		errScanf = scanf ("%d", &choice);

		if (errScanf != 1)
		{
			printf ("Inserted not a number or too much arguments! Insert a number from 1 to 6!\n");
			flushInput ();
			continue;
		}

		switch (choice)
		{
		case 1:
			ULStackNew (&stack);
			break;
		case 2:
			printf ("Enter value: ");
			errScanf = scanf ("%lu", &value);
			if (errScanf == 1)
			{
				if (stack.elems == NULL)
				{
					printf ("No stack! Create one before pushing.\n");
				}
				else
				{
					ULStackPush (&stack, value);
				}
			}
			else
			{
				printf ("Input is not a number! Nothing pushed.\n");
				flushInput ();
			}
			break;
		case 3:
			if (stack.elems != NULL && stack.logLength > 0)
			{
				value = ULStackPop (&stack);
				printf ("%lu", value);
			}
			else
			{
				printf ("No elements on the stack!\n");
			}
			break;
		case 4:
			value = GetULStackNumberOfElements (&stack);
			printf ("Elements on Stack: %lu\n", value);
			break;
		case 5:
			if (stack.elems != NULL)
			{
				ULStackDispose (&stack);
			}
			else
			{
				printf ("The stack is already disposed!\n");
			}
			break;
		case 6:
			break;
		default:
			printf ("Wrong Input! Instert a number from 1 to 6.\n");
		}
	}
	while (choice != 6);

	if (stack.elems != NULL)
	{
		ULStackDispose (&stack);
	}

	return 0;
}
