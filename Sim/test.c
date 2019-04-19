#include <stdio.h>
#include <stdint.h>
#include <string.h>


void main(void){

uint16_t output[248];

for(uint16_t i = 0; i < 248; i++){
  output[i] = i+1;
}

uint16_t *ptrBLK = output;

printf("%d\n", (sizeof(output)/sizeof(uint8_t)));
}
