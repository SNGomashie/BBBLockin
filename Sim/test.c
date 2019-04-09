#include <stdio.h>
#include <stdint.h>



void main(void){
  char data[] = "";
sprintf(data, "%d %d\n", 0x0000, 0xFFFF);
  printf(data);
}
