/*           Direct Digital Synthesis (DDS)            */
/* Generate every frequency ( < 2/fs ) from a sine LUT */
/* Capture the period of a function generator SYNC output*/
/*  And generate a sinewave with a corresponding period  */

#include <stdint.h>
#include <stdio.h>
#include <pru_cfg.h>
#include <pru_intc.h>
#include <pru_iep.h>
#include <pru_ctrl.h>
#include "resource_table.h"

#define LED 7
#define TEST 5
#define HOST_INT 30

volatile register unsigned int __R30;
volatile register unsigned int __R31;

void initIEP(uint32_t comp);

void main(void){
  __R30 = 0x00000000;

  initIEP(0x4E20);
  initINTC();
  
  __R30 ^= 1 << TEST;

  while(1){
    if(__R31 & (1 << HOST_INT)){
      __R30 ^= 1 << LED;
      CT_INTC.SICR |= (1 << 7);
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

/*               Initialize interrupts               */
/* Interrupt from sys_event 7 to channel 0 to host 0 */
void initINTC(void){
  /* Enable sys_event */
  CT_INTC.EISR_bit.EN_SET_IDX = 0x7;

  /* Enable host int */
  CT_INTC.HIEISR_bit.HINT_EN_SET_IDX = 0x0;

  /* Set type and polarity of system events (SIPRx / SITRx) */
  CT_INTC.SIPR0 |= (1 << 7);
  CT_INTC.SITR0 &= ~(1 << 7);

  /* Map sys_event to INTC channel (CHANMAP) */
  CT_INTC.CMR1_bit.CH_MAP_7 = 0x0;

  /* Map channel to host interrupt (HOSTMAP) */
  CT_INTC.HMR0_bit.HINT_MAP_0 = 0x0;

  /* Clear all sys_events (SECR) */
  CT_INTC.SECR0 |= (1 << 7);

  /* Enable host interrupt (HIER) */
  CT_INTC.HIER |= (1 << 0);

  /* Globally enable all interrupts (GER) */
  CT_INTC.GER = 0x1;
}
