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

#define PIN25 7
#define HOST_INT (1 << 31)

volatile register unsigned int __R30;
volatile register unsigned int __R31;

void initIEP(uint32_t comp);
void initINTC(void);

void main(void){
  __R30 = 0x00000000;


  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

  initINTC();
  initIEP(0x1312D00);

  while(1){
    while (__R31 & HOST_INT){
      /* Clear Compare status */
      CT_IEP.TMR_CMP_STS = 0xFF;
      /* Clear the status of the interrupt */
      CT_INTC.SECR0 = (1 << 7);

      __R30 ^= 1 << PIN25;
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
  CT_IEP.TMR_CNT = 0x0;

  /* Clear overflow register */
  CT_IEP.TMR_GLB_STS_bit.CNT_OVF = 0x1;

  /* Set compare values */
  CT_IEP.TMR_CMP0 = comp;

  /* Clear compare status */
  CT_IEP.TMR_CMP_STS_bit.CMP_HIT = 0xFF;

  /* Disable compensation */
  CT_IEP.TMR_COMPEN_bit.COMPEN_CNT = 0x0;

  /* Enable CMP0 and reset on event */
  CT_IEP.TMR_CMP_CFG_bit.CMP0_RST_CNT_EN = 0x1;
  CT_IEP.TMR_CMP_CFG_bit.CMP_EN = 0x1;

  /* Set increment to 1 (default = 5)*/
  CT_IEP.TMR_GLB_CFG_bit.DEFAULT_INC = 0x0001;

  /* Enable counter */
  CT_IEP.TMR_GLB_CFG = 0x11;
}

/*               Initialize interrupts               */
/* Interrupt from sys_event 7 to channel 0 to host 0 */
void initINTC(void){
  __R31 = 0x00000000;
  CT_INTC.CMR1_bit.CH_MAP_7 = 1;
  CT_INTC.HMR0_bit.HINT_MAP_1 = 1;
  CT_INTC.SICR = 7;
  CT_INTC.EISR = 7;
  CT_INTC.HIEISR |= (1 << 0);
  CT_INTC.SECR0 = 0xFFFFFFFF;
  CT_INTC.SECR1 = 0xFFFFFFFF;
  // /* Enable sys_event */
  // CT_INTC.EISR_bit.EN_SET_IDX = 0x7;
  //
  // /* Enable host int */
  // CT_INTC.HIEISR_bit.HINT_EN_SET_IDX = 0x0;
  //
  // /* Set type and polarity of system events (SIPRx / SITRx) */
  // CT_INTC.SIPR0 |= (1 << 7);
  // CT_INTC.SITR0 &= ~(1 << 7);
  //
  // /* Map sys_event to INTC channel (CHANMAP) */
  // CT_INTC.CMR1_bit.CH_MAP_7 = 0x0;
  //
  // /* Map channel to host interrupt (HOSTMAP) */
  // CT_INTC.HMR0_bit.HINT_MAP_0 = 0x0;
  //
  // /* Clear all sys_events (SECR) */
  // CT_INTC.SECR0 |= (1 << 7);
  //
  // /* Enable host interrupt (HIER) */
  // CT_INTC.HIER |= (1 << 0);
  //
  // /* Globally enable all interrupts (GER) */
  // CT_INTC.GER = 0x1;
}
