#ifndef __DDS_LIB_C_
#define __DDS_LIB_C_

#include <stdint.h>
#include <stdio.h>
#include "pru_dds.h"

void DDSinitialize(struct DDS32 *n, uint32_t samp_period){
  n->accumulator = 0;
  n->samp_period = samp_period;
  n->period = CT_ECAP.CAP1;
  n->value = sinLUT256[0];
  DDSsetfreq(n);
}

void DDSsetfreq(struct DDS32 *n){
  uint64_t temp_val;
  /* Capture period and calculate phase incrementor */
  n->period = CT_ECAP.CAP1;

  /* Calculate optimal phase increment for the corresponding period */
  temp_val = (uint64_t)n->samp_period * (uint64_t)P2_24;
  n->incrementor = temp_val / n->period;
}

void DDSstep(struct DDS32 *n){
  uint8_t index;
  DDSinterpolate();
  n->accumulator += n->incrementor;
}

void DDSinterpolate(struct DDS32 *n){
  uint32_t index = 0;
  uint32_t out1, out2 = 0;
  uint32_t fraction = 0;
  uint64_t diff_x_frac = 0;
  uint32_t temp_out = 0;
  int32_t diff = 0;

  /* Extract int part of accumulator */
  index = (n->accumulator >> 16) & 0xff;

  /* Find LUT output and next output */
  out1 = sinLUT256[index];
  out2 = sinLUT256[index+1];

  /* Ectract frac part of accumulator */
  fraction = 0xFFFF & n->accumulator;

  /* Calculate the difference between the 2 samples */
  diff = out2-out1;

  /* Multiply by frac part of accumulator */
  diff_x_frac = (int32_t)diff * (uint32_t)fraction;

  /*      Mask least significant 32-bits      */
  /* because we multiply unsigned with signed */
  temp_out = diff_x_frac & 0xFFFFFFFF;

  /*           division by 2^16             */
  /* if temp_out signed convert to unsigned */
  /* after division convert back to signed  */
  /*    if temp_out unsigned just divide    */
  if(temp_out & (1 << 31)){
    temp_out = ~temp_out + 1;
    temp_out /= P2_16;
    temp_out = ~temp_out + 1;
  } else {
    temp_out /= P2_16;
  }
  n->value = (out1 + temp_out);
}

#endif
