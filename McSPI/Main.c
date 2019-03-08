#include <stdint.h>
#include <pru_cfg.h>
#include <pru_ctrl.h>
#include <pru_intc.h>
#include <sys_mcspi.h>
#include "resource_table.h"

/* PRCM Registers */
#define CM_PER_BASE	((volatile uint8_t *)(0x44E00000))
#define SPI0_CLKCTRL  (0x4C)
#define ON (0x2)

#define HOST_INT			((uint32_t) 1 << 30)

//Define pin locations
#define NRD       7
#define CS        5
#define MISO      3
#define MOSI      1
#define CLK       2
#define CONVST    0
#define BUSY      16

volatile register uint32_t __R30;
volatile register uint32_t __R31;
void main(void)
{
  __R30 &= ~(1 << CONVST);

  /* Clear SYSCFG[STANDBY_INIT] to enable OCP master port */
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

  // Enable global interrupts
   CT_INTC.GER = 0x1;

  // Clear current events
  __R31 = 0x00000000;

  /* Reset McSPI0 module */
  CT_MCSPI0.SYSCONFIG_bit.SOFTRESET = 1;

  /* Wait until reset is done */
  while(!(CT_MCSPI0.SYSSTATUS_bit.RESETDONE == 1));

  CT_MCSPI0.SYST_bit.SSB = 1; // Clear interrupts

  //Reset interrupt status
  CT_MCSPI0.IRQSTATUS = 0xFFFF;

  //Configure interrupts
  CT_MCSPI0.IRQENABLE_bit.EOWKE = 0X1;

  // Set clock devider, SPI clock = 48MHz, Device clock = 20Mhz. devider = 4;
  CT_MCSPI0.CH0CONF_bit.CLKD = 0x2;

  // Set CS polarity
  CT_MCSPI0.CH0CONF_bit.EPOL = 0x1;

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

  // Enable channel
  CT_MCSPI0.CH0CTRL_bit.EN = 0x1;

  //Write word to transmit
  CT_MCSPI0.TX0 = 0x8800;

}
