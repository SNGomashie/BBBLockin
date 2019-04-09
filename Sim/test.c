#include <stdio.h>
#include <stdint.h>



void main(void){
  char data[] = "";
  sprintf(data, "%d %d %d %d\n", 0xFFFF, 0x0000, 0x0FF0, 0xF00F);
  printf(data);
}
