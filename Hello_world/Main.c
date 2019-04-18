#include <stdint.h>
#include <stdio.h>
#include <pru_cfg.h>
#include "pru_peripheral.h"
#include "pru_rpmsg_lib.h"

void main (void) {
  char* data;
  uint8_t i = 0;
  __R30 = 0x00000000;
  /*Allow OCP master port access by the PRU so the PRU can read external memories. */
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

  RPMSGinitialize();

  data = RPMSGreceive();

  for(i = 0; i < 10; i++){
  /* Send chars to the ARM, buf = payload, 11 is length of payload. */
    RPMSGsend(data);
    __delay_cycles(20000);    // Wait 1/2 second
  }
}
