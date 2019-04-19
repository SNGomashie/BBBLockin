/*           Direct Digital Synthesis (DDS)            */
/* Generate every frequency ( < 2/fs ) from a sine LUT */
/* Capture the period of a function generator SYNC output*/
/*  And generate a sinewave with a corresponding period  */

#include <stdint.h>
#include <stdio.h>
#include <pru_cfg.h>
#include <pru_ctrl.h>
#include "pru_peripheral.h"
#include "pru_rpmsg_lib.h"

#define HOST_INT (1 << 31)
#define PIN 7

/* data RAM definition for debugging */
#define PRU0_MEM 0x00000000
volatile uint32_t *pru0_mem =  (unsigned int *) PRU0_MEM;

#define P2_16 0xFFFF
#define P2_24 0x01000000

uint16_t blkdata[248];

void main(void){
  /* Initialize variables */
  uint32_t samp_period =0;
  uint32_t samp_freq =0;
  uint16_t x = 0;
  char* RPMsg_in;
  char data[] = "";

  /* NCO */
  uint64_t incrementor = 0;
  uint32_t accumulator = 0;
  uint32_t output = 0;

  __R30 = 0x00000000;
  /*Allow OCP master port access by the PRU so the PRU can read external memories. */
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

  /*  Initialization  */
  RPMSGinitialize();
  INTCinitialize(7, 1, 1);
  eCAPinitialize();

  RPMsg_in = RPMSGreceive();
  samp_freq = atoi(RPMsg_in);
  samp_period = (1000000000 / samp_freq) / 5;

  IEPinitialize(samp_period, 1, cmp);
  UARTinitialize();
  IEPstart();

  /* Main loop */
  while(1){
    /* Capture period and calculate phase incrementor */
    period = CT_ECAP.CAP1;

    /* Calculate optimal phase increment for the corresponding period */
    incrementor = (uint64_t)samp_period * (uint64_t)P2_24;
    incrementor /= period;

    /* Timer interrupt polling */
    while(__R31 & HOST_INT){
      IEPclear_int();
      INTCclear(7);
      /* Toggle pin (debugging)*/
      __R30 ^= 1 << PIN;

      /* interpolate to get accurate output */
      output = interpolate(accumulator);

      /* Format string to be send */
      sprintf(data,"%d, %d\n", output, accumulator);
      // sprintf(data, "%x %x\n", accumulator, period);

      /* Print to serial port */
      UARTtransmit(data);

      /* add incrementor to phase */
      accumulator += incrementor;

      /* Limit the phase accumulator to 24 bits */
      /*       Q00000000.0000000000000000       */
      /*        --------.----------------       */
      /*       int part . fractional part       */
      /*        0 - 256 .    0 - 65336          */
      accumulator &= (P2_24) - 1;

    }
    // if(!(x < 248)){
    //   RPMSGtransmit_block(blkdata);
    //   x = 0;
    //   break;
    // }
  }
  __halt();
}

/*          Interpolation function            */
/*         Interpolate between two            */
/*         values from the sine LUT           */
/*          interpolation formula:            */
/*  out1 + (out2 - out1) / 2^16 * accumulator */
uint32_t interpolate(uint32_t pos){
  /* Variable initialization */
  uint32_t index = 0;
  uint32_t out1, out2 = 0;
  uint32_t fraction = 0;
  uint64_t diff_x_frac = 0;
  uint32_t temp_out = 0;
  int32_t diff = 0;

  /* Extract int part of accumulator */
  index = pos >> 16;

  /* Find LUT output and next output */
  out1 = sinLUT256[index];
  out2 = sinLUT256[index+1];

  /* Ectract frac part of accumulator */
  fraction = 0xFFFF & pos;

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
  return (out1 + temp_out);
}
