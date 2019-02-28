#Blinky example
In this example you will learn how to compile code for the PRU and how to load it onto the PRU. Once this example is running it should blink USR3 (The LED closest to the Ethernet port). You can change which led is blinking in the code.
<br><br>

TI includes the PRU Code Generation Tools (CGT) as part of Code Composer Studio V6 (CCSv6). The PRU CGT has also been released in CLI form. This can be used to compile C code on the BeagleBone. The CGT consists of the C compiler (clpru), the assembler (asmpru), the linker (lnkpru) and the output file generation tool (hexpru). the output of this toolchain is an .out file which can be directly loaded into /lib/firmware under the name amm335x-pruX-fw (where X is the pru you are using). The PRU has to be offline before we can load new firmware into it.
![Image](https://i.imgur.com/8idytpS.png)
<br><br>

Assuming the setup has been followed until *running our first program* paragraph. We can now compile and run our program. Simply navigate to the correct folder **/Blinky**. And type the following command in the command line. The makefile automatically copies the .out file into /lib/firmware under the correct name.
```
source setup.sh
make
```
The terminal should return:
```
-    Stopping PRU 0
stop
-       copying firmware file /tmp/pru0-gen/hello.out to /lib/firmware/am335x-pru0-fw
-    Starting PRU 0
start

```
