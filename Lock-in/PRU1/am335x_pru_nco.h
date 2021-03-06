#ifndef __AM335X_PRU_NCO_H_
#define __AM335X_PRU_NCO_H_

#include <pru_ecap.h>

#define LUTSIZE 1024
#define QSIZE (LUTSIZE / 4)
#define HSIZE (LUTSIZE / 2)
#define TQSIZE ((LUTSIZE * 3) / 4)
#define POW (LUTSIZE << 16)
#define FRAC 0xFFFF

struct NCO {
  volatile uint32_t *ptrSperiod;
  volatile uint32_t *ptrPeriod;
  uint64_t incrementor;
  uint32_t sin_accumulator;
  uint32_t cos_accumulator;
  int16_t sin_output;
  int16_t cos_output;
};


void NCOinitialize(struct NCO *n, volatile uint32_t* ptrSperiod);
void NCOsetfreq(struct NCO *n);
void NCOinterpolate(struct NCO *n);
void NCOstep(struct NCO *n);

#endif
