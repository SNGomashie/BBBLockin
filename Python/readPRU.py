#!/usr/bin/python

import struct
import os.path
import sys
import pastebin

# Character device PRU0
CHAR_DEV0 = "/dev/rpmsg_pru30"

# Sysfs interface for PRU0&PRU1
REMOTEPROC_STATE0 = "/sys/class/remoteproc/remoteproc1/state"
REMOTEPROC_FIRM0 = "/sys/class/remoteproc/remoteproc1/firmware"

RPMSG_BUF_SIZE = 512
charBuf = "\0" * 512

DATA = "data.txt"

def main():
    # Initialize PRUs
    PRUstate = open(REMOTEPROC_STATE0, "r+")
    state = PRUstate.read(7)
    if 'running' in state:
        print("-    PRU0 is running")

    elif 'offline' in state:
        print("-    PRU0 is offline, starting now")
        try:
            PRUstate.write('start')
            print("-    PRU0 is being started")
        except IOError:
            print("-  ERROR  PRU0 failed to start")
            PRUstate.close()
            sys.exit()

# Start communication over rpmsg
    try:
        data = os.open(DATA, os.O_CREAT | os.O_APPEND | os.O_RDWR)
        PRUdev = open(CHAR_DEV0, "rb+", 0)
        samp_rate = input("-    Set sample rate: ")
        PRUdev.write(bytes(samp_rate, 'ASCII'))
        print("-    Communication established")
    except IOError:
        print("-  ERROR  Could not open device: 'rpmsg_pru30'")
        PRUstate.close()
        PRUdev.close()
        sys.exit()


# Receive several messages over rpmsg
    while(1):
        try:
            charBuf = PRUdev.read(RPMSG_BUF_SIZE)
            intBuf = struct.unpack('<248H', charBuf)
            print(intBuf)
            print("Datatype is= %d" % (type(intBuf)))
        except KeyboardInterrupt:
            try:
                PRUstate.write('stop')
                PRUstate.close()
                PRUdev.close()
                print("PRU is offline")
                sys.exit()
            except IOError:
                print("-  ERROR  Could not stop PRU")
                sys.exit()


# Upload image to imgur
if __name__ == '__main__':
    main()
