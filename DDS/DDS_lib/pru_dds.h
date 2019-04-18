#ifndef __DDS_LIB_H_
#define __DDS_LIB_H_

#define P2_16 0xFFFF
#define P2_24 0x01000000

struct DDS32{
  uint32_t accumulator;
  uint32_t incrementor;
  uint32_t period;
  uint32_t samp_period;
  uint16_t value;
};

void DDSinitialize(struct DDS32 *n, uint32_t samp_period);
void DDSsetfreq(struct DDS32 *n);
void DDSstep(struct DDS32 *n);
void DDSinterpolate(struct DDS32 *n);

#endif
