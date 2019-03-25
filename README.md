# Beaglebone Black - Lock-in amplifier

## Introduction
During my intership at SRON Netherlands Institute for Space Research I had to design a lock-in amplifier on the BeagleBone Black using the Programmable Real-time Units. These are barebone processors, on which we can load firmware from the linux userspace. After loading the firmware they can function completely independently of the main ARM processor.

- [Exploring Beaglebone 2nd edition](https://smtebooks.eu/book/getfile1/16254/MTYyNTQ=)
- [TI PRU-ICSS wiki](http://processors.wiki.ti.com/index.php/PRU-ICSS)
- [TI AM335x Sitara processor datasheet](http://www.ti.com/lit/ds/symlink/am3358.pdf)
- [TI AM335x Technical Reference Manual](https://www.ti.com/lit/ug/spruh73p/spruh73p.pdf)
- [TI PRU Optimizing C/C++ Compiler](http://www.ti.com/lit/ug/spruhv7c/spruhv7c.pdf)
- [TI PRU Read Latencies](http://www.ti.com/lit/an/sprace8a/sprace8a.pdf)
- [TI Hands-on labs](http://processors.wiki.ti.com/index.php/PRU_Training:_Hands-on_Labs)
- [Old Elinux entry](https://elinux.org/Ti_AM33XX_PRUSSv2)


## Table of contents

1. [ Task List ](#task-list)
2. [ Setup ](#setup)
   - [Format your SD card (windows 10 and linux)](#format)
   - [Download the latest image from the Beaglebone site](#latest)
   - [Flash image onto SD card (windows 10 and linux) ](#flash)
   - [Enabling the Beaglebone PRU](#en_pru)
   - [Installing the TI PRU Code Generation Tools](#installing-the-ti-pru-code-generation-toolspru-cgt)
   - [Configure GPIO at startup](#Configure-gpio-at-startup)
3. [Programming the PRU](#programming-the-pru)
   - [Cloning the Repo](#cloning-the-repositories)
   - [Examples](#running-our-first-program)
     1. [PRUlib](https://github.com/SNGomashie/BBBLockin/tree/master/Python)
     2. [Blinky](https://github.com/SNGomashie/BBBLockin/tree/master/Blinky)
     3. [Hello_world](https://github.com/SNGomashie/BBBLockin/tree/master/Hello_world)
     4. [Scratchpad](https://github.com/SNGomashie/BBBLockin/tree/master/Scratchpad)
     5. [Bit-banging an SPI slave](https://github.com/SNGomashie/BBBLockin/tree/master/Bit-banged_ADC)
     6. [Using the onboard ADC](https://github.com/SNGomashie/BBBLockin/tree/master/Onboard_ADC)
     7. [Using the McSPI chip](https://github.com/SNGomashie/BBBLockin/tree/master/McSPI)
4. [Simulating a Lock-in Amplifier](https://github.com/SNGomashie/BBBLockin/tree/master/Sim)



## Task List

- [x] Enable the BeagleBone PRU
- [x] Run first program on the PRU
- [x] Write python module to interface with PRU
- [x] Read first message from PRU using remoteproc and python
- [x] Write library for easy usage of remoteproc on the PRU
- [x] Send message from PRU0 -> PRU1 using scratch pad
- [x] Read the onboard ADC
- [x] Bit-bang an SPI ADC
- [x] Using the McSPI chip
- [ ] Determine max speed for lock in on PRU
- [ ] Determine reading speed PRU
- [ ] Implement lock-in amplifier on the PRU

## Setup
   <a name="F-w10"></a>
 ### Format your SD card (windows 10):
 ---
 Open windows start menu and search for:
 ```
 diskpart
 ```
 Wait until loaded and write:
 ```
 list disk
 ```

 Select the disk you want by writing (where X is the disk you want to format) :
 ```
 select disk X
 ```
 write (This will delete all the data on the SD card):
 ```
 clean
 ```

 write (to create a partition on the SD card):
 ```
 create partition primary
 ```
 write (to do a quick format):
 ```
 format quick
 ```

 ### Format your SD card (linux) (Ubuntu 14.04 tested):
 ---
 Open a terminal on you linux machine

 Identify your device by running (run without SD card and than with SD card plugged to quickly identify your SD card):
 ```
 lsblk
 ```

 write (Where sdX is your device name like 'sdb'):
 ```
 sudo parted /dev/sdX
 (parted) mklabel msdos
 (parted) mkpart primary fat32 1MiB 100%
 (parted) set 1 boot on
 (parted) quit
 sudo mkfs.vfat /dev/sdX
 ```
<a name="latest"></a>
### Download the latest image from the Beaglebone site
---
 we are using Debian 9.5 2018-10-07 4GB SD IoT (https://beagleboard.org/latest-images)

 We should now have an .img file. We can flash it on our SD card

 ### Flash image onto SD card (Windows 10)
 ---
 Download image flasher from: https://etcher.io/
 Run Etcher and Select your image and drive and press Flash!

 ### Flash image onto SD card (Linux)(Ubuntu 14.04 tested)
 ---
 Locate the SD card using
 ```
 lsblk
 ```
 unmount the partitions:
 ```
 sudo umount /dev/sdX
 ```
 copy the content of the image file onto the SD card by running:
 ```
 sudo dd bs=1M if=<Image location>.img of=/dev/sdX
 ```
<a name="en_pru"></a>
### Enabling the Beaglebone PRU
---
Follow the guide linked to start your BeagleBone from the SD card
http://beagleboard.org/getting-started
Connect to the Beaglebone using SSH
Use [putty](https://www.putty.org/) for SSH on windows
Or enter this to SSH on linux (Ubuntu 14.04 tested)
```
ssh 192.168.7.2 -l debian -u temppwd
```

We start by expanding our file system partition on our MicroSD:
```
cd /opt/scripts/tools
git pull || true
sudo ./grow_partition.sh
sudo reboot
```
After the reboot we update our kernel (We run 4.14.94-ti-rt-r93):
```
sudo apt-get install linux-image-4.14.94-ti-rt-r93

sudo reboot

sudo ./opt/scripts/tools/update_kernel.sh

sudo reboot
```
We will now edit our U-boot overlay:
```
sudo cd /boot/uEnv.txt
```
With v4.14x-ti PRU via remoteproc can be enabled by:
```
dtb_overlay=/lib/firmware/AM335X-PRU-RPROC-4-14-TI-00A0.dtbo

uboot_overlay_pru=/lib/firmware/AM335X-PRU-RPROC-4-14-TI-00A0.dtbo
```
We also disable video and audio because they have some conflicting pins
```
disable_uboot_overlay_emmc=1

disable_uboot_overlay_video=1

disable_uboot_overlay_audio=1

```
<a name="ti_cgt"></a>
### Installing the TI PRU Code Generation Tools(PRU-CGT)
---
```
sudo apt-get install ti-pru-cgt-installer
reboot
```

### See if PRUs/PRU-CGT are running (sometimes they take some time to startup)
---
This command print the linux kernel message buffer
```
dmesg | grep remoteproc
```
and it should return (remoteproc0 is not related to the PRUs but is necessary)
```
[  316.413590] remoteproc remoteproc1: 4a334000.pru is available
[  316.563207] remoteproc remoteproc2: 4a338000.pru is available
```
This command shows which loadable kernel modules are currently loaded.
```
lsmod | grep pru
```

and it should return
```
pruss_soc_bus          16384  0
pru_rproc              28672  0
pruss                  16384  1 pru_rproc
pruss_intc             20480  1 pru_rproc

```

This command shows where the TI PRU Code Generation Tool is located:
```
whereis clpru
```

### Configure GPIO

Configuring a pin on the BeagleBone Black is very simple. You can use the 'config-pin' utility to configure the pin. You only need to know the position on the header. For example: pin 30 on the P9 header (P9_30)
<img src="https://i.imgur.com/v99FHnQ.png" alt="drawing"/>
<br><br>

To load our custom pin configuration at startup we first create a file with our pin configs. (In this example we use pin 17 on the P9 header and configure it as SPI chip select)
```
~$ sudo nano /usr/bin/enable-spi-pins.sh

#!/bin/bash

config-pin P9_17 spi_cs
```
We have to give this file the right permission by running:
```
~$ sudo chmod 755 /usr/bin/enable-spi-pins.sh
```
Now we create another file which will call the previous file on startup.
```
~$ sudo nano /lib/systemd/system/enable-spi-pins.service

[Unit]
Description=Enable SPI pins
After=generic-board-startup.service

[Service]
Type=simple
ExecStart=/usr/bin/enable-spi-pins.sh

[Install]
WantedBy=multi-user.target
```
Now we enable our new systemd service
```
~$ sudo systemctl daemon-reload
~$ sudo systemctl enable enable-spi-pins.service
```
Reboot and test if the pins are set on boot

## Programming the PRU

The PRU can be progammed in both assembly and C. Although assembly code could run faster than C, this requiers learning a new language and optimizing assembly code. This takes **ALOT** of
time.

### Cloning the repositories
---
First we clone the PRU software support package. This is a package offered by TI which includes libraries and more
```
cd /usr/lib/ti/
git clone git://git.ti.com/pru-software-support-package/pru-software-support-package.git
```

The first progam we are going to load to our PRUs is 'blinky' this program will make USR3 blink 5 times.

```
cd
git clone git@github.com:SNGomashie/BBB-Lockin.git
```

### Running our first program
There are several examples available from both this github and the TI PRU software support package. For a quick start go thourgh the examples on this Github. It is recommended to go through the Python library first so you can easily read messages from the PRU.
1. [PRUlib](https://github.com/SNGomashie/BBBLockin/tree/master/Python)
2. [Blinky](https://github.com/SNGomashie/BBBLockin/tree/master/Blinky)
3. [Hello_world](https://github.com/SNGomashie/BBBLockin/tree/master/Hello_world)
4. [Scratchpad](https://github.com/SNGomashie/BBBLockin/tree/master/Scratchpad)
5. [Bit-banging an SPI slave](https://github.com/SNGomashie/BBBLockin/tree/master/Bit-banged_ADC)
