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
Navr = 1001  # num of samples to be averaged
n = 13.219  # Randomness
w = 0.001  # Weight of EWMA

# Samples
T = P * Tr  # periods
n = int(T * Fs)
t = np.linspace(0, T, n, endpoint=False)

# Experimental signal
VsigCos = Ai * np.cos((t * 2 * np.pi * Fr) - (np.pi / 10))

# Reference signals(sinus and cosinus)
VrefCos = Ar * np.cos(t * 2 * np.pi * Fr)
VrefSin = Ar * np.cos((t * 2 * np.pi * Fr) - (np.pi / 2))

# Noise generation
np.random.seed(45)
Noise = n * np.random.randn(len(t))

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

Vc = np.zeros(len(t))
Vs = np.zeros(len(t))
Qvc = np.zeros(len(t))
Ivs = np.zeros(len(t))
R = np.zeros(len(t))
Ao = np.zeros(len(t))

i = 0
# Qvc, Ivs, R = [0, 0, 0]
for s in VsigCosandNoise:
    Vc[i] = (s * VrefCos[i])
    Vs[i] = (s * VrefSin[i])
    # Multiply signal with reference Sin and Cos
    # Qvc = w * (s * VrefCos[i]) + (1 - w) * Qvc
    # Ivs = w * (s * VrefSin[i]) + (1 - w) * Ivs
    Qvc[i] = (a[0] * Vc[i]) + (a[1] * Vc[i - 1]) + (a[2] * Vc[i - 2]) + (a[3] * Vc[i - 3]) + (a[4] * Vc[i - 4]) + \
        (b[0] * Qvc[i - 1]) + (b[1] * Qvc[i - 2]) + \
        (b[2] * Qvc[i - 3]) + (b[3] * Qvc[i - 4]) + (b[4] * Qvc[i - 5])
    Ivs[i] = (a[0] * Vs[i]) + (a[1] * Vs[i - 1]) + (a[2] * Vs[i - 2]) + (a[3] * Vs[i - 3]) + (a[4] * Vs[i - 4]) + \
        (b[0] * Ivs[i - 1]) + (b[1] * Ivs[i - 2]) + \
        (b[2] * Ivs[i - 3]) + (b[3] * Ivs[i - 4]) + (b[4] * Ivs[i - 5])

    R[i] = (2 * np.sqrt((np.power(Ivs[i], 2)) + (np.power(Qvc[i], 2))))
    Ao[i] = (a[0] * R[i]) + (a[1] * R[i - 1]) + (a[2] * R[i - 2]) + (a[3] * R[i - 3]) + (a[4] * R[i - 4]) + \
        (b[0] * Ao[i - 1]) + (b[1] * Ao[i - 2]) + \
        (b[2] * Ao[i - 3]) + (b[3] * Ao[i - 4]) + (b[4] * Ao[i - 5])
    # Ao[i] = R
    i += 1

finalSNR = 10 * np.log10(np.mean(Ao[-1000:]) / Ai)
w, h = signal.freqz(b, a, worN=8000)
plt.semilogx(0.5 * Fs * w / np.pi, np.abs(h), 'b')
plt.plot(Fcl, 0.5 * np.sqrt(2), 'ko')
# plt.axvline(Fcl, color='k')
# plt.yscale('log')
# plt.xlim(0, 100)
plt.title("Lowpass Filter Frequency Response")
plt.xlabel('Frequency [Hz]')
plt.grid()
plt.show()
# plt.plot(t, VsigCosandNoise, 'b-', label="signal")
print("SNR pre-LIA : %.3f dB" % SNR)
print("SNR post-LIA: %.3f dB" % finalSNR)
print(np.mean(Ao[-1000:]))
plt.title("Magnitude spectrum ")
plt.magnitude_spectrum(VsigCosandNoise, Fs=Fs, color='C2', label='unfiltered')
plt.magnitude_spectrum(Ao, Fs=Fs, color='C1', label='filtered')
plt.legend()
plt.show()

plt.title("Filtered signal ")
plt.plot(t, VsigCosandNoise, 'b-', linewidth=2, label='unfilted signal')
plt.plot(t, Ao, 'g-', linewidth=2, label='filtered signal')
plt.xlabel('Time [sec]')
plt.ylabel('Amlitude [Voltage]')
plt.grid()
# plt.ylim(0, 2)
plt.legend()
plt.subplots_adjust(hspace=0.35)
plt.show()
