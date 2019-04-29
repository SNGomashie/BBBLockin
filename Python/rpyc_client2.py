import rpyc
import numpy as np
import matplotlib.pyplot as plt

samples = 4096

c = rpyc.connect("192.168.7.2", 18861)

c.root.pru_initialize(0)

c.root.pru_start()

tot = np.ceil(samples / 248)
c.root.pru_transmit(tot)

data = c.root.pru_blk_receive(samples)

sin_array = np.asarray(data)

sample = len(sin_array)
x = np.arange(sample)

np.savetxt('sampled_data.out', sin_array)

plt.plot(x[:samples], sin_array[:samples])
plt.ylim(0, 65536)
plt.show()

plt.title("FFT of SIN sampled by BeagleBone Black 16 bit")
plt.magnitude_spectrum(sin_array[:samples], Fs=10000, scale='dB', color='C2')
plt.xscale("log")
plt.xlim(0, 5000)
plt.ylim(-100, 100)
plt.grid()
plt.show()

c.close()
