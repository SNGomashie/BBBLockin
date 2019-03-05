import os
import argparse

parser = argparse.ArgumentParser(description='Select ADC channel(5/6): ')
parser.add_argument("channel")
args = parser.parse_args()
remoteproc_state = "/sys/class/remoteproc/remoteproc1/state"
state = os.open(remoteproc_state, os.O_RDWR)
cur_state = os.read(state, 7)
if b'offline' in cur_state:
    try:
        os.write(state, b"start")
        print("pru is online")
    except:
        print("failed to start device")
else:
    print("pru is online")

print("Reading voltage at ADC channel: "+ args.channel)
Voltage = convertVoltage(readADCchannel(args.channel))
print("Voltage on ADC channel 5 is: " + Voltage)


def readADCchannel(adcChannel):
    outputFile = "/dev/rpmsgpru30"
    try:
        dev = os.open(CHAR_DEV0, os.O_RDWR)
    except:
        print("Failed to open character device")
        return
    os.write(dev, b'5')
    return_voltage = os.read(dev, 4)
    os.close(dev)
    return return_voltage

def convertVoltage(rawVoltage):
    finalVoltage = (rawVoltage * 1.8)/(4096 - 1)
    return finalVoltage
