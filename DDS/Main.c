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
#define PIN 7

/* The FIFO size on the PRU UART is 16 bytes; however, we are (arbitrarily)
 * only going to send 8 at a time */
#define FIFO_SIZE	16
#define MAX_CHARS	16
#define BUFFER		40

#define SAMP_FREQ 100

volatile register unsigned int __R30;
volatile register unsigned int __R31;

const uint16_t sinLUT256[257] = {0x8000,0x8324,0x8647,0x896a,0x8c8b,0x8fab,0x92c7,0x95e1,0x98f8,0x9c0b,0x9f19,0xa223,0xa527,0xa826,0xab1f,0xae10,0xb0fb,0xb3de,0xb6b9,0xb98c,0xbc56,0xbf17,0xc1cd,0xc47a,0xc71c,0xc9b3,0xcc3f,0xcebf,0xd133,0xd39a,0xd5f5,0xd842,0xda82,0xdcb3,0xded7,0xe0eb,0xe2f1,0xe4e8,0xe6cf,0xe8a6,0xea6d,0xec23,0xedc9,0xef5e,0xf0e2,0xf254,0xf3b5,0xf504,0xf641,0xf76b,0xf884,0xf989,0xfa7c,0xfb5c,0xfc29,0xfce3,0xfd89,0xfe1d,0xfe9c,0xff09,0xff61,0xffa6,0xffd8,0xfff5,0xffff,0xfff5,0xffd8,0xffa6,0xff61,0xff09,0xfe9c,0xfe1d,0xfd89,0xfce3,0xfc29,0xfb5c,0xfa7c,0xf989,0xf884,0xf76b,0xf641,0xf504,0xf3b5,0xf254,0xf0e2,0xef5e,0xedc9,0xec23,0xea6d,0xe8a6,0xe6cf,0xe4e8,0xe2f1,0xe0eb,0xded7,0xdcb3,0xda82,0xd842,0xd5f5,0xd39a,0xd133,0xcebf,0xcc3f,0xc9b3,0xc71c,0xc47a,0xc1cd,0xbf17,0xbc56,0xb98c,0xb6b9,0xb3de,0xb0fb,0xae10,0xab1f,0xa826,0xa527,0xa223,0x9f19,0x9c0b,0x98f8,0x95e1,0x92c7,0x8fab,0x8c8b,0x896a,0x8647,0x8324,0x8000,0x7cdb,0x79b8,0x7695,0x7374,0x7054,0x6d38,0x6a1e,0x6707,0x63f4,0x60e6,0x5ddc,0x5ad8,0x57d9,0x54e0,0x51ef,0x4f04,0x4c21,0x4946,0x4673,0x43a9,0x40e8,0x3e32,0x3b85,0x38e3,0x364c,0x33c0,0x3140,0x2ecc,0x2c65,0x2a0a,0x27bd,0x257d,0x234c,0x2128,0x1f14,0x1d0e,0x1b17,0x1930,0x1759,0x1592,0x13dc,0x1236,0x10a1,0xf1d,0xdab,0xc4a,0xafb,0x9be,0x894,0x77b,0x676,0x583,0x4a3,0x3d6,0x31c,0x276,0x1e2,0x163,0xf6,0x9e,0x59,0x27,0xa,0x0,0xa,0x27,0x59,0x9e,0xf6,0x163,0x1e2,0x276,0x31c,0x3d6,0x4a3,0x583,0x676,0x77b,0x894,0x9be,0xafb,0xc4a,0xdab,0xf1d,0x10a1,0x1236,0x13dc,0x1592,0x1759,0x1930,0x1b17,0x1d0e,0x1f14,0x2128,0x234c,0x257d,0x27bd,0x2a0a,0x2c65,0x2ecc,0x3140,0x33c0,0x364c,0x38e3,0x3b85,0x3e32,0x40e8,0x43a9,0x4673,0x4946,0x4c21,0x4f04,0x51ef,0x54e0,0x57d9,0x5ad8,0x5ddc,0x60e6,0x63f4,0x6707,0x6a1e,0x6d38,0x7054,0x7374,0x7695,0x79b8,0x7cdb,0x8000};

void initIEP(uint32_t comp);
void initECAP(void);
void initUART(void);
void serialPRINT(volatile char* Message);
void initINTC(void);

#define PRU0_MEM 0x00000000
volatile uint32_t *pru0_mem =  (unsigned int *) PRU0_MEM;

