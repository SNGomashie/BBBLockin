#ifndef __LTC_1859_C_
#define __LTC_1859_C_

#include <stdio.h>
#include <stdint.h>
#include "LTC1859.h"


void LTC1859initialize(void){
  __R30 = 0x00000000;

  while(!(CT_MCSPI0.SYSSTATUS_bit.RESETDONE == 0x1));
  CT_MCSPI0.CH0CONF_bit.FORCE = 0x1;
  /* Set pins */
  __R30 |= (1 << CS);

  /* Start converson to clock in SPI word */
  __R30 |= (1 << CONVST);

  /* Delay so conversion starts before continuing */
  __delay_cycles(50);

  /* pull down CONVST */
  __R30 &= ~(1 << CONVST);

  /* Wait until conversion is done */
  while(!(__R31 & (1 << _BUSY)));
}

uint16_t LTC1859transfer(uint16_t spi_word){
  /* Enable McSPI channel */
  CT_MCSPI0.CH0CTRL_bit.EN = 0x1;

  /* Check if McSPI TX register is empty, if it is continue */
  while(!(CT_MCSPI0.IRQSTATUS_bit.TX0_EMPTY == 0x1));

  /* Write word to be transmitted into TX register */
  CT_MCSPI0.TX0 = spi_word;

  /* Check if McSPI RX register is full, if it is continue */
  while(!(CT_MCSPI0.IRQSTATUS_bit.RX0_FULL == 0x1));

  /* Disable McSPI channel */
  CT_MCSPI0.CH0CTRL_bit.EN = 0x0;

  /* Clear interrupts */
  CT_MCSPI0.IRQSTATUS = 0xFFFF;

  /* Return */
  return CT_MCSPI0.RX0;
}

uint16_t LTC1859readout(uint8_t chan, uint8_t mode){
  const uint8_t ADCch[] = {0, 4, 1, 5, 2, 6, 3, 7};
  uint16_t SPIsend = 0;
  uint16_t result = 0;

  switch(mode){
    case 0:
      SPIsend = (ADCch[chan] << 12) | 0b0000000000000000; // single-ended, input +/-5V
    case 1:
      SPIsend = (ADCch[chan] << 12) | 0b1000100000000000; // single-ended, input 0V to 5V
    case 2:
      SPIsend = (ADCch[chan] << 12) | 0b1000010000000000; // single-ended, input +/-10V
    case 3:
      SPIsend = (ADCch[chan] << 12) | 0b1000110000000000; // single-ended, input 0V to 10V
  }

  while(!(__R31 & (1 << _BUSY)));

  result = LTC1859transfer(SPIsend);

  LTC1859conversion(CONVST);

  return result;
}

void LTC1859conversion(uint8_t pin){
  /* Start conversion, Stop reading */
  __R30 |= (1 << pin);

  /* Delay until conversion starts */
  __delay_cycles(50);

  /* pull down CONVST */
  __R30 &= ~(1 << pin);
}

#endif
