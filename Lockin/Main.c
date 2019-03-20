// Access the CYCLE and STALL registers
// pr1_pru0_pru_r31_16
#include <stdint.h>
#include <math.h>
#include <pru_cfg.h>
#include <pru_iep.h>
#include <pru_ctrl.h>
#include "resource_table.h"

volatile register uint32_t __R30;
volatile register uint32_t __R31;

uint8_t idx = 0;
uint8_t jdx = 0;

const uint8_t Navr = 1000;
const uint8_t Nchl = 4;
uint16_t in = 0;

static uint32_t Q[Nchl], I[Nchl], R[Nchl];

void main(void)
{
    // These will be kept in registers and never witten to DRAM
    uint32_t cycle;

    // Clear SYSCFG[STANDBY_INIT] to enable OCP master port
    CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

    PRU0_CTRL.CTRL_bit.CTR_EN = 1;  // Enable cycle counter
    PRU0_CTRL.CYCLE = 0x0001;
    for (idx = 0; idx < Nchl; idx++) {
      jdx = (idx + 1) % Nchl;
      in = 0xFFFF;
      Q[idx] -= Q[idx] / Navr;
      Q[idx] += (-1 * in) / Navr;

      I[idx] -= I[idx] / Navr;
      I[idx] += (-1 * in) / Navr;

      R[idx] -= R[idx] / Navr;
      R[idx] -= sqrt(pow(I[idx], 2) + pow(Q[idx], 2)) / Navr;

    }
    cycle = PRU0_CTRL.CYCLE;
    __halt();
}
