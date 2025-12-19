#!/bin/bash

set -e

# -------------------------------
# Параметры эксперимента
# -------------------------------
EXEC=./a.out
SRC=nbody_omp.c
INPUT=input.txt
TEND=0.01
#RUNS=5

THREADS_LIST="1 2 4 8"

RESULTS_DIR=results
PLOTS_DIR=$RESULTS_DIR/plots

mkdir -p $PLOTS_DIR

# -------------------------------
# Компиляция
# -------------------------------
echo "Компиляция..."
#gcc -O3 -fopenmp $SRC -o nbody -lm
clang -Xpreprocessor -fopenmp \
  -I$(brew --prefix libomp)/include \
  -L$(brew --prefix libomp)/lib -lomp \
  nbody_omp.c

# -------------------------------
# Замеры времени
# -------------------------------
echo "threads,time" > $RESULTS_DIR/times.csv

for t in $THREADS_LIST; do
    echo "Запуск с $t потоками"
    export OMP_NUM_THREADS=$t

    TIME=$($EXEC $t $TEND $INPUT \
           | grep "Среднее время" | awk '{print $4}')

    echo "$t,$TIME" >> $RESULTS_DIR/times.csv
done

# -------------------------------
# Постобработка + графики
# -------------------------------
python3 << EOF
import csv
import matplotlib.pyplot as plt

threads = []
times = []

with open("$RESULTS_DIR/times.csv") as f:
    reader = csv.DictReader(f)
    for row in reader:
        threads.append(int(row["threads"]))
        times.append(float(row["time"]))

t1 = times[0]
speedup = [t1 / t for t in times]
efficiency = [s / p for s, p in zip(speedup, threads)]

# --- сохраняем таблицы ---
with open("$RESULTS_DIR/speedup.csv", "w") as f:
    f.write("threads,speedup\n")
    for p, s in zip(threads, speedup):
        f.write(f"{p},{s}\n")

with open("$RESULTS_DIR/efficiency.csv", "w") as f:
    f.write("threads,efficiency\n")
    for p, e in zip(threads, efficiency):
        f.write(f"{p},{e}\n")

# --- график времени ---
plt.figure()
plt.plot(threads, times, marker="o")
plt.xlabel("Число потоков")
plt.ylabel("Время, с")
plt.title("Время выполнения")
plt.grid()
plt.savefig("$PLOTS_DIR/time.png")

# --- график ускорения ---
plt.figure()
plt.plot(threads, speedup, marker="o")
plt.xlabel("Число потоков")
plt.ylabel("Ускорение")
plt.title("Ускорение")
plt.grid()
plt.savefig("$PLOTS_DIR/speedup.png")

# --- график эффективности ---
plt.figure()
plt.plot(threads, efficiency, marker="o")
plt.xlabel("Число потоков")
plt.ylabel("Эффективность")
plt.title("Эффективность")
plt.grid()
plt.savefig("$PLOTS_DIR/efficiency.png")
EOF

echo "Готово. Графики лежат в $PLOTS_DIR"
