#include <stdint.h>
#include <stdlib.h>
#include <pru_intc.h>
#include <pru_cfg.h>
#include "resource_table.h"

#define RAND_MAX 0xFFFF;

typedef struct {
	uint32_t reg0;
	uint32_t reg1;
	uint32_t reg2;
	uint32_t reg3;
} bufferData;

bufferData dmemBuf;

#define PRU1_PRU0_INTERRUPT (20)

volatile register uint32_t __R30;
volatile register uint32_t __R31;

#define SHARE_MEM  0x00010000
volatile uint32_t *shared =  (unsigned int *) SHARE_MEM;

#define RAND_MAX 0xFFFF;

void main(void) {

  bufferData buf;

  /* Clear the status of all interrupts */
  CT_INTC.SECR0 = 0xFFFFFFFF;
  CT_INTC.SECR1 = 0xFFFFFFFF;

  dmemBuf.reg0 = (0xFFFF << 4);
  dmemBuf.reg1 = (0xF0FF << 4);
  dmemBuf.reg2 = 0x0000FF0F;
  dmemBuf.reg3 = 0x0000FFF0;

  while ((__R31 & (1<<30)) == 0) {
  }
	buf = dmemBuf;
  __xout(14, 0, 0, buf);

  CT_INTC.SICR = PRU1_PRU0_INTERRUPT;

__halt();
}
