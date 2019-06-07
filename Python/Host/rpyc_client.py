import rpyc
import numpy as np
import matplotlib.pyplot as plt

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


def transfertype(type):
    if 'int16' == type:
        bits = 16
        type = 16
    elif 'int32' == type:
        bits = 32
        type = 32
    elif '3int16' == type:
        bits = 48
        type = 48
    elif '2int32' == type:
        bits = 62
        type = 62
    return bits, type


def format(type, neg, data):
    data = np.asarray(data)
    n = 0
    if type == 62:
        leng = int(len(data)/2)
        Qa = np.zeros(leng)
        In = np.zeros(leng)
        for i in range(leng):
            Qa[i] = data[n]
            In[i] = data[(n+1)]
            n += 2
        for i, x in enumerate(Qa):
            Qa[i] = twos_comp(int(x), 32)
        for i, x in enumerate(In):
            In[i] = twos_comp(int(x), 32)
        return Qa, In
    elif type == 48:
        leng = int(len(data)/3)
        adc = np.zeros(leng)
        sin = np.zeros(leng)
        cos = np.zeros(leng)
        for i in range(leng):
            adc[i] = data[n]
            sin[i] = data[(n+1)]
            cos[i] = data[(n+2)]
            n += 3
        for i, x in enumerate(adc):
            adc[i] = twos_comp(int(x), 16)
        for i, x in enumerate(sin):
            sin[i] = twos_comp(int(x), 16)
        for i, x in enumerate(cos):
            cos[i] = twos_comp(int(x), 16)
        return adc, sin, cos
    elif type == 32 or type == 16:
        if neg == 1:
            for i, x in enumerate(data):
                data[i] = twos_comp(int(x), type)
        return data


if __name__ == "__main__":
    Sample_frequency = 4000
    frequency = 100
    period = 1 / frequency

    T = 2 * period
    t = np.linspace(0, T, int(T * Sample_frequency), endpoint=False)

    samples = len(t)
    bits, type = transfertype("int16")

    command = command(Sample_frequency, samples, bits)

    c = rpyc.connect("192.168.7.2", 18861)  # Connect to RpyC server
    c.root.pru_initialize(2)  # Create objects for both PRUs
    c.root.pru_start(2)  # Start both PRUs
    c.root.pru_transmit(command, 0)
    result = c.root.pru_blk_receive(samples, bits, type, 0)  # Receive a number of samples

    if type == 62:
        Qa, In = format(type, 1, result)
        np.savetxt('C:/Users/senag/Documents/Project_SRON/Output_files/Qa_out.out', Qa[:samples])  # Save results in file
        np.savetxt('C:/Users/senag/Documents/Project_SRON/Output_files/In_out.out', In[:samples])  # Save results in file
        plt.figure()
        plt.plot(Qa[:samples])
        plt.plot(In[:samples])
        plt.figure()
        plt.magnitude_spectrum(Qa[:samples], Fs=Sample_frequency, color='red', scale='dB')
        plt.magnitude_spectrum(In[:samples], Fs=Sample_frequency, color='blue', scale='dB')
    elif type == 48:
        adc, sin, cos = format(type, 1, result)
        np.savetxt('C:/Users/senag/Documents/Project_SRON/Output_files/adc_out.out', adc[:samples])  # Save results in file
        np.savetxt('C:/Users/senag/Documents/Project_SRON/Output_files/sin_out.out', sin[:samples])  # Save results in file
        np.savetxt('C:/Users/senag/Documents/Project_SRON/Output_files/cos_out.out', cos[:samples])  # Save results in file
    elif type == 16 or type == 32:
        data = format(type, 1, result)
        plt.figure()
        plt.plot(data)
        # np.savetxt('C:/Users/senag/Documents/Project_SRON/Output_files/cos_out.out', data[:samples])  # Save results in file

    c.close()
