import numpy as np
import scipy.fftpack
import matplotlib.pyplot as plt


# define the configuration
fs = 8000  # sampling frequency
dt = 1 / fs  # step time
Ar = 1  # reference amplitude
Ai = 1.8  # input amplitude
fo = 50  # frequency of sinusoid
n = np.arange(0, 4 / fo, dt)  # time axis
a = 0.1  # Noise scaling factor


# Quantizition
quant_bits = 16
quant_levels = np.power(2, quant_bits) / 2
quant_step = 1 / quant_levels

# Experimental signal
VsigCos = Ai * np.cos((n * 2 * np.pi * fo) - (np.pi / 10))
# Added phase difference

# Reference signal(square wave) ( Also uncomment Ao)
# VrefCos = Ar * signal.square( n * 2 * np.pi * fo)
# VrefSin = Ar * signal.square(( n * 2 * np.pi * fo) - (np.pi/2))

# Reference signals(sinus and cosinus)
VrefCos = Ar * np.cos(n * 2 * np.pi * fo)
VrefSin = Ar * np.cos((n * 2 * np.pi * fo) - (np.pi / 2))

# Noise generation
np.random.seed(45)
Expnoise = a * np.random.randn(len(n), 1)

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

J = len(n)
T = dt
x = np.linspace(0.0, J * T, J)
Vcfft = scipy.fftpack.fft(Vc)
Vsfft = scipy.fftpack.fft(Vs)

xfft = np.linspace(0.0, 1.0 / (2.0 * T), J / 2)
plt.plot(xfft, 2.0 / J * np.abs(Vcfft[:J // 2]))
plt.show()

plt.plot(n, VsigCos, 'rx')
plt.show()
