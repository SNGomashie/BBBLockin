#!/usr/bin/python

import os
import argparse
import time

def readADCchannel(adcChannel):
    outputFile = "/dev/rpmsg_pru30"
    try:
        dev = os.open(outputFile, os.O_RDWR)
    except:
        print("Failed to open character device")
        return
    os.write(dev, b'5')
    return_voltage = os.read(dev, 4)
    os.close(dev)
    return int.from_bytes(return_voltage, byteorder='big')

def convertVoltage(rawVoltage):
    finalVoltage = (rawVoltage * 1.7)/(4096 - 1)
    return str(finalVoltage)

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

time.sleep(2)

print("Reading voltage at ADC channel: "+ readADCchannel(args.channel))
