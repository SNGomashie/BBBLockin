#include <stdio.h>
#include <stdint.h>

void main(void){

// inputs
uint64_t out1 = 10;
uint64_t out2 = 15;

// fixed point number
uint64_t t = 16777216;

uint64_t temp_out = 0;

temp_out = out2-out1;

temp_out *= t;

printf("%d", temp_out);
}
