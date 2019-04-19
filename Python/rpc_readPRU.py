import rpyc
import struct
import sys
import time
import os.path


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
            except IOError:
                print("-  ERROR  PRU0 failed to start")
                self.PRUstate.close()
                sys.exit()
        pass

    def on_disconnect(self, conn):
        self.PRUstate = open(self.REMOTEPROC_STATE0, "r+")
        try:
            self.PRUstate.write('stop')
            self.PRUstate.close()
            sys.exit()
        except IOError:
            print("-  ERROR  PRU0 failed to stop")
            self.PRUstate.close()
            sys.exit()
        pass

    def exposed_pru_communicate(self, samp_rate):
        try:
            self.PRUdev = open(self.CHAR_DEV0, "rb+", 0)
            self.PRUdev.write(bytes(samp_rate, 'ASCII'))
            print("-    Communication established")
        except IOError:
            print("-  ERROR  Could not open device: 'rpmsg_pru30'")
            sys.exit()

    def exposed_pru_read(self):
        charBuf = self.PRUdev.read(self.RPMSG_BUF_SIZE)
        intBuf = struct.unpack('<248H', charBuf)
        return intBuf

    def exposed_redirect(self, stdout):
        sys.stdout = stdout

    def exposed_restore(self):
        sys.stdout = sys.__stdout__


if __name__ == '__main__':
    from rpyc.utils.server import ThreadedServer
    t = ThreadedServer(BeagleBoneDDS, port=18861)
    t.start()
