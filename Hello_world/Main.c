#include <stdint.h>
#include <stdio.h>
#include <pru_cfg.h>
#include "pru_peripheral.h"
#include "pru_rpmsg_lib.h"

/* data RAM definition for debugging */
#define PRU0_MEM 0x00000000
volatile uint32_t *pru0_mem =  (unsigned int *) PRU0_MEM;

void main (void) {
  char* data;
  uint8_t i = 0;
  int j = 0;
  __R30 = 0x00000000;
  /*Allow OCP master port access by the PRU so the PRU can read external memories. */
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

  RPMSGinitialize();

  data = RPMSGreceive();

  sscanf(data, "%d", pru0_mem[0]);

  // pru0_mem[0] = (uint32_t)data[0];

  pru0_mem[1] = (1000000000 / data[0]) / 5;

  for(i = 0; i < 10; i++){
  /* Send chars to the ARM, buf = payload, 11 is length of payload. */
    RPMSGtransmit(data);
    __delay_cycles(20000);    // Wait 1/2 second
  }
}
