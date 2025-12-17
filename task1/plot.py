import pandas as pd
import matplotlib.pyplot as plt
import os

df = pd.read_csv("results.csv")

# создаём папку, если вдруг
os.makedirs("graphs", exist_ok=True)

# ВРЕМЯ
plt.figure()
for p in df.points.unique():
    sub = df[df.points == p].sort_values("threads")
    plt.plot(sub.threads, sub.time, marker="o", label=f"points={p}")
plt.title("Execution time")
plt.xlabel("Threads")
plt.ylabel("Time (s)")
plt.grid(True)
plt.legend()
plt.savefig("graphs/time_all.png")
plt.close()

# УСКОРЕНИЕ
plt.figure()
for p in df.points.unique():
    sub = df[df.points == p].sort_values("threads")
    T1 = sub[sub.threads == 1].time.values[0]
    speedup = T1 / sub.time
    plt.plot(sub.threads, speedup, marker="o", label=f"points={p}")
plt.plot(df.threads.unique(), df.threads.unique(), "--", label="Ideal")
plt.title("Speedup")
plt.xlabel("Threads")
plt.ylabel("Speedup")
plt.grid(True)
plt.legend()
plt.savefig("graphs/speedup_all.png")
plt.close()

# ЭФФЕКТИВНОСТЬ
plt.figure()
for p in df.points.unique():
    sub = df[df.points == p].sort_values("threads")
    T1 = sub[sub.threads == 1].time.values[0]
    efficiency = (T1 / sub.time) / sub.threads
    plt.plot(sub.threads, efficiency, marker="o", label=f"points={p}")
plt.title("Efficiency")
plt.xlabel("Threads")
plt.ylabel("Efficiency")
plt.ylim(0, 1.05)
plt.grid(True)
plt.legend()
plt.savefig("graphs/efficiency_all.png")
plt.close()
