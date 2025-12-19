import csv
import matplotlib.pyplot as plt
from collections import defaultdict

data = defaultdict(dict)

with open("results_multi/raw_times.csv") as f:
    reader = csv.DictReader(f)
    for row in reader:
        N = int(row["N"])
        p = int(row["threads"])
        t = float(row["time"])
        data[N][p] = t

threads = sorted(next(iter(data.values())).keys())

# -------- время --------
plt.figure()
for N in sorted(data.keys()):
    times = [data[N][p] for p in threads]
    plt.plot(threads, times, marker="o", label=f"N={N}")

plt.xlabel("Число потоков")
plt.ylabel("Время, с")
plt.title("Время выполнения")
plt.legend()
plt.grid()
plt.savefig("results_multi/plots/time.png")

# -------- ускорение --------
plt.figure()
for N in sorted(data.keys()):
    t1 = data[N][threads[0]]
    speedup = [t1 / data[N][p] for p in threads]
    plt.plot(threads, speedup, marker="o", label=f"N={N}")

plt.xlabel("Число потоков")
plt.ylabel("Ускорение")
plt.title("Ускорение")
plt.legend()
plt.grid()
plt.savefig("results_multi/plots/speedup.png")

# -------- эффективность --------
plt.figure()
for N in sorted(data.keys()):
    t1 = data[N][threads[0]]
    efficiency = [(t1 / data[N][p]) / p for p in threads]
    plt.plot(threads, efficiency, marker="o", label=f"N={N}")

plt.xlabel("Число потоков")
plt.ylabel("Эффективность")
plt.title("Эффективность")
plt.legend()
plt.grid()
plt.savefig("results_multi/plots/efficiency.png")
