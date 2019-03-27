# PRU instruction speed test
In this example you will learn how to keep track of the amount of cycles the PRU takes for a certain instruction. We do this so we can determine how fast the PRU can do certain tasks. This might be multiplication and addition or complex tasks like reading an SPI device or reading the ADC.

## PRU registers
The PRU-ICSS has registers for controlling the cores, debugging the core and data registers within their RAM. These can come in handy while working with the PRUs. following is a table with all the registers and their addresses:

Function | Address
--- | ---
PRU ADDRESS | 0x4a30_0000
PRU0 CTRL | 0x0002_2000
PRU0 DBG | 0x0002_2400
PRU1 CTRL | 0x0002_4000
PRU1 DBG | 0x0002_4400
PRU0 DATA | 0x0000_0000
PRU1 DATA | 0x0000_2000
PRU SHARED | 0x0001_0000

PRU ADDRESS is the global address of the PRU-ICSS, all the addresses below the PRU ADDRESS are offsets. The PRU CTRL register allows full control over the PRU from one register. This includes starting and stopping the PRU, putting the PRU in sleep mode or putting the PRU in single step mode. The PRU DBG register contains the 31 registers internal on each PRU. This includes R30 and R31. The PRUx DATA register is the 8kB DATA RAM for each PRU. And the PRU SHARED register is the 12kB of shared memory between the 2 PRU cores. All of these registers can be accessed from the ARM user space using any program to read/write from/to any location in memory like __devmem2__.

### CYCLE register
For this example we are particularly interested in the CYCLE register. This register increments by 1 for each cycle the PRU goes through. If we use this register wisely we can extract how many cycles a certain operation takes. We do this by:
1. First enabling the cycle counter
2. Than clearing the CYCLE register by writing 0 into the register.
3. Immediately run the statement to be evaluated
5. Save the value of the CYCLE register into R0 internal to the PRU.

This can be read out using devmem2 at the address 0x00022400. A single cycle on the PRU takes 5ns (Processor runs at 200Mhz).
