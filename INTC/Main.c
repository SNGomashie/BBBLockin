/*           Direct Digital Synthesis (DDS)            */
/* Generate every frequency ( < 2/fs ) from a sine LUT */
/* Capture the period of a function generator SYNC output*/
/*  And generate a sinewave with a corresponding period  */

#include <stdint.h>
#include <stdio.h>
#include <pru_cfg.h>
#include <pru_ctrl.h>
#include "resource_table.h"
#include "pru_peripheral.h"

#define PIN25 7
#define HOST_INT (1 << 31)

void main(void){
  __R30 = 0x00000000;

  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;
  INTCinitialize(7, 1, 1);
  IEPinitialize(0x30D3B, 1, cmp);
  IEPstart();

  while(1){
    while (__R31 & HOST_INT){
      /* Clear Compare status */
      CT_IEP.TMR_CMP_STS = (1 << 0);

      /* delay for 5 cycles, clearing takes time */
      __delay_cycles(5);

      INTCclear(7);

      /* Toggle pin */
      __R30 ^= 1 << PIN25;
    }
  }
  __halt();
}
