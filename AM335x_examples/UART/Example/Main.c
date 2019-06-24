/*            Made by Sena Gomashie 16-05-2019          */
/*                   For SRON Groningen                 */
/*        Abstraction layer between UART and user       */
/*                        Example:                      */
/* Prints an undefined amount of 'hallo' to Serial port */

#include "stdint.h"
#include "stdio.h"
#include "GPIO.h"
#include "UART.h"
#include "resource_table_empty.h"

void main(void){
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;  // Allow OCP master port access by the PRU so the PRU can read external memories.

  UART_set_baudrate(115200);
  UART_set_protocol(8, 0, 0, 0, 0, 0);
  UART_set_FIFO(1, 1, 1, 1, 2);
  UART_set_PEM(1, 1, 1);
  UART_set_modem(0, 0, 0);

  while(1){
    UART_transmit("hallo\n");
    __delay_cycles(20000000);
  }

  __halt();
}
