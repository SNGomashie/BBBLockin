#ifndef __LTC_1859_C_
#define __LTC_1859_C_

#include <stdio.h>
#include <stdint.h>
#include "LTC1859.h"

const uint16_t ADCch[] = {0, 4, 1, 5, 2, 6, 3, 7};

void LTC1859initialize(void){
  __R30 = 0x00000000;

  while(!(CT_MCSPI0.SYSSTATUS_bit.RESETDONE == 0x1));

  /* Set pins */
  __R30 &= ~(1 << CS);
  __R30 |= (1 << _RD);
  __R30 |= (1 << CONVST);
  __delay_cycles(100);
}

uint16_t LTC1859singletransfer(uint8_t chan, uint8_t mode){

  __R30 |= (1 << CS);

  uint16_t SPIsend = 0;
switch(mode){
  case 0:
    SPIsend = (ADCch[chan] << 12) | 0b0000000000000000; // single-ended, input +/-5V
  case 1:
    SPIsend = (ADCch[chan] << 12) | 0b1000100000000000; // single-ended, input 0V to 5V
  case 2:
    SPIsend = (ADCch[chan] << 12) | 0b1000010000000000; // single-ended, input +/-10V
  case 3:
    SPIsend = (ADCch[chan] << 12) | 0b1000110000000000; // single-ended, input 0V to 10V
  default:
    SPIsend = (ADCch[chan] << 12) | 0b1000100000000000; // single-ended, input 0V to 5V
}
  /* Check if ADC is busy with conversion and continue if not*/
  while(!(__R31 & (1 << _BUSY)));
  /* pull down CONVST and _RD */
  __R30 &= ~(1 << CONVST);
  __R30 &= ~(1 << _RD);

  CT_MCSPI0.CH0CONF_bit.FORCE = 0x1;
  CT_MCSPI0.CH0CTRL_bit.EN = 0x1;

  /* Check if McSPI TX register is empty, if it is continue */
  while(!(CT_MCSPI0.IRQSTATUS_bit.TX0_EMPTY == 0x1));

  /* Write word to be transmitted into TX register */
  CT_MCSPI0.TX0 = SPIsend;

  /* Check if McSPI RX register is full, if it is continue */
  while(!(CT_MCSPI0.IRQSTATUS_bit.RX0_FULL == 0x1));

  CT_MCSPI0.CH0CTRL_bit.EN = 0x0;
  /* Clear interrupts */
  CT_MCSPI0.IRQSTATUS = 0xFFFF;

  /* Start conversion, Stop reading */
  __R30 |= (1 << CONVST);
  // __R30 |= (1 << _RD);

  /* Delay until conversion starts */
  __delay_cycles(100);
  __R30 &= ~(1 << CONVST);
  /* Wait until Conversion is done */
  while(!(__R31 & (1 << _BUSY)));

  /* pull down CONVST and _RD */

  __R30 &= ~(1 << _RD);

  CT_MCSPI0.CH0CTRL_bit.EN = 0x1;
  /* Write nothing to TX Register */
  CT_MCSPI0.TX0 = 0x0000;

  /* Wait until RX register is full */
  while(!(CT_MCSPI0.IRQSTATUS_bit.RX0_FULL == 0x1));

  CT_MCSPI0.CH0CTRL_bit.EN = 0x0;

  CT_MCSPI0.CH0CONF_bit.FORCE = 0x0;

  /* Clear interrupts */
  CT_MCSPI0.IRQSTATUS = 0xFFFF;



  /* Start conversion */
  // __R30 |= (1 << CONVST);
  // __R30 |= (1 << _RD);

  return CT_MCSPI0.RX0;
}


