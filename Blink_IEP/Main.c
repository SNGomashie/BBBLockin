/*           Direct Digital Synthesis (DDS)            */
/* Generate every frequency ( < 2/fs ) from a sine LUT */
/* Capture the period of a function generator SYNC output*/
/*  And generate a sinewave with a corresponding period  */

#include <stdint.h>
#include <stdio.h>
#include <pru_cfg.h>
#include <pru_iep.h>
#include <pru_ctrl.h>
#include "resource_table.h"

#define LED 7

volatile register unsigned int __R30;
volatile register unsigned int __R31;

void initIEP(uint32_t comp);

void main(void){
  __R30 = 0x00000000;

  initIEP(0x4E20);

  while(1){
    while(!(CT_IEP.TMR_CMP_STS == 0x00)){
      __R30 ^= 1 << LED;
      CT_IEP.TMR_CMP_STS = 0x0001;
    }
  }
  __halt();
}



/*     Initialize IEP module      */
/*    defines sample frequency    */
/* comp is sample period in cycles*/
void initIEP (uint32_t comp){
  /* sample period = timer period*/

  /* Disable counter */
  CT_IEP.TMR_GLB_CFG_bit.CNT_EN = 0x0000;

  /* Clear CNT register */
  CT_IEP.TMR_CNT = 0xFFFFFFFF;

  /* Clear overflow register */
  CT_IEP.TMR_GLB_STS = 0x0001;

  /* Clear compare status */
  CT_IEP.TMR_CMP_STS = 0x0001;

  /* Set compare values */
  CT_IEP.TMR_CMP0 = comp;

  /* Enable compare event */
  CT_IEP.TMR_CMP_CFG = 0x0003;

  /* Set increment to 1 (default = 5)*/
  CT_IEP.TMR_GLB_CFG_bit.DEFAULT_INC = 0x0001;

  /* Enable counter */
  CT_IEP.TMR_GLB_CFG_bit.CNT_EN = 0x0001;
}
