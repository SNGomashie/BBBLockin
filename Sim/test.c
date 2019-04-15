#include <stdio.h>
#include <stdint.h>



void main(void){
  uint32_t R_1 = 0x11000000;
  uint32_t R_2 = 0;
  uint32_t mask = 0x0FFFFFFF;

  R_2 = mask & R_1;

  char data[] = "";
  sprintf(data, "%d, %d\n", R_1, R_2);
  printf("%s\n", data);



}
