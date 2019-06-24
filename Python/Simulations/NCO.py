import numpy as np
import matplotlib.pyplot as plt
from scipy import signal


class NCO():

    def __init__(self, period, sample_period, LUT_size):
        self.LUT_size = LUT_size

        self.i = np.arange(0, LUT_size)
        sine_LUT = np.int16(32767 * np.sin(2 * np.pi * self.i / LUT_size))
        self.sine_LUT = np.append(sine_LUT, 0)
        np.savetxt('C:/Users/senag/Documents/Project_SRON/Output_files/pynco.out', self.sine_LUT, fmt='%X', delimiter=',')
        self.period = np.uint32(period / 100)
        self.accumulator = np.uint32(0)
        self.sample_period = np.uint32(sample_period / 100)
        self.output = self.sine_LUT[0]
        self.int_output = self.sine_LUT[0]

        self.int_bits = np.log2(self.LUT_size)
        self.frac_bits = 16

        self.set()

    def set(self, period):
        self.period = period
        norm = np.uint32(2 ** (self.int_bits + self.frac_bits) / self.period)
        self.increment = np.uint64(norm * self.sample_period)

    def interpolate(self):
        index = np.uint16(self.accumulator >> 16)
        fraction = np.uint16(0xFFFF & self.accumulator)

        out1 = np.int16(self.sine_LUT[index])
        out2 = np.int16(self.sine_LUT[index + 1])

        diff = np.int16(out2 - out1)

        outfrac = np.int32(diff * fraction)

        outfrac = outfrac >> 16

        self.int_output = np.int16(out1 + outfrac)

    def step(self):
        self.interpolate()
        self.output = self.sine_LUT[self.accumulator >> 16]
        self.accumulator += np.uint32(self.increment)
        self.accumulator = int((2 ** (self.int_bits + self.frac_bits)) - 1) & self.accumulator


if __name__ == "__main__":
    plt.close("all")
    Fs = 10000  # Sample frequency (Hz)
    Fo = 200  # Desired Frequency (Hz)

    Ts = (1000000000 / Fs) / 5  # Sample period (PRU cycles: 5ns)
    To = (1000000000 / Fo) / 5  # Desired period (PRU cycles: 5ns)

    T = 2000 * (To / 100000000)  # periods
    n = int(T * Fs)
    t = np.linspace(0, T, n, endpoint=False)

    py_output = np.int16(32767 * np.sin((t * 2 * np.pi * Fo)))  # Generate perfect sine for comparison

    output = []  # Output array
    int_output = []
    accumulator = []  # Accumulator array

    DDS = NCO(To, Ts, 1024)  # Create NCO object
    DDS.set()  # Set the step-size (Incrementor)

    NCOres = (Fs/100) / 2 ** (DDS.int_bits + DDS.frac_bits)  # Calculate the NCO resultion based on parameters

    for i in t:
        DDS.step()
        output.append(DDS.output)
        int_output.append(DDS.int_output)
        accumulator.append(DDS.accumulator)
        # print("Output: %d, Accumulator: %d, Incrementor: %d" % (DDS.output, DDS.accumulator >> 16, int(DDS.increment) >> 16))

    print("output size: %d" % (len(output)))
    output = np.array(output)

    plt.figure()
    plt.plot(output, 'r.')
    plt.plot(int_output, 'b.')

    plt.figure()
    plt.subplot(2, 1, 1)
    spec, freq, line = plt.magnitude_spectrum(output, Fs=Fs, color='blue', scale='dB')
    peak, _ = signal.find_peaks(spec, height=10000)
    plt.plot(freq[peak], 20 * np.log10(spec[peak]), 'x')
    plt.ylim(-150, 150)
    # plt.xlim(0, 500)
    plt.subplot(2, 1, 2)
    plt.magnitude_spectrum(int_output, Fs=Fs, scale='dB')
    plt.ylim(-150, 150)
    # plt.xlim(0, 500)

    print(" Fout: %d\n Fout(Real): %f\n Fs: %d\n LUT size: %d\n NCO resulution: %lf\n" % (Fo, freq[peak], Fs, DDS.LUT_size, NCOres))
