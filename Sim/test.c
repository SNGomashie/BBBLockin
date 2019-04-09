#include <stdio.h>
#include <stdint.h>



void main(void){
  char data[] = "";
sprintf(data, "%04X %04X %04X %04X\n", 0x0000, 0xFFFF, 0x0FF0, 0xF00F);
  printf(data);
}
