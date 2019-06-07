/*       Made by Sena Gomashie 20-05-2019       */
/*               For SRON Groningen             */
/*   Abstraction layer between IEP and user    */
/*                 Example:                     */
/* Toggles the pin closest to the ethernet port */
/*        at a frequency of 1 Hz         */

#include "stdint.h"
#include "stdio.h"
#include "GPIO.h"
#include "IEP.h"
#include "resource_table_empty.h"

void main(void){
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;  // Allow OCP master port access by the PRU so the PRU can read external memories.

  IEP_reset();
  IEP_mode(0);
  IEP_frequency(1);
  IEP_increment(1);
  IEP_start();

  PRU_LED_SET(USR3); // Set USR3 (closed to the Ethernet port)

  __halt();
}
