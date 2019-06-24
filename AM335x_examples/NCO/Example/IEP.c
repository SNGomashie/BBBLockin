#ifndef __AM335X_PRU_IEP_C_
#define __AM335X_PRU_IEP_C_

#include "IEP.h"

void IEP_reset(void){
  CT_IEP.TMR_GLB_CFG_bit.CNT_EN = 0x0000;  // Disable counter
  CT_IEP.TMR_CNT = 0xFFFFFFFF;  // Clear count register
  CT_IEP.TMR_GLB_STS = 0x0001;  // Clear overflow register
  CT_IEP.TMR_CMP_STS = 0x0001;  // Clear compare status
}

void IEP_mode(uint8_t mode){
  if (mode == 0) {
    CT_IEP.TMR_CMP_CFG = 0x0003;
  } else {
    /* code */
  }
}

void IEP_frequency(uint32_t frequency){
  uint32_t period = 100000000 / frequency;
  CT_IEP.TMR_CMP0 = period;
}

void IEP_period(uint32_t period){
  CT_IEP.TMR_CMP0 = period;
}

void IEP_increment(uint16_t increment){
  CT_IEP.TMR_GLB_CFG_bit.DEFAULT_INC = increment;
}

void IEP_start(void){
  /* Enable counter */
  CT_IEP.TMR_GLB_CFG_bit.CNT_EN = 0x0001;
}

void IEP_stop(void){
  /* Disable counter */
  CT_IEP.TMR_GLB_CFG_bit.CNT_EN = 0x0000;
}

void IEP_clear(void){
  /* Clear Compare status */
  CT_IEP.TMR_CMP_STS = (1 << 0);

  /* delay for 5 cycles, clearing takes time */
  __delay_cycles(5);
}

#endif
