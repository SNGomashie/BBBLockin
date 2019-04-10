/*           Direct Digital Synthesis (DDS)            */
/* Generate every frequency ( < 2/fs ) from a sine LUT */
/* Capture the period of a function generator SYNC output*/
/*  And generate a sinewave with a corresponding period  */

#include <stdint.h>
#include <stdio.h>
#include <pru_cfg.h>
#include <pru_ctrl.h>
#include <pru_intc.h>
#include <pru_iep.h>
#include <pru_uart.h>
#include <pru_ecap.h>
#include "resource_table.h"

#define HOST_INT (1 << 31)

/* The FIFO size on the PRU UART is 16 bytes; however, we are (arbitrarily)
 * only going to send 8 at a time */
#define FIFO_SIZE	16
#define MAX_CHARS	16
#define BUFFER		40

volatile register unsigned int __R30;
volatile register unsigned int __R31;

void initIEP(uint32_t comp);
void initECAP(void);
void initUART(void);
void serialPRINT(volatile char* Message);
void initINTC(void);

void main(void){
  /* Initialize variables */
  uint32_t period = 0;
  uint32_t samp_period = 0x01312D00;
  uint32_t incrementor = 0;
  uint32_t accumulator = 0;
  uint32_t pow2_32 = 0xFFFFFFFF;
  char data[] = "";

  /*  Initialization  */
  initIEP(0x30D3B);
  initECAP();
  initUART();
  initINTC();

  /* Main loop */
  while(1){
    /* Capture period and calculate phase incrementor */
    period = CT_ECAP.CAP1;

    /* Calculate optimal phase increment for the corresponding period */
    incrementor = (uint64_t)samp_period * (uint64_t)pow2_32;
    incrementor /= period;

    /* Timer interrupt polling */
    while(__R31 & HOST_INT){
      /* Clear the status of the interrupt */
      CT_INTC.SICR = 7;

      /* Clear compare status */
      CT_IEP.TMR_CMP_STS_bit.CMP_HIT = 0xFF;

      /* Format string to be send */
      // sprintf(data,"%x, %d\n", sinLUT[accumulator >> 23], accumulator);
      sprintf(data, "%x\n", period);

      /* Print to serial port */
      serialPRINT(data);

      /* add incrementor to phase */
      accumulator += incrementor;
    }
  }
  __halt();
}

/*     Initialize IEP module      */
/*    defines sample frequency    */
/* comp is sample period in cycles*/
void initIEP (uint32_t comp){
  /* sample period = timer period*/

  /* Disable counter */
  CT_IEP.TMR_GLB_CFG_bit.CNT_EN = 0x0000;

  /* Clear CNT register */
  CT_IEP.TMR_CNT = 0x0;

  /* Clear overflow register */
  CT_IEP.TMR_GLB_STS_bit.CNT_OVF = 0x1;

  /* Set compare values */
  CT_IEP.TMR_CMP0 = comp;

  /* Clear compare status */
  CT_IEP.TMR_CMP_STS_bit.CMP_HIT = 0xFF;

  /* Disable compensation */
  CT_IEP.TMR_COMPEN_bit.COMPEN_CNT = 0x0;

  /* Enable CMP0 and reset on event */
  CT_IEP.TMR_CMP_CFG_bit.CMP0_RST_CNT_EN = 0x1;
  CT_IEP.TMR_CMP_CFG_bit.CMP_EN = 0x1;

  /* Set increment to 1 (default = 5)*/
  CT_IEP.TMR_GLB_CFG_bit.DEFAULT_INC = 0x0001;

  /* Enable counter */
  CT_IEP.TMR_GLB_CFG = 0x11;
}

