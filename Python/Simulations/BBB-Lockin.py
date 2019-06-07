import numpy as np
import matplotlib.pyplot as plt
from scipy import signal


def plot(x):
    plt.figure()
    plt.plot(x)


def quadrature(sin, cos, adc):
    Qa = (sin * adc) >> 14
    In = (cos * adc) >> 14
    return Qa, In


def sample_data():

    ADC = np.int16(np.genfromtxt('C:/Users/senag/Documents/Project_SRON/Output_files/adc_out.out', delimiter='\n', unpack=True, dtype=float))
    # NCO data from PRU-ICSS
    SIN = np.int16(np.genfromtxt('C:/Users/senag/Documents/Project_SRON/Output_files/sin_out.out', delimiter='\n', unpack=True, dtype=float))

    COS = np.int16(np.genfromtxt('C:/Users/senag/Documents/Project_SRON/Output_files/cos_out.out', delimiter='\n', unpack=True, dtype=float))

    # Qa = np.int16(np.genfromtxt('C:/Users/senag/Documents/Project_SRON/Output_files/Q_out.out', delimiter='\n', unpack=True, dtype=float))
    # In = np.int16(np.genfromtxt('C:/Users/senag/Documents/Project_SRON/Output_files/I_out.out', delimiter='\n', unpack=True, dtype=float))
    return ADC, SIN, COS


def decimate(Qa, In, M):
    Qa_filt = signal.decimate(Qa, M)
    In_filt = signal.decimate(In, M)
    return Qa_filt, In_filt

if __name__ == "__main__":
    ADC, SIN, COS = sample_data()
    Qa, In = quadrature(SIN, COS, ADC)
    Ao = 2 * np.sqrt((np.mean(Qa)**2)+(np.mean(In)**2))
    print("Ao: %d, Qa: %d, In: %d" % (Ao, np.mean(Qa), np.mean(In)))
    plot(Qa)
    plot(In)
