# Scratchpad
In this example you will learn how to use scratchpad to communicate between the 2 PRU cores. Scratchpad allows for single cycle write and reads between the two PRUs. This is faster than using the shared memory which requires 1 cycle for writes and 3 for reads. One PRU wil generate 4 values and send these to the other PRU. The second PRU will send this data to the userspace using RPMsg.
<br><br>

The scratchpad can be accessed through a single C statement. To read:
```
__xin(unsigned int device_id ,
unsigned int base_register , unsigned int
use_remapping , void& object );
```
And to write:
```
__xout ( unsigned int device_id ,
unsigned int base_register , unsigned int
use_remapping , void& object );
```
- *device_id*: (Most commonly used)
  - 10: Scratch pad bank 0 accessible by both PRU cores
  - 11: Scratch pad bank 1 accessible by both PRU cores
  - 12: Scratch pad bank 2 accessible by both PRU cores
  - 14: Scratch pad Direct connect mode
- *base_register*: Value from 0-32 that corresponds to the register that must be used as the base register for the transfer
- *use_remapping*: Boolean value
  - if 0(false) the reads/writes go into register specified by __base_register__
  - if 1(true) causes reads/writes to go into a different register
- *object*: An object with a size less than 44 bytes. also specifies how many registers will be filled from the __base_register__ if __use_remapping__ = 0
<br><br>

## Running Scratchpad
Running this example requires firmware to be loaded to both cores. This is done the exact same way we did in other examples but twice. Navigate to **/Scratchpad** there should be a folder in there for each PRU. Run the make file in both folders and copy the firmware from the **/gen** folder to **/lib/firmware**
```
cd /Scratchpad
cd /pru0
make
cd gen
sudo cp pru0.out /lib/firmware/am335x-pru1-fw
cd ..
cd ..
cd /pru1
Make
cd gen
sudo cp pru1.out /lib/firmware/am335x-pru0-fw
```
We use the PRUlib module to read the character device created by the PRU. More information on this module can be found in **/python/README.md**
```
>>> import PRUlib
>>> PRUlib.goodmorning()
>>> PRUlib.read(1)
Device is running, character device will be read
b'0001,0002,0003,0004\n'
b'0002,0004,0006,0008\n'
b'0003,0006,0009,000C\n'
b'0004,0008,000C,0010\n'
payload saved in data.txt
>>>
```
