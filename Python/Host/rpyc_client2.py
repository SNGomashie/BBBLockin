import rpyc
import numpy as np
import matplotlib.pyplot as plt
from scipy import signal, fftpack

%matplotlib qt5

rpyc.core.protocol.DEFAULT_CONFIG['sync_request_timeout'] = 120


def command(sample_freq, samples, bits):
    bytes = bits / 8
    bytes = 496 / bytes

    packets = np.ceil(samples / bytes)  # Total number of packets to be received
    num = int(packets) << 16
    command = num | sample_freq
    return command


def twos_comp(val, bits):
    """compute the int value of a 2 complement number"""
    if (val & (1 << (bits - 1))) != 0: # if sign bit is set e.g., 8bit: 128-255
        val = val - (1 << bits)        # compute negative value
    return val


def format(data, neg, bits):
    result = np.asarray(data)
    if neg is 0:
        return result
    elif neg is 1:
        neg_data = np.empty(len(data))
        for i, x in enumerate(data):
            neg_data[i] = twos_comp(int(x), bits)
        return neg_data


def fullreceive(data):
    n = 0
    len = int(len(data)/3)
    adc = np.zeros(len)
    sin = np.zeros(len)
    cos = np.zeros(len)
    for i in range(len):
        adc[i] = array[n]
        sin[i] = array[(n+1)]
        cos[i] = array[(n+2)]
        n += 3
    sin_result = format(sin, 1, bits)  # if two complement translate to pythonian
    adc_result = format(adc, 1, bits)
    cos_result = format(cos, 1, bits)
    return sin_result, cos_result, adc_result


def plots(samples, freq, data):
    plt.figure()
    spec, freq, line = plt.magnitude_spectrum(data, Fs=freq, scale='dB', color='C2')
    peak, _ = signal.find_peaks(spec, height=10000)
    plt.plot(freq[peak], 20 * np.log10(spec[peak]), 'x')
    print(freq[peak])
    plt.xlim(0, 500)

    plt.figure()
    plt.plot(data[:samples],)
    # plt.xlim(0, 300)


def transfertype(type):
    if 'int16' in type:
        bits = 16
        type = 16
    elif 'int32' in type:
        bits = 32
        type = 32
    elif '3int16' in type:
        bits = 16
        type = 48
    return bits, type


if __name__ == "__main__":
    Time = 1
    freq = 4000  # Sample frequency (Hz)
    Fo = 100
    To = (1000000000 / Fo) / 5

    T = 10 * (To / 100000000)  # periods
    n = int(T * freq)
    t = np.linspace(0, T, n, endpoint=False)

    samples = len(t)  # Amount of samples
    bits, type = transfertype("3int16")

    c = rpyc.connect("192.168.7.2", 18861)  # Connect to RpyC server
    c.root.pru_initialize(2)  # Create objects for both PRUs
    c.root.pru_start(2)  # Start both PRUs
    c.root.pru_transmit(command(freq, samples, bits), 0)  # Transmit the 'command' to PRU0
    data = c.root.pru_blk_receive(samples, bits, type, 0)  # Receive a number of samples
    array = np.asarray(data[:samples])
    result = format(array, 1, bits)  # if two complement translate to pythonian
    # print("Result size: %d" % (len(result)))
    # for i in result:
    #     print("%d\n" % (i))
    plots(samples, freq, result)  # Plot results
    # np.savetxt('C:/Users/senag/Documents/Project_SRON/Output_files/adc_out.out', adc_result)  # Save results in file
    # np.savetxt('C:/Users/senag/Documents/Project_SRON/Output_files/sin_out.out', sin_result)  # Save results in file
    # np.savetxt('C:/Users/senag/Documents/Project_SRON/Output_files/cos_out.out', cos_result)  # Save results in file
    c.close()  # Close connection to RPC server
