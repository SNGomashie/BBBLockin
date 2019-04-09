// Access the CYCLE and STALL registers
// devmem2 0x4A32200C - PRU0 CTRl register 0x4A322000 - offset cycle register 0x000C
#include <stdint.h>
#include <pru_cfg.h>
#include <pru_ctrl.h>
#include "resource_table.h"


volatile register unsigned int __R30;
volatile register unsigned int __R31;

#define PRU0_MEM 0x00000000
volatile uint32_t *pru0_mem =  (unsigned int *) PRU0_MEM;

typedef struct {
	uint32_t op1;
	uint32_t op2;
} operands;

operands buf;

void main(void)
{
  uint64_t result = 0;
  uint32_t cycle;
  uint32_t samp_period = 0x01312D00;
  uint32_t pow2_32 = 0xFFFFFFFF;
  uint32_t period = 0x0BEBC200;

  buf.op1 = samp_period;
  buf.op2 = pow2_32;

    // Clear SYSCFG[STANDBY_INIT] to enable OCP master port
    CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

    PRU0_CTRL.CTRL_bit.CTR_EN = 1;  // Enable cycle counter
    // Reset cycle counter, cycle is on the right side to force the compiler
    // to put it in it's own register
    PRU0_CTRL.CYCLE = cycle;

    result = (uint64_t)buf.op1 * (uint64_t)buf.op2;
    result /= period;
    cycle = PRU0_CTRL.CYCLE;
    pru0_mem[1] = cycle;    // Read cycle and store in a register
    pru0_mem[0] = result;
    __halt();
}
