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

const uint16_t sinLUT256[257] = {0x8000,0x8324,0x8647,0x896a,0x8c8b,0x8fab,0x92c7,0x95e1,0x98f8,0x9c0b,0x9f19,0xa223,0xa527,0xa826,0xab1f,0xae10,0xb0fb,0xb3de,0xb6b9,0xb98c,0xbc56,0xbf17,0xc1cd,0xc47a,0xc71c,0xc9b3,0xcc3f,0xcebf,0xd133,0xd39a,0xd5f5,0xd842,0xda82,0xdcb3,0xded7,0xe0eb,0xe2f1,0xe4e8,0xe6cf,0xe8a6,0xea6d,0xec23,0xedc9,0xef5e,0xf0e2,0xf254,0xf3b5,0xf504,0xf641,0xf76b,0xf884,0xf989,0xfa7c,0xfb5c,0xfc29,0xfce3,0xfd89,0xfe1d,0xfe9c,0xff09,0xff61,0xffa6,0xffd8,0xfff5,0xffff,0xfff5,0xffd8,0xffa6,0xff61,0xff09,0xfe9c,0xfe1d,0xfd89,0xfce3,0xfc29,0xfb5c,0xfa7c,0xf989,0xf884,0xf76b,0xf641,0xf504,0xf3b5,0xf254,0xf0e2,0xef5e,0xedc9,0xec23,0xea6d,0xe8a6,0xe6cf,0xe4e8,0xe2f1,0xe0eb,0xded7,0xdcb3,0xda82,0xd842,0xd5f5,0xd39a,0xd133,0xcebf,0xcc3f,0xc9b3,0xc71c,0xc47a,0xc1cd,0xbf17,0xbc56,0xb98c,0xb6b9,0xb3de,0xb0fb,0xae10,0xab1f,0xa826,0xa527,0xa223,0x9f19,0x9c0b,0x98f8,0x95e1,0x92c7,0x8fab,0x8c8b,0x896a,0x8647,0x8324,0x8000,0x7cdb,0x79b8,0x7695,0x7374,0x7054,0x6d38,0x6a1e,0x6707,0x63f4,0x60e6,0x5ddc,0x5ad8,0x57d9,0x54e0,0x51ef,0x4f04,0x4c21,0x4946,0x4673,0x43a9,0x40e8,0x3e32,0x3b85,0x38e3,0x364c,0x33c0,0x3140,0x2ecc,0x2c65,0x2a0a,0x27bd,0x257d,0x234c,0x2128,0x1f14,0x1d0e,0x1b17,0x1930,0x1759,0x1592,0x13dc,0x1236,0x10a1,0xf1d,0xdab,0xc4a,0xafb,0x9be,0x894,0x77b,0x676,0x583,0x4a3,0x3d6,0x31c,0x276,0x1e2,0x163,0xf6,0x9e,0x59,0x27,0xa,0x0,0xa,0x27,0x59,0x9e,0xf6,0x163,0x1e2,0x276,0x31c,0x3d6,0x4a3,0x583,0x676,0x77b,0x894,0x9be,0xafb,0xc4a,0xdab,0xf1d,0x10a1,0x1236,0x13dc,0x1592,0x1759,0x1930,0x1b17,0x1d0e,0x1f14,0x2128,0x234c,0x257d,0x27bd,0x2a0a,0x2c65,0x2ecc,0x3140,0x33c0,0x364c,0x38e3,0x3b85,0x3e32,0x40e8,0x43a9,0x4673,0x4946,0x4c21,0x4f04,0x51ef,0x54e0,0x57d9,0x5ad8,0x5ddc,0x60e6,0x63f4,0x6707,0x6a1e,0x6d38,0x7054,0x7374,0x7695,0x79b8,0x7cdb,0x8000};


/* data RAM definition for debugging */
#define PRU0_MEM 0x00000000
volatile uint32_t *pru0_mem =  (unsigned int *) PRU0_MEM;

#define P2_16 0xFFFF
#define P2_24 0x01000000

uint16_t blkdata[248];

void main(void){
  /* Initialize variables */
  uint32_t period = 0;
  uint32_t samp_period =0;
  uint16_t samp_freq =0;
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
