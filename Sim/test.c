#include <stdio.h>
#include <stdint.h>
#include <string.h>

void main(void){
uint32_t freq = 10000;
uint32_t num = 17;

uint32_t command = freq | (num << 16);

printf("%d, %d, %d, %d\n", command, (command >> 16), (command & 0x0000FFFF), num << 16);

}
