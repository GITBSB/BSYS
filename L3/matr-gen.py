#!/usr/bin/python3
import sys
import random
import time

minvalue = 0
maxvalue = 25

if len(sys.argv) != 5:
    sys.stdout.write("Usage: ./matr-gen.py #rows #cols #min #max\n")
    sys.stdout.write("#rows and #cols must be positive integers\n")
    sys.exit(1)

rows = int(sys.argv[1])
cols = int(sys.argv[2])
minvalue = float(sys.argv[3])
maxvalue = float(sys.argv[4])

if rows <= 0 or cols <= 0:
    sys.stdout.write("#rows and #cols must be positive integers\n")
    sys.exit(1)

for i in range(rows):
    random.seed(i + time.time())
    for j in range(cols):
        ele = random.uniform(minvalue, maxvalue)
        sys.stdout.write("{0:.5f}".format(ele))
        sys.stdout.write(" ")
    sys.stdout.write("\n")
