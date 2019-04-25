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
#include "resource_table.h"

#define PRU0_MEM 0x00000000
volatile uint32_t *pru0_mem =  (unsigned int *) PRU0_MEM;

void main(void){
  uint16_t result = 0;
  char data[16] = "";
  /* Clear output register */
  __R30 = 0x00000000;

  /* Clear SYSCFG[STANDBY_INIT] to enable OCP master port */
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

  /* Access PRCM (without CT) to initialize McSPI0 clock */
  PRCMinitialize();
  UARTinitialize();
  /* Initialize the McSPI module */
  McSPIinitialze(0x2, 0xF, 0x0);

  /* Initialize the LTC1859 adc */
  LTC1859initialize();
  
  while(1){
    result = LTC1859singletransfer(0, 1);
    sprintf(data, "-  %d", result);
    UARTtransmit(data);
  }


  __halt();
}
