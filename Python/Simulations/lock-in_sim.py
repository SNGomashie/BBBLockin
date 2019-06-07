import numpy as np
import matplotlib.pyplot as plt
from scipy import signal



n = np.linspace(0, 1, 1 * 10000)
adc = 1.8 * np.cos((n * 2 * np.pi * 100) - (np.pi / 10))


def generate_signals():
    data['adc'] = 1.8 * np.cos((n * 2 * np.pi * parameters['Fr']) - (np.pi / 10))
    data['cos'] = 2 * np.cos(n * 2 * np.pi * parameters['Fr'])
    data['sin'] = 2 * np.cos((n * 2 * np.pi * parameters['Fr']) - (np.pi / 2))


def generate_noise(a, k):
    np.random.seed(k + 69)
    data['noise'] = a * np.random.randn(len(n))


def add_noise():
    data['adc_noise'] = data['adc'] + data['noise']


def calculate_SNR(sig, noise):
    return (10 * np.log10(np.sum(sig ** 2)/np.sum(noise ** 2)))


def calculate_magnitude(Q, I):
    return np.sqrt((Q ** 2) + (I ** 2))


def quadrature():
    data['Q'] = data['adc_noise'] * data['sin']
    data['I'] = data['adc_noise'] * data['cos']


def time_plot(x, y):
    plt.figure()
    plt.plot(x, y)


def psd_plot(x):
    plt.figure()
    plt.psd(x)


def fft_plot(x, Fs):
    plt.figure()
    plt.magnitude_spectrum(x, Fs=Fs, color='black', scale='dB')


if __name__ == "__main__":
    parameters = {
        'Fs': 200000,
        'Fr': 100,
        'time': 1,
    }

    n = np.linspace(0, parameters['time'], (int(parameters['time'] * parameters['Fs'])))

    data = {
        "adc": [],
        "adc_noise": [],
        "cos": [],
        "sin": [],
        "noise": [],
        "Q": [],
        "I": []
    }

    results = {
        'SNR': [],
        'Ao': [],
        'Phase': [],
        'Err': [],
        'aveSNR': [],
        'aveAo': [],
        'aveErr': []
    }

    generate_signals()
    time_plot(n, data['adc'])
    for a in np.logspace(-1, 4, 100):
        for k in range(1000):
            generate_noise(a, k)
            SNR = calculate_SNR(data['adc'], data['noise'])
            add_noise()
            quadrature()
            data['Q'] = np.mean(data['Q'])
            data['I'] = np.mean(data['I'])
            results['SNR'].append(SNR)
            results['Ao'].append(calculate_magnitude(data['Q'], data['I']))
        results['aveSNR'].append(np.mean(results['SNR']))
        results['aveAo'].append(np.mean(results['Ao']))

    time_plot(results['aveSNR'], results['aveAo'])
