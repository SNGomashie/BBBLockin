import numpy as np
import matplotlib.pyplot as plt
from scipy import signal

# Frequncies and periods
Fs = 80000  # Hz
Ts = 1 / Fs  # s
Fr = 100  # Hz
Tr = 1 / Fr  # s
# Constants
Ar = 1  # V
Ai = 1.8  # V
P = 1000  # Periods
Navr = 1001  # num of samples to be averaged
a = 13.219  # Randomness
w = 0.0001  # Weight of EWMA

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

# Noise generation
np.random.seed(45)
Noise = a * np.random.randn(len(t))

# SNR
npwr = sum(np.power(Noise, 2))
sigpwr = sum(np.power(VsigCos, 2))
SNR = 10 * np.log10((sigpwr) / (npwr))

# Input + Noise
VsigCosandNoise = VsigCos + Noise

nyq = 0.5 * Fs
Fcl = 0.5 / nyq
Fch = 3 / nyq
order = 6
N, Wn = signal.buttord(Fcl, Fch, 0.5, 40)
b, a = signal.butter(N, Wn, btype='low', analog=False)
print(b)
print(a)

i = 0
Qvc, Ivs, R = [0, 0, 0]
for s in VsigCosandNoise:
    # Multiply signal with reference Sin and Cos
    Qvc = w * (s * VrefCos[i]) + (1 - w) * Qvc
    # Qvc -= Qvc / Navr
    # Qvc += (s * VrefCos[i]) / Navr
    Ivs = w * (s * VrefSin[i]) + (1 - w) * Ivs
    # Ivs -= Ivs / Navr
    # Ivs += (s * VrefSin[i]) / Navr
    R = w * (2 * np.sqrt((np.power(Ivs, 2)) + (np.power(Qvc, 2)))) + (1 - w) * R
    # R -= R / Navr
    # R += 2 * np.sqrt((np.power(Ivs, 2)) + (np.power(Qvc, 2))) / Navr
    Ao[i] = R
    i += 1
finalSNR = 10 * np.log10(np.mean(Ao[-1000:]) / Ai)
filtAo = signal.lfilter(b, a, Ao)
w, h = signal.freqz(b, a)
plt.semilogx(w, 20 * np.log10(abs(h)), 'b')
plt.title("Lowpass Filter Frequency Response")
plt.xlabel('Frequency [Hz]')
plt.grid()
plt.show()
# plt.plot(t, VsigCosandNoise, 'b-', label="signal")
print("SNR pre-LIA : %.3f dB" % SNR)
print("SNR post-LIA: %.3f dB" % finalSNR)
print(np.mean(Ao[-1000:]))
plt.title("Magnitude spectrum ")
plt.magnitude_spectrum(VsigCosandNoise, Fs=Fs, color='C2', scale='dB')
plt.magnitude_spectrum(Ao, Fs=Fs, color='C1', scale='dB')
plt.show()

plt.title("Filtered signal ")
plt.plot(t, VsigCosandNoise, 'b-', linewidth=2, label='unfilted signal')
plt.plot(t, Ao, 'g-', linewidth=2, label='filtered signal')
plt.xlabel('Time [sec]')
plt.ylabel('Amlitude [Voltage]')
plt.grid()
plt.ylim(0, 2)
plt.legend()
plt.subplots_adjust(hspace=0.35)
plt.show()