// uint16_t LTC1859singletransfer(uint8_t chan){
//   // uint16_t SPIsend = (ADCch[chan] << 12) | 0b0000000000000000; // single-ended, input +/-5V
//   uint16_t SPIsend = (ADCch[chan] << 12) | 0b1000100000000000; // single-ended, input 0V to 5V
//   // uint16_t SPIsend = (ADCch[chan] << 12) | 0b1000010000000000; // single-ended, input +/-10V
//   // uint16_t SPIsend = (ADCch[chan] << 12) | 0b1000110000000000; // single-ended, input 0V to 10V
//   // uint16_t SPIsend = 0b1010100000000000;
//   /* Check if ADC is busy with conversion and continue if not*/
//   while(!(__R31 & (1 << _BUSY)));
//   /* pull down CONVST and _RD */
//   __R30 &= ~(1 << CONVST);
//   __R30 &= ~(1 << _RD);
//
//   CT_MCSPI0.CH0CONF_bit.FORCE = 0x1;
//   CT_MCSPI0.CH0CTRL_bit.EN = 0x1;
//
//   /* Check if McSPI TX register is empty, if it is continue */
//   while(!(CT_MCSPI0.IRQSTATUS_bit.TX0_EMPTY == 0x1));
//
//   /* Write word to be transmitted into TX register */
//   CT_MCSPI0.TX0 = SPIsend;
//
//   /* Check if McSPI RX register is full, if it is continue */
//   while(!(CT_MCSPI0.IRQSTATUS_bit.RX0_FULL == 0x1));
//
//   CT_MCSPI0.CH0CTRL_bit.EN = 0x0;
//   /* Clear interrupts */
//   CT_MCSPI0.IRQSTATUS = 0xFFFF;
//
//   /* Start conversion, Stop reading */
//   __R30 |= (1 << CONVST);
//   // __R30 |= (1 << _RD);
//
//   /* Delay until conversion starts */
//   __delay_cycles(100);
//   __R30 &= ~(1 << CONVST);
//   /* Wait until Conversion is done */
//   while(!(__R31 & (1 << _BUSY)));
//
//   /* pull down CONVST and _RD */
//
//   __R30 &= ~(1 << _RD);
//
//   CT_MCSPI0.CH0CTRL_bit.EN = 0x1;
//   /* Write nothing to TX Register */
//   CT_MCSPI0.TX0 = 0x0000;
//
//   /* Wait until RX register is full */
//   while(!(CT_MCSPI0.IRQSTATUS_bit.RX0_FULL == 0x1));
//
//   CT_MCSPI0.CH0CTRL_bit.EN = 0x0;
//
//   CT_MCSPI0.CH0CONF_bit.FORCE = 0x0;
//
//   /* Clear interrupts */
//   CT_MCSPI0.IRQSTATUS = 0xFFFF;
//
//
//
//   /* Start conversion */
//   // __R30 |= (1 << CONVST);
//   // __R30 |= (1 << _RD);
//
//   return CT_MCSPI0.RX0;
// }

// uint16_t LTC1859transfer(uint8_t chan[]){
//   /* Set pins */
//   /* TODO: Can this happen outside the function? */
//   __R30 |= (1 << CS);
//   __R30 |= (1 << _RD);
//
//   uint8_t i;
//   uint8_t channels;
//   channels = sizeof(chan);
//
//   for(i = 0; i < (channels; i++){
//     uint8_t j = 0;
//     // uint16_t SPIsend = (ADCch[chan] << 12) | 0b1000000000000000; // single-ended, input +/-5V
//     uint16_t SPIsend = (ADCch[chan[i]] << 12) | 0b1000100000000000; // single-ended, input 0V to 5V
//     // uint16_t SPIsend = (ADCch[chan] << 12) | 0b1000010000000000; // single-ended, input +/-10V
//     // uint16_t SPIsend = (ADCch[chan] << 12) | 0b1000110000000000; // single-ended, input 0V to 10V
//
//     /* Check if ADC is busy with conversion and continue if not*/
//     while(!(__R31 & (1 << _BUSY)));
//
//     /* pull down CONVST and _RD */
//     __R30 &= ~(1 << CONVST);
//     __R30 &= ~(1 << _RD);
//
//     /* Check if McSPI TX register is empty, if it is continue */
//     while(!(CT_MCSPI0.IRQSTATUS_bit.TX0_EMPTY == 0x1));
//
//     /* Write word to be transmitted into TX register */
//     CT_MCSPI0.TX0 = SPIsend;
//
//     /* Check if McSPI RX register is full, if it is continue */
//     while(!(CT_MCSPI0.IRQSTATUS_bit.RX0_FULL == 0x1));
//
//     if( j >= 1){
//       /* Return value in RX register */
//       results[i] = CT_MCSPI0.RX0;
//       j++
//     } else{
//       CT_MCSPI0.RX0 = 0x0;
//     }
//
//     /* Clear interrupts */
//     CT_MCSPI0.IRQSTATUS = 0xFFFF;
//
//     /* Start conversion, Stop reading */
//     __R30 |= (1 << CONVST);
//     __R30 |= (1 << _RD);
//
//     /* Delay until conversion starts */
//     while((__R31 & (1 << _BUSY)));
//
//     /* Wait until Conversion is done */
//     while(!(__R31 & (1 << _BUSY)));
//
//   }
//   return results
// }

#endif
