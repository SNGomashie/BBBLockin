/*       Made by Sena Gomashie 16-05-2019       */
/*               For SRON Groningen             */
/*   Abstraction layer between GPIO and user    */
/*                 Example:                     */
/* Toggles the pin closest to the ethernet port */
/*        and sets P9.31 , PRU0 pin 7           */

#include "stdint.h"
#include "stdio.h"
#include "GPIO.h"
#include "resource_table_empty.h"

void main(void){
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;  // Allow OCP master port access by the PRU so the PRU can read external memories.

  PRU_LED_SET(USR3); // Set USR3 (closed to the Ethernet port)

  PRU_GPO_SET(7); // Set P9.31, PRU0 pin 7

  __halt();
}
