import numpy as np
from scipy import signal
import matplotlib.pyplot as plt



# define the configuration
fs = 16000 # sampling frequency
dt = 1/fs # step time
Ar = 1 # Reference amplitude
Ai = 1.8 # Input amplitude
n = np.arange(0, 0.025, dt) # Time axis
fo = 100 # Refernce frequency
errIncr = 0.02 # Error percentage allowed

#Quantizition
quant_bits = 16
quant_levels = np.power(2,quant_bits) / 2
quant_step = 1 / quant_levels

# Experimental signal (Sinusoid)
VsigCos = Ai * np.cos((n * 2 * np.pi * fo)-(np.pi/10)) # Added phase difference

# Reference signal(square wave)
# VrefCos = Ar * signal.square( n * 2 * np.pi * fo)
# VrefSin = Ar * signal.square(( n * 2 * np.pi * fo) - (np.pi/2))

# Reference signals(sinus and cosinus)
VrefCos = Ar * np.cos(n * 2 * np.pi * fo)
VrefSin = Ar * np.cos((n * 2 * np.pi * fo) - (np.pi/2)) # Cosinus phase shifted to create Sinus

# Amount of random noise samples to be averaged
kmax = np.arange(0,100,1)

# Amount of noise levels
stepn = -1
x = np.logspace(stepn,2,100)

Ao = np.zeros(len(kmax))
SNR = np.zeros(len(kmax))
Aerr = np.zeros(len(kmax))
aveSNRsin = np.zeros(len(x))
aveAerrsin = np.zeros(len(x))
aveAo = np.zeros(len(x))
Aolimstore = np.zeros(len(x))
Aerrlimstore = np.zeros(len(x))
Aolim = np.zeros(len(x))
Aerrlim = np.zeros(len(x))

i = 0

for a in x:
    for k in kmax:
        # Noise generation
        np.random.seed(k+45)
        Expnoise = a * np.random.randn(len(n), 1)

        # SNR
        npwr = np.sum(np.power(Expnoise,2))
        sigpwr = np.sum(np.power(VsigCos,2))

        # Input + Noise
        VsigCosandNoise = VsigCos + Expnoise

        # Make a 16bit number
        VsigCosandNoise16 = np.round(VsigCosandNoise / quant_step) * quant_step

        #Convet back to double
        VsigCosandNoiseQ = np.float64(VsigCosandNoise16)

        # Multiply signal with reference Sin and Cos
        Vc = VsigCosandNoiseQ * VrefCos
        Vs = VsigCosandNoiseQ * VrefSin

        # calculate RMS
        Ivs = np.mean(Vs)
        Qvc = np.mean(Vc)

        # Find magnitude
        Ao[k] = 2 * np.sqrt((np.power(Ivs, 2))+(np.power(Qvc, 2)))

        # print("Input amplitude = " + str(Ai))
        # print("Output amplitude = " + str(Ao))

        # Find phase
        Phase = np.arctan2(Ivs, Qvc)

        # print("Phase difference signal and refenrece = " + str(Phase))

        SNR[k] = 10*np.log10((sigpwr)/(npwr))
        Aerr[k] = Ao[k] - Ai
    # END


    aveSNRsin[i] = np.mean(SNR)
    aveAerrsin[i] = np.mean(np.power(Aerr, 2))
    aveAo[i] = np.mean(Ao)
    i += 1
# END
l = 0
while (aveAo[l] <= (Ai+(Ai*errIncr))):
    Aolim = aveSNRsin[1,l]
    Aerrlim = aveAerrsin[1,l]
    l += 1
Aerrlimstore = Aerrlim
Aolimstore = Aolim
print(Aerrlimstore)
print(Aolimstore)
