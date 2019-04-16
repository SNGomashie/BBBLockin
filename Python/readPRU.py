#!/usr/bin/python

import sys

# Character device PRU0
CHAR_DEV0 = "/dev/rpmsg_pru30"

# Sysfs interface for PRU0&PRU1
REMOTEPROC_STATE0 = "/sys/class/remoteproc/remoteproc1/state"
REMOTEPROC_FIRM0 = "/sys/class/remoteproc/remoteproc1/firmware"


def main():
    # Initialize PRUs
    PRUstate = open(REMOTEPROC_STATE0, "r+")
    print(PRUstate.read(7))
    if 'running' in PRUstate.read(7):
        print("PRU0 is running")
        PRUstate.close()
        # Start communication over rpmsg
        try:
            PRUdev = open(CHAR_DEV0, "r+", 0)
            print("Sending message to start communication")
            PRUdev.write('S')
            print("Communication established")
            samp_rate = input("Set sample rate: ")
            PRUdev.write(samp_rate)
            num_samp = input("Set number of samples: ")
            PRUdev.write(num_samp)
            print("Sample rate & number of samples is set")
        except:
            print("Could not open device: 'rpmsg_pru30'")
            PRUdev.close()
            sys.exit()

    elif 'offline' in PRUstate.read(7):
        print("PRU0 is offline, starting now")
        try:
            PRUstate.write(b'start')
            print("PRU0 is being started")
            PRUstate.close()
        except:
            print("PRU0 failed to start")
            PRUstate.close()
            sys.exit()


# Receive several messages over rpmsg

# Stop PRU

# FFT

# Upload image to imgur
if __name__ == '__main__':
    main()