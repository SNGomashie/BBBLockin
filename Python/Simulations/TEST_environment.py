import numpy as np
from scipy import signal
import matplotlib.pyplot as plt


def butter_lowpass(cutoff, fs, order=5):
    nyq = 0.5 * fs
    normal_cutoff = cutoff / nyq
    b, a = signal.butter(order, normal_cutoff, btype='low', analog=False)
    return b, a


def butter_lowpass_filter(data, cutoff, fs, order=5):
    b, a = butter_lowpass(cutoff, fs, order=order)
    y = signal.lfilter(b, a, data)
    return y


def generate_input(n, Fr, Fs, off):
    out = 5 * np.sin(n * 2 * np.pi * Fr) + off
    return out


if __name__ == "__main__":
    plt.close('all')
    # Parameters
    Fs = 4000  # Sample frequency (Hz)
    Fr = 200  # Reference freuqnecy (Hz)
    T = 100  # Amount of periods
    M = 60  # Decimation factor
    Fs_M = 4000 / M  # Sample frequency after decimation (Hz)
    Cutoff = 5 # Cut-off frequency (Hz)

    # Timing
    Time = T / Fr
    n = np.linspace(0, Time, int(Fs*Time))
    n_M = np.linspace(0, Time, int(Fs_M*Time))

    # Generate sinus with DC offset
    sin = generate_input(n=n, Fr=200, Fs=4000, off=10)

    # Decimate
    sin_M = signal.decimate(sin, M)

    # Filter design
    b, a = butter_lowpass(Cutoff, Fs, order=6)
    b_M, a_M = butter_lowpass(Cutoff, Fs_M, order=6)
    # Low-pass DC
    result = signal.lfilter(b, a, sin)
    result_M = signal.lfilter(b_M, a_M, sin_M)

    # Plots

    # Frequency reponse plots
    w, h = signal.freqz(b, a, worN=8000)
    w_M, h_M = signal.freqz(b_M, a_M, worN=8000)

    plt.figure()
    plt.subplot(2, 1, 1)
    plt.plot(0.5*Fs*w/np.pi, np.abs(h), 'b')
    plt.plot(Cutoff, 0.5*np.sqrt(2), 'ko')
    plt.axvline(Cutoff, color='k')
    plt.xlim(0, 0.5*Fs)
    plt.title("Lowpass Filter Frequency Response")
    plt.xlabel('Frequency [Hz]')
    plt.grid()

    plt.subplot(2, 1, 2)
    plt.plot(0.5*Fs_M*w_M/np.pi, np.abs(h_M), 'b')
    plt.plot(Cutoff, 0.5*np.sqrt(2), 'ko')
    plt.axvline(Cutoff, color='k')
    plt.xlim(0, 0.5*Fs_M)
    plt.title("Lowpass Filter Frequency Response")
    plt.xlabel('Frequency [Hz]')
    plt.grid()

    # Output plots
    plt.figure()
    plt.subplot(3, 1, 1)
    plt.plot(result)
    plt.subplot(3, 1, 2)
    plt.plot(result_M)
    plt.subplot(3, 1, 3)
    plt.magnitude_spectrum(sin, Fs, color='blue', scale='dB')
    plt.magnitude_spectrum(sin_M, Fs_M, color='blue', scale='dB')
    plt.xlim(0, 400)
