import rpyc
import struct
import time
import numpy as np
import sys

rpyc.core.protocol.DEFAULT_CONFIG['allow_pickle'] = True


class BeagleBoneDDS(rpyc.Service):

    RPMSG_BUF_SIZE = 512
    charBuf = "\0" * 512

    def on_connect(self, conn):
        print("-    Connection has been established")
        pass

    def on_disconnect(self, conn):
        print("\n-    Connection has been terminated")
        t.close()
        pass

    def exposed_pru_initialize(self, pru):
        self.pru = pru
        if pru == 0:
            self.CHAR_DEV = "/dev/rpmsg_pru30"
            self.STATE = "/sys/class/remoteproc/remoteproc1/state"
        elif pru == 1:
            self.CHAR_DEV = "/dev/rpmsg_pru31"
            self.STATE = "/sys/class/remoteproc/remoteproc2/state"

        self.PRUstate = open(self.STATE, "r+")

    def exposed_pru_start(self):
        state = self.PRUstate.read(7)
        if 'running' in state:
            print("-    PRU%d is running" % (self.pru))

        elif 'offline' in state:
            print("-    PRU%d is offline, starting now" % (self.pru))
            try:
                self.PRUstate.write('start')
                print("-    PRU%d has started" % (self.pru))
                self.PRUstate.close()
                time.sleep(2)
            except IOError:
                print("-  ERROR  PRU%d failed to start" % (self.pru))
                self.PRUstate.close()
                t.close()
        pass

    def exposed_pru_stop(self):
        state = self.PRUstate.read(7)
        if 'offline' in state:
            print("-    PRU%d is offline" % (self.pru))

        elif 'offline' in state:
            print("-    PRU%d is running, stopping now" % (self.pru))
            try:
                self.PRUdev.close()
                # Delay for a few seconds so the kernel can flush the remoteproc FIFO
                time.sleep(2)
                self.PRUstate.write('stop')
                print("-    PRU%d has stopped" % (self.pru))
                self.PRUstate.close()
                time.sleep(2)
            except IOError:
                print("-  ERROR  PRU%d failed to stop" % (self.pru))
                self.PRUstate.close()
                t.close()
        pass

    def exposed_pru_transmit(self, message):
        try:
            self.PRUdev = open(self.CHAR_DEV, "rb+", 0)
            self.PRUdev.write(bytes(str(message), 'ASCII'))
            print('-    "%s" transmitted to PRU%d' % (message, self.pru))
        except IOError:
            print("-  ERROR  Could not open device: 'rpmsg_pru30'")
            t.close()
        pass

    def exposed_pru_blk_receive(self, samples):
        fullBuf = np.empty(0)
        tot = np.ceil(samples / 248)
        self.PRUdev.write(bytes(str(tot), 'ASCII'))
        print("-    %d samples will be transfered in %d packets" % (samples, tot))
        for i in range(np.uint16(tot)):
            charBuf = self.PRUdev.read(self.RPMSG_BUF_SIZE)
            intBuf = np.asarray(struct.unpack('<248H', charBuf))
            fullBuf = np.append(fullBuf, intBuf)
            print("\r-    RPMsg packet received ( %d / %d )" % ((i + 1), tot), end='')
        return fullBuf

    def exposed_pru_receive(self):
        charBuf = self.PRUdev.read(self.RPMSG_BUF_SIZE)
        intBuf = np.asarray(struct.unpack('<248H', charBuf))
        print("-    One RPMsg packet received")
        return intBuf



if __name__ == '__main__':
    from rpyc.utils.server import ThreadedServer
    t = ThreadedServer(BeagleBoneDDS, port=18861)
    t.start()
