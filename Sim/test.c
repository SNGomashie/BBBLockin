#include <stdio.h>
#include <stdint.h>



void main(void){
  uint32_t R_1 = 0x80000000;
  uint32_t R_2 = (1 << 31);
  uint32_t R_3 = 0;


  if(R_1 & R_2){
    printf("Y1\n");
  }

}
