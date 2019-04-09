#include <stdio.h>
#include <stdint.h>



void main(void){
  uint32_t a = 0x80 | 0x8 | 0x4 | 0x2| 0x01;
  printf(a);
}
