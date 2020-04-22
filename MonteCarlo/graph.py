import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

data = np.array(pd.read_csv("datafull.csv", sep=';')['time'])

data2 = np.array(pd.read_csv("data.csv", sep=';')['time'])
data2 += np.array(pd.read_csv("data2.csv", sep=';')['time'])
data2 += np.array(pd.read_csv("data3.csv", sep=';')['time'])
data2 += np.array(pd.read_csv("data4.csv", sep=';')['time'])
data2 += np.array(pd.read_csv("data5.csv", sep=';')['time'])
data2 /= 5

a = np.ones((data.size,)) * data[0]
data = a/data

a = np.ones((data2.size,)) * data2[0]
data2 = a/data2

plt.plot(np.linspace(1, 16, 16), data, "r-o", label="claster")
plt.plot(np.linspace(1, 16, 16), data2, "b-o", label="personal")
plt.xlabel("Number of threads")
plt.ylabel("Acceleration")
plt.legend()
plt.grid()
plt.show()