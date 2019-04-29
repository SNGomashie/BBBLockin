#ifndef __AM335X_PRU_DDS_C_
#define __AM335X_PRU_DDS_C_

#include <stdio.h>
#include <stdint.h>
#include "am335x_pru_dds.h"

void DDSinitialize(struct DDS *n, uint32_t sample_period){
  n->period = &CT_ECAP.CAP1;
  n->accumulator = 0;
  n->sample_period = sample_period;
  n->output = sinLUT[0];
  DDSsetfreq(n);
}

void DDSsetfreq(struct DDS *n){
  uint32_t norm_period = 0;
  uint32_t temp_period = 0;

  temp_period = *(n->period) / 100;
  norm_period = P2_24 / temp_period;
  n->incrementor = (uint64_t)norm_period * (uint64_t)n->sample_period;
}

void DDSinterpolate(struct DDS *n){
  uint32_t index = 0;
  uint32_t out1, out2 = 0;
  uint32_t fraction = 0;
  uint64_t diff_x_frac = 0;
  int32_t diff = 0;
  uint32_t temp_out = 0;

  /* Extract int part of accumulator */
  index = n->accumulator >> 16;

  /* Find LUT output and next output */
  out1 = sinLUT[index];
  out2 = sinLUT[index+1];

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
    n->output = (out1 + temp_out);
}

void DDSstep(struct DDS *n){
  DDSinterpolate(n);
  n->accumulator += n->incrementor;
  n->accumulator &= (P2_24) - 1;
}

#endif
