#include <stdio.h>
#include <stdint.h>
#include "pru_peripheral.h"
#include "resource_table.h"

void main(void){
  char data[16] = "";
  int32_t c = 0;
  int16_t a = -32768;
  uint16_t b = 65535;
  c = a * b;
  UARTinitialize();
  sprintf(data, "%d\n", c);
  UARTtransmit(data);

  __halt();
}
