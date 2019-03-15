
import numpy as np
import matplotlib.pyplot as plt

# define the configuration
# Fs = np.array([8000, 16000, 32000, 64000, 80000, 128000])  # sampling frequency
Fs = np.array([200, 600])
Aref = 1  # Reference amplitude
Ain = 1.8  # Input amplitude
# Fr = np.arange(50, 300, 50)  # Refernce frequency
Fr = np.array([50, 100, 200, 300, 400, 500, 600])
errIncr = 0.01  # Error percentage allowed

# Quantizition
quant_bits = 16
quant_levels = np.power(2, quant_bits) / 2
quant_step = 1 / quant_levels

# Amount of random noise samples to be averaged
Nsamples = np.arange(0, 100, 1)

# Amount of noise levels
stepn = -1
Nlevel = np.logspace(stepn, 2, 100)

# Variables
aveSNRFr = np.zeros(len(Fr))
aveAerrFr = np.zeros(len(Fr))
aveAoFr = np.zeros(len(Fr))
FsvFr = np.zeros((len(Fr), len(Fs)))
SNRFr = np.zeros(len(Fr))
Aolim = np.zeros(1)

q = 0
for s in Fs:
    j = 0
    for r in Fr:
        dt = 1 / s
        pt = np.arange(0, 4 / r, dt)

        # Experimental signal (Sinusoid)
        # Added phase difference
        VsigCos = Ain * np.cos((pt * 2 * np.pi * r) - (np.pi / 10))
        VsigCos16 = np.uint16(VsigCos)
        # Reference signal(square wave) ( Also uncomment Ao)
        # VrefCos = Ar * signal.square( n * 2 * np.pi * fo)
        # VrefSin = Ar * signal.square(( n * 2 * np.pi * fo) - (np.pi/2))

        # Reference signals(sinus and cosinus)
        VrefCos = Aref * np.cos(pt * 2 * np.pi * r)
        # Cosinus phase shifted to create Sinus
        VrefSin = Aref * np.cos((pt * 2 * np.pi * r) - (np.pi / 2))

        Ao = np.zeros(len(Nsamples))
        SNR = np.zeros(len(Nsamples))
        Aerr = np.zeros(len(Nsamples))
        aveSNRsamp = np.zeros(len(Nlevel))
        aveAerrsamp = np.zeros(len(Nlevel))
        aveAosamp = np.zeros(len(Nlevel))

        i = 0
        for l in Nlevel:
            for p in Nsamples:

                # Noise generation
                np.random.seed(p + 45)
                Expnoise = l * np.random.randn(len(pt), 1)

                # SNR
                npwr = np.sum(np.power(Expnoise, 2))
                sigpwr = np.sum(np.power(VsigCos, 2))

                # Input + Noise
                VsigCosandNoise = VsigCos + Expnoise

                # Make a 16bit number
                VsigCosandNoise16 = np.round(VsigCosandNoise / quant_step) * quant_step

                # Convet back to double
                VsigCosandNoiseQ = np.float64(VsigCosandNoise16)

                # Multiply signal with reference Sin and Cos
                Vc = VsigCosandNoiseQ * VrefCos
                Vs = VsigCosandNoiseQ * VrefSin

                # calculate RMS
                Ivs = np.mean(Vs)
                Qvc = np.mean(Vc)

                # Find magnitude (Square wave)
                # Ao[k] = (np.pi/4) * 2 * np.sqrt((np.power(Ivs, 2))+(np.power(Qvc, 2)));

                # Find magnitude (Sinusoid)
                Ao[p] = 2 * np.sqrt((np.power(Ivs, 2)) + (np.power(Qvc, 2)))

                # Find phase
                Phase = np.arctan2(Ivs, Qvc)

                SNR[p] = 10 * np.log10((sigpwr) / (npwr))
                Aerr[p] = Ao[p] - Ain

            aveSNRsamp[i] = np.mean(SNR)  # Average SNR of 1000 samples at l noise level
            # Average error of 1000 samples at i noise level
            aveAerrsamp[i] = np.mean(np.power(Aerr, 2))
            aveAosamp[i] = np.mean(Ao)  # Average amplitude of 1000 samples at l noise level

        print("SNR = %2.4f\nFsample = %d\nFreference = %d\n" % Aolim, s, r)
