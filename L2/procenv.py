#! usr/bin/python

import getopt
import sys
import os
import subprocess
from prettytable import PrettyTable


# dictionary with informations about current working directory,
# UID und GID andenvironment variables:
# User, Home, PWD, Path, Shell, Virtual-Environment (Python)
def get_env():
    dic = {'CWD': os.getcwd(),
           'HOME': os.getenv("HOME"),
           'PWD': os.environ['PWD'],
           'Path': os.environ['PATH'],
           'Shell': os.environ['SHELL'],
           'User': os.environ['LOGNAME'],
           'UID': os.getuid(),
           'GID': os.getgid(),
           'VEnv': os.environ['VIRTUAL_ENV']}
    return dic


# returns dictionary in which Key is process after hierarchy and Value
# is the triple (pid, name, status). Starting with running process
def get_tree():
    d = {}
    pid = os.getpid()
    i = 0
    while(True):
        d[i] = [pid, nameofpid(pid), stateofproc(pid)]
        i += 1
        if(str(pid).strip() == '1'):
            return d
        pid = getppid(pid)


# prints the usage instruction for the help menu
def usage():
    print('\nHelp:'
          '\n-h/--help: help for possible parameters'
          '\n-e/--env: current working directory, UID und GID, '
          '\n\t  environment variables: User, Home, PWD, Path, '
          '\n\t\t\t\t Shell, Virtual-Environment (Python) '
          '\n-p/--pidtree:  PID hierarchy with name and status')


# receives the programm parameters and executes the specific functions
def main():
    try:
        opts, args = getopt.getopt(sys.argv[1:], 'hep',
                                   ['env', 'pidtree', 'help'])
    except getopt.GetoptError as err:
        print(err)
        usage()
        sys.exit(2)
    for o, a in opts:
        if o in ('-h', '--help'):
            usage()
            sys.exit()
        elif o in ('-e', '--env'):
            envdic = get_env()
            print()
            for key, value in envdic.items():
                print('{}: {}'.format(key, value))
            print()
        elif o in ('-p', '--pidtree'):
            printpt()


# prints pretty table
def printpt():
    pidtree = get_tree()
    pt = PrettyTable(["PID", "Name", "Status"])
    pt.align["PID"] = "l"
    pt.padding_width = 1
    for key, value in pidtree.items():
        pt.add_row(value)
    print(pt)


# get parent of pid
def getppid(pid):
    pid = os.popen('ps -o ppid= -p {}'.format(pid))
    pid = pid.read().replace('\n', '')
    return pid


# get name of pid
def nameofpid(pid):
    pid = os.popen('ps -p {} -o comm='.format(pid))
    pid = pid.read().replace('\n', '')
    return pid


# get state of pid
def stateofproc(pid):
    for line in open('/proc/' + str(pid).strip() + '/status').readlines():
        if line.startswith("State:"):
            return line.split(":", 1)[1].strip().split(' ')[0]


if __name__ == "__main__":
    main()
