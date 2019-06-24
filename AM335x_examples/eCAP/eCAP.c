#ifndef __AM335X_PRU_ECAP_C_
#define __AM335X_PRU_ECAP_C_

#include "eCAP.h"

void eCAP_polarity(uint8_t event, uint8_t polarity){
  switch(event){
    case 0:
      if(polarity == 0){
        CT_ECAP.ECCTL1 &= ~(1 << CAP1POL);
      } else {
        CT_ECAP.ECCTL1 |= (1 << CAP1POL);
      }
      break;
    case 1:
      if(polarity == 0){
        CT_ECAP.ECCTL1 &= ~(1 << CTRRST2);
      } else {
        CT_ECAP.ECCTL1 |= (1 << CTRRST2);
      }
      break;
    case 2:
      if(polarity == 0){
        CT_ECAP.ECCTL1 &= ~(1 << CAP3POL);
      } else {
        CT_ECAP.ECCTL1 |= (1 << CAP3POL);
      }
      break;
    case 3:
      if(polarity == 0){
        CT_ECAP.ECCTL1 &= ~(1 << CAP4POL);
      } else {
        CT_ECAP.ECCTL1 |= (1 << CAP4POL);
      }
      break;
  }
}

void eCAP_reset(uint8_t event, uint8_t mode){
  switch(event){
    case 0:
      if(mode == 0){
        CT_ECAP.ECCTL1 &= ~(1 << CTRRST1);
      } else {
        CT_ECAP.ECCTL1 |= (1 << CTRRST1);
      }
      break;
    case 1:
      if(mode == 0){
        CT_ECAP.ECCTL1 &= ~(1 << CAP2POL);
      } else {
        CT_ECAP.ECCTL1 |= (1 << CAP2POL);
      }
      break;
    case 2:
      if(mode == 0){
        CT_ECAP.ECCTL1 &= ~(1 << CTRRST3);
      } else {
        CT_ECAP.ECCTL1 |= (1 << CTRRST3);
      }
      break;
    case 3:
      if(mode == 0){
        CT_ECAP.ECCTL1 &= ~(1 << CTRRST4);
      } else {
        CT_ECAP.ECCTL1 |= (1 << CTRRST4);
      }
      break;
  }
}

void eCAP_load(uint8_t load){
  if(load == 0){
    CT_ECAP.ECCTL1 &= ~(1 << CAPLDEN);
  }else {
    CT_ECAP.ECCTL1 |= (1 << CAPLDEN);
  }
}

void eCAP_prescaler(uint8_t scaler){
  CT_ECAP.ECCTL1 |= (scaler << PRESCALE);
}

void eCAP_mode(uint8_t mode){
  if(mode == 0){
    CT_ECAP.ECCTL2 &= ~(1 << CAP_APWM);
  }else{
    CT_ECAP.ECCTL2 |= (1 << CAP_APWM);
  }
}

void eCAP_continuous(uint8_t mode){
  if (mode == 0) {
    CT_ECAP.ECCTL2 &= ~(1 << CONT_ONESHT);
  } else {
    CT_ECAP.ECCTL2 |= (1 << CONT_ONESHT);
  }
}

void eCAP_sync(uint8_t out, uint8_t in){
  	CT_ECAP.ECCTL2 |= (out << SYNCO_SEL);
    if (in == 0) {
      CT_ECAP.ECCTL2 &= ~(1 << SYNCI_EN);
    } else {
      CT_ECAP.ECCTL2 |= (1 << SYNCI_EN);
    }
}

void eCAP_wrap(uint8_t reg){
  switch (reg) {
    case 0:
        CT_ECAP.ECCTL2 &= ~(1 << STOP_WRAP);
        break;
    case 1:
        CT_ECAP.ECCTL2 |= (1 << STOP_WRAP);
        break;
    case 2:
        CT_ECAP.ECCTL2 |= (2 << STOP_WRAP);
        break;
    case 3:
        CT_ECAP.ECCTL2 |= (3 << STOP_WRAP);
        break;
  }
}

void eCAP_start(void){
  CT_ECAP.ECCTL2 |= (1 << TSCTRSTOP);
}

void eCAP_stop(void){
  CT_ECAP.ECCTL2 &= ~(1 << TSCTRSTOP);
}

#endif
