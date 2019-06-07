/*         Made by Sena Gomashie 20-05-2019        */
/*                For SRON Groningen               */
/*    Abstraction layer between RPMsg and user     */
/*                     Example:                    */
/* Echo's back any input into the character device */

#include "stdint.h"
#include "stdio.h"
#include "GPIO.h"
#include "RPMsg.h"

void main(void){
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;  // Allow OCP master port access by the PRU so the PRU can read external memories.
  char* data;
  RPMSGinitialize();  // Initialize RPMsg

  while(1){
    data = RPMSGreceive(); // Receive data
    RPMSGtransmit(data);  // Echo back
  }

  __halt();
}
