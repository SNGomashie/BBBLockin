#include <stdint.h>
#include <stdio.h>
#include <pru_cfg.h>
#include "resource_table.h"
#include "pru_peripheral.h"
#include "pru_rpmsg_lib.h"

void main (void) {

  uint8_t i = 0;
  __R30 = 0x00000000;
  /*Allow OCP master port access by the PRU so the PRU can read external memories. */
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

  RPMSGinitialize();

  RPMSGreceive();

  for(i = 0; i < 10; i++){
  /* Send chars to the ARM, buf = payload, 11 is length of payload. */
    RPMSGsend("Hallo world");
    __delay_cycles(20000);    // Wait 1/2 second
  }
}
