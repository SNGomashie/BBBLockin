// Access the CYCLE and STALL registers
// pr1_pru0_pru_r31_16
#include <stdint.h>
#include <pru_cfg.h>
#include <pru_iep.h>
#include <pru_ctrl.h>
#include "resource_table.h"

volatile register uint32_t __R30;
volatile register uint32_t __R31;

uint8_t idx = 0;
uint8_t jdx = 0;
uint32_t tempVal = 0;

/* Moving average parameters */
uint16_t Navr = 1000; //Amount of samples to be averaged

/* exponentially weighted moving average parameters */
w = 0.001  // Weight of EWMA

/* ADC parameters */
uint8_t Nchl = 4;
uint16_t in = 0;

static uint32_t Q[Nchl], I[Nchl], R[Nchl];

/* Calculate the square root using the iterative babylonian method */
uint16_t sqrt(uint32_t x);

void main(void)
{
    /* These will be kept in registers and never witten to DRAM */
    uint32_t cycle;

    /* Clear SYSCFG[STANDBY_INIT] to enable OCP master port */
    CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

    /* Enable cycle counter */
    PRU0_CTRL.CTRL_bit.CTR_EN = 1;

    /* Clear cycle counter */
    PRU0_CTRL.CYCLE = cycle;

    for (idx = 0; idx < Nchl; idx++) {
      jdx = (idx + 1) % Nchl;
      in = 0xFFFF;
      // Moving average filter
      // Q[idx] -= Q[idx] / Navr;
      // Q[idx] += (-1 * in) / Navr;
      //
      // I[idx] -= I[idx] / Navr;
      // I[idx] += (1 * in) / Navr;
      //
      // tempVal = (I[idx] * I[idx]) + (Q[idx] * Q[idx])
      //
      // R[idx] -= R[idx] / Navr;
      // R[idx] += sqrt()

      // exponentially weighted moving average
      I[idx] = W * (in * COS) + (1 - W) * I[idx]
      Q[idx] = W * (in * SIN) + (1 - W) * Q[idx]

      tempVal = (I[idx] * I[idx]) + (Q[idx] * Q[idx])

      R[idx] = W * (2 * sqrt(tempVal)) + (1 - W) * R[idx]

    }
    cycle = PRU0_CTRL.CYCLE;
    __halt();
}

uint16_t sqrt(uint32_t x) {
	uint32_t fa,sa,ta;
	uint32_t error,error_last;

  // Since the squareroot of X will always be smaller than X / 2
	// We can choose an (arbitrary) first approach as X / 2
	// The closer this number is to the final answer the faster this routines completes.

	fa = x>>1;		// Divide number by 2, bitshifting performs the same operation

	// Divide the argument number by the first approach
	sa = x/fa;

	// Get the mean between the two previous numbers (add them and divide by 2).
	ta = (fa+sa)>>1;

	error_last=-1;
	error=0;

	/*
	 * Repeat this routine until the integer output value is no longer changing.
	 */
	do {
		error_last=error;
		fa = ta;
		sa = x/fa;
		ta = (fa+sa)>>1;
		error=iabs(x - ta*ta);
	} while (error_last!=error);

	// Return the integer result: square root of the input argument x.
	return ta;
}
