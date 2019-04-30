import struct
import time
import numpy as np


class PRU_ICSS:
    # Class atributes
    RPMSG_BUF_SIZE = 512
    charBuf = "\0" * 512

    def __init__(self, pru):
        self.pru = pru
        if self.pru == 0:
            self.CHAR_DEV = "/dev/rpmsg_pru30"
            self.STATE = "/sys/class/remoteproc/remoteproc1/state"
        elif self.pru == 1:
            self.CHAR_DEV = "/dev/rpmsg_pru31"
            self.STATE = "/sys/class/remoteproc/remoteproc2/state"
        self.PRU_status = open(self.STATE, "r+")

    def status(self):
        state = self.PRU_status.read(7)
        return state

    def start(self):
        state = self.status()
        if 'running' in state:
            print("-    PRU%d is running" % (self.pru))
        elif 'offline' in state:
            print("-    PRU%d is offline, starting now" % (self.pru))
            try:
                self.PRU_status.write('start')
                print("-    PRU%d has started" % (self.pru))
                time.sleep(2)
            except IOError:
                print("-  ERROR  PRU%d failed to start" % (self.pru))

    def stop(self):
        if 'offline' in self.status():
            print("-    PRU%d is offline" % (self.pru))
        elif 'offline' in self.status():
            print("-    PRU%d is running, stopping now" % (self.pru))
            try:
                self.PRU_dev.close()
                print("-    Closing character device")
                # Delay for a few seconds so the kernel can flush the remoteproc FIFO
                time.sleep(2)
                self.PRU_status.write('stop')
                print("-    PRU%d has stopped" % (self.pru))
                self.PRU_status.close()
                time.sleep(2)
            except IOError:
                print("-  ERROR  PRU%d failed to stop" % (self.pru))
                self.PRU_status.close()

    def transmit(self, message):
        try:
            self.PRU_dev = open(self.CHAR_DEV, "rb+", 0)
            self.PRU_dev.write(bytes(str(message), 'ASCII'))
            print('-    "%s" transmitted to PRU%d' % (message, self.pru))
        except IOError:
            print("-  ERROR  Could not open device: 'rpmsg_pru30'")
        pass

    def blkreceive(self, samples, data_size):
        fullBuf = np.empty(0)
        div = data_size / 8
        num = 496 / div
        tot = np.ceil(samples / num)
        self.PRU_dev.write(bytes(str(tot), 'ASCII'))
        print("-    %d samples will be transfered in %d packets" % (samples, tot))
        for i in range(np.uint16(tot)):
            charBuf = self.PRU_dev.read(self.RPMSG_BUF_SIZE)
            if data_size == 16:
                intBuf = np.asarray(struct.unpack('<248H', charBuf))
            elif data_size == 32:
                intBuf = np.asarray(struct.unpack('<124H', charBuf))
            fullBuf = np.append(fullBuf, intBuf)
            print("\r-    RPMsg packet received ( %d / %d )" % ((i + 1), tot), end='')
        return fullBuf

    def receive(self):
        charBuf = self.PRU_dev.read(self.RPMSG_BUF_SIZE)
        intBuf = np.asarray(struct.unpack('<248H', charBuf))
        print("-    Packter received from PRU%d" % (self.pru))
        return intBuf
