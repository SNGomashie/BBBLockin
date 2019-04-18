#!/usr/bin/python

import sys

# Character device PRU0
CHAR_DEV0 = "/dev/rpmsg_pru30"

# Sysfs interface for PRU0&PRU1
REMOTEPROC_STATE0 = "/sys/class/remoteproc/remoteproc1/state"
REMOTEPROC_FIRM0 = "/sys/class/remoteproc/remoteproc1/firmware"

RPMSG_BUF_SIZE = 512
readBuf = "\0" * 512


def main():
    # Initialize PRUs
    PRUstate = open(REMOTEPROC_STATE0, "r+")
    state = PRUstate.read(7)
    if 'running' in state:
        print("PRU0 is running")
        PRUstate.close()

    elif 'offline' in state:
        print("PRU0 is offline, starting now")
        try:
            PRUstate.write('start')
            print("PRU0 is being started")
            PRUstate.close()
        except IOError:
            print("PRU0 failed to start")
            PRUstate.close()
            sys.exit()


# Start communication over rpmsg
    try:
        PRUdev = open(CHAR_DEV0, "rb+", 0)
        samp_rate = input("Set sample rate: ")
        print(":".join("{:02x}".format(ord(c)) for c in samp_rate))
        PRUdev.write(bytes(samp_rate, 'ASCII'))
        print("Communication established")
        # PRUdev.close()
        # sys.exit()
        # samp_rate = input("Set sample rate: ")
        # PRUdev.write(samp_rate.encode())
        # num_samp = input("Set number of samples: ")
        # PRUdev.write(num_samp.encode())
        # print("Sample rate & number of samples is set")
    except IOError:
        print("Could not open device: 'rpmsg_pru30'")
        PRUdev.close()
        sys.exit()


# Receive several messages over rpmsg
    while(1):
        readBuf = PRUdev.read(RPMSG_BUF_SIZE)
        print(readBuf)

# Stop PRU

# FFT


# Upload image to imgur
if __name__ == '__main__':
    main()
