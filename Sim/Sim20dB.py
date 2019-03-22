import numpy as np
from scipy import signal
import matplotlib.pyplot as plt

# define the configuration
fs = 300  # sampling frequency
dt = 1 / fs  # step time
Ar = 1  # Reference amplitude
Ai = 1.8  # Input amplitude
fo = 100  # Refernce frequency
n = np.arange(0, 1 / fo, dt)  # Time axis
errIncr = 0.02  # Error percentage allowed

# Quantizition
quant_bits = 16
quant_levels = np.power(2, quant_bits) / 2
quant_step = 1 / quant_levels

# Experimental signal (Sinusoid)
VsigCos = Ai * np.cos((n * 2 * np.pi * fo) - (np.pi / 10)
                      )  # Added phase difference

# Reference signal(square wave) ( Also uncomment Ao)
# VrefCos = Ar * signal.square( n * 2 * np.pi * fo)
# VrefSin = Ar * signal.square(( n * 2 * np.pi * fo) - (np.pi/2))

# Reference signals(sinus and cosinus)
VrefCos = Ar * np.cos(n * 2 * np.pi * fo)
# Cosinus phase shifted to create Sinus
VrefSin = Ar * np.cos((n * 2 * np.pi * fo) - (np.pi / 2))

# Amount of random noise samples to be averaged
kmax = np.arange(0, 1000, 1)


# Initialize arrays
Ao = np.zeros(len(kmax))
SNR = np.zeros(len(kmax))
Aerr = np.zeros(len(kmax))
aveSNRsin = np.zeros(len(x))
aveAerrsin = np.zeros(len(x))
aveAo = np.zeros(len(x))

i = 0
for k in kmax:
    # Noise generation
    np.random.seed(k + 45)
    Expnoise = 0.1 * np.random.randn(len(n), 1)

    # SNR
    npwr = np.sum(np.power(Expnoise, 2))
    sigpwr = np.sum(np.power(VsigCos, 2))

    # Input + Noise
    VsigCosandNoise = VsigCos + Expnoise

    # Make a 16bit number
    VsigCosandNoise16 = np.round(VsigCosandNoise / quant_step) * quant_step

    # Convet back to double
    VsigCosandNoiseQ = np.float64(VsigCosandNoise16)

    # Multiply signal with reference Sin and Cos
    Vc = VsigCosandNoiseQ * VrefCos
    Vs = VsigCosandNoiseQ * VrefSin

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

aveSNRsin = np.mean(SNR)
aveAerrsin = np.mean(np.power(Aerr, 2))
aveAo = np.mean(Ao)
i += 1
print(aveSNRsin)
print(aveAo)