#!/bin/bash
set -e

SRC=nbody_omp.c
EXEC=./a.out
INPUT=input.txt
TEND=100
RUNS=3

N_LIST="1000 2000 5000 10000"
THREADS_LIST="1 2 4 8"

RESULTS=results_multi
PLOTS=$RESULTS/plots

mkdir -p $PLOTS

echo "N,threads,time" > $RESULTS/raw_times.csv

# компиляция
clang -Xpreprocessor -fopenmp \
  -I$(brew --prefix libomp)/include \
  -L$(brew --prefix libomp)/lib -lomp \
  $SRC

for N in $N_LIST; do
    echo "=============================="
    echo "N = $N"

    for P in $THREADS_LIST; do
        echo "  Потоки = $P"

        python3 genInput.py $N $INPUT
        export OMP_NUM_THREADS=$P

        TIME=$($EXEC $P $TEND $INPUT \
               | grep "Среднее время" | awk '{print $4}')

        echo "$N,$P,$TIME" >> $RESULTS/raw_times.csv
    done
done
