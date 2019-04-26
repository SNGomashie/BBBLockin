#include <stdint.h>
#include <stdio.h>
#include "resource_table_empty.h"
#include "pru_peripheral.h"

#define DEBUG_PIN 7

void main(void) {
  __R30 = 0x00000000;

  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;
  INTCinitialize(7, 1, 1);
  IEPinitialize(0x30D3B, 1, cmp);
  INTERNCOMinitialize(23);
  IEPstart();
  while(1){
    while(__R31 & (1 << 31)){
      IEPclear_int();
      INTCclear(7);
      INTERNCOMpoke();
    }
  }
  __halt();
}
