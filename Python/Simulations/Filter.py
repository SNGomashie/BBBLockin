import numpy as np
import matplotlib.pyplot as plt
from scipy import signal, fftpack


def ma():
    Fs = 10000
    cutoff = 0.1
    nyq = 0.5 * Fs

    N = 40000

    taps = signal.firwin(N, cutoff/nyq, window=('hamming'))

    w, h = signal.freqz(taps, 1.0, worN=20000)

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
