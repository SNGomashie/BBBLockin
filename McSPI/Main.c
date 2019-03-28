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
#include <pru_cfg.h>
#include <pru_ctrl.h>
#include <sys_mcspi.h>
#include "resource_table.h"

/* Define pin locations */
#define CS      5
#define SDI
#define SDO
#define SCK
#define _RD     7
#define _BUSY   16
#define CONVST  0

/* Input and output registers */
volatile register uint32_t __R30;
volatile register uint32_t __R31;

/* PRCM (Power Reset Clock Module) Registers */
/* Base adress for the clock control module */
#define CM_PER_BASE	((volatile uint8_t *)(0x44E00000))
/* Adress for the SPI clk control register */
#define SPI0_CLKCTRL  (0x4C)
#define ON (0x2)

#define PRU0_MEM 0x00000000
volatile uint32_t *pru0_mem =  (unsigned int *) PRU0_MEM;

/* Function declaration */
void initSPImod(void);
void initSPIchan(void);
uint16_t SPItransfer(uint8_t chan);

void main(void){
  uint32_t cycle;
  volatile uint8_t *ptr_cm;

  /* Clear output register */
  __R30 = 0x00000000;

  /* Clear SYSCFG[STANDBY_INIT] to enable OCP master port */
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

  /* Access PRCM (without CT) to initialize McSPI0 clock */
  ptr_cm = CM_PER_BASE;
  ptr_cm[SPI0_CLKCTRL] = ON;

  /* Initialize the McSPI module */
  initSPImod();

  /* Initialize channel 0 of the McSPI module */
  initSPIchan();

  /* Set pins */
  __R30 |= (1 << CS);
  __R30 |= (1 << _RD);

  PRU0_CTRL.CTRL_bit.CTR_EN = 1;  // Enable cycle counter
  PRU0_CTRL.CYCLE = cycle;

  pru0_mem[0] = SPItransfer(0);

  cycle = PRU0_CTRL.CYCLE;    // Read cycle and store in a register

  __halt();
}


void initSPImod(void){
  /* Reset McSPI0 module */
  CT_MCSPI0.SYSCONFIG_bit.SOFTRESET = 0x0001;

  /* Wait until reset is done */
  while(!(CT_MCSPI0.SYSSTATUS_bit.RESETDONE == 0x1));

  /* Set SPI module to Master Mode */
  CT_MCSPI0.MODULCTRL_bit.MS = 0x0;
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

uint16_t SPItransfer(uint8_t chan){
  const uint8_t ADCch[] = {0, 4, 1, 5, 2, 6, 3, 7};

  uint16_t SPIsend = (ADCch[chan] << 12) | 0b1000000000000000; // single-ended, input +/-5V
  uint16_t SPIsend = (ADCch[chan] << 12) | 0b1000100000000000; // single-ended, input 0V to 5V
  uint16_t SPIsend = (ADCch[chan] << 12) | 0b1000010000000000; // single-ended, input +/-10V
  uint16_t SPIsend = (ADCch[chan] << 12) | 0b1000110000000000; // single-ended, input 0V to 10V



  /* Check if ADC is busy with conversion and continue if not*/
  while(!(__R31 & (1 << _BUSY)));

  /* pull down CONVST and _RD */
  __R30 &= ~(1 << CONVST);
  __R30 &= ~(1 << _RD);

  /* Enable channel */
  CT_MCSPI0.CH0CTRL_bit.EN = 0x1;

  /* Check if McSPI TX register is empty, if it is continue */
  while(!(CT_MCSPI0.IRQSTATUS_bit.TX0_EMPTY == 0x1));

  /* Write word to be transmitted into TX register */
  CT_MCSPI0.TX0 = SPIsend;

  /* Check if McSPI RX register is full, if it is continue */
  while(!(CT_MCSPI0.IRQSTATUS_bit.RX0_FULL == 0x1));

  /* Clear interrupts */
  CT_MCSPI0.IRQSTATUS = 0xFFFF;

  /* Disable channel */
  CT_MCSPI0.CH0CTRL_bit.EN = 0x0;

  /* Start conversion, Stop reading */
  __R30 |= (1 << CONVST);
  __R30 |= (1 << _RD);

  /* Delay until conversion starts */
  __delay_cycles(100);

  /* Wait until Conversion is done */
  while(!(__R31 & (1 << _BUSY)));

  /* pull down CONVST and _RD */
  __R30 &= ~(1 << CONVST);
  __R30 &= ~(1 << _RD);

  /* Enable channel */
  CT_MCSPI0.CH0CTRL_bit.EN = 0x1;

  /* Write nothing to TX Register */
  CT_MCSPI0.TX0 = 0x0000;

  /* Wait until RX register is full */
  while(!(CT_MCSPI0.IRQSTATUS_bit.RX0_FULL == 0x1));

  /* Clear interrupts */
  CT_MCSPI0.IRQSTATUS = 0xFFFF;

  /* Disable channel */
  CT_MCSPI0.CH0CTRL_bit.EN = 0x0;

  /* Start conversion */
  __R30 |= (1 << CONVST);
  __R30 |= (1 << _RD);

  /* Return value in RX register */
  return CT_MCSPI0.RX0;
}
