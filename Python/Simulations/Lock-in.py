import numpy as np
import matplotlib.pyplot as plt

Fs = 10000  # Sample frequency (Hz)
Ts = 1 / Fs  # Sample period (S)

Fr = 100  # Reference frequency (Hz)
Tr = 1 / Fr  # Referece period (S)

Ar = 2
Ae = 1.8

T = 1  # Total time (S)
S = int(T * Fs)  # Amount of samples
t = np.linspace(0, T, S, endpoint=False)

Ve = Ae * np.sin((2 * np.pi * t * Fr) - (np.pi))  # Experiment signal
Vc = Ar * np.cos(2 * np.pi * t * Fr)  # Cosine reference signal
Vs = Ar * np.sin(2 * np.pi * t * Fr)  # Sinus reference signal

a = 10 ** 1


np.random.seed(45)
Expnoise = a * np.random.randn(len(t))

npwr = np.sum(np.power(Expnoise, 2))
spwr = np.sum(np.power(Ve, 2))

SNR = 10 * np.log10((spwr) / (npwr))
print(SNR)

adcNoise = Ve + Expnoise
plt.psd(Ve)
plt.psd(Expnoise)
plt.psd(adcNoise)
plt.show()
plt.magnitude_spectrum(adcNoise, Fs=Fs, color='black', scale='dB')
plt.show()

mul = (2 ** 15) / np.max(np.abs(adcNoise))

adc16 = np.int16(adcNoise * mul)

adcQ16 = np.divide(adc16, 1) * 1

adcQ = np.double(adc16) / mul

plt.magnitude_spectrum(adcQ, Fs=Fs, color='red', scale='dB')
plt.show()

X = adcQ * Vc
Y = adcQ * Vs

X_ = np.mean(X)
Y_ = np.mean(Y)

R = np.sqrt(pow(X_, 2) + pow(Y_, 2))
P = np.arctan2(X_, Y_)
print("Output [V]: %03f" % (R))
print("Phase [V]: %03f" % (P))

plt.psd(X)
plt.psd(Y)
plt.show()

plt.figure(figsize=(10, 6))
plt.subplot(3, 2, 1)
plt.title("Time domain plot experiment signal")
plt.plot(t, Ve, 'k.')
plt.ylabel("Amplitude [V]")
plt.xlabel("Time [S]")
plt.xlim(0, 0.01)
plt.grid()
plt.subplot(3, 2, 3)
plt.title("Time domain plot cosine signal")
plt.plot(t, Vc, 'r.')
plt.ylabel("Amplitude [V]")
plt.xlabel("Time [S]")
plt.xlim(0, 0.01)
plt.grid()
plt.subplot(3, 2, 5)
plt.title("Time domain plot in-phase (I / X) signal")
plt.plot(t, X, 'm.')
plt.ylabel("Amplitude [V]")
plt.xlabel("Time [S]")
plt.xlim(0, 0.01)
plt.grid()
plt.subplot(3, 2, 2)
plt.magnitude_spectrum(Ve, Fs=Fs, color='black', scale='dB')
plt.grid()
plt.subplot(3, 2, 4)
plt.magnitude_spectrum(Vc, Fs=Fs, color='red', scale='dB')
plt.grid()
plt.subplot(3, 2, 6)
plt.magnitude_spectrum(X, Fs=Fs, color='m', scale='dB')
plt.grid()
plt.tight_layout()
plt.savefig("plots2.png")
plt.show()
