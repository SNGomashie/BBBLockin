#include <stdio.h>
#include <stdint.h>
#include <string.h>

void main(void){
uint16_t freq = 10000;
uint16_t num = 4096;

uint32_t command = freq | (num << 16);

printf("%d, %d, %d\n", command, (command >> 16), (command & 0x0000FFFF));

}
