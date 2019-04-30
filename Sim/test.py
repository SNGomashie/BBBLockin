import numpy as np
import matplotlib.pyplot as plt


Fs = 10000
Ts = 1/Fs
P = 100
Fr = 100
Tr = 1/Fr
Ai = 1.8

T = P * Tr  # periods
n = int(T * Fs)
t = np.linspace(0, T, n, endpoint=False)

Sinout = np.int16(26214 * np.sin(t * 2 * np.pi * Fr))
Sinref = np.int16(13108 * np.sin(t * 2 * np.pi * Fr))
Cosref = np.int16(13108 * np.cos(t * 2 * np.pi * Fr))

Vq = Sinout * Sinref
Vi = Sinout * Cosref

Qa = np.mean(Vq)
In = np.mean(Vi)

raw = 2 * np.sqrt((np.power(Qa, 2)) + (np.power(In, 2)))

scale = 5 / 65536
Ao = raw * scale
Ao *= scale
print(Ao)

plt.plot(t, Sinout)
plt.show()
plt.plot(t, Sinref)
plt.show()


plt.magnitude_spectrum(Sinout, Fs=Fs, color='C2', scale='dB')
plt.xscale("log")
plt.xlim(0, 200)
plt.show()


plt.magnitude_spectrum(Sinref, Fs=Fs, color='C2', scale='dB')
plt.xscale("log")
plt.xlim(0, 200)
plt.show()

# plt.magnitude_spectrum(Ao, Fs=Fs, color='C2', scale='dB')
# plt.xscale("log")
# plt.xlim(0, 200)
# plt.show()
