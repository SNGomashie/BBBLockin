#ifndef __LTC_1859_H_
#define __LTC_1859_H_

#include <sys_mcspi.h>

/* Define pin locations */
#define CS      7
#define _RD     1
#define _BUSY   0
#define CONVST  5
#define DEBUG   3

#ifndef __R30
  volatile register unsigned int __R30;
  volatile register unsigned int __R31;
#endif



void LTC1859initialize(void);

uint16_t LTC1859transfer(uint16_t spi_word);

void LTC1859conversion(uint8_t pin);

uint16_t LTC1859readout(uint8_t chan, uint8_t mode);

#endif
