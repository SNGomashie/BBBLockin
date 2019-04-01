// Access the MAC
#include <stdint.h>
#include <pru_cfg.h>
#include <pru_ctrl.h>
#include <pru_ecap.h>
#include "resource_table.h"

volatile register unsigned int __R30;
volatile register unsigned int __R31;

/* ECCTL1 register */
#define CAP1POL 0
#define CTRRST1 1
#define CAP2POL 2
#define CTRRST2 3
#define CAP3POL 4
#define CTRRST3 5
#define CAP4POL 6
#define CTRRST4 7
#define CAPLDEN 8
#define PRESCALE 9
#define FREE_SOFT 14

/* ECCTL2 register */
#define CONT_ONESHT 0
#define	STOP_WRAP 1
#define RE_ARM 3
#define TSCTRSTOP 4
#define SYNCI_EN 5
#define SYNCO_SEL 6
#define SWSYNC 8
#define CAP_APWM 9
#define APWMPOL 10

#define PRU0_MEM 0x00000000
volatile uint32_t *pru0_mem =  (unsigned int *) PRU0_MEM;

void initECAP(void);

void main(void)
{
	initECAP();

	while(1){
		pru0_mem[0] = CT_ECAP.CAP2 - CT_ECAP.CAP1;
	}
}

void initECAP(void){
	/* Soft reset */
	CT_ECAP.ECCTL1 |= (0x01 << FREE_SOFT);

	/* Capture polarity & Capture reset */
	CT_ECAP.ECCTL1 |= (0 << CAP1POL) | (0 << CTRRST1);
	CT_ECAP.ECCTL1 |= (0 << CAP2POL) | (0 << CTRRST2);
	CT_ECAP.ECCTL1 |= (0 << CAP3POL) | (0 << CTRRST3);
	CT_ECAP.ECCTL1 |= (0 << CAP4POL) | (0 << CTRRST4);

	/* Enable loading of CAP registers */
	CT_ECAP.ECCTL1 |= (1 << CAPLDEN);

	/* Select prescaler */
	CT_ECAP.ECCTL1 &= ~(0x1111 << PRESCALE);

	/* Continuous or oneshot mdoe */
	CT_ECAP.ECCTL2 &= ~(1 << CONT_ONESHT);

	/* Wrap after CAP2 */
	CT_ECAP.ECCTL2 |= (1 << STOP_WRAP);

	/* Time Stamp (TSCTR) Counter Stop (freeze) Control */
	CT_ECAP.ECCTL2 |= (1 << TSCTRSTOP);

	/* Disable SYNC-in option */
	CT_ECAP.ECCTL2 &= ~(1 << SYNCI_EN);

	/* Disable SYNC-out signal */
	CT_ECAP.ECCTL2 |= (0x11 << SYNCO_SEL);

	/* Enable capture mode */
	CT_ECAP.ECCTL2 &= ~(1 << CAP_APWM);
}
