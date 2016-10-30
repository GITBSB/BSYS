#!/usr/bin/python

import getopt
import sys
import ast
import os
import configparser
import re

longopts = ['help', 'debug', 'input=', 'config=']
start = ''
loops = ''
tolerance = ''


# method of Heron to calculate the square root of any number
def sqrt2(var, debugbool):
    # Default-Values if parameters not defined, e.g.call from other file
    global start
    if start == '':
        start = 1
    global loops
    if loops == '':
        loops = 100
    global tolerance
    if tolerance == '':
        tolerance = '1e-14'
    # get value from tolerance
    t = re.findall('\d+|\D+', str(tolerance))
    t = int(t[-1])

    debugstr = ''
    assert (isinstance(var, (int, float,))) and (var > 0)
    s = start
    if debugbool is False:
        for x in range(0, loops):
            s = 0.5 * (s + (var/s))
        srs = '{:.{}f}'.format(s, t)
        return srs
    else:
        debugstr += 'Testing with var = {:.{}f}\n'.format(var, t)
        prev = var
        for x in range(0, loops):
            s = 0.5 * (s + (var/s))
            if prev == s:
                debugstr += 'After {} iterations, s = {:.{}f}\n\n' \
                            .format(x, s, t)
                return debugstr
            debugstr += 'Before iteration {}, s = {:.{}f}\n' \
                        .format(x, s, t)
            prev = s
            x += 1
        return debugstr


# prints the usage instruction for the help menu
def usage():
    print('\nHelp:'
          '\nParameter -i/--input is minimum and should be first param\n'
          '\n-h/--help : help for possible parameters'
          '\n-d/--debug : Debugging Output on the console'
          '\n-i/--input LISTE :'
          'a list with numbers seperated with , : e.g. 3,2.4,1e3,7.1,..'
          '\n-c/--config FILENAME :'
          'configuration data from DATEINAME (Default: forksqrt.cfg)')


# creates two pipes by witch the parent and child process which
# can bidirectional connect to each other
def pipee(strlist, debugbool):
    cread, pwrite = os.pipe()
    pread, cwrite = os.pipe()
    if os.fork():
        # parent process
        # writes list of numbers as string in pipe and
        # reads and prints the result the child has calculated
        for fd in (cwrite, cread):
            os.close(fd)
        w = os.fdopen(pwrite, 'w')
        w.write(strlist)
        w.close()

        r = os.fdopen(pread)
        result = r.read()
        r.close()
        print(result)
    else:
        # child process
        # gets the numbers from parent and calls the function sqrt2
        # to calculate the square root and writes result in pipe
        for fd in (pread, pwrite):
            os.close(fd)
        r = os.fdopen(cread)
        nlist = r.read()
        r.close()
        nlist = nlist.split(',')
        sqrtnum = ''
        for e in nlist:
            try:
                num = ast.literal_eval(e)
            except:
                print("error:", sys.exc_info()[0])
                usage()
                sys.exit(2)
            if debugbool is False:
                sqrtnum += (str(sqrt2(num, False)) + ' ')
                sqrtnum += '\n'
            else:
                sqrtnum += (str(sqrt2(num, True)))
        w = os.fdopen(cwrite, 'w')
        w.write(sqrtnum)
        w.close()


# reads parameters of configfile and saves them as global variables
def configutil(name):
    try:
        print(name)
        config = configparser.ConfigParser()
        config.read(name)
        confsqrt = config['sqrt2']
        global start
        if 'start' in confsqrt:
            start = int(confsqrt['start'])
        global loops
        if 'loops' in confsqrt:
            loops = int(confsqrt['loops'])
        global tolerance
        if 'tolerance' in confsqrt:
            tolerance = confsqrt['tolerance']
    except KeyError:
        print('File not found: default values activated!\n')
        configutil('forksqrt.cfg')


# receives the programm parameters and executes the specific functions
def main():
    strlist = ''
    inputbool = False
    debugbool = False
    configbool = False
    try:
        opts, args = getopt.getopt(sys.argv[1:], 'hdi:c:', longopts)
    except getopt.GetoptError as err:
        print(err)
        usage()
        sys.exit(2)
    output = None
    for o, a in opts:
        if o in ('-h', '--help'):
            usage()
            sys.exit()
        elif (o in ('-d', '--debug')):
            debugbool = True
        elif o in ('-i', '--input'):
            strlist = a
            inputbool = True
        elif o in ('-c', '--config'):
            output = a
            configutil(output)
            configbool = True
    if inputbool is False:
        usage()
        sys.exit()
    if configbool is False:
        configutil('forksqrt.cfg')
    if debugbool is True:
        pipee(strlist, True)
    else:
        pipee(strlist, False)


if __name__ == "__main__":
    main()
