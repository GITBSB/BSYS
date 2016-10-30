#! /usr/bin/env python3 -tt

import getopt
import sys
import helper
import logging
import subprocess


logging.basicConfig(filename='mygetopt.log', level=logging.INFO,
                    format='%(asctime)s %(levelname)s %(message)s',
                    datefmt='%m/%d/%Y %I:%M:%S %p')

longopts = ['help', 'version', 'output=', 'python',
            'system', 'cpu', 'memory=']


# prints the usage instruction for the help menu
def usage():
    print('\nHelp:\n-h/--help : help for possible parameters'
          '\n-v/--version : GIT Token of the commit'
          '\n-o/--output FILENAME :'
          'output in a file with the name FILENAME'
          '\n-p/--python : Python Environment function'
          '\n-s/--system : System info'
          '\n-c/--cpu : CPU info'
          '\n-m/--memory all/size/resident/share/text: \n'
          '\tall(1)\t\tentries for all runnable tasks in the system\n'
          '\tsize(2)\t\ttotal program size\n'
          '\tresident(3)\tresident set size\n'
          '\tshare(4)\tshared pages\n\ttext(5)\t\ttext (code)')


# executes functions for the specific programm parameters
def main():
    logging.info('started')
    try:
        opts, args = getopt.getopt(sys.argv[1:], 'hvo:pscm:', longopts)
    except getopt.GetoptError as err:
        logging.warning('wrong call')
        print(err)
        usage()
        sys.exit(2)
    output = None
    for o, a in opts:
        if o in ('-h', '--help'):
            logging.info('help option')
            usage()
            sys.exit()
        elif o in ('-v', '--version'):
            print('Git Token')
            subprocess.call('git rev-parse --short HEAD', shell=True)
        elif o in ('-o', '--output'):
            output = a
            subprocess.call('script -f ' + a, shell=True)
        elif o in('-p', '--python'):
            helper.pythoninfo()
        elif o in('-s', '--system'):
            helper.systeminfo()
        elif o in('-c', '--cpu'):
            helper.cpuinfo()
        elif o in('-m', '--memory'):
            output = a
            namesort = ''
            unit = ''
            ml = []
            if (output == 'all'):
                dm = helper.memory_sum()
                print('-------------------------------------')
                print('size:\t\t{0} MB\nresident:\t{1} MB\n'
                      'share:\t\t{2}\ntext:\t\t{3}\nSUM:\t\t{4}'
                      .format(dm['size'], dm['resident'], dm['share'],
                              dm['text'], dm['SUM']))
                print('-------------------------------------')
                sys.exit()
            elif (output == 'size'):
                ml = helper.memory_top('size')
                namesort = 'size'
                unit = 'MB'
            elif (output == 'resident'):
                ml = helper.memory_top('resident')
                namesort = 'resident'
                unit = 'MB'
            elif (output == 'share'):
                ml = helper.memory_top('share')
                namesort = 'share'
            elif (output == 'text'):
                ml = helper.memory_top('text')
                namesort = 'text'
            else:
                logging.warning('wrong call for -m/--memory')
                usage()
                sys.exit()
            print('-------------------------------------')
            print('Sorted List: ' + namesort)
            for n in ml[:10]:
                print('{:<70}{} {}'.format(n[0][:65], n[1], unit))
            print('-------------------------------------')
        else:
            assert False, "unhandled option"
            logging.warning("call unhandled option")

if __name__ == "__main__":
    main()