/*    Initialize eCAP module   */
/* Tracks the reference period */
void initECAP(void){
	/* Soft reset */
	CT_ECAP.ECCTL1 |= (0x01 << 14);

	/* Capture polarity & Capture reset */
	CT_ECAP.ECCTL1 &= ~(1 << 0);
	CT_ECAP.ECCTL1 |= (1 << 1);

	/* Enable loading of CAP registers */
	CT_ECAP.ECCTL1 |= (1 << 8);

	/* Select prescaler */
	CT_ECAP.ECCTL1 &= ~(0x1111 << 9);

	/* Continuous or oneshot mdoe */
	CT_ECAP.ECCTL2 &= ~(1 << 0);

	/* Wrap after CAP2 */
	CT_ECAP.ECCTL2 &= ~(1 << 1);

	/* Time Stamp (TSCTR) Counter Stop (freeze) Control */
	CT_ECAP.ECCTL2 |= (1 << 4);

	/* Disable SYNC-in option */
	CT_ECAP.ECCTL2 &= ~(1 << 5);

	/* Disable SYNC-out signal */
	CT_ECAP.ECCTL2 |= (0x11 << 6);

	/* Enable capture mode */
	CT_ECAP.ECCTL2 &= ~(1 << 9);
}

/*   Initialize UART module  */
/* For ARDUINO-ish debugging */
void initUART(void){

    /* Set up UART to function at 115200 baud - DLL divisor is 104 at 16x oversample
     * 192MHz / 104 / 16 = ~115200 */
    CT_UART.DLL = 104;
    CT_UART.DLH = 0;
    CT_UART.MDR_bit.OSM_SEL = 0x0;

    /* If FIFOs are to be used, select desired trigger level and enable
     * FIFOs by writing to FCR. FIFOEN bit in FCR must be set first before
     * other bits are configured */
    /* Enable FIFOs for now at 1-byte, and flush them */
    CT_UART.FCR = (0x80) | (0x8) | (0x4) | (0x2) | (0x01); // 8-byte RX FIFO trigger

    /* Choose desired protocol settings by writing to LCR */
    /* 8-bit word, 1 stop bit, no parity, no break control and no divisor latch */
    CT_UART.LCR = 3;
    CT_UART.LCR_bit.DLAB = 0;
    /* If flow control is desired write appropriate values to MCR. */
  	/* No flow control for now, but enable loopback for test */
  	CT_UART.MCR = 0x00;

    /* Choose desired response to emulation suspend events by configuring
     * FREE bit and enable UART by setting UTRST and URRST in PWREMU_MGMT */
    /* Allow UART to run free, enable UART TX/RX */
    CT_UART.PWREMU_MGMT_bit.FREE = 0x1;
    CT_UART.PWREMU_MGMT_bit.URRST = 0x1;
    CT_UART.PWREMU_MGMT_bit.UTRST = 0x1;

    /* Turn off RTS and CTS functionality */
    CT_UART.MCR_bit.AFE = 0x0;
    CT_UART.MCR_bit.RTS = 0x0;

  }

/* Send message over UART */
void serialPRINT(volatile char* Message){
  uint8_t cnt, index = 0;

  while (1) {
    cnt = 0;

    /* Wait until the TX FIFO and the TX SR are completely empty */
    while (!CT_UART.LSR_bit.TEMT);

    /* Loop aslong as there are new characters in Message and cnt is not bigger than MAX_CHARS*/
    while (Message[index] != NULL && cnt < MAX_CHARS) {
      /* Fill TX FIFO with 1 char */
      CT_UART.THR = Message[index];
      index++;
      cnt++;
    }
    /* if there are no new characters in Message break out of the while(1) loop */
    if (Message[index] == NULL)
      break;
  }

  /* Wait until the TX FIFO and the TX SR are completely empty */
  while (!CT_UART.LSR_bit.TEMT);

}

/*               Initialize interrupts               */
/* Interrupt from sys_event 7 to channel 0 to host 0 */
void initINTC(void){
  /* Clear all host interrupts */
  __R31 = 0x00000000;

  /* Connect sys_evt 7 to channel 1 */
  CT_INTC.CMR1_bit.CH_MAP_7 = 1;

  /* Connect channel 1 to host interrupt 1 */
  CT_INTC.HMR0_bit.HINT_MAP_1 = 1;

  /* Clear sys_evt 7 */
  CT_INTC.SICR = 7;

  /* Enable sys_evt 7 */
  CT_INTC.EISR = 7;

  /* Enable host interrupt */
  CT_INTC.HIEISR = 1;

  /* Clear all sys_evt */
  CT_INTC.SECR0 = 0xFFFFFFFF;
  CT_INTC.SECR1 = 0xFFFFFFFF;
}
