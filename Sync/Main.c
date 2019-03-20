// Access the CYCLE and STALL registers
// pr1_pru0_pru_r31_16
#include <stdint.h>
#include <pru_cfg.h>
#include <pru_iep.h>
#include <pru_ctrl.h>
#include "resource_table.h"

#define SYNC 16     // Bit number to output on

volatile register uint32_t __R30;
volatile register uint32_t __R31;

#define PRU0_MEM 0x00000000
volatile uint32_t *pru0_mem =  (unsigned int *) PRU0_MEM;

void initIEP(void);
uint32_t lockPeriod(uint32_t pin);
uint32_t lockPeriod(uint32_t pin);

void main(void)
{
    // These will be kept in registers and never witten to DRAM
    uint32_t cycle, period;

    // Clear SYSCFG[STANDBY_INIT] to enable OCP master port
    CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

    period = lockPeriodIEP(SYNC);

    __halt();
}

void initIEP (void){
  /* Disable counter */
  CT_IEP.TMR_GLB_CFG_bit.CNT_EN = 0x0000;

  /* Clear CNT register */
  CT_IEP.TMR_CNT = 0xFFFFFFFF;

  /* Clear overflow register */
  CT_IEP.TMR_GLB_STS = 0x0001;

  /* Set increment to 1 (default = 5)*/
  CT_IEP.TMR_GLB_CFG_bit.DEFAULT_INC = 0x0001;

}

uint32_t lockPeriod (uint8_t pin){
  PRU0_CTRL.CTRL_bit.CTR_EN = 1;  // Enable cycle counter
  while (!(__R31 & (1 << pin)));
  PRU0_CTRL.CYCLE = cycle;
  while (__R31 & (1 << pin));
  while(!(__R31 & (1 << pin)));
  return PRU0_CTRL.CYCLE;    // Read cycle and store in a register
}

uint32_t lockPeriodIEP (uint8_t pin){
  initIEP();
  while (!(__R31 & (1 << pin)));
  CT_IEP.TMR_GLB_CFG_bit.CNT_EN = 0x1;  // Enable counter
  while (__R31 & (1 << pin));
  while(!(__R31 & (1 << pin)));
  CT_IEP.TMR_GLB_CFG_bit.CNT_EN = 0x0;
  return CT_IEP.TMR_CNT;    // Read cycle and store in a register

}
