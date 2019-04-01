// Access the MAC
#include <stdint.h>
#include <pru_cfg.h>
#include <pru_ctrl.h>
#include <pru_ecap.h>
#include "resource_table.h"


volatile register unsigned int __R30;
volatile register unsigned int __R31;

#define PRU0_MEM 0x00000000
volatile uint32_t *pru0_mem =  (unsigned int *) PRU0_MEM;

void main(void)
{
	/* Configure eCAP module */
	CT_ECAP.ECCTL1 = 0b0100000100000010;

	CT_ECAP.ECCTL2 = 0b0000000011000000;

	__delay_cycles(10);

	while(1){
		pru0_mem[0] = CT_ECAP.CAP1;
	}
}
