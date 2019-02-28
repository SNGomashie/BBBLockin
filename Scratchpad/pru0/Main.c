#include <stdint.h>
#include <stdlib.h>
#include <pru_intc.h>
#include <pru_cfg.h>
#include "resource_table.h"

#define PRU0
#define RAND_MAX 0xFFFF

typedef struct {
	uint16_t reg0;
	uint16_t reg1;
	uint16_t reg2;
	uint16_t reg3;
} bufferData;

bufferData dmemBuf;

volatile register unsigned int __R30;
volatile register unsigned int __R31;

#define SHARE_MEM  0x00010000
volatile uint32_t *shared =  (unsigned int *) SHARE_MEM;

void main(void) {
	shared[0] = 0x00000000;
	srand(time(0));

while(1) {
	while(shared[0] == 0x00000000){
		/* Fill the struct with 16 bit random values */
	  dmemBuf.reg0 = dmemBuf.reg0 + 0x0001;
	  dmemBuf.reg1 = dmemBuf.reg1 + 0x0002;
	  dmemBuf.reg2 = dmemBuf.reg2 + 0x0003;
	  dmemBuf.reg3 = dmemBuf.reg3 + 0x0004;

		/* Send interrupt over shared memory */
		shared[0] = 0xFFFFFFFF;

	  __xout(10, 0, 0, dmemBuf);
		__delay_cycles(200000000);
	}
}


__halt();
}
