## C Code files
* genstacklib.h
	* Contains the API for the generic stack with functions to Create, Push, Pop, Dispose andd query the number of elements on the stack.

* genstacklib.c
	* This is the implementation file of the stack. (Re-) allocation of memory, dispose push and pop.

* gentest1.c
	* A test for the generic stack with int s.

* gentest2.c
	* A test for the generic stack with float s.

* gentest3.c
	* A test for the generic stack with char-arrays.

* gentest4.c
	* A test for the generic stack with doube-arrays.

* Makefile
	* The makefile to produce the genstack library and the executables to test it.

## Python Files
* forksqrt.py
	* Calculates square roots of numbers with options of debug and a configuration file.

* forksqrt.cfg
	* Default configuration file with parameters start, loops, tolerance.

* test_forksqrt.py
	* Tests for function sqrt2() of forksqrt.py.

* forksqrtmax.cfg
	* Configuration file for max. precision of decimal places.

* procenv.py
	* PID hierarchy and system, environment variables.
