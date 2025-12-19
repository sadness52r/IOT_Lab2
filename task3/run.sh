#!/bin/bash

NUM_KEYS=100
TOTAL_OPS=50000
RUNS=5
THREADS=4


PROFILES=(
  "0.9 0.1 read-heavy"
  "0.2 0.8 write-heavy"
)

echo "Compiling..."
g++ -O2 -pthread Main.cpp my_rand.c -o main || exit 1
g++ -O2 -pthread pth_ll_rwl.cpp my_rand.c -o pth_ll_rwl || exit 1
echo "Compilation done."
echo ""

echo "\"Test\" \"Time_ms\"" > timings.dat

for profile in "${PROFILES[@]}"; do
  read SEARCH INSERT LABEL <<< "$profile"

  echo "=== Profile: $LABEL ==="

  sum_main=0
  sum_pth=0

  for ((i=1;i<=RUNS;i++)); do
    input=$(printf "%d\n%d\n%f\n%f\n" \
      $NUM_KEYS $TOTAL_OPS $SEARCH $INSERT)

    echo "Run $i..."

    t_main=$(printf "%s" "$input" | ./main $THREADS \
      | grep "Elapsed time" | awk '{print $4}')

    t_pth=$(printf "%s" "$input" | ./pth_ll_rwl $THREADS \
      | grep "Elapsed time" | awk '{print $4}')

    echo "  main=$t_main ms, pthread=$t_pth ms"

    sum_main=$((sum_main + t_main))
    sum_pth=$((sum_pth + t_pth))
  done

  avg_main=$((sum_main / RUNS))
  avg_pth=$((sum_pth / RUNS))

  echo "Average main: $avg_main ms"
  echo "Average pthread: $avg_pth ms"
  echo ""

  echo "\"$LABEL (custom rwlock)\" $avg_main" >> timings.dat
  echo "\"$LABEL (pthread rwlock)\" $avg_pth" >> timings.dat
done

gnuplot <<EOF
set terminal png size 900,600
set output 'comparison.png'
set title "RWLock Performance Comparison"
set ylabel "Average time (ms)"
set style data histograms
set style fill solid 0.7 border -1
set boxwidth 0.8
set xtics rotate by -30
set yrange [0:*]
plot 'timings.dat' using 2:xtic(1) title 'Average elapsed time'
EOF

echo "Graph saved as comparison.png"
