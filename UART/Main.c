#include <stdio.h>
#include <stdint.h>
#include "pru_peripheral.h"
#include "resource_table.h"

void main(void){
  char data[16] = "";
  uint32_t c = 0;
  int16_t a = -10;
  uint16_t b = 10;
  c = a * b;
  UARTinitialize();
  sprintf(data, "%d\n", c);
  UARTtransmit(data);

  __halt();
}
