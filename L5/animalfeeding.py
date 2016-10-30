#! /usr/bin/env python3 -tt

import argparse
import threading
import random
from threading import Thread, Lock, Condition
import time
from colorama import Fore, Style
import numpy as np
import sys


parser = argparse.ArgumentParser()
parser.add_argument('--cn', type=int, default=6,
                    help='Number of cats (Default 6)')
parser.add_argument('--dn', type=int, default=4,
                    help='Number of dogs (Default 4)')
parser.add_argument('--mn', type=int, default=2,
                    help='Number of mice (Default 2)')
parser.add_argument('--ct', type=int, default=15,
                    help='time a cat is satisfied (Default 15)')
parser.add_argument('--dt', type=int, default=10,
                    help='time a dog is satisfied (Default 10)')
parser.add_argument('--mt', type=int, default=1,
                    help='time a mouse is satisfied (Default 1)')
parser.add_argument('--ce', type=int, default=5,
                    help='how many times a cat wants to eat (Default 5)')
parser.add_argument('--de', type=int, default=5,
                    help='how many times a dog wants to eat (Default 5)')
parser.add_argument('--me', type=int, default=5,
                    help='how many times a mouse wants to eat (Default 5)')
parser.add_argument('--e', type=int, default=1,
                    help='eating time interval lower boundary (Default 1 or the same as -E if not given)')
parser.add_argument('--E', type=int, default=1,
                    help='eating time interval upper boundary (Default 1 or the same as -e if not given)')
parser.add_argument('--dish', type=int, default=2,
                    help='Number of Food-Dishes (Default 2)')
parser.add_argument('--file', type=argparse.FileType('w+', encoding='UTF-8'), help='file to flush data into')
parser.add_argument('--v', default=False, action='store_true', help='Verbose print statements')
args = parser.parse_args()

if not args.file:
    args.file = sys.stdout

threads = []
barrier = threading.Barrier(args.cn + args.dn + args.mn)
dish_eating = {}
dish_cv = threading.Condition()
num_eating_animals = 0
animals = []
me_cats = []
me_dogs = []
me_mice = []
cats = []
dogs = []
mice = []

cats_mutex = Lock()
dogs_mutex = Lock()
mice_mutex = Lock()
animals_mutex = Lock()
sched_cv = threading.Condition()
sched_starvation = threading.Condition()

flag_finished = 0
selected_animal = ''


class cat():
    def __init__(self, ID, time_eating, time_satisfied, number_eating):
        self.name = 'cat'
        self.ID = str(ID)
        self.time_eating = time_eating
        self.time_satisfied = time_satisfied
        self.number_eating = number_eating
        self.waiting_queue = cats
        self.mutex = cats_mutex
        self.cond = threading.Condition()
        self.color = Fore.RED
        self.me_times = me_cats


class dog():
    def __init__(self, ID, time_eating, time_satisfied, number_eating):
        self.name = 'dog'
        self.ID = str(ID)
        self.time_eating = time_eating
        self.time_satisfied = time_satisfied
        self.number_eating = number_eating
        self.waiting_queue = dogs
        self.mutex = dogs_mutex
        self.cond = threading.Condition()
        self.color = Fore.BLUE
        self.me_times = me_dogs


class mouse():
    def __init__(self, ID, time_eating, time_satisfied, number_eating):
        self.name = 'mouse'
        self.ID = str(ID)
        self.time_eating = time_eating
        self.time_satisfied = time_satisfied
        self.number_eating = number_eating
        self.waiting_queue = mice
        self.mutex = mice_mutex
        self.cond = threading.Condition()
        self.color = Fore.YELLOW
        self.me_times = me_mice


def animal_feeding(a):
    global num_eating_animals
    global animals

    mydish = 0
    if args.v is True:
        print(a.color + 'Created: {}'.format(a.name + a.ID))
    barrier.wait()
    while a.number_eating > 0:
        a.mutex.acquire()
        a.waiting_queue.append(a)
        a.mutex.release()

        animals_mutex.acquire()
        animals.append(a)
        animals_mutex.release()

        a.cond.acquire()
        start_waiting = time.time()
        a.cond.wait()
        a.cond.release()
        result_waiting = time.time() - start_waiting
        if args.v is True:
            print(a.color + '{}: animal continues'.format(a.name+a.ID))

        animals_mutex.acquire()
        animals.remove(a)
        animals_mutex.release()

        a.mutex.acquire()
        a.waiting_queue.remove(a)
        a.me_times.append([int(a.ID), result_waiting])
        a.mutex.release()

        dish_cv.acquire()
        for i in range(args.dish):
            if dish_eating[i] == 'none':
                mydish = i
                break
        dish_eating[mydish] = a.name
        num_eating_animals += 1
        sched_cv.acquire()
        if args.v is True:
            print(a.color + '{}: signal scheduler'.format(a.name+a.ID))
        sched_cv.notify()
        sched_cv.release()
        print_eating(a, 'started', mydish)
        dish_cv.release()

        time.sleep(a.time_eating)

        dish_cv.acquire()
        dish_eating[mydish] = 'none'
        num_eating_animals -= 1
        print_eating(a, 'finished', mydish)
        dish_cv.notify()
        if num_eating_animals == 0:
            sched_starvation.acquire()
            sched_starvation.notify()
            sched_starvation.release()
        dish_cv.release()
        a.number_eating -= 1


