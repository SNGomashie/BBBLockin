#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "pru_peripheral.h"
#include "am335x_pru_dds.h"

/* Pin number used for debugging */
#define DEBUG_PIN 10

void main(void) {
  /************************/
  /*        SETUP         */
  /************************/
  __R30 = 0x00000000;  // Clear al output pins
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;  // Allow OCP master port access by the PRU so the PRU can read external memories.
  /************************/
  /************************/



  /*************************/
  /* Variable declarations */
  /*************************/
  uint32_t *uint32Sample_period = &sRAM[3];
  /* TODO: Change to using scratchpad */
  uint32_t uint32Sample_amount = 0;

  struct DDS ddsReference;
  /*************************/
  /*************************/



  /*************************/
  /*    Initializations    */
  /*************************/
  eCAPinitialize();  // Initialize enchanced capture module

  INTERNCOMlisten();  // Wait until sample period is loaded into shared memory

  DDSinitialize(&ddsReference, *uint32Sample_period);  // Initialize Numerical Oscillator
  /*************************/
  /*************************/



  /*************************/
  /*      Main program     */
  /*************************/


  /*       Main loop       */
  while(1){
    while(__R31 & (1 << 31)){   // IEP interrupt polling
      while(!(CT_IEP.TMR_CMP_STS == 0));  // Wait until PRU0 has cleared the interrupt
        DDSsetfreq(&ddsReference);  // Change the tuning word to stay in-phase
        DDSstep(&ddsReference);  // Go to the next value of the sin wave

        /* TODO: over scratchpad or shared memory?? */

        INTERNCOMpoke();
        INTERNCOMlisten();
    }
  }
  __halt();
}
