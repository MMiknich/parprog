import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

data = np.array(pd.read_csv("out_file.csv", sep=';')['time'])

a = np.ones((data.size,)) * data[0]
print(a, data)
data = a/data

plt.plot(np.linspace(1, 16, 16), data, "r-o")
plt.xlabel("Number of threads")
plt.ylabel("Acceleration")
plt.grid()
plt.show()