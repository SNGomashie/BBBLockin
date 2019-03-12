import numpy as np
from scipy import signal
import matplotlib.pyplot as plt



# define the configuration
fs = 16000 # sampling frequency
dt = 1/fs # step time
Ar = 1 # reference amplitude
Ai = 1.8 #input amplitude
n = np.arange(0, 0.025, dt) #time axis
fo = 100 #frequency of sinusoid
a = 10 #Noise scaling factor

#Quantizition
quant_bits = 16
quant_levels = np.power(2,quant_bits) / 2
quant_step = 1 / quant_levels

# Experimental signal
VsigCos = Ai * np.cos((n * 2 * np.pi * fo)-(np.pi/10))
# Added phase difference

# Reference signal(square wave) ( Also uncomment Ao)
# VrefCos = Ar * signal.square( n * 2 * np.pi * fo)
# VrefSin = Ar * signal.square(( n * 2 * np.pi * fo) - (np.pi/2))

# Reference signals(sinus and cosinus)
VrefCos = Ar * np.cos(n * 2 * np.pi * fo)
VrefSin = Ar * np.cos((n * 2 * np.pi * fo) - (np.pi/2))

# Noise generation
np.random.seed(45)
Expnoise = a * np.random.randn(len(n), 1)

# SNR
npwr = np.sum(np.power(Expnoise,2))
sigpwr = np.sum(np.power(VsigCos,2))

# Input + Noise
VsigCosandNoise = VsigCos + Expnoise

# Make a 16bit number
VsigCosandNoise16 = np.round(VsigCosandNoise / quant_step) * quant_step

#Convet back to double
VsigCosandNoiseQ = np.float64(VsigCosandNoise16)

# Multiply signal with reference Sin and Cos
Vc = VsigCosandNoiseQ * VrefCos
Vs = VsigCosandNoiseQ * VrefSin

# calculate RMS
Ivs = np.mean(Vs)
Qvc = np.mean(Vc)

# Find magnitude (Square wave)
#Ao[k] = (np.pi/4) * 2 * np.sqrt((np.power(Ivs, 2))+(np.power(Qvc, 2)));

# Find magnitude
Ao = 2 * np.sqrt((np.power(Ivs, 2))+(np.power(Qvc, 2)))

print("Input amplitude : %.3f V" % Ai)
print("Output amplitude : %.6f V" % Ao)

# Find phase
Phase = np.arctan2(Ivs, Qvc)
print("Phase difference : %.3f rads" % Phase)

SNR = 10*np.log10((sigpwr)/(npwr))
Aerr = Ao - Ai
print("SNR is : %.3f dB" % SNR )
print("Amplitude error : %.6f V" % Aerr)

plt.plot(n, VsigCos, 'rx')
plt.show()
plt.plot(n, VsigCosandNoiseQ)
plt.show()
plt.plot(n, Vs)
plt.show()
plt.plot(n, Vc)
plt.show()
