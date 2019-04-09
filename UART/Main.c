#include <stdio.h>
#include <stdint.h>
#include <pru_uart.h>
#include "resource_table.h"

/* The FIFO size on the PRU UART is 16 bytes; however, we are (arbitrarily)
 * only going to send 8 at a time */
#define FIFO_SIZE	16
#define MAX_CHARS	16
#define BUFFER		40

void initUART(void);

void printUART(volatile char* Message);

char readUART(void);

/* Input and output registers */
volatile register uint32_t __R30;
volatile register uint32_t __R31;

void main(void){
  char data[16] = "";
  char data2[16] = "";

  initUART();

  while(1){
  __R30 |= (1 << 7);
    sprintf(data, "%d %d %d %d\n", 0x0000, 0xFFFF, 0xF0F0, 0x0F0F);
    printUART(data);
  __R30 &= ~(1 << 7); 
    __delay_cycles(200000);
  }
  __halt();
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

/* Read message from UART */
char readUART(void){
  while (!CT_UART.LSR_bit.DR);

  return CT_UART.RBR_bit.DATA;
}
