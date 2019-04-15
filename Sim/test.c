#include <stdio.h>
#include <stdint.h>

void main(void){

uint32_t a = (5 * (1 << 24) / 4);
uint32_t b = (10 - 5);
uint32_t c = (a * b) >> 8;
uint32_t d = 0x00FFFFFF & a;
printf("%d, %d, %d, %d", (a >> 24), b, c, d);
}
