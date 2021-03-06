#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "pru_peripheral.h"
#include "am335x_pru_nco.h"
#include "resource_table.h"



/**************************/
/*       Definitions      */
/**************************/
#define DEBUG_PIN 10  // Pin used for debugging | P8_28
#define PRU0_PRU1_START_INT (13+16)  // Interrupt for starting PRU1 | sys_evt 29, channel 0, host_int 0
#define PRU1_PRU0_SEND_INT (14+16)  // Interrupt for starting PRU1 | sys_evt 29, channel 1, host_int 1
#define PRU0_PRU1_STOP_INT (15+16)  // Interrupt for stopping PRU1 | sys_evt 27, channel 0, host_int 0
#define PRU_ICSS_IEP_INT (7)  // Interrupt from IEP timer | sys_evt 7, channel 1, host_int 1
/**************************/
/**************************/



/*****************************/
/* PRU-ICSS Memory locations */
/*****************************/
#define SHARE_MEM  0x00010000  // 12kB of shared memory
volatile uint32_t *sMEM =  (unsigned int *) SHARE_MEM;

#define PRU0_MEM  0x00000000  // 8kB of Data RAM
volatile uint32_t *dMEM0 =  (unsigned int *) PRU0_MEM;

#define PRU1_MEM  0x00002000  // 8kB of Data RAM of the secondary PRU
volatile uint32_t *dMEM1 =  (unsigned int *) PRU1_MEM;
/*****************************/
/*****************************/



void main(void) {

  __R30 = 0x00000000;  // Clear al output pins

  /*************************/
  /* Variable declarations */
  /*************************/
  struct NCO oscReference;
  /*************************/
  /*************************/



  /*************************/
  /*    Initializations    */
  /*************************/
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;  // Allow OCP master port access by the PRU so the PRU can read external memories.

  INTCinitialize(PRU1_PRU0_SEND_INT, 1, 1);  // Initialize interrupt controller | sys_evt 30, channel 1, host_int 1
  eCAPinitialize();  // Initialize enchanced capture module

  NCOinitialize(&oscReference, &sMEM[2]);  // Initialize Numerical Oscillator
  /*************************/
  /*************************/



  /*************************/
  /*      Main program     */
  /*************************/



  /*       Main loop       */
  while(1){
    INTERNCOMlisten(0, PRU0_PRU1_START_INT);  // Wait until sample period is loaded into shared memory
    do{
      while(__R31 & (1 << 31)){   // IEP interrupt polling
        while(!(CT_IEP.TMR_CMP_STS == 0));  // Wait until PRU0 has cleared the interrupt
          NCOsetfreq(&oscReference);  // Change the tuning word to stay in-phase
          NCOstep(&oscReference);  // Go to the next value of the sin wave

          sMEM[0] = oscReference.sin_output;  // Save the cos output of the NCO in shared memory reg 0
          sMEM[1] = oscReference.cos_output;  // Save the sin output of the NCo in shared memory reg 1

          INTERNCOMpoke(PRU1_PRU0_SEND_INT);  // Tell PRU0 that data is ready
      }
    }while(!(__R31 & (1 << 30)));
  }
  __halt();
}
