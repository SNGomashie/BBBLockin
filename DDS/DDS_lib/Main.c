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
#include "pru_dds.h"

#define HOST_INT (1 << 31)
#define PIN 7

#define SAMP_FREQ 10000

/* data RAM definition for debugging */
#define PRU0_MEM 0x00000000
volatile uint32_t *pru0_mem =  (unsigned int *) PRU0_MEM;
#define PRU1_MEM 0x00020000
volatile uint32_t *pru1_mem =  (unsigned int *) PRU1_MEM;

void main(void){
  /* Initialize variables */
  uint32_t samp_period =0;
  char* RPMsg_in;
  char RPMsg_out[] = "";
  struct DDS32 osc;

  /*  Initialization  */
  RPMSGinitialize();
  INTCinitialize(7, 1, 1);

  RPMsg_in = RPMSGreceive();

  samp_period = (1000000000 / RPMsg_in[0]) / 5;

  eCAPintialize();
  IEPinitialize(samp_period, 1, cmp);
  DDSinitialize(&osc, samp_period);



  /* Main loop */
  while(1){
    /* Timer interrupt polling */
    while(__R31 & HOST_INT){
        INTCclear(7);
        IEPclear_int();
        DDSsetfreq(&osc);
        /* Toggle pin (debugging)*/
        sprintf(RPMsg_out, "%x\n", osc.value);
        RPMSGtransmit(RPMsg_out);
        __R30 ^= 1 << PIN;
        DDSstep(&osc);
    }
  }
  __halt();
}
