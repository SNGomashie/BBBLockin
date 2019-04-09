// Access the MAC
#include <stdint.h>
#include <pru_cfg.h>
#include <pru_ctrl.h>
#include "resource_table.h"


volatile register unsigned int __R30;
volatile register unsigned int __R31;

#define PRU0_MEM 0x00000000
volatile uint32_t *pru0_mem =  (unsigned int *) PRU0_MEM;

typedef struct {
	uint32_t op1;
	uint32_t op2;
} operands;

operands buf;

void main(void)
{
    uint64_t result = 0;
    volatile uint64_t storeValue = 0;

    /* Generate 32-bit numbers */
    uint32_t a = 0x01312D00;
    uint32_t b = 0xFFFFFFFF;
		uint32_t c = 0x0BEBC200;

    /* Load operands into R28/R29 */
    buf.op1 = a;
    buf.op2 = b;

    /* Multiply 2 32-bit numbers */
    result = (uint64_t)buf.op1 * (uint64_t)buf.op2;

    /* Since the result is 32-bits wide we have to save it into 2 registers */
    pru0_mem[0] = result >> 32;
    pru0_mem[1] = result;

		result /= c;

		pru0_mem[2] = result;

    /* stop PRU */
    __halt();
}
