#ifndef __AM335X_PRU_DDS_H_
#define __AM335X_PRU_DDS_H_

#include <pru_ecap.h>

#define P2_16 0xFFFF
#define P2_24 0x01000000

struct DDS {
  uint32_t sample_period;
  volatile uint32_t *period;
  uint64_t incrementor;
  uint32_t accumulator;
  uint32_t output;
};


void DDSinitialize(struct DDS *n, uint32_t sample_periods);
void DDSsetfreq(struct DDS *n);
void DDSinterpolate(struct DDS *n);
void DDSstep(struct DDS *n);

#endif
