# McSPI (SPI peripherals)
In this example we will read out the LTC1859 from Analog Devices using the Multichannel Serial Port Interface (McSPI) of the device. The goal of this example is to explore more ways to read out SPI devices using the PRUs. This example will also teach the reader how to access other peripherals using the constants table.

## Constants table
The PRU constants table is a structure of hard-coded memory addresses for commonly used peripherals and memories. The constants table is used to more efficiently load/store data to these commonly accessed addresses.
| Entry No.  |Region Pointed To   |Value[31:0]   |
|---|---|---|
|0    |PRU-ICSS INTC (local)   |0x0002_0000   |
|1    |DMTIMER2   |0x4804_0000   |
|2    |I2C1   |0x4802_A000   |
|3    |PRU-ICSS-eCAP(local)   |0x0003_0000   |
|4    |PRU-ICSS CFG(local)   |0x0002_6000   |
|5    |MMCHS 0   |0x4806_0000   |
|6    |MCSPI 0   |0x4803_0000   |
|7    |PRU-ICSS UART0(local)   |0x0002_8000   |
|8    |McASP0 DMA   |0x4600_0000  |
|9    |GEMAC   |0x4A10_0000   |
|10   |Reserved   |0x4831_8000   |
|11   |UART 1   |0x4802_2000   |
|12   |UART 2   |0x4802_4000   |
|13   |Reserved   |0x4831_0000   |
|14   |DCAN0   |0x481C_C000   |
|15   |DCAN1   |0x481D_0000   |
|16   |MCSPI1   |0x481A_0000   |
|17   |I2C2   |0x4819_C000   |
|18   |eHRPWM1  |0x4830_0000   |
|19   |eHRPWM2   |0x4830_2000   |
|20   |eHRPWM3   |0x4830_4000   |
|21   |PRU-ICSS MDIO(local)   |0x0003_2400   |
|22   |Mailbox 0   |0x480C_8000   |
|23   |Spinlock   |0x480C_A000   |
|24   |PRU-ICSS PRU0/1 Data RAM(local)   |0x0000_0n00, n = c24_blk_index[3:0]   |
|25   |PRU-ICSS PRU0/1 Data RAM(local)   |0x0000_2n00, n = c25_blk_index[3:0]   |
|26   |PRU-ICSS IEP(local)   |0x0002_En00, n = c26_blk_index[3:0]   |
|27   |PRU-ICSS MII-RT(local)   |0x0003_2n00, n = c27_blk_index[3:0   |
|28   |PRU-ICSS Shared RAM(local)   |0x00nn_nn00, nnnn = c28_pointer[15:0]   |
|29   |TPCC   |0x49nn_nn00, nnnn = c29_pointer[15:0]   |
|30   |L3 OCMC0   |0x40nn_nn00, nnnn = c30_pointer[15:0]   |
|31   |EMIF0 DDR Base   |0x80nn_nn00, nnnn = c31_pointer[15:0]   |

## McSPI module
The McSPI module can function as a Master or slave and has a transmit mode, receive mode and transmit and receive mode.   
