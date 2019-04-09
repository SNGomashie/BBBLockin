//  Attempt to use the McSPI chip to read the ADC
//
//	Pin configuration:
//  CS	( chip select ):  					P9.27 pr1_pru0_pru_r30_5
//  CS	( McSPI chip select ):  		P9.17 cs
//  SDI ( Serial Data In (MOSI)):   P9.18 spi
//  SDO ( Serial Data Out (MISO)):  P9.21 sdo
//  SCK ( Serial clock ) :      		P9.22 sck
//  nRD (  Read Input ) :						P9.25 pr1_pru0_pru_r30_7
//  BUSY_ (conversion done):				P9.24 pr1_pru0_pru_r31_16
//  Convst (  Start conversion ) : 	P9.31 pr1_pru0_pru_r30_0

/* Includes */
#include <stdint.h>
#include <stdio.h>
#include <pru_cfg.h>
#include <pru_ctrl.h>
#include <pru_uart.h>
#include <sys_mcspi.h>
#include "resource_table.h"

/* Define pin locations */
#define CS      5
#define SDI
#define SDO
#define SCK
#define _RD     7
#define _BUSY   16
#define CONVST  0

/* The FIFO size on the PRU UART is 16 bytes; however, we are (arbitrarily)
 * only going to send 8 at a time */
#define FIFO_SIZE	16
#define MAX_CHARS	8
#define BUFFER		40

/* Input and output registers */
volatile register uint32_t __R30;
volatile register uint32_t __R31;

/* PRCM (Power Reset Clock Module) Registers */
/* Base adress for the clock control module */
#define CM_PER_BASE	((volatile uint8_t *)(0x44E00000))
/* Adress for the SPI clk control register */
#define SPI0_CLKCTRL  (0x4C)
#define ON (0x2)

#define PRU0_MEM 0x00000000
volatile uint32_t *pru0_mem =  (unsigned int *) PRU0_MEM;

/* Function declaration */
void initSPImod(void);
void initSPIchan(void);
uint16_t SPItransfer(uint8_t chan);
void initUART(void);
void printUART(volatile char* Message);



void main(void){
  uint32_t cycle;
  char data[] = "";
  volatile uint8_t *ptr_cm;

  /* Clear output register */
  __R30 = 0x00000000;

  /* Clear SYSCFG[STANDBY_INIT] to enable OCP master port */
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

  /* Access PRCM (without CT) to initialize McSPI0 clock */
  ptr_cm = CM_PER_BASE;
  ptr_cm[SPI0_CLKCTRL] = ON;

  /* Initialize UART */
  initUART();

  /* Initialize the McSPI module */
  initSPImod();

  /* Initialize channel 0 of the McSPI module */
  initSPIchan();

  /* Set pins */
  __R30 |= (1 << CS);
  __R30 |= (1 << _RD);
  
  while(1){
    sprintf(data, "%d", SPItransfer(0));
    printUART(data);
    __delay_cycles(200000);
  }

  __halt();
}

/* Initialize SPI module */
void initSPImod(void){
  /* Reset McSPI0 module */
  CT_MCSPI0.SYSCONFIG_bit.SOFTRESET = 0x0001;

  /* Wait until reset is done */
  while(!(CT_MCSPI0.SYSSTATUS_bit.RESETDONE == 0x1));

  /* Set SPI module to Master Mode */
  CT_MCSPI0.MODULCTRL_bit.MS = 0x0;
}

/* Initialize SPI channel */
void initSPIchan(void){
  /* Reset interrupt status */
  CT_MCSPI0.IRQSTATUS = 0xFFFF;

  /* Configure interrupts */
  CT_MCSPI0.IRQENABLE = 0x0;

  // Set clock devider, SPI clock = 48MHz, Device clock = 20Mhz. devider = 4;
  CT_MCSPI0.CH0CONF_bit.CLKD = 0x2;

  // Set CS polarity
  CT_MCSPI0.CH0CONF_bit.EPOL = 0x0;

  /* Set world length to 16bit */
  CT_MCSPI0.CH0CONF_bit.WL = 0xF;

  // Set transfer and receive mode
  CT_MCSPI0.CH0CONF_bit.TRM = 0x0;

  // Set SPID0 as not a transmissionline
  CT_MCSPI0.CH0CONF_bit.DPE0 = 0x1;

  // Set SPID1 as transmissionline
  CT_MCSPI0.CH0CONF_bit.DPE1 = 0x0;

  // Set SPID0 as input
  CT_MCSPI0.CH0CONF_bit.IS = 0x0;
}

/* Do an SPI tranfser to/from the selected channel */
uint16_t SPItransfer(uint8_t chan){
  const uint8_t ADCch[] = {0, 4, 1, 5, 2, 6, 3, 7};

  // uint16_t SPIsend = (ADCch[chan] << 12) | 0b1000000000000000; // single-ended, input +/-5V
  uint16_t SPIsend = (ADCch[chan] << 12) | 0b1000100000000000; // single-ended, input 0V to 5V
  // uint16_t SPIsend = (ADCch[chan] << 12) | 0b1000010000000000; // single-ended, input +/-10V
  // uint16_t SPIsend = (ADCch[chan] << 12) | 0b1000110000000000; // single-ended, input 0V to 10V

  /* Check if ADC is busy with conversion and continue if not*/
  while(!(__R31 & (1 << _BUSY)));

  /* pull down CONVST and _RD */
  __R30 &= ~(1 << CONVST);
  __R30 &= ~(1 << _RD);

  /* Enable channel */
  CT_MCSPI0.CH0CTRL_bit.EN = 0x1;

  /* Check if McSPI TX register is empty, if it is continue */
  while(!(CT_MCSPI0.IRQSTATUS_bit.TX0_EMPTY == 0x1));

  /* Write word to be transmitted into TX register */
  CT_MCSPI0.TX0 = SPIsend;

  /* Check if McSPI RX register is full, if it is continue */
  while(!(CT_MCSPI0.IRQSTATUS_bit.RX0_FULL == 0x1));

  /* Clear interrupts */
  CT_MCSPI0.IRQSTATUS = 0xFFFF;

  /* Disable channel */
  CT_MCSPI0.CH0CTRL_bit.EN = 0x0;

  /* Start conversion, Stop reading */
  __R30 |= (1 << CONVST);
  __R30 |= (1 << _RD);

  /* Delay until conversion starts */
  __delay_cycles(100);

  /* Wait until Conversion is done */
  while(!(__R31 & (1 << _BUSY)));

  /* pull down CONVST and _RD */
  __R30 &= ~(1 << CONVST);
  __R30 &= ~(1 << _RD);

  /* Enable channel */
  CT_MCSPI0.CH0CTRL_bit.EN = 0x1;

  /* Write nothing to TX Register */
  CT_MCSPI0.TX0 = 0x0000;

  /* Wait until RX register is full */
  while(!(CT_MCSPI0.IRQSTATUS_bit.RX0_FULL == 0x1));

  /* Clear interrupts */
  CT_MCSPI0.IRQSTATUS = 0xFFFF;

  /* Disable channel */
  CT_MCSPI0.CH0CTRL_bit.EN = 0x0;

  /* Start conversion */
  __R30 |= (1 << CONVST);
  __R30 |= (1 << _RD);

  /* Return value in RX register */
  return CT_MCSPI0.RX0;
}

/* Initialize the UART module */
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

/* Print message over UART */
void printUART(volatile char* Message){
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
