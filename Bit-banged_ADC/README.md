# Bit banging an ADC
In this example we will use the PRUs to bit bang the LTC1859 from Analog Devices. The goal of this example is to explore the ways to read out SPI devices and enhance the users knowledge of the PRU. We will use a little bit from every predecing tutorial. We will use the scratchpad, GPIO and REMOTEPROC.
<br><br>

## Bit banging
Bit banging is slang for various techniques in which serial communications use software instead of dedicated hardware to process and make use of signals. Software manually reads and sets the pins of the device being used. The software is also responsible for controlling the parameters such as timing, synchronisation etc.
<br><br>

## LTC1859
The LTC1859 is a 16-bit, 100ksps, 8ch ADC with input ranges 5V, 10V, ±5V, ±10V it offers communication via the SPI interface. This is a serial interface commonly used for several applications. The 8 channels can be used in single ended mode, differential mode or combinations of these modes. All channels are ±25V protected
The LTC1859 works by first receiving a command which includes which channel to use, the input range and power managment.


<br><br>

## Running Hello_world
Navigate to the correct folder **/Hello_world** and run the make file. After this you can move the .out file into /lib/firmware/ under the correct name
```
cd /hello_world
make
cd gen
sudo cp HELLO_WORLD.out /lib/firmware/am335x-pru1-fw
```
We use the PRUlib module to read the character device created by the PRU. More information on this module can be found in **/python/README.md**
```
>>> import PRUlib
>>> PRUlib.init(1)
>>> PRUlib.read(1)
Device is running, character device will be read
b'hallo world\n'
b'hallo world\n'
b'hallo world\n'
payload saved in data.txt
```
