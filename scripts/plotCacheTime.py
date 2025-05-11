import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("cacheTime.csv")

plt.plot(df["Cache size"], df["Time"], marker="o")
plt.xlabel("Cache size")
plt.ylabel("Execution time")
plt.title("Cache size vs Execution time")
plt.grid(True)
plt.tight_layout()
plt.show()