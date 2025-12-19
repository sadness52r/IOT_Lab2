import random

n = 100000
filename = "input.txt"

with open(filename, "w") as f:
    f.write(f"{n}\n")
    for _ in range(n):
        m = random.uniform(1e20, 1e25)
        x = random.uniform(-1e11, 1e11)
        y = random.uniform(-1e11, 1e11)
        vx = random.uniform(-1e3, 1e3)
        vy = random.uniform(-1e3, 1e3)
        f.write(f"{m} {x} {y} {vx} {vy}\n")

filename