def print_eating(a, what, mydish):
    global num_eating_animals
    print(a.color + '{} ['.format(time.strftime("%X")), end="")
    for i in range(args.dish):
        if dish_eating[i] == 'none':
            print(':-:', end="")
        elif dish_eating[i] == 'cat':
            print(':c:', end="")
        elif dish_eating[i] == 'dog':
            print(':d:', end="")
        elif dish_eating[i] == 'mouse':
            print(':m:', end="")
    print('] {} (id {}) {} eating from dish {}'
          .format(a.name, a.ID, what, mydish))


def scheduler():
    global flag_finished
    global num_eating_animals
    animaltype = 'none'
    count = 0

    while flag_finished == 0:
        dish_cv.acquire()
        while num_eating_animals >= args.dish:
            dish_cv.wait()
        n_eating_a = num_eating_animals
        dish_cv.release()
        if n_eating_a == 0:
            animaltype = 'none'

        animals_mutex.acquire()
        if len(animals) > 0:
            if animaltype == 'none':
                count = 0
                a = animals[0]
                animaltype = a.name
                if args.v is True:
                    print(Fore.WHITE + 'Scheduler changed animaltype: {}'
                          .format(animaltype))
                animals_mutex.release()
                notify_animal(a, animaltype)
            elif count > 3:
                print(Fore.CYAN + 'Scheduler starvation border')
                animals_mutex.release()
                animaltype = 'none'
                if num_eating_animals > 0:
                    sched_starvation.acquire()
                    sched_starvation.wait()
                    sched_starvation.release()
            else:
                i = 0
                an = animals[:3]
                animals_mutex.release()
                for a in an:
                    if animaltype == a.name:
                        if i == 0:
                            count = 0
                        else:
                            count += 1
                        notify_animal(a, animaltype)
                        break
                    else:
                        i += 1
        else:
            animals_mutex.release()


def notify_animal(a, animaltype):
    a.cond.acquire()
    if args.v is True:
        print(Fore.WHITE + 'scheduler notify {}'.format(animaltype))
    a.cond.notify()
    a.cond.release()

    sched_cv.acquire()
    sched_cv.wait()
    if args.v is True:
        print(Fore.WHITE + 'scheduler continues')
    sched_cv.release()


def measurement_times(f):
    print()
    for ma in [me_cats, me_dogs, me_mice]:
        if len(ma) > 0:
            if ma == me_cats:
                f.write('\nCats:\n')
            elif ma == me_dogs:
                f.write('\nDogs:\n')
            else:
                f.write('\nMice:\n')
            a = np.array(ma)
            f.write('\tMin: {}\n'.format(a.min(0)[-1]))
            f.write('\tMax: {}\n'.format(a.max(0)[-1]))
            f.write('\tAvg: {}\n'.format(a.mean(0)[-1]))


def main():
    global flag_finished
    for i in (args.cn, args.dn, args.mn, args.ct, args.dt, args.mt,
              args.ce, args.de, args.me, args.e, args.E, args.dish):
        if i < 0:
            print('{} can not be negative'.format(i))
            return
    if args.e > 1 and args.E == 1:
        args.E = args.e
    if args.E > 1 and args.e == 1:
        args.e = args.E

    if args.dish < 1:
        print('The Number of Food-Dishes has to be greater than 0')
        return

    if(args.E < args.e):
        print('The upper-boundary({}) has to be greater'
              'than lower-boundary({})!'.format(args.E, args.e))
        return

    for i in range(args.dish):
        dish_eating[i] = 'none'

    # produce cats
    for i in range(args.cn):
        c = cat(i, random.uniform(args.e, args.E), args.ct, args.ce)
        t = threading.Thread(target=animal_feeding, args=(c,))
        threads.append(t)
        t.start()

    # produce dogs
    for i in range(args.dn):
        d = dog(i, random.uniform(args.e, args.E), args.dt, args.de)
        t = threading.Thread(target=animal_feeding, args=(d,))
        threads.append(t)
        t.start()

    # produce mice
    for i in range(args.mn): 
        m = mouse(i, random.uniform(args.e, args.E), args.mt, args.me)
        t = threading.Thread(target=animal_feeding, args=(m,))
        threads.append(t)
        t.start()

    # scheduler thread
    s_t = threading.Thread(target=scheduler)
    s_t.start()

    # join threads
    for t in threads:
        t.join()

    flag_finished = 1
    s_t.join()
    print(Style.RESET_ALL)
    measurement_times(args.file)

if __name__ == "__main__":
    main()
