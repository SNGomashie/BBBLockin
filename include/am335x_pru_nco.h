#ifndef __AM335X_PRU_NCO_H_
#define __AM335X_PRU_NCO_H_

#include <pru_ecap.h>

#define P2_16 0xFFFF
#define P2_24 0x01000000

struct NCO {
  uint32_t sample_period;
  volatile uint32_t *period;
  uint64_t incrementor;
  uint32_t sin_accumulator;
  uint32_t cos_accumulator;
  uint32_t sin_output;
  uint32_t cos_output;
};


void NCOinitialize(struct NCO *n, uint32_t samp_period);
void NCOsetfreq(struct NCO *n);
void NCOinterpolate(struct NCO *n);
void NCOstep(struct NCO *n);

#endif
