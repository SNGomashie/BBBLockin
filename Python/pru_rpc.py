import rpyc
from pru_icss import PRU_ICSS

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
        if pru == 0:
            self.PRU0 = PRU_ICSS(0)
        if pru == 1:
            self.PRU1 = PRU_ICSS(1)
        if pru == 2:
            self.PRU0 = PRU_ICSS(0)
            self.PRU1 = PRU_ICSS(1)
        pass

    def exposed_pru_start(self, pru):
        if pru == 0:
            self.PRU0.start()
        if pru == 1:
            self.PRU1.start()
        if pru == 2:
            self.PRU0.start()
            self.PRU1.start()
        pass

    def exposed_pru_stop(self, pru):
        if pru == 0:
            self.PRU0.stop()
            t.close()
        if pru == 1:
            self.PRU1.stop()
            t.close()
        if pru == 2:
            self.PRU0.stop()
            self.PRU1.stop()
            t.close()
        pass

    def exposed_pru_transmit(self, message, pru):
        if pru == 0:
            self.PRU0.transmit(message)
        if pru == 1:
            self.PRU1.transmit(message)

    def exposed_pru_blk_receive(self, samples, data_size, pru):
        if pru == 0:
            return self.PRU0.blkreceive(samples, data_size)
        elif pru == 1:
            return self.PRU1.blkreceive(samples, data_size)

    def exposed_pru_receive(self, pru):
        if pru == 0:
            return self.PRU0.receive()
        elif pru == 1:
            return self.PRU0.receive()


if __name__ == '__main__':
    from rpyc.utils.server import ThreadedServer
    t = ThreadedServer(BeagleBoneDDS, port=18861)
    t.start()
