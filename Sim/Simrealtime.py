import numpy as np
import matplotlib.pyplot as plt
from scipy import signal
from collections import deque

In = 0
Qa = 0
R = 0

XIbuf = deque([0, 0, 0, 0, 0], maxlen=5)
XQbuf = deque([0, 0, 0, 0, 0], maxlen=5)
Inbuf = deque([0, 0, 0, 0, 0], maxlen=5)
Qabuf = deque([0, 0, 0, 0, 0], maxlen=5)
Rbuf = deque([0, 0, 0, 0, 0], maxlen=5)
_Rbuf = deque([0, 0, 0, 0, 0], maxlen=5)


# Recursive moving average filter
def moving_average_filter(X, COS, SIN, Navr):
    global In
    global Qa
    global R
    In -= In / Navr
    In += (X * COS) / Navr
    Qa -= Qa / Navr
    Qa -= (X * SIN) / Navr
    R -= R / Navr
    R += 2 * np.sqrt((np.power(In, 2)) + (np.power(Qa, 2))) / Navr
    return R


# Recursive exponentially weighted moving average filter
def EWMA_filter(X, COS, SIN, W):
    global In
    global Qa
    global R
    In = W * (X * COS) + (1 - W) * In
    Qa = W * (X * SIN) + (1 - W) * Qa
    R = W * (2 * np.sqrt((np.power(In, 2)) + (np.power(Qa, 2)))) + (1 - W) * R
    return R


# Butterworth filter initialization
def butterworth_initialize(Fs, cl, ch):
    nyq = 0.5 * Fs
    Fcl = cl / nyq
    Fch = ch / nyq
    N, Wn = signal.buttord(Fcl, Fch, 0.5, 40)
    b, a = signal.butter(N, Wn, btype='low', analog=False)
    return b, a


# Recursive butterworth filter
def butterworth_filter(X, COS, SIN, Fs):
    global XIbuf
    global XQbuf
    global _Rbuf
    global Inbuf
    global Qabuf
    global Rbuf

    b, a = butterworth_initialize(Fs, 0.5, 3)

    XIbuf.appendleft(X * COS)
    XQbuf.appendleft(X * SIN)

    Inbuf.appendleft((a[0] * XIbuf[0]) + (a[1] * XIbuf[1]) + (a[2] * XIbuf[2]) + (a[3] * XIbuf[3]) + (a[4] * XIbuf[4]) + (b[0] * Inbuf[0]) + (b[1] * Inbuf[1]) + (b[2] * Inbuf[2]) + (b[3] * Inbuf[3]) + (b[4] * Inbuf[4]))
    Qabuf.appendleft((a[0] * XQbuf[0]) + (a[1] * XQbuf[1]) + (a[2] * XQbuf[2]) + (a[3] * XQbuf[3]) + (a[4] * XQbuf[4]) + (b[0] * Qabuf[0]) + (b[1] * Qabuf[1]) + (b[2] * Qabuf[2]) + (b[3] * Qabuf[3]) + (b[4] * Qabuf[4]))

    _Rbuf.appendleft(2 * np.sqrt((np.power(Inbuf[0], 2)) + (np.power(Qabuf[0], 2))))

    Rbuf.appendleft((a[0] * _Rbuf[0]) + (a[1] * _Rbuf[1]) + (a[2] * _Rbuf[2]) + (a[3] * _Rbuf[3]) + (a[4] * _Rbuf[4]) + (b[0] * Rbuf[0]) + (b[1] * Rbuf[1]) + (b[2] * Rbuf[2]) + (b[3] * Rbuf[3]) + (b[4] * Rbuf[4]))

    return Rbuf[0]


