import matplotlib.pyplot as plt

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
plt.plot(range(1, 17), y[0:16])
plt.plot(range(1, 17), y[16:32])
plt.plot(range(1, 11), y[32:42])
plt.show()
fig.savefig("foo.pdf", bbox_inches='tight')
