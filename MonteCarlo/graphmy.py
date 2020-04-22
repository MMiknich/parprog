import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

data = np.array(pd.read_csv("data.csv", sep=';')['time'])
data += np.array(pd.read_csv("data2.csv", sep=';')['time'])
data += np.array(pd.read_csv("data3.csv", sep=';')['time'])
data += np.array(pd.read_csv("data4.csv", sep=';')['time'])
data += np.array(pd.read_csv("data5.csv", sep=';')['time'])
data /= 5
a = np.ones((data.size,)) * data[0]
data = a/data

plt.plot(np.linspace(1, 16, 16), data, "r-o")
plt.xlabel("Number of threads")
plt.ylabel("Acceleration")
plt.grid()
plt.show()