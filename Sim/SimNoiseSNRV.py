import numpy as np
from scipy import signal, fftpack
import matplotlib.pyplot as plt

# Frequncies and periods
Fs = 4800  # Hz
Ts = 1 / Fs  # s
Fr = 100  # Hz
Tr = 1 / Fr  # s

# Constants
Ar = 1  # V
Ai = 1.8  # V
P = 500
Navr = 1000  # num of samples to be averaged

# Samples
T = P * Tr  # periods
n = int(T * Fs)
t = np.linspace(0, T, n, endpoint=False)

# Experimental signal (Sinusoid)
VsigCos = Ai * np.cos((t * 2 * np.pi * Fr) - (np.pi / 10)
                      )  # Added phase difference

# Reference signal(square wave) ( Also uncomment Ao)
# VrefCos = Ar * signal.square( n * 2 * np.pi * fo)
# VrefSin = Ar * signal.square(( n * 2 * np.pi * fo) - (np.pi/2))

# Reference signals(sinus and cosinus)
VrefCos = Ar * np.cos(t * 2 * np.pi * Fr)
# Cosinus phase shifted to create Sinus
VrefSin = Ar * np.cos((t * 2 * np.pi * Fr) - (np.pi / 2))

# Amount of random noise samples to be averaged
kmax = np.arange(0, 100, 1)

# Amount of noise levels
stepn = -1
x = np.logspace(stepn, 2, 100)

# Initialize arrays
Ao = np.zeros(len(kmax))
SNR = np.zeros(len(kmax))
Aerr = np.zeros(len(kmax))
aveSNRsin = np.zeros(len(x))
aveAerrsin = np.zeros(len(x))
aveAo = np.zeros(len(x))
we_did_it = 0
i = 0
for a in x:
    for k in kmax:
        # Noise generation
        np.random.seed(k + 45)
        Expnoise = a * np.random.randn(len(t))

        # SNR
        npwr = np.sum(np.power(Expnoise, 2))
        sigpwr = np.sum(np.power(VsigCos, 2))

        # Input + Noise
        VsigCosandNoise = VsigCos + Expnoise

        # Multiply signal with reference Sin and Cos
        Vc = VsigCosandNoise * VrefCos
        Vs = VsigCosandNoise * VrefSin

        # calculate RMS
        Ivs = np.mean(Vs)
        Qvc = np.mean(Vc)

        # Find magnitude (Square wave)
        # Ao[k] = (np.pi/4) * 2 * np.sqrt((np.power(Ivs, 2))+(np.power(Qvc, 2)));

        # Find magnitude (Sinusoid)
        Ao[k] = 2 * np.sqrt((np.power(Ivs, 2)) + (np.power(Qvc, 2)))

        # Find phase
        Phase = np.arctan2(Ivs, Qvc)

        SNR[k] = 10 * np.log10((sigpwr) / (npwr))
        Aerr[k] = Ao[k] - Ai
        # END

    aveSNRsin[i] = np.mean(SNR)
    aveAerrsin[i] = np.mean(np.power(Aerr, 2))
    aveAo[i] = np.mean(Ao)

    if (aveSNRsin[i] < -20 and we_did_it == 0):
        print(aveSNRsin[i])
        print(a)
        print(i)
        J = len(VsigCosandNoise)
        T = Ts
        x = np.linspace(0.0, J * T, J)
        Aofft = fftpack.fft(VsigCosandNoise)
        xfft = np.linspace(0.0, 1.0 / (2.0 * T), J / 2)
        plt.plot(xfft, 2.0 / J * np.abs(Aofft[:J // 2]))
        plt.xlim(0, 200)
        plt.show()
        we_did_it = 1
    i += 1
    # END

plt.plot(aveSNRsin, aveAo, 'r-')
plt.axhline(y=1.8, linestyle="--")
plt.yscale('log')
plt.grid()
plt.title('Voltage x SNR')
plt.show()
