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

/* Define pin locations */
#define CS
#define SDI
#define SDO
#define SCK
#define _RD 7
#define _BUSY 16
#define CONVST 0

/* Input and output registers */
volatile register uint32_t __R30;
volatile register uint32_t __R31;

/* Function declaration */
void initSPImod(void);
void initSPIchan(void);
void SPItransfer(uint8_t chan);

void main(void){

  __R30 = 0x00000000;

  /* Clear SYSCFG[STANDBY_INIT] to enable OCP master port */
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

  /* Initialize the McSPI module */
  initSPImod();

 // /* Initialize channel 0 of the McSPI module */
 //  initSPIchan();

 /* Set pins */
 __R30 |= (1 << _RD);
 __R30 |= (1 << CONVST);
}


void initSPImod(void){
  /* Reset McSPI0 module */
  CT_MCSPI0.SYSCONFIG_bit.SOFTRESET = 0x1;

  /* Wait until reset is done */
  // while((CT_MCSPI0.SYSSTATUS_bit.RESETDONE == 0x0));

  /* Set SPI module to Master Mode */
  // CT_MCSPI0.MODULCTRL_bit.MS = 0x0;
}


void initSPIchan(void){
  /* Reset interrupt status */
  CT_MCSPI0.IRQSTATUS = 0xFFFF;

  /* Configure interrupts */
  CT_MCSPI0.IRQENABLE = 0x0;

  // Set clock devider, SPI clock = 48MHz, Device clock = 20Mhz. devider = 4;
  CT_MCSPI0.CH0CONF_bit.CLKD = 0x2;

  // Set CS polarity
  CT_MCSPI0.CH0CONF_bit.EPOL = 0x0;

  /* Set world length to 16bit */
  CT_MCSPI0.CH0CONF_bit.WL = 0xF;

  // Set transfer and receive mode
  CT_MCSPI0.CH0CONF_bit.TRM = 0x0;

  // Set SPID0 as not a transmissionline
  CT_MCSPI0.CH0CONF_bit.DPE0 = 0x1;

  // Set SPID1 as transmissionline
  CT_MCSPI0.CH0CONF_bit.DPE1 = 0x0;

  // Set SPID0 as input
  CT_MCSPI0.CH0CONF_bit.IS = 0x0;
}
