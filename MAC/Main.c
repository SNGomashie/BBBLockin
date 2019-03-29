// Access the MAC
#include <stdint.h>
#include <pru_cfg.h>
#include <pru_ctrl.h>
#include "resource_table.h"


volatile register unsigned int __R30;
volatile register unsigned int __R31;

typedef struct {
	uint32_t op1;
	uint32_t op2;
} operands;

operands buf;

void main(void)
{
    /* Generate 32-bit numbers */
    uint32_t a = 0xFFFFFFFF;
    uint32_t b = 0xF0F0F0F0;

    /* Initialize output*/
    uint32_t x = 0;
    uint32_t y= 0;

    /* Clear R25 for MAC mode = 0 */
    __xin(0, 25, 1, 0x00000000);

    /* Store the mode on the MAC */
    __xout(0, 25, 1, 0);

    /* Load operands into R28/R29 */
    buf.op1 = a;
    buf.op2 = b;

    /* Delay for 1 clock cycle to perform multiplication */
    __delay_cycles(1);

    /* Load pruduct into the PRU */
    __xin(0, 26, 1, x);
    __xin(0, 27, 1, y);

    /* stop PRU */
    __halt();
}
