import numpy as np
import matplotlib.pyplot as plt
from scipy import signal


def main():
    Fs = 10000  # Sample frequency in Hz
    cutoff = 0.1  # Cutoff requency in Hz
    nyq = 0.5 * Fs

    N = 40000  # Number of taps in FIR filter
    order = 6  # Order number of IIR filter
    passband = 1
    stopband = 40

    ''' Generate coëfficients for FIR Filter
        firwin(num of taps, normalized cutoff, window type)
        butter(filter order, normalized cutoff, filter type, analog=False)
        cheby1(filter order, passband ripple, cutoff frequency, filter tpye, sample frequency)
        cheby2(filter order, stopband attenuation, cutoff frequency, filter type, sample frequency)
        ellip(filter order, passband ripple, stopband attenuation, cutoff frequency, filter type, sample frequency)
    '''
    taps = signal.firwin(N, cutoff/nyq, window=('hamming'))
    # b, a = signal.butter(order, cutoff/nyq, btype='low', analog=False)
    # b, a = signal.cheby1(order, passband, cutoff, 'lp', fs=Fs)
    # b, a = signal.cheby2(order, stopband, cutoff, 'lp', fs=Fs)
    # b, a = signal.ellip(order, passband, stopband, cutoff, 'lp', fs=Fs)

    w, h = signal.freqz(taps, 1.0, worN=20000)  # Compute the frequency response

    # Draw freuency plot
    plt.figure()
    plt.plot(nyq*w/np.pi, 20 * np.log10(np.abs(h)), 'b')
    plt.xlim(0, 10)
    plt.ylim(-150, 0)
    plt.title("FIR hamming Frequency Response")
    plt.xlabel('Frequency [Hz]')
    plt.ylabel('Attenuation [dB]')
    plt.grid()


if __name__ == "__main__":
    main()
