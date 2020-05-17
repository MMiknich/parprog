import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

data = np.array(pd.read_csv("per.csv", sep=';')['time'])
print(data)
a = np.ones((data.size,)) * data[0]
print(a, data)
data = a/data

plt.plot(np.linspace(1, 4, 4), data, "r-o", label='long')

data = np.array(pd.read_csv("ter.csv", sep=';')['time'])

a = np.ones((data.size,)) * data[0]
print(a, data)
data = a/data

plt.plot(np.linspace(1, 16, 16), data, "b-o", label='short')

plt.xlabel("Number of threads")
plt.ylabel("Acceleration")
plt.legend()
plt.grid()
plt.show()