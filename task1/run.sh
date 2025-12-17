#!/bin/bash

mkdir -p graphs  
THREADS="1 2 4 8"
POINTS="10000 50000 100000 500000"
OUT=results.csv

echo "points,threads,time" > $OUT

for p in $POINTS
do
    for t in $THREADS
    do
        echo "points=$p threads=$t"
        time=$(./a.out $t $p)
        echo "$p,$t,$time" >> $OUT
    done
done
