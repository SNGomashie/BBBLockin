import rpyc
import struct
import time
import numpy as np

rpyc.core.protocol.DEFAULT_CONFIG['allow_pickle'] = True


class BeagleBoneDDS(rpyc.Service):
    # Character device PRU0
    CHAR_DEV0 = "/dev/rpmsg_pru30"

    # Sysfs interface for PRU0&PRU1
    REMOTEPROC_STATE0 = "/sys/class/remoteproc/remoteproc1/state"

    RPMSG_BUF_SIZE = 512
    charBuf = "\0" * 512

    def on_connect(self, conn):
        # Initialize PRUs
        self.PRUstate = open(self.REMOTEPROC_STATE0, "r+")
        state = self.PRUstate.read(7)
        if 'running' in state:
            print("-    PRU0 is running")

        elif 'offline' in state:
            print("-    PRU0 is offline, starting now")
            try:
                self.PRUstate.write('start')
                print("-    PRU0 is being started")
                self.PRUstate.close()
                time.sleep(2)
            except IOError:
                print("-  ERROR  PRU0 failed to start")
                self.PRUstate.close()
                t.close()
        pass

    def on_disconnect(self, conn):
        self.PRUstate = open(self.REMOTEPROC_STATE0, "r+")
        try:
            self.PRUstate.write('stop')
            self.PRUstate.close()
            t.close()
        except IOError:
            print("-  ERROR  PRU0 failed to stop")
            self.PRUstate.close()
            t.close()
        pass

    def exposed_pru_close(self):
        self.PRUstate = open(self.REMOTEPROC_STATE0, "r+")
        try:
            self.PRUstate.write('stop')
            self.PRUstate.close()
        except IOError:
            print("-  ERROR  PRU0 failed to stop")
            self.PRUstate.close()

    def exposed_close_server(self):
        try:
            t.close()
        except EOFError:
            print("connection lost")
    
    def exposed_pru_communicate(self, samp_rate):
        try:
            self.PRUdev = open(self.CHAR_DEV0, "rb+", 0)
            self.PRUdev.write(bytes(samp_rate, 'ASCII'))
            print("-    Communication established")
        except IOError:
            print("-  ERROR  Could not open device: 'rpmsg_pru30'")
            t.close()

    def exposed_pru_read(self, samples):
        fullBuf = np.empty(0)
        tot = np.ceil(samples / 248)
        for i in range(np.uint16(tot)):
            charBuf = self.PRUdev.read(self.RPMSG_BUF_SIZE)
            intBuf = np.asarray(struct.unpack('<248H', charBuf))
            fullBuf = np.append(fullBuf, intBuf)
            print("\r-    rpmsg packet received ( %d / %d )" % ((i + 1), tot), end='')
        self.exposed_pru_close()
        return fullBuf


if __name__ == '__main__':
    from rpyc.utils.server import ThreadedServer
    t = ThreadedServer(BeagleBoneDDS, port=18861)
    t.start()
