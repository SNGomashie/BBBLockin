import numpy as np
import matplotlib.pyplot as plt
from scipy import signal, fftpack

adc = np.int32(np.genfromtxt('C:/Users/senag/Documents/Project_SRON/Output_files/adc_out.out', delimiter='\n', unpack=True, dtype=float))
cos = np.int32(np.genfromtxt('C:/Users/senag/Documents/Project_SRON/Output_files/cos_out.out', delimiter='\n', unpack=True, dtype=float))
sin = np.int32(np.genfromtxt('C:/Users/senag/Documents/Project_SRON/Output_files/sin_out.out', delimiter='\n', unpack=True, dtype=float))
Qa = np.int32(np.genfromtxt('C:/Users/senag/Documents/Project_SRON/Output_files/Qa_out.out', delimiter='\n', unpack=True, dtype=float))
In = np.int32(np.genfromtxt('C:/Users/senag/Documents/Project_SRON/Output_files/In_out.out', delimiter='\n', unpack=True, dtype=float))

Sample_frequency = 4000
frequency = 100
period = 1 / frequency

T = 2000 * period
n = np.linspace(0, T, int(T * Sample_frequency), endpoint=False)

plot = "black_window"

x = np.linspace(-10, 10, 200)
sinc = np.sinc(x)

window = np.blackman(200)

if plot == "input":
    plt.figure()
    plt.subplot(3, 2, 1)
    plt.title("ADC input @ 100Hz")
    plt.plot(adc, 'r')
    plt.xlim(0, 400)
    plt.subplot(3, 2, 2)
    plt.title("FFT of ADC input @ 100Hz")
    plt.magnitude_spectrum(adc, Fs=4000, color='red', scale='dB', label="ADC input")
    plt.xlim(0, 500)
    plt.subplot(3, 2, 3)
    plt.title("COS reference @ 100Hz")
    plt.plot(cos, 'b')
    plt.xlim(0, 400)
    plt.subplot(3, 2 ,4)
    plt.title("FFT of COS reference values @ 100Hz")
    plt.magnitude_spectrum(cos, Fs=4000, color='blue', scale='dB', label="COS reference")
    plt.xlim(0, 500)
    plt.subplot(3, 2, 5)
    plt.title("SIN refernce @ 100Hz")
    plt.plot(sin, 'orange')
    plt.xlim(0, 400)
    plt.subplot(3, 2, 6)
    plt.title("FFT of SIN reference values @ 100HZ")
    plt.magnitude_spectrum(sin, Fs=4000, color='orange', scale='dB', label="SIN reference")
    plt.xlim(0, 500)
    plt.tight_layout()
elif plot == "quadrature":
    plt.figure()
    plt.subplot(221)
    plt.title("Quadrature result")
    plt.plot(Qa)
    plt.xlim(0, 400)
    plt.subplot(222)
    plt.title("FFT of quadrature result")
    plt.magnitude_spectrum(Qa, Fs=4000, color='orange', scale='dB')
    plt.xlim(0, 500)
    plt.subplot(223)
    plt.title("In-phase result")
    plt.plot(In)
    plt.xlim(0, 400)
    plt.subplot(224)
    plt.title("FFT of In-phase result")
    plt.magnitude_spectrum(In, Fs=4000, color='orange', scale='dB')
    plt.xlim(0, 500)
    plt.tight_layout()
elif plot == "average":
    Fs = 4000
    N1 = 10
    N2 = 50
    N3 = 200
    B1 = np.full(N1-1, 1/N1)
    B2 = np.full(N2-1, 1/N2)
    B3 = np.full(N3-1, 1/N3)
    MAw, MAh = signal.freqz(B1, 1.0, worN=8000)
    MAw2, MAh2 = signal.freqz(B2, 1.0, worN=8000)
    MAw3, MAh3 = signal.freqz(B3, 1.0, worN=8000)
    plt.plot(0.5*Fs*MAw/np.pi, np.abs(MAh), 'r', label="L = 10")
    plt.plot(0.5*Fs*MAw2/np.pi, np.abs(MAh2), 'b', label="L = 50")
    plt.plot(0.5*Fs*MAw3/np.pi, np.abs(MAh3), 'k', label="L = 200")
    plt.title("Moving Average Frequency Response")
    plt.xlabel('Frequency [Hz]')
    plt.legend()
    plt.grid()
    plt.tight_layout()
elif plot == "sinc":
    plt.figure()
    plt.title("Sinc function")
    plt.ylabel("Amplitude")
    plt.plot(x, sinc)
elif plot == "blackman":
    plt.figure()
    plt.title("Blackman window")
    plt.ylabel("Amplitude")
    plt.xlabel("Sample")
    plt.plot(window)
elif plot == "black_window":
    plt.figure()
    plt.title("Normalized window sinc filter")
    plt.ylabel("Amplitude")
    plt.xlabel("Sample")
    plt.plot((window*sinc))
