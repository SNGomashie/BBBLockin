#include <stdio.h>
#include <stdint.h>
#include "pru_peripheral.h"
#include "resource_table.h"

void main(void){
  char data[16] = "";

  UARTinitialize();
  sprintf(data, "%d\n", 0x0000);
  UARTtransmit("hallo");

  __halt();
}
