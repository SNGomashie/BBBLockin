#include <stdio.h>
#include <stdint.h>
#include "pru_peripheral.h"
#include "resource_table.h"


void main(void){
  char data[16] = "";
  char data2[16] = "";

  UARTinitialize(115200);

  while(1){
  __R30 |= (1 << 7);
    sprintf(data, "%d\n", 0x0000, 0xFFFF, 0xF0F0, 0x0F0F);
    UARTtransmit(data);
  __R30 &= ~(1 << 7);
    __delay_cycles(200000);
  }
  __halt();
}
