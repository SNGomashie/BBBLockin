import numpy as np
import matplotlib.pyplot as plt


def Quantizise(adcNoise):
    mul = (2 ** 15) / np.max(np.abs(adcNoise))

    adc16 = np.int16(adcNoise * mul)

    adcQ16 = np.divide(adc16, 1) * 1

    adcQ = np.double(adcQ16) / mul
    return adcQ


def main():
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

    adcNoise = Ve + Expnoise

    adcQ = Quantizise(adcNoise)

    X = adcQ * Vc
    Y = adcQ * Vs

    X_ = np.mean(X)
    Y_ = np.mean(Y)

    R = np.sqrt(pow(X_, 2) + pow(Y_, 2))
    P = np.arctan2(X_, Y_)

    print("Output [V]: {:03f},  Phase [degrees]: {:03f}, SNR [dB]: {:03f}".format(R, P, SNR))


if __name__ == "__main__":
    main()
