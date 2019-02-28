# Hello world (REMOTEPROC/RPMsg)
In this example you will learn how to send and receive message from the ARM to the PRU, and from the PRU to the arm. We do this using the REMOTEPROC framework and the RPMsg message passing mechanism. The goal of this example is to familiarize the user with the REMOTEPROC/RPMsg framework
<br><br>

## REMOTEPROC
The remoteproc framework allows the host' ARM processor to load firmware into the guest PRU cores, start the PRU cores, stop the PRU cores and configure resources for the PRUs. Remoteproc is included as a kernel driver. This is a core remoteproc driver providing load/run/halt API to more specific remoteproc drivers. The PRU firmware binaries exist in the file system (/lib/firmware/). The sysfs interface, which is exposed to the user space, is used to specify the firmware to be loaded and to start or stop the PRU cores. the remoteproc driver loads the pru-rproc module. This module checks is the PRU firmware binaries exist. These must be located in /lib/firmware/. It also parses the firmware binaries looking for .resource_table. This section of the firmware specifies the system resources the PRU will need during execution. The pru-rproc module than configures all of the resources requested by the firmwares, loads the binary into the instruction RAM of the PRUs and loads the resource table into the PRUs data RAM. Finally pru_rproc instructs the PRU cores to begin execution.
<br><br>

## RPMsg
RPMsg is a message passing mechanism that requests resources through remoteproc and builds on top of the existing virtio framework. Shared buffers are requested through the aforementioned .resource_table and are provided by remoteproc during the PRU firmware loading. The shared buffers are contained inside vring data structures in the DDR memory, vrings are virtual ring buffers. Two vrings are provided per PRU core, one is for ARM to PRU communication and one is for PRU to ARM communication. Mailboxes are used to notify the PRU/ARM when messages are available in the shared buffer. On the ARM Linux side RPMsg communications are received in kernelspace and are made available to the user through a character device.
On the PRU side a RPMsg library is provided that tries to abstract the communication to a point where a user can call pru_rpmsg_receive or pru_rpmsg_send to communicate with the arm core. This library is provided in the Linux processor SDK. Although TI supports REMOTEPROC/RPMsg there is very little actual documentation on the framework. This makes the learning curve quite steep.
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
