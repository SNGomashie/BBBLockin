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
    uint32_t a = 0xFFFFFFFF;
    uint32_t b = 0xF0F0F0F0;

    uint32_t l = 0x00;
    /* Initialize output*/
    uint32_t x = 0;
    uint32_t y= 0;

    /* Load operands into R28/R29 */
    buf.op1 = a;
    buf.op2 = b;


    result = (uint64_t)buf.op1 * (uint64_t)buf.op2;
    pru0_mem[0] = result >> 32;
    pru0_mem[1] = result;
    /* stop PRU */
    __halt();
}
