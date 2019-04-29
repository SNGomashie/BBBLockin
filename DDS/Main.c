/*           Direct Digital Synthesis (DDS)            */
/* Generate every frequency ( < 2/fs ) from a sine LUT */
/* Capture the period of a function generator SYNC output*/
/*  And generate a sinewave with a corresponding period  */

#include <stdint.h>
#include <stdio.h>
#include "pru_peripheral.h"
#include "pru_rpmsg_lib.h"
#include "am335x_pru_dds.h"

#define HOST_INT (1 << 31)
#define PIN 7

/* data RAM definition for debugging */
#define PRU1_MEM 0x00002000
volatile uint32_t *pru1_mem =  (unsigned int *) PRU1_MEM;

uint16_t blkdata[248];

void main(void){
  /* Initialize variables */
  uint32_t period = 0;
  uint32_t samp_period =0;
  uint16_t samp_freq =0;
  uint8_t amount_samp = 0;
  char* RPMsg_in;
  uint16_t x = 0;
  uint8_t i = 0;

  struct DDS osc;

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

  DDSinitialize(&osc, samp_period);
  IEPinitialize(samp_period, 1, cmp);
  UARTinitialize();
  IEPstart();

  RPMsg_in = RPMSGreceive();
  amount_samp = atoi(RPMsg_in);

  /* Main loop */
  while(1){
    /* Timer interrupt polling */
    while(__R31 & HOST_INT){

      IEPclear_int();
      INTCclear(7);
      /* Capture period and calculate phase incrementor */
      DDSsetfreq(&osc);

      DDSstep(&osc);

      blkdata[x] = osc.output;
      pru1_mem[0] = *osc.period;
      x++;
      if(!(x < 248)){
        RPMSGtransmit_block(blkdata);
        x = 0;
        i++;
        if(!(i < amount_samp)){
          IEPstop();
        }
      }

    }
  }
  __halt();
}
