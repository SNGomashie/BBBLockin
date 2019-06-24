/*     Made by Sena Gomashie 20-05-2019    */
/*            For SRON Groningen           */
/* Abstraction layer between eCAP and user */
/*                 Example:                */
/*      Prints period of the SYNC input    */

#include "stdint.h"
#include "stdio.h"
#include "GPIO.h"
#include "UART.h"
#include "eCAP.h"
#include "resource_table_empty.h"

void main(void){
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;  // Allow OCP master port access by the PRU so the PRU can read external memories.
  char data[] = "";
  // Configure eCAP
  eCAP_polarity(0, 0);
  eCAP_polarity(1, 0);
  eCAP_polarity(2, 0);
  eCAP_polarity(3, 0);
  eCAP_reset(0, 1);
  eCAP_reset(1, 1);
  eCAP_reset(2, 1);
  eCAP_reset(3, 1);
  eCAP_load(1);
  eCAP_prescaler(0);
  eCAP_mode(0);
  eCAP_continuous(0);
  eCAP_sync(2, 0);
  eCAP_wrap(0);

  // Configure UART
  UART_set_baudrate(115200);
  UART_set_protocol(8, 0, 0, 0, 0, 0);
  UART_set_FIFO(1, 1, 1, 1, 2);
  UART_set_PEM(1, 1, 1);
  UART_set_modem(0, 0, 0);

  // Start eCAP
  eCAP_start();

  while(1){
    // ptrECAP is a pointer to reg0 of eCAP
    sprintf(data, "%d\n", *ptrECAP);
    // Send over UART for easy debugging
    UART_transmit(data);
    __delay_cycles(20000000);
  }

  __halt();
}
