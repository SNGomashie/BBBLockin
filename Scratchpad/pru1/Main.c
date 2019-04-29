#include <stdint.h>
#include <pru_cfg.h>
#include "resource_table.h"

/* Data object to be send through the scratchpad */
typedef struct buffer{
  uint32_t reg0;
} bufferData;

bufferData dmemBuf;

/* Interrupt definitions */
#define INT_OFF 0x00000000
#define INT_ON 0xFFFFFFFF

/* Shared memory location & defintions */
#define SHARE_MEM  0x00010000
volatile uint32_t *shared =  (unsigned int *) SHARE_MEM;

/* data RAM definition for debugging */
#define PRU1_MEM 0x00002000
volatile uint32_t *pru1_mem =  (unsigned int *) PRU1_MEM;

/* Global variable definitions */
volatile register uint32_t __R30;
volatile register uint32_t __R31;

unsigned int test2;
unsigned int* test2Ptr = &test2;

void main (void) {


  /* Inifinite loop */
  while(1){
    while(shared[0] == INT_ON){
      /* Read scratchpad */
      __xin(10, 0, 0, test2Ptr);


      pru1_mem[0] = test2;

      /* reset shared memory interrupt*/
      shared[0] = INT_OFF;

      /* Delay half a second */
      __delay_cycles(200000000);
    }
  }

}
