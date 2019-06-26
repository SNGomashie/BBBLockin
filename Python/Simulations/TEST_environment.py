import matplotlib.pyplot as plt
import numpy as np

adc = np.load("C:/Users/senag/Documents/GitHub/BBBLockin/Output files/adc_out.npy")
sin = np.load("C:/Users/senag/Documents/GitHub/BBBLockin/Output files/sin_out.npy")
sin2 = np.load("C:/Users/senag/Documents/GitHub/BBBLockin/Output files/sin2_out.npy")
cos = np.load("C:/Users/senag/Documents/GitHub/BBBLockin/Output files/cos_out.npy")
Qa = np.load("C:/Users/senag/Documents/GitHub/BBBLockin/Output files/Qa_out.npy")
In = np.load("C:/Users/senag/Documents/GitHub/BBBLockin/Output files/In_out.npy")
noise = np.load("C:/Users/senag/Documents/GitHub/BBBLockin/Output files/noise_out.npy")

t = np.linspace(0, 1, 10000)

plt.figure(figsize=(8, 5))
plt.subplot(221)
plt.title("Quadrature result")
plt.ylabel("Amplitude [V]")
plt.xlabel("Time [S]")
plt.xlim(0, 0.083)
plt.ylim(-1000, 1000)
plt.plot(t, Qa[:10000], "purple")
plt.grid()

plt.subplot(223)
plt.title("Inphase result")
plt.ylabel("Amplitude [V]")
plt.xlabel("Time [S]")
plt.xlim(0, 0.083)
plt.ylim(-1000, 1000)
plt.plot(t, In[:10000], "black")
plt.grid()

plt.subplot(222)
plt.title("FFT of quadrature result")
plt.magnitude_spectrum(Qa[:10000, 0], Fs=10000, scale="dB", color="purple")
plt.xlim(0, 500)
plt.ylim(-100, 100)
plt.grid()

plt.subplot(224)
plt.title("FFT of Inphase result")
plt.magnitude_spectrum(In[:10000, 0], Fs=10000, scale='dB', color="black")
plt.xlim(0, 500)
plt.ylim(-100, 100)
plt.grid()

plt.tight_layout()
