import matplotlib.pyplot as plt
import numpy as np

 
file = open("temp_data2", "r")
str = file.readline()
xt = []
x = []
y = []
while str:
    xt.append(str.split())
    str = file.readline()
for i in xt:
    x.append(float(i[0]))
    y.append(float(i[1]))
xt = []
file = open("temp_data", "r")
str = file.readline()
i = 0
while str:
    xt.append(str.split())
    str = file.readline()
for j in xt:
    y[i] = (y[i]+float(j[1]))/2
    i += 1
xt = []
file = open("temp_data3", "r")
str = file.readline()
i = 0
while str:
    xt.append(str.split())
    str = file.readline()
for j in xt:
    y[i] = (y[i]+float(j[1]))/2
    i += 1
for i in range(1, 16):
    y[i] = y[0]/y[i]
for i in range(17, 32):
    y[i] = y[16]/y[i]
for i in range(33, len(y)):
    y[i] = y[32]/y[i]
y[0] = 1
y[16] = 1
y[32] = 1
print(len(y))
fig = plt.figure()
ax = fig.add_subplot(1, 1, 1)
ax.set_title('Acceleration')
ax.plot(range(1, 17), y[0:16], label="2k")
ax.plot(range(1, 17), y[16:32], label="10k")
ax.plot(range(1, 11), y[32:42], label="50k")
plt.xlabel("Number of processes")
plt.ylabel("Acceleration")
plt.legend()
plt.show()
fig.savefig("foo.pdf", bbox_inches='tight')

x1 = np.array(list(range(1, 17)))
x2 = np.array(list(range(1,11)))

y1 = np.array(y[0:16])
y2 = np.array(y[16:32])
y3 = np.array(y[32:42])

fig = plt.figure()
ax = fig.add_subplot(1, 1, 1)
ax.set_title('Efficiency')
ax.plot(x1, y1/x1, label="2k")
ax.plot(x1, y2/x1, label="10k")
ax.plot(x2, y3/x2, label="50k")
plt.xlabel("Number of processes")
plt.ylabel("Efficiency")
plt.legend()
plt.show()
fig.savefig("bar.pdf", bbox_inches='tight')