import rpyc
import pru_icss
from Agilent33522A import FunctionGenerator
import sys

rpyc.core.protocol.DEFAULT_CONFIG['allow_pickle'] = True
rpyc.core.protocol.DEFAULT_CONFIG['sync_request_timeout'] = 120


class BeagleBoneRpyC(rpyc.Service):

    def on_connect(self, conn):
        print("-    Connection has been established")

    def on_disconnect(self, conn):
        try:
            self.PRUSS.close_char(0)
            self.PRUSS.close_char(1)
            self.PRUSS.stop(0)
            self.PRUSS.stop(1)
            print("\n-      Connection has been terminated")
            t.close()
            sys.exit()
        except IOError:
            print("-    Unable to terminate connection")
        pass

    def exposed_fgen_initialize(self, settings=""):
        try:
            self.FGEN = FunctionGenerator(1)
            print("-    %s" % (self.FGEN.getIdn()))
            print("-    Function generator is ready for usage")
        except IOError:
            print("-    Failed to find USB instrument")
        if settings == "":
            pass
        else:
            self.FGEN.loadSettings(settings)

    def exposed_fgen_write(self, SCPI):
        try:
            self.FGEN.write(SCPI)
            print("-    SCPI command send: %s" % SCPI)
        except IOError:
            print("-    Failed to write command")

    def exposed_pru_initialize(self):
        try:
            self.PRUSS = pru_icss.PRU_ICSS()
            print("-    PRU-ICSS object has been created")
        except IOError:
            print("-    Failed to initialize PRU-ICSS")
        pass

    def exposed_pru_start(self, pru):
        self.PRUSS.start(pru)
        pass

    def exposed_pru_stop(self, pru):
        self.PRUSS.stop(pru)
        pass

    def exposed_pru_transmit(self, pru, message):
        self.PRUSS.transmit(pru, message)
        pass

    def exposed_pru_receive(self, pru, type, samples=0):
        return self.PRUSS.receive(pru, type, samples)
        del self.FGEN
        pass

    def exposed_pru_open(self, pru):
        self.PRUSS.open_char(pru)
        pass

    def exposed_pru_close(self, pru):
        self.PRUSs.close_char(pru)
        pass


if __name__ == '__main__':
    from rpyc.utils.server import ThreadedServer
    t = ThreadedServer(BeagleBoneRpyC, port=18861)
    t.start()
