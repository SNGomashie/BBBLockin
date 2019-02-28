#include <stdint.h>
#include <pru_cfg.h>
#include "prucommlib.h"

void main (void) {
  //Allow OCP master port access by the PRU so the PRU can read external memories
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;
  //Initialize the RPMsg framework
  while(rpmsg_init() != 1);
  while(1) {
  //Send chars to the ARM,  send(char array, length of array)
    send("hallo world\n", 12);
    __delay_cycles(200000000);    // Wait 1/2 second
  }
}
