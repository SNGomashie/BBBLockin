#ifndef __AM335X_PRU_UART_C_
#define __AM335X_PRU_UART_C_

#include "UART.h"

void UART_set_baudrate(uint32_t baud_rate){
  uint16_t divisor = 0;

  divisor = (192000000 / baud_rate) / 16;
  CT_UART.DLL = (divisor & 0x00FF);
  CT_UART.DLH = (divisor & 0xFF00);
  CT_UART.MDR_bit.OSM_SEL = 0x0;
}

void UART_set_protocol(uint8_t wordlength, uint8_t parity_en, uint8_t select_en, uint8_t stick_en, uint8_t break_en, uint8_t latch_en){
  CT_UART.LCR_bit.WLS = wordlength - 5;
  CT_UART.LCR_bit.PEN = parity_en;
  CT_UART.LCR_bit.EPS = select_en;
  CT_UART.LCR_bit.SP = stick_en;
  CT_UART.LCR_bit.BC = break_en;
  CT_UART.LCR_bit.DLAB = latch_en;
}

void UART_set_FIFO(uint8_t fifo_en, uint8_t clearRX, uint8_t clearTX, uint8_t DMA_mode, uint8_t trigger){
  CT_UART.FCR_bit.FIFOEN = fifo_en;
  CT_UART.FCR_bit.RXCLR = clearRX;
  CT_UART.FCR_bit.TXCLR = clearTX;
  CT_UART.FCR_bit.DMAMODE1 = DMA_mode;
  CT_UART.FCR_bit.RXFIFTL = trigger;
}

void UART_set_modem(uint8_t rts_en, uint8_t loop_en, uint8_t auto_en){
  CT_UART.MCR_bit.RTS = rts_en;
  CT_UART.MCR_bit.LOOP = loop_en;
  CT_UART.MCR_bit.AFE = auto_en;
}

void UART_set_PEM(uint8_t on, uint8_t rx, uint8_t tx){
  CT_UART.PWREMU_MGMT_bit.FREE = on;
CT_UART.PWREMU_MGMT_bit.URRST = rx;
CT_UART.PWREMU_MGMT_bit.UTRST = tx;
}

void UART_transmit(volatile char* Message){
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

char UART_receive(void){
  while (!CT_UART.LSR_bit.DR);

  return CT_UART.RBR_bit.DATA;
}

#endif
