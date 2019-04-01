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
maxErr = Ai * 1.001
P = 1000

# Samples
T = P * Tr  # periods
n = int(T * Fs)
t = np.linspace(0, T, n, endpoint=False)

# Experimental signal (Sinusoid)
VsigCos = Ai * np.cos((t * 2 * np.pi * Fr) - (np.pi / 10))  # Added phase difference

# Reference signal(square wave) ( Also uncomment Ao)
# VrefCos = Ar * signal.square(t * 2 * np.pi * Fr)
# VrefSin = Ar * signal.square((t * 2 * np.pi * Fr) - (np.pi/2))

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

i = 0
# Loop thourgh different noise levels
for a in x:
    # Loop through 100 simulations for each noise level
    for k in kmax:
        # Noise generation
        np.random.seed(k + 45)
        Expnoise = a * np.random.randn(len(t))

        # SNR
        npwr = np.sum(np.power(Expnoise, 2))
        sigpwr = np.sum(np.power(VsigCos, 2))
        SNR[k] = 10 * np.log10((sigpwr) / (npwr))

        # Input + Noise
        VsigCosandNoise = VsigCos + Expnoise

        # Multiply signal with reference Sin and Cos
        Vc = VsigCosandNoise * VrefCos
        Vs = VsigCosandNoise * VrefSin

        # calculate RMS
        Ivs = np.mean(Vs)
        Qvc = np.mean(Vc)

        # Find magnitude (Square wave)
        # Ao[k] = (np.pi/4) * 2 * np.sqrt((np.power(Ivs, 2))+(np.power(Qvc, 2)))

        # Find magnitude (Sinusoid)
        Ao[k] = 2 * np.sqrt((np.power(Ivs, 2)) + (np.power(Qvc, 2)))

        # Find phase
        Phase = np.arctan2(Ivs, Qvc)

        # Find amount of error
        Aerr[k] = Ao[k] - Ai
        # END

    # Take the average of the 100 simulations per noise level
    aveSNRsin[i] = np.mean(SNR)
    aveAerrsin[i] = np.mean(Aerr)
    aveAo[i] = np.mean(Ao)

    # When the error is over 0.001% break the loop and save the SNR and output
    if aveAo[i] > maxErr:
        maxSNR = aveSNRsin[i]
        maxAo = aveAo[i]
        finSNR = 10 * np.log10((Ai) / (maxAo))
        break
    i += 1
    # END

print("input SNR is : %.3f dB" % maxSNR)
print("output SNR is : %.3f dB" % finSNR)
print("Output amplitude : %.6f V" % maxAo)
plt.plot(aveSNRsin, aveAo, 'r-')
plt.axhline(y=1.8, linestyle="--")
plt.yscale('log')
plt.ylim(1.7, 1.9)
plt.yticks(np.arange(1.7, 1.9, step=0.01))
plt.grid(True)
plt.title('Voltage x SNR')
plt.show()
