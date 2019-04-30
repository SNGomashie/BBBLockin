import rpyc
import numpy as np
import matplotlib.pyplot as plt

samples = pow(2, 14)
bits = 32

result_bit = bits / 8
bytes = 496 / result_bit

tot = np.ceil(samples / bytes)
sample_frequency = 10000
num = int(tot) << 16
command = num | sample_frequency

c = rpyc.connect("192.168.7.2", 18861)

c.root.pru_initialize(2)

c.root.pru_start(2)

c.root.pru_transmit(command, 0)

data = c.root.pru_blk_receive(samples, bits, 0)

c.root.pru_stop(2)

print(data[:1])

sin_array = np.asarray(data)

sample = len(sin_array)
x = np.arange(sample)

np.savetxt('sampled_data.out', sin_array)

plt.plot(x[:100], sin_array[:100])
plt.show()

plt.title("FFT of SIN sampled by BeagleBone Black 16 bit")
plt.magnitude_spectrum(sin_array[:samples], Fs=10000, scale='dB', color='C2')
plt.xscale("log")
plt.xlim(0, 200)
plt.ylim(-100, 100)
plt.grid()
plt.show()

c.close()
