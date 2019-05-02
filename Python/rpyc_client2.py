import rpyc
import numpy as np
import matplotlib.pyplot as plt


def twos_comp(val, bits):
    """compute the int value of a 2 complement number"""
    if (val & (1 << (bits - 1))) != 0: # if sign bit is set e.g., 8bit: 128-255
        val = val - (1 << bits)        # compute negative value
    return val                         # return positive value as is


samples = pow(2, 15)  # Amount of samples wanted
bits = 16  # Amount of bits per sample
sample_frequency = 10000  # Sample frequency in Hz

temp_size = bits / 8  # Calculate the amount of samples in 1 package
bytes = 496 / temp_size

tot = np.ceil(samples / bytes)  # Total number of packets to be received
num = int(tot) << 16
command = num | sample_frequency  # Create command for the PRU with the number of packets and sample frequency

c = rpyc.connect("192.168.7.2", 18861)  # Connect to RpyC server

c.root.pru_initialize(2)  # Create objects for both PRUs

c.root.pru_start(2)  # Start both PRUs

c.root.pru_transmit(command, 0)  # Transmit the 'command' to PRU0

data = c.root.pru_blk_receive(samples, bits, 0)  # Receive a number of samples

sin_array = np.asarray(data)  # Cast in numpy array

y = np.zeros(len(sin_array))  # Create array for translation 2 comp -> int

k = 0  # Initialize number

for i in sin_array:
    y[k] = twos_comp(int(i), 16)  # Translate all the numbers from 2 comp -> int
    k += 1

sample = len(sin_array)
x = np.arange(sample)

np.savetxt('sampled_data.out', sin_array)

plt.plot(x[:samples], y[:samples])
# plt.xlim(0, 100)
plt.show()

plt.title("FFT of SIN sampled by BeagleBone Black 16 bit")
plt.magnitude_spectrum(sin_array[:samples], Fs=sample_frequency, scale='dB', color='C2')
# plt.xscale("log")
plt.xlim(0, 500)
plt.ylim(-100, 100)
plt.grid()
plt.show()

c.close()
