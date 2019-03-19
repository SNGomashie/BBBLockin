import numpy as np
import matplotlib.pyplot as plt
from scipy import signal

# Frequncies and periods
Fs = 4800  # Hz
Ts = 1 / Fs  # s
Fr = 100  # Hz
Tr = 1 / Fr  # s
# Constants
Ar = 1  # V
Ai = 1.8  # V
P = 1000  # Periods
a = 13.219  # Randomness

# Samples
T = P * Tr  # periods
n = int(T * Fs)
t = np.linspace(0, T, n, endpoint=False)
Ao = np.zeros(len(t))

# Quantizition
quant_bits = 16
quant_levels = np.power(2, quant_bits) / 2
quant_step = 1 / quant_levels

# Experimental signal
VsigCos = Ai * np.cos((t * 2 * np.pi * Fr) - (np.pi / 10))
# Added phase difference

# Reference signal(square wave) ( Also uncomment Ao)
# VrefCos = Ar * signal.square( n * 2 * np.pi * fo)
# VrefSin = Ar * signal.square(( n * 2 * np.pi * fo) - (np.pi/2))

# Reference signals(sinus and cosinus)
VrefCos = Ar * np.cos(t * 2 * np.pi * Fr)
VrefSin = Ar * np.cos((t * 2 * np.pi * Fr) - (np.pi / 2))

# Noise generation
np.random.seed(45)
Expnoise = a * np.random.randn(len(t), 1)

# SNR
npwr = np.sum(np.power(Expnoise, 2))
sigpwr = np.sum(np.power(VsigCos, 2))

# Input + Noise
VsigCosandNoise = VsigCos + Expnoise

nyq = 0.5 * Fs
Fc = 0.01 / nyq
a = 1
b, a = signal.butter(4, 0.0001, btype='low', analog=False)

# Multiply signal with reference Sin and Cos
Vc = VsigCosandNoise * VrefCos
Vs = VsigCosandNoise * VrefSin

# calculate RMS
filtVc = signal.lfilter(b, a, Vs)
filtVs = signal.lfilter(b, a, Vc)
Ivs = np.mean(filtVs)
Qvc = np.mean(filtVc)

# Find magnitude (Square wave)
# Ao[k] = (np.pi/4) * 2 * np.sqrt((np.power(Ivs, 2))+(np.power(Qvc, 2)));

# Find magnitude
Ao = 2 * np.sqrt((np.power(Ivs, 2)) + (np.power(Qvc, 2)))

print("Input amplitude : %.3f V" % Ai)
print("Output amplitude : %.6f V" % Ao)

# Find phase
Phase = np.arctan2(Ivs, Qvc)
print("Phase difference : %.3f rads" % Phase)

SNR = 10 * np.log10((sigpwr) / (npwr))
Aerr = Ao - Ai
print("SNR is : %.3f dB" % SNR)
print("Amplitude error : %.6f V" % Aerr)

plt.plot(n, VsigCos, 'rx')
plt.show()
plt.plot(n, VsigCosandNoise)
plt.show()
plt.plot(n, Vs)
plt.show()
plt.plot(n, Vc)
plt.show()
