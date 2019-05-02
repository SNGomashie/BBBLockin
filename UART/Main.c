#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "pru_peripheral.h"
#include "resource_table.h"

void main(void){
  char data[16] = "";
  uint32_t c = 0;
  uint64_t b = 2146483648‬;
  c = sqrt(b);
  UARTinitialize();
  sprintf(data, "%d\n", c);
  UARTtransmit(data);

  __halt();
}
