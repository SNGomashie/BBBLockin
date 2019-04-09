#include <stdio.h>
#include <stdint.h>



void main(void){
  char data[] = "";
sprintf(data, "%d %d %d\n", 0x0000, 0xFFFF, 0xF0F0);
  printf(data);
}
