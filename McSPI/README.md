# McSPI (SPI peripherals)
In this example we will read out the LTC1859 from Analog Devices using the Multichannel Serial Port Interface (McSPI) of the device. The goal of this example is to explore more ways to read out SPI devices using the PRUs. This example will also teach the reader how to access other peripherals using the constants table.
<br><br>

## Constants table
The PRU constants table is a structure of hard-coded memory addresses for commonly used peripherals and memories. The constants table is used to more efficiently load/store data to these commonly accessed addresses. The constants table for the PRUs can be found in __*TI am335x TRM Chapter 4.4.1.1 table 4.9. PRU0/1 Constants Table*__. TI provides a few headers which make using the constants table a little easier. These can be found in *pru-software-support-package-5.4.0\include\am335x* many of the header files here will be using the constants table. We are particularly interested in sys_mcspi.h. This file includes declarations for every register available in the McSPI module. We also see these lines at the bottom of our file.
```
volatile __far sysMcspi CT_MCSPI0 __attribute__((cregister("MCSPI0", far), peripheral));
volatile __far sysMcspi CT_MCSPI1 __attribute__((cregister("MCSPI1", far), peripheral));

```
These bind the addresses of the registers corresponding to MCSPI0/1 to the names CT_MCSPI0/1. More information about this statement can be found in the __*TI PRU Optimizing C/C++ Compiler V2.3 User's Guide Chapter 5*__


## McSPI module
The Master/Slave Multichannel Serial Port Interface(McSPI) module is a general-purpose receive/transmit master/slave controller that can interface with up to four external slave devices or one external master. It allows duplex, synchronous and serial communication between de CPU and an SPI compliant external device. More information on this module can be found in __*TI am335x TRM Chapter 24 Multichannel Serial Port Interface (McSPI)*__.
<br><br>

### LTC1859
We use the McSPI module to read out our LTC1859 ADC. The LTC1859 is an 8 channel, 16-bit, 100ksps Analog-to-Digital converter with a programmable input range form 0V to 5V, 0V to 10V, ±5V or ±10V (Single-ended or differential). The image below shows the operating sequence for the LTC1859:
<img src="https://i.imgur.com/xuggF9d.png" alt="drawing" width="400" />
