#!/usr/bin/env python3
""" Module for interfacing with the AM335X PRU-ICSS
.. moduleauthor:: Sena Gomashie <s.gomashie@hotmail.nl>
"""
import struct
import time
import numpy as np


__all__ = ['PRU_ICSS']


class PRU_ICSS():
    """ Provides a communication interface to the AM335X PRU-ICSS.

    :note: The pru-icss kernel modules must be loaded, see dmesg
    """

    # RPMsg Defaults
    RPMSG_BUF_SIZE = 512
    charBuf = "\0" * 512

    def __init__(self):
        self.CHAR_DEV = ["/dev/rpmsg_pru30", "/dev/rpmsg_pru31"]
        self.STATE = ["/sys/class/remoteproc/remoteproc1/state", "/sys/class/remoteproc/remoteproc2/state"]
        self.dev = [None] * 2

    def status(self, pru):
        """ Method returns the currect status of the selected pru

        : param pru: int, pru of interest
        """
        state = open(self.STATE[pru], 'r+')
        status = state.read(7)
        state.close()
        return status

    def start(self, pru):
        """ This method starts a PRU via the sysfs interface.

        :param pru: int, pru to be started.
        """
        state = open(self.STATE[pru], 'r+')
        status = self.status(pru)
        if 'running' in status:
            print("-    PRU%d is running" % (pru))
        elif 'offline' in status:
            try:
                state.write('start')
                print("-    PRU%d has started" % (pru))
                state.close()
                time.sleep(.1)
            except IOError:
                print("-  ERROR  PRU%d failed to start" % (pru))
                state.close()

    def stop(self, pru):
        """ This method stops a PRU via the sysfs interface.

        :param pru: int, pru to be stopped.
        """
        state = open(self.STATE[pru], 'r+')
        status = self.status(pru)
        if 'offline' in status:
            print("-    PRU%d is offline" % (pru))
        elif 'running' in status:
            try:
                state.write('stop')
                print("-    PRU%d has stopped" % (pru))
                state.close()
                time.sleep(.1)
            except IOError:
                print("-  ERROR  PRU%d failed to stop" % (pru))
                state.close()

    def open_char(self, pru):
        """ This method creates a file object for the RPMsg character devive.

        :param pru: int, the pru of which a file object should be created
        """
        try:
            self.dev[pru] = open(self.CHAR_DEV[pru], "rb+", 0)
        except IOError:
            print("-  ERROR  Could not open device: 'rpmsg_pru30'")

    def close_char(self, pru):
        """ This method destroys a file object for the selected character device.
            The kernel is not able to flush the remoteproc FIFO fast enough so
            a delay is added.

        :param pru: int, the pru of which the file object should be destroyed
        """
        try:
            self.dev[pru].close()
            print("\n-    Closing character device")
            # Delay for a few seconds so the kernel can flush the remoteproc FIFO
            time.sleep(.1)
        except IOError:
            print("\n-    ERROR could not close char dev")

    def transmit(self, pru, message):
        """ This method transmits a message to the PRU-ICSS using RPMsg

        :param pru: int, the pru to which the message should be send
        :parm pru: (int or string), This can be anything aslong as the PRU is
                   expecting it
        """
        try:
            self.dev[pru].write(bytes(str(message), 'ASCII'))
            print('-    "%s" transmitted to PRU%d' % (message, pru))
        except IOError:
            print("-  ERROR  Could not open device: 'rpmsg_pru30'")
        pass

    def receive(self, pru, block, samples=0):
        """ This method receives messages from the PRU-ICSS using RPMsg
            These messages have a maximum length of 496 bytes. This allows for
            packets of sizes:
            - 8bits: 496 (int8)
            - 16bits: 248 (int16)
            - 32bits: 124 (int32)
            - 2 * 16-bits: 124 (2int16)
            - 3 * 16-bits: 82 (3int16)
            - 2 * 32-bits: 62 (2int32)

        :param pru: int, the pru from which to receive a message
        :param block: str, defines the configuration of the received 496 bytes
        :param samples: int, the amount of samples to received. 0 is infinite
        """
        size = block.split("int")
        if size[0] == '':
            size[0] = 1
        data = []
        buf = []
        if samples == 0:
            try:
                while(1):
                    charBuf = self.dev[pru].read(self.RPMSG_BUF_SIZE)
                    if block == "int8":
                        buf = struct.unpack('<496b', charBuf)
                    if block == "int16":
                        buf = struct.unpack('<248h', charBuf)
                    if block == "int32":
                        buf = struct.unpack('<124i', charBuf)
                    if block == "2int16":
                        buf = struct.unpack('<124h124h', charBuf)
                    if block == "3int16":
                        buf = struct.unpack('<82h82h82h', charBuf)
                    if block == "2int32":
                        buf = struct.unpack('<62i62i', charBuf)
                    data.extend(buf)
                    print("\r-    RPMsg packet received {:06d}".format(len(data)), end='')
            except KeyboardInterrupt:
                self.close_char(pru)
                self.stop(pru)
                return data
        else:
            num = 496 / (int(size[0]) * int(size[1]) / 8)
            tot = np.ceil(samples / num)
            print("-    %d samples will be transfered in %d packets" % (samples, tot))
            try:
                for i in range(int(tot)):
                    charBuf = self.dev[pru].read(self.RPMSG_BUF_SIZE)
                    if block == "int8":
                        buf = struct.unpack('<496b', charBuf)
                    if block == "int16":
                        buf = struct.unpack('<248h', charBuf)
                    if block == "int32":
                        buf = struct.unpack('<124i', charBuf)
                    if block == "2int16":
                        buf = struct.unpack('<124h124h', charBuf)
                    if block == "3int16":
                        buf = struct.unpack('<82h82h82h', charBuf)
                    if block == "2int32":
                        buf = struct.unpack('<62i62i', charBuf)
                    data.extend(buf)
                    print("\r-    RPMsg packet received ( %d / %d )" % ((i + 1), tot), end='')
                data = np.reshape(data, (int(len(data)/int(size[0])), int(size[0])))
                self.close_char(pru)
                self.stop(pru)
                return data
            except KeyboardInterrupt:
                self.close_char(pru)
                self.stop(pru)
                return data
