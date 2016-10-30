#!/bin/bash

# Make sure to overwrite old values
rm rand_t.dat

# Build work from source to make sure it is available and not throw error messages
make work

# Determine 10 Values between 1 and 2 seconds
times=1
timems=300

for i in {1..10}
do
    /usr/bin/time -f "%e" -o timeout.dat ./work $times $timems
    realtime=`cat timeout.dat`
    realtime=`echo "$realtime * 1000" | bc -l`
    echo "$times$timems $realtime" >> rand_t.dat
    timems=`expr $timems + 10`
done


# Determine 10 Values between 2 and 3 seconds
times=2
timems=700

for i in {1..10}
do
    /usr/bin/time -f "%e" -o timeout.dat ./work $times $timems
    realtime=`cat timeout.dat`
    realtime=`echo "$realtime * 1000" | bc -l`
    echo "$times$timems $realtime" >> rand_t.dat
    timems=`expr $timems + 10`
done

rm timeout.dat
