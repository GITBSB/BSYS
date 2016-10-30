#! /usr/bin/env python3 -tt

import os
import platform
import logging
import subprocess


# returns sorted list for size/resident/share/text
def memory_top(elem):
    logging.info('called memory_top function')
    ms = memstat()
    mlist = []
    if elem == 'size':
        for r in range(len(ms)):
            mlist.append((ms[r][0], round(ms[r][2])))
    if elem == 'resident':
        for r in range(len(ms)):
            mlist.append((ms[r][0], round(ms[r][3])))
    if elem == 'share':
        for r in range(len(ms)):
            mlist.append((ms[r][0], round(ms[r][4])))
    if elem == 'text':
        for r in range(len(ms)):
            mlist.append((ms[r][0], round(ms[r][5])))
    if not mlist:
        assert False, "unhandled option"
    mlist = sorted(mlist, key=lambda tup: tup[1], reverse=True)
    return mlist


# returns a dictionary with the sums of all tasks for
# size, resident, share, text, SUM
def memory_sum():
    logging.info('called memory_sum function')
    ms = memstat()
    dictm = {}
    for r in range(len(ms)):
        dictm['size'] = round(sumCol(ms, 2))
        dictm['resident'] = round(sumCol(ms, 3))
        dictm['share'] = round(sumCol(ms, 4))
        dictm['text'] = round(sumCol(ms, 5))
        dictm['SUM'] = round(sumCol(ms, 6))
    return dictm


# saves runnable tasks in tmp, parses them and generate Matrix for all
# processes with statistics(name,pid,size,resident,shared pages,text)
def memstat():
    tmp = subprocess.check_output('ps xu', shell=True)
    tasks = tmp.decode()
    tasks = tasks.split("\n")
    tasks = tasks[1:len(tasks)-1]
    numproc = len(tasks)
    pstat = [[0 for x in range(7)] for x in range(numproc)]
    i = 0
    for n in range(numproc):
        while '  ' in tasks[i]:
            tasks[i] = tasks[i].replace('  ', ' ')
        t = tasks[i].split(' ')
        pstat[i][0] = ' '.join(t[10:])  # Name
        pstat[i][1] = t[1]      # PID
        if pstat[i][0] in ('ps xu', '/bin/sh -c ps xu'):
            i += 1
            continue
        pidstat = subprocess.check_output('cat /proc/' + pstat[i][1] +
                                          '/statm', shell=True)
        pidstat = pidstat.decode()
        pidstat = pidstat.split(' ')
        pstat[i][2] = int(pidstat[0])/1000  # size in MB
        pstat[i][3] = int(pidstat[1])/1000  # resident in MB
        pstat[i][4] = int(pidstat[2])   # number of shared pages
        pstat[i][5] = int(pidstat[3])   # text (code)
        sumall = pstat[i][2] + pstat[i][3] + pstat[i][4] + pstat[i][5]
        pstat[i][6] = sumall
        i += 1
    return pstat


# sums up all values of a column
def sumCol(m, c):
    totalsum = 0
    for r in range(len(m)):
        totalsum += m[r][c]
    return totalsum


# prints the Python version
def pythoninfo():
    logging.info('called pythoninfo')
    os.system("python --version")
    return True


# prints system informations
def systeminfo():
    logging.info('called systeminfo')
    print('-------------------------------------')
    print('LABEL\t\t\tCONTENT')
    print('Architecture\t\t{0}\nSystem\t\t\t{1}\nLinux Distribution\t{2}'
          .format(platform.architecture()[0], platform.system(),
                  platform.linux_distribution()[0]))
    print('Linux Release\t\t{0}\nKernel Release\t\t{1}'
          .format(platform.linux_distribution()[1], platform.release()))
    print('-------------------------------------')
    return True


# prints cpu informations
# searches in each line of /proc/cpuinfo for:
# modelname, architecture, cpu family, modelname, CPU Freq, cpu codename
def cpuinfo():
    logging.info('called cpuinfo')
    print('-------------------------------------')
    with open('/proc/cpuinfo') as f:
        intfamnum = 0
        intmodelnum = 0
        for line in f:
            if line.strip():
                modelname(line)
                archit(line)
                cpumhz(line)
                cpuname(line)
                famnum = cpufamily(line)
                modelnum = model(line)
                if type(famnum) is int:
                    intfamnum = famnum
                if type(modelnum) is int:
                    intmodelnum = modelnum
                if (intfamnum != 0) and (intmodelnum != 0):
                    codename(intfamnum, intmodelnum)
                    intfamnum = 0
                    intmodelnum = 0
    print('-------------------------------------')
    return True


# prints the codename
# parses proc_code.txt and compares the
# modelnumber and familynumber with those of /proc/cpuinfo
def codename(famnum, modelnum):
    with open("proc_code.txt") as f:
        for line in f:
            if line.strip():
                sline = line.rstrip('\n').split(':')
                fnum = sline[2].replace(" ", "")
                mnum = sline[3].replace(" ", "")
                if (sline[2][1].isdigit()) and (sline[3][1].isdigit()):
                    if (fnum.replace("x0", "x").lower() == str(hex(famnum))) \
                            and (mnum.replace("x0", "x").lower() ==
                                 str(hex(modelnum))):
                        print('Codename:\t' + sline[0])


# prints cpu name
def cpuname(line):
    if line.rstrip('\n').startswith('processor'):
        print('\nProcessor ' + line.rstrip('\n').split(':')[1] + ':')


# prints cpu frequence
def cpumhz(line):
    if line.rstrip('\n').startswith('cpu MHz'):
        cpumhz = line.rstrip('\n').split(':')[1]
        print('Frequency:\t' + cpumhz.lstrip() + 'MHz')


# prints cpu family and returns it
def cpufamily(line):
    if line.rstrip('\n').startswith('cpu family'):
        family = ''
        family += line.rstrip('\n').split(':')[1]
        family = family.replace(" ", "")
        family = int(family)
        print('Family:\t\t{0}'.format(family))
        return family


# prints model and returns it
def model(line):
    if (line.split(None, 1)[0] == 'model') \
            and (len(line.rstrip('\n').split(':')[1]) < 10):
        model = ''
        model += line.rstrip('\n').split(':')[1]
        model = int(model)
        print('Model:\t\t{0}'.format(model))
        return model


# prints modelname
def modelname(line):
    if line.rstrip('\n').startswith('model name'):
        model_name = line.rstrip('\n').split(':')[1]
        print('Modelname:\t' + model_name.lstrip())


# prints architecture
def archit(line):
    if line.rstrip('\n').startswith('flags') \
            or line.rstrip('\n').startswith('Features'):
        if 'lm' in line.rstrip('\n').split():
            print('Architecture:\t64-bit')
        else:
            print('Architecture:\t32-bit')
