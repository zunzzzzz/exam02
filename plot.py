import matplotlib.pyplot as plt
import numpy as np
import serial
import time

Fs = 100.0; 
Ts = 1.0 / Fs; 
t = np.arange(0, 1, Ts) 
x = np.zeros(int(Fs)) 
y = np.zeros(int(Fs)) 
z = np.zeros(int(Fs)) 
tilt = np.zeros(int(Fs)) 

serdev = '/dev/ttyACM0'
s = serial.Serial(serdev)
count = 0
for i in range(0, int(Fs)):
    line = s.readline() # Read an echo string from K66F terminated with '\n'
    data = line.split()
    # print(data)
    x[count] = float(data[0])
    y[count] = float(data[1])
    z[count] = float(data[2])
    line = s.readline()
    tilt[count] = int(line)
    count += 1

fig, ax = plt.subplots(2, 1)
# ax[0].plot(t, x, t, y, t, z)
ax[0].plot(t, x, label = 'x')
ax[0].plot(t, y, label = 'y')
ax[0].plot(t, z, label = 'z')
ax[0].set_xlabel('Time')
ax[0].set_ylabel('Acc Vector')
ax[0].legend()
ax[1].stem(t, tilt)
# ax[1].bar(t, tilt, 0.003, color = "blue")
ax[1].set_xlabel('Time')
ax[1].set_ylabel('Flag')

plt.show()
s.close()