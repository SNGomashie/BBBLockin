#include <stdint.h>
#include <stdio.h>
#include "resource_table_empty.h"
#include "pru_peripheral.h"

#define DEBUG_PIN 7

void main(void) {
  while(1){
    while(__R31 & (1 << 31)){
      __R30 ^= (1 << DEBUG_PIN);
    }
  }
  __halt();
}
