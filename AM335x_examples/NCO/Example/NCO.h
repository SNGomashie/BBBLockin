#ifndef __AM335X_PRU_NCO_H_
#define __AM335X_PRU_NCO_H_

#include "stdint.h"
#include "stdio.h"
#include <pru_ecap.h>

#define LUTSIZE 1024
#define QSIZE (LUTSIZE / 4)
#define HSIZE (LUTSIZE / 2)
#define TQSIZE ((LUTSIZE * 3) / 4)
#define POW (LUTSIZE << 16)
#define FRAC 0xFFFF

struct NCO {
  uint32_t sample_period;
  volatile uint32_t *period;
  uint64_t incrementor;
  uint32_t sin_accumulator;
  uint32_t cos_accumulator;
  uint32_t sin_output;
  uint32_t cos_output;
};


/**
 * Initliaze the numerically controlled oscillator and calculate the tuning word
 *
 * @param n            NCO struct
 * @param samp_period  Sample period
 * @return             void.
 */
void NCOinitialize(struct NCO *n, uint32_t samp_period);

/**
 * Calculate the tuning word (Tuning word = (2^n * Sample Period) / Desired period)
 * n = number of bits in the accumulator
 *
 * @param n            NCO struct
 * @return             void.
 */
void NCOsetfreq(struct NCO *n);

/**
 * Interpolate between 2 points to get a more accumrate sine wave (output = out1 + (out2 - out1)/2^n * t)
 * out1 = output from only the integer part of the index
 * out2 = out1 + 1 step
 * n = number of bits in the fraction part of the index
 * t = fractional part of the index
 *
 * @param n            NCO struct
 * @return             void.
 */
void NCOinterpolate(struct NCO *n);

/**
 * Add the tuning word to the accumulator to go the the next step
 *
 * @param n            NCO struct
 * @return             void.
 */
void NCOstep(struct NCO *n);

#endif