void main(void){
  /* Initialize variables */
  uint32_t period = 0;
  uint32_t samp_period = 0;
  char data[] = "";

  /* NCO */
  uint64_t incrementor = 0;
  uint32_t accumulator = 0;

  /* Interpolation */
  uint32_t index1, index2 = 0;
  uint32_t out1, out2 = 0;
  uint64_t temp_out = 0;
  uint32_t inter_out = 0;
  uint32_t output = 0;
  int32_t fraction = 0;
  int32_t diff = 0;
  uint32_t index = 0;

  /* constants */
  uint32_t pow2_16 = 0xFFFF;
  uint32_t pow2_24 = 0x01000000;


  samp_period = (1000000000 / SAMP_FREQ) / 5;

  /*  Initialization  */
  initINTC();
  initECAP();
  initIEP(samp_period);
  initUART();


  /* Main loop */
  while(1){
    /* Capture period and calculate phase incrementor */
    period = CT_ECAP.CAP1;

    /* Calculate optimal phase increment for the corresponding period */
    incrementor = (uint64_t)samp_period * (uint64_t)pow2_24;
    incrementor /= period;

    /* Timer interrupt polling */
    while(__R31 & HOST_INT){
      /* Clear Compare status */
      CT_IEP.TMR_CMP_STS = (1 << 0);

      /* delay for 5 cycles, clearing takes time */
      __delay_cycles(5);

      /* Clear the status of the interrupt */
      CT_INTC.SICR = 7;

      /* delay for 5 cycles, clearing takes time */
      __delay_cycles(5);

      /* Toggle pin */
      __R30 ^= 1 << PIN;

      /* Interpolation */
      /* Find closest outputs */
      index = accumulator >> 16;
      out1 = sinLUT256[index];
      out2 = sinLUT256[index+1];

      /* Mask fractional part */
      fraction = 0xFFFF & accumulator;
      diff = out2-out1;
      temp_out = (int32_t)diff * (uint32_t)fraction;
      inter_out = temp_out & 0xFFFFFFFF;

      if(inter_out & (1 << 31)){
        inter_out = ~inter_out + 1;
        inter_out /= pow2_16;
        inter_out = ~inter_out + 1;
      } else {
        inter_out /= pow2_16;
      }

      output = out1 + inter_out;

      /* Format string to be send */
      sprintf(data,"%d\n", output);
      // sprintf(data, "%x %x\n", accumulator, period);

      /* Print to serial port */
      serialPRINT(data);

      /* add incrementor to phase */
      accumulator += incrementor;

      /* Limit the phase accumulator to 24 bits */
      /*       Q00000000.0000000000000000       */
      /*        --------.----------------       */
      /*       int part . fractional part       */
      /*        0 - 256 .    0 - 65336          */
      accumulator &= (pow2_24) - 1;
    }

  }
  __halt();
}

/*     Initialize IEP module      */
/*    defines sample frequency    */
/* comp is sample period in cycles*/
void initIEP (uint32_t comp){
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
  CT_IEP.TMR_CMP_CFG = 0x0003;

  /* Set increment to 1 (default = 5)*/
  CT_IEP.TMR_GLB_CFG_bit.DEFAULT_INC = 0x0001;

  /* Enable counter */
  CT_IEP.TMR_GLB_CFG_bit.CNT_EN = 0x0001;
}

/*    Initialize eCAP module   */
/* Tracks the reference period */
void initECAP(void){
	CT_ECAP.ECCTL2 &= ~(1 << 4);
	/* Difference mode */
	CT_ECAP.ECCTL1 |= (1 << 1);

	/* Enable loading of CAP registers */
	CT_ECAP.ECCTL1 |= (1 << 8);

	CT_ECAP.ECCTL2 |= (0x2 << 6);

	CT_ECAP.ECCTL2 |= (1 << 4);
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
  /* Clear any pending PRU-generated events */
  __R31 = 0x00000000;

  /* Connect sys_evt 7 to channel 1 */
  CT_INTC.CMR1_bit.CH_MAP_7 = 1;

  /* Connect channel 1 to host interrupt 1 */
  CT_INTC.HMR0_bit.HINT_MAP_1 = 1;

  /* Clear sys_evt 7 */
  CT_INTC.SICR = 7;

  /* Enable sys_evt 7 */
  CT_INTC.EISR = 7;

  /* Enable Host interrupt 1 */
	CT_INTC.HIEISR |= (1 << 0); /*TODO: Enable proper event */;

	// Globally enable host interrupts
	CT_INTC.GER = 1; /*TODO: Enable global events */;
}
