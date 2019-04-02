import numpy as np
import matplotlib.pyplot as plt
import time
import threading

FRACTION_BITS = 16
FRACTION_DIVISOR = (1 << FRACTION_BITS)


SHIFT_AMOUNT = np.uint32(10)
SHIFT_MASK = np.uint32((1 << SHIFT_AMOUNT) - 1)
CYCLE_TIME = 5  # ns
S_NS = np.uint64(1000000000)

fsample = 1000  # Hz
sample_period = 1 / fsample
fout = np.uint32(1 << 6)  # Hz
period = np.uint32(1 / fout)  # s
fin = 2.5
periodCycle = np.uint64(((1 / fin) * S_NS) / CYCLE_TIME)
periodCyclefix = np.uint64(periodCycle << SHIFT_AMOUNT)
periodNSfix = np.uint64(periodCyclefix * CYCLE_TIME)

print(periodCycle)
print(periodCyclefix >> SHIFT_AMOUNT)
print(periodNSfix >> SHIFT_AMOUNT)

freq = np.uint64(((S_NS << SHIFT_AMOUNT) / periodNSfix) >> SHIFT_AMOUNT)
print(freq)


x = np.uint32(5 << SHIFT_AMOUNT)
y = 2

x /= np.uint32(y)

print("%f" % np.double((x & ((1 << SHIFT_AMOUNT) - 1)) / (1 << SHIFT_AMOUNT)))







print(bin(periodS))
periodS = np.uint64(periodCycle * 5)  # s
print(bin(200000))
print(bin(periodS >> 9))
2000000

period = np.uint32(200000000 << 4)
print(bin(period))

phase_accumilator = 0
freq = np.uint32(1 / period)
print(bin(freq))
tuning_word = (freq*(np.power(2, 12)))/fsample
print(bin(tuning_word))

def DDS(sample_freq, period):

    phase_accumilator += np.uint32(tuning_word << 20)
    threading.Timer((1 / sample_freq), DDS).start()
    return np.uint32(phase_accumilator >> 20)


print(DDS(fsample, period))

P = .25

T = P * t
n = int(T * fs)

t = np.linspace(0, T, n, endpoint=False)
SIN = np.sin(t * 2 * np.pi * f)



plt.plot(t, SIN)
