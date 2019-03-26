import numpy as np
import matplotlib.pyplot as plt
from scipy import signal, fftpack

# Frequncies and periods
Fs = 4800  # Hz
Ts = 1 / Fs  # s
Fr = 100  # Hz
Tr = 1 / Fr  # s

# Constants
Ar = 1  # V
Ai = 1.8  # V
P = 1000  # Periods
a = 0  # Randomness

# Samples
T = P * Tr  # periods
n = int(T * Fs)
t = np.linspace(0, T, n, endpoint=False)
Ao = np.zeros(len(t))

# Experimental signal
VsigCos = Ai * np.cos((t * 2 * np.pi * Fr) - (np.pi / 10))

# Reference signals(sinus and cosinus)
VrefCos = Ar * np.cos(t * 2 * np.pi * Fr)
VrefSin = Ar * np.cos((t * 2 * np.pi * Fr) - (np.pi / 2))

# Reference signal(square wave) ( Also uncomment Ao)
# VrefCos = Ar * signal.square(t * 2 * np.pi * Fr)
# VrefSin = Ar * signal.square((t * 2 * np.pi * Fr) - (np.pi/2))

# Noise generation
np.random.seed(45)
Noise = a * np.random.randn(len(t))

# SNR
npwr = sum(np.power(Noise, 2))
sigpwr = sum(np.power(VsigCos, 2))
SNR = 10 * np.log10((sigpwr) / (npwr))

# Multiply signal with reference Sin and Cos
Vc = VsigCos * VrefCos
Vs = VsigCos * VrefSin

# calculate RMS
Ivs = np.mean(Vs)
Qvc = np.mean(Vc)

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

# Find phase
Phase = np.arctan2(Ivs, Qvc)

plt.plot(t, VsigCos, 'r-')
plt.xlim(0, 0.1)
plt.show()

plt.magnitude_spectrum(VsigCos, Fs=Fs, color='C2', scale='dB')
plt.xlim(0, 200)
plt.show()

plt.subplot(2, 1, 1)
plt.magnitude_spectrum(Vc, Fs=Fs, color='C2', scale='dB')
plt.xlim(-10, 210)
plt.subplot(2, 1, 2)
plt.magnitude_spectrum(Vs, Fs=Fs, color='C1', scale='dB')
plt.xlim(-10, 210)
plt.show()
