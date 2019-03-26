import numpy as np
import matplotlib.pyplot as plt
from scipy import signal, fftpack

# Frequncies and periods
Fs = 10000  # Hz
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

# Experimental signal
VsigCos = Ai * np.cos((t * 2 * np.pi * Fr) - (np.pi / 10))

# Reference signals(sinus and cosinus)
VrefCos = Ar * np.cos(t * 2 * np.pi * Fr)
VrefSin = Ar * np.cos((t * 2 * np.pi * Fr) - (np.pi / 2))

# Reference signal(square wave) ( Also uncomment Ao)
VrefCossqr = Ar * signal.square(t * 2 * np.pi * Fr)
VrefSinsqr = Ar * signal.square((t * 2 * np.pi * Fr) - (np.pi/2))

# Noise generation
np.random.seed(45)
Noise = a * np.random.randn(len(t))

# SNR
npwr = sum(np.power(Noise, 2))
sigpwr = sum(np.power(VsigCos, 2))
SNR = 10 * np.log10((sigpwr) / (npwr))

# Input + Noise
VsigCosandNoise = VsigCos + Noise

# Multiply signal with reference Sin and Cos
Vc = VsigCosandNoise * VrefCos
Vs = VsigCosandNoise * VrefSin

# Multiply signal with reference Sin and Cos without noise
Vcnn = VsigCos * VrefCos
Vsnn = VsigCos * VrefSin

# calculate RMS
Ivs = np.mean(Vs)
Qvc = np.mean(Vc)

# Find magnitude (Square wave)
# Ao[k] = (np.pi/4) * 2 * np.sqrt((np.power(Ivs, 2))+(np.power(Qvc, 2)));

# Calculate magnitude of vector
Ao = 2 * np.sqrt((np.power(Ivs, 2)) + (np.power(Qvc, 2)))

# print refernce voltage and Lock-in output
print("Input amplitude : %.3f V" % Ai)
print("Output amplitude : %.6f V" % Ao)

# Find phase
Phase = np.arctan2(Ivs, Qvc)
print("Phase difference : %.3f rads" % Phase)

# Print noise immunity
Aerr = Ao - Ai
print("SNR is : %.3f dB" % SNR)
print("Amplitude error : %.6f V" % Aerr)


plt.subplot(2, 1, 1)
plt.title("Experiment signal")
plt.xlabel("Time [s]")
plt.ylabel("Amplitude [V]")
plt.grid(True)
plt.xlim(0, 0.1)
plt.plot(t, VsigCos)
plt.subplot(2, 1, 2)
plt.title("SYNC signal")
plt.xlabel("Time [s]")
plt.ylabel("Amplitude [V]")
plt.grid(True)
plt.xlim(0, 0.1)
plt.plot(t, VrefCossqr)
plt.tight_layout()
plt.show()

plt.subplot(3, 1, 1)
plt.title("Experiment signal")
plt.xlabel("Time [s]")
plt.ylabel("Amplitude [V]")
plt.grid(True)
plt.xlim(0, 0.1)
plt.plot(t, VsigCos)
plt.subplot(3, 1, 2)
plt.title("Reference cosinus")
plt.xlabel("Time [s]")
plt.ylabel("Amplitude [V]")
plt.grid(True)
plt.xlim(0, 0.1)
plt.plot(t, VrefCos)
plt.subplot(3, 1, 3)
plt.title("Reference sinus")
plt.xlabel("Time [s]")
plt.ylabel("Amplitude [V]")
plt.grid(True)
plt.xlim(0, 0.1)
plt.plot(t, VrefSin)
plt.tight_layout()
# plt.savefig('sigrefref.png')
plt.show()

plt.subplot(2, 1, 1)
plt.title("Experiment signal without noise")
plt.plot(t, VsigCos)
plt.xlabel("Time [s]")
plt.ylabel("Amplitude [V]")
plt.xlim(0, 0.1)
plt.subplot(2, 1, 2)
plt.title("Experiment signal with noise")
plt.plot(t, VsigCosandNoise)
plt.xlabel("Time [s]")
plt.ylabel("Amplitude [V]")
plt.xlim(0, 0.1)
plt.grid(True)
plt.tight_layout()
# plt.savefig('Vnoisenonoisetime.png')
plt.show()

plt.subplot(2, 1, 1)
plt.title("Signal without noise")
plt.magnitude_spectrum(VsigCos, Fs=Fs, color='C1', scale='dB')
plt.xlim(0, 200)
plt.ylim(-30, 0)
plt.subplot(2, 1, 2)
plt.title("Signal wth noise")
plt.magnitude_spectrum(VsigCosandNoise, Fs=Fs, color='C2', scale='dB')
plt.xlim(0, 200)
plt.ylim(-30, 0)
plt.tight_layout()
# plt.savefig('Vnoisenonoisefreq.png')
plt.show()

plt.subplot(2, 2, 1)
plt.title("Signal * Cos no noise")
plt.magnitude_spectrum(Vcnn, Fs=Fs, color='C1', scale='dB')
plt.xlim(-10, 210)
plt.ylim(-30, 0)
plt.subplot(2, 2, 2)
plt.title("Signal * Sin no noise")
plt.magnitude_spectrum(Vsnn, Fs=Fs, color='C2', scale='dB')
plt.xlim(-10, 210)
plt.ylim(-30, 0)
plt.subplot(2, 2, 3)
plt.title("Signal * Cos noise")
plt.magnitude_spectrum(Vc, Fs=Fs, color='C3', scale='dB')
plt.xlim(-10, 210)
plt.ylim(-30, 0)
plt.subplot(2, 2, 4)
plt.title("Signal * Sin noise")
plt.magnitude_spectrum(Vs, Fs=Fs, color='C4', scale='dB')
plt.xlim(-10, 210)
plt.ylim(-30, 0)
plt.tight_layout()
# plt.savefig('vcvs.png')
plt.show()
