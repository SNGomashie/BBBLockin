import numpy as np
import matplotlib.pyplot as plt


def running_mean(x, N=100):
    cumsum = np.cumsum(np.insert(x, 0, 0))
    return (cumsum[N:] - cumsum[:-N]) / float(N)


# Frequncies and periods
Fs = 300  # Hz
Ts = 1 / Fs  # s
Fr = 100  # Hz
Tr = 1 / Fr  # s

# Constants
Ar = 1  # V
Ai = 1.8  # V
Navr = 1000  # num of samples to be averaged

# Samples
T = 5000 * Tr  # periods
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
Expnoise = 14 * np.random.randn(len(t))

# SNR
npwr = np.sum(np.power(Expnoise, 2))
sigpwr = np.sum(np.power(VsigCos, 2))

# Input + Noise
VsigCosandNoise = VsigCos

i = 0
Qvc, Ivs, R = [0, 0, 0]
for s in VsigCosandNoise:
    # Multiply signal with reference Sin and Cos
    Qvc -= Qvc / Navr
    Qvc += (s * VrefCos[i]) / Navr
    Ivs -= Ivs / Navr
    Ivs += (s * VrefSin[i]) / Navr
    R -= R / Navr
    R += 2 * np.sqrt((np.power(Ivs, 2)) + (np.power(Qvc, 2))) / Navr
    SNR = 10 * np.log10((sigpwr) / (npwr))
    Ao[i] = R
    i += 1
plt.subplot(2, 1, 2)
plt.plot(t, VsigCosandNoise, 'b-', label="signal")
plt.plot(t, Ao, 'g-', linewidth=2, label='filtered signal')
plt.xlabel('Time [sec]')
plt.grid()
plt.ylim(-2, 2)
plt.legend()
plt.subplots_adjust(hspace=0.35)
plt.show()
print(SNR)
