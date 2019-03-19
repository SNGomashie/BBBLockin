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

void main(void)
{
    // These will be kept in registers and never witten to DRAM
    uint32_t cycle;

    // Clear SYSCFG[STANDBY_INIT] to enable OCP master port
    CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

    PRU0_CTRL.CTRL_bit.CTR_EN = 1;  // Enable cycle counter


    while (!(__R31 & (1 << SYNC)));
      PRU0_CTRL.CYCLE = cycle;
      while (__R31 & (1 << SYNC));
      while(!(__R31 & (1 << SYNC)));
      cycle = PRU0_CTRL.CYCLE;    // Read cycle and store in a register
    __halt();
}
