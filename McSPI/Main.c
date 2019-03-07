#include <stdint.h>
#include <pru_cfg.h>
#include <pru_ctrl.h>
#include <sys_mcspi.h>
#include "resource_table.h"


/* PRCM Registers */
#define CM_PER_BASE	((volatile uint8_t *)(0x44E00000))
#define SPI0_CLKCTRL  (0x4C)
#define ON (0x2)


volatile register uint8_t __R31;


void main(void){
  uint32_t result;
	volatile uint8_t *ptr_cm;

	ptr_cm = CM_PER_BASE;

  /* Clear SYSCFG[STANDBY_INIT] to enable OCP master port */
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

  /* Read IEPCLK[OCP_EN] for IEP clock source */
  result = CT_CFG.IEPCLK_bit.OCP_EN;

  /* Access PRCM (without CT) to initialize McSPI0 clock */
	ptr_cm[SPI0_CLKCTRL] = ON;

  /* Reset McSPI0 module */
  CT_MCSPI0.SYSCONFIG.SOFTRESET = 1;

  /* Wait until reset is done */
  while(!(CT_MCSPI0.SYSSTATUS.RESETDONE = 1));

  /* Set SPI module to Master Mode*/
  CT_MCSPI0.MODULCTRL.MS = 0;

  /* Set world length to 16bit */
  CT_MCSPI0.CH0CONF.WL = F;

  // Set clock devider, SPI clock = 48MHz, Device clock = 20Mhz. devider = 4;
  CT_MCSPI0.CH0CONF.CLKD = 2;

  //Reset interrupt status
  CT_MCSPI0.IRQSTATUS = 0x00000000;

  // Enable channel
  CT_MCSPI0.CH0CTRL.EN = 1;

  //Write word to transmit
  CT_MCSPI0.TX0 = 0b10001000;

  // Enable channel
  CT_MCSPI0.CH0CTRL.EN = 0;

}
