#include <stdint.h>
#include <stdlib.h>
#include <pru_intc.h>
#include <pru_cfg.h>
#include "resource_table.h"

#define PRU0

typedef struct {
	uint32_t reg6;
	uint32_t reg7;
	uint32_t reg8;
	uint32_t reg9;
} bufferData;

bufferData dmemBuf;

#define PRU1_PRU0_INTERRUPT (20)


#define GPIO1 0x4804C000
#define GPIO_CLEARDATAOUT 0x190
#define GPIO_SETDATAOUT 0x194
#define USR0 (1<<21)
#define USR1 (1<<22)
#define USR2 (1<<23)
#define USR3 (1<<24)
unsigned int volatile * const GPIO1_CLEAR = (unsigned int *) (GPIO1 + GPIO_CLEARDATAOUT);
unsigned int volatile * const GPIO1_SET   = (unsigned int *) (GPIO1 + GPIO_SETDATAOUT);

volatile register unsigned int __R30;
volatile register unsigned int __R31;

#define SHARE_MEM  0x00010000
volatile uint32_t *shared =  (unsigned int *) SHARE_MEM;

#define RAND_MAX 0xFFFF;

void main(void) {
	shared[0] = 0xFFFFFFFF;
  bufferData buf;

  /* Clear the status of all interrupts */

  dmemBuf.reg6 = 0xFFFFFFFF;
  dmemBuf.reg7 = 0xFFFFFFFF;
  dmemBuf.reg8 = 0xFFFFFFFF;
  dmemBuf.reg9 = 0xFFFFFFFF;

	*GPIO1_CLEAR = USR3; //turn off light

	__delay_cycles(500000000/5);

	shared[0] = 0xFFFFFFFF;

	buf = dmemBuf;
  __xout(14, 6, 0, buf);

	*GPIO1_SET = USR3; //turn on light

  CT_INTC.SICR = PRU1_PRU0_INTERRUPT;

__halt();
}
