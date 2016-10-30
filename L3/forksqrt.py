#! /usr/bin/env python3 -tt

import getopt
import sys
import ast
import os
import configparser
import re
import numpy as np
import time

start = ''
loops = ''
tolerance = ''

pread = ['', '', '', '']
cwrite = ['', '', '', '']

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
          '\n-h/--help : help for possible parameters'
          '\n-d/--debug : Debugging Output on the console'
          '\n-i/--input LISTE :'
          'a list with numbers seperated with , : e.g. 3,2.4,1e3,7.1,..'
          '\n-f/--file FILE : read data from file'
          '\n-o/--output FILE : print restult to file'
          '\n-p/--proc : four childprocceses instead of one'
          '\n-s/--size : FRAGMENT_SIZE size of fragments'
          '\n-c/--config FILENAME :'
          'configuration data from DATEINAME (Default: forksqrt.cfg)')


# gets result from chilprocess and returns it
def pipeparent(idx):
    os.close(cwrite[idx])
    pread[idx] = os.fdopen(pread[idx])
    result = pread[idx].read()
    pread[idx].close()
    return result


# calls sqrt2 function with the data to calculate as parameter
# and writes the result in the pipe for the parent process
def pipechild(s, debugbool, idx):
    s = ",".join(s)
    if s is '':
        sys.exit()
    os.close(pread[idx])
    nlist = s.split(',')
    sqrtnum = ''
    for e in nlist:
        try:
            num = ast.literal_eval(e)
        except:
            print("error:", sys.exc_info()[idx])
            usage()
            sys.exit()
        if debugbool is False:
            sqrtnum += (str(sqrt2(num, False)) + ' ')
            sqrtnum += '\n'
        else:
            sqrtnum += (str(sqrt2(num, True)))
    cwrite[idx] = os.fdopen(cwrite[idx], 'w')
    cwrite[idx].write(sqrtnum)
    cwrite[idx].flush()
    #cwrite[idx].close()


# provides up to 4 childprocesses with pipes
# which will deliver the result to parent process
def forkfun(s, num, i, debugbool):
    outputstr = ''
    try:
        if os.fork():
            if num is 4:
                if os.fork():
                    if os.fork():
                        if os.fork():
                            pass
                        else:
                            #child4
                            pipechild(s[i+3], debugbool, 3)
                            sys.exit()
                    else:
                        #child3:
                        pipechild(s[i+2], debugbool, 2)
                        sys.exit()
                else:
                    #child2
                    pipechild(s[i+1], debugbool, 1)
                    sys.exit()
        else:
            #child1
            pipechild(s[i], debugbool, 0)
            sys.exit()
    except:
        sys.exit()
    
    for idx in range (0, num):
        outputstr += pipeparent(idx)
    return outputstr


# devides data in fragments with same size
# it can occur that last fragment has other size due to uneven numbers
def getfragments(data, fs):
    d = []
    st = 0
    ed = fs
    while ed < len(data):
        d.append(data[st:ed])
        st += fs
        ed += fs
    d.append(data[st:len(data)])
    return d


# slection for the options pipesize and fragmentsize
# in which way the datas get executed
def pipeoption(num, strlist, debugbool, outputfile, fragmentsize):
    targetstr = ''
    s = []
    data = strlist.split(',')
    data.pop()
    global pread
    global cwrite
    for i in range(0, num):
        pread[i], cwrite[i] = os.pipe()
    # given fragmentsize
    if (fragmentsize is not ''):
        fs = int(fragmentsize)
        tmps = getfragments(data, fs)
        s = tmps
        for i in range(0, len(tmps), num):
            for j in range(0, num):
                pread[j], cwrite[j] = os.pipe()
            # 1 pipe: puts fragment always as first element of array
            if num is 1:
                s = []
                s.append(tmps[i])
                i = 0 
                targetstr += forkfun(s, num, i, debugbool)
            # 4 pipes
            else:
                targetstr += forkfun(s, num, i, debugbool)

    # no fragmentsize and 4 pipes
    elif (fragmentsize is '') and (num is 4):
        s = np.array_split(data, 4)
        targetstr += forkfun(s, num, 0, debugbool)
    # no fragmentsize and 1 pipe
    elif (fragmentsize is '') and (num is 1):
        s.append(data)
        targetstr = forkfun(s, num, 0, debugbool)
    else:
        usage()
        sys.exit()
    
    if outputfile is not '':
        with open(outputfile, 'w') as f:
            f.write(targetstr)
    else:
        print(targetstr)


# reads parameters of configfile and saves them as global variables
def configutil(name):
    try:
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


# receives the programm parameters and  
# executes function pipeoption with the parameters
def main():
    longopts = ['help', 'debug', 'input=', 'config=',
                'file=', 'output=', 'proc', 'size=']
    fragmentsize = ''
    strlist = ''
    outputfile = ''
    inputbool = False
    debugbool = False
    pipenumbers = 1
    try:
        opts, args = getopt.getopt(sys.argv[1:], 'hdi:c:f:o:ps:', longopts)
    except getopt.GetoptError as err:
        print(err)
        usage()
        sys.exit(2)
    output = None
    for o, a in opts:
        if o in ('-h', '--help'):
            usage()
            sys.exit()
        elif o in ('-d', '--debug'):
            debugbool = True
        elif o in ('-i', '--input'):
            strlist = a + ','
            inputbool = True
        elif o in ('-c', '--config'):
            output = a
            configutil(output)
        elif o in ('-f', '--file'):
            try:
                with open(a, 'r') as f:
                    for line in f:
                        if line.strip():
                            strlist += line.rstrip('\n') + ','
                inputbool = True
            except IOError:
                print('Error: File does not exist!')
                usage()
                sys.exit()
        elif o in ('-o', '--output'):
            outputfile = a
        elif o in ('-p', '--proc'):
            pipenumbers = 4
        elif o in ('-s', '--size'):
            fragmentsize = a
    if inputbool is False:
        usage()
        sys.exit()
    t1 = time.time()
    pipeoption(pipenumbers, strlist, debugbool, outputfile, fragmentsize)
    t2 = time.time()
    extime = t2 - t1
    print('Executiontime: {}\nPipenumbers: {}   Fragmentsize: {}'
          .format(extime, pipenumbers, fragmentsize))

if __name__ == "__main__":
    main()
