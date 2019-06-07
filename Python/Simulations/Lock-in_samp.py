import numpy as np
import matplotlib.pyplot as plt

%matplotlib qt5

Fs = 80000  # Sample frequency (Hz)
Ar = 2  # Reference amplitude [V]
Ae = 1.8  # Experiment amplitude [V]
t = 1
n = np.linspace(0, (t * Fs))
Fr = 2000  # Reference frequency (Hz)
Err = 0.001

adc = Ae * np.cos((n * 2 * np.pi * Fr) - (np.pi / 10))
cos = Ar * np.cos(n * 2 * np.pi * Fr)
sin = Ar * np.cos((n * 2 * np.pi * Fr) - (np.pi / 2))

Aolimstore = np.array([])
Aerrlimstore = np.array([])

kmax = 1000

aveSNRsin = []
aveAerrsin = []
aveAo = []

x = np.logspace(-1, 2, 100)

for a in x:
    npwr = []
    sigpwr = []
    Ao = []
    SNR = []
    Aerr = []
    for k in range(kmax):
        # Noise addition
        np.random.seed(k + 45)
        Expnoise = a * np.random.randn(len(n))

        npwr.append(np.sum(Expnoise ** 2))
        sigpwr.append(np.sum(adc ** 2))

        adcNoise = adc + Expnoise

        mul = (2 ** 15) / np.max(np.abs(adcNoise))

        adc16 = np.int16(adcNoise * mul)

        adcQ16 = np.divide(adc16, 1) * 1

        adcQ = np.double(adc16) / mul

        Vs = adcQ * sin
        Vc = adcQ * cos

        Qa = np.mean(Vs)
        In = np.mean(Vc)

        Ao.append(np.sqrt((Qa ** 2)+(In ** 2)))

        SNR.append(10*np.log10((sigpwr[k])/(npwr[k])))

        Aerr.append(Ao[k]-Ae)

    SNR = np.array(SNR)
    Aerr = np.array(Aerr)
    Ao = np.array(Ao)

    aveSNRsin.append(np.mean(SNR))
    aveAerrsin.append(np.mean(Aerr ** 2))
    aveAo.append(np.mean(Ao))

aveSNRsin = np.array(aveSNRsin)
aveAo = np.array(aveAo)

Cutoff = [Ae+(Ae*Err)] * len(aveSNRsin)  # Create line at error point
idx = np.argwhere(np.diff(np.sign(Cutoff - aveAo))).flatten()

plt.plot(aveSNRsin, aveAo, 'k-', label="Output curve")
plt.plot(aveSNRsin, Cutoff, 'r--', label="0.1% error")
plt.plot(aveSNRsin[idx], aveAo[idx], 'r.', label="0.1% error")
plt.grid(True)
plt.xlabel("SNR [db]")
plt.ylabel("Ouput [V]")
plt.legend()
plt.show()
