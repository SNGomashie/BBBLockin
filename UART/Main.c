#include <stdio.h>
#include <stdint.h>
#include "pru_peripheral.h"
#include "resource_table.h"

void main(void){
  char data[16] = "";

  UARTinitialize();

  while(1){
  __R30 |= (1 << 7);
    sprintf(data, "%d\n", 0x0000);
    UARTtransmit(data);
  __R30 &= ~(1 << 7);
    __delay_cycles(200000);
  }
  __halt();
}
