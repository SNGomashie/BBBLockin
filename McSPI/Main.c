#include <stdint.h>
#include <pru_cfg.h>
#include <pru_ctrl.h>
#include <sys_mcspi>
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
  CT_MCSPI0.CH0CONF_bit.WL = F;


  /* Access CH0CONF register, Set word size to 16-bit*/
  MCSPI0_CH0CONF |= (0xE << 7);

  /* Access CH0CTRL register, Activate channel 1*/
  MCSPI0_CH0CTRL ^= 0x0;


}
