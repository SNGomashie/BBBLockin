#include <stdint.h>
#include <pru_cfg.h>
#include "resource_table.h"

/* Shared memory location & definiton*/
#define SHARE_MEM  0x00010000
volatile uint32_t *shared =  (unsigned int *) SHARE_MEM;

/* data RAM definition for debugging */
#define PRU1_MEM 0x00002000
volatile uint32_t *pru1_mem =  (unsigned int *) PRU1_MEM;

void main (void) {
  uint32_t data = 0;
  uint32_t* dataPtr = &data;
  /* Inifinite loop */
  while(1){
    while(shared[0] == INT_ON){
      /* Read scratchpad */
      __xin(14, 0, 0, dataPtr);
      
      pru1_mem[1] = data;
      /* reset shared memory interrupt*/
      shared[0] = INT_OFF;

      /* Delay half a second */
      __delay_cycles(200000000);
    }
  }

}
