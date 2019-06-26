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
    M = 1  # Decimation factor
    Fs_d = int(Fs / M)  # Decimated sample frequency
    Nyq = 0.5 * Fs_d

    N = 50000  # Filter length
    C = 0.1  # Cut-off frequency in Hz

    taps = signal.firwin(N, C/Nyq, window=('hamming'))  # Generate taps for FIR
                                                        # filter
    I = 200000  # Number of samples

    t = np.linspace(0, I/(Fs/Nch), I)

    CH1 = 4.75
    CH2 = 4.75

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

    # for i in range(Nch):  # Decimate
        # Qa_d.append(signal.decimate(Qa[:, i], M, ftype='fir'))
        # In_d.append(signal.decimate(In[:, i], M, ftype='fir'))

    for i in range(Nch):  # Filter Q & A
        FIRQa.append(signal.lfilter(taps, 1, Qa[:, i]))
        FIRIn.append(signal.lfilter(taps, 1, In[:, i]))

    for i in range(Nch):  # Calculate magnitude
        Ao.append(magnitude(FIRQa[i][:], FIRIn[i][:]))  # Calculate magnitude

    for i in range(Nch):  # Filter Magnitude
        FIRAo.append(signal.lfilter(taps, 1, Ao[i][:]))  # Filter magnitude

    FIRAo = scale(FIRAo)  # ADC Scalar

    snr = 20 * np.log10(150/(CH1 * 1000))
    print('\nSNR: %f    ' % (snr), end=" ")

    for i in range(Nch):
        print("Channel %d: %f," % (i, np.mean(FIRAo[i][-1000:])), end=" ")

    plt.figure(figsize=(7, 7))
    plt.subplot(313)
    plt.title("Magnitude")
    plt.xlabel("Time [S]")
    plt.ylabel("Amplitude [V]")
    for i in range(Nch):
        plt.plot(t, FIRAo[i][:I])
    plt.subplot(321)
    plt.title("Quadrature signal")
    plt.xlabel("Time [S]")
    plt.ylabel("Amplitude [bits]")
    plt.plot(t, Qa[:I, 0], 'b')
    plt.ylim(-10000, 10000)
    plt.subplot(322)
    plt.title("Inphase signal")
    plt.xlabel("Time [S]")
    plt.ylabel("Amplitude [bits]")
    plt.plot(t, In[:I, 0], 'r')
    plt.ylim(-10000, 10000)
    plt.subplot(323)
    plt.title("Filtered Inphase signal")
    plt.xlabel("Time [S]")
    plt.ylabel("Amplitude [bits]")
    plt.plot(t, FIRQa[0][:I], 'b')
    plt.ylim(-10000, 10000)
    plt.subplot(324)
    plt.title("Filtered Quadrature signal")
    plt.xlabel("Time [S]")
    plt.ylabel("Amplitude [bits]")
    plt.plot(t, FIRIn[0][:I], 'r')
    plt.ylim(-10000, 10000)
    plt.tight_layout()

    plt.figure()
    plt.subplot(313)
    plt.title("magnitude FFT")
    plt.xlim(0, 500)
    plt.magnitude_spectrum(FIRAo[0][:], Fs=(Fs/Nch), scale='dB')
    plt.ylim(-150, 150)
    plt.grid()
    plt.subplot(321)
    plt.title("Quadrature signal FFT")
    plt.xlim(0, 500)
    plt.magnitude_spectrum(Qa[:, 0], Fs=(Fs/Nch), scale='dB')
    plt.ylim(-150, 150)
    plt.grid()
    plt.subplot(322)
    plt.title("Inphase signal FFT")
    plt.xlim(0, 500)
    plt.magnitude_spectrum(In[:, 0], Fs=(Fs/Nch), scale='dB')
    plt.ylim(-150, 150)
    plt.grid()
    plt.subplot(323)
    plt.title("Filtered Inphase signal")
    plt.xlim(0, 500)
    plt.magnitude_spectrum(FIRQa[0][:], Fs=(Fs/Nch), scale='dB')
    plt.ylim(-150, 150)
    plt.grid()
    plt.subplot(324)
    plt.title("Filtered Quadrature signal")
    plt.xlim(0, 500)
    plt.magnitude_spectrum(FIRIn[0][:], Fs=(Fs/Nch), scale='dB')
    plt.ylim(-150, 150)
    plt.grid()
    plt.tight_layout()


if __name__ == '__main__':
    main()
