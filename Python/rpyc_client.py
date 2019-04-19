import rpyc
import time


c = rpyc.connect("192.168.7.2", 18861)

time.sleep(5)

c.root.pru_communicate("10000")
