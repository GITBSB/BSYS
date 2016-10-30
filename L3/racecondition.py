#! /usr/bin/env python3 -tt

import getopt
import sys
import _thread
import time
import threading
from threading import Thread
import numpy as np


globalvar = 0
threadcount = 0
t_wait_clock = 0.000001
access = []
last = []
lock = _thread.allocate_lock()
lockcount = _thread.allocate_lock()


# prints the usage instruction for the help menu
def usage():
    print('\nHelp:'
          '\n-h/--help : help for possible parameters'
          '\n-l/--loops NUMBER: number of loops, '
          'each loop increments the global variable (Default: 10)'
          '\n-t/--thread NUMBER : number of threads which get created'
          '(Default: 40)'
          '\n-g/--gil : block shared access of the global variable'
          '\n-p/-peterson : peterson algorithm to protect the global var'
          '\n-m/--mutex : mutex to protect the global variable')


# prevent race condition with lock
def race_lock(n_threads, n_loops,):
    threads = []
    for i in range(n_threads):
        threads.append(threading.Thread(target=opt_inc_gv, 
                                        args=(n_loops, True)))
    for i in range(n_threads):
        threads[i].start()
    for i in range(n_threads):
        threads[i].join()
    return [n_threads * n_loops, globalvar]



# forces race condition without locking
def race(n_threads, n_loops):
    threads = []
    for i in range(n_threads):
        threads.append(race_helper(n_loops))
    for i in range(n_threads):
        threads[i].start()
    for i in range(n_threads):
        threads[i].join()
    return [n_threads * n_loops, globalvar]


# class for threading to create race condition without GIL
class race_helper(Thread):
    def __init__(self, n_loops):
        Thread.__init__(self)
        self.n_loops = n_loops
    def run(self):
        opt_inc_gv(self.n_loops, False)


# executes the inc_global function in each loop
def opt_inc_gv(n_loops, l_mutex):
    global threadcount
    for n in range(0, n_loops):
        if l_mutex:
            lock.acquire()
            inc_globalvar()
            lock.release()
        else:
            inc_globalvar()
    with lockcount:
        threadcount -= 1


# increases the global variable with one
# pauses the thread for a short moment to see the race condition
def inc_globalvar():
    global globalvar
    readglobal = globalvar
    time.sleep(t_wait_clock)
    globalvar = readglobal + 1


# locks global variable with a mutex
def race_m(n_threads, n_loops):
    for i in range(0, n_threads):
        _thread.start_new_thread(opt_inc_gv, (n_loops, True))
    exs = n_threads * n_loops
    while threadcount > 0:
        pass
    return [exs, globalvar]


# starts threads and executes the function for the paterson algorithm
def race_p(n_threads, n_loops):
    global access
    global last
    access = np.full(n_threads + 1, -1, dtype=np.int)
    last = np.full(n_threads, -1, dtype=np.int)
    threads = []
    for i in range(n_threads):
        threads.append(threading.Thread(target=petlock, 
                                        args=(n_threads, i, n_loops)))
    for i in range(n_threads):
        threads[i].start()
    for i in range(n_threads):
        threads[i].join()
    return [n_threads * n_loops, globalvar]


# locks global variable after peterson algorithm
def petlock(n_threads, p, n_loops):
    global access
    global last
    level = 1
    while level < n_threads:
        access[p] = level
        last[level] = p

        i = 0
        while i <= n_threads:
            if p is not i:
                while (access[i] >= level) and (last[level] == p):
                    pass
            i += 1
        level += 1
    # enter critical section
    opt_inc_gv(n_loops, False)
    # leave critical section
    access[p] = -1


# receives the programm parameters and
# executes the specific the specific functions
def main():
    longopts = ['threads=', 'loops=', 'help', 'gil',
                'peterson', 'mutex']
    n_threads = 40
    n_loops = 10
    rlock = False
    peterson = False
    mutex = False
    wait_time = 1
    r = ''
    try:
        opts, args = getopt.getopt(sys.argv[1:], 't:l:hgpm', longopts)
    except getopt.GetoptError as err:
        print(err)
        usage()
        sys.exit(2)
    for o, a in opts:
        if o in ('-h', '--help'):
            usage()
            sys.exit()
        elif o in ('-l', '--loops'):
            n_loops = a
        elif o in ('-t', '--threads'):
            n_threads = a
        elif o in ('-g', '--gil'):
            rlock = True
        elif o in ('-p', '--peterson'):
            peterson = True
        elif o in ('-m', '--mutex'):
            mutex = True
    try:

        n_threads = int(n_threads)
        n_loops = int(n_loops)
    except ValueError:
        print('Value Error: numbers expected!')
        usage()
        sys.exit()

    reset_global(n_threads)
    if peterson is True:
        print_result(('\nPeterson algorithm:'),
                     race_p, n_threads, n_loops)
        reset_global(n_threads)
    if mutex is True:
        print_result(('\nMutex:'), race_m, n_threads, n_loops)
        reset_global(n_threads)
    if rlock is True:
        print_result(('\nWith lock:'), race_lock, n_threads, n_loops)
        reset_global(n_threads)
    else:
        print_result(('\nWithout lock:'), race, n_threads, n_loops)


# meassures function execution time and prints the result to the console
def print_result(name, fun, *args):
    s_time = time.time()
    r = fun(*args)
    e_time = time.time() - s_time
    print(name)
    print('Global_var should have become {}'.format(r[0]))
    print('After {} modifications, global_var is {}'.format(r[0], r[1]))
    print('Execution Time: {}'.format(e_time))


# resets the global variable
def reset_global(n_threads):
    global threadcount
    global globalvar
    threadcount = n_threads
    globalvar = 0


if __name__ == "__main__":
    main()
