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
- [LTC1859 Datasheet](https://www.analog.com/en/products/ltc1859.html#product-overview)


## Table of contents

1. [ Task List ](#task-list)
2. [ Setup ](#setup)
   - [Format your SD card (windows 10 and linux)](#format)
   - [Download the latest image from the Beaglebone site](#latest)
   - [Flash image onto SD card (windows 10 and linux) ](#flash)
   - [Enabling the Beaglebone PRU](#en_pru)
   - [Installing the TI PRU Code Generation Tools](#installing-the-ti-pru-code-generation-toolspru-cgt)
   - [Configure GPIO at startup](#Configure-gpio)
3. [Programming the PRU](#programming-the-pru)
   - [Cloning the Repo](#cloning-the-repositories)
   - [Examples](#running-our-first-program)
     1. [PRUlib](https://github.com/SNGomashie/BBBLockin/tree/master/Python)
     2. [Blink](https://github.com/SNGomashie/BBBLockin/tree/master/Blinky)
     3. [RPMSG](https://github.com/SNGomashie/BBBLockin/tree/master/Hello_world)
     4. [PRU Speed test](https://github.com/SNGomashie/BBBLockin/tree/master/Cycle_test)
     5. [UART](https://github.com/SNGomashie/BBBLockin/tree/master/UART)
     6. [Scratchpad](https://github.com/SNGomashie/BBBLockin/tree/master/Scratchpad)
     7. [MAC](https://github.com/SNGomashie/BBBLockin/tree/master/MAC)
     8. [Bit-banging an SPI slave](https://github.com/SNGomashie/BBBLockin/tree/master/Bit-banged_ADC)
     9. [onboard ADC](https://github.com/SNGomashie/BBBLockin/tree/master/Onboard_ADC)
     10. [McSPI](https://github.com/SNGomashie/BBBLockin/tree/master/McSPI)
     11. [SYNC w/ timer](https://github.com/SNGomashie/BBBLockin/tree/master/Sync)
     12. [SYNC w/ eCAP](https://github.com/SNGomashie/BBBLockin/tree/master/eCAP)
4. [Simulating a Lock-in Amplifier](https://github.com/SNGomashie/BBBLockin/tree/master/Sim)
5. [Generating Sine/Cosine through DDS](https://github.com/SNGomashie/BBBLockin/tree/master/DDS)
5. [Implementing the Lock-in Amplifier](https://github.com/SNGomashie/BBBLockin/tree/master/Lockin)



## Task List

- [x] Enable the BeagleBone PRU
- [x] Run first program on the PRU
- [x] Write python module to interface with PRU
- [x] Read first message from PRU using remoteproc and python
- [x] Write library for easy usage of remoteproc on the PRU
- [x] Send message from PRU0 -> PRU1 using scratch pad
- [x] Read the onboard ADC
- [x] Use the MAC
- [x] Bit-bang an SPI ADC
- [x] Using the McSPI chip
- [x] Measure period using timer/cycle
- [x] Send messages over UART
- [x] Determine max speed for lock in on PRU
- [x] Determine reading speed PRU
- [x] Measure period using eCAP
- [x] Implement DDS
- [ ] Implement lock-in amplifier on the PRU

## Setup

### Requierments
1. BeagleBone Black
2. 16GB microSD card
3. MicroSD to USB / MicroSD to SD adapter
4. MicroUSB cable to power the BeagleBone Black
4. An internet connection wired or wireless
5. (Optional - UART example) 3V3 FTDI cable
6. (Optional - SPI example) SPI ADC
7. (Optional - eCAP example)  Function generator

### Quick start guide
1. Download the latest image from the [BeagleBone website](https://beagleboard.org/latest-images)
(We use Debian 9.5 2018-10-07 4GB SD IoT)
2. [Flash]((#flash) this image onto your 16Gb SD card
3. Boot from the SD card by pressing [the button close to the microSD card slot and the power button](http://beagleboard.org/getting-started#step1).
4. [SSH into the beaglebone](#en_pru)
5. [Restore partition size and install a new kernel and update your system](#en_pru).
(We use linux 4.14.94-ti-rt-r93)
6. Update bootloader, update system, update bb overlays
7. Load cape universala
8. __Make sure you are booting from the SD card, eMMC bootloader does not load PRUs__
9. Run /opt/scripts/tools/version.sh to see if the cores are running
10. If you followed these steps and it still does not work. Make sure you are booting from your SD card



<a name="F-w10"></a>

### Format your SD card (windows 10)
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
sudo apt-get update
sudo reboot

sudo apt-get install linux-image-4.14.94-ti-rt-r93
sudo reboot

cd /opt/scripts/tools/developers/
sudo ./update_bootloader.sh
reboot

sudo apt install --only-upgrade bb-cape-overlays
sudo config-pin overlay cape-universala
sudo reboot

```
We will now edit our U-boot overlay to prevent conflicting pins:
```
sudo cd /boot/uEnv.txt
```
With v4.14x-ti PRU via remoteproc should be enabled by default:
```
uboot_overlay_pru=/lib/firmware/AM335X-PRU-RPROC-4-14-TI-00A0.dtbo
```
We also disable video and audio because they have some conflicting pins
```
disable_uboot_overlay_emmc=1

disable_uboot_overlay_video=1

disable_uboot_overlay_audio=1

```

### See if PRUs are running
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

## Configure GPIO

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
Clone this repository to your beaglebone home directory
```
cd
git clone git@github.com:SNGomashie/BBB-Lockin.git
```

### Example programs
There are several examples available from both this github and the TI PRU software support package. For a quick start go through the examples on this Github. It is recommended to go through the Python library first so you can easily read messages from the PRU.
1. [PRUlib](https://github.com/SNGomashie/BBBLockin/tree/master/Python) - A python module for the PRUs
2. [Blink](https://github.com/SNGomashie/BBBLockin/tree/master/Blinky) - Blink a LED
3. [RPMSG](https://github.com/SNGomashie/BBBLockin/tree/master/Hello_world) - Send messages using the remoteproc framework
4. [Speed test](https://github.com/SNGomashie/BBBLockin/tree/master/Cycle_test) - Test the speed of instructions
5. [UART](https://github.com/SNGomashie/BBBLockin/tree/master/UART) - Send messages over UART to host PC
6. [Scratchpad](https://github.com/SNGomashie/BBBLockin/tree/master/Scratchpad) - Use scratchpad for instant messaging between PRU cores
7. [MAC](https://github.com/SNGomashie/BBBLockin/tree/master/MAC) - Multiply and accumulate 32-bit numbers in a 32-bit processor
8. [Bit-banging an SPI slave](https://github.com/SNGomashie/BBBLockin/tree/master/Bit-banged_ADC) - Implement software SPI
9. [onboard ADC](https://github.com/SNGomashie/BBBLockin/tree/master/Onboard_ADC) - Read the onboard ADC
10. [McSPI](https://github.com/SNGomashie/BBBLockin/tree/master/McSPI) - Implement hardware SPI
11. [SYNC w/ timer](https://github.com/SNGomashie/BBBLockin/tree/master/Sync) - Measure periods using timers
12. [SYNC w/ ecap](https://github.com/SNGomashie/BBBLockin/tree/master/eCAP) - Measure periods using the eCAP module

## Problems
If PRUs are not loaded and you followed all the steps. __Please see if u are booting from the SD card and not eMMC__

If clpru is not found run:
```
sudo apt-get install ti-pru-cgt-installer
reboot
```

If the pru-software-support-package is not avaiable run:
```
cd /usr/lib/ti/
git clone git://git.ti.com/pru-software-support-package/pru-software-support-package.git
```
