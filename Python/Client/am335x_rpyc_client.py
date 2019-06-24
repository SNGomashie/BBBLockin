import rpyc
import matplotlib.pyplot as plt
from scipy import signal
import numpy as np


def format(data, Nch):
    data = np.asarray(data)

    Qa_ch = np.zeros((int(len(data[:, 0])/Nch), Nch))
    In_ch = np.zeros((int(len(data[:, 1])/Nch), Nch))

    Qa = data[:, 0]
    In = data[:, 1]

    for i, x in enumerate(Qa):
        Qa_ch[(int(i/Nch)-1, (i % Nch))] = x
    for i, x in enumerate(In):
        In_ch[(int(i/Nch))-1, (i % Nch)] = x

    return Qa_ch, In_ch


def command(Fs, Nch):
    return (Nch << 16) | Fs


def magnitude(Qa, In):
    Ao = np.sqrt((Qa**2)+(In**2))
    return Ao


def scale(Ao):
    Ao = [i * 1.5259254737998596148564104129154e-4 for i in Ao]
    # Ao *= 1.5259254737998596148564104129154e-4
    return Ao


def main():
    Fs = 10000  # Sample frequency in Hz / Nch
    Nch = 4  # Number of channels to sample
    Fs_n = int(Fs * Nch)
    M = 10  # Decimation factor
    Fs_d = int(Fs / M)
    Nyq = 0.5 * Fs_d

    N = 10000  # Filter length
    C = 0.1  # Cut-off frequency in Hz

    taps = signal.firwin(N, C/Nyq, window=('hamming'))  # Generate taps for FIR
                                                        # filter
    I = 20000  # Number of samples

    CH1 = 0.150
    CH2 = 0.250

    Qa_d = []
    In_d = []

    Ao = []
    FIRAo = []
    FIRQa = []
    FIRIn = []

    BB = rpyc.connect("192.168.7.2", 18861)  # Connect to remote BeagleBone
    BB.root.pru_initialize()  # Initialize PRU-ICSS class
    BB.root.fgen_initialize()  # Initialize Agilent 33522A class

    BB.root.pru_stop(0)  # Stop PRU0
    BB.root.pru_stop(1)  # Stop PRU1

    BB.root.fgen_write("SOURCE1:VOLTAGE %f" % (CH1))  # Set CH1 voltage to 'CH1'
    BB.root.fgen_write("SOURCE2:VOLTAGE %f" % (CH2))  # Set CH2 voltage to 'CH2'

    BB.root.pru_start(0)  # Start PRU0
    BB.root.pru_start(1)  # Start PRU1

    BB.root.pru_open(0)  # Open character device for PRU0

    BB.root.pru_transmit(0, command(Fs_n, Nch))  # Send Sample frequency and number of channels

    data = BB.root.pru_receive(0, '2int32', (Nch * I))  # Receive samples

    Qa, In = format(data, Nch)  # Split 1D array into 2, 2D arrays

    for i in range(Nch):  # Decimate
        Qa_d.append(signal.decimate(Qa[:, i], 10))
        In_d.append(signal.decimate(In[:, i], 10))

    for i in range(Nch):  # Filter Q & A
        FIRQa.append(signal.lfilter(taps, 1, Qa_d[i]))
        FIRIn.append(signal.lfilter(taps, 1, In_d[i]))

    for i in range(Nch):  # Calculate magnitude
        Ao.append(magnitude(FIRQa[i], FIRIn[i]))  # Calculate magnitude

    for i in range(Nch):  # Filter Magnitude
        FIRAo.append(signal.lfilter(taps, 1, Ao[i]))  # Filter magnitude

    FIRAo = scale(FIRAo)  # ADC Scalar

    plt.figure()
    plt.subplot(313)
    plt.title("Magnitude")
    plt.plot(FIRAo[0], 'orange')
    plt.plot(FIRAo[1], 'red')
    plt.subplot(321)
    plt.title("Quadrature signal")
    plt.plot(Qa[:, 0], 'b')
    plt.ylim(-32767, 32767)
    plt.subplot(322)
    plt.title("Inphase signal")
    plt.plot(In[:, 0], 'r')
    plt.ylim(-32767, 32767)
    plt.subplot(323)
    plt.title("Filtered Inphase signal")
    plt.plot(FIRQa[0], 'b')
    plt.ylim(-32767, 32767)
    plt.subplot(324)
    plt.title("Filtered Quadrature signal")
    plt.plot(FIRIn[0], 'r')
    plt.ylim(-32767, 32767)
    plt.tight_layout()

    plt.figure()
    plt.subplot(313)
    plt.title("magnitude FFT")
    plt.magnitude_spectrum(FIRAo[0], Fs=(Fs/Nch), scale='dB')
    plt.ylim(-150, 150)
    plt.grid()
    plt.subplot(321)
    plt.title("Quadrature signal FFT")
    plt.magnitude_spectrum(Qa[:, 0], Fs=(Fs/Nch), scale='dB')
    plt.ylim(-150, 150)
    plt.grid()
    plt.subplot(322)
    plt.title("Inphase signal FFT")
    plt.magnitude_spectrum(In[:, 0], Fs=(Fs/Nch), scale='dB')
    plt.ylim(-150, 150)
    plt.grid()
    plt.subplot(323)
    plt.title("Filtered Inphase signal")
    plt.magnitude_spectrum(FIRQa[0], Fs=(Fs/Nch), scale='dB')
    plt.ylim(-150, 150)
    plt.grid()
    plt.subplot(324)
    plt.title("Filtered Quadrature signal")
    plt.magnitude_spectrum(FIRIn[0], Fs=(Fs/Nch), scale='dB')
    plt.ylim(-150, 150)
    plt.grid()
    plt.tight_layout()

    snr = 20 * np.log10(150/(CH1 * 1000))
    print('\nSNR: %f    ' % (snr), end=" ")
    print(np.shape(FIRAo))
    print(type(FIRAo))
    for i in range(Nch):
        print("Channel %d: %f," % (i, np.mean(FIRAo[i][-100:])), end=" ")


if __name__ == '__main__':
    main()
