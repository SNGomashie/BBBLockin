/*           Direct Digital Synthesis (DDS)            */
/* Generate every frequency ( < 2/fs ) from a sine LUT */
/* Capture the period of a function generator SYNC output*/
/*  And generate a sinewave with a corresponding period  */

#include <stdint.h>
#include <stdio.h>
#include <pru_cfg.h>
#include <pru_ctrl.h>
#include "resource_table.h"

volatile register unsigned int __R30;
volatile register unsigned int __R31;

void initIEP(uint32_t comp);
void initECAP(void);
void initUART(void);
void serialPRINT(volatile char* Message);
char serialREAD(void);

void main(void){
  /* Initialize variables */
  uint32_t period = 0;
  uint32_t samp_period = 0;
  uint32_t incrementor = 0;
  uint32_t accumulator = 0;
  volatile char* data;

  /*  Initialization  */
  initECAP();
  initIEP(0x4E20);
  initUART();

  /* Main loop */
  while(1){
    /* Capture period and calculate phase incrementor */
    period = CT_ECAP.CAP1;

    /* Calculate optimal phase increment for the corresponding period */
    incrementor = (uint64_t)(samp_period * 4294967296) / period;

    /* Timer interrupt polling */
    while(){

      /* Format string to be send */
      sprintf(data,"%x, %d\n", sinLUT[accumulator >> 23], accumulator);

      /* Print sine amplitude to serial port */
      serialPRINT(data);

      /* add incrementor to phase */
      accumulator += incrementor;
    }
  }
  __halt();
}

/*  Initialize eCAP module  */
/* captures period from SYNC */
void initECAP(void){
	/* Soft reset */
	CT_ECAP.ECCTL1 |= (0x01 << FREE_SOFT);

	/* Capture polarity & Capture reset */
	CT_ECAP.ECCTL1 &= ~(1 << CAP1POL);
	CT_ECAP.ECCTL1 |= (1 << CTRRST1);

	/* Enable loading of CAP registers */
	CT_ECAP.ECCTL1 |= (1 << CAPLDEN);

	/* Select prescaler */
	CT_ECAP.ECCTL1 &= ~(0x1111 << PRESCALE);

	/* Continuous or oneshot mdoe */
	CT_ECAP.ECCTL2 &= ~(1 << CONT_ONESHT);

	/* Wrap after CAP2 */
	CT_ECAP.ECCTL2 &= ~(1 << STOP_WRAP);

	/* Time Stamp (TSCTR) Counter Stop (freeze) Control */
	CT_ECAP.ECCTL2 |= (1 << TSCTRSTOP);

	/* Disable SYNC-in option */
	CT_ECAP.ECCTL2 &= ~(1 << SYNCI_EN);

	/* Disable SYNC-out signal */
	CT_ECAP.ECCTL2 |= (0x11 << SYNCO_SEL);

	/* Enable capture mode */
	CT_ECAP.ECCTL2 &= ~(1 << CAP_APWM);
}

/*     Initialize IEP module      */
/*    defines sample frequency    */
/* comp is sample period in cycles*/
void initIEP (uint32_t comp){
  /* sample period = timer period*/
  samp_period = comp;

  /* Disable counter */
  CT_IEP.TMR_GLB_CFG_bit.CNT_EN = 0x0000;

  /* Clear CNT register */
  CT_IEP.TMR_CNT = 0xFFFFFFFF;

  /* Clear overflow register */
  CT_IEP.TMR_GLB_STS = 0x0001;

  /* Clear compare status */
  CT_IEP.TMR_CMP_STS = 0x0001;

  /* Set compare values */
  CT_IEP.TMR_CMP0 = comp;

  /* Enable compare event */
  CT_IEP.TMR_CMP_CFG.CMP_EN = 0x0003;

  /* Set increment to 1 (default = 5)*/
  CT_IEP.TMR_GLB_CFG_bit.DEFAULT_INC = 0x0001;

  /* Enable counter */
  CT_IEP.TMR_GLB_CFG_bit.CNT_EN = 0x0001;
}

/*   Initialize UART module  */
/* For ARDUINO-ish debugging */
void initUART(void){
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

/* Receive message over UART */
char serialREAD(void){
  while (!CT_UART.LSR_bit.DR);

  return CT_UART.RBR_bit.DATA;
}