# Recursive RC filter
def RC_filter(X, COS, SIN, b, a):
    global XIbuf
    global XQbuf
    global _Rbuf
    global Inbuf
    global Qabuf
    global Rbuf

    XIbuf.appendleft(X * COS)
    XQbuf.appendleft(X * SIN)
    In = (a[0] * XIbuf[0]) + (b[0] * Inbuf[0]) + (b[1] * Inbuf[1]) + (b[2] * Inbuf[2]) + (b[3] * Inbuf[3])
    Qa = (a[0] * XQbuf[0]) + (b[0] * Qabuf[0]) + (b[1] * Qabuf[1]) + (b[2] * Qabuf[2]) + (b[3] * Qabuf[3])
    Inbuf.appendleft(In)
    Qabuf.appendleft(Qa)

    _R = 2 * np.sqrt((np.power(Inbuf[0], 2)) + (np.power(Qabuf[0], 2)))
    _Rbuf.appendleft(_R)

    R = (a[0] * _Rbuf[0]) + (b[0] * Rbuf[0]) + (b[1] * Rbuf[1]) + (b[2] * Rbuf[2]) + (b[3] * Rbuf[3])
    Rbuf.appendleft(R)


# Moving average filter parameters
Navr = 1000

# EWMA filter parameters
w = 0.001  # Weight of EWMA

# RC filter parameters
d = 500  # Number of samples to decay to 36.8% of its final value
x = np.exp(-1 / d)
RCa = np.array([np.power((1 - x), 4)])
RCb = np.array([(4 * x), (np.power((-6 * x), 2)), (np.power((4 * x), 3)), (np.power(-x, 4))])

# Frequncies and periods
Fs = 4800  # Hz
Ts = 1 / Fs  # s
Fr = 100  # Hz
Tr = 1 / Fr  # s
# Constants
Ar = 1  # V
Ai = 1.8  # V
P = 1000  # Periods
a = 13.219  # Randomness

# Samples
T = P * Tr  # periods
n = int(T * Fs)
t = np.linspace(0, T, n, endpoint=False)
Ao = np.zeros(len(t))

# Experimental signal
VsigCos = Ai * np.cos((t * 2 * np.pi * Fr) - (np.pi / 10))

# Reference signals(sinus and cosinus)
VrefCos = Ar * np.cos(t * 2 * np.pi * Fr)
VrefSin = Ar * np.cos((t * 2 * np.pi * Fr) - (np.pi / 2))

# Noise generation
np.random.seed(45)
Noise = a * np.random.randn(len(t))

# SNR
npwr = sum(np.power(Noise, 2))
sigpwr = sum(np.power(VsigCos, 2))
SNR = 10 * np.log10((sigpwr) / (npwr))

# Input with Noise
# VsigCosandNoise = VsigCos + Noise

# Input without noise
VsigCosandNoise = VsigCos

i = 0
Qvc, Ivs, R = [0, 0, 0]
for s in VsigCosandNoise:
    # Moving average filter
    # Ao[i] = moving_average_filter(s, VrefCos[i], VrefSin[i], Navr)

    # EWMA filter
    Ao[i] = EWMA_filter(s, VrefCos[i], VrefSin[i], w)

    # Butterworth filter
    # Ao[i] = butterworth_filter(s, VrefCos[i], VrefSin[i], Fs)

    # RC filter
    # Ao[i] = RC_filter(s, VrefCos[i], VrefSin[i], RCb, RCa)
    i += 1

finalSNR = 10 * np.log10(np.mean(Ao[-1000:]) / Ai)

# Print important variables
print("SNR pre-LIA : %.3f dB" % SNR)
print("SNR post-LIA: %.3f dB" % finalSNR)
print(np.mean(Ao[-1000:]))

# Plot frequency reponse
w, h = signal.freqz(RCb, RCa)
plt.semilogx(w, 20 * np.log10(abs(h)), 'b')
plt.title("Lowpass Filter Frequency Response")
plt.xlabel('Frequency [Hz]')
plt.grid()
plt.show()


# Plot FFT of output
plt.title("Magnitude spectrum ")
plt.magnitude_spectrum(VsigCosandNoise, Fs=Fs, color='C2', scale='dB')
plt.magnitude_spectrum(Ao, Fs=Fs, color='C1', scale='dB')
plt.show()

# Plot out and input
plt.title("Filtered signal ")
plt.plot(t, VsigCosandNoise, 'b-', linewidth=2, label='unfilted signal')
plt.plot(t, Ao, 'g-', linewidth=2, label='filtered signal')
plt.xlabel('Time [sec]')
plt.ylabel('Amlitude [Voltage]')
plt.grid()
plt.ylim(0, 2)
plt.legend()
plt.subplots_adjust(hspace=0.35)
plt.show()
