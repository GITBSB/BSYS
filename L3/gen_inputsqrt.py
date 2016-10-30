#! /usr/bin/env python3 -tt

import sys
import random
import getopt


# prints the usage instruction for the help menu
def usage():
    print('\nHelp:'
          '\n-h/--help : help for possible parameters'
          '\n-n/--number : number of the amount of floating points (int)'
          '\n-o/--output FILE : print restult to file')


# prints a specified number of random floating point numbers
# in an output file
def main():
    longopts = ['help','number=', 'output=']
    num = ''
    opfile = ''
    try:
        opts, args = getopt.getopt(sys.argv[1:],'hn:o:', longopts)
    except getopt.GetoptError as err:
        print(err)
        usage()
        sys.exit()
    for o, a in opts:
        if o in ('-h', '--help'):
            usage()
            sys.exit()
        elif o in ('-n', '--number'):
            num = a
        elif o in ('-o', '--output'):
            opfile = a

    if (num is not '') and (opfile is not ''):
        try:
            num = int(num)
        except ValueError:
            print('Error: The given number is not int!')
            usage()
            sys.exit()
        f = open(opfile, 'w')
        for n in range(0, num):
            f.write(str(random.uniform(0, 1000)) + '\n') #9e+307
        f.close()
    else:
        usage()
        sys.exit()


if __name__ == "__main__":
    main()
