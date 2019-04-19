#include <stdio.h>
#include <stdint.h>
#include <string.h>

uint16_t output[248];

void testfunc(uint16_t array);

void main(void){

for(uint16_t i = 0; i < 248; i++){
  output[i] = i+1;
}

uint16_t *ptrBLK = output;

testfunc(ptrBLK);
}

void testfunc(uint16_t* array){
  printf("%d\n", (sizeof(array)/sizeof(uint8_t)));
  printf("%d\n", (sizeof(output)/sizeof(uint8_t)));
}
