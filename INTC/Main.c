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
  initIEP(0x30D3B);

  while(1){
    while (__R31 & HOST_INT){
      /* Clear Compare status */
      CT_IEP.TMR_CMP_STS = (1 << 0);

      /* delay for 5 cycles, clearing takes time */
      __delay_cycles(5);

      /* Clear the status of the interrupt */
      CT_INTC.SICR = 7;

      /* delay for 5 cycles, clearing takes time */
      __delay_cycles(5);

      /* Toggle pin */
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
  /* Clear any pending PRU-generated events */
  __R31 = 0x00000000;

  /* Connect sys_evt 7 to channel 1 */
  CT_INTC.CMR1_bit.CH_MAP_7 = 1;

  /* Connect channel 1 to host interrupt 1 */
  CT_INTC.HMR0_bit.HINT_MAP_1 = 1;

  /* Clear sys_evt 7 */
  CT_INTC.SICR = 7;

  /* Enable sys_evt 7 */
  CT_INTC.EISR = 7;

  /* Enable Host interrupt 1 */
	CT_INTC.HIEISR |= (1 << 0); /*TODO: Enable proper event */;

	// Globally enable host interrupts
	CT_INTC.GER = 1; /*TODO: Enable global events */;
}
