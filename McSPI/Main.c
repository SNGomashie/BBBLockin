//  Attempt to read the
//  SPI ADC read program in C.
//
//	Pin configuration:
//  CS	( chip select ):  					P9.17 cs
//  SDI ( Serial Data In (MOSI)):   P9.18 spi
//  SDO ( Serial Data Out (MISO)):  P9.21 sdo
//  SCK ( Serial clock ) :      		P9.22 sck
//  nRD (  Read Input ) :						P9.25 pr1_pru0_pru_r30_7
//  BUSY_ (conversion done):					P9.26 pr1_pru0_pru_r30_16
//  Convst (  Start conversion ) : 	P9.31 pr1_pru0_pru_r30_0

/* Includes */
#include <stdint.h>
#include <pru_cfg.h>
#include <pru_ctrl.h>
#include <sys_mcspi.h>
#include "resource_table.h"

//Define pin locations
#define CS
#define SDI
#define SDO
#define SCK
#define _RD 7
#define _BUSY 16
#define CONVST 0

volatile register uint32_t __R30;
volatile register uint32_t __R31;

void initSPI(void)
void SPItransfer(uint8_t chan)

void main(void){
  /* Clear SYSCFG[STANDBY_INIT] to enable OCP master port */
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

  while(1){
    __R30 |= (1 << CS)
    __R30 &= ~(1 << CS)
  }
}
