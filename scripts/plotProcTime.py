import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("procTime.csv")

plt.plot(df["Processes"], df["Time"], marker="o")
plt.xlabel("Nº of processes")
plt.ylabel("Execution time")
plt.title("Nº of of processes vs Execution time")
plt.grid(True)
plt.tight_layout()
plt.show()