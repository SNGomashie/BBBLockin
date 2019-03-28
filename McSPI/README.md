# Multichannel Serial Port Interface (McSPI)
In this example we will read out the LTC1859 from Analog Devices using the Multichannel Serial Port Interface (McSPI) of the device. The goal of this example is to explore more ways to read out SPI devices using the PRUs. This example will also teach the reader how to access other peripherals using the constants table.
<br><br>

## Pin configuration

Pin # | Function | Logic analyzer channel | Cable color
--- | --- | --- | ---
P9_01|GND||Brown
P9_03|3V3||Black
P9_17|SPI0 CS0|CH3|Yellow
P9_18|SPI0 D1 - SDI(MOSI)|CH2|Orange
P9_21|SPI0 D0 - SDO(MISO)|CH1|Green
P9_22|SPI0 CLK|CH0|Blue
P9_25|PRU RD_|CH4|Purple
P9_26|PRU BUSY_|CH6|Red
P9_31|PRU CONVST|CH5|Gray


## Constants table
The PRU constants table is a structure of hard-coded memory addresses for commonly used peripherals and memories. The constants table is used to more efficiently load/store data to these commonly accessed addresses. The constants table for the PRUs can be found in __*TI am335x TRM Chapter 4.4.1.1 table 4.9. PRU0/1 Constants Table*__. TI provides a few headers which make using the constants table a little easier. These can be found in *pru-software-support-package-5.4.0\include\am335x* many of the header files here will be using the constants table. We are particularly interested in sys_mcspi.h. This file includes declarations for every register available in the McSPI module. We also see these lines at the bottom of our file.
```
volatile __far sysMcspi CT_MCSPI0 __attribute__((cregister("MCSPI0", far), peripheral));
volatile __far sysMcspi CT_MCSPI1 __attribute__((cregister("MCSPI1", far), peripheral));
```
These bind the addresses of the registers corresponding to MCSPI0/1 to the names CT_MCSPI0/1. More information about this statement can be found in the __*TI PRU Optimizing C/C++ Compiler V2.3 User's Guide Chapter 5*__
<br><br><br>

## McSPI module
The Master/Slave Multichannel Serial Port Interface(McSPI) module is a general-purpose receive/transmit master/slave controller that can interface with up to four external slave devices or one external master. It allows duplex, synchronous and serial communication between de CPU and an SPI compliant external device. More information on this module can be found in __*TI am335x TRM Chapter 24 Multichannel Serial Port Interface (McSPI)*__.
<br><br>

### LTC1859
We use the McSPI module to read out our LTC1859 ADC. The LTC1859 is an 8 channel, 16-bit, 100ksps Analog-to-Digital converter with a programmable input range form 0V to 5V, 0V to 10V, ±5V or ±10V (Single-ended or differential). The LTC1859 operates in SPI mode 0, MSB first and has a clock frequency of 20Mhz. The image below shows the operating sequence for the LTC1859:
<img src="https://i.imgur.com/xuggF9d.png" alt="drawing"/>
1. First we send an 8-bit word over the Serial Data In (SDI (Master Out Slave In)) line that contains information about the mux address in the 4 MSBs and information about the input range and power management in the 4 LSB.
2. Allow the ADC to do the conversion by raising the CONVST pin (Pin 28 on the IC). CONVST needs to be high for 40ns to start a conversion, one conversion takes 5000 ns. While a conversion is ongoing the BUSY line(Pin 22 of the IC) will be low.
3. The conversion will be clocked out on the Serial Data Out(SDO (Master In Slave Out)) while the RD pin(Pin 27 of the IC) is low and a new word can be loaded into the SDI line (Step one).
<br><br>
The image below show the configuration for the input data word
<img src="https://i.imgur.com/blsXJrE.png" alt="drawing"/>
The table below shows the timing characteristics:

| Symbol | Parameter | Value |
| --- | --- | --- |
| Tconv | Conversion time| 5000ns|
| Tclk  | Clock period   | 50ns  |
| Fclk  | Clock frequency| 20Mhz |

<br><br>

### McSPI registers
The McSPI module needs to be configured properly to interface with the LTC1859 ADC. This is done by writing into registers of the McSPI module. The registers we are most concerned with are shown in the table below. A more detailed description of the registers can be found in the __*TI am353x TRM Chapter 24 Paragraph 4 McSPI register*__
<br><br>

offset | Acronym | Name | Description
--- | --- | --- | ---
0x0110|MCSPI_SYSCONFIG| McSPI system configuration register| allows control of various parameters of the module. Not sensitive to software reset.
0x0114|MCSPI_SYSSTATUS| McSPI system status register|Provides status information about the module
0x0118|MCSPI_IRQSTATUS| McSPI interrupt status register|Groups the status of the module internal events that can generate a interrupt
0x011C|MCSPI_IRQENABLE| McSPI interrupt enable register|Enables/disables module sources of interrupts
0x0124|MCSPI_SYST| McSPI system register| Used to configure the interconnect
0x0128|MCSPI_MODULCTRL| McSPI module control register| Used to configure Serial Port Interface
0x012C|MCSPI_CH0CONF| McSPI channel 0 configuration register|Used to configure channel 0
0x0130|MCSPI_CH0STAT| McSPI channel 0 status register|Provides status iformation about McSPI channel 0 receive/transmit buffers
0x0134|MCSPI_CH0CTRL| McSPI channel 0 control register|Used to control channel 0
0x0138|MCSPI_TX0| McSPI chanenl 0 FIFO transmit buffer register|Contains word to be transmitted
0x013C|MCSPI_RX0| McSPI channel 0 FIFO receive buffer register|Contains received word
0x017C|MCSPI_XFERLEVEL| McSPI transfer levels register |Provides transfer levels needed while using FIFO buffer during transfer
0x0180|MCSPI_DAFTX| McSPI DMA address aligned FIFO transmitter register|DMA TX register
0x01A0|MCSPI_DAFRX| McSPI DMA address aligned FIFO receiver register|DMA RX register

<br><br>

### Programming aid

We split our interaction with the McSPI module into three parts, The module initialization, channel initialization and the main program. In the initialization steps we configure the module and channel (and possibly the DMA) and in the main program we actually send/receive information.
<br><br>

__CLock__

Before being able to reset the McSPI chip you will have to configure the Power Reset Clock Module(PRCM) and activating the module functional clock. More information about the PRCM can be found in the __*TI am353x TRM Chapter 8 Power, Reset, and Clock Management (PRCM)*__

__Module initialization__
1. Hard or soft reset
2. Wait until reset is done by reading the *McSPI_SYSSTATUS* register
3. Configure the *MCSPI_MODULCTRL* and *MCSPI_SYSCONFIG* registers
<br><br>

__Channel initialization__
1. Reset interrupts in the *MCSPI_IRQSTATUS* register and enable/disable interrupts in the *MCSPI_IRQENABLE* registers.
2. Configure the channel by writing in the *MCSPI_CH0CONF* register
3. Enable channel by writing to the *MCSPI_CH0CTRL* register
<br><br>

__Main Program__
1.  TX empty - Generate DMA write event/ polling TX empty flag by CPU to write First
transmit word into MCSPI_TX0
2. RX full - Generate DMA read event/ polling RX full flag by CPU to read word into memory from MCSPI_RX0
3. Disable channel by writing to the *MCSPI_CH0CTRL* register
