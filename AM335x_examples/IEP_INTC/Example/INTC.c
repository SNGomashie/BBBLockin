#ifndef __AM335X_PRU_INTC_C_
#define __AM335X_PRU_INTC_C_

#include "INTC.h"

void INTC_initialize(uint8_t sys_evt, uint8_t chan, uint8_t host_int){
  // Clear current interrupts
  __R31 = 0x00000000;

  // Configure INTC
  if (sys_evt < 4) {
    CT_INTC.CMR0 |= (chan << (sys_evt * 8));
  } else if (sys_evt < 8) {
    CT_INTC.CMR1 |= (chan << ((sys_evt * 8) - 32));
  } else if (sys_evt < 12) {
    CT_INTC.CMR2 |= (chan << ((sys_evt * 8) - 64));
  } else if (sys_evt < 16) {
    CT_INTC.CMR3 |= (chan << ((sys_evt * 8) - 96));
  } else if (sys_evt < 20) {
    CT_INTC.CMR4 |= (chan << ((sys_evt * 8) - 128));
  } else if (sys_evt < 24) {
    CT_INTC.CMR5 |= (chan << ((sys_evt * 8) - 160));
  } else if (sys_evt < 28) {
    CT_INTC.CMR6 |= (chan << ((sys_evt * 8) - 192));
  } else if (sys_evt < 32) {
    CT_INTC.CMR7 |= (chan << ((sys_evt * 8) - 224));
  } else if (sys_evt < 36) {
    CT_INTC.CMR8 |= (chan << ((sys_evt * 8) - 256));
  } else if (sys_evt < 40) {
    CT_INTC.CMR9 |= (chan << ((sys_evt * 8) - 288));
  } else if (sys_evt < 44) {
    CT_INTC.CMR10 |= (chan << ((sys_evt * 8) - 320));
  } else if (sys_evt < 48) {
    CT_INTC.CMR11 |= (chan << ((sys_evt * 8) - 352));
  } else if (sys_evt < 52) {
    CT_INTC.CMR12 |= (chan << ((sys_evt * 8) - 384));
  } else if (sys_evt < 56) {
    CT_INTC.CMR13 |= (chan << ((sys_evt * 8) - 416));
  } else if (sys_evt < 60) {
    CT_INTC.CMR14 |= (chan << ((sys_evt * 8) - 448));
  } else if (sys_evt < 64) {
    CT_INTC.CMR15 |= (chan << ((sys_evt * 8) - 480));
  }

  if (chan < 4) {
    CT_INTC.HMR0 |= (host_int << (chan * 8));
  } else if (chan < 8) {
    CT_INTC.HMR1 |= (host_int << ((chan * 8) - 32));
  } else if (chan < 12) {
    CT_INTC.HMR2 |= (host_int << ((chan * 8) - 64));
  }

  CT_INTC.SICR = sys_evt;
  CT_INTC.EISR |= sys_evt;
  CT_INTC.HIEISR |= host_int;

  // Enable global interrupts
  CT_INTC.GER = 1;
}

void INTC_clear(uint8_t sys_evt){
  CT_INTC.SICR = sys_evt;
  __delay_cycles(5);
}

void INTC_poke(uint8_t inter){
  __R31 |= inter;
}

void INTC_listen(uint8_t prus, uint8_t inter){
  switch(prus){
    case 0:
    while(!(__R31 & (1 << 30)));
    INTCclear(inter);
    __delay_cycles(5);
    break;
    case 1:
    while(!(__R31 & (1 << 31)));
    INTCclear(inter);
    __delay_cycles(5);
    break;
  }
}

#endif
