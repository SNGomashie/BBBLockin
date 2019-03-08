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

#define HOST_INT			((uint32_t) 1 << 31)

//Define pin locations
#define NRD 7
#define CS 5
#define MISO 3
#define MOSI 1
#define CLK 2
#define CONVST 0

volatile register uint32_t __R30;
volatile register uint32_t __R31;

void initSPI(void);
void initINTC(void);

void main(void){
  uint32_t result;
	volatile uint8_t *ptr_cm;
  __R30 &= ~(1 << CONVST);
	ptr_cm = CM_PER_BASE;

  /* Clear SYSCFG[STANDBY_INIT] to enable OCP master port */
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

  /* Read IEPCLK[OCP_EN] for IEP clock source */
  result = CT_CFG.IEPCLK_bit.OCP_EN;

  /* Access PRCM (without CT) to initialize McSPI0 clock */
	ptr_cm[SPI0_CLKCTRL] = ON;

  initSPI();

  initINTC();

  // Enable channel
  CT_MCSPI0.CH0CTRL_bit.EN = 0x1;

  //Write word to transmit
  CT_MCSPI0.TX0 = 0x8800;

  //Wait until interrupt
  while ((__R31 & (1<<30)) == 0) {
	}

  __R30 |= (1 << CONVST);

  // Disable channel
  CT_MCSPI0.CH0CTRL_bit.EN = 0x0;

  __halt();
}
void initSPI(void){
  /* Reset McSPI0 module */
  CT_MCSPI0.SYSCONFIG_bit.SOFTRESET = 1;

  /* Wait until reset is done */
  while(!(CT_MCSPI0.SYSSTATUS_bit.RESETDONE == 1));

  // Set input and output
  CT_MCSPI0.SYST = 0b010101000001;

  /* Set SPI module to Master Mode*/
  CT_MCSPI0.MODULCTRL_bit.MS = 0x0;

  //Reset interrupt status
  CT_MCSPI0.IRQSTATUS = 0x0000;

  //Configure interrupts
  CT_MCSPI0.IRQENABLE = 0x10005;

  // Set clock devider, SPI clock = 48MHz, Device clock = 20Mhz. devider = 4;
  CT_MCSPI0.CH0CONF_bit.CLKD = 0x2;

  // Set CS polarity
  CT_MCSPI0.CH0CONF_bit.EPOL = 1;

  /* Set world length to 16bit */
  CT_MCSPI0.CH0CONF_bit.WL = 0xF;

  // Set transfer and receive mode
  CT_MCSPI0.CH0CONF_bit.TRM = 0x0;

  // Set SPID0 as not a transmissionline
  CT_MCSPI0.CH0CONF_bit.DPE0 = 1;

  // Set SPID1 as transmissionline
  CT_MCSPI0.CH0CONF_bit.DPE1 = 0;

  // Set SPID0 as input
  CT_MCSPI0.CH0CONF_bit.IS = 0;

  // Set amount of bytes in buffer
  CT_MCSPI0.XFERLEVEL_bit.AEL = 1;
  CT_MCSPI0.XFERLEVEL_bit.AFL = 1;
}

void initINTC(void){
    __R31 = 0x00000000;					      //Clear any events
     // CT_INTC.SIPR1_bit.POLARITY_63_32 = 0x800; // Set polarity of interrupt
     // CT_INTC.SITR1_bit.TYPE_63_32 = 0x000; // Set type of interrupt
     // CT_INTC.CMR11_bit.CH_MAP_44 = 0b111; //map event 44 to channel 0
     // CT_INTC.HMR0_bit.HINT_MAP_0 = 0x0;//map channel 0 to host 0
     // CT_INTC.SECR1_bit.ENA_STS_63_32 = 0x800; // clear system event 44 (McSPI)
     CT_INTC.HIEISR = 0x0;			// Enable Host interrupt 1
     CT_INTC.GER = 0x1;
}
