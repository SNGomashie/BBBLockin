//  Attempt to use the McSPI chip to read the ADC
//
//	Pin configuration:
//  CS	( chip select ):  					P9.27 pr1_pru0_pru_r30_5
//  CS	( McSPI chip select ):  		P9.17 cs
//  SDI ( Serial Data In (MOSI)):   P9.18 spi
//  SDO ( Serial Data Out (MISO)):  P9.21 sdo
//  SCK ( Serial clock ) :      		P9.22 sck
//  nRD (  Read Input ) :						P9.25 pr1_pru0_pru_r30_7
//  BUSY_ (conversion done):				P9.24 pr1_pru0_pru_r31_16
//  Convst (  Start conversion ) : 	P9.31 pr1_pru0_pru_r30_0

/* Includes */
#include <stdint.h>
#include <stdio.h>
#include "pru_peripheral.h"
#include "LTC1859.h"
#include "pru_rpmsg_lib.h"

#define HOST_INT (1 << 31)

#define PRU0_MEM 0x00000000
volatile uint32_t *pru0_mem =  (unsigned int *) PRU0_MEM;

uint16_t blkdata[248];

void main(void){
  uint16_t adc_out = 0;
  char* RPMsg_in;

  /* Clear output register */
  __R30 = 0x00000000;

  /* Clear SYSCFG[STANDBY_INIT] to enable OCP master port */
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

  RPMSGinitialize();
  INTCinitialize(7, 1, 1);
  /* Access PRCM (without CT) to initialize McSPI0 clock */
  PRCMinitialize();
  /* Initialize the McSPI module */
  McSPIinitialze(0x5, 0xF, 0x0);
  /* Initialize the LTC1859 adc */
  LTC1859initialize();
  IEPinitialize(20000, 1, cmp);
  IEPstart();

  while(1){
    while(__R31 & HOST_INT){
      IEPclear_int();
      INTCclear(7);
      adc_out = LTC1859readout(0, 1);
      blkdata[x] = adc_out;
      x++;
      if(!(x < 248)){
        RPMSGtransmit_block(blkdata);
        x = 0;
        i++;
        if(!(i < amount_samp)){
          IEPstop();
        }
      }
    // sprintf(data, "%d\n", adc_out);
    // UARTtransmit(data);
    // __delay_cycles(0x1312D00);
    }
  }

  __halt();
}
